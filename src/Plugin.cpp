// HandwritingCinema90s2000s : étape 3
// Moteur de base : timing stop-motion (24/12/8 fps), jitter Simplex (position,
// rotation, échelle), ink bleed (étalement + rugosité qui bout à chaque image),
// Write-On par ordre de tracé avec pression variable, couleur, opacité, fusion.
// Étape 3 : 20 presets JSON appliqués au changement de menu + 7 mécanismes de
// révélation (stroke, sweep, scanline, erosion, flash, pop, per_char).
// Reste à brancher : grain / rayures, textures de matière, effets propres à
// chaque preset (étape 4).

#include <algorithm>
#include <cmath>
#include <memory>
#include <mutex>
#include <string>

#include "EdgeFx.h"
#include "Noise.h"
#include "Preset.h"
#include "Presets.h"
#include "TextRaster.h"
#include "ofxsImageEffect.h"

using namespace OFX;

namespace {

constexpr char kPluginId[] = "com.handwritingcinema.Cinema90s2000s";

// Identifiants de paramètres
constexpr char kPreset[]     = "preset";
constexpr char kText[]       = "text";
constexpr char kFontSize[]   = "fontSize";
constexpr char kPosition[]   = "position";
constexpr char kWriteOn[]    = "writeOn";
constexpr char kInkBleed[]   = "inkBleed";
constexpr char kStopMotion[] = "stopMotion";
constexpr char kJitter[]     = "jitter";
constexpr char kGrain[]      = "grainScratch";
constexpr char kInkColor[]   = "inkColor";
constexpr char kOpacity[]    = "opacity";
constexpr char kBlend[]      = "blendMode";
constexpr char kSeed[]       = "seed";

enum Blend { kNormal = 0, kMultiply, kOverlay, kScreen };

float blendChannel(int mode, float s, float i) {  // s = fond, i = encre
  switch (mode) {
    case kMultiply: return s * i;
    case kScreen:   return 1.f - (1.f - s) * (1.f - i);
    case kOverlay:  return s < 0.5f ? 2.f * s * i : 1.f - 2.f * (1.f - s) * (1.f - i);
    default:        return i;
  }
}

inline float smooth(float e0, float e1, float x) {
  const float t = std::clamp((x - e0) / (e1 - e0), 0.f, 1.f);
  return t * t * (3.f - 2.f * t);
}

inline float hash01(long a, int b) {
  uint32_t h = static_cast<uint32_t>(a) * 0x9E3779B1u ^ (static_cast<uint32_t>(b) + 0x7F4A7C15u) * 0x85EBCA6Bu;
  h ^= h >> 15; h *= 0x2C1B3C6Du; h ^= h >> 12; h *= 0x297A2D39u; h ^= h >> 15;
  return (h & 0xFFFFFFu) / 16777216.f;
}

// Visibilité 0..1 de l'encre au point (mx, my) du masque, selon le mécanisme du preset.
// `press` : modulation de pression (utilisée par le tracé uniquement).
float revealAt(const hc::Preset& pr, const hc::Mask& m, float mx, float my, float p,
               long k, int seed, int px, float sd, float& press) {
  using R = hc::Reveal;
  const hc::Simplex& sx = hc::simplex();
  press = 1.f;
  if (p <= 0.f) return 0.f;
  const float u = mx / std::max(1, m.w), v = my / std::max(1, m.h);

  switch (pr.reveal) {
    case R::Stroke: {  // ordre d'écriture ligne par ligne, front incliné
      const float front = 0.03f;
      const float ord = m.orderAt(mx, my);
      const float pn = sx.noise(ord * m.total / std::max(8.f, px * 0.5f) + 5.f, sd + 63.f);
      press = 0.82f + 0.18f * (0.5f + 0.5f * pn);
      return std::clamp((p * (1.f + front) - ord) / front, 0.f, 1.f);
    }
    case R::Sweep: {   // bande qui s'étale de gauche à droite, bord biseauté
      const float uu = u + (v - 0.5f) * 0.06f;
      return std::clamp((p * 1.12f - uu) / 0.06f, 0.f, 1.f);
    }
    case R::Scanline: {  // balayage ligne par ligne de haut en bas
      const float band = std::max(2.f, px * 0.05f);
      const float vq = std::floor(my / band) * band / std::max(1, m.h);
      return vq < p ? 1.f : 0.f;
    }
    case R::Erosion: {  // dissolution par seuil de bruit (statique : le motif ne boue pas)
      const float f = 1.f / std::max(4.f, px * 0.12f);
      float n = 0.7f * sx.fbm(mx * f, my * f + sd) + 0.3f * sx.noise(mx * f * 4.f + 9.f, my * f * 4.f + sd);
      n = 0.5f + 0.5f * std::clamp(n, -1.f, 1.f);
      if (pr.radial) {
        const float d = std::sqrt((u - 0.5f) * (u - 0.5f) + (v - 0.5f) * (v - 0.5f)) / 0.7071f;
        n = 0.5f * n + 0.5f * std::clamp(d, 0.f, 1.f);
      }
      return std::clamp((p * 1.2f - 0.1f - n) / 0.1f, 0.f, 1.f);
    }
    case R::Flash:  // apparition stroboscopique : la probabilité d'être allumé monte avec p
      return hash01(k, seed) < p * 1.15f ? 1.f : 0.f;
    case R::Pop:    // apparition instantanée
      return 1.f;
    case R::PerChar: {  // une lettre par image, séquentielle ou aléatoire
      const int idx = m.charAt(mx, my);
      if (pr.charRandom) return hash01(idx * 7919 + 13, seed) < p ? 1.f : 0.f;
      return static_cast<float>(idx) < p * m.totalChars ? 1.f : 0.f;
    }
  }
  return 1.f;
}

class HandwritingPlugin : public ImageEffect {
 public:
  explicit HandwritingPlugin(OfxImageEffectHandle h) : ImageEffect(h) {
    dst_ = fetchClip(kOfxImageEffectOutputClipName);
    if (getContext() != eContextGenerator)
      src_ = fetchClip(kOfxImageEffectSimpleSourceClipName);

    preset_     = fetchChoiceParam(kPreset);
    text_       = fetchStringParam(kText);
    fontSize_   = fetchDoubleParam(kFontSize);
    position_   = fetchDouble2DParam(kPosition);
    writeOn_    = fetchDoubleParam(kWriteOn);
    inkBleed_   = fetchDoubleParam(kInkBleed);
    stopMotion_ = fetchChoiceParam(kStopMotion);
    jitter_     = fetchDoubleParam(kJitter);
    grain_      = fetchBooleanParam(kGrain);
    inkColor_   = fetchRGBParam(kInkColor);
    opacity_    = fetchDoubleParam(kOpacity);
    blend_      = fetchChoiceParam(kBlend);
    seed_       = fetchIntParam(kSeed);
  }

  void render(const RenderArguments& args) override;
  void changedParam(const InstanceChangedArgs& args, const std::string& paramName) override;

 private:
  // Masque rasterisé + étalement d'encre, mis en cache (texte, taille, rayon).
  std::shared_ptr<hc::Mask> maskFor(const std::string& text, int px, int spread) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!cache_ || text != cachedText_ || px != cachedPx_ || spread != cachedSpread_) {
      std::shared_ptr<hc::Mask> base = hc::rasterizeText(text, px);
      cache_ = base ? hc::spreadInk(*base, spread) : nullptr;
      cachedText_ = text;
      cachedPx_ = px;
      cachedSpread_ = spread;
    }
    return cache_;
  }

  Clip *dst_ = nullptr, *src_ = nullptr;
  ChoiceParam *preset_, *stopMotion_, *blend_;
  StringParam* text_;
  DoubleParam *fontSize_, *writeOn_, *inkBleed_, *jitter_, *opacity_;
  Double2DParam* position_;
  BooleanParam* grain_;
  RGBParam* inkColor_;
  IntParam* seed_;

  std::mutex mutex_;
  std::shared_ptr<hc::Mask> cache_;
  std::string cachedText_;
  int cachedPx_ = -1, cachedSpread_ = -1;
};

void HandwritingPlugin::changedParam(const InstanceChangedArgs& args, const std::string& name) {
  // Choisir un preset dans le menu pousse ses valeurs dans les curseurs ;
  // l'utilisateur peut ensuite les retoucher librement.
  if (name != kPreset || args.reason != eChangeUserEdit) return;
  const auto& all = hc::presets();
  int presetIdx = 0;
  preset_->getValue(presetIdx);
  const hc::Preset& p = all[std::clamp(presetIdx, 0, static_cast<int>(all.size()) - 1)];
  inkBleed_->setValue(p.inkBleed);
  jitter_->setValue(p.jitter);
  opacity_->setValue(p.opacity);
  stopMotion_->setValue(p.stopMotion);
  blend_->setValue(p.blend);
  grain_->setValue(p.grain);
  inkColor_->setValue(p.color[0], p.color[1], p.color[2]);
}

void HandwritingPlugin::render(const RenderArguments& args) {
  const double t = args.time;

  std::unique_ptr<Image> dst(dst_->fetchImage(t));
  if (!dst) throwSuiteStatusException(kOfxStatFailed);
  if (dst->getPixelDepth() != eBitDepthFloat ||
      dst->getPixelComponents() != ePixelComponentRGBA)
    throwSuiteStatusException(kOfxStatErrFormat);

  std::unique_ptr<Image> src;
  if (src_ && src_->isConnected()) src.reset(src_->fetchImage(t));

  // --- Temps stop-motion -------------------------------------------------
  // Les images sources gardent le temps réel ; tout ce qui est animé par le
  // plugin (Write-On, jitter, rugosité) est figé sur la dernière "image dessinée".
  static const double kFps[3] = {24.0, 12.0, 8.0};
  int stopIdx = 0;
  stopMotion_->getValueAtTime(t, stopIdx);
  const double animFps = kFps[std::clamp(stopIdx, 0, 2)];
  const double projFps = std::max(1.0, getFrameRate());
  const double step = std::max(1.0, projFps / animFps);
  const long k = static_cast<long>(std::floor(t / step + 1e-6));  // index d'image dessinée
  const double tq = k * step;

  // --- Paramètres (évalués au temps quantifié) ---------------------------
  std::string text;
  text_->getValueAtTime(tq, text);
  const double size = fontSize_->getValueAtTime(tq);
  OfxPointD pos = {0.5, 0.5};
  position_->getValueAtTime(tq, pos.x, pos.y);
  const float progress = static_cast<float>(std::clamp(writeOn_->getValueAtTime(tq) / 100.0, 0.0, 1.0));
  const float bleed = static_cast<float>(std::clamp(inkBleed_->getValueAtTime(tq) / 100.0, 0.0, 1.0));
  const double jit = std::clamp(jitter_->getValueAtTime(tq) / 100.0, 0.0, 1.0);
  const int seed = seed_->getValueAtTime(tq);
  const auto& allPresets = hc::presets();
  int presetIdxT = 0;
  preset_->getValueAtTime(tq, presetIdxT);
  const hc::Preset& preset =
      allPresets[std::clamp(presetIdxT, 0, static_cast<int>(allPresets.size()) - 1)];
  double r, g, b;
  inkColor_->getValueAtTime(tq, r, g, b);
  const float opacity = static_cast<float>(std::clamp(opacity_->getValueAtTime(tq) / 100.0, 0.0, 1.0));
  int mode = 0;
  blend_->getValueAtTime(tq, mode);
  if (!src) mode = kNormal;  // en générateur, pas de fond à fusionner

  // --- Rasterisation + étalement d'encre ---------------------------------
  const int px = std::max(4, static_cast<int>(std::lround(size * args.renderScale.x)));
  const int spread = static_cast<int>(std::lround(bleed * px * 0.04));
  const std::shared_ptr<hc::Mask> mask = maskFor(text, px, spread);

  const OfxRectI rod = dst->getRegionOfDefinition();
  const double cx = rod.x1 + pos.x * (rod.x2 - rod.x1);
  const double cy = rod.y1 + pos.y * (rod.y2 - rod.y1);  // OFX : y vers le haut
  const int mw = mask ? mask->w : 0;
  const int mh = mask ? mask->h : 0;

  // --- Jitter organique (position, rotation, échelle) --------------------
  const hc::Simplex& sx = hc::simplex();
  const float sd = static_cast<float>(seed) * 3.17f;
  const float kf = static_cast<float>(k) * 0.83f;  // chaque image tire un bruit différent
  const double dx  = sx.noise(kf + 3.1f,   sd)          * jit * 0.04 * px;
  const double dy  = sx.noise(kf + 41.7f,  sd + 9.f)    * jit * 0.04 * px;
  const double rot = sx.noise(kf + 97.3f,  sd + 19.f)   * jit * 0.05;
  const double scl = 1.0 + sx.noise(kf + 151.9f, sd + 29.f) * jit * 0.05;
  const double cs = std::cos(rot), sn = std::sin(rot);

  // Rugosité des bords : le bruit est collé au texte et "bout" à chaque image.
  const float freq = 1.f / std::max(3.f, px * 0.06f);
  const float offX = static_cast<float>(k) * 37.1f + sd;
  const float offY = static_cast<float>(k) * 11.7f + sd * 1.9f;

  const OfxRectI win = args.renderWindow;
  for (int y = win.y1; y < win.y2; ++y) {
    if (abort()) return;
    for (int x = win.x1; x < win.x2; ++x) {
      float* d = static_cast<float*>(dst->getPixelAddress(x, y));
      if (!d) continue;
      const float* s = src ? static_cast<const float*>(src->getPixelAddress(x, y)) : nullptr;
      const float sr = s ? s[0] : 0.f, sg = s ? s[1] : 0.f, sb = s ? s[2] : 0.f;
      const float sa = s ? s[3] : 0.f;

      float cov = 0.f;
      if (mask) {
        // pixel -> coordonnées du masque (transformation inverse du jitter)
        const double ux = x + 0.5 - (cx + dx), uy = y + 0.5 - (cy + dy);
        const double vx = (cs * ux + sn * uy) / scl;
        const double vy = (-sn * ux + cs * uy) / scl;
        const float mx = static_cast<float>(vx + mw / 2.0);
        const float my = static_cast<float>(mh / 2.0 - vy);

        if (mx > -2.f && my > -2.f && mx < mw + 2.f && my < mh + 2.f) {
          const float bl = mask->sample(mx, my);
          if (bl > 0.003f) {
            float c = bl;
            if (bleed > 0.f) {
              const float n = sx.fbm(mx * freq + offX, my * freq + offY);
              const float thr = 0.5f - 0.22f * bleed + n * 0.30f * bleed;
              c = smooth(thr - 0.12f, thr + 0.12f, bl);
            }
            float press = 1.f;
            const float vis = revealAt(preset, *mask, mx, my, progress, k, seed, px, sd, press);
            cov = c * vis * press;
          }
        }
      }
      const float a = cov * opacity;

      if (a <= 0.f) {
        d[0] = sr; d[1] = sg; d[2] = sb; d[3] = sa;
        continue;
      }

      // Fusion (sources prémultipliées) : formule de compositing W3C
      const float ur = sa > 0.f ? sr / sa : 0.f;
      const float ug = sa > 0.f ? sg / sa : 0.f;
      const float ub = sa > 0.f ? sb / sa : 0.f;
      const float br = blendChannel(mode, ur, static_cast<float>(r));
      const float bg = blendChannel(mode, ug, static_cast<float>(g));
      const float bb = blendChannel(mode, ub, static_cast<float>(b));
      d[0] = sr * (1.f - a) + a * (sa * br + (1.f - sa) * static_cast<float>(r));
      d[1] = sg * (1.f - a) + a * (sa * bg + (1.f - sa) * static_cast<float>(g));
      d[2] = sb * (1.f - a) + a * (sa * bb + (1.f - sa) * static_cast<float>(b));
      d[3] = a + sa * (1.f - a);
    }
  }
}

// ---------------------------------------------------------------------------
class Factory : public PluginFactoryHelper<Factory> {
 public:
  Factory() : PluginFactoryHelper<Factory>(kPluginId, 1, 0) {}
  void load() override {}
  void unload() override {}
  void describe(ImageEffectDescriptor& desc) override;
  void describeInContext(ImageEffectDescriptor& desc, ContextEnum ctx) override;
  ImageEffect* createInstance(OfxImageEffectHandle h, ContextEnum) override {
    return new HandwritingPlugin(h);
  }
};

void Factory::describe(ImageEffectDescriptor& desc) {
  desc.setLabels("Handwriting Cinema 90s-2000s", "Handwriting Cinema",
                 "Handwriting Cinema 90s-2000s");
  desc.setPluginGrouping("Cinema 90s");
  desc.setPluginDescription(
      "Titres et écritures manuscrites style cinéma indé 1990-2010.");
  desc.addSupportedContext(eContextFilter);
  desc.addSupportedContext(eContextGenerator);
  desc.addSupportedBitDepth(eBitDepthFloat);
  desc.setSingleInstance(false);
  desc.setHostFrameThreading(false);
  desc.setSupportsMultiResolution(true);
  desc.setSupportsTiles(false);
  desc.setTemporalClipAccess(false);
  desc.setRenderTwiceAlways(false);
  desc.setSupportsMultipleClipPARs(false);
}

void Factory::describeInContext(ImageEffectDescriptor& desc, ContextEnum ctx) {
  ClipDescriptor* dst = desc.defineClip(kOfxImageEffectOutputClipName);
  dst->addSupportedComponent(ePixelComponentRGBA);
  dst->setSupportsTiles(false);

  if (ctx != eContextGenerator) {
    ClipDescriptor* src = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    src->addSupportedComponent(ePixelComponentRGBA);
    src->setSupportsTiles(false);
  }

  // Preset
  {
    ChoiceParamDescriptor* p = desc.defineChoiceParam(kPreset);
    p->setLabel("Preset");
    for (const char* name : hc::kPresetNames) p->appendOption(name);
    p->setDefault(0);
    p->setAnimates(false);
  }
  // Texte multi-lignes
  {
    StringParamDescriptor* p = desc.defineStringParam(kText);
    p->setLabel("Text");
    p->setStringType(eStringTypeMultiLine);
    p->setDefault("Dear Diary,\nit's me again.");
    p->setAnimates(false);
  }
  // Taille / position
  {
    DoubleParamDescriptor* p = desc.defineDoubleParam(kFontSize);
    p->setLabel("Font Size");
    p->setRange(4, 2000);
    p->setDisplayRange(10, 400);
    p->setDefault(120);
  }
  {
    Double2DParamDescriptor* p = desc.defineDouble2DParam(kPosition);
    p->setLabel("Position");
    p->getPropertySet().propSetString(kOfxParamPropDoubleType, "OfxParamDoubleTypeNormalisedXY", 0, false);
    p->setDefaultCoordinateSystem(eCoordinatesNormalised);
    p->setDefault(0.5, 0.5);
  }
  // Write-On 0..100 %
  {
    DoubleParamDescriptor* p = desc.defineDoubleParam(kWriteOn);
    p->setLabel("Write-On Progress (%)");
    p->setRange(0, 100);
    p->setDisplayRange(0, 100);
    p->setDefault(100);
  }
  // Style
  {
    DoubleParamDescriptor* p = desc.defineDoubleParam(kInkBleed);
    p->setLabel("Ink Bleed / Roughen");
    p->setRange(0, 100);
    p->setDisplayRange(0, 100);
    p->setDefault(30);
  }
  {
    ChoiceParamDescriptor* p = desc.defineChoiceParam(kStopMotion);
    p->setLabel("Stop-Motion Speed");
    p->appendOption("24 fps (Smooth)");
    p->appendOption("12 fps (Hand-drawn)");
    p->appendOption("8 fps (Saccadé)");
    p->setDefault(1);
  }
  {
    DoubleParamDescriptor* p = desc.defineDoubleParam(kJitter);
    p->setLabel("Jitter Amount");
    p->setRange(0, 100);
    p->setDisplayRange(0, 100);
    p->setDefault(25);
  }
  {
    BooleanParamDescriptor* p = desc.defineBooleanParam(kGrain);
    p->setLabel("Grain & Scratch Layer");
    p->setDefault(true);
  }
  // Couleur & composition
  {
    RGBParamDescriptor* p = desc.defineRGBParam(kInkColor);
    p->setLabel("Ink Color");
    p->setDefault(0.04, 0.04, 0.05);
  }
  {
    DoubleParamDescriptor* p = desc.defineDoubleParam(kOpacity);
    p->setLabel("Opacity (%)");
    p->setRange(0, 100);
    p->setDisplayRange(0, 100);
    p->setDefault(100);
  }
  {
    ChoiceParamDescriptor* p = desc.defineChoiceParam(kBlend);
    p->setLabel("Blend Mode");
    p->appendOption("Normal");
    p->appendOption("Multiply");
    p->appendOption("Overlay");
    p->appendOption("Screen");
    p->setDefault(0);
  }
  {
    IntParamDescriptor* p = desc.defineIntParam(kSeed);
    p->setLabel("Seed");
    p->setRange(0, 100000);
    p->setDefault(1);
  }
}

}  // namespace

namespace hc { void registerTitlesPlugin(OFX::PluginFactoryArray& ids); }

namespace OFX {
namespace Plugin {
void getPluginIDs(PluginFactoryArray& ids) {
  static Factory p;
  ids.push_back(&p);
  hc::registerTitlesPlugin(ids);   // second effet du bundle : Cinema Titles
}
}  // namespace Plugin
}  // namespace OFX

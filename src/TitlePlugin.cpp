// Cinema Titles 90s-2000s : titres animés lettre par lettre (entrée / idle / sortie),
// 170+ presets, tout est réglable dans l'inspecteur. Le moteur est dans TitleEngine.cpp.
#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "TitleEngine.h"
#include "ofxsImageEffect.h"

using namespace OFX;

namespace {

constexpr char kTitlesId[] = "com.handwritingcinema.CinemaTitles";

// ---- Table des paramètres ------------------------------------------------------
// type : 'd' double, 'i' int, 'c' choix, 'b' booléen, 'r' couleur, 's' texte, 'm' texte multi-lignes
struct PD {
  const char* id;
  const char* label;
  char type;
  double lo, hi, d0, d1, d2;   // bornes (d) / défaut (d0, ou RGB d0 d1 d2)
  const char* group;           // nullptr = racine
  const char* table;           // menu : nom de table du moteur ("in", "curve"...) ou spéciale
};

const PD kParams[] = {
    // --- Texte
    {"preset", "Preset", 'c', 0, 0, 0, 0, 0, nullptr, "@preset"},
    {"text", "Text", 'm', 0, 0, 0, 0, 0, nullptr, nullptr},
    {"fontSize", "Font Size", 'd', 4, 3000, 120, 0, 0, nullptr, nullptr},
    {"upper", "Uppercase", 'b', 0, 0, 0, 0, 0, "Text", nullptr},
    {"font", "Font Style", 'c', 0, 0, 6, 0, 0, "Text", "font"},
    {"fontName", "Font Name / Path (override)", 's', 0, 0, 0, 0, 0, "Text", nullptr},
    {"tracking", "Letter Spacing (em)", 'd', -0.3, 2, 0, 0, 0, "Text", nullptr},
    {"lineSpacing", "Line Spacing", 'd', 0.6, 3, 1.15, 0, 0, "Text", nullptr},
    // --- Timing
    {"timeMode", "Timeline Mode", 'c', 0, 0, 0, 0, 0, "Timing", "@timemode"},
    {"inStart", "Entry Start (frames)", 'd', 0, 100000, 0, 0, 0, "Timing", nullptr},
    {"outStart", "Exit Start (frames, 0 = auto from end)", 'd', 0, 100000, 0, 0, 0, "Timing", nullptr},
    {"inProgress", "Manual Entry Progress (%)", 'd', 0, 100, 100, 0, 0, "Timing", nullptr},
    {"outProgress", "Manual Exit Progress (%)", 'd', 0, 100, 0, 0, 0, "Timing", nullptr},
    {"stopMotion", "Animation Rate", 'c', 0, 0, 0, 0, 0, "Timing", "@fps"},
    {"motionBlur", "Motion Blur", 'd', 0, 2, 1, 0, 0, "Timing", nullptr},
    {"seed", "Seed", 'i', 0, 100000, 1, 0, 0, "Timing", nullptr},
    // --- Entrée
    {"inKind", "Entry Type", 'c', 0, 0, 0, 0, 0, "Entry", "in"},
    {"inFrames", "Entry Duration / Letter (frames)", 'd', 1, 600, 10, 0, 0, "Entry", nullptr},
    {"inA", "Entry Amount A", 'd', -720, 720, 0, 0, 0, "Entry", nullptr},
    {"inB", "Entry Amount B", 'd', -720, 720, 0, 0, 0, "Entry", nullptr},
    {"inDir", "Entry Direction", 'c', 0, 0, 0, 0, 0, "Entry", "dir"},
    {"inCurve", "Entry Curve (Feel)", 'c', 0, 0, 1, 0, 0, "Entry", "curve"},
    // --- Idle
    {"idleKind", "Idle Type", 'c', 0, 0, 0, 0, 0, "Idle", "idle"},
    {"idleFrames", "Idle Period (frames)", 'd', 1, 3000, 40, 0, 0, "Idle", nullptr},
    {"idleA", "Idle Amount A", 'd', -10, 10, 0, 0, 0, "Idle", nullptr},
    {"idleB", "Idle Amount B", 'd', -360, 360, 0, 0, 0, "Idle", nullptr},
    {"idleC", "Idle Shared By All Letters (0/1)", 'd', 0, 1, 0, 0, 0, "Idle", nullptr},
    {"idleDir", "Idle Direction", 'c', 0, 0, 0, 0, 0, "Idle", "dir"},
    // --- Sortie
    {"outKind", "Exit Type", 'c', 0, 0, 0, 0, 0, "Exit", "out"},
    {"outFrames", "Exit Duration / Letter (frames)", 'd', 1, 600, 8, 0, 0, "Exit", nullptr},
    {"outA", "Exit Amount A", 'd', -720, 720, 0, 0, 0, "Exit", nullptr},
    {"outB", "Exit Amount B", 'd', -720, 720, 0, 0, 0, "Exit", nullptr},
    {"outDir", "Exit Direction", 'c', 0, 0, 0, 0, 0, "Exit", "dir"},
    {"outCurve", "Exit Curve (Feel)", 'c', 0, 0, 2, 0, 0, "Exit", "curve"},
    // --- Ordre / ancrage
    {"order", "Reading Order", 'c', 0, 0, 0, 0, 0, "Order & Anchor", "order"},
    {"delay", "Entry Delay / Letter (frames)", 'd', 0, 60, 2, 0, 0, "Order & Anchor", nullptr},
    {"outDelay", "Exit Delay / Letter (frames)", 'd', 0, 60, 1, 0, 0, "Order & Anchor", nullptr},
    {"anchor", "Animation Origin", 'c', 0, 0, 0, 0, 0, "Order & Anchor", "anchor"},
    // --- Look
    {"fill", "Fill Color", 'r', 0, 0, 1, 1, 1, "Look", nullptr},
    {"gradAmt", "Gradient Amount", 'd', 0, 1, 0, 0, 0, "Look", nullptr},
    {"gradTop", "Gradient Top", 'r', 0, 0, 1, 1, 1, "Look", nullptr},
    {"gradBottom", "Gradient Bottom", 'r', 0, 0, 1, 1, 1, "Look", nullptr},
    {"gradH", "Gradient Left / Right", 'b', 0, 0, 0, 0, 0, "Look", nullptr},
    {"gradLeft", "Gradient Left", 'r', 0, 0, 1, 1, 1, "Look", nullptr},
    {"gradRight", "Gradient Right", 'r', 0, 0, 1, 1, 1, "Look", nullptr},
    {"strokeColor", "Stroke Color", 'r', 0, 0, 0, 0, 0, "Look", nullptr},
    {"strokeW", "Stroke Width (em)", 'd', 0, 0.3, 0, 0, 0, "Look", nullptr},
    {"shadowColor", "Shadow Color", 'r', 0, 0, 0, 0, 0, "Look", nullptr},
    {"shadowAlpha", "Shadow Opacity", 'd', 0, 1, 0, 0, 0, "Look", nullptr},
    {"shadowDx", "Shadow Offset X (em)", 'd', -1, 1, 0, 0, 0, "Look", nullptr},
    {"shadowDy", "Shadow Offset Y (em)", 'd', -1, 1, 0, 0, 0, "Look", nullptr},
    {"shadowBlur", "Shadow Blur (em)", 'd', 0, 0.5, 0, 0, 0, "Look", nullptr},
    {"glowColor", "Glow Color", 'r', 0, 0, 1, 1, 1, "Look", nullptr},
    {"glowR", "Glow Radius (em)", 'd', 0, 1, 0, 0, 0, "Look", nullptr},
    {"glowAmt", "Glow Intensity", 'd', 0, 5, 0, 0, 0, "Look", nullptr},
    {"grain", "Grain Inside Letters", 'd', 0, 1, 0, 0, 0, "Look", nullptr},
    {"plateColor", "Plate Color", 'r', 0, 0, 0, 0, 0, "Look", nullptr},
    {"plateAlpha", "Plate Opacity", 'd', 0, 1, 0, 0, 0, "Look", nullptr},
    {"platePad", "Plate Padding (em)", 'd', 0, 2, 0.3, 0, 0, "Look", nullptr},
    {"scrambleSet", "Scramble Characters", 'c', 0, 0, 0, 0, 0, "Look", "@scramble"},
    {"blend", "Blend Mode", 'c', 0, 0, 0, 0, 0, "Look", "blend"},
    {"opacity", "Opacity (%)", 'd', 0, 100, 100, 0, 0, "Look", nullptr},
    // --- typographie fine / matière
    {"italic", "Synthetic Italic", 'd', 0, 0.6, 0, 0, 0, "Text", nullptr},
    {"embolden", "Synthetic Bold (em)", 'd', 0, 0.3, 0, 0, 0, "Text", nullptr},
    {"outReverse", "Exit From Last Letter (backspace)", 'b', 0, 0, 0, 0, 0, "Order & Anchor", nullptr},
    {"chromaBase", "RGB Split (em)", 'd', 0, 0.5, 0, 0, 0, "Look", nullptr},
    {"flashColor", "Flash Color", 'r', 0, 0, 1, 1, 1, "Look", nullptr},
    {"texKind", "Letter Texture", 'c', 0, 0, 0, 0, 0, "Look", "texture"},
    {"texAmt", "Texture Amount", 'd', 0, 1, 0, 0, 0, "Look", nullptr},
    {"texScale", "Texture Scale", 'd', 0.2, 6, 1, 0, 0, "Look", nullptr},
    {"bevel", "Bevel / Emboss", 'd', 0, 1.5, 0, 0, 0, "Look", nullptr},
    // --- atmosphère
    {"vignette", "Vignette", 'd', 0, 1, 0, 0, 0, "Atmosphere & FX", nullptr},
    {"vignetteSize", "Vignette Size", 'd', 0.1, 1.5, 0.6, 0, 0, "Atmosphere & FX", nullptr},
    {"gradeAmt", "Color Grade Amount", 'd', 0, 1, 0, 0, 0, "Atmosphere & FX", nullptr},
    {"gradeSat", "Saturation", 'd', 0, 2, 1, 0, 0, "Atmosphere & FX", nullptr},
    {"gradeContrast", "Contrast", 'd', 0.3, 2, 1, 0, 0, "Atmosphere & FX", nullptr},
    {"gradeShadow", "Grade Shadows", 'r', 0, 0, 0, 0, 0, "Atmosphere & FX", nullptr},
    {"gradeHigh", "Grade Highlights", 'r', 0, 0, 1, 1, 1, "Atmosphere & FX", nullptr},
    {"scanAmt", "Scanlines", 'd', 0, 1, 0, 0, 0, "Atmosphere & FX", nullptr},
    {"scanSpacing", "Scanline Spacing (px)", 'd', 1, 16, 3, 0, 0, "Atmosphere & FX", nullptr},
    {"grainAll", "Film Grain (whole frame)", 'd', 0, 1, 0, 0, 0, "Atmosphere & FX", nullptr},
    {"scratches", "Film Scratches & Dust", 'd', 0, 1, 0, 0, 0, "Atmosphere & FX", nullptr},
    {"particle", "Particles", 'c', 0, 0, 0, 0, 0, "Atmosphere & FX", "particle"},
    {"particleAmt", "Particle Amount", 'd', 0, 3, 0.5, 0, 0, "Atmosphere & FX", nullptr},
    {"particleSize", "Particle Size", 'd', 0.2, 5, 1, 0, 0, "Atmosphere & FX", nullptr},
    {"particleSpeed", "Particle Speed", 'd', 0.1, 5, 1, 0, 0, "Atmosphere & FX", nullptr},
    {"particleColor", "Particle Color", 'r', 0, 0, 1, 1, 1, "Atmosphere & FX", nullptr},
    {"fogAmt", "Fog / Mist", 'd', 0, 1, 0, 0, 0, "Atmosphere & FX", nullptr},
    {"fogColor", "Fog Color", 'r', 0, 0, 0.8, 0.8, 0.85, "Atmosphere & FX", nullptr},
    {"flareAmt", "Anamorphic Flare", 'd', 0, 1, 0, 0, 0, "Atmosphere & FX", nullptr},
    {"flareColor", "Flare Color", 'r', 0, 0, 1, 0.85, 0.6, "Atmosphere & FX", nullptr},
};
constexpr size_t kParamCount = sizeof(kParams) / sizeof(kParams[0]);

const char* const kGroups[] = {"Text", "Timing", "Entry", "Idle", "Exit", "Order & Anchor", "Look", "Atmosphere & FX"};

constexpr char kPosition[] = "position";

std::string groupId(const char* label) {
  std::string s = "grp_";
  for (const char* c = label; *c; ++c)
    if (std::isalnum(static_cast<unsigned char>(*c))) s += *c;
  return s;
}

inline float clamp01(double v) { return static_cast<float>(std::clamp(v, 0.0, 1.0)); }

float blendCh(int mode, float s, float i) {  // s = fond, i = titre
  switch (mode) {
    case tc::kMultiply: return s * i;
    case tc::kScreen: return 1.f - (1.f - s) * (1.f - i);
    case tc::kOverlay: return s < 0.5f ? 2.f * s * i : 1.f - 2.f * (1.f - s) * (1.f - i);
    default: return i;
  }
}


// Valeurs d'inspecteur correspondant à un preset (id -> {v0, v1, v2}).
using Vals = std::map<std::string, std::array<double, 3>>;

Vals valuesOf(const tc::Params& P) {
  Vals v;
  auto d = [&](const char* id, double x) { v[id] = {x, 0, 0}; };
  auto r = [&](const char* id, const tc::Rgb& c) { v[id] = {c.r, c.g, c.b}; };
  d("font", P.font); d("upper", P.upper ? 1 : 0); d("tracking", P.tracking); d("lineSpacing", P.lineSpacing);
  d("order", P.order); d("delay", P.delay); d("outDelay", P.outDelay); d("anchor", P.anchor);
  d("stopMotion", P.fps == 0 ? 0 : (P.fps >= 20 ? 1 : (P.fps >= 10 ? 2 : (P.fps >= 7 ? 3 : 4))));
  d("motionBlur", P.motionBlur);
  d("inKind", P.in.kind); d("inFrames", P.in.frames); d("inA", P.in.a); d("inB", P.in.b);
  d("inDir", P.in.dir); d("inCurve", P.in.curve);
  d("idleKind", P.idle.kind); d("idleFrames", P.idle.frames); d("idleA", P.idle.a); d("idleB", P.idle.b);
  d("idleC", P.idle.c); d("idleDir", P.idle.dir);
  d("outKind", P.out.kind); d("outFrames", P.out.frames); d("outA", P.out.a); d("outB", P.out.b);
  d("outDir", P.out.dir); d("outCurve", P.out.curve);
  r("fill", P.fill); d("gradAmt", P.gradAmt); r("gradTop", P.gradTop); r("gradBottom", P.gradBottom);
  d("gradH", P.gradH ? 1 : 0); r("gradLeft", P.gradLeft); r("gradRight", P.gradRight);
  r("strokeColor", P.stroke); d("strokeW", P.strokeW);
  r("shadowColor", P.shadow); d("shadowAlpha", P.shadowAlpha); d("shadowDx", P.shadowDx);
  d("shadowDy", P.shadowDy); d("shadowBlur", P.shadowBlur);
  r("glowColor", P.glow); d("glowR", P.glowR); d("glowAmt", P.glowAmt);
  d("grain", P.grain);
  r("plateColor", P.plate); d("plateAlpha", P.plateAlpha); d("platePad", P.platePad);
  d("scrambleSet", P.scrambleSet); d("blend", P.blend);
  d("italic", P.italic); d("embolden", P.embolden); d("outReverse", P.outReverse ? 1 : 0); d("chromaBase", P.chromaBase);
  r("flashColor", P.flashColor);
  d("texKind", P.texKind); d("texAmt", P.texAmt); d("texScale", P.texScale); d("bevel", P.bevel);
  d("vignette", P.vignette); d("vignetteSize", P.vignetteSize); d("gradeAmt", P.gradeAmt); d("gradeSat", P.gradeSat);
  d("gradeContrast", P.gradeContrast); r("gradeShadow", P.gradeShadow); r("gradeHigh", P.gradeHigh);
  d("scanAmt", P.scanAmt); d("scanSpacing", P.scanSpacing); d("grainAll", P.grainAll); d("scratches", P.scratches);
  d("particle", P.particle); d("particleAmt", P.particleAmt); d("particleSize", P.particleSize); d("particleSpeed", P.particleSpeed);
  r("particleColor", P.particleColor); d("fogAmt", P.fogAmt); r("fogColor", P.fogColor); d("flareAmt", P.flareAmt); r("flareColor", P.flareColor);
  return v;
}

class TitlesPlugin : public ImageEffect {
 public:
  explicit TitlesPlugin(OfxImageEffectHandle h) : ImageEffect(h) {
    dst_ = fetchClip(kOfxImageEffectOutputClipName);
    if (getContext() != eContextGenerator) src_ = fetchClip(kOfxImageEffectSimpleSourceClipName);
    position_ = fetchDouble2DParam(kPosition);
    for (size_t i = 0; i < kParamCount; ++i) {
      const PD& p = kParams[i];
      switch (p.type) {
        case 'd': db_[p.id] = fetchDoubleParam(p.id); break;
        case 'i': in_[p.id] = fetchIntParam(p.id); break;
        case 'c': ch_[p.id] = fetchChoiceParam(p.id); break;
        case 'b': bo_[p.id] = fetchBooleanParam(p.id); break;
        case 'r': rgb_[p.id] = fetchRGBParam(p.id); break;
        default: st_[p.id] = fetchStringParam(p.id); break;
      }
    }
  }

  void render(const RenderArguments& args) override;
  void changedParam(const InstanceChangedArgs& args, const std::string& name) override;

 private:
  double D(const char* id, double t) { return db_[id]->getValueAtTime(t); }
  int C(const char* id, double t) { int v = 0; ch_[id]->getValueAtTime(t, v); return v; }
  bool B(const char* id, double t) { bool v = false; bo_[id]->getValueAtTime(t, v); return v; }
  tc::Rgb RGBv(const char* id, double t) {
    double r = 0, g = 0, b = 0;
    rgb_[id]->getValueAtTime(t, r, g, b);
    return tc::Rgb{static_cast<float>(r), static_cast<float>(g), static_cast<float>(b)};
  }
  tc::Params read(double t);

  std::shared_ptr<tc::Layout> layoutFor(const std::string& text, int px, const std::string& font,
                                        float tracking, float lineSpacing, float strokeEm, int scr, float italic, float embolden) {
    std::lock_guard<std::mutex> lock(mutex_);
    char key[64];
    std::snprintf(key, sizeof(key), "|%d|%.4f|%.4f|%.4f|%d|%.3f|%.3f|", px, tracking, lineSpacing, strokeEm, scr, italic, embolden);
    const std::string k = text + key + font;
    if (!layout_ || k != layoutKey_) {
      layout_ = tc::buildLayout(text, px, font, tracking, lineSpacing, strokeEm, scr, italic, embolden);
      layoutKey_ = k;
    }
    return layout_;
  }

  Clip *dst_ = nullptr, *src_ = nullptr;
  Double2DParam* position_ = nullptr;
  std::map<std::string, DoubleParam*> db_;
  std::map<std::string, IntParam*> in_;
  std::map<std::string, ChoiceParam*> ch_;
  std::map<std::string, BooleanParam*> bo_;
  std::map<std::string, RGBParam*> rgb_;
  std::map<std::string, StringParam*> st_;

  std::mutex mutex_;
  std::shared_ptr<tc::Layout> layout_;
  std::string layoutKey_;
};

tc::Params TitlesPlugin::read(double t) {
  tc::Params P;
  P.font = C("font", t);
  st_["fontName"]->getValueAtTime(t, P.fontName);
  P.upper = B("upper", t);
  P.tracking = static_cast<float>(D("tracking", t));
  P.lineSpacing = static_cast<float>(D("lineSpacing", t));
  P.order = C("order", t);
  P.delay = static_cast<float>(D("delay", t));
  P.outDelay = static_cast<float>(D("outDelay", t));
  P.anchor = C("anchor", t);

  P.in.kind = C("inKind", t);
  P.in.frames = static_cast<float>(D("inFrames", t));
  P.in.a = static_cast<float>(D("inA", t));
  P.in.b = static_cast<float>(D("inB", t));
  P.in.dir = C("inDir", t);
  P.in.curve = C("inCurve", t);
  P.idle.kind = C("idleKind", t);
  P.idle.frames = static_cast<float>(D("idleFrames", t));
  P.idle.a = static_cast<float>(D("idleA", t));
  P.idle.b = static_cast<float>(D("idleB", t));
  P.idle.c = static_cast<float>(D("idleC", t));
  P.idle.dir = C("idleDir", t);
  P.out.kind = C("outKind", t);
  P.out.frames = static_cast<float>(D("outFrames", t));
  P.out.a = static_cast<float>(D("outA", t));
  P.out.b = static_cast<float>(D("outB", t));
  P.out.dir = C("outDir", t);
  P.out.curve = C("outCurve", t);

  P.fill = RGBv("fill", t);
  P.gradAmt = static_cast<float>(D("gradAmt", t));
  P.gradTop = RGBv("gradTop", t);
  P.gradBottom = RGBv("gradBottom", t);
  P.gradH = B("gradH", t);
  P.gradLeft = RGBv("gradLeft", t);
  P.gradRight = RGBv("gradRight", t);
  P.stroke = RGBv("strokeColor", t);
  P.strokeW = static_cast<float>(D("strokeW", t));
  P.shadow = RGBv("shadowColor", t);
  P.shadowAlpha = static_cast<float>(D("shadowAlpha", t));
  P.shadowDx = static_cast<float>(D("shadowDx", t));
  P.shadowDy = static_cast<float>(D("shadowDy", t));
  P.shadowBlur = static_cast<float>(D("shadowBlur", t));
  P.glow = RGBv("glowColor", t);
  P.glowR = static_cast<float>(D("glowR", t));
  P.glowAmt = static_cast<float>(D("glowAmt", t));
  P.grain = static_cast<float>(D("grain", t));
  P.plate = RGBv("plateColor", t);
  P.plateAlpha = static_cast<float>(D("plateAlpha", t));
  P.platePad = static_cast<float>(D("platePad", t));
  P.scrambleSet = C("scrambleSet", t);
  P.blend = C("blend", t);
  P.motionBlur = static_cast<float>(D("motionBlur", t));
  P.opacity = static_cast<float>(D("opacity", t) / 100.0);
  P.italic = static_cast<float>(D("italic", t));
  P.embolden = static_cast<float>(D("embolden", t));
  P.outReverse = B("outReverse", t);
  P.chromaBase = static_cast<float>(D("chromaBase", t));
  P.flashColor = RGBv("flashColor", t);
  P.texKind = C("texKind", t); P.texAmt = static_cast<float>(D("texAmt", t)); P.texScale = static_cast<float>(D("texScale", t));
  P.bevel = static_cast<float>(D("bevel", t));
  P.vignette = static_cast<float>(D("vignette", t)); P.vignetteSize = static_cast<float>(D("vignetteSize", t));
  P.gradeAmt = static_cast<float>(D("gradeAmt", t)); P.gradeSat = static_cast<float>(D("gradeSat", t));
  P.gradeContrast = static_cast<float>(D("gradeContrast", t));
  P.gradeShadow = RGBv("gradeShadow", t); P.gradeHigh = RGBv("gradeHigh", t);
  P.scanAmt = static_cast<float>(D("scanAmt", t)); P.scanSpacing = static_cast<float>(D("scanSpacing", t));
  P.grainAll = static_cast<float>(D("grainAll", t)); P.scratches = static_cast<float>(D("scratches", t));
  P.particle = C("particle", t); P.particleAmt = static_cast<float>(D("particleAmt", t));
  P.particleSize = static_cast<float>(D("particleSize", t)); P.particleSpeed = static_cast<float>(D("particleSpeed", t));
  P.particleColor = RGBv("particleColor", t);
  P.fogAmt = static_cast<float>(D("fogAmt", t)); P.fogColor = RGBv("fogColor", t);
  P.flareAmt = static_cast<float>(D("flareAmt", t)); P.flareColor = RGBv("flareColor", t);
  return P;
}

void TitlesPlugin::changedParam(const InstanceChangedArgs& args, const std::string& name) {
  // Choisir un preset pousse toutes ses valeurs dans l'inspecteur ; tout reste modifiable ensuite.
  if (name != "preset" || args.reason != eChangeUserEdit) return;
  const auto& all = tc::presets();
  int idx = 0;
  ch_["preset"]->getValue(idx);
  const tc::Params& P = all[std::clamp(idx, 0, static_cast<int>(all.size()) - 1)];

  const Vals v = valuesOf(P);
  for (size_t i = 0; i < kParamCount; ++i) {
    const PD& p = kParams[i];
    const auto it = v.find(p.id);
    if (it == v.end()) continue;
    const auto& x = it->second;
    switch (p.type) {
      case 'd': db_[p.id]->setValue(x[0]); break;
      case 'c': ch_[p.id]->setValue(static_cast<int>(std::lround(x[0]))); break;
      case 'b': bo_[p.id]->setValue(x[0] > 0.5); break;
      case 'r': rgb_[p.id]->setValue(x[0], x[1], x[2]); break;
      default: break;
    }
  }
  st_["fontName"]->setValue(P.fontName);
}

void TitlesPlugin::render(const RenderArguments& args) {
  const double t = args.time;
  std::unique_ptr<Image> dst(dst_->fetchImage(t));
  if (!dst) throwSuiteStatusException(kOfxStatFailed);
  if (dst->getPixelDepth() != eBitDepthFloat || dst->getPixelComponents() != ePixelComponentRGBA)
    throwSuiteStatusException(kOfxStatErrFormat);
  std::unique_ptr<Image> src;
  if (src_ && src_->isConnected()) src.reset(src_->fetchImage(t));

  // --- cadence d'animation (stop-motion) ---
  static const double kFps[5] = {0.0, 24.0, 12.0, 8.0, 6.0};
  const int fpsIdx = std::clamp(C("stopMotion", t), 0, 4);
  const double projFps = std::max(1.0, getFrameRate());
  const double step = fpsIdx == 0 ? 1.0 : std::max(1.0, projFps / kFps[fpsIdx]);
  const double tq = std::floor(t / step + 1e-6) * step;

  tc::Params P = read(tq);
  std::string text;
  st_["text"]->getValueAtTime(tq, text);
  if (P.upper)
    for (auto& c : text)
      if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');

  const double size = D("fontSize", tq);
  const int px = std::max(6, static_cast<int>(std::lround(size * args.renderScale.x)));
  const std::string fontPath = tc::resolveFont(P.font, P.fontName);
  const auto layout = layoutFor(text, px, fontPath, P.tracking, P.lineSpacing, P.strokeW, P.scrambleSet, P.italic, P.embolden);

  // --- temps de la timeline ---
  const int nGlyphs = layout ? static_cast<int>(layout->g.size()) : 0;
  const double inSpan = tc::entrySpan(P, std::max(1, nGlyphs));
  const double outSpan = tc::exitSpan(P, std::max(1, nGlyphs));
  tc::Times T;
  T.seed = in_["seed"]->getValueAtTime(tq);
  T.tidle = tq;
  T.shutter = 0.5 * std::max(1.0, step);
  if (C("timeMode", tq) == 1) {   // manuel : deux curseurs pilotent entrée et sortie
    const double pin = D("inProgress", tq) / 100.0, pout = D("outProgress", tq) / 100.0;
    T.tin = pin >= 1.0 ? inSpan + 1000.0 : pin * inSpan;
    if (pin <= 0.0) T.tin = -1.0;
    T.tout = pout <= 0.0 ? -1.0 : pout * outSpan;
  } else {                        // auto : entrée au début, sortie calée sur la fin du plan
    T.tin = tq - D("inStart", tq);
    const double dur = getEffectDuration();
    double outStart = D("outStart", tq);
    if (outStart <= 0.0 && dur > 1.0 && dur < 1.0e6) outStart = dur - outSpan;
    T.tout = (outStart > 0.0 && outSpan > 0.0) ? tq - outStart : -1.0;
  }

  // --- rendu du titre dans un canevas (y vers le bas) ---
  const OfxRectI win = args.renderWindow;
  const int cw = win.x2 - win.x1, ch = win.y2 - win.y1;
  tc::Canvas canvas;
  tc::FrameFx frameFx;
  canvas.init(std::max(1, cw), std::max(1, ch));
  if (layout && !layout->g.empty()) {
    OfxPointD pos = {0.5, 0.5};
    position_->getValueAtTime(tq, pos.x, pos.y);
    const OfxRectI rod = dst->getRegionOfDefinition();
    const double cxOfx = rod.x1 + pos.x * (rod.x2 - rod.x1);
    const double cyOfx = rod.y1 + pos.y * (rod.y2 - rod.y1);
    frameFx = tc::renderTitle(*layout, P, T, cxOfx - win.x1, win.y2 - cyOfx, canvas, args.renderScale.x);
  }

  // --- composition sur la source (+ atmosphère plein cadre) ---
  const int mode = src ? P.blend : tc::kNormal;
  const OfxRectI rodAll = dst->getRegionOfDefinition();
  const double fcx = 0.5 * (rodAll.x1 + rodAll.x2), fcy = 0.5 * (rodAll.y1 + rodAll.y2);
  const double halfW = std::max(1.0, 0.5 * (rodAll.x2 - rodAll.x1)), halfH = std::max(1.0, 0.5 * (rodAll.y2 - rodAll.y1));
  const bool grading = P.gradeAmt > 0.001f || P.gradeSat != 1.f || P.gradeContrast != 1.f;
  const int frameNo = static_cast<int>(std::floor(tq));
  auto smooth01 = [](float e0, float e1, float x) {
    const float u = std::clamp((x - e0) / (e1 - e0), 0.f, 1.f);
    return u * u * (3.f - 2.f * u);
  };
  auto rnd01 = [](uint32_t a, uint32_t b2, uint32_t c2) {
    uint32_t h = a * 0x9E3779B1u ^ (b2 + 0x7F4A7C15u) * 0x85EBCA6Bu ^ (c2 + 0x165667B1u) * 0xC2B2AE35u;
    h ^= h >> 15; h *= 0x2C1B3C6Du; h ^= h >> 12; h *= 0x297A2D39u; h ^= h >> 15;
    return (h & 0xFFFFFFu) / 16777216.f;
  };
  const float spacing = std::max(1.f, P.scanSpacing * static_cast<float>(args.renderScale.x));
  for (int y = win.y1; y < win.y2; ++y) {
    if (abort()) return;
    const int j = win.y2 - 1 - y;
    for (int x = win.x1; x < win.x2; ++x) {
      float* d = static_cast<float*>(dst->getPixelAddress(x, y));
      if (!d) continue;
      const float* s = src ? static_cast<const float*>(src->getPixelAddress(x, y)) : nullptr;
      float sr = s ? s[0] : 0.f, sg = s ? s[1] : 0.f, sb = s ? s[2] : 0.f;
      const float sa = s ? s[3] : 0.f;

      // fond : vignette + étalonnage (sur la source seulement)
      if (sa > 0.f && (P.vignette > 0.001f || grading)) {
        float r = sr / sa, g = sg / sa, b = sb / sa;
        if (grading) {
          const float Y = 0.2126f * r + 0.7152f * g + 0.0722f * b;
          r = Y + (r - Y) * P.gradeSat; g = Y + (g - Y) * P.gradeSat; b = Y + (b - Y) * P.gradeSat;
          r = (r - 0.5f) * P.gradeContrast + 0.5f; g = (g - 0.5f) * P.gradeContrast + 0.5f; b = (b - 0.5f) * P.gradeContrast + 0.5f;
          const float L = std::clamp(Y * P.gradeContrast + (0.5f - 0.5f * P.gradeContrast), 0.f, 1.f);
          const float tr = P.gradeShadow.r + (P.gradeHigh.r - P.gradeShadow.r) * L;
          const float tg = P.gradeShadow.g + (P.gradeHigh.g - P.gradeShadow.g) * L;
          const float tb = P.gradeShadow.b + (P.gradeHigh.b - P.gradeShadow.b) * L;
          r += (tr - r) * P.gradeAmt; g += (tg - g) * P.gradeAmt; b += (tb - b) * P.gradeAmt;
        }
        if (P.vignette > 0.001f) {
          const double dx = (x + 0.5 - fcx) / halfW, dy = (y + 0.5 - fcy) / halfH;
          const float dist = static_cast<float>(std::sqrt(dx * dx + dy * dy) / 1.4142);
          const float k = 1.f - P.vignette * smooth01(1.05f - P.vignetteSize, 1.95f - P.vignetteSize, dist);
          r *= k; g *= k; b *= k;
        }
        sr = std::max(0.f, r) * sa; sg = std::max(0.f, g) * sa; sb = std::max(0.f, b) * sa;
      }

      const float* c = &canvas.px[((size_t)j * canvas.w + (x - win.x1)) * 4];
      const float a = std::clamp(c[3] * P.opacity, 0.f, 1.f);
      float orr, og, ob, oa;
      if (a <= 0.f) {
        orr = sr; og = sg; ob = sb; oa = sa;
      } else {
        const float cr = c[0] / c[3], cg = c[1] / c[3], cb = c[2] / c[3];
        if (mode == tc::kAdd) {
          orr = sr + cr * a; og = sg + cg * a; ob = sb + cb * a;
        } else {
          const float ur = sa > 0.f ? sr / sa : 0.f, ug = sa > 0.f ? sg / sa : 0.f, ub = sa > 0.f ? sb / sa : 0.f;
          orr = sr * (1.f - a) + a * (sa * blendCh(mode, ur, cr) + (1.f - sa) * cr);
          og = sg * (1.f - a) + a * (sa * blendCh(mode, ug, cg) + (1.f - sa) * cg);
          ob = sb * (1.f - a) + a * (sa * blendCh(mode, ub, cb) + (1.f - sa) * cb);
        }
        oa = a + sa * (1.f - a);
      }

      // post : scanlines, grain plein cadre, flash / noir de cadre
      if (P.scanAmt > 0.001f) {
        const float ph = static_cast<float>(j) / spacing;
        const float k = 1.f - P.scanAmt * 0.5f * (1.f - std::cos(6.2831853f * ph));
        orr *= k; og *= k; ob *= k;
      }
      if (P.grainAll > 0.001f && oa > 0.f) {
        const float gn = (rnd01(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(frameNo * 7 + T.seed)) - 0.5f) * P.grainAll * 0.4f * oa;
        orr = std::max(0.f, orr + gn); og = std::max(0.f, og + gn); ob = std::max(0.f, ob + gn);
      }
      if (frameFx.black > 0.001f) {
        const float k = 1.f - frameFx.black;
        orr *= k; og *= k; ob *= k; oa = oa * k + frameFx.black;
      }
      if (frameFx.white > 0.001f) {
        const float w = frameFx.white;
        orr = orr * (1.f - w) + w; og = og * (1.f - w) + w; ob = ob * (1.f - w) + w; oa = oa * (1.f - w) + w;
      }
      d[0] = orr; d[1] = og; d[2] = ob; d[3] = oa;
    }
  }
}

// ---- Fabrique ---------------------------------------------------------------------
class Factory : public PluginFactoryHelper<Factory> {
 public:
  Factory() : PluginFactoryHelper<Factory>(kTitlesId, 1, 0) {}
  void load() override {}
  void unload() override {}
  void describe(ImageEffectDescriptor& desc) override;
  void describeInContext(ImageEffectDescriptor& desc, ContextEnum ctx) override;
  ImageEffect* createInstance(OfxImageEffectHandle h, ContextEnum) override { return new TitlesPlugin(h); }
};

void Factory::describe(ImageEffectDescriptor& desc) {
  desc.setLabels("Cinema Titles 90s-2000s", "Cinema Titles", "Cinema Titles 90s-2000s");
  desc.setPluginGrouping("Cinema 90s");
  desc.setPluginDescription("Titres animes lettre par lettre : 170+ presets cinema / TV / anime 1990-2010.");
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

void defineOne(ImageEffectDescriptor& desc, const PD& p, GroupParamDescriptor* group) {
    static const Vals kFirst = valuesOf(tc::presets()[0]);
    std::array<double, 3> dv = {p.d0, p.d1, p.d2};
    const auto fv = kFirst.find(p.id);
    if (fv != kFirst.end()) dv = fv->second;
    ParamDescriptor* base = nullptr;
    switch (p.type) {
      case 'd': {
        DoubleParamDescriptor* d = desc.defineDoubleParam(p.id);
        d->setRange(p.lo, p.hi);
        d->setDisplayRange(p.lo, p.hi);
        d->setDefault(dv[0]);
        base = d;
        break;
      }
      case 'i': {
        IntParamDescriptor* d = desc.defineIntParam(p.id);
        d->setRange(static_cast<int>(p.lo), static_cast<int>(p.hi));
        d->setDefault(static_cast<int>(p.d0));
        base = d;
        break;
      }
      case 'b': {
        BooleanParamDescriptor* d = desc.defineBooleanParam(p.id);
        d->setDefault(dv[0] > 0.5);
        base = d;
        break;
      }
      case 'r': {
        RGBParamDescriptor* d = desc.defineRGBParam(p.id);
        d->setDefault(dv[0], dv[1], dv[2]);
        base = d;
        break;
      }
      case 's': {
        StringParamDescriptor* d = desc.defineStringParam(p.id);
        d->setDefault(std::string(p.id) == "fontName" ? tc::presets()[0].fontName : std::string());
        base = d;
        break;
      }
      case 'm': {
        StringParamDescriptor* d = desc.defineStringParam(p.id);
        d->setStringType(eStringTypeMultiLine);
        d->setDefault("DEAR DIARY\nit's me again");
        base = d;
        break;
      }
      default: {  // 'c'
        ChoiceParamDescriptor* d = desc.defineChoiceParam(p.id);
        const std::string tbl = p.table ? p.table : "";
        if (tbl == "@preset") {
          for (const auto& pr : tc::presets()) d->appendOption(pr.name);
        } else if (tbl == "@timemode") {
          d->appendOption("Auto (entry at start, exit at end of clip)");
          d->appendOption("Manual (progress sliders)");
        } else if (tbl == "@fps") {
          d->appendOption("Project rate");
          d->appendOption("24 fps");
          d->appendOption("12 fps (hand-made)");
          d->appendOption("8 fps (stop-motion)");
          d->appendOption("6 fps");
        } else if (tbl == "@scramble") {
          d->appendOption("Letters & digits");
          d->appendOption("Binary (0 / 1)");
        } else {
          int n = 0;
          const char* const* names = tc::names(tbl.c_str(), n);
          for (int k = 0; k < n; ++k) d->appendOption(names[k]);
        }
        d->setDefault(static_cast<int>(std::lround(dv[0])));
        d->setAnimates(false);
        base = d;
        break;
      }
    }
    base->setLabel(p.label);
    if (group) base->setParent(*group);
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

  std::map<std::string, GroupParamDescriptor*> groups;

  // Deux passes : d'abord les paramètres racine (Preset, Text, Size), puis Position,
  // puis les groupes repliables avec leurs paramètres.
  for (int pass = 0; pass < 2; ++pass) {
    if (pass == 1) {
      Double2DParamDescriptor* pp = desc.defineDouble2DParam(kPosition);
      pp->setLabel("Position");
      pp->getPropertySet().propSetString(kOfxParamPropDoubleType, "OfxParamDoubleTypeNormalisedXY", 0, false);
      pp->setDefaultCoordinateSystem(eCoordinatesNormalised);
      pp->setDefault(0.5, 0.5);
      for (const char* g : kGroups) {
        GroupParamDescriptor* gp = desc.defineGroupParam(groupId(g).c_str());
        gp->setLabel(g);
        gp->setOpen(std::string(g) == "Text" || std::string(g) == "Timing");
        groups[g] = gp;
      }
    }
    for (size_t i = 0; i < kParamCount; ++i) {
      const PD& p = kParams[i];
      if ((p.group == nullptr) != (pass == 0)) continue;
      defineOne(desc, p, p.group ? groups[p.group] : nullptr);
    }
  }
}


}  // namespace

namespace hc {
void registerTitlesPlugin(OFX::PluginFactoryArray& ids) {
  static Factory f;
  ids.push_back(&f);
}
}  // namespace hc

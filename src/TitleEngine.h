#pragma once
// Moteur de titres animés lettre par lettre (sans dépendance OpenFX).
// - mise en page + rasterisation FreeType glyphe par glyphe
// - machine à états : entrée / idle / sortie, ordre, décalage par lettre, courbes
// - rendu CPU : transformation affine par lettre, motion blur par sous-échantillonnage,
//   flou, aberration chromatique, contour, ombre, lueur, dégradé, grain, dissolution...
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace tc {

// ---- Vocabulaire ---------------------------------------------------------
enum Curve { kLinear, kEaseOut, kEaseIn, kEaseInOut, kElastic, kBounce, kBackOut, kExpoOut, kSteps, kCurveCount };
enum Dir { kUp, kDown, kLeft, kRight, kUpLeft, kUpRight, kDownLeft, kDownRight, kRandom, kAlternate, kDirCount };
enum Order { kLTR, kRTL, kCenter, kEdges, kRandomOrder, kOddEven, kAll, kOrderCount };
enum Anchor { kLetter, kTextCenter, kBottomLeft, kTopCenter, kBottomCenter, kAnchorCount };
enum Blend { kNormal, kMultiply, kOverlay, kScreen, kAdd, kBlendCount };
enum Font { kHand, kMarker, kTypewriter, kSerif, kSerifBold, kSerifFine, kSans, kSansBold, kImpact,
            kCondensed, kMono, kScript, kGothic, kStencil, kRounded, kRoman, kChalk, kBrush, kFontCount };

enum InKind { kInNone, kInFade, kInPop, kInSlide, kInDrop, kInZoom, kInFocus, kInStrobe, kInStep,
              kInSpinY, kInSpinX, kInSpinZ, kInWipe, kInDissolve, kInScramble, kInSplit, kInChroma,
              kInFlash, kInSpread, kInGlow, kInStretch, kInCount };
enum OutKind { kOutNone, kOutFade, kOutSlide, kOutFall, kOutZoomOut, kOutZoomThrough, kOutSquashY,
               kOutSquashX, kOutCRT, kOutTear, kOutBurn, kOutDissolve, kOutBlur, kOutFlash, kOutMelt,
               kOutScatter, kOutStrobe, kOutRise, kOutChroma, kOutCut, kOutWipe, kOutSpinY, kOutStretch,
               kOutCount };
enum IdleKind { kIdleNone, kIdleJitter, kIdleBop, kIdleWave, kIdlePulse, kIdleFlicker, kIdleWobble,
                kIdleDrift, kIdleBreathe, kIdleSlowZoom, kIdleGlowPulse, kIdleSweep, kIdleScanline,
                kIdleChromaJit, kIdleBlurPulse, kIdleFloat, kIdleInterlace, kIdleSteps, kIdleCount };

// Tables de noms (libellé UI). Le jeton DSL = libellé en minuscules, alphanumérique seul.
const char* const* names(const char* table, int& count);  // "in","out","idle","curve","dir","order","anchor","blend","font"
int findName(const char* table, const std::string& token, int fallback);
std::string tokenOf(const std::string& label);

struct Rgb { float r = 1, g = 1, b = 1; };

struct Anim {
  int kind = 0;
  float frames = 10;   // durée par lettre (idle : période)
  float a = 0, b = 0, c = 0;
  int dir = 0;
  int curve = kEaseOut;
};

// Tout ce que décrit un preset ET tout ce que l'inspecteur expose.
struct Params {
  std::string name, category;
  int font = kSans;
  std::string fontName;          // surcharge : nom partiel ou chemin de fichier
  int fps = 0;                   // 0 = cadence du projet, sinon stop-motion
  int order = kLTR;
  float delay = 2, outDelay = 1; // frames entre deux lettres
  int anchor = kLetter;
  float tracking = 0, lineSpacing = 1.15f;
  bool upper = false;
  Anim in, idle, out;
  Rgb fill{1, 1, 1}, gradTop{1, 1, 1}, gradBottom{1, 1, 1}, gradLeft{1, 1, 1}, gradRight{1, 1, 1};
  float gradAmt = 0;
  bool gradH = false;            // dégradé horizontal (gauche/droite) en plus du vertical
  Rgb stroke{0, 0, 0}; float strokeW = 0;            // em
  Rgb shadow{0, 0, 0}; float shadowDx = 0, shadowDy = 0, shadowBlur = 0, shadowAlpha = 0;
  Rgb glow{1, 1, 1}; float glowR = 0, glowAmt = 0;
  float grain = 0;
  Rgb plate{0, 0, 0}; float plateAlpha = 0, platePad = 0.3f;
  int blend = kNormal;
  int scrambleSet = 0;           // 0 alphanum, 1 binaire
  float motionBlur = 1.f;        // 0..2 (1 = obturateur 180°)
  float opacity = 1.f;
};

// ---- Mise en page --------------------------------------------------------
struct GlyphImg {
  int w = 0, h = 0;
  float ox = 0, oy = 0;          // origine du bitmap, relative au pivot du glyphe (px)
  std::vector<float> a;
  float at(int x, int y) const { return (x < 0 || y < 0 || x >= w || y >= h) ? 0.f : a[(size_t)y * w + x]; }
  float sample(float lx, float ly) const;
};

struct Glyph {
  uint32_t cp = 0;
  float px = 0, py = 0;          // pivot, relatif au centre du titre (px, y vers le bas)
  float adv = 0;
  int line = 0;
  GlyphImg fill, stroke;
};

struct Layout {
  std::vector<Glyph> g;
  std::vector<GlyphImg> altFill, altStroke;
  float em = 1;
  float minX = 0, maxX = 0, minY = 0, maxY = 0;   // boîte du titre, relative au centre
};

// Chemin de police pour une classe (ou un nom utilisateur). "" si rien trouvé.
std::string resolveFont(int fontClass, const std::string& userName);

std::shared_ptr<Layout> buildLayout(const std::string& utf8, int px, const std::string& fontPath,
                                    float tracking, float lineSpacing, float strokeEm, int scrambleSet);

// ---- Rendu ---------------------------------------------------------------
struct Times {
  double tin = 0;      // frames depuis le début de l'entrée (<0 : pas commencé)
  double tout = -1;    // frames depuis le début de la sortie (<0 : pas commencée)
  double tidle = 0;    // frames depuis le début
  double shutter = 0.5;// frames d'exposition pour le motion blur
  int seed = 1;
};

struct Canvas {
  int w = 0, h = 0;
  std::vector<float> px;   // RGBA prémultiplié, y vers le bas
  void init(int W, int H) { w = W; h = H; px.assign((size_t)W * H * 4, 0.f); }
};

// (cx, cy) : centre du titre dans le canevas (px, y vers le bas).
void renderTitle(const Layout& L, const Params& P, const Times& T, double cx, double cy, Canvas& C);

// Durée (frames) de l'entrée / de la sortie de tout le titre.
double entrySpan(const Params& P, int nGlyphs);
double exitSpan(const Params& P, int nGlyphs);

// Presets compilés (voir TitlePresetsData.inc).
const std::vector<Params>& presets();

}  // namespace tc

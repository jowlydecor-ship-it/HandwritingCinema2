#include "TitleEngine.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <dirent.h>
#include <sys/stat.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <mutex>

#include "Noise.h"

namespace tc {

namespace {

constexpr float kPi = 3.14159265358979f;

const char* const kInNames[] = {"None", "Fade", "Pop", "Slide", "Drop", "Zoom", "Focus", "Strobe", "Step",
                                "Spin Y", "Spin X", "Spin Z", "Wipe", "Dissolve", "Scramble", "Split",
                                "Chroma", "Flash", "Spread", "Glow In", "Stretch", "Exposure"};
const char* const kOutNames[] = {"None", "Fade", "Slide", "Fall", "Zoom Out", "Zoom Through", "Squash Y",
                                 "Squash X", "CRT Off", "Tear", "Burn", "Dissolve", "Blur", "Flash", "Melt",
                                 "Scatter", "Strobe", "Rise", "Chroma", "Cut", "Wipe", "Spin Y", "Stretch", "Whiteout",
                                 "Blackout", "Step Off"};
const char* const kIdleNames[] = {"None", "Jitter", "Bop", "Wave", "Pulse", "Flicker", "Wobble", "Drift",
                                  "Breathe", "Slow Zoom", "Glow Pulse", "Sweep", "Scanline",
                                  "Chroma Jitter", "Blur Pulse", "Float", "Interlace", "Steps"};
const char* const kCurveNames[] = {"Linear", "Ease Out", "Ease In", "Ease In-Out", "Elastic", "Bounce",
                                   "Back Out", "Expo Out", "Steps", "Ease Out Quad", "Ease Out Expo", "Ease Out Cubic",
                                   "Ease Out Back", "Ease In Quad", "Ease In-Out Quad", "Fast Linear"};
const char* const kDirNames[] = {"Up", "Down", "Left", "Right", "Up-Left", "Up-Right", "Down-Left",
                                 "Down-Right", "Random", "Alternate"};
const char* const kOrderNames[] = {"Left to Right", "Right to Left", "Center Out", "Edges In", "Random",
                                   "Odd / Even", "All Together"};
const char* const kAnchorNames[] = {"Letter Center", "Text Center", "Bottom Left", "Top Center",
                                    "Bottom Center"};
const char* const kBlendNames[] = {"Normal", "Multiply", "Overlay", "Screen", "Add"};
const char* const kFontNames[] = {"Handwritten", "Marker Cartoon", "Typewriter", "Serif", "Serif Bold",
                                  "Serif Fine", "Sans", "Sans Bold", "Impact", "Condensed", "Mono", "Script",
                                  "Gothic", "Stencil", "Rounded", "Roman Caps", "Chalk", "Brush"};

const char* const kParticleNames[] = {"None", "Snow", "Rain", "Dust", "Sparkle", "Ash", "Bokeh", "Bubbles",
                                     "Embers", "Spores", "Confetti", "Static"};
const char* const kTextureNames[] = {"None", "Fine Grain", "Paper", "Concrete", "Brushed Metal", "Sand",
                                     "Rust", "Oil Impasto", "Scratched", "Halftone", "Wood"};

template <size_t N>
const char* const* pick(const char* const (&arr)[N], int& n) { n = static_cast<int>(N); return arr; }

inline float clamp01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }
inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline float smoothstep(float e0, float e1, float x) {
  const float t = clamp01((x - e0) / (e1 - e0));
  return t * t * (3.f - 2.f * t);
}

inline float hash01(uint32_t a, uint32_t b = 0, uint32_t c = 0) {
  uint32_t h = a * 0x9E3779B1u ^ (b + 0x7F4A7C15u) * 0x85EBCA6Bu ^ (c + 0x165667B1u) * 0xC2B2AE35u;
  h ^= h >> 15; h *= 0x2C1B3C6Du; h ^= h >> 12; h *= 0x297A2D39u; h ^= h >> 15;
  return (h & 0xFFFFFFu) / 16777216.f;
}

bool fileExists(const std::string& p) {
  struct stat st;
  return ::stat(p.c_str(), &st) == 0;
}

std::vector<uint32_t> decodeUtf8(const std::string& s) {
  std::vector<uint32_t> out;
  for (size_t i = 0; i < s.size();) {
    unsigned char c = static_cast<unsigned char>(s[i]);
    uint32_t cp = 0;
    int n = 0;
    if (c < 0x80) { cp = c; n = 1; }
    else if ((c >> 5) == 0x6) { cp = c & 0x1F; n = 2; }
    else if ((c >> 4) == 0xE) { cp = c & 0x0F; n = 3; }
    else if ((c >> 3) == 0x1E) { cp = c & 0x07; n = 4; }
    else { ++i; continue; }
    for (int k = 1; k < n && i + k < s.size(); ++k)
      cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3F);
    out.push_back(cp);
    i += n;
  }
  return out;
}

}  // namespace

// ---- Tables de noms -------------------------------------------------------
const char* const* names(const char* table, int& count) {
  const std::string t = table;
  if (t == "in") return pick(kInNames, count);
  if (t == "out") return pick(kOutNames, count);
  if (t == "idle") return pick(kIdleNames, count);
  if (t == "curve") return pick(kCurveNames, count);
  if (t == "dir") return pick(kDirNames, count);
  if (t == "order") return pick(kOrderNames, count);
  if (t == "anchor") return pick(kAnchorNames, count);
  if (t == "blend") return pick(kBlendNames, count);
  if (t == "font") return pick(kFontNames, count);
  if (t == "particle") return pick(kParticleNames, count);
  if (t == "texture") return pick(kTextureNames, count);
  count = 0;
  return nullptr;
}

std::string tokenOf(const std::string& label) {
  std::string out;
  for (unsigned char c : label)
    if (std::isalnum(c)) out += static_cast<char>(std::tolower(c));
  return out;
}

int findName(const char* table, const std::string& token, int fallback) {
  int n = 0;
  const char* const* arr = names(table, n);
  const std::string want = tokenOf(token);
  for (int i = 0; i < n; ++i)
    if (tokenOf(arr[i]) == want) return i;
  for (int i = 0; i < n; ++i)                      // préfixe ("marker" -> "Marker Cartoon")
    if (!want.empty() && tokenOf(arr[i]).compare(0, want.size(), want) == 0) return i;
  return fallback;
}

// ---- Glyph image ----------------------------------------------------------
float GlyphImg::sample(float lx, float ly) const {
  const float x = lx - ox - 0.5f, y = ly - oy - 0.5f;
  const int x0 = static_cast<int>(std::floor(x)), y0 = static_cast<int>(std::floor(y));
  const float fx = x - x0, fy = y - y0;
  const float top = at(x0, y0) * (1.f - fx) + at(x0 + 1, y0) * fx;
  const float bot = at(x0, y0 + 1) * (1.f - fx) + at(x0 + 1, y0 + 1) * fx;
  return top * (1.f - fy) + bot * fy;
}

// ---- Polices --------------------------------------------------------------
namespace {

std::string toLower(std::string s) {
  for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return s;
}

std::string scanDirs(const std::string& wanted) {
  const char* home = std::getenv("HOME");
  std::vector<std::string> dirs = {"/System/Library/Fonts", "/System/Library/Fonts/Supplemental",
                                   "/Library/Fonts", "/usr/share/fonts/truetype/dejavu",
                                   "/usr/share/fonts/truetype/liberation",
                                   "/usr/share/fonts/truetype/freefont"};
  if (home) dirs.push_back(std::string(home) + "/Library/Fonts");
  const std::string wTok = tokenOf(wanted);
  if (wTok.empty()) return "";
  std::string best; size_t bestLen = 1u << 30;
  for (const auto& d : dirs) {
    DIR* dir = opendir(d.c_str());
    if (!dir) continue;
    while (dirent* e = readdir(dir)) {
      const std::string fn = e->d_name;
      const std::string ext = toLower(fn.size() > 4 ? fn.substr(fn.size() - 4) : fn);
      if (ext != ".ttf" && ext != ".otf" && ext != ".ttc") continue;
      const std::string tk = tokenOf(fn.substr(0, fn.size() - 4));
      if (tk.find(wTok) == std::string::npos) continue;
      // meilleure correspondance : nom le plus court, exact en priorité
      const size_t score = tk == wTok ? 0 : tk.size();
      if (score < bestLen) { bestLen = score; best = d + "/" + fn; }
    }
    closedir(dir);
    if (bestLen == 0) break;
  }
  if (!best.empty()) return best;
  return "";
}

std::string firstExisting(std::initializer_list<const char*> list) {
  for (const char* p : list)
    if (fileExists(p)) return p;
  return "";
}

}  // namespace

std::string resolveFont(int fontClass, const std::string& userName) {
  if (!userName.empty()) {
    size_t start = 0;
    while (start <= userName.size()) {          // "Futura~Helvetica Neue" : premier trouvé
      size_t end = userName.find('~', start);
      if (end == std::string::npos) end = userName.size();
      std::string one = userName.substr(start, end - start);
      while (!one.empty() && one.front() == ' ') one.erase(one.begin());
      while (!one.empty() && one.back() == ' ') one.pop_back();
      if (!one.empty()) {
        if (one.find('/') != std::string::npos && fileExists(one)) return one;
        const std::string hit = scanDirs(one);
        if (!hit.empty()) return hit;
      }
      start = end + 1;
    }
  }
  const std::string S = "/System/Library/Fonts/", SS = "/System/Library/Fonts/Supplemental/";
  std::string r;
  switch (fontClass) {
    case kHand:
      r = firstExisting({(SS + "Bradley Hand Bold.ttf").c_str(), (S + "Noteworthy.ttc").c_str(),
                         (S + "MarkerFelt.ttc").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSans-Oblique.ttf"});
      break;
    case kMarker:
      r = firstExisting({(S + "MarkerFelt.ttc").c_str(), (SS + "Comic Sans MS Bold.ttf").c_str(),
                         (SS + "Impact.ttf").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"});
      break;
    case kTypewriter:
      r = firstExisting({(SS + "AmericanTypewriter.ttc").c_str(), (SS + "Courier New.ttf").c_str(),
                         (S + "Courier.ttc").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"});
      break;
    case kSerif:
      r = firstExisting({(SS + "Georgia.ttf").c_str(), (SS + "Times New Roman.ttf").c_str(),
                         "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf"});
      break;
    case kSerifBold:
      r = firstExisting({(SS + "Georgia Bold.ttf").c_str(), (SS + "Times New Roman Bold.ttf").c_str(),
                         "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf"});
      break;
    case kSerifFine:
      r = firstExisting({(SS + "Didot.ttc").c_str(), (SS + "Bodoni 72.ttc").c_str(), (SS + "Baskerville.ttc").c_str(),
                         (SS + "Georgia.ttf").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf"});
      break;
    case kSans:
      r = firstExisting({(S + "HelveticaNeue.ttc").c_str(), (S + "Helvetica.ttc").c_str(), (SS + "Arial.ttf").c_str(),
                         "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"});
      break;
    case kSansBold:
      r = firstExisting({(SS + "Arial Black.ttf").c_str(), (SS + "Arial Bold.ttf").c_str(),
                         "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"});
      break;
    case kImpact:
    case kStencil:
      r = firstExisting({(SS + "Impact.ttf").c_str(), (SS + "Arial Black.ttf").c_str(),
                         "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"});
      break;
    case kCondensed:
      r = firstExisting({(S + "Avenir Next Condensed.ttc").c_str(), (SS + "Arial Narrow Bold.ttf").c_str(),
                         (SS + "Impact.ttf").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSansCondensed-Bold.ttf"});
      break;
    case kMono:
      r = firstExisting({(S + "Menlo.ttc").c_str(), (S + "Monaco.ttf").c_str(), (SS + "Courier New.ttf").c_str(),
                         "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"});
      break;
    case kScript:
      r = firstExisting({(SS + "SnellRoundhand.ttc").c_str(), (SS + "Brush Script.ttf").c_str(),
                         (SS + "Zapfino.ttf").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Italic.ttf"});
      break;
    case kGothic:
      r = firstExisting({(SS + "Luminari.ttf").c_str(), (SS + "Papyrus.ttc").c_str(), (SS + "Herculanum.ttf").c_str(),
                         (SS + "Georgia Bold.ttf").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf"});
      break;
    case kRounded:
      r = firstExisting({(S + "ArialRoundedMTBold.ttf").c_str(), (SS + "Arial Rounded Bold.ttf").c_str(),
                         (SS + "Comic Sans MS.ttf").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"});
      break;
    case kRoman:
      r = firstExisting({(SS + "Copperplate.ttc").c_str(), (S + "Optima.ttc").c_str(), (SS + "Trattatello.ttf").c_str(),
                         (SS + "Georgia.ttf").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf"});
      break;
    case kChalk:
      r = firstExisting({(SS + "Chalkduster.ttf").c_str(), (S + "Noteworthy.ttc").c_str(),
                         "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"});
      break;
    case kBrush:
    default:
      r = firstExisting({(SS + "Brush Script.ttf").c_str(), (S + "Noteworthy.ttc").c_str(),
                         (S + "MarkerFelt.ttc").c_str(), "/usr/share/fonts/truetype/dejavu/DejaVuSans-BoldOblique.ttf"});
      break;
  }
  if (r.empty())
    r = firstExisting({"/System/Library/Fonts/Helvetica.ttc", "/System/Library/Fonts/SFNS.ttf",
                       "/System/Library/Fonts/Supplemental/Arial.ttf",
                       "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"});
  return r;
}

// ---- Mise en page ----------------------------------------------------------
namespace {

// Dilatation (épaisseur du contour) : disque exact pour les petits rayons, carré sinon.
GlyphImg dilate(const GlyphImg& in, int r) {
  GlyphImg out;
  if (r <= 0) return in;
  out.w = in.w + 2 * r;
  out.h = in.h + 2 * r;
  out.ox = in.ox - r;
  out.oy = in.oy - r;
  out.a.assign((size_t)out.w * out.h, 0.f);
  const bool disc = r <= 12;
  // pad
  std::vector<float> src((size_t)out.w * out.h, 0.f);
  for (int y = 0; y < in.h; ++y)
    for (int x = 0; x < in.w; ++x) src[(size_t)(y + r) * out.w + (x + r)] = in.a[(size_t)y * in.w + x];

  if (disc) {
    std::vector<int> hw(2 * r + 1);
    for (int dy = -r; dy <= r; ++dy) hw[dy + r] = static_cast<int>(std::floor(std::sqrt(std::max(0.f, (float)(r * r - dy * dy)) + 0.25f)));
    for (int y = 0; y < out.h; ++y)
      for (int x = 0; x < out.w; ++x) {
        float m = 0.f;
        for (int dy = -r; dy <= r; ++dy) {
          const int yy = y + dy;
          if (yy < 0 || yy >= out.h) continue;
          const int w = hw[dy + r];
          const float* row = &src[(size_t)yy * out.w];
          for (int dx = -w; dx <= w; ++dx) {
            const int xx = x + dx;
            if (xx < 0 || xx >= out.w) continue;
            m = std::max(m, row[xx]);
          }
        }
        out.a[(size_t)y * out.w + x] = m;
      }
  } else {
    std::vector<float> tmp((size_t)out.w * out.h, 0.f);
    for (int y = 0; y < out.h; ++y)
      for (int x = 0; x < out.w; ++x) {
        float m = 0.f;
        for (int dx = -r; dx <= r; ++dx) {
          const int xx = x + dx;
          if (xx >= 0 && xx < out.w) m = std::max(m, src[(size_t)y * out.w + xx]);
        }
        tmp[(size_t)y * out.w + x] = m;
      }
    for (int y = 0; y < out.h; ++y)
      for (int x = 0; x < out.w; ++x) {
        float m = 0.f;
        for (int dy = -r; dy <= r; ++dy) {
          const int yy = y + dy;
          if (yy >= 0 && yy < out.h) m = std::max(m, tmp[(size_t)yy * out.w + x]);
        }
        out.a[(size_t)y * out.w + x] = m;
      }
  }
  return out;
}

struct Rendered { GlyphImg img; double adv = 0; bool ok = false; };

Rendered renderGlyph(FT_Face face, uint32_t cp, float pivotDown, float italic, float embolden, int px) {
  Rendered r;
  FT_Matrix m;
  m.xx = 0x10000; m.xy = static_cast<FT_Fixed>(italic * 0x10000); m.yx = 0; m.yy = 0x10000;
  FT_Set_Transform(face, &m, nullptr);
  if (FT_Load_Char(face, cp, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP) != 0) {
    // police bitmap seule : repli sur le rendu direct
    if (FT_Load_Char(face, cp, FT_LOAD_RENDER | FT_LOAD_NO_HINTING) != 0) return r;
  } else {
    if (embolden > 0.f && face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
      FT_Outline_Embolden(&face->glyph->outline, static_cast<FT_Pos>(embolden * px * 64.f));
    if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) return r;
  }
  const FT_GlyphSlot s = face->glyph;
  r.adv = s->advance.x / 64.0 + (embolden > 0.f ? embolden * px : 0.0);
  r.ok = true;
  const FT_Bitmap& bm = s->bitmap;
  r.img.w = static_cast<int>(bm.width);
  r.img.h = static_cast<int>(bm.rows);
  r.img.ox = static_cast<float>(s->bitmap_left - r.adv * 0.5);
  r.img.oy = pivotDown - static_cast<float>(s->bitmap_top);
  r.img.a.assign((size_t)std::max(0, r.img.w) * std::max(0, r.img.h), 0.f);
  for (unsigned row = 0; row < bm.rows; ++row)
    for (unsigned col = 0; col < bm.width; ++col)
      r.img.a[(size_t)row * bm.width + col] = bm.buffer[row * bm.pitch + col] / 255.f;
  return r;
}

}  // namespace

std::shared_ptr<Layout> buildLayout(const std::string& utf8, int px, const std::string& fontPath,
                                    float tracking, float lineSpacing, float strokeEm, int scrambleSet,
                                    float italic, float embolden) {
  if (fontPath.empty()) return nullptr;
  px = std::max(6, px);
  FT_Library lib;
  if (FT_Init_FreeType(&lib)) return nullptr;
  FT_Face face = nullptr;
  if (FT_New_Face(lib, fontPath.c_str(), 0, &face) != 0) {
    FT_Done_FreeType(lib);
    return nullptr;
  }
  FT_Set_Pixel_Sizes(face, 0, px);

  auto L = std::make_shared<Layout>();
  L->em = static_cast<float>(px);
  const float pivotDown = 0.33f * px;
  const int sr = static_cast<int>(std::lround(strokeEm * px));

  // lignes
  std::vector<std::vector<uint32_t>> lines(1);
  for (uint32_t c : decodeUtf8(utf8)) {
    if (c == '\r') continue;
    if (c == '\n') lines.emplace_back();
    else lines.back().push_back(c);
  }
  const double lineH = px * std::max(0.6f, lineSpacing);
  const double trk = tracking * px;

  struct Item { Glyph g; double penX; };
  std::vector<std::vector<Item>> rows(lines.size());
  std::vector<double> widths(lines.size(), 0.0);
  for (size_t li = 0; li < lines.size(); ++li) {
    double pen = 0.0;
    for (uint32_t c : lines[li]) {
      Rendered r = renderGlyph(face, c, pivotDown, italic, embolden, px);
      if (!r.ok) continue;
      if (r.img.w > 0 && r.img.h > 0) {
        Item it;
        it.g.cp = c;
        it.g.adv = static_cast<float>(r.adv);
        it.g.line = static_cast<int>(li);
        it.g.fill = std::move(r.img);
        it.penX = pen + r.adv * 0.5;  // pivot = centre de la cellule
        rows[li].push_back(std::move(it));
      }
      pen += r.adv + trk;
    }
    widths[li] = std::max(0.0, pen - trk);
  }

  const double nLines = static_cast<double>(lines.size());
  float minX = 1e9f, maxX = -1e9f, minY = 1e9f, maxY = -1e9f;
  for (size_t li = 0; li < lines.size(); ++li) {
    const double y = (static_cast<double>(li) - (nLines - 1.0) * 0.5) * lineH;
    int kIdx = 0;
    const int lineN = static_cast<int>(rows[li].size());
    for (auto& it : rows[li]) {
      it.g.ci = static_cast<float>(kIdx++) - (lineN - 1) * 0.5f;
      it.g.px = static_cast<float>(it.penX - widths[li] * 0.5);
      it.g.py = static_cast<float>(y);
      if (sr > 0) it.g.stroke = dilate(it.g.fill, sr);
      const GlyphImg& im = sr > 0 ? it.g.stroke : it.g.fill;
      minX = std::min(minX, it.g.px + im.ox);
      maxX = std::max(maxX, it.g.px + im.ox + im.w);
      minY = std::min(minY, it.g.py + im.oy);
      maxY = std::max(maxY, it.g.py + im.oy + im.h);
      L->g.push_back(std::move(it.g));
    }
  }
  if (L->g.empty()) {
    FT_Done_Face(face);
    FT_Done_FreeType(lib);
    return L;
  }
  L->minX = minX; L->maxX = maxX; L->minY = minY; L->maxY = maxY;

  // glyphes de remplacement (Scramble)
  const std::string pool = scrambleSet == 1 ? "01" : "0123456789ABCDEFGHJKLMNPRSTUVWXZ";
  for (char c : pool) {
    Rendered r = renderGlyph(face, static_cast<uint32_t>(c), pivotDown, italic, embolden, px);
    if (!r.ok || r.img.w <= 0) continue;
    L->altStroke.push_back(sr > 0 ? dilate(r.img, sr) : GlyphImg());
    L->altFill.push_back(std::move(r.img));
  }
  FT_Done_Face(face);
  FT_Done_FreeType(lib);
  return L;
}

// ---- Courbes ----------------------------------------------------------------
namespace {

float cubicBezier(float x1, float y1, float x2, float y2, float x) {
  float lo = 0.f, hi = 1.f, t = x;
  for (int i = 0; i < 26; ++i) {
    const float u = 1.f - t;
    const float cx = 3.f * u * u * t * x1 + 3.f * u * t * t * x2 + t * t * t;
    if (cx < x) lo = t; else hi = t;
    t = 0.5f * (lo + hi);
  }
  const float u = 1.f - t;
  return 3.f * u * u * t * y1 + 3.f * u * t * t * y2 + t * t * t;
}

float applyCurve(int curve, float x) {
  x = clamp01(x);
  if (x >= 1.f) return 1.f;
  switch (curve) {
    case kEaseOutQuad: return cubicBezier(0.25f, 1.f, 0.5f, 1.f, x);
    case kEaseOutExpo: return cubicBezier(0.16f, 1.f, 0.3f, 1.f, x);
    case kEaseOutCubic: return cubicBezier(0.215f, 0.61f, 0.355f, 1.f, x);
    case kEaseOutBack: return cubicBezier(0.34f, 1.56f, 0.64f, 1.f, x);
    case kEaseInQuad: return cubicBezier(0.55f, 0.085f, 0.68f, 0.53f, x);
    case kEaseInOutQuad: return cubicBezier(0.4f, 0.f, 0.2f, 1.f, x);
    case kFastLinear: return cubicBezier(0.f, 0.f, 0.2f, 1.f, x);
    default: break;
  }
  switch (curve) {
    case kLinear: return x;
    case kEaseOut: { const float u = 1.f - x; return 1.f - u * u * u; }
    case kEaseIn: return x * x * x;
    case kEaseInOut: return x < 0.5f ? 4.f * x * x * x : 1.f - std::pow(-2.f * x + 2.f, 3.f) * 0.5f;
    case kElastic:
      if (x >= 1.f) return 1.f;
      return 1.f - std::exp(-7.5f * x) * std::cos(4.f * kPi * x);   // dépassement ~15 %, 2 rebonds
    case kBounce: {
      const float n1 = 7.5625f, d1 = 2.75f;
      if (x < 1.f / d1) return n1 * x * x;
      if (x < 2.f / d1) { x -= 1.5f / d1; return n1 * x * x + 0.75f; }
      if (x < 2.5f / d1) { x -= 2.25f / d1; return n1 * x * x + 0.9375f; }
      x -= 2.625f / d1; return n1 * x * x + 0.984375f;
    }
    case kBackOut: { const float c1 = 1.70158f, c3 = c1 + 1.f; const float u = x - 1.f; return 1.f + c3 * u * u * u + c1 * u * u; }
    case kExpoOut: return x >= 1.f ? 1.f : 1.f - std::pow(2.f, -10.f * x);
    case kSteps: return x >= 1.f ? 1.f : std::floor(x * 4.f) / 4.f;
  }
  return x;
}

struct GState {
  float dx = 0, dy = 0, rot = 0, sx = 1, sy = 1, alpha = 1;
  float blurX = 0, blurY = 0, chroma = 0, glow = 0, flash = 0, dissolve = 0, block = 0;
  float wipe = 1; int wipeDir = kLeft;
  float topDx = 0, botDx = 0, topDy = 0, botDy = 0;
  bool scramble = false;
  float frameWhite = 0, frameBlack = 0;
};

struct Globals { float sweep = -10, sweepW = 0.12f, sweepAmt = 0.8f, scan = -10, scanAmt = 0.5f; };

struct GInfo { int i, n, rank, nRank; float u; uint32_t id; int seed; int fin, fout, fidle; float ci = 0.f; };

void dirVec(int dir, const GInfo& gi, float& vx, float& vy) {
  if (dir == kAlternate) { static const int seq[4] = {kUp, kDown, kLeft, kRight}; dir = seq[gi.i & 3]; }
  if (dir == kRandom) { const float a = hash01(gi.id, gi.seed, 91) * 2.f * kPi; vx = std::cos(a); vy = std::sin(a); return; }
  switch (dir) {
    case kUp: vx = 0; vy = -1; break;
    case kDown: vx = 0; vy = 1; break;
    case kLeft: vx = -1; vy = 0; break;
    case kRight: vx = 1; vy = 0; break;
    case kUpLeft: vx = -0.7071f; vy = -0.7071f; break;
    case kUpRight: vx = 0.7071f; vy = -0.7071f; break;
    case kDownLeft: vx = -0.7071f; vy = 0.7071f; break;
    case kDownRight: vx = 0.7071f; vy = 0.7071f; break;
    default: vx = 0; vy = 0;
  }
}

inline float rnd(const GInfo& gi, int salt) { return hash01(gi.id, gi.seed, salt); }
inline float sgn(const GInfo& gi, int salt) { return rnd(gi, salt) * 2.f - 1.f; }

// "flicker" tiré au sort pour les apparitions / disparitions stroboscopiques
float strobePattern(const GInfo& gi, int frame) {
  const float h = hash01(gi.id * 7 + 3, frame, gi.seed);
  return h < 0.35f ? 0.f : (h < 0.6f ? 0.3f : 1.f);
}

void applyIn(const Anim& A, float tl /*frames locales*/, const GInfo& gi, GState& s) {
  if (A.kind == kInNone) return;
  if (tl <= 0.f) { s.alpha = 0.f; return; }
  const float e = clamp01(tl / std::max(0.5f, A.frames));
  const float c = applyCurve(A.curve, e);
  float vx, vy;
  switch (A.kind) {
    case kInFade: s.alpha *= clamp01(c); break;
    case kInPop: {
      const float k = lerp(A.a, 1.f, c);
      s.sx *= k; s.sy *= k;
      s.rot += (1.f - c) * A.b * kPi / 180.f * sgn(gi, 11);
      break;
    }
    case kInSlide:
      dirVec(A.dir, gi, vx, vy);
      s.dx += vx * (1.f - c) * (A.a != 0.f ? A.a : 1.5f);
      s.dy += vy * (1.f - c) * (A.a != 0.f ? A.a : 1.5f);
      if (A.b > 1.5f) s.alpha *= clamp01(c);
      else if (A.b > 0.5f) s.alpha *= clamp01(e * 3.f);
      break;
    case kInDrop:
      dirVec(A.dir, gi, vx, vy);
      if (vx == 0.f && vy == 0.f) vy = -1.f;
      s.dx += vx * (1.f - c) * (A.a != 0.f ? A.a : 3.f);
      s.dy += vy * (1.f - c) * (A.a != 0.f ? A.a : 3.f);
      s.rot += (1.f - c) * A.b * kPi / 180.f * sgn(gi, 12);
      break;
    case kInZoom: {
      const float k = lerp(A.a, 1.f, c);
      s.sx *= k; s.sy *= k;
      if (A.b > 1.5f) s.alpha *= clamp01(c);
      else if (A.b > 0.5f) s.alpha *= clamp01(e * 3.f);
      break;
    }
    case kInFocus: {
      const float k = lerp(A.a, 1.f, c);
      s.sx *= k; s.sy *= k;
      const bool vert = A.dir == kUp || A.dir == kDown;
      const bool hor = A.dir == kLeft || A.dir == kRight;
      s.blurX += (vert ? 0.f : 1.f) * (1.f - e) * A.b;
      s.blurY += (hor ? 0.f : 1.f) * (1.f - e) * A.b;
      s.alpha *= clamp01(e * 1.2f);
      break;
    }
    case kInStrobe:
      if (e < 1.f) {
        s.alpha *= strobePattern(gi, gi.fin);
        if (hash01(gi.id, gi.fin, 77) > 0.7f) s.dx += (hash01(gi.id, gi.fin, 78) - 0.5f) * 2.f * A.a;
      }
      break;
    case kInStep: break;
    case kInSpinY: s.sx *= std::cos((1.f - c) * A.a * kPi / 180.f); break;
    case kInSpinX: s.sy *= std::cos((1.f - c) * A.a * kPi / 180.f); break;
    case kInSpinZ: {
      s.rot += (1.f - c) * A.a * kPi / 180.f;
      if (A.b != 0.f) { const float k = lerp(A.b, 1.f, c); s.sx *= k; s.sy *= k; }
      break;
    }
    case kInWipe: s.wipe = c; s.wipeDir = A.dir; break;
    case kInDissolve: s.dissolve = std::max(s.dissolve, 1.f - e); s.block = A.b; break;
    case kInScramble: if (e < 1.f) s.scramble = true; break;
    case kInSplit:
      if (A.dir == kLeft || A.dir == kRight) { s.topDx -= (1.f - c) * A.a; s.botDx += (1.f - c) * A.a; }
      else { s.topDy -= (1.f - c) * A.a; s.botDy += (1.f - c) * A.a; }
      break;
    case kInChroma: s.chroma += (1.f - c) * A.a; s.alpha *= clamp01(e * 3.f); break;
    case kInFlash: s.flash = std::max(s.flash, (1.f - e) * (1.f - e) * (A.a != 0.f ? A.a : 1.f)); break;
    case kInSpread: s.dx += gi.ci * (1.f - c) * A.a; s.alpha *= (A.b > 0.5f) ? clamp01(c) : clamp01(e * 3.f); break;
    case kInGlow: s.glow += (1.f - e) * A.a; s.alpha *= clamp01(e * 1.5f); break;
    case kInStretch: s.sx *= lerp(A.a, 1.f, c); s.sy *= lerp(A.b, 1.f, c); s.alpha *= clamp01(e * 4.f); break;
    case kInExposure: {
      s.frameWhite = std::max(s.frameWhite, (1.f - e) * (A.a != 0.f ? A.a : 1.f));
      const float k = lerp(A.b != 0.f ? A.b : 1.f, 1.f, c);
      s.sx *= k; s.sy *= k;
      break;
    }
  }
}

void applyOut(const Anim& A, float tl, const GInfo& gi, GState& s) {
  if (A.kind == kOutNone || tl <= 0.f) return;
  const float x = clamp01(tl / std::max(0.5f, A.frames));
  const float c = applyCurve(A.curve, x);
  const bool gone = x >= 1.f;
  float vx, vy;
  switch (A.kind) {
    case kOutFade: s.alpha *= 1.f - c; break;
    case kOutSlide:
      dirVec(A.dir, gi, vx, vy);
      s.dx += vx * c * (A.a != 0.f ? A.a : 1.5f);
      s.dy += vy * c * (A.a != 0.f ? A.a : 1.5f);
      if (A.b > 0.5f) s.alpha *= 1.f - smoothstep(0.6f, 1.f, x);
      if (gone) s.alpha = 0.f;
      break;
    case kOutFall:
      s.dy += c * (A.a != 0.f ? A.a : 3.f);
      s.rot += c * A.b * kPi / 180.f * sgn(gi, 21);
      if (gone) s.alpha = 0.f;
      break;
    case kOutZoomOut: { const float k = lerp(1.f, A.a, c); s.sx *= k; s.sy *= k; s.alpha *= gone ? 0.f : 1.f - c * c; break; }
    case kOutZoomThrough: { const float k = lerp(1.f, A.a != 0.f ? A.a : 6.f, c); s.sx *= k; s.sy *= k; s.alpha *= 1.f - x * x; break; }
    case kOutSquashY: s.sy *= std::max(0.f, 1.f - c); if (gone) s.alpha = 0.f; break;
    case kOutSquashX: s.sx *= std::max(0.f, 1.f - c); if (gone) s.alpha = 0.f; break;
    case kOutCRT: {
      const float p1 = smoothstep(0.f, 0.55f, x), p2 = smoothstep(0.55f, 1.f, x);
      s.sy *= std::max(0.f, 1.f - 0.985f * p1);
      s.sx *= std::max(0.f, 1.f - p2);
      s.flash = std::max(s.flash, p1 * 0.7f);
      if (gone) s.alpha = 0.f;
      break;
    }
    case kOutTear:
      s.topDx -= c * (A.a != 0.f ? A.a : 1.5f);
      s.botDx += c * (A.a != 0.f ? A.a : 1.5f);
      s.alpha *= 1.f - smoothstep(0.6f, 1.f, x);
      if (gone) s.alpha = 0.f;
      break;
    case kOutBurn:
      s.glow += c * (A.a != 0.f ? A.a : 2.f);
      s.flash = std::max(s.flash, c * c * (A.b != 0.f ? A.b : 0.8f));
      s.alpha *= 1.f - smoothstep(0.55f, 1.f, x);
      break;
    case kOutDissolve: s.dissolve = std::max(s.dissolve, x); s.block = A.b; break;
    case kOutBlur: {
      const bool vert = A.dir == kUp || A.dir == kDown;
      const bool hor = A.dir == kLeft || A.dir == kRight;
      s.blurX += (vert ? 0.f : 1.f) * c * A.a;
      s.blurY += (hor ? 0.f : 1.f) * c * (A.b != 0.f ? A.b : A.a);
      s.alpha *= 1.f - c;
      break;
    }
    case kOutFlash: s.flash = std::max(s.flash, gone ? 0.f : 1.f); if (gone) s.alpha = 0.f; break;
    case kOutMelt:
      s.sy *= 1.f + (A.a != 0.f ? A.a : 1.5f) * c;
      s.dy += c * (A.a != 0.f ? A.a : 1.5f) * 0.35f;
      s.blurY += c * 0.1f;
      s.alpha *= 1.f - smoothstep(0.4f, 1.f, x);
      break;
    case kOutScatter: {
      const float ang = rnd(gi, 22) * 2.f * kPi;
      const float d = (A.a != 0.f ? A.a : 3.f) * c * (0.5f + rnd(gi, 23));
      s.dx += std::cos(ang) * d; s.dy += std::sin(ang) * d;
      s.rot += sgn(gi, 24) * c * (A.b != 0.f ? A.b : 180.f) * kPi / 180.f;
      s.alpha *= 1.f - smoothstep(0.5f, 1.f, x);
      break;
    }
    case kOutStrobe:
      s.alpha *= gone ? 0.f : strobePattern(gi, gi.fout);
      break;
    case kOutRise:
      s.dy -= c * (A.a != 0.f ? A.a : 2.f);
      s.dx += sgn(gi, 25) * c * 0.3f;
      s.alpha *= 1.f - smoothstep(0.3f, 1.f, x);
      break;
    case kOutChroma: s.chroma += c * (A.a != 0.f ? A.a : 0.2f); s.alpha *= 1.f - c; break;
    case kOutCut: s.alpha = 0.f; break;
    case kOutWipe: s.wipe = std::min(s.wipe, 1.f - c); s.wipeDir = A.dir; if (gone) s.alpha = 0.f; break;
    case kOutSpinY: s.sx *= std::cos(c * (A.a != 0.f ? A.a : 90.f) * kPi / 180.f); if (gone) s.alpha = 0.f; break;
    case kOutStretch: s.sx *= lerp(1.f, A.a, c); s.sy *= lerp(1.f, A.b, c); if (gone) s.alpha = 0.f; break;
    case kOutWhiteout: s.frameWhite = std::max(s.frameWhite, c * (A.a != 0.f ? A.a : 1.f)); break;
    case kOutBlackout: s.frameBlack = std::max(s.frameBlack, c); break;
    case kOutStepOff: s.alpha = 0.f; break;
  }
}

void applyIdle(const Anim& A, double t, const GInfo& gi, GState& s, Globals& G) {
  if (A.kind == kIdleNone) return;
  const float f = std::max(1.f, A.frames);
  const float ph = 2.f * kPi * static_cast<float>(t) / f;
  const uint32_t id = A.c > 0.5f ? 0u : gi.id;   // c>0.5 : bruit commun à tout le titre
  float vx, vy;
  switch (A.kind) {
    case kIdleJitter: {
      const int k = static_cast<int>(std::floor(t / f));
      s.dx += (hash01(id, k, 1) * 2.f - 1.f) * A.a;
      s.dy += (hash01(id, k, 2) * 2.f - 1.f) * A.a;
      s.rot += (hash01(id, k, 3) * 2.f - 1.f) * A.b * kPi / 180.f;
      const float sc = 1.f + (hash01(id, k, 4) * 2.f - 1.f) * A.a * 0.5f;
      s.sx *= sc; s.sy *= sc;
      break;
    }
    case kIdleBop: s.dy += A.a * std::sin(ph + gi.i * A.b + rnd(gi, 31) * 2.f * kPi * 0.3f); break;
    case kIdleWave: s.dy += A.a * std::sin(ph + gi.i * (A.b != 0.f ? A.b : 0.5f)); break;
    case kIdlePulse: { const float k = 1.f + A.a * std::sin(ph + gi.i * A.b); s.sx *= k; s.sy *= k; break; }
    case kIdleFlicker: {
      const float n = hash01(id, static_cast<int>(std::floor(t / std::max(1.f, f))), 5);
      s.alpha *= 1.f - A.a * n;
      break;
    }
    case kIdleWobble: s.rot += A.a * kPi / 180.f * std::sin(ph + gi.i * A.b + rnd(gi, 32) * 2.f); break;
    case kIdleDrift:
      dirVec(A.dir, gi, vx, vy);
      s.dx += vx * A.a * static_cast<float>(t) / f;
      s.dy += vy * A.a * static_cast<float>(t) / f;
      break;
    case kIdleBreathe:
      s.dx += gi.u * A.a * (A.b > 0.5f ? static_cast<float>(t) / f : std::sin(ph));
      break;
    case kIdleSlowZoom: { const float k = 1.f + A.a * static_cast<float>(t) / f; s.sx *= k; s.sy *= k; break; }
    case kIdleGlowPulse: s.glow += A.a * (0.5f + 0.5f * std::sin(ph)); break;
    case kIdleSweep:
      G.sweep = std::fmod(static_cast<float>(t) / f, 1.f) * 1.6f - 0.3f;
      G.sweepW = A.b > 0.f ? A.b : 0.12f;
      G.sweepAmt = A.a;
      break;
    case kIdleScanline:
      G.scan = std::fmod(static_cast<float>(t) / f, 1.f);
      G.scanAmt = A.a;
      break;
    case kIdleChromaJit: s.chroma += A.a * hash01(id, static_cast<int>(std::floor(t / std::max(1.f, f))), 6); break;
    case kIdleBlurPulse: {
      const float v = A.a * (0.5f + 0.5f * std::sin(ph));
      if (A.dir == kUp || A.dir == kDown) s.blurY += v; else s.blurX += v;
      break;
    }
    case kIdleFloat:
      s.dy += A.a * std::sin(ph + gi.i * A.b);
      s.dx += A.a * 0.5f * std::cos(ph * 0.77f + gi.i * A.b);
      break;
    case kIdleInterlace:
      if (static_cast<int>(std::floor(t)) % static_cast<int>(f) == 0) s.dy += A.a;
      break;
    case kIdleSteps: {
      static const float seq[5] = {0.2f, 0.f, 0.6f, 0.4f, 1.f};
      s.alpha *= seq[(static_cast<int>(std::floor(t / f)) + (A.c > 0.5f ? 0 : gi.i)) % 5];
      break;
    }
  }
}

void computeState(const Params& P, const GInfo& gi, const Times& T, double shift, GState& s, Globals* G) {
  s = GState();
  const double tin = T.tin - shift, tout = T.tout - shift, tidle = T.tidle - shift;
  GInfo g = gi;
  g.fin = static_cast<int>(std::floor(tin));
  g.fout = static_cast<int>(std::floor(tout));
  g.fidle = static_cast<int>(std::floor(tidle));
  s.chroma += P.chromaBase;
  applyIn(P.in, static_cast<float>(tin - gi.rank * P.delay), g, s);
  Globals dummy;
  applyIdle(P.idle, tidle, g, s, G ? *G : dummy);
  const int rankOut = P.outReverse ? (gi.nRank - 1 - gi.rank) : gi.rank;
  applyOut(P.out, static_cast<float>(tout - rankOut * P.outDelay), g, s);
}

void computeRanks(int order, int n, int seed, std::vector<int>& rank) {
  rank.assign(n, 0);
  const float mid = (n - 1) * 0.5f;
  switch (order) {
    case kLTR: for (int i = 0; i < n; ++i) rank[i] = i; break;
    case kRTL: for (int i = 0; i < n; ++i) rank[i] = n - 1 - i; break;
    case kCenter: for (int i = 0; i < n; ++i) rank[i] = static_cast<int>(std::lround(std::fabs(i - mid))); break;
    case kEdges: for (int i = 0; i < n; ++i) rank[i] = static_cast<int>(std::lround(mid - std::fabs(i - mid))); break;
    case kRandomOrder: {
      std::vector<std::pair<float, int>> v;
      for (int i = 0; i < n; ++i) v.push_back({hash01(i * 131 + 7, seed, 55), i});
      std::sort(v.begin(), v.end());
      for (int r = 0; r < n; ++r) rank[v[r].second] = r;
      break;
    }
    case kOddEven: for (int i = 0; i < n; ++i) rank[i] = (i % 2 == 0) ? i / 2 : n / 2 + i / 2; break;
    default: break;  // kAll : tout à 0
  }
}

void boxBlur(std::vector<float>& a, int w, int h, int rx, int ry) {
  if (rx <= 0 && ry <= 0) return;
  std::vector<float> tmp(a.size());
  auto pass = [&](const std::vector<float>& in, std::vector<float>& out, int R, bool horiz) {
    if (R <= 0) { out = in; return; }
    const int len = horiz ? w : h, lines = horiz ? h : w;
    const float inv = 1.f / (2 * R + 1);
    for (int l = 0; l < lines; ++l) {
      auto idx = [&](int i) { return horiz ? (size_t)l * w + i : (size_t)i * w + l; };
      float sum = 0.f;
      for (int i = -R; i <= R; ++i)
        if (i >= 0 && i < len) sum += in[idx(i)];
      for (int i = 0; i < len; ++i) {
        out[idx(i)] = sum * inv;
        const int add = i + R + 1, rem = i - R;
        if (add < len) sum += in[idx(add)];
        if (rem >= 0) sum -= in[idx(rem)];
      }
    }
  };
  pass(a, tmp, rx, true);
  pass(tmp, a, ry, false);
}

struct Xf {
  double m00, m01, m10, m11;     // M = R * S
  double i00, i01, i10, i11;     // M^-1
  double offx, offy;
  bool ok;
};

Xf makeXf(const GState& s, double em) {
  Xf x{};
  const double c = std::cos(s.rot), n = std::sin(s.rot);
  x.ok = std::fabs(s.sx) > 1e-3 && std::fabs(s.sy) > 1e-3;
  x.m00 = c * s.sx; x.m01 = -n * s.sy;
  x.m10 = n * s.sx; x.m11 = c * s.sy;
  if (x.ok) {
    x.i00 = c / s.sx;  x.i01 = n / s.sx;
    x.i10 = -n / s.sy; x.i11 = c / s.sy;
  }
  x.offx = s.dx * em; x.offy = s.dy * em;
  return x;
}


// ---- Matière des lettres -------------------------------------------------------
float texValue(int kind, float x, float y, float scale, const hc::Simplex& sx, int seed) {
  const float s = std::max(0.2f, scale);
  const uint32_t ux = static_cast<uint32_t>(std::max(0.f, x)), uy = static_cast<uint32_t>(std::max(0.f, y));
  const float h = hash01(ux, uy, static_cast<uint32_t>(seed)) - 0.5f;
  switch (kind) {
    case kTGrain: return 1.f + h * 0.55f;
    case kTPaper: return 1.f + 0.14f * sx.fbm(x * 0.03f / s, y * 0.03f / s) + 0.14f * h;
    case kTConcrete: {
      const float n = sx.fbm(x * 0.02f / s, y * 0.02f / s);
      const float speck = hash01(ux / 2, uy / 2, 91u) > 0.985f ? -0.45f : 0.f;
      return 1.f + 0.2f * n + speck + 0.12f * h;
    }
    case kTBrushed: return 1.f + 0.18f * sx.noise(x * 0.004f / s, y * 0.9f / s) + 0.06f * h;
    case kTSand: return 1.f + 0.5f * h + 0.07f * sx.noise(x * 0.05f / s, y * 0.05f / s);
    case kTRust: {
      const float n = sx.fbm(x * 0.025f / s, y * 0.025f / s);
      return 1.f - 0.55f * smoothstep(0.05f, 0.55f, n) + 0.1f * h;
    }
    case kTImpasto: {
      const float n1 = sx.fbm(x * 0.05f / s, y * 0.05f / s), n2 = sx.fbm((x + 1.8f) * 0.05f / s, (y + 1.8f) * 0.05f / s);
      return 1.f + 1.4f * (n2 - n1);
    }
    case kTScratch: {
      const float l = std::fabs(sx.noise(x * 0.6f / s, y * 0.004f / s));
      return 1.f - (l < 0.05f ? 0.45f : 0.f) + 0.08f * h;
    }
    case kTHalftone: {
      const float cell = 7.f * s;
      float fx = std::fmod(x, cell) / cell - 0.5f, fy = std::fmod(y, cell) / cell - 0.5f;
      return std::sqrt(fx * fx + fy * fy) < 0.3f ? 0.55f : 1.15f;
    }
    case kTWood: return 1.f + 0.22f * sx.noise(x * 0.01f / s, y * 0.15f / s) + 0.05f * h;
    default: return 1.f;
  }
}

// ---- Atmosphère (particules, brume, flare, rayures) ------------------------------
void blobOver(Canvas& C, double x, double y, double rad, float soft, float r, float g, float b, float a) {
  const int x0 = std::max(0, static_cast<int>(std::floor(x - rad - 1))), x1 = std::min(C.w, static_cast<int>(std::ceil(x + rad + 1)));
  const int y0 = std::max(0, static_cast<int>(std::floor(y - rad - 1))), y1 = std::min(C.h, static_cast<int>(std::ceil(y + rad + 1)));
  const double inv = 1.0 / std::max(0.5, rad);
  for (int yy = y0; yy < y1; ++yy)
    for (int xx = x0; xx < x1; ++xx) {
      const double d = std::hypot(xx + 0.5 - x, yy + 0.5 - y) * inv;
      if (d >= 1.0) continue;
      const float k = static_cast<float>(1.0 - smoothstep(1.f - soft, 1.f, static_cast<float>(d)));
      const float al = a * k;
      if (al <= 0.f) continue;
      float* p = &C.px[((size_t)yy * C.w + xx) * 4];
      p[0] = r * al + p[0] * (1.f - al); p[1] = g * al + p[1] * (1.f - al);
      p[2] = b * al + p[2] * (1.f - al); p[3] = al + p[3] * (1.f - al);
    }
}

void lineOver(Canvas& C, double x0, double y0, double x1, double y1, double w, float r, float g, float b, float a) {
  const double len = std::hypot(x1 - x0, y1 - y0);
  const int steps = std::max(1, static_cast<int>(len / std::max(0.7, w * 0.6)));
  for (int i = 0; i <= steps; ++i) {
    const double t = static_cast<double>(i) / steps;
    blobOver(C, x0 + (x1 - x0) * t, y0 + (y1 - y0) * t, w, 0.9f, r, g, b, a);
  }
}

void drawFx(const Params& P, const Times& T, Canvas& C, double /*cx*/, double cy, double ps, int frame,
            const hc::Simplex& sx) {
  const int W = C.w, H = C.h;
  const double t = T.tidle;
  const uint32_t seed = static_cast<uint32_t>(T.seed);

  // brume (grille grossière + interpolation)
  if (P.fogAmt > 0.002f) {
    const int cs = 8, gw = W / cs + 2, gh = H / cs + 2;
    std::vector<float> g((size_t)gw * gh);
    const float sc = 0.0035f / static_cast<float>(std::max(0.25, ps));
    for (int j = 0; j < gh; ++j)
      for (int i = 0; i < gw; ++i) {
        const float n = 0.5f + 0.5f * sx.fbm(i * cs * sc + static_cast<float>(t) * 0.006f, j * cs * sc * 1.4f + 5.f);
        g[(size_t)j * gw + i] = clamp01(n * 1.5f - 0.25f);
      }
    for (int y = 0; y < H; ++y) {
      const float fy = static_cast<float>(y) / cs;
      const int j0 = static_cast<int>(fy);
      const float ty = fy - j0;
      for (int x = 0; x < W; ++x) {
        const float fx = static_cast<float>(x) / cs;
        const int i0 = static_cast<int>(fx);
        const float tx = fx - i0;
        const float a0 = g[(size_t)j0 * gw + i0] * (1 - tx) + g[(size_t)j0 * gw + i0 + 1] * tx;
        const float a1 = g[(size_t)(j0 + 1) * gw + i0] * (1 - tx) + g[(size_t)(j0 + 1) * gw + i0 + 1] * tx;
        const float al = clamp01((a0 * (1 - ty) + a1 * ty) * P.fogAmt);
        if (al <= 0.f) continue;
        float* p = &C.px[((size_t)y * W + x) * 4];
        p[0] = P.fogColor.r * al + p[0] * (1.f - al); p[1] = P.fogColor.g * al + p[1] * (1.f - al);
        p[2] = P.fogColor.b * al + p[2] * (1.f - al); p[3] = al + p[3] * (1.f - al);
      }
    }
  }

  // particules
  if (P.particle != kPNone && P.particle != kPStatic && P.particleAmt > 0.001f) {
    const int N = static_cast<int>(std::lround(std::clamp(P.particleAmt, 0.f, 3.f) * 380.0 * (double)W * H / (1920.0 * 1080.0)));
    const double sp = std::max(0.05f, P.particleSpeed) * ps, sz = std::max(0.2f, P.particleSize) * ps;
    const Rgb pc = P.particleColor;
    for (int i = 0; i < N; ++i) {
      const float h1 = hash01(i, seed, 1), h2 = hash01(i, seed, 2), h3 = hash01(i, seed, 3), h4 = hash01(i, seed, 4);
      double x = 0, y = 0;
      auto wrap = [](double v, double m) { v = std::fmod(v, m); return v < 0 ? v + m : v; };
      switch (P.particle) {
        case kPSnow: {
          const double r = (1.5 + 3.5 * h3) * sz;
          x = wrap(h1 * W + (-1.2 - h3) * t * sp * 1.0 + std::sin(t * 0.05 + h4 * 6.28) * 10 * ps, W);
          y = wrap(h2 * H + (2.0 + 2.8 * h3) * t * sp, H);
          blobOver(C, x, y, r, 0.9f, pc.r, pc.g, pc.b, 0.55f + 0.4f * h4);
          break;
        }
        case kPRain: {
          x = wrap(h1 * W - 4.0 * t * sp, W);
          y = wrap(h2 * H + (26.0 + 10 * h3) * t * sp, H);
          lineOver(C, x, y, x + 4.0 * sz, y - 34.0 * sz * (0.6 + 0.6 * h3), 0.9 * sz, pc.r, pc.g, pc.b, 0.28f + 0.2f * h4);
          break;
        }
        case kPDust: {
          x = wrap(h1 * W + (5.0 + 5 * h3) * t * sp, W);
          y = wrap(h2 * H + std::sin(t * 0.04 + h4 * 6.28) * 14 * ps + 0.6 * t * sp * h3, H);
          blobOver(C, x, y, (0.9 + 1.4 * h3) * sz, 0.9f, pc.r, pc.g, pc.b, 0.25f + 0.35f * h4);
          break;
        }
        case kPSparkle: {
          const int slot = static_cast<int>(std::floor(t * 0.04 * sp + h4 * 10));
          x = hash01(i, slot, 11) * W; y = hash01(i, slot, 12) * H;
          const double ph = std::fmod(t * 0.04 * sp + h4 * 10, 1.0);
          const float tw = static_cast<float>(std::sin(ph * 3.14159));
          const double r = (2.0 + 5.0 * h3) * sz;
          blobOver(C, x, y, r * 0.5, 0.9f, 1.f, 1.f, 1.f, tw);
          lineOver(C, x - r * 2, y, x + r * 2, y, 0.6 * sz, pc.r, pc.g, pc.b, 0.8f * tw);
          lineOver(C, x, y - r * 2, x, y + r * 2, 0.6 * sz, pc.r, pc.g, pc.b, 0.8f * tw);
          break;
        }
        case kPAsh: {
          x = wrap(h1 * W + std::sin(t * 0.03 + h4 * 6.28) * 22 * ps - 0.5 * t * sp, W);
          y = wrap(h2 * H + (1.0 + 1.6 * h3) * t * sp, H);
          blobOver(C, x, y, (1.5 + 3.0 * h3) * sz, 0.7f, pc.r, pc.g, pc.b, 0.55f);
          break;
        }
        case kPBokeh: {
          x = wrap(h1 * W + 0.4 * t * sp * (h3 - 0.5), W);
          y = wrap(h2 * H - 0.35 * t * sp * (0.4 + h3), H);
          blobOver(C, x, y, (22.0 + 55.0 * h3) * sz, 0.5f, pc.r, pc.g, pc.b, 0.07f + 0.1f * h4);
          break;
        }
        case kPBubbles: {
          x = wrap(h1 * W + std::sin(t * 0.04 + h4 * 6.28) * 12 * ps, W);
          y = wrap(h2 * H - (1.2 + 2.0 * h3) * t * sp, H);
          const double r = (3.0 + 9.0 * h3) * sz;
          blobOver(C, x, y, r, 0.35f, pc.r, pc.g, pc.b, 0.35f);
          break;
        }
        case kPEmbers: {
          x = wrap(h1 * W + std::sin(t * 0.07 + h4 * 6.28) * 16 * ps, W);
          y = wrap(h2 * H - (2.0 + 3.0 * h3) * t * sp, H);
          const float fl = 0.5f + 0.5f * static_cast<float>(std::sin(t * 0.4 + h4 * 20));
          blobOver(C, x, y, (1.4 + 2.6 * h3) * sz, 0.8f, pc.r, pc.g, pc.b, 0.4f + 0.55f * fl);
          break;
        }
        case kPSpores: {
          x = wrap(h1 * W + std::sin(t * 0.02 + h4 * 6.28) * 30 * ps, W);
          y = wrap(h2 * H - 0.5 * t * sp * (0.3 + h3), H);
          blobOver(C, x, y, (1.5 + 3.0 * h3) * sz, 0.9f, pc.r, pc.g, pc.b, 0.3f + 0.3f * h4);
          break;
        }
        case kPConfetti: {
          static const float pal[6][3] = {{1, .2f, .5f}, {1, .85f, .1f}, {.2f, .8f, 1}, {.5f, 1, .3f}, {.8f, .4f, 1}, {1, .5f, .1f}};
          const int k = static_cast<int>(h4 * 6) % 6;
          x = wrap(h1 * W + std::sin(t * 0.06 + h3 * 9) * 18 * ps, W);
          y = wrap(h2 * H + (3.0 + 3.0 * h3) * t * sp, H);
          const double ang = t * 0.15 * (h3 - 0.5) * 6 + h1 * 6.28;
          lineOver(C, x - std::cos(ang) * 4 * sz, y - std::sin(ang) * 4 * sz, x + std::cos(ang) * 4 * sz, y + std::sin(ang) * 4 * sz, 2.2 * sz, pal[k][0], pal[k][1], pal[k][2], 0.9f);
          break;
        }
        default: break;
      }
    }
  }
  if (P.particle == kPStatic && P.particleAmt > 0.001f) {
    for (int y = 0; y < H; ++y)
      for (int x = 0; x < W; ++x) {
        const float n = hash01(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(frame * 13 + seed));
        const float al = clamp01((n - 0.45f) * P.particleAmt * 0.5f);
        if (al <= 0.f) continue;
        float* p = &C.px[((size_t)y * W + x) * 4];
        const float v = 0.4f + 0.6f * n;
        p[0] = v * al + p[0] * (1.f - al); p[1] = v * al + p[1] * (1.f - al);
        p[2] = v * al + p[2] * (1.f - al); p[3] = al + p[3] * (1.f - al);
      }
  }

  // flare anamorphique
  if (P.flareAmt > 0.002f) {
    const double fx = W * (0.15 + 0.7 * std::fmod(t / 160.0, 1.0));
    const double sy = 0.012 * H + 1.0;
    const int y0 = std::max(0, static_cast<int>(cy - 4 * sy)), y1 = std::min(H, static_cast<int>(cy + 4 * sy));
    for (int y = y0; y < y1; ++y) {
      const double dy = (y + 0.5 - cy) / sy;
      const double ky = std::exp(-dy * dy);
      for (int x = 0; x < W; ++x) {
        const double dx = (x + 0.5 - fx) / (0.35 * W);
        const double core = std::exp(-(dx * dx) - dy * dy * 0.05);
        const float al = clamp01(static_cast<float>((ky * std::exp(-dx * dx) * 0.8 + core * 0.15) * P.flareAmt));
        if (al <= 0.f) continue;
        float* p = &C.px[((size_t)y * W + x) * 4];
        p[0] = P.flareColor.r * al + p[0] * (1.f - al); p[1] = P.flareColor.g * al + p[1] * (1.f - al);
        p[2] = P.flareColor.b * al + p[2] * (1.f - al); p[3] = al + p[3] * (1.f - al);
      }
    }
  }

  // rayures + poussières de pellicule
  if (P.scratches > 0.002f) {
    const int fr2 = static_cast<int>(std::floor(t));
    const int nL = static_cast<int>(std::lround(P.scratches * 6.f));
    for (int k = 0; k < nL; ++k) {
      if (hash01(k, fr2, seed + 5) < 0.45f) continue;
      const double x = hash01(k, fr2, seed + 6) * W, len = (0.2 + 0.8 * hash01(k, fr2, seed + 7)) * H;
      const double y = hash01(k, fr2, seed + 8) * (H - len);
      lineOver(C, x, y, x + (hash01(k, fr2, 9) - 0.5) * 6 * ps, y + len, 0.55 * ps, 1.f, 1.f, 1.f, 0.30f);
    }
    const int nD = static_cast<int>(std::lround(P.scratches * 45.f));
    for (int k = 0; k < nD; ++k) {
      const bool dark = hash01(k, fr2, 21) < 0.5f;
      blobOver(C, hash01(k, fr2, 22) * W, hash01(k, fr2, 23) * H, (0.8 + 2.2 * hash01(k, fr2, 24)) * ps, 0.9f,
               dark ? 0.f : 1.f, dark ? 0.f : 1.f, dark ? 0.f : 1.f, 0.35f);
    }
  }
}

}  // namespace

double entrySpan(const Params& P, int n) {
  if (P.in.kind == kInNone) return 0.0;
  int ranks = n;
  if (P.order == kAll) ranks = 1;
  else if (P.order == kCenter || P.order == kEdges) ranks = (n + 1) / 2;
  return std::max(0, ranks - 1) * static_cast<double>(P.delay) + std::max(0.5f, P.in.frames);
}

double exitSpan(const Params& P, int n) {
  if (P.out.kind == kOutNone) return 0.0;
  int ranks = n;
  if (P.order == kAll) ranks = 1;
  else if (P.order == kCenter || P.order == kEdges) ranks = (n + 1) / 2;
  return std::max(0, ranks - 1) * static_cast<double>(P.outDelay) + std::max(0.5f, P.out.frames);
}

// ---- Rendu ----------------------------------------------------------------------
FrameFx renderTitle(const Layout& L, const Params& P, const Times& T, double cx, double cy, Canvas& C, double pixelScale) {
  FrameFx frameFx;
  const int n = static_cast<int>(L.g.size());
  if (n == 0 || C.w <= 0 || C.h <= 0) return frameFx;
  const double em = L.em;
  const hc::Simplex& sx = hc::simplex();

  std::vector<int> rank;
  computeRanks(P.order, n, T.seed, rank);
  const int nRank = *std::max_element(rank.begin(), rank.end()) + 1;
  const float halfW = std::max(1.f, std::max(std::fabs(L.minX), std::fabs(L.maxX)));
  const float titleW = std::max(1.f, L.maxX - L.minX), titleH = std::max(1.f, L.maxY - L.minY);

  // point d'ancrage (coordonnées titre)
  auto anchorOf = [&](const Glyph& g, double& ax, double& ay) {
    switch (P.anchor) {
      case kTextCenter: ax = 0; ay = 0; break;
      case kBottomLeft: ax = L.minX; ay = L.maxY; break;
      case kTopCenter: ax = 0; ay = L.minY; break;
      case kBottomCenter: ax = 0; ay = L.maxY; break;
      default: ax = g.px; ay = g.py; break;
    }
  };

  // états finaux + globaux
  std::vector<GState> final(n);
  Globals G;
  double meanAlpha = 0.0;
  for (int i = 0; i < n; ++i) {
    GInfo gi{i, n, rank[i], nRank, L.g[i].px / halfW, static_cast<uint32_t>(i * 2654435761u + 17u), T.seed, 0, 0, 0};
    gi.ci = L.g[i].ci;
    computeState(P, gi, T, 0.0, final[i], i == 0 ? &G : nullptr);
    frameFx.white = std::max(frameFx.white, clamp01(final[i].frameWhite));
    frameFx.black = std::max(frameFx.black, clamp01(final[i].frameBlack));
    meanAlpha += clamp01(final[i].alpha);
  }
  meanAlpha /= n;

  // plaque derrière le titre
  if (P.plateAlpha > 0.001f && meanAlpha > 0.001) {
    const double pad = P.platePad * em;
    const int x0 = std::max(0, static_cast<int>(std::floor(cx + L.minX - pad))),
              x1 = std::min(C.w, static_cast<int>(std::ceil(cx + L.maxX + pad)));
    const int y0 = std::max(0, static_cast<int>(std::floor(cy + L.minY - pad))),
              y1 = std::min(C.h, static_cast<int>(std::ceil(cy + L.maxY + pad)));
    const float a = clamp01(P.plateAlpha * static_cast<float>(meanAlpha));
    for (int y = y0; y < y1; ++y)
      for (int x = x0; x < x1; ++x) {
        float* d = &C.px[((size_t)y * C.w + x) * 4];
        d[0] = P.plate.r * a + d[0] * (1.f - a);
        d[1] = P.plate.g * a + d[1] * (1.f - a);
        d[2] = P.plate.b * a + d[2] * (1.f - a);
        d[3] = a + d[3] * (1.f - a);
      }
  }

  const bool hasStroke = P.strokeW > 0.f;
  const double shDx = P.shadowDx * em, shDy = P.shadowDy * em;
  const bool hasShadow = P.shadowAlpha > 0.001f;
  const int fr = static_cast<int>(std::floor(T.tidle));
  const float sdOff = static_cast<float>(T.seed) * 3.17f;

  std::vector<float> covF, covR, covB, covS, covH;
  for (int gi_ = 0; gi_ < n; ++gi_) {
    const Glyph& g = L.g[gi_];
    if (final[gi_].alpha <= 0.0005f && T.shutter <= 0.0) continue;

    // sous-états pour le motion blur
    const double shutter = std::max(0.0, T.shutter * P.motionBlur);
    GInfo gi{gi_, n, rank[gi_], nRank, g.px / halfW, static_cast<uint32_t>(gi_ * 2654435761u + 17u), T.seed, 0, 0, 0};
    gi.ci = g.ci;
    GState s0;
    computeState(P, gi, T, 0.0, s0, nullptr);
    int M = 1;
    GState s1 = s0;
    if (shutter > 1e-6) {
      computeState(P, gi, T, shutter, s1, nullptr);
      const double R = 0.5 * std::max(g.fill.w, g.fill.h) + 1.0;
      const double disp = std::hypot((s0.dx - s1.dx) * em, (s0.dy - s1.dy) * em) +
                          std::fabs(s0.rot - s1.rot) * R + std::fabs(s0.sx - s1.sx) * R + std::fabs(s0.sy - s1.sy) * R +
                          std::fabs(s0.topDx - s1.topDx) * em + std::fabs(s0.botDx - s1.botDx) * em;
      M = std::clamp(static_cast<int>(std::ceil(disp / 1.5)), 1, 24);
    }
    std::vector<GState> sub(M);
    for (int m = 0; m < M; ++m) {
      if (m == 0) sub[m] = s0;
      else computeState(P, gi, T, shutter * m / M, sub[m], nullptr);
    }
    bool anyVisible = false, anyChroma = false;
    for (const auto& st : sub) {
      if (st.alpha > 0.0005f && st.sx != 0.f && st.sy != 0.f) anyVisible = true;
      if (st.chroma > 1e-4f) anyChroma = true;
    }
    if (!anyVisible) continue;

    const GlyphImg& baseF = g.fill;
    const GlyphImg& baseS = hasStroke ? g.stroke : g.fill;
    double ax, ay;
    anchorOf(g, ax, ay);
    const double gx = g.px, gy = g.py;

    // boîte écran
    const double padPx = 3.0 + (hasShadow ? std::fabs(shDx) + std::fabs(shDy) + P.shadowBlur * em * 2.0 : 0.0) +
                         P.glowR * em * 2.5 + (std::max(s0.blurX, s1.blurX) + std::max(s0.blurY, s1.blurY)) * em * 1.5 +
                         (std::max(s0.chroma, s1.chroma) + 0.1) * em * (anyChroma ? 1.0 : 0.0) +
                         (std::fabs(s0.topDx) + std::fabs(s0.botDx) + std::fabs(s0.topDy) + std::fabs(s0.botDy)) * em;
    double bx0 = 1e18, by0 = 1e18, bx1 = -1e18, by1 = -1e18;
    for (const auto& st : sub) {
      const Xf xf = makeXf(st, em);
      if (!xf.ok) continue;
      const double lx[2] = {baseS.ox, baseS.ox + baseS.w}, ly[2] = {baseS.oy, baseS.oy + baseS.h};
      for (int a = 0; a < 2; ++a)
        for (int b = 0; b < 2; ++b) {
          const double px_ = (gx - ax) + lx[a], py_ = (gy - ay) + ly[b];
          const double wx = cx + ax + xf.m00 * px_ + xf.m01 * py_ + xf.offx;
          const double wy = cy + ay + xf.m10 * px_ + xf.m11 * py_ + xf.offy;
          bx0 = std::min(bx0, wx); bx1 = std::max(bx1, wx);
          by0 = std::min(by0, wy); by1 = std::max(by1, wy);
        }
    }
    if (bx0 > bx1) continue;
    const int X0 = std::max(0, static_cast<int>(std::floor(bx0 - padPx))), X1 = std::min(C.w, static_cast<int>(std::ceil(bx1 + padPx)));
    const int Y0 = std::max(0, static_cast<int>(std::floor(by0 - padPx))), Y1 = std::min(C.h, static_cast<int>(std::ceil(by1 + padPx)));
    const int bw = X1 - X0, bh = Y1 - Y0;
    if (bw <= 0 || bh <= 0) continue;

    const size_t N = (size_t)bw * bh;
    covF.assign(N, 0.f); covS.assign(N, 0.f);
    if (anyChroma) { covR.assign(N, 0.f); covB.assign(N, 0.f); } else { covR.clear(); covB.clear(); }
    if (hasShadow) covH.assign(N, 0.f); else covH.clear();

    const float invM = 1.f / M;
    for (int m = 0; m < M; ++m) {
      const GState& st = sub[m];
      const Xf xf = makeXf(st, em);
      if (!xf.ok || st.alpha <= 0.0005f) continue;
      const float alpha = clamp01(st.alpha) * invM;

      // image affichée (Scramble : glyphe de remplacement)
      const GlyphImg* imF = &baseF;
      const GlyphImg* imS = &baseS;
      if (st.scramble && !L.altFill.empty()) {
        const size_t k = static_cast<size_t>(hash01(gi.id, static_cast<uint32_t>(std::floor(T.tin)), 9) * L.altFill.size()) % L.altFill.size();
        imF = &L.altFill[k];
        imS = hasStroke && k < L.altStroke.size() ? &L.altStroke[k] : &L.altFill[k];
      }

      for (int y = 0; y < bh; ++y) {
        for (int x = 0; x < bw; ++x) {
          const double wx = X0 + x + 0.5 - cx - ax - xf.offx, wy = Y0 + y + 0.5 - cy - ay - xf.offy;
          double lx = xf.i00 * wx + xf.i01 * wy - (gx - ax);
          double ly = xf.i10 * wx + xf.i11 * wy - (gy - ay);
          // découpe haut / bas (Split, Tear)
          bool inTop = ly < 0.0;
          double sxl = lx, syl = ly;
          if (st.topDx != 0.f || st.botDx != 0.f || st.topDy != 0.f || st.botDy != 0.f) {
            if (inTop) { sxl -= st.topDx * em; syl -= st.topDy * em; if (syl >= 0.0) continue; }
            else { sxl -= st.botDx * em; syl -= st.botDy * em; if (syl < 0.0) continue; }
          }
          float vis = alpha;
          if (st.wipe < 0.999f) {
            const float u = (static_cast<float>(sxl) - baseF.ox) / std::max(1, baseF.w);
            const float v = (static_cast<float>(syl) - baseF.oy) / std::max(1, baseF.h);
            float q;
            switch (st.wipeDir) {
              case kRight: q = 1.f - u; break;
              case kUp: q = v; break;
              case kDown: q = 1.f - v; break;
              default: q = u;
            }
            vis *= smoothstep(q - 0.04f, q + 0.04f, st.wipe * 1.08f - 0.04f + 0.0f) * (st.wipe > 0.f ? 1.f : 0.f);
          }
          if (st.dissolve > 0.001f) {
            float qx = static_cast<float>(sxl), qy = static_cast<float>(syl);
            if (st.block > 0.f) {
              const float bs = std::max(2.f, st.block * static_cast<float>(em));
              qx = std::floor(qx / bs) * bs; qy = std::floor(qy / bs) * bs;
            }
            const float f = 1.f / std::max(3.f, static_cast<float>(em) * 0.07f);
            float nn = 0.5f + 0.5f * std::clamp(sx.fbm(qx * f + sdOff, qy * f + gi_ * 3.7f), -1.f, 1.f);
            vis *= smoothstep(st.dissolve - 0.05f, st.dissolve + 0.05f, nn);
          }
          if (vis <= 0.f) continue;
          const size_t idx = (size_t)y * bw + x;
          covF[idx] += vis * imF->sample(static_cast<float>(sxl), static_cast<float>(syl));
          covS[idx] += vis * imS->sample(static_cast<float>(sxl), static_cast<float>(syl));
          if (anyChroma) {
            const float ch = st.chroma * static_cast<float>(em);
            covR[idx] += vis * imF->sample(static_cast<float>(sxl) - ch, static_cast<float>(syl));
            covB[idx] += vis * imF->sample(static_cast<float>(sxl) + ch, static_cast<float>(syl));
          }
          if (hasShadow) {
            // ombre : même glyphe décalé en espace écran
            const double wx2 = wx - shDx, wy2 = wy - shDy;
            const double lx2 = xf.i00 * wx2 + xf.i01 * wy2 - (gx - ax);
            const double ly2 = xf.i10 * wx2 + xf.i11 * wy2 - (gy - ay);
            covH[idx] += vis * imS->sample(static_cast<float>(lx2), static_cast<float>(ly2));
          }
        }
      }
    }

    // flou (mise au point / directionnel) sur tout le glyphe
    const GState& sf = s0;
    const int rbx = static_cast<int>(std::lround(sf.blurX * em)), rby = static_cast<int>(std::lround(sf.blurY * em));
    if (rbx > 0 || rby > 0) {
      boxBlur(covF, bw, bh, rbx, rby);
      boxBlur(covS, bw, bh, rbx, rby);
      if (anyChroma) { boxBlur(covR, bw, bh, rbx, rby); boxBlur(covB, bw, bh, rbx, rby); }
    }
    if (hasShadow && P.shadowBlur > 0.f) {
      const int r = static_cast<int>(std::lround(P.shadowBlur * em));
      boxBlur(covH, bw, bh, r, r);
      boxBlur(covH, bw, bh, r, r);
    }
    // lueur
    std::vector<float> covG;
    const float glowGain = P.glowAmt + sf.glow;
    if (glowGain > 0.001f && P.glowR > 0.f) {
      covG = hasStroke ? covS : covF;
      const int r = std::max(1, static_cast<int>(std::lround(P.glowR * em)));
      boxBlur(covG, bw, bh, r, r);
      boxBlur(covG, bw, bh, r, r);
    }

    // biseau : coverage adoucie dont on lit le gradient
    std::vector<float> covBv;
    int rbv = 1;
    if (P.bevel > 0.001f) {
      covBv = covF;
      rbv = std::max(1, static_cast<int>(std::lround(0.016 * em)));
      boxBlur(covBv, bw, bh, rbv, rbv);
    }

    // composition
    for (int y = 0; y < bh; ++y) {
      for (int x = 0; x < bw; ++x) {
        const size_t idx = (size_t)y * bw + x;
        const double px_ = X0 + x + 0.5, py_ = Y0 + y + 0.5;
        float* d = &C.px[((size_t)(Y0 + y) * C.w + (X0 + x)) * 4];
        auto over = [&](float r, float gg, float b, float a) {
          a = clamp01(a);
          if (a <= 0.f) return;
          d[0] = r * a + d[0] * (1.f - a);
          d[1] = gg * a + d[1] * (1.f - a);
          d[2] = b * a + d[2] * (1.f - a);
          d[3] = a + d[3] * (1.f - a);
        };
        if (hasShadow) over(P.shadow.r, P.shadow.g, P.shadow.b, clamp01(covH[idx]) * P.shadowAlpha);
        if (!covG.empty()) over(P.glow.r, P.glow.g, P.glow.b, clamp01(covG[idx] * glowGain));
        if (hasStroke) over(P.stroke.r, P.stroke.g, P.stroke.b, clamp01(covS[idx]));

        const float cf = clamp01(covF[idx]);
        const float cr = anyChroma ? clamp01(covR[idx]) : cf, cb = anyChroma ? clamp01(covB[idx]) : cf;
        const float amax = std::max(cf, std::max(cr, cb));
        if (amax <= 0.f) continue;

        // couleur : dégradé + sweep + scanline + flash + grain
        const float u = clamp01(static_cast<float>((px_ - cx - L.minX) / titleW));
        const float v = clamp01(static_cast<float>((py_ - cy - L.minY) / titleH));
        float col[3] = {P.fill.r, P.fill.g, P.fill.b};
        if (P.gradAmt > 0.001f) {
          const Rgb tv{lerp(P.gradTop.r, P.gradBottom.r, v), lerp(P.gradTop.g, P.gradBottom.g, v), lerp(P.gradTop.b, P.gradBottom.b, v)};
          const Rgb lh{lerp(P.gradLeft.r, P.gradRight.r, u), lerp(P.gradLeft.g, P.gradRight.g, u), lerp(P.gradLeft.b, P.gradRight.b, u)};
          const float k = P.gradH ? 0.5f : 1.f;
          const float tr = P.gradH ? tv.r + lh.r : tv.r, tg = P.gradH ? tv.g + lh.g : tv.g, tb = P.gradH ? tv.b + lh.b : tv.b;
          col[0] = lerp(col[0], k * tr, P.gradAmt);
          col[1] = lerp(col[1], k * tg, P.gradAmt);
          col[2] = lerp(col[2], k * tb, P.gradAmt);
        }
        if (G.sweep > -5.f) {
          const float dd = (u + 0.25f * v - G.sweep) / G.sweepW;
          const float band = std::exp(-dd * dd) * G.sweepAmt;
          for (float& c3 : col) c3 = lerp(c3, 1.f, clamp01(band));
        }
        if (G.scan > -5.f) {
          const float dd = (v - G.scan) / 0.04f;
          const float band = std::exp(-dd * dd) * G.scanAmt;
          for (float& c3 : col) c3 = lerp(c3, 1.f, clamp01(band));
        }
        if (P.texKind != kTNone && P.texAmt > 0.001f) {
          const float tv2 = texValue(P.texKind, static_cast<float>(px_), static_cast<float>(py_), P.texScale * static_cast<float>(std::max(0.3, pixelScale)), sx, T.seed);
          const float mt = lerp(1.f, tv2, clamp01(P.texAmt));
          for (float& c3 : col) c3 = std::max(0.f, c3 * mt);
        }
        if (!covBv.empty()) {
          auto at2 = [&](int xx, int yy) { return covBv[(size_t)std::clamp(yy, 0, bh - 1) * bw + std::clamp(xx, 0, bw - 1)]; };
          const float gx = at2(x + 1, y) - at2(x - 1, y), gy = at2(x, y + 1) - at2(x, y - 1);
          const float hl = std::clamp((gx + gy) * (rbv + 0.5f) * 0.9f, -1.f, 1.f);
          for (float& c3 : col) c3 = std::max(0.f, c3 + (hl > 0.f ? (1.f - c3) : c3) * hl * P.bevel);
        }
        if (sf.flash > 0.001f) {
          col[0] = lerp(col[0], P.flashColor.r, clamp01(sf.flash));
          col[1] = lerp(col[1], P.flashColor.g, clamp01(sf.flash));
          col[2] = lerp(col[2], P.flashColor.b, clamp01(sf.flash));
        }
        if (P.grain > 0.001f) {
          const float gn = hash01(static_cast<uint32_t>(X0 + x), static_cast<uint32_t>(Y0 + y), static_cast<uint32_t>(fr * 7 + T.seed)) - 0.5f;
          const float k = 1.f + gn * P.grain * 1.4f;
          for (float& c3 : col) c3 = std::max(0.f, c3 * k);
        }
        if (anyChroma && (cr != cf || cb != cf)) {
          // canaux décalés : R vers la gauche, B vers la droite, G centre
          d[0] = col[0] * cr + d[0] * (1.f - cr);
          d[1] = col[1] * cf + d[1] * (1.f - cf);
          d[2] = col[2] * cb + d[2] * (1.f - cb);
          d[3] = amax + d[3] * (1.f - amax);
        } else {
          over(col[0], col[1], col[2], cf);
        }
      }
    }
  }

  drawFx(P, T, C, cx, cy, std::max(0.25, pixelScale), fr, sx);
  return frameFx;
}

}  // namespace tc

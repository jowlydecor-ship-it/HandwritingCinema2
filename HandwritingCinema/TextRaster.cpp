#include "TextRaster.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <dlfcn.h>
#include <sys/stat.h>

#include <algorithm>
#include <cstdint>

namespace hc {

namespace {

void anchor() {}

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

std::vector<std::vector<uint32_t>> splitLines(const std::vector<uint32_t>& cps) {
  std::vector<std::vector<uint32_t>> lines(1);
  for (uint32_t c : cps) {
    if (c == '\r') continue;
    if (c == '\n') lines.emplace_back();
    else lines.back().push_back(c);
  }
  return lines;
}

}  // namespace

std::string resourcesDir() {
  Dl_info info;
  if (dladdr(reinterpret_cast<void*>(&anchor), &info) && info.dli_fname) {
    std::string p = info.dli_fname;  // .../Contents/MacOS/X.ofx
    for (int i = 0; i < 2; ++i) {
      auto s = p.find_last_of('/');
      if (s == std::string::npos) return "";
      p.resize(s);
    }
    return p + "/Resources";
  }
  return "";
}

std::shared_ptr<Mask> rasterizeText(const std::string& utf8, int px) {
  px = std::max(4, px);

  FT_Library lib;
  if (FT_Init_FreeType(&lib)) return nullptr;

  // Police : celle du bundle d'abord, sinon polices système macOS.
  const std::string res = resourcesDir();
  const std::string candidates[] = {
      res + "/fonts/default.ttf",
      "/System/Library/Fonts/Courier.ttc",
      "/System/Library/Fonts/Supplemental/Courier New.ttf",
      "/System/Library/Fonts/Monaco.ttf",
      "/System/Library/Fonts/Helvetica.ttc"};
  FT_Face face = nullptr;
  for (const auto& path : candidates) {
    if (fileExists(path) && FT_New_Face(lib, path.c_str(), 0, &face) == 0) break;
    face = nullptr;
  }
  if (!face) {
    FT_Done_FreeType(lib);
    return nullptr;
  }
  FT_Set_Pixel_Sizes(face, 0, px);

  const int ascent = static_cast<int>(face->size->metrics.ascender >> 6);
  int lineH = static_cast<int>(face->size->metrics.height >> 6);
  if (lineH <= 0) lineH = static_cast<int>(px * 1.2);

  const auto lines = splitLines(decodeUtf8(utf8));
  const int pad = std::max(2, px / 4);

  // Passe 1 : largeur de chaque ligne.
  std::vector<int> widths(lines.size(), 0);
  std::vector<std::vector<int>> penEnd(lines.size());
  int maxW = 0;
  for (size_t i = 0; i < lines.size(); ++i) {
    int w = 0;
    for (uint32_t c : lines[i]) {
      if (FT_Load_Char(face, c, FT_LOAD_DEFAULT) == 0)
        w += static_cast<int>(face->glyph->advance.x >> 6);
      penEnd[i].push_back(w);
    }
    widths[i] = w;
    maxW = std::max(maxW, w);
  }

  auto mask = std::make_shared<Mask>();
  mask->w = maxW + 2 * pad;
  mask->h = static_cast<int>(lines.size()) * lineH + 2 * pad;
  mask->a.assign(static_cast<size_t>(mask->w) * mask->h, 0.f);
  mask->lineH = lineH;
  mask->pad = pad;
  mask->widths = widths;
  mask->cum.assign(lines.size(), 0);
  int run = 0;
  for (size_t i = 0; i < lines.size(); ++i) { mask->cum[i] = run; run += widths[i]; }
  mask->total = std::max(1, run);
  mask->penEnd = penEnd;
  mask->charBase.assign(lines.size(), 0);
  int nch = 0;
  for (size_t i = 0; i < lines.size(); ++i) { mask->charBase[i] = nch; nch += static_cast<int>(penEnd[i].size()); }
  mask->totalChars = std::max(1, nch);

  // Passe 2 : dessin des glyphes (alignés à gauche).
  for (size_t i = 0; i < lines.size(); ++i) {
    int penX = pad;
    const int baseline = pad + ascent + static_cast<int>(i) * lineH;
    for (uint32_t c : lines[i]) {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0) continue;
      const FT_GlyphSlot g = face->glyph;
      const FT_Bitmap& bm = g->bitmap;
      for (unsigned row = 0; row < bm.rows; ++row) {
        const int y = baseline - g->bitmap_top + static_cast<int>(row);
        if (y < 0 || y >= mask->h) continue;
        for (unsigned col = 0; col < bm.width; ++col) {
          const int x = penX + g->bitmap_left + static_cast<int>(col);
          if (x < 0 || x >= mask->w) continue;
          const float v = bm.buffer[row * bm.pitch + col] / 255.f;
          float& dst = mask->a[static_cast<size_t>(y) * mask->w + x];
          dst = std::max(dst, v);
        }
      }
      penX += static_cast<int>(g->advance.x >> 6);
    }
  }

  FT_Done_Face(face);
  FT_Done_FreeType(lib);
  return mask;
}

}  // namespace hc

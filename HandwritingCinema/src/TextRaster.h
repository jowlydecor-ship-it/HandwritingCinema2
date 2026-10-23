#pragma once
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace hc {

// Masque de couverture 0..1, origine en haut à gauche (y vers le bas).
// Porte aussi la géométrie des lignes, pour calculer l'ordre de tracé du Write-On.
struct Mask {
  int w = 0, h = 0;
  std::vector<float> a;

  // Géométrie de mise en page
  int lineH = 1, pad = 0, total = 1;
  std::vector<int> widths, cum;
  std::vector<std::vector<int>> penEnd;  // x du stylo après chaque caractère (par ligne)
  std::vector<int> charBase;             // index global du 1er caractère de chaque ligne
  int totalChars = 1;

  float at(int x, int y) const {
    if (x < 0 || y < 0 || x >= w || y >= h) return 0.f;
    return a[static_cast<size_t>(y) * w + x];
  }

  // Échantillonnage bilinéaire (centres de pixels à +0.5)
  float sample(float x, float y) const {
    x -= 0.5f; y -= 0.5f;
    const int x0 = static_cast<int>(std::floor(x)), y0 = static_cast<int>(std::floor(y));
    const float fx = x - x0, fy = y - y0;
    const float top = at(x0, y0) * (1.f - fx) + at(x0 + 1, y0) * fx;
    const float bot = at(x0, y0 + 1) * (1.f - fx) + at(x0 + 1, y0 + 1) * fx;
    return top * (1.f - fy) + bot * fy;
  }

  // Ordre de tracé 0..1 : ligne par ligne, gauche -> droite, avec un front
  // légèrement incliné comme un poignet qui écrit.
  float orderAt(float mx, float my) const {
    const int n = static_cast<int>(widths.size());
    if (n == 0) return 0.f;
    const int line = std::clamp(static_cast<int>(std::floor((my - pad) / lineH)), 0, n - 1);
    const float xr = std::clamp(mx - pad, 0.f, static_cast<float>(widths[line]));
    const float slant = (my - pad - line * lineH - lineH * 0.5f) * 0.35f;
    return std::clamp((cum[line] + xr + slant) / total, 0.f, 1.f);
  }

  // Index global (0..totalChars-1) du caractère sous le point (mx, my).
  int charAt(float mx, float my) const {
    const int n = static_cast<int>(widths.size());
    if (n == 0) return 0;
    const int line = std::clamp(static_cast<int>(std::floor((my - pad) / lineH)), 0, n - 1);
    const std::vector<int>& pe = penEnd[line];
    if (pe.empty()) return charBase[line];
    const int xr = static_cast<int>(std::floor(mx - pad));
    int ci = static_cast<int>(std::upper_bound(pe.begin(), pe.end(), xr) - pe.begin());
    ci = std::min(ci, static_cast<int>(pe.size()) - 1);
    return charBase[line] + ci;
  }
};

// (méthode ci-dessous)
// Dossier Contents/Resources du bundle (déduit de l'emplacement du binaire).
std::string resourcesDir();

// Rasterise un texte UTF-8 multi-lignes avec FreeType.
// Renvoie nullptr si aucune police n'a pu être chargée.
std::shared_ptr<Mask> rasterizeText(const std::string& utf8, int pixelSize);

}  // namespace hc

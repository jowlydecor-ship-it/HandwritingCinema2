#pragma once
// Bruit Simplex 2D (Gustavson), sortie dans [-1, 1], table de permutation fixe.
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <random>

namespace hc {

class Simplex {
 public:
  Simplex() {
    std::array<uint8_t, 256> p;
    std::iota(p.begin(), p.end(), 0);
    std::mt19937 rng(1337);
    std::shuffle(p.begin(), p.end(), rng);
    for (int i = 0; i < 512; ++i) perm_[i] = p[i & 255];
  }

  float noise(float xin, float yin) const {
    static const float F2 = 0.3660254037844386f, G2 = 0.21132486540518713f;
    const float s = (xin + yin) * F2;
    const int i = static_cast<int>(std::floor(xin + s));
    const int j = static_cast<int>(std::floor(yin + s));
    const float t = (i + j) * G2;
    const float x0 = xin - (i - t), y0 = yin - (j - t);
    const int i1 = x0 > y0 ? 1 : 0, j1 = x0 > y0 ? 0 : 1;
    const float x1 = x0 - i1 + G2, y1 = y0 - j1 + G2;
    const float x2 = x0 - 1.f + 2.f * G2, y2 = y0 - 1.f + 2.f * G2;
    const int ii = i & 255, jj = j & 255;
    const int g0 = perm_[ii + perm_[jj]] % 12;
    const int g1 = perm_[ii + i1 + perm_[jj + j1]] % 12;
    const int g2 = perm_[ii + 1 + perm_[jj + 1]] % 12;
    return 70.f * (corner(g0, x0, y0) + corner(g1, x1, y1) + corner(g2, x2, y2));
  }

  // Deux octaves : rugosité fine des bords d'encre.
  float fbm(float x, float y) const {
    return 0.65f * noise(x, y) + 0.35f * noise(2.f * x + 17.f, 2.f * y + 31.f);
  }

 private:
  static float corner(int g, float x, float y) {
    static const float grad[12][2] = {{1, 1},  {-1, 1}, {1, -1}, {-1, -1},
                                      {1, 0},  {-1, 0}, {1, 0},  {-1, 0},
                                      {0, 1},  {0, -1}, {0, 1},  {0, -1}};
    float t = 0.5f - x * x - y * y;
    if (t < 0.f) return 0.f;
    t *= t;
    return t * t * (grad[g][0] * x + grad[g][1] * y);
  }
  std::array<uint8_t, 512> perm_;
};

inline const Simplex& simplex() {
  static const Simplex s;
  return s;
}

}  // namespace hc

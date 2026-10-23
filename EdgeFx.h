#pragma once
// Ink bleed, partie 1 : étalement de l'encre (flou en boîte, 2 passes H+V).
// La rugosité des bords (partie 2) est appliquée par pixel dans le rendu,
// avec un bruit qui change à chaque image du stop-motion.
#include <memory>
#include <vector>

#include "TextRaster.h"

namespace hc {

inline void boxPass(const std::vector<float>& in, std::vector<float>& out,
                    int w, int h, int R, bool horiz) {
  const int len = horiz ? w : h, lines = horiz ? h : w;
  const float inv = 1.f / (2 * R + 1);
  for (int l = 0; l < lines; ++l) {
    auto idx = [&](int i) { return horiz ? static_cast<size_t>(l) * w + i
                                         : static_cast<size_t>(i) * w + l; };
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
}

inline std::shared_ptr<Mask> spreadInk(const Mask& m, int R) {
  auto out = std::make_shared<Mask>(m);
  if (R <= 0) return out;
  std::vector<float> tmp(m.a.size());
  for (int pass = 0; pass < 2; ++pass) {
    boxPass(out->a, tmp, m.w, m.h, R, true);
    boxPass(tmp, out->a, m.w, m.h, R, false);
  }
  return out;
}

}  // namespace hc

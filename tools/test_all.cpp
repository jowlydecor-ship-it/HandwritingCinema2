#include <chrono>
#include <cstdio>
#include <cmath>
#include "../src/TitleEngine.h"
using namespace tc;
int main() {
  const auto& all = presets();
  printf("%zu presets\n", all.size());
  int bad = 0; double worst = 0; std::string worstName;
  for (size_t i = 0; i < all.size(); ++i) {
    const Params& P = all[i];
    auto L = buildLayout("Dear Diary", 56, resolveFont(P.font, P.fontName), P.tracking, P.lineSpacing, P.strokeW, P.scrambleSet, P.italic, P.embolden);
    if (!L || L->g.empty()) { printf("LAYOUT FAIL %s\n", P.name.c_str()); ++bad; continue; }
    int n = (int)L->g.size();
    double inS = entrySpan(P, n), outS = exitSpan(P, n), hold = 20, total = inS + hold + outS;
    double maxA = 0, hiddenAtStart = 1, holdA = 0;
    auto t0 = std::chrono::steady_clock::now();
    for (int k = 0; k <= 20; ++k) {
      double t = total * k / 20.0;
      Times T; T.tin = t; T.tout = t - (inS + hold); T.tidle = t; T.seed = 1;
      Canvas C; C.init(440, 200);
      renderTitle(*L, P, T, 220, 100, C);
      double a = 0; for (size_t q = 3; q < C.px.size(); q += 4) a += C.px[q];
      if (k == 0) hiddenAtStart = a;
      if (std::fabs(t - (inS + hold * 0.5)) < total / 20.0) holdA = std::max(holdA, a);
      maxA = std::max(maxA, a);
      for (float v : C.px) if (!std::isfinite(v)) { printf("NaN in %s t=%f\n", P.name.c_str(), t); ++bad; break; }
    }
    double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count() / 21;
    if (ms > worst) { worst = ms; worstName = P.name; }
    if (maxA < 50) { printf("EMPTY? %s maxA=%.1f\n", P.name.c_str(), maxA); ++bad; }
    if (holdA < 50) { printf("NO HOLD VISIBLE: %s holdA=%.1f (in=%d out=%d)\n", P.name.c_str(), holdA, P.in.kind, P.out.kind); }
  }
  printf("bad=%d  slowest %.1f ms/frame (%s)\n", bad, worst, worstName.c_str());
}

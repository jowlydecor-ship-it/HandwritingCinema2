// Harnais de test local : rend des images d'un preset en bande PPM.
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include "../src/TitleEngine.h"
using namespace tc;
int main(int argc, char** argv) {
  int idx = argc > 1 ? atoi(argv[1]) : 0;
  std::string text = argc > 2 ? argv[2] : "HELLO\nWORLD";
  const char* out = argc > 3 ? argv[3] : "/tmp/out.ppm";
  const auto& all = presets();
  if (argc > 1 && std::string(argv[1]) == "list") { for (size_t i=0;i<all.size();++i) printf("%zu %s\n", i, all[i].name.c_str()); return 0; }
  Params P = all[idx % all.size()];
  printf("preset %d: %s in=%d idle=%d out=%d font=%d\n", idx, P.name.c_str(), P.in.kind, P.idle.kind, P.out.kind, P.font);
  std::string f = resolveFont(P.font, "");
  printf("font: %s\n", f.c_str());
  if (P.upper) for (auto& c : text) c = toupper(c);
  const int px = 56, W = 440, H = 200;
  auto L = buildLayout(text, px, f, P.tracking, P.lineSpacing, P.strokeW, P.scrambleSet);
  if (!L) { printf("layout fail\n"); return 1; }
  int n = (int)L->g.size();
  double inSpan = entrySpan(P, n), outSpan = exitSpan(P, n);
  double hold = 20, total = inSpan + hold + outSpan;
  const int FR = 8;
  std::vector<float> sheet((size_t)W*FR*H*3);
  for (int k = 0; k < FR; ++k) {
    double t = total * k / (FR - 1) * 0.999;
    Times T; T.tin = t; T.tout = t - (inSpan + hold); T.tidle = t; T.seed = 1;
    Canvas C; C.init(W, H);
    renderTitle(*L, P, T, W/2.0, H/2.0, C);
    for (int y=0;y<H;++y) for (int x=0;x<W;++x) {
      float* s=&C.px[((size_t)y*W+x)*4];
      float bg = 0.30f + 0.10f*(y/(float)H);
      for (int ch=0;ch<3;++ch) sheet[((size_t)y*W*FR + k*W + x)*3+ch] = s[ch] + bg*(1-s[3]);
    }
  }
  FILE* fp=fopen(out,"wb"); fprintf(fp,"P6\n%d %d\n255\n", W*FR, H);
  for (float v: sheet){ int q=(int)std::lround(std::fmin(1.f,std::fmax(0.f,v))*255); fputc(q,fp);} fclose(fp);
  return 0;
}

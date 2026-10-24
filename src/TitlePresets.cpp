// Presets compilés : chaque ligne de TitlePresetsData.inc décrit un preset en texte.
//   Nom|cat=...|font=...|in=kind/f=12/a=1/b=0/c=0/dir=left/curve=elastic|idle=...|out=...|...
// Généré par tools/gen_presets.py (source de vérité : tools/presets_src.py).
#include <algorithm>
#include <cstdlib>
#include <sstream>

#include "TitleEngine.h"

namespace tc {
namespace {

const char* const kData[] = {
#include "TitlePresetsData.inc"
};

std::vector<std::string> split(const std::string& s, char sep) {
  std::vector<std::string> out;
  std::string cur;
  for (char c : s) {
    if (c == sep) { out.push_back(cur); cur.clear(); }
    else cur += c;
  }
  out.push_back(cur);
  return out;
}

float num(const std::string& s, float fallback = 0.f) {
  if (s.empty()) return fallback;
  return static_cast<float>(std::atof(s.c_str()));
}

Rgb hex(const std::string& s, Rgb fallback = Rgb()) {
  if (s.size() != 7 || s[0] != '#') return fallback;
  auto h = [&](int i) { return static_cast<float>(std::strtol(s.substr(i, 2).c_str(), nullptr, 16)) / 255.f; };
  return Rgb{h(1), h(3), h(5)};
}

// Valeurs par défaut propres à chaque type d'animation.
Anim defaults(const char* table, int kind) {
  Anim a;
  a.kind = kind;
  const std::string t = table;
  auto set = [&](float f, float aa, float bb, int dir, int curve, float cc = 0.f) {
    a.frames = f; a.a = aa; a.b = bb; a.c = cc; a.dir = dir; a.curve = curve;
  };
  if (t == "in") {
    switch (kind) {
      case kInFade: set(10, 0, 0, kUp, kEaseOut); break;
      case kInPop: set(8, 0, 0, kUp, kBackOut); break;
      case kInSlide: set(10, 1.5f, 0, kLeft, kEaseOut); break;
      case kInDrop: set(14, 3, 0, kUp, kBounce); break;
      case kInZoom: set(10, 3, 1, kUp, kEaseOut); break;
      case kInFocus: set(12, 1.5f, 0.35f, kLeft, kEaseOut); break;
      case kInStrobe: set(10, 0.1f, 0, kUp, kLinear); break;
      case kInStep: set(1, 0, 0, kUp, kLinear); break;
      case kInSpinY: case kInSpinX: set(12, 90, 0, kUp, kEaseOut); break;
      case kInSpinZ: set(14, 360, 0, kUp, kEaseOut); break;
      case kInWipe: set(10, 0, 0, kLeft, kEaseOut); break;
      case kInDissolve: set(16, 0, 0, kUp, kLinear); break;
      case kInScramble: set(10, 0, 0, kUp, kLinear); break;
      case kInSplit: set(10, 1, 0, kUp, kEaseOut); break;
      case kInChroma: set(10, 0.15f, 0, kUp, kEaseOut); break;
      case kInFlash: set(6, 1, 0, kUp, kLinear); break;
      case kInSpread: set(14, 0.8f, 0, kUp, kEaseOut); break;
      case kInGlow: set(14, 2, 0, kUp, kEaseOut); break;
      case kInStretch: set(10, 2, 0.2f, kUp, kEaseOut); break;
      default: break;
    }
  } else if (t == "out") {
    switch (kind) {
      case kOutFade: set(8, 0, 0, kUp, kLinear); break;
      case kOutSlide: set(8, 1.5f, 0, kRight, kEaseIn); break;
      case kOutFall: set(12, 3, 0, kDown, kEaseIn); break;
      case kOutZoomOut: set(8, 0, 0, kUp, kEaseIn); break;
      case kOutZoomThrough: set(8, 6, 0, kUp, kEaseIn); break;
      case kOutSquashY: case kOutSquashX: set(4, 0, 0, kUp, kEaseIn); break;
      case kOutCRT: set(8, 0, 0, kUp, kLinear); break;
      case kOutTear: set(10, 1.5f, 0, kUp, kEaseIn); break;
      case kOutBurn: set(14, 2, 0.8f, kUp, kEaseIn); break;
      case kOutDissolve: set(16, 0, 0, kUp, kLinear); break;
      case kOutBlur: set(10, 0.4f, 0, kLeft, kEaseIn); break;
      case kOutFlash: set(2, 0, 0, kUp, kLinear); break;
      case kOutMelt: set(16, 1.5f, 0, kUp, kEaseIn); break;
      case kOutScatter: set(12, 3, 180, kUp, kEaseIn); break;
      case kOutStrobe: set(10, 0, 0, kUp, kLinear); break;
      case kOutRise: set(14, 2, 0, kUp, kEaseIn); break;
      case kOutChroma: set(8, 0.2f, 0, kUp, kEaseIn); break;
      case kOutCut: set(1, 0, 0, kUp, kLinear); break;
      case kOutWipe: set(10, 0, 0, kRight, kEaseIn); break;
      case kOutSpinY: set(10, 90, 0, kUp, kEaseIn); break;
      case kOutStretch: set(8, 3, 0.1f, kUp, kEaseIn); break;
      default: break;
    }
  } else {  // idle
    switch (kind) {
      case kIdleJitter: set(1, 0.02f, 2, kUp, kLinear); break;
      case kIdleBop: set(40, 0.03f, 0.6f, kUp, kLinear); break;
      case kIdleWave: set(40, 0.03f, 0.5f, kUp, kLinear); break;
      case kIdlePulse: set(24, 0.04f, 0, kUp, kLinear); break;
      case kIdleFlicker: set(3, 0.4f, 0, kUp, kLinear); break;
      case kIdleWobble: set(24, 3, 0.7f, kUp, kLinear); break;
      case kIdleDrift: set(60, 0.1f, 0, kRight, kLinear); break;
      case kIdleBreathe: set(120, 0.1f, 0, kUp, kLinear); break;
      case kIdleSlowZoom: set(120, 0.04f, 0, kUp, kLinear); break;
      case kIdleGlowPulse: set(30, 0.8f, 0, kUp, kLinear); break;
      case kIdleSweep: set(60, 0.8f, 0.12f, kUp, kLinear); break;
      case kIdleScanline: set(40, 0.5f, 0, kUp, kLinear); break;
      case kIdleChromaJit: set(3, 0.06f, 0, kUp, kLinear); break;
      case kIdleBlurPulse: set(40, 0.1f, 0, kLeft, kLinear); break;
      case kIdleFloat: set(90, 0.04f, 0.4f, kUp, kLinear); break;
      case kIdleInterlace: set(5, 0.012f, 0, kUp, kLinear, 1); break;
      case kIdleSteps: set(3, 1, 0, kUp, kLinear); break;
      default: break;
    }
  }
  return a;
}

Anim parseAnim(const char* table, const std::string& v) {
  const auto parts = split(v, '/');
  int n = 0;
  names(table, n);
  const int kind = findName(table, parts.empty() ? "none" : parts[0], 0);
  Anim a = defaults(table, kind);
  for (size_t i = 1; i < parts.size(); ++i) {
    const auto kv = split(parts[i], '=');
    if (kv.size() != 2) continue;
    const std::string& k = kv[0];
    if (k == "f") a.frames = num(kv[1], a.frames);
    else if (k == "a") a.a = num(kv[1], a.a);
    else if (k == "b") a.b = num(kv[1], a.b);
    else if (k == "c") a.c = num(kv[1], a.c);
    else if (k == "dir") a.dir = findName("dir", kv[1], a.dir);
    else if (k == "curve") a.curve = findName("curve", kv[1], a.curve);
  }
  return a;
}

Params parseLine(const std::string& line) {
  Params P;
  const auto fields = split(line, '|');
  if (fields.empty()) return P;
  P.name = fields[0];
  for (size_t i = 1; i < fields.size(); ++i) {
    const size_t eq = fields[i].find('=');
    if (eq == std::string::npos) continue;
    const std::string k = fields[i].substr(0, eq), v = fields[i].substr(eq + 1);
    const auto p = split(v, '/');
    auto at = [&](size_t j) { return j < p.size() ? p[j] : std::string(); };
    if (k == "cat") P.category = v;
    else if (k == "font") P.font = findName("font", v, kSans);
    else if (k == "fps") P.fps = static_cast<int>(num(v));
    else if (k == "order") P.order = findName("order", v, kLTR);
    else if (k == "delay") P.delay = num(v, P.delay);
    else if (k == "odelay") P.outDelay = num(v, P.outDelay);
    else if (k == "anchor") P.anchor = findName("anchor", v, kLetter);
    else if (k == "track") P.tracking = num(v);
    else if (k == "upper") P.upper = num(v) > 0.5f;
    else if (k == "in") P.in = parseAnim("in", v);
    else if (k == "idle") P.idle = parseAnim("idle", v);
    else if (k == "out") P.out = parseAnim("out", v);
    else if (k == "fill") P.fill = hex(v, P.fill);
    else if (k == "stroke") { P.stroke = hex(at(0), P.stroke); P.strokeW = num(at(1)); }
    else if (k == "shadow") {
      P.shadow = hex(at(0), P.shadow);
      P.shadowDx = num(at(1)); P.shadowDy = num(at(2)); P.shadowBlur = num(at(3)); P.shadowAlpha = num(at(4), 1.f);
    }
    else if (k == "glow") { P.glow = hex(at(0), P.glow); P.glowR = num(at(1)); P.glowAmt = num(at(2)); }
    else if (k == "grad") {
      P.gradTop = hex(at(0), P.fill); P.gradBottom = hex(at(1), P.fill);
      P.gradH = !at(2).empty() && !at(3).empty();
      P.gradLeft = hex(at(2), P.fill); P.gradRight = hex(at(3), P.fill); P.gradAmt = num(at(4), 1.f);
    }
    else if (k == "grain") P.grain = num(v);
    else if (k == "plate") { P.plate = hex(at(0), P.plate); P.plateAlpha = num(at(1), 1.f); P.platePad = num(at(2), 0.3f); }
    else if (k == "blend") P.blend = findName("blend", v, kNormal);
    else if (k == "scr") P.scrambleSet = static_cast<int>(num(v));
    else if (k == "mb") P.motionBlur = num(v, 1.f);
  }
  return P;
}

}  // namespace

const std::vector<Params>& presets() {
  static const std::vector<Params> all = [] {
    std::vector<Params> v;
    for (const char* line : kData) v.push_back(parseLine(line));
    if (v.empty()) v.push_back(Params());
    return v;
  }();
  return all;
}

}  // namespace tc

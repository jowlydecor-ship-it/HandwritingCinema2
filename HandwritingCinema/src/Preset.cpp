#include "Preset.h"

#include <algorithm>
#include <cctype>
#include <fstream>

#include <nlohmann/json.hpp>

#include "Presets.h"
#include "TextRaster.h"

namespace hc {
namespace {

struct Builtin { Reveal reveal; bool rnd; bool radial; };
// Même ordre que kPresetNames.
const Builtin kBuiltin[20] = {
    {Reveal::Stroke, false, false},   // Diary Marker
    {Reveal::Stroke, false, false},   // Sharpie Scribble
    {Reveal::Sweep, false, false},    // Highlighter Note
    {Reveal::Stroke, false, false},   // Ballpoint Scratch
    {Reveal::Flash, false, false},    // Scratch on Film
    {Reveal::Erosion, false, false},  // Blair Witch Charcoal
    {Reveal::Erosion, false, false},  // Bleached Negative
    {Reveal::PerChar, true, false},   // Ransom Note
    {Reveal::PerChar, false, false},  // Mechanical Typewriter
    {Reveal::Scanline, false, false}, // Thermal Fax
    {Reveal::Stroke, false, false},   // Classroom Chalkboard
    {Reveal::Erosion, false, true},   // Spray Stencil
    {Reveal::Stroke, false, false},   // Wet Paint Brush
    {Reveal::Pop, false, false},      // Camcorder OSD
    {Reveal::Sweep, false, false},    // Photocopier Burn
    {Reveal::Stroke, false, false},   // Graphite Sketch
    {Reveal::Sweep, false, false},    // Liquid Correction Tape
    {Reveal::Pop, false, false},      // Stamp & Ink Pad
    {Reveal::Flash, false, false},    // Neon Sign Flicker
    {Reveal::Stroke, false, false}};  // White Correction Marker

std::string slug(const std::string& s) {
  std::string out;
  for (unsigned char c : s) {
    if (std::isalnum(c)) out += static_cast<char>(std::tolower(c));
    else if (!out.empty() && out.back() != '_') out += '_';
  }
  while (!out.empty() && out.back() == '_') out.pop_back();
  return out;
}

Reveal parseReveal(const std::string& s, Reveal fallback) {
  if (s == "stroke") return Reveal::Stroke;
  if (s == "sweep") return Reveal::Sweep;
  if (s == "scanline") return Reveal::Scanline;
  if (s == "erosion") return Reveal::Erosion;
  if (s == "flash") return Reveal::Flash;
  if (s == "pop") return Reveal::Pop;
  if (s == "per_char") return Reveal::PerChar;
  return fallback;
}

int parseBlend(const std::string& s) {
  if (s == "Multiply") return 1;
  if (s == "Overlay") return 2;
  if (s == "Screen") return 3;
  return 0;
}

int fpsToIndex(int fps) { return fps >= 18 ? 0 : (fps >= 10 ? 1 : 2); }

std::vector<Preset> load() {
  using nlohmann::json;
  std::vector<Preset> out;
  const std::string dir = resourcesDir() + "/presets/";
  for (size_t i = 0; i < kPresetNames.size(); ++i) {
    Preset p;
    p.name = kPresetNames[i];
    p.reveal = kBuiltin[i].reveal;
    p.charRandom = kBuiltin[i].rnd;
    p.radial = kBuiltin[i].radial;

    std::ifstream f(dir + slug(p.name) + ".json");
    if (f) {
      json j = json::parse(f, nullptr, /*allow_exceptions=*/false);
      if (!j.is_discarded() && j.is_object()) {
        p.reveal = parseReveal(j.value("reveal", ""), p.reveal);
        p.charRandom = j.value("charRandom", p.charRandom);
        p.radial = j.value("radial", p.radial);
        if (j.contains("params") && j["params"].is_object()) {
          const json& q = j["params"];
          p.inkBleed = q.value("inkBleed", p.inkBleed);
          p.jitter = q.value("jitter", p.jitter);
          p.opacity = q.value("opacity", p.opacity);
          p.stopMotion = fpsToIndex(q.value("stopMotionFps", 12));
          p.blend = parseBlend(q.value("blendMode", "Normal"));
          p.grain = q.value("grainScratch", p.grain);
          if (q.contains("inkColor") && q["inkColor"].is_array() && q["inkColor"].size() == 3)
            for (int c = 0; c < 3; ++c) p.color[c] = q["inkColor"][c].get<double>();
        }
      }
    }
    out.push_back(p);
  }
  return out;
}

}  // namespace

const std::vector<Preset>& presets() {
  static const std::vector<Preset> all = load();  // init thread-safe (C++11)
  return all;
}

}  // namespace hc

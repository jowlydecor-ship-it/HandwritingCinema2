#pragma once
#include <string>
#include <vector>

namespace hc {

// Mécanisme de révélation (Write-On) : voir revealAt() dans Plugin.cpp.
enum class Reveal { Stroke, Sweep, Scanline, Erosion, Flash, Pop, PerChar };

struct Preset {
  std::string name;
  Reveal reveal = Reveal::Stroke;
  bool charRandom = false;  // PerChar : ordre aléatoire (Ransom Note) ou séquentiel
  bool radial = false;      // Erosion : du centre vers l'extérieur (Spray Stencil)
  double inkBleed = 30, jitter = 25, opacity = 100;
  int stopMotion = 1;       // 0 = 24 fps, 1 = 12 fps, 2 = 8 fps
  double color[3] = {0.04, 0.04, 0.05};
  int blend = 0;            // 0 Normal, 1 Multiply, 2 Overlay, 3 Screen
  bool grain = true;
};

// Les 20 presets, dans l'ordre du menu. Chargés une seule fois depuis
// Resources/presets/*.json ; les valeurs manquantes retombent sur des défauts intégrés.
const std::vector<Preset>& presets();

}  // namespace hc

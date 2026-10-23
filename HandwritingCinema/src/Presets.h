#pragma once
#include <array>

namespace hc {

// Ordre = ordre du menu déroulant. Les valeurs des presets (JSON) seront
// chargées depuis Resources/presets/ à l'étape 3.
inline constexpr std::array<const char*, 20> kPresetNames = {{
    // Cat. 1 : marqueur, feutre, journal intime
    "Diary Marker", "Sharpie Scribble", "Highlighter Note", "Ballpoint Scratch",
    // Cat. 2 : pellicule scratchée, horreur, thriller
    "Scratch on Film", "Blair Witch Charcoal", "Bleached Negative", "Ransom Note",
    // Cat. 3 : machine à écrire, fax
    "Mechanical Typewriter", "Thermal Fax",
    // Cat. 4 : craie, peinture, street art
    "Classroom Chalkboard", "Spray Stencil", "Wet Paint Brush",
    // Cat. 5 : caméscope VHS, photocopieuse
    "Camcorder OSD", "Photocopier Burn",
    // Cat. 6 : crayon, correcteur, tampon, néon, posca
    "Graphite Sketch", "Liquid Correction Tape", "Stamp & Ink Pad",
    "Neon Sign Flicker", "White Correction Marker"}};

}  // namespace hc

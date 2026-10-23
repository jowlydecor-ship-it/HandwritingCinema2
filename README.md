# HandwritingCinema90s2000s : étape 3

Squelette OpenFX (Resolve Studio, macOS) : bundle, CMake, tous les paramètres
de l'inspecteur, rendu CPU minimal (texte FreeType + Write-On provisoire +
couleur / opacité / fusion).

## Build
    brew install cmake
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build -j
    sudo cmake --build build --target install_resolve

Redémarre Resolve : l'effet apparaît dans OpenFX > Cinema 90s
(Generators et Effects). Sans accès réseau pour FetchContent :
`-DOFX_ROOT=/chemin/vers/openfx` et clone FreeType à la main.

## Ce qui est branché / pas encore
- Branché : texte, taille, position, couleur, opacité, fusion, Stop-Motion
  (24/12/8 fps), Jitter Simplex, Ink Bleed, Seed, et **les 20 presets** :
  choisir un preset dans le menu pousse ses valeurs (bleed, fps, jitter,
  couleur, opacité, fusion, grain) dans les curseurs, que tu peux retoucher.
- Mécanismes de révélation (pilotés par Write-On Progress) :
  - stroke : ligne par ligne, front incliné, pression variable
    (Diary Marker, Sharpie, Ballpoint, Chalkboard, Wet Paint, Graphite, Posca)
  - sweep : bande gauche -> droite à bord biseauté
    (Highlighter, Correction Tape, Photocopier)
  - scanline : balayage haut -> bas par bandes (Thermal Fax)
  - erosion : dissolution par bruit, radiale pour Spray Stencil
    (Charcoal, Bleached Negative, Spray Stencil)
  - flash : stroboscope, la probabilité d'être allumé monte avec le slider
    (Scratch on Film, Neon Sign Flicker)
  - pop : apparition instantanée (Stamp, Camcorder OSD)
  - per_char : une lettre à la fois, séquentielle (Typewriter) ou aléatoire (Ransom Note)
- Déclaré seulement : Grain & Scratch.

## Presets JSON
Resources/presets/<nom>.json (ex. diary_marker.json). Champs : reveal,
charRandom, radial, params {inkBleed, stopMotionFps, jitter, inkColor,
opacity, blendMode, grainScratch}. Fichier absent ou invalide : le plugin
retombe sur les défauts intégrés.

## Prochaine étape (4) : matière et effets propres à chaque preset
Grain 35mm, rayures et poussières, textures (craie, fusain, papier), halo des
coupes de pellicule, aberration chromatique, tampon (zoom), machine à écrire
(recul), tracé de bord pour le néon, etc.

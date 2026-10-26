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

---
## Étape 4 : second effet « Cinema Titles 90s-2000s » (240 presets)

Le bundle contient DEUX effets (Resolve : OpenFX > Cinema 90s) :
- **Handwriting Cinema 90s-2000s** : l'effet manuscrit d'origine (inchangé).
- **Cinema Titles 90s-2000s** : titres animés lettre par lettre, 240 presets détaillés.

### Moteur
- Entrée / mouvement continu (idle) / sortie par lettre, avec ordre de lecture, décalage, courbes
  (dont les cubic-bezier exacts : EaseOutQuad/Expo/Cubic/Back, EaseInQuad, EaseInOutQuad, Fast Linear,
  Elastic ~15 % + 2 rebonds, Bounce, Steps), cadence stop-motion (24/12/8/6 fps) et motion blur réel.
- Typographie : polices Mac réelles par nom (Futura, Didot, Bodoni, Courier New, Impact, Copperplate,
  Rockwell, Brush Script...), repli automatique, italique et gras synthétiques, espacement en em.
- Matière des lettres : dégradé 4 points, contour, ombre, lueur, biseau, textures (papier, béton, métal
  brossé, sable, rouille, huile, rayures, demi-teintes, bois), grain, plaque de couleur, séparation RVB.
- Atmosphère plein cadre : vignette, étalonnage (saturation / contraste / split-tone), scanlines, grain,
  rayures de pellicule, brume, flare anamorphique, particules (neige, pluie, poussière, étincelles,
  cendres, bokeh, bulles, braises, spores, confettis, neige TV), flash blanc / fondu noir du cadre.
- Mode Auto (entrée au début, sortie calée sur la fin du plan) ou Manual (curseurs animables).

Choisir un preset pousse toutes ses valeurs dans l'inspecteur ; tout reste modifiable.

### Ajouter / modifier des presets
Éditer `tools/presets_src.py` puis `python3 tools/gen_presets.py` (régénère `src/TitlePresetsData.inc`).
Test local sans OpenFX : `g++ -O2 -std=c++17 -I<freetype> -Isrc src/TitleEngine.cpp src/TitlePresets.cpp tools/test_render.cpp -lfreetype`

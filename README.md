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
## Étape 4 : second effet « Cinema Titles 90s-2000s »

Le bundle contient maintenant DEUX effets (Resolve : OpenFX > Cinema 90s) :
- **Handwriting Cinema 90s-2000s** : l'effet manuscrit d'origine (inchangé).
- **Cinema Titles 90s-2000s** : titres animés lettre par lettre, 174 presets
  (cinéma, sitcoms, séries ado/SF/policières, anime & cartoons).

### Ce que fait Cinema Titles
Chaque lettre a sa propre entrée, son mouvement continu (idle) et sa sortie, avec
un décalage par lettre, un ordre de lecture, une courbe de mouvement, du motion blur
réel (sous-échantillonnage) et un flou directionnel / de mise au point.
- Entrées : Fade, Pop, Slide, Drop, Zoom, Focus, Strobe, Step, Spin X/Y/Z, Wipe,
  Dissolve, Scramble, Split, Chroma, Flash, Spread, Glow In, Stretch.
- Idle : Jitter, Bop, Wave, Pulse, Flicker, Wobble, Drift, Breathe, Slow Zoom,
  Glow Pulse, Sweep, Scanline, Chroma Jitter, Blur Pulse, Float, Interlace, Steps.
- Sorties : Fade, Slide, Fall, Zoom Out/Through, Squash X/Y, CRT Off, Tear, Burn,
  Dissolve, Blur, Flash, Melt, Scatter, Strobe, Rise, Chroma, Cut, Wipe, Spin Y, Stretch.
- Courbes : Linear, Ease Out/In/In-Out, Elastic (dépassement ~15 %, 2 rebonds),
  Bounce, Back Out, Expo Out, Steps.
- Ordre : gauche→droite, droite→gauche, centre, bords, aléatoire, pair/impair, tout.
- Origine de l'animation : centre de la lettre, centre du texte, bas-gauche, haut, bas.
- Look : couleur, dégradé 4 points, contour, ombre, lueur, grain dans les lettres,
  plaque de couleur, mode de fusion, police par style (18 styles) ou par nom/chemin.

Choisir un preset pousse toutes ses valeurs dans l'inspecteur ; tout reste modifiable.
Mode **Auto** : entrée au début du plan, sortie calée sur la fin. Mode **Manual** :
deux curseurs (progression entrée / sortie) à animer avec des keyframes.

### Ajouter / modifier des presets
Éditer `tools/presets_src.py` puis `python3 tools/gen_presets.py` (régénère
`src/TitlePresetsData.inc`). Test local sans OpenFX :
`g++ -O2 -std=c++17 -I<freetype> -Isrc src/TitleEngine.cpp src/TitlePresets.cpp tools/test_render.cpp -lfreetype`

# Source de vérité des presets. `python3 tools/gen_presets.py` -> src/TitlePresetsData.inc
PRESETS = []

def p(name, cat, font, fill, i="none", d="none", o="none", order="ltr", delay=2, odelay=1,
      stroke=None, shadow=None, glow=None, grad=None, grain=0, plate=None,
      blend="normal", scr=0, mb=1, fps=0, track=0, upper=0, anchor="letter"):
    f = [name, f"cat={cat}", f"font={font}", f"fill={fill}", f"order={order}",
         f"delay={delay}", f"odelay={odelay}", f"in={i}", f"idle={d}", f"out={o}"]
    if stroke: f.append("stroke=%s/%s" % stroke)
    if shadow: f.append("shadow=%s/%s/%s/%s/%s" % shadow)
    if glow: f.append("glow=%s/%s/%s" % glow)
    if grad: f.append("grad=" + "/".join(str(x) for x in grad))
    if grain: f.append(f"grain={grain}")
    if plate: f.append("plate=%s/%s/%s" % plate)
    if blend != "normal": f.append(f"blend={blend}")
    if scr: f.append(f"scr={scr}")
    if mb != 1: f.append(f"mb={mb}")
    if fps: f.append(f"fps={fps}")
    if track: f.append(f"track={track}")
    if upper: f.append("upper=1")
    if anchor != "letter": f.append(f"anchor={anchor}")
    PRESETS.append("|".join(f))

T1, T2, T3, T4 = "Thriller & Glitch", "Pop, Skate & Sitcom", "Indie & Melancolie", "Blockbusters & Action"
# ---------------------------------------------------------------- Partie 1
p("01 Se7en", T1, "hand", "#E8E4DC", "strobe/f=12/a=0.12", "jitter/f=1/a=0.012/b=1", "tear/f=10/a=1.2",
  order="random", delay=1.5, odelay=0)
p("02 Fight Club", T1, "sansbold", "#5FA8D3", "pop/f=8/a=0.05/curve=easeout", "breathe/f=240/a=0.25/b=1",
  "squashy/f=3", order="all", glow=("#66CCFF", 0.04, 0.5), mb=1.6)
p("03 Pi", T1, "mono", "#E8FFE8", "scramble/f=10", "blurpulse/f=60/a=0.05/dir=up", "dissolve/f=18/b=0.07",
  scr=1, delay=2)
p("04 Natural Born Killers", T1, "marker", "#FFE000", "strobe/f=14/a=0.25", "jitter/f=2/a=0.03/b=10",
  "zoomthrough/f=14/a=8", order="random", delay=1, stroke=("#FF0080", 0.07))
p("05 Spawn", T1, "gothic", "#B8B8C4", "slide/f=16/a=1.5/dir=down/curve=easeout", "wave/f=50/a=0.012/b=0.9",
  "melt/f=20/a=1.2", mb=2, grad=("#D8D8E0", "#50505C", "", "", 1), shadow=("#000000", 0, 0.06, 0.08, 0.9))
p("06 Blair Witch Project", T1, "hand", "#D5DCCC", "step", "interlace/f=5/a=0.01", "chroma/f=3/a=0.15",
  order="all", grain=0.35, fps=12)
p("07 Lost Highway", T1, "serif", "#C9A24A", "focus/f=45/a=1.0/b=0.4/dir=upleft/curve=easeinout",
  "flicker/f=2/a=0.4/c=1", "wipe/f=24/dir=right/curve=easeinout", order="all", grain=0.1)
p("08 Requiem for a Dream", T1, "sans", "#FFFFFF", "strobe/f=6/a=0", "jitter/f=1/a=0.002", "zoomout/f=4/a=0",
  order="all", grain=0.5)
p("09 Resident Evil", T1, "mono", "#FF2A2A", "wipe/f=14/dir=left/curve=linear", "scanline/f=30/a=0.7",
  "dissolve/f=12/b=0.12", order="all", glow=("#FF0000", 0.12, 0.9))
p("10 Cube", T1, "condensed", "#FFB000", "slide/f=8/a=1.2/dir=alternate/curve=easeout", "none",
  "scatter/f=12/a=1/b=15/curve=easeout", delay=3)
p("11 Memento", T1, "typewriter", "#EDEAE0", "focus/f=14/a=1.3/b=0.4/dir=upleft",
  "drift/f=240/a=0.06/dir=right", "fade/f=36/curve=linear", order="all")
p("12 Dark City", T1, "serifbold", "#D9B382", "spinx/f=16/a=85/curve=easeout", "float/f=120/a=0.015",
  "zoomout/f=22/a=0.15/curve=easein", order="all", grad=("#D9B382", "#7A5A32", "", "", 1),
  shadow=("#000000", 0.15, 0.2, 0.08, 0.6))
p("13 Panic Room", T1, "sansbold", "#8E918F", "slide/f=20/a=2/dir=right/curve=easeinout", "float/f=200/a=0.008",
  "zoomthrough/f=18/a=4", order="all", shadow=("#000000", 0.05, 0.08, 0.06, 0.7))
p("14 Matrix", T1, "mono", "#00FF66", "scramble/f=14", "flicker/f=2/a=0.35", "melt/f=16/a=2",
  order="random", delay=1.5, glow=("#00FF41", 0.08, 0.9))
p("15 The Ring", T1, "sans", "#FFFFFF", "chroma/f=12/a=0.18", "interlace/f=4/a=0.01", "crtoff/f=8",
  order="all", glow=("#FFFFFF", 0.08, 0.5), grain=0.3)
p("16 X-Files (cinema)", T1, "serif", "#AEBBA6", "wipe/f=24/dir=left/curve=easeinout", "pulse/f=90/a=0.02",
  "blur/f=24/a=0.3/b=0.3/dir=upleft", order="all")
p("17 From Hell", T1, "script", "#4A0E2A", "dissolve/f=26", "blurpulse/f=60/a=0.02/dir=down", "melt/f=24/a=1.6",
  order="all")
p("18 American Psycho", T1, "seriffine", "#F4F0E6", "slide/f=10/a=1.2/dir=right/curve=linear", "none",
  "fall/f=6/a=4/curve=easein", delay=2, order="ltr", track=0.08)
p("19 Snatch", T1, "impact", "#FFC400", "pop/f=5/a=0/curve=backout", "none", "slide/f=5/a=2.5/dir=left",
  order="all", stroke=("#000000", 0.08), mb=1.5)
p("20 28 Days Later", T1, "stencil", "#D2171E", "stretch/f=6/a=0.3/b=1", "jitter/f=1/a=0.004/b=2",
  "scatter/f=14/a=6/b=40", order="all", grain=0.6)
p("21 Max Payne", T1, "sans", "#FFFFFF", "dissolve/f=24", "slowzoom/f=240/a=0.03", "wipe/f=20/dir=right",
  order="all", shadow=("#000000", 0.02, 0.03, 0.06, 0.9))
p("22 Gothika", T1, "hand", "#A6C8FF", "strobe/f=16/a=0.02", "glowpulse/f=4/a=1.2", "flash/f=1", order="all",
  glow=("#5A9CFF", 0.1, 0.8))
p("23 Phone Game", T1, "mono", "#7CFF4F", "wipe/f=10/dir=left/curve=linear", "interlace/f=8/a=0.008",
  "squashy/f=6", order="all")
p("24 Blade", T1, "sansbold", "#E4E8EE", "split/f=10/a=0.8/dir=up", "sweep/f=50/a=0.9/b=0.1",
  "slide/f=5/a=3/dir=left", order="all", mb=2, grad=("#F2F4F8", "#7C8794", "", "", 1))
p("25 Underworld", T1, "serifbold", "#9DB4FF", "spiny/f=24/a=100/curve=easeout", "float/f=150/a=0.03/b=0.2",
  "zoomout/f=26/a=0.4/curve=easein", order="all", grad=("#6F93FF", "#B8C4D8", "", "", 1))
# ---------------------------------------------------------------- Partie 2
p("26 Malcolm in the Middle", T2, "marker", "#FFD400", "drop/f=16/a=3/b=15/dir=up/curve=elastic",
  "bop/f=36/a=0.02/b=0.9", "slide/f=8/a=3.5/dir=downright/curve=easein", order="random", delay=2.5,
  stroke=("#000000", 0.07), shadow=("#000000", 0.05, 0.06, 0, 1), mb=2)
p("27 Jackass", T2, "stencil", "#FFFFFF", "pop/f=3/a=3/curve=easeout", "jitter/f=2/a=0.006/b=5",
  "fall/f=4/a=3/curve=easein", order="all", grain=0.5, fps=12)
p("28 Pimp My Ride", T2, "rounded", "#FFD84A", "spinz/f=14/a=720/b=0.2", "pulse/f=30/a=0.05",
  "slide/f=6/a=3/dir=left", order="ltr", delay=2, mb=2, grad=("#FFE680", "#B8860B", "", "", 1),
  stroke=("#FFFFFF", 0.03), glow=("#FFD700", 0.06, 0.6))
p("29 Friends", T2, "hand", "#FFFFFF", "zoom/f=8/a=0.6/b=1/curve=easeout", "float/f=120/a=0.015/b=0",
  "fade/f=6", order="all", shadow=("#000000", 0.02, 0.03, 0.05, 0.6))
p("30 That 70s Show", T2, "rounded", "#FF9A1F", "spinz/f=16/a=40/b=0.05/curve=easeout", "wave/f=60/a=0.03/b=0.6",
  "zoomthrough/f=14/a=7", order="all", grad=("#FFB02E", "#B5541A", "", "", 1))
p("31 Skins", T2, "sans", "#FFFFFF", "slide/f=18/a=1.5/dir=left/curve=expoout", "breathe/f=200/a=-0.2/b=1",
  "burn/f=8/a=1/b=1", order="all", track=0.05)
p("32 Buffy", T2, "gothic", "#B8B8C0", "wipe/f=18/dir=right/curve=linear", "glowpulse/f=40/a=1.0",
  "rise/f=24/a=1.5", order="all", glow=("#A00000", 0.12, 0.2))
p("33 Charmed", T2, "serif", "#F3E2A6", "glow/f=24/a=3", "wave/f=70/a=0.02/b=0.4", "dissolve/f=22/b=0.03",
  order="all", glow=("#FFF2B0", 0.1, 0.5))
p("34 Clueless", T2, "script", "#FF6FB5", "spiny/f=12/a=90", "pulse/f=20/a=0.05", "slide/f=10/a=2/b=1/dir=up",
  delay=3, stroke=("#FFFFFF", 0.04))
p("35 Dawson's Creek", T2, "serif", "#FFFFFF", "fade/f=30/curve=easeinout", "float/f=200/a=0.012", "fade/f=24",
  delay=6, odelay=3, shadow=("#000000", 0, 0.03, 0.2, 0.7))
p("36 Fresh Prince", T2, "brush", "#FF2E9A", "wipe/f=16/dir=left", "pulse/f=22/a=0.06/b=1.7", "wipe/f=12/dir=left",
  delay=3, grad=("#FF2E9A", "#FFE600", "", "", 1), shadow=("#3A00FF", 0.04, 0.05, 0, 1))
p("37 Alias (spy)", T2, "sansbold", "#FF3030", "scramble/f=8", "scanline/f=45/a=0.9", "scatter/f=10/a=4/b=0",
  delay=2)
p("38 Smallville", T2, "serifbold", "#FFE27A", "pop/f=14/a=0.05/curve=backout", "sweep/f=70/a=0.8",
  "zoomthrough/f=12/a=6", order="all", mb=1.5, grad=("#FFE27A", "#C0392B", "", "", 1),
  stroke=("#5A1A00", 0.03))
p("39 The O.C.", T2, "sans", "#1A1A1A", "flash/f=14/a=1", "drift/f=300/a=0.05/dir=right", "fade/f=24",
  order="all", track=0.12)
p("40 MTV News", T2, "sansbold", "#FFFFFF", "strobe/f=10/a=0.2", "jitter/f=3/a=0.01", "squashx/f=4", order="all",
  plate=("#FFD400", 1, 0.12), grain=0.3, fps=12)
p("41 Sabrina", T2, "script", "#B26BFF", "pop/f=14/a=0.1/b=30/curve=elastic", "wobble/f=60/a=6/b=0.5",
  "rise/f=22/a=1.5", glow=("#FFEB6B", 0.08, 0.9))
p("42 Degrassi", T2, "hand", "#1A3EFF", "flash/f=6/a=1", "jitter/f=2/a=0.004/b=2", "slide/f=6/a=2.5/dir=up",
  order="all", plate=("#FFF04D", 0.85, 0.12))
p("43 One Tree Hill", T2, "serif", "#FFFFFF", "wipe/f=14/dir=left/curve=linear", "none", "fade/f=14/curve=linear",
  order="all")
p("44 Roswell", T2, "sans", "#D6E4EA", "dissolve/f=22", "glowpulse/f=40/a=1.2", "blur/f=18/b=0.4/dir=up",
  order="all", glow=("#BFFFF0", 0.07, 0.6))
p("45 Veronica Mars", T2, "typewriter", "#E8D18A", "drop/f=8/a=1.2/dir=up/curve=bounce", "none",
  "slide/f=7/a=3/dir=left", order="all", shadow=("#000000", 0.05, 0.05, 0, 1), grain=0.7, fps=12)
p("46 Scrubs", T2, "rounded", "#3FD6FF", "step", "pulse/f=3/a=0.02", "wipe/f=8/dir=right", delay=1)
p("47 Futurama", T2, "rounded", "#E8362B", "spinz/f=18/a=270/b=0.1/curve=backout", "float/f=100/a=0.02/b=0.1",
  "scatter/f=12/a=3/b=200", order="all", stroke=("#FFE14A", 0.04), shadow=("#000000", 0.05, 0.06, 0, 1))
p("48 King of the Hill", T2, "serifbold", "#B33A22", "slide/f=8/a=1/dir=down/curve=steps", "none",
  "fall/f=3/a=3/curve=linear", order="all", fps=12)
p("49 South Park", T2, "marker", "#FFFFFF", "step", "wobble/f=8/a=6/b=1.9", "spiny/f=6/a=90", order="random",
  delay=2, stroke=("#000000", 0.09), fps=8)
p("50 My Super Sweet 16", T2, "script", "#FF4FB3", "zoom/f=16/a=0.3/b=1", "sweep/f=40/a=1.0/b=0.08", "fade/f=14",
  order="all", grad=("#FF4FB3", "#FFD27A", "", "", 1), stroke=("#FFFFFF", 0.03), glow=("#FFD6F2", 0.08, 0.7))
# ---------------------------------------------------------------- Partie 3
p("51 Eternal Sunshine", T3, "typewriter", "#F2E8D2", "focus/f=14/a=2/b=0.5/dir=left", "jitter/f=2/a=0.004/b=0.3",
  "burn/f=22/a=2.5/b=1", order="center", delay=2, grain=0.3)
p("52 Magnolia", T3, "serif", "#FFFFFF", "fade/f=45/curve=easeinout", "slowzoom/f=300/a=0.04",
  "fade/f=24/curve=linear", order="all")
p("53 Donnie Darko", T3, "sans", "#FFFFFF", "wipe/f=20/dir=down/curve=linear", "breathe/f=300/a=0.2/b=1",
  "zoomout/f=40/a=0.02/curve=easein", order="all", track=0.1)
p("54 Lost in Translation", T3, "sans", "#FF8AD6", "fade/f=24", "glowpulse/f=70/a=0.6",
  "blur/f=24/a=0.25/b=0.25/dir=upleft", order="random", delay=4, glow=("#FF4FD0", 0.1, 0.9))
p("55 Garden State", T3, "hand", "#EAF1FF", "slide/f=20/a=0.8/b=1/dir=down/curve=easeout", "float/f=160/a=0.01",
  "slide/f=14/a=2/b=1/dir=right/curve=easeinout", delay=2)
p("56 Amelie", T3, "serif", "#E7B33A", "slide/f=12/a=0.5/dir=down/curve=elastic", "wobble/f=40/a=2/b=0.8",
  "rise/f=24/a=3", delay=3)
p("57 The Virgin Suicides", T3, "script", "#FFEFF2", "focus/f=30/a=1/b=0.25/dir=upright", "blurpulse/f=140/a=0.06/dir=up",
  "burn/f=24/a=0.5/b=0.9", order="all")
p("58 Trainspotting", T3, "impact", "#FFFFFF", "slide/f=5/a=3/dir=right/curve=linear", "none",
  "zoomthrough/f=2/a=10", order="all", plate=("#FF6A00", 1, 0.15), mb=2)
p("59 High Fidelity", T3, "serif", "#FFE9A0", "spinz/f=18/a=360/b=0.05/curve=easeout", "jitter/f=1/a=0.004/b=1.5",
  "slide/f=5/a=3/dir=left", order="all")
p("60 Run Lola Run", T3, "condensed", "#FF1A1A", "slide/f=6/a=4/dir=right/curve=linear", "flicker/f=6/a=0.7/c=1",
  "slide/f=5/a=4/dir=left/curve=linear", order="all", mb=2)
p("61 Memento Mori", T3, "roman", "#FFFFFF", "step", "none", "dissolve/f=20/b=0.2", delay=5, track=0.5)
p("62 Adaptation", T3, "typewriter", "#F5F5F0", "strobe/f=12/a=0.05", "jitter/f=4/a=0.005", "zoomout/f=14/a=0.05",
  order="random", delay=1)
p("63 Big Fish", T3, "serif", "#E8C26A", "spinx/f=20/a=88/curve=backout", "wave/f=90/a=0.025/b=0.5",
  "rise/f=26/a=2.5", order="all")
p("64 Punch-Drunk Love", T3, "sans", "#FFFFFF", "chroma/f=14/a=0.3", "chromajitter/f=2/a=0.03",
  "blur/f=16/a=0.6/dir=left", order="all", grad=("#FF3CAC", "#2B86C5", "#784BA0", "#FFD200", 1))
p("65 Ghost World", T3, "marker", "#111111", "step", "none", "slide/f=8/a=2.5/dir=left", order="all",
  plate=("#FFE61A", 1, 0.25))
p("66 Coffee and Cigarettes", T3, "sans", "#FFFFFF", "step", "none", "cut", order="oddeven", delay=4, odelay=0)
p("67 Science of Sleep", T3, "stencil", "#F4C27A", "step", "jitter/f=1/a=0.006/b=3", "fall/f=8/a=2/curve=easein",
  order="random", delay=3, odelay=3, fps=8)
p("68 Little Miss Sunshine", T3, "rounded", "#FFD21F", "slide/f=10/a=4/dir=left/curve=easeout", "jitter/f=1/a=0.003",
  "slide/f=8/a=4/dir=left", order="all", mb=2)
p("69 Marie Antoinette", T3, "script", "#FF4FD8", "dissolve/f=14/b=0.05", "blurpulse/f=30/a=0.02/dir=down",
  "fade/f=10", order="all", glow=("#FF4FD8", 0.08, 0.8))
p("70 Mysterious Skin", T3, "sans", "#D0D0D0", "glow/f=30/a=3", "float/f=200/a=0.012",
  "blur/f=30/a=0.5/dir=upleft", order="all", track=0.2)
p("71 Squid and the Whale", T3, "serif", "#FFFFFF", "fade/f=4", "none", "cut", order="all")
p("72 Brokeback Mountain", T3, "serif", "#FFFFFF", "fade/f=30/curve=easeinout", "slowzoom/f=400/a=0.03",
  "fade/f=24", order="all", shadow=("#000000", 0, 0.02, 0.1, 0.5))
p("73 21 Grams", T3, "sans", "#E5E5E0", "strobe/f=12/a=0.02", "jitter/f=2/a=0.003", "burn/f=2/a=0/b=1",
  order="all", grain=0.7)
p("74 Elephant", T3, "sans", "#FFFFFF", "fade/f=48/curve=linear", "slowzoom/f=300/a=0.05",
  "fade/f=48/curve=linear", order="all")
p("75 No Country for Old Men", T3, "serif", "#FFFFFF", "step", "none", "cut", order="all")
# ---------------------------------------------------------------- Partie 4
p("76 GoldenEye", T4, "serifbold", "#FFE9A0", "spiny/f=20/a=180/curve=easeout", "sweep/f=40/a=0.9/b=0.1",
  "scatter/f=16/a=3/b=360", order="all", grad=("#FFE9A0", "#A8741A", "", "", 1))
p("77 Armageddon", T4, "sansbold", "#C8D0DA", "pop/f=12/a=0.2/curve=backout", "slowzoom/f=100/a=0.06",
  "burn/f=10/a=2/b=1", order="all", mb=1.5, grad=("#C8D0DA", "#5A6470", "", "", 1), glow=("#FF8A2A", 0.1, 0.8))
p("78 Independence Day", T4, "sansbold", "#8EA6B8", "fade/f=24", "drift/f=200/a=0.04/dir=down",
  "zoomout/f=12/a=0.05/curve=easein", order="all", stroke=("#1D2A36", 0.03), shadow=("#000000", 0, 0.25, 0.1, 0.5))
p("79 Men in Black", T4, "rounded", "#D8DDE6", "flash/f=3/a=1", "scanline/f=60/a=0.8", "burn/f=3/a=0/b=1",
  order="all", glow=("#3CF0FF", 0.08, 0.8))
p("80 Gladiator", T4, "roman", "#E6C47A", "dissolve/f=26", "float/f=180/a=0.012", "dissolve/f=26", order="all",
  grad=("#E6C47A", "#8F6B2C", "", "", 1))
p("81 Minority Report", T4, "sans", "#7FD8FF", "chroma/f=12/a=0.1", "wave/f=40/a=0.008", "slide/f=6/a=3/dir=right",
  order="all", glow=("#3AA8FF", 0.08, 0.9))
p("82 Spider-Man", T4, "impact", "#E01B2A", "slide/f=12/a=3/dir=upleft/curve=elastic", "none",
  "zoomthrough/f=10/a=7", order="all", stroke=("#1E3FD8", 0.05))
p("83 The Mummy", T4, "gothic", "#E8C979", "dissolve/f=26/b=0.04", "glowpulse/f=50/a=0.8", "melt/f=14/a=1.2",
  order="all", grad=("#E8C979", "#8A6A2E", "", "", 1), glow=("#FFC857", 0.08, 0.5))
p("84 Jurassic Park", T4, "impact", "#F2B705", "drop/f=8/a=4/dir=up/curve=bounce", "none", "fall/f=10/a=3",
  order="all", stroke=("#8E1B10", 0.05))
p("85 Mission: Impossible", T4, "sansbold", "#6E7580", "wipe/f=10/dir=left/curve=linear", "pulse/f=5/a=0.03",
  "scatter/f=12/a=2/b=30", order="all", shadow=("#000000", 0.04, 0.06, 0.04, 0.9))
p("86 Godzilla", T4, "sansbold", "#5A6B63", "strobe/f=14/a=0.08", "jitter/f=1/a=0.012/b=0.8", "squashy/f=8",
  order="all", grain=0.4)
p("87 Bad Boys", T4, "condensed", "#E8EEF5", "slide/f=8/a=3/dir=alternate/curve=backout", "sweep/f=40/a=0.8/b=0.14",
  "slide/f=6/a=3/dir=up", delay=1.5, grad=("#E8EEF5", "#6D7C8A", "", "", 1), glow=("#FF8A2A", 0.08, 0.6), mb=1.5)
p("88 Tomb Raider", T4, "sansbold", "#B8C4CC", "wipe/f=12/dir=left", "sweep/f=60/a=0.7", "squashy/f=6", order="all",
  glow=("#00FFD5", 0.05, 0.5))
p("89 X-Men", T4, "sansbold", "#5C7CC8", "scramble/f=8", "sweep/f=60/a=0.9/b=0.1", "tear/f=12/a=1.2", scr=1,
  grad=("#5C7CC8", "#1B2A55", "", "", 1), stroke=("#C8D0DA", 0.02))
p("90 Charlie's Angels", T4, "rounded", "#FF2A9D", "pop/f=14/a=0/curve=elastic", "wobble/f=24/a=4/b=0",
  "slide/f=6/a=4/dir=right", order="all", stroke=("#FFFFFF", 0.07), glow=("#FFD6F2", 0.06, 0.6))
p("91 Starship Troopers", T4, "sansbold", "#9AA3AD", "drop/f=3/a=2/dir=up/curve=linear", "none",
  "slide/f=3/a=2/dir=up/curve=linear", order="all")
p("92 Total Recall", T4, "sansbold", "#FFFFFF", "chroma/f=14/a=0.35", "chromajitter/f=3/a=0.02", "chroma/f=10/a=0.4",
  order="all")
p("93 Speed", T4, "sansbold", "#FF2020", "slide/f=8/a=4/dir=right/curve=expoout", "jitter/f=1/a=0.004",
  "slide/f=4/a=5/dir=left", order="all", mb=2)
p("94 The Bourne Identity", T4, "sans", "#D0D6D2", "zoom/f=4/a=1.4/b=1", "drift/f=200/a=0.02/dir=right", "cut",
  order="all")
p("95 Cloverfield", T4, "sans", "#FFF3B0", "strobe/f=8/a=0.1", "jitter/f=1/a=0.012/b=1.5/c=1", "tear/f=8/a=0.8",
  order="all", grain=0.5)
p("96 Pitch Black", T4, "sans", "#101418", "glow/f=24/a=4", "glowpulse/f=60/a=0.8", "cut", order="all",
  glow=("#BFE9FF", 0.2, 1.2))
p("97 Tremors", T4, "stencil", "#C79A5A", "slide/f=8/a=3/dir=down/curve=easeout", "bop/f=6/a=0.01/c=1",
  "fall/f=10/a=4/curve=easein", order="all", stroke=("#5A3A1A", 0.03))
p("98 Air Force One", T4, "serifbold", "#D8DCE4", "spinx/f=14/a=80", "drift/f=300/a=0.04/dir=right",
  "slide/f=18/a=2/b=1/dir=right/curve=easeinout", delay=2, shadow=("#000000", 0, 0.03, 0.02, 0.9))
p("99 Face/Off", T4, "sansbold", "#DCEBFF", "spiny/f=8/a=180/curve=easeout", "sweep/f=40/a=0.8/b=0.12",
  "spiny/f=8/a=180", order="all", grad=("#DCEBFF", "#4A7BD8", "", "", 1))
# ---------------------------------------------------------------- TV, Teen, SF, Crime
S1, S2, S3, S4, S5 = "TV Sitcom & Teen", "TV Sci-Fi & Fantastique", "TV Crime & Action", "Anime & Cartoon", "TV Sitcom & Teen"
p("100 Everybody Hates Chris", S1, "sansbold", "#FFD400", "pop/f=4/a=2.5/curve=easeout", "none", "zoomout/f=5/a=0",
  delay=4, stroke=("#B01E00", 0.06))
p("101 My Wife and Kids", S1, "sans", "#FFFFFF", "slide/f=10/a=1.5/dir=alternate", "pulse/f=20/a=0.03",
  "dissolve/f=14/b=0.12", delay=2)
p("102 The Office", S1, "sans", "#3A3A3A", "slide/f=14/a=1/dir=left/curve=linear", "none",
  "slide/f=14/a=1/dir=right/curve=linear", order="all")
p("103 Arrested Development", S1, "serifbold", "#FFD21A", "step", "none", "cut", order="all")
p("104 King of Queens", S1, "sansbold", "#FFFFFF", "focus/f=10/a=1/b=0.3/dir=left", "drift/f=120/a=0.05/dir=right",
  "wipe/f=8/dir=left", order="all", stroke=("#123F9A", 0.04))
p("105 Boy Meets World", S1, "serifbold", "#F2E6C8", "spinz/f=18/a=20/b=0.2/curve=easeout", "wobble/f=70/a=3/b=0",
  "dissolve/f=12", order="all")
p("106 Kenan and Kel", S1, "marker", "#86FF2E", "drop/f=12/a=2/dir=up/curve=bounce", "pulse/f=10/a=0.06",
  "zoomthrough/f=8/a=8", delay=3, stroke=("#FF7A00", 0.06))
p("107 Full House", S1, "serif", "#FFF0B8", "slide/f=18/a=1.2/dir=down/curve=easeout", "wave/f=80/a=0.02",
  "slide/f=14/a=1.5/dir=up", shadow=("#000000", 0.03, 0.04, 0.1, 0.5))
p("108 Saved by the Bell", S1, "rounded", "#FF6EC7", "slide/f=8/a=3/dir=downleft/curve=easeout", "wobble/f=20/a=4/b=1",
  "scatter/f=10/a=4/b=90", grad=("#FF6EC7", "#4DEEEA", "", "", 1), stroke=("#000000", 0.04))
p("109 How I Met Your Mother", S1, "serif", "#FFFFFF", "pop/f=10/a=0.7/b=4/curve=backout", "none", "burn/f=6/a=0/b=1",
  order="all", plate=("#000000", 0.5, 0.15))
p("110 The Parkers", S1, "marker", "#FF9A1F", "slide/f=10/a=0.5/dir=left/curve=elastic", "pulse/f=14/a=0.05",
  "fall/f=10/a=3", stroke=("#000000", 0.05))
p("111 Home Improvement", S1, "stencil", "#D9D9D9", "wipe/f=10/dir=left/curve=steps", "jitter/f=1/a=0.006/b=0",
  "fall/f=10/a=3/curve=easein", order="all", stroke=("#222222", 0.03))
p("112 Reba", S1, "serif", "#FFFFFF", "zoom/f=12/a=1.3/b=1", "float/f=120/a=0.015", "slide/f=12/a=1.5/dir=right",
  order="all")
p("113 30 Rock", S1, "seriffine", "#FFD27A", "strobe/f=12/a=0.02", "glowpulse/f=20/a=0.6", "cut", delay=2,
  glow=("#FFB347", 0.08, 0.9))
p("114 Daria", S1, "sans", "#1A1A1A", "step", "none", "cut", delay=4, stroke=("#7A8B3C", 0.03))
p("115 Gossip Girl", S1, "seriffine", "#E8C877", "step", "none", "fade/f=4", delay=6, order="ltr")
p("116 The Real World", S1, "impact", "#FFFFFF", "strobe/f=12/a=0.15", "jitter/f=5/a=0.012/b=4", "dissolve/f=12/b=0.12",
  order="random", plate=("#000000", 0.8, 0.12))
p("117 Beavis and Butt-Head", S1, "marker", "#FFE44D", "slide/f=6/a=1.5/dir=left/curve=steps", "jitter/f=1/a=0.008/b=3",
  "fall/f=6/a=3", order="all", stroke=("#000000", 0.08), fps=8)
p("118 Celebrity Deathmatch", S1, "rounded", "#C65A2E", "drop/f=14/a=3/dir=up/curve=bounce", "pulse/f=26/a=0.05/b=0.8",
  "squashy/f=8", stroke=("#5A2210", 0.04))
p("119 Freaks and Geeks", S1, "sans", "#F4E9B2", "slide/f=40/a=0.3/dir=down/curve=linear", "none", "fade/f=30",
  order="all")
p("120 Gilmore Girls", S1, "serif", "#F0DDA0", "focus/f=20/a=1/b=0.2/dir=left", "float/f=130/a=0.012",
  "dissolve/f=20", order="all")
# --- Sci-Fi
p("121 Lost", S2, "sans", "#FFFFFF", "focus/f=90/a=2/b=0.5/dir=upleft/curve=easeinout", "blurpulse/f=240/a=0.04",
  "zoomthrough/f=60/a=2.5/curve=easeinout", order="all", track=0.1)
p("122 Stargate SG-1", S2, "roman", "#B7C4CF", "spinx/f=20/a=90", "wave/f=80/a=0.01", "zoomthrough/f=10/a=12",
  order="all", glow=("#5AA8FF", 0.05, 0.5))
p("123 Fringe", S2, "sans", "#D8F6FF", "dissolve/f=20/b=0.03", "sweep/f=60/a=0.7", "dissolve/f=14/b=0.02",
  order="all", glow=("#6AD8FF", 0.05, 0.6))
p("124 Heroes", S2, "script", "#FFE9B0", "glow/f=30/a=5", "float/f=150/a=0.02", "fade/f=24", order="all",
  glow=("#FFD27A", 0.2, 1.4))
p("125 Firefly", S2, "stencil", "#D9A066", "slide/f=14/a=2/dir=right/curve=easeout", "jitter/f=1/a=0.003",
  "slide/f=8/a=3/dir=left", order="all", grain=0.3)
p("126 Battlestar Galactica", S2, "condensed", "#AEB4B8", "slide/f=10/a=1.5/dir=right/curve=steps", "jitter/f=1/a=0.003",
  "slide/f=6/a=4/dir=left", delay=1, mb=2)
p("127 Farscape", S2, "sansbold", "#FFFFFF", "chroma/f=16/a=0.4", "wave/f=70/a=0.02", "zoomout/f=14/a=0", order="all")
p("128 Twin Peaks", S2, "serifbold", "#8B5A2B", "fade/f=90/curve=linear", "float/f=200/a=0.006", "fade/f=90/curve=linear",
  order="all", stroke=("#39FF14", 0.015), glow=("#39FF14", 0.04, 0.7))
p("129 Sliders", S2, "rounded", "#8A6BFF", "spinz/f=26/a=720/b=0.02", "breathe/f=60/a=0.08", "zoomout/f=14/a=0",
  order="all", glow=("#4DA8FF", 0.06, 0.8))
p("130 Doctor Who", S2, "sansbold", "#E8E8F0", "spinz/f=22/a=30/b=0.1", "wobble/f=60/a=2", "zoomthrough/f=10/a=9",
  order="all", stroke=("#274B8A", 0.03))
p("131 Quantum Leap", S2, "mono", "#8FE2FF", "flash/f=8/a=1", "flicker/f=2/a=0.3", "burn/f=4/a=1/b=1", order="all",
  glow=("#3AA8FF", 0.06, 0.7))
p("132 Andromeda", S2, "roman", "#E8D8A0", "wipe/f=14/dir=left", "sweep/f=80/a=0.7", "dissolve/f=16", order="all")
p("133 Dark Angel", S2, "condensed", "#3EDAFF", "strobe/f=12/a=0.12", "flicker/f=3/a=0.35", "fall/f=6/a=3", delay=2)
p("134 The Pretender", S2, "sans", "#E6F2FF", "scramble/f=6", "sweep/f=90/a=0.5", "wipe/f=6/dir=right", delay=1.5)
p("135 Kyle XY", S2, "sans", "#C6FF9A", "focus/f=14/a=1/b=0.25/dir=upleft", "glowpulse/f=40/a=0.7",
  "blur/f=14/a=0.3/dir=upleft", order="center", delay=1, glow=("#8CFF4F", 0.08, 0.6))
# --- Crime / Action TV
p("136 24", S3, "mono", "#FFB000", "step", "flicker/f=12/a=0.8/c=1", "cut", delay=2)
p("137 CSI", S3, "sans", "#FFFFFF", "flash/f=6/a=1", "sweep/f=50/a=0.8", "burn/f=4/a=0/b=1", order="all",
  glow=("#19D2FF", 0.08, 0.8))
p("138 The Wire", S3, "serif", "#D8D2C0", "slide/f=36/a=0.6/b=1/dir=left/curve=linear", "none", "fade/f=24",
  order="all", grain=0.6)
p("139 The Sopranos", S3, "serifbold", "#F2F2F2", "slide/f=14/a=2/dir=right/curve=linear", "none", "cut", order="all")
p("140 Prison Break", S3, "stencil", "#9AA0A6", "wipe/f=12/dir=down", "jitter/f=2/a=0.008", "fall/f=8/a=3",
  order="all", grain=0.4)
p("141 Dexter", S3, "serif", "#FFFFFF", "wipe/f=14/dir=up/curve=easein", "pulse/f=36/a=0.03", "blur/f=14/a=0.4/dir=left",
  order="all", stroke=("#B30000", 0.015), glow=("#B30000", 0.04, 0.7))
p("142 The Shield", S3, "stencil", "#FFD400", "slide/f=5/a=3/dir=right/curve=easeout", "jitter/f=1/a=0.008/b=1",
  "slide/f=5/a=3/dir=left", order="all", stroke=("#000000", 0.06), mb=2)
p("143 NCIS", S3, "sansbold", "#FFFFFF", "step", "none", "zoomthrough/f=2/a=10", order="all", fps=8)
p("144 Law and Order", S3, "serif", "#FFFFFF", "slide/f=10/a=2/dir=left", "none", "fade/f=6", order="all",
  stroke=("#4A3CFF", 0.012), glow=("#5A4BFF", 0.04, 0.5))
p("145 Criminal Minds", S3, "typewriter", "#E8E0CC", "focus/f=14/a=1/b=0.3/dir=upleft", "jitter/f=2/a=0.004",
  "fade/f=24", delay=1, grain=0.5)
p("146 Monk", S3, "sans", "#8A7F6A", "slide/f=12/a=1.2/dir=left/curve=linear", "none",
  "slide/f=12/a=1.2/dir=right/curve=linear", order="all")
p("147 Cold Case", S3, "hand", "#D0E2F5", "focus/f=30/a=1/b=0.4/dir=upleft/curve=easeinout", "flicker/f=6/a=0.15/c=1",
  "blur/f=28/a=0.4/dir=upleft", order="all")
p("148 Miami Vice", S3, "sansbold", "#FF4FA8", "slide/f=10/a=3/dir=downleft", "wave/f=50/a=0.02",
  "slide/f=8/a=3/dir=right", order="all", grad=("#FF4FA8", "#3CE6FF", "", "", 1), shadow=("#3A00A0", 0.04, 0.05, 0, 0.9))
p("149 Walker, Texas Ranger", S3, "serifbold", "#FFE08A", "pop/f=8/a=1.6/curve=backout", "sweep/f=50/a=0.8",
  "zoomout/f=8/a=0.1", order="all", grad=("#FFE08A", "#8A5A12", "", "", 1))
p("150 Justified", S3, "serifbold", "#1A1308", "dissolve/f=16", "none", "fall/f=6/a=3", order="all",
  plate=("#D8C494", 0.9, 0.2))
p("151 Bones", S3, "sans", "#4AA8FF", "glow/f=18/a=3", "flicker/f=3/a=0.2", "dissolve/f=16", order="all",
  glow=("#3A7CFF", 0.08, 0.9))
p("152 Burn Notice", S3, "sansbold", "#FF8A1A", "flash/f=6/a=1", "sweep/f=60/a=0.7", "burn/f=8/a=2/b=0.6", order="all")
p("153 Numb3rs", S3, "mono", "#E8FFF2", "scramble/f=12", "flicker/f=3/a=0.15", "dissolve/f=18/b=0.04",
  order="random", delay=1.5)
p("154 La Femme Nikita", S3, "sans", "#E8E8F0", "slide/f=14/a=1.4/dir=down/curve=easeout", "flicker/f=10/a=0.3/c=1",
  "burn/f=4/a=0/b=1", order="all", mb=2)
# --- Anime & Cartoons
p("155 Evangelion", S4, "serifbold", "#FFFFFF", "step", "none", "cut", order="all", plate=("#000000", 1, 0.5))
p("156 Cowboy Bebop", S4, "sansbold", "#FFFFFF", "slide/f=10/a=1.5/dir=left/curve=easeout", "wobble/f=70/a=2",
  "slide/f=6/a=3/dir=left", delay=3, plate=("#1E50D8", 1, 0.18))
p("157 Dragon Ball Z", S4, "impact", "#FFF176", "spinz/f=14/a=720/b=0.1", "jitter/f=1/a=0.012/b=1",
  "scatter/f=12/a=4/b=0", order="all", grad=("#FFF176", "#FF8A00", "", "", 1), stroke=("#C1121F", 0.07),
  glow=("#FFE600", 0.12, 0.9))
p("158 Pokemon", S4, "marker", "#FFCB05", "pop/f=12/a=0/curve=elastic", "pulse/f=18/a=0.04/b=0.5",
  "zoomthrough/f=8/a=8", delay=2, stroke=("#2A62B6", 0.08))
p("159 Yu-Gi-Oh!", S4, "roman", "#FFE9A0", "spiny/f=14/a=180/curve=easeout", "sweep/f=40/a=0.9", "dissolve/f=16",
  order="all", grad=("#FFE9A0", "#B07A14", "", "", 1), glow=("#FFD27A", 0.1, 0.8))
p("160 Sailor Moon", S4, "script", "#FFB6E1", "spinz/f=20/a=360/b=0.05", "pulse/f=24/a=0.05/b=0.6", "fade/f=14",
  glow=("#FFE6A8", 0.1, 0.8))
p("161 Gundam Wing", S4, "condensed", "#C8CDD3", "slide/f=8/a=2/dir=alternate/curve=easeout", "sweep/f=50",
  "zoomthrough/f=6/a=12", delay=3, stroke=("#FFD400", 0.02))
p("162 Naruto", S4, "brush", "#FF7A1A", "dissolve/f=6", "jitter/f=1/a=0.006/b=0", "blur/f=6/a=0.8/dir=left",
  delay=1, stroke=("#7A1A00", 0.05), mb=1.5)
p("163 Bleach", S4, "gothic", "#FFFFFF", "split/f=6/a=1.2/dir=left", "none", "dissolve/f=16", order="all",
  stroke=("#000000", 0.04))
p("164 One Piece", S4, "brush", "#F4E3B2", "slide/f=16/a=2/dir=left/curve=elastic", "wave/f=50/a=0.05/b=0.5",
  "rise/f=14/a=3", delay=2, stroke=("#A61E1E", 0.05))
p("165 Samurai Champloo", S4, "brush", "#FFFFFF", "spinz/f=8/a=720/b=0.05", "jitter/f=2/a=0.005/b=1",
  "wipe/f=10/dir=right", order="all", stroke=("#111111", 0.04), fps=12)
p("166 Fullmetal Alchemist", S4, "roman", "#E6EEF5", "slide/f=24/a=1/b=1/dir=down", "glowpulse/f=40/a=0.8",
  "dissolve/f=16/b=0.03", order="all", grad=("#E6EEF5", "#7A8A9A", "", "", 1), glow=("#4AA8FF", 0.08, 0.5))
p("167 Death Note", S4, "gothic", "#F2F2F2", "step", "flicker/f=40/a=0.5/c=1", "fade/f=24", delay=2)
p("168 Code Geass", S4, "gothic", "#FF3040", "zoom/f=12/a=3/b=1", "float/f=100/a=0.02", "scatter/f=14/a=2/b=200",
  order="all", grad=("#FF3040", "#FFD27A", "", "", 1))
p("169 The Powerpuff Girls", S4, "marker", "#FF6FB5", "zoom/f=12/a=0.1/b=1/curve=elastic", "pulse/f=14/a=0.05",
  "rise/f=8/a=6/curve=easein", order="all", stroke=("#000000", 0.08), shadow=("#000000", 0.06, 0.08, 0, 1))
p("170 Dexter's Laboratory", S4, "rounded", "#8FFF3A", "slide/f=8/a=1/dir=up/curve=steps", "flicker/f=3/a=0.3",
  "fall/f=8/a=3", delay=3, stroke=("#1A3A00", 0.04))
p("171 Courage the Cowardly Dog", S4, "marker", "#8E6BD8", "stretch/f=10/a=1.4/b=0.7/curve=elastic",
  "jitter/f=1/a=0.012/b=2", "zoomout/f=10/a=0", stroke=("#1E0F3A", 0.05))
p("172 Samurai Jack", S4, "sansbold", "#FFFFFF", "step", "none", "wipe/f=3/dir=right/curve=linear", order="all",
  plate=("#D0161B", 1, 0.2))
p("173 Avatar: The Last Airbender", S4, "roman", "#A8D8B8", "dissolve/f=22/b=0.02", "float/f=150/a=0.02",
  "dissolve/f=22", order="all")
p("174 Teen Titans", S4, "marker", "#FFD400", "slide/f=8/a=3/dir=downleft/curve=easeout", "glowpulse/f=20/a=0.8",
  "slide/f=6/a=3/dir=upright", order="all", stroke=("#3A1A7A", 0.07), glow=("#B060FF", 0.08, 0.7))

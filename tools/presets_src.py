# Source de vérité des presets (240 fiches). `python3 tools/gen_presets.py` -> src/TitlePresetsData.inc
PRESETS = []
REF = 120.0          # px de référence : 1 em = 120 px à 1080p
def E(px): return round(px / REF, 4)
def A(kind, f=None, a=None, b=None, c=None, dir=None, curve=None):
    s = kind
    for k, v in (("f", f), ("a", a), ("b", b), ("c", c), ("dir", dir), ("curve", curve)):
        if v is not None: s += f"/{k}={v}"
    return s

# polices (alternatives séparées par ~ : première trouvée sur le Mac)
F = dict(
  futcond="Avenir Next Condensed~Futura", fut="Futura", helv="Helvetica Neue~Arial", helvl="Helvetica Neue~Arial",
  black="Arial Black~Helvetica Neue", impact="Impact~Arial Black", courier="Courier New", courierb="Courier New Bold~Courier New",
  typew="American Typewriter~Courier New", bodoni="Bodoni 72~Didot~Georgia", didot="Didot~Bodoni 72~Georgia",
  trajan="Copperplate~Optima~Baskerville", euro="DIN Alternate~Avenir Next Condensed~Arial Black",
  caslon="Big Caslon~Baskerville~Georgia", garamond="Hoefler Text~Baskerville~Georgia", georgia="Georgia",
  georgiab="Georgia Bold~Georgia", rock="Rockwell~Georgia Bold", mono="Menlo~Monaco~Courier New", brush="Brush Script~Snell Roundhand",
  papyrus="Papyrus~Luminari", gill="Gill Sans~Optima", comic="Comic Sans MS Bold~Marker Felt", frank="Avenir Next Condensed~Impact",
  goth="Luminari~Papyrus", chalk="Chalkduster~Noteworthy", snell="Snell Roundhand~Brush Script", marker="Marker Felt~Comic Sans MS Bold",
  optima="Optima~Gill Sans", times="Times New Roman~Georgia", timesb="Times New Roman Bold~Georgia Bold", rounded="Arial Rounded MT Bold~Arial Black",
)
CLS = dict(futcond="condensed", fut="sans", helv="sans", helvl="sans", black="sansbold", impact="impact", courier="typewriter", courierb="typewriter",
  typew="typewriter", bodoni="seriffine", didot="seriffine", trajan="roman", euro="sansbold", caslon="serif", garamond="serif", georgia="serif",
  georgiab="serifbold", rock="serifbold", mono="mono", brush="brush", papyrus="gothic", gill="sans", comic="marker", frank="condensed", goth="gothic",
  chalk="chalk", snell="script", marker="marker", optima="roman", times="serif", timesb="serifbold", rounded="rounded")

def hx(r, g, b): return "#%02X%02X%02X" % (r, g, b)

def p(name, f, fill, i="none", d="none", o="none", order="ltr", delay=2, odelay=1, track=0, fps=0, mb=1,
      stroke=None, shadow=None, glow=None, grad=None, plate=None, grain=0, tex=None, bevel=0, chroma=0,
      vig=None, grade=None, scan=None, grainall=0, part=None, fog=None, flare=None, scratch=0,
      ital=0, bold=0, upper=0, anchor="letter", outrev=0, flashc=None, scr=0, blend="normal"):
    fields = [name, f"font={CLS[f]}", f"fn={F[f]}", f"fill={fill}", f"order={order}", f"delay={delay}", f"odelay={odelay}",
              f"in={i}", f"idle={d}", f"out={o}"]
    def add(k, v): fields.append(f"{k}={v}")
    if stroke: add("stroke", "%s/%s" % stroke)
    if shadow: add("shadow", "%s/%s/%s/%s/%s" % shadow)
    if glow: add("glow", "%s/%s/%s" % glow)
    if grad: add("grad", "/".join(str(x) for x in grad))
    if plate: add("plate", "%s/%s/%s" % plate)
    if grain: add("grain", grain)
    if tex: add("tex", "%s/%s/%s" % tex)
    if bevel: add("bevel", bevel)
    if chroma: add("chroma", chroma)
    if vig: add("vig", "%s/%s" % vig)
    if grade: add("grade", "/".join(str(x) for x in grade))
    if scan: add("scan", "%s/%s" % scan)
    if grainall: add("grainall", grainall)
    if part: add("part", "/".join(str(x) for x in part))
    if fog: add("fog", "%s/%s" % fog)
    if flare: add("flare", "%s/%s" % flare)
    if scratch: add("scratch", scratch)
    if ital: add("ital", ital)
    if bold: add("bold", bold)
    if upper: add("upper", 1)
    if track: add("track", track)
    if fps: add("fps", fps)
    if mb != 1: add("mb", mb)
    if anchor != "letter": add("anchor", anchor)
    if outrev: add("outrev", 1)
    if flashc: add("flashc", flashc)
    if scr: add("scr", scr)
    if blend != "normal": add("blend", blend)
    PRESETS.append("|".join(fields))

# --- étalonnages récurrents : (saturation, contraste, quantité, ombres, hautes lumières)
SEPIA = (0.3, 1.12, 0.85, "#2B1D0C", "#D9C39A")
WARM = (0.9, 1.05, 0.6, "#3B2211", "#F4D09C")
COLD = (0.55, 1.1, 0.7, "#0B1620", "#B9CBD8")
NOIR = (0.0, 1.35, 0.9, "#000000", "#FFFFFF")
TEAL = (0.95, 1.15, 0.8, "#06303A", "#FFB070")
INDIGO = (0.8, 1.1, 0.8, "#0A0A3A", "#B7B0E8")
GREENGRAY = (0.45, 1.1, 0.8, "#10170F", "#C9D3BC")
AMBER = (0.9, 1.1, 0.7, "#2A1500", "#FFCB7A")
DESERT = (0.8, 1.12, 0.8, "#3A1E0A", "#F5D6A2")

# ------------------------------------------------------------------ 001 - 020
p("001 Breaking Bad", "futcond", "#FFFFFF", A("slide", 16, E(200), 0, dir="left", curve="easeoutcubic"),
  A("glowpulse", 48, 0.8), A("blur", 10, 0.125, 0.125, dir="upleft"), order="all", bold=0.02, plate=(hx(19, 77, 46), 0.85, 0.22),
  glow=("#39FF14", 0.04, 0.0), fog=(0.18, "#39FF14"), tex=("fine grain", 0.3, 1), chroma=0.02, upper=1)
p("002 Better Call Saul", "brush", "#FFE600", A("slide", 8, E(1920), 0, dir="right", curve="steps"), A("jitter", 1, E(2), 0),
  A("slide", 4, E(1920), 0, dir="left", curve="linear"), order="all", fps=8, stroke=(hx(139, 0, 0), 0.017), chroma=0.08, grain=0.5,
  scan=(0.1, 3), grainall=0.15)
p("003 Malcolm", "impact", "#FFCC00", A("drop", 14, E(800) / 4, 15, dir="up", curve="elastic"), A("wave", 8, E(4), 0.5), A("slide", 6, E(1500), 0, dir="right", curve="easeinquad"),
  delay=2, fps=12, stroke=("#000000", 0.07), shadow=("#000000", 0.083, 0.083, 0, 1), mb=2)
p("004 Se7en", "courierb", "#E0E0E0", A("strobe", 6, 0.2), A("breathe", 48, 0.25, 0), A("tear", 3, E(500) / 1, curve="linear"), order="all", ital=0.2,
  scratch=0.8, grain=0.5, grainall=0.2, grade=NOIR)
p("005 The O.C.", "helvl", "#FFFFFF", A("exposure", 20, 4, 1.05, curve="easeoutquad"), A("slowzoom", 240, 0.03), A("whiteout", 12, 1), order="all", track=0.4,
  glow=("#FFFFFF", 0.1, 0.4), flashc="#FFFFFF")
p("006 Breaking Bad (Periodic)", "futcond", "#FFFFFF", A("slide", 16, E(250), 0, dir="left", curve="easeoutcubic"), A("glowpulse", 48, 0.8),
  A("blur", 10, 0.125, 0.125, dir="upleft"), order="all", plate=(hx(19, 77, 46), 0.85, 0.2), glow=("#39FF14", 0.05, 0.3), fog=(0.2, "#39FF14"), chroma=0.025, upper=1)
p("007 Better Call Saul (VHS)", "brush", "#FFE600", A("slide", 8, E(1920), 0, dir="right", curve="steps"), A("jitter", 1, E(2), 0),
  A("stretch", 4, 2.2, 0.9), order="all", fps=8, stroke=(hx(139, 0, 0), 0.017), chroma=0.1, grain=0.5, scan=(0.12, 2), scratch=0.4)
p("008 The Wire", "courierb", "#D2D2D2", A("slide", 24, E(300), 0, dir="left", curve="linear"), A("drift", 240, 0.0017, 0, dir="right"), A("dissolve", 12, 0, 0.03),
  order="all", scan=(0.25, 2), grade=SEPIA, tex=("fine grain", 0.5, 1), grainall=0.3, vig=(0.4, 0.6))
p("009 Succession", "bodoni", "#F0F0F0", A("zoom", 20, 1.02, 1, curve="easeoutexpo"), "none", A("fade", 10, curve="linear"), order="center", delay=1,
  track=0.2, vig=(0.5, 0.55), upper=1)
p("010 The Sopranos", "timesb", "#FFFFFF", A("slide", 12, E(400), 0, dir="right", curve="easeoutquad"), A("jitter", 1, E(0.5), 0, 1), "cut", order="all",
  ital=0.2, stroke=("#000000", 0.025), shadow=("#000000", 0.067, 0.067, 0, 1), grade=(0.9, 1.5, 0.8, "#000000", "#FFFFFF"))
p("011 Mad Men", "black", "#FFFFFF", A("slide", 18, E(300), 0, dir="up", curve="easeoutexpo"), "none", A("slide", 10, E(40), 1, dir="up", curve="linear"),
  order="all", track=-0.05, shadow=("#000000", 0, 0.125, 0, 1), grade=WARM, vig=(0.3, 0.6))
p("012 True Detective", "frank", "#FFFFFF", A("fade", 30, curve="easeinoutquad"), A("flicker", 96, 0.2, 0, 1), A("fade", 15, curve="linear"), order="all",
  track=0.25, vig=(0.9, 0.5), grade=(0.55, 1.2, 0.8, "#05080A", "#C8D6C0"), fog=(0.18, "#A8B8A0"), tex=("paper", 0.25, 1))
p("013 Fargo", "courierb", "#141414", A("slide", 16, E(500), 0, dir="left", curve="fastlinear"), A("jitter", 1, E(1), 0), A("wipe", 8, dir="right", curve="linear"),
  order="all", part=("snow", 0.9, 1, 1, "#FFFFFF"), grade=(0.4, 1.0, 0.6, "#C8D0D8", "#FFFFFF"), vig=(0.2, 0.7))
p("014 Peaky Blinders", "rock", "#F0E6D2", A("zoom", 14, 0.2, 1, curve="easeoutquad"), A("float", 20, 0.017, 0.2), A("dissolve", 10, 0, 0.04), order="center", delay=1,
  track=0.1, stroke=("#000000", 0.04), fps=12, part=("ash", 0.8, 1, 1, "#1A1A1A"), grade=(0.35, 1.3, 0.85, "#0A0805", "#D8C7A8"), vig=(0.6, 0.5))
p("015 Dexter", "impact", "#FFFFFF", A("drop", 10, E(400) / 4, 0, dir="up", curve="easeinquad"), A("pulse", 24, 0.015), A("slide", 8, E(100), 0, dir="right", curve="easeinquad"),
  order="all", tex=("brushed metal", 0.0, 1), stroke=("#C70000", 0.02), glow=("#C70000", 0.05, 0.8), vig=(0.5, 0.6), grade=(1.0, 1.1, 0.4, "#2A0000", "#FFFFFF"))
p("016 Stranger Things", "frank", "#CC0000", A("stretch", 24, 0.6, 1.5, curve="easeoutquad"), A("flicker", 4, 0.3), A("flicker", 6, 0.8), order="center", delay=2,
  stroke=("#000000", 0.06), glow=("#FF3A1A", 0.375, 2.5), bevel=0.4, scan=(0.25, 3), upper=1, track=0.04)
p("017 Game of Thrones", "trajan", "#D2D7DC", A("spinx", 30, 45, 0, curve="easeoutexpo"), "none", A("fall", 15, 2.2, 25, curve="easeinquad"), order="random", delay=1,
  track=0.35, bold=0.02, bevel=0.6, tex=("rust", 0.35, 1), grad=("#EDEFF2", "#8A9099", "", "", 1), stroke=("#000000", 0.012), upper=1)
p("018 Westworld", "fut", "#FFFFFF", A("wipe", 20, dir="up", curve="linear"), A("glowpulse", 40, 0.6), A("wipe", 12, dir="down", curve="linear"), order="all",
  track=0.15, glow=("#FFFFFF", 0.08, 0.5), vig=(0.3, 0.6))
p("019 The Mandalorian", "euro", "#C8CDD2", A("slide", 14, E(600), 0, dir="right", curve="easeoutcubic"), A("jitter", 1, E(0.4), 0, 1), A("slide", 8, E(600), 1, dir="left", curve="easeinquad"),
  order="all", track=0.2, tex=("brushed metal", 0.55, 1), bevel=0.45, grade=DESERT, scratch=0.2)
p("020 The Last of Us (Fungal)", "helv", "#B4BEA0", A("dissolve", 28, 0, 0.02), A("pulse", 80, 0.01), A("dissolve", 14, 0, 0.06), order="random", delay=2, bold=0.03,
  stroke=("#2A2A1A", 0.03), tex=("rust", 0.5, 1.4), part=("spores", 0.9, 1, 0.6, "#C8E070"), grade=(0.6, 1.1, 0.7, "#10140A", "#C8D090"), vig=(0.5, 0.55))
# ------------------------------------------------------------------ 021 - 040
p("021 Arcane", "trajan", "#E6F5FF", A("slide", 22, E(40), 0, dir="down", curve="easeoutexpo"), A("float", 48, 0.02, 0.3), A("dissolve", 12, 0, 0.0), delay=1,
  glow=("#2DE0FF", 0.25, 2.0), grad=("#E6F5FF", "#F0C878", "", "", 0.6), tex=("oil impasto", 0.35, 1), part=("sparkle", 0.5, 0.8, 1, "#7FE8FF"), bevel=0.3, upper=1)
p("022 Cyberpunk: Edgerunners", "euro", "#FF007F", A("strobe", 6, 0.15), A("jitter", 2, E(4), 0), A("cut"), order="random", delay=0, fps=12, bold=0.03,
  stroke=("#000000", 0.07), chroma=0.12, part=("static", 0.3, 1, 1, "#FFFFFF"), scan=(0.15, 3), glow=("#FFE600", 0.1, 0.5), upper=1)
p("023 Spider-Verse", "comic", "#FFFFFF", A("pop", 8, 1.2, 0, curve="easeoutexpo"), A("jitter", 1, E(2) / 2, 0), A("stretch", 5, 1.5, 1.3), order="all", fps=12, bold=0.03,
  stroke=("#000000", 0.06), shadow=("#00E5FF", 0.07, 0.07, 0, 1), chroma=0.07, tex=("halftone", 0.35, 1))
p("024 The Bear", "helv", "#FFFFFF", A("strobe", 8, 0), A("pulse", 6, 0.008), A("cut"), order="all", track=-0.03, bold=0.04, grainall=0.4, grain=0.2, flare=(0.2, "#FFFFFF"),
  grade=(0.8, 1.3, 0.7, "#000000", "#FFFFFF"), flashc="#FFFFFF")
p("025 Succession (Dynamic)", "garamond", "#F5F0E6", A("slide", 24, E(150) / 2, 1, dir="down", curve="easeoutquad"), A("jitter", 1, E(1.2), 0.3, 1), A("fade", 18, curve="linear"),
  order="center", delay=2, ital=0.2, shadow=("#000000", 0, 0.04, 0.12, 0.6), grade=AMBER, grainall=0.45, scratch=0.5, vig=(0.4, 0.6))
p("026 Euphoria", "courierb", "#FFFFFF", A("focus", 24, 1.1, 0.33, dir="upleft", curve="easeoutexpo"), A("glowpulse", 72, 0.8), A("blur", 15, 0.3, 0.3, dir="upleft"), order="random", delay=2,
  stroke=("#FF2DCB", 0.025), glow=("#B13BFF", 0.4, 2.2), part=("sparkle", 0.9, 0.7, 0.6, "#FFFFFF"), grade=(1.1, 1.05, 0.5, "#1A0040", "#FF9BE8"))
p("027 Severance", "helvl", "#DCE6E1", A("slide", 16, E(200), 0, dir="left", curve="linear"), "none", A("cut"), order="all", track=0.25,
  scan=(0.15, 4), grade=(0.7, 1.0, 0.6, "#001A18", "#C8E8E0"))
p("028 The White Lotus", "didot", "#EBD7AF", A("spread", 30, 0.4, 1, curve="easeoutquad"), A("float", 48, 0.007, 0.0), A("dissolve", 18, 0, 0.02), order="center", delay=2, ital=0.2, track=0.1,
  grad=("#F6E6BE", "#C2A25E", "", "", 1), shadow=("#2A4A2A", 0.0, 0.04, 0.12, 0.5), part=("dust", 0.6, 1, 0.5, "#FFE9A0"), flare=(0.15, "#FFE9A0"))
p("029 Andor", "euro", "#F0F0F0", A("slide", 14, E(300), 0, dir="down", curve="easeoutexpo"), "none", A("slide", 10, E(300), 0, dir="down", curve="linear"), order="all", track=-0.02,
  bold=0.03, stroke=("#000000", 0.03), tex=("concrete", 0.55, 1), grade=NOIR)
p("030 Beef", "impact", "#FFFFFF", A("strobe", 8, 0.1), A("jitter", 1, E(2), 6), A("tear", 4, 0.5), order="random", delay=1, fps=12, stroke=("#C00000", 0.06),
  shadow=("#000000", 0.05, 0.06, 0, 1), tex=("paper", 0.5, 1.4), grain=0.3)
p("031 The Last of Us (Prestige)", "helv", "#EBEBEB", A("zoom", 24, 1.04, 2, curve="easeoutquad"), A("slowzoom", 96, 0.002), A("fade", 14, curve="linear"), order="all", track=0.3,
  shadow=("#000000", 0, 0.03, 0.12, 0.5), tex=("concrete", 0.3, 1), vig=(0.7, 0.5), grade=GREENGRAY)
p("032 Shogun", "brush", "#141414", A("wipe", 16, dir="left", curve="easeoutexpo"), "none", A("dissolve", 10, 0, 0.0), delay=2, track=-0.05, bold=0.015,
  tex=("paper", 0.4, 1), grade=(0.9, 1.0, 0.5, "#EADBB8", "#F4E8C8"), plate=("#E9DDB8", 0.9, 0.4), glow=("#7A1A1A", 0.015, 0.0))
p("033 Oppenheimer", "courierb", "#F5F5F5", A("scramble", 12), A("jitter", 1, E(0.8), 0), A("whiteout", 4, 1), order="random", delay=1, glow=("#FFB347", 0.15, 1.2),
  scr=0, grainall=0.55, grain=0.3, part=("embers", 0.6, 0.8, 1, "#FFB347"), grade=NOIR, flashc="#FFF2D8")
p("034 Poor Things", "bodoni", "#FAF0E6", A("pop", 20, 0.3, 0, curve="easeoutback"), A("pulse", 20, 0.02), A("stretch", 10, 0.1, 0.1), order="center", delay=1, track=0.2, ital=0.2,
  stroke=("#2A1A1A", 0.03), chroma=0.1, fps=12, vig=(0.8, 0.5), tex=("paper", 0.3, 1), grade=(0.2, 1.2, 0.9, "#1A1208", "#E8E0D0"))
p("035 Barbie", "fut", "#FF1493", A("slide", 15, E(400), 1, dir="down", curve="easeoutback"), A("float", 16, E(6) / 1, 0.0), A("zoomout", 6, 0, curve="easeinquad"), delay=2, ital=0.2, bold=0.05,
  stroke=("#FFFFFF", 0.07), shadow=("#FF1493", 0.05, 0.08, 0.3, 0.9), bevel=0.7, tex=("none", 0, 1), part=("sparkle", 0.4, 0.8, 1, "#FFFFFF"))
p("036 Dune", "fut", "#E1CDAA", A("spread", 36, 0.8, 1, curve="easeoutexpo"), A("none"), A("dissolve", 20, 0, 0.03), order="center", delay=3, track=0.4, bold=0.02,
  tex=("sand", 0.5, 1), part=("dust", 1.2, 1, 1.5, "#E8C088"), grade=DESERT, flare=(0.1, "#FFD9A0"))
p("037 Blade Runner 2049", "euro", "#FF8C00", A("slide", 24, E(250), 0, dir="up", curve="easeoutquad"), A("glowpulse", 96, 0.7), A("fade", 16), order="all", track=0.3, bold=0.03,
  glow=("#FF8C00", 0.5, 2.5), fog=(0.3, "#FF8C00"), part=("ash", 0.7, 1, 0.8, "#E8A060"), flare=(0.3, "#FFB060"), upper=1)
p("038 Everything Everywhere", "comic", "#FF00FF", A("strobe", 10, 0.2), A("jitter", 2, E(5), 8), A("zoomout", 2, 0), order="random", delay=1, fps=12, chroma=0.12, stroke=("#00FFFF", 0.05),
  grad=("#FF00FF", "#FFFF00", "#00FFFF", "#FF00FF", 1), part=("confetti", 0.6, 1, 1, "#FFFFFF"), bold=0.03)
p("039 Knives Out", "typew", "#F0EBDC", A("pop", 18, 0.2, 30, curve="easeoutback"), "none", A("scatter", 10, 3, 200), order="random", delay=2, shadow=("#000000", 0.03, 0.04, 0.0, 0.9),
  tex=("paper", 0.4, 1), vig=(0.5, 0.6), grade=WARM)
p("040 Baby Driver", "black", "#FFFFFF", A("pop", 12, 0.8, 0, curve="steps"), A("pulse", 12, 0.02), A("cut"), order="all", track=-0.04, stroke=("#E00000", 0.04),
  flashc="#E00000", glow=("#FFFFFF", 0.1, 0.3), part=("rain", 0.1, 1, 1, "#FFFFFF"))
# ------------------------------------------------------------------ 041 - 060
p("041 Jurassic Park", "trajan", "#DCB96E", A("zoom", 30, 1.2, 1, curve="easeoutexpo"), A("jitter", 1, E(0.5), 0), A("dissolve", 16, 0, 0.0), order="center", delay=2, track=0.25,
  stroke=("#2A1A00", 0.04), bevel=0.6, grad=("#F6DD9A", "#8A6A22", "", "", 1), fog=(0.3, "#B0C0A0"), part=("dust", 0.6, 1, 0.6, "#FFE9A0"), upper=1)
p("042 Jaws", "impact", "#F0F0F0", A("slide", 24, E(300), 0, dir="down", curve="easeinquad"), A("float", 30, 0.017, 0), A("fall", 10, 2.5, 0, curve="easeinquad"), order="all", track=-0.02,
  shadow=("#05102A", 0.0, 0.07, 0.15, 0.9), fog=(0.25, "#1E5A7A"), grade=(0.6, 1.2, 0.8, "#001426", "#A8D4E8"), vig=(0.8, 0.5))
p("043 Ford v Ferrari", "euro", "#FFFFFF", A("slide", 8, E(1200), 0, dir="right", curve="fastlinear"), A("jitter", 1, E(1.5), 0), A("slide", 4, E(1200), 0, dir="left", curve="linear"), order="all",
  track=0.15, mb=2, stroke=("#C00000", 0.015), part=("dust", 0.3, 1, 3, "#FFFFFF"), grade=(0.8, 1.3, 0.6, "#101010", "#FFFFFF"))
p("044 Harry Potter", "garamond", "#EBC86E", A("dissolve", 36, 0, 0.02), A("float", 40, 0.025, 0), A("dissolve", 18, 0, 0.02), order="random", delay=2, track=0.2,
  stroke=("#2A1800", 0.03), bevel=0.45, glow=("#FFD27A", 0.15, 0.8), part=("sparkle", 1.0, 0.8, 0.6, "#FFE9A0"), vig=(0.5, 0.55), upper=1)
p("045 Wonka", "rounded", "#8A2BE2", A("drop", 18, E(350) / 4, 20, dir="up", curve="easeoutback"), A("pulse", 20, 0.03), A("dissolve", 10, 0, 0.02), delay=2, stroke=("#FFF3D6", 0.06),
  bevel=0.7, part=("confetti", 0.6, 0.8, 0.6, "#FFFFFF"), shadow=("#4A1A00", 0.03, 0.05, 0.1, 0.8))
p("046 Top Gun: Maverick", "euro", "#F0F5FA", A("slide", 10, E(1000), 0, dir="right", curve="fastlinear"), A("jitter", 1, E(1.2), 0), A("slide", 4, E(1000), 0, dir="left", curve="linear"), order="all",
  track=0.2, mb=2, bevel=0.5, tex=("brushed metal", 0.45, 1), stroke=("#101820", 0.025), flare=(0.5, "#9FD0FF"), grade=(0.9, 1.25, 0.6, "#10202A", "#FFE0B0"))
p("047 The Batman", "typew", "#DCDCDC", A("focus", 30, 1.0, 0.18, dir="up", curve="easeoutquad"), A("flicker", 96, 0.12, 0, 1), A("fade", 16), order="random", delay=2, track=0.25, shadow=("#8B0000", 0.0, 0.05, 0.15, 0.9),
  part=("rain", 1.4, 1, 1, "#9AA8B8"), vig=(0.9, 0.5), grade=(0.5, 1.4, 0.9, "#1A0000", "#C8C8D0"), glow=("#8B0000", 0.05, 0.4))
p("048 Avatar: Way of Water", "gill", "#C8F0FF", A("slide", 36, E(350), 1, dir="down", curve="easeoutexpo"), A("float", 48, 0.025, 0.2), A("dissolve", 20, 0, 0.03), delay=2, track=0.35,
  glow=("#00E5FF", 0.33, 2.2), part=("bubbles", 0.9, 0.7, 0.8, "#BFF6FF"), grade=(1.1, 1.1, 0.5, "#001A33", "#B8FFF0"), fog=(0.12, "#40D8E8"))
p("049 Gran Turismo", "euro", "#FFFFFF", A("slide", 8, E(800), 0, dir="left", curve="fastlinear"), A("jitter", 1, E(0.8), 0), A("slide", 4, E(1000), 0, dir="right", curve="linear"), order="all", track=0.2, mb=2,
  stroke=("#E00000", 0.02), shadow=("#2C6BFF", 0.03, 0.03, 0.0, 0.9), scan=(0.1, 3))
p("050 Interstellar", "helvl", "#FFFAF0", A("focus", 48, 1.1, 0.4, dir="upleft", curve="easeoutexpo"), A("pulse", 80, 0.015), A("zoomout", 24, 0, curve="easeinquad"), order="center", delay=3, track=0.4,
  glow=("#FFB347", 0.58, 3.0), flare=(0.3, "#FFB86A"), part=("dust", 0.5, 0.6, 0.4, "#FFE0B0"), chroma=0.02, grain=0.2, grainall=0.2, grade=AMBER)
p("051 Rush (1976)", "euro", "#FFFFFF", A("slide", 9, E(900), 0, dir="left", curve="fastlinear"), A("jitter", 1, E(1), 0), A("slide", 4, E(900), 0, dir="right", curve="linear"), order="all", track=0.15, mb=2,
  stroke=("#C00000", 0.015), part=("dust", 0.3, 1, 3, "#FFFFFF"), grade=SEPIA, grainall=0.4, scratch=0.5)
p("052 Gladiator", "trajan", "#D2AF73", A("zoom", 36, 1.15, 1, curve="easeoutexpo"), "none", A("dissolve", 18, 0, 0.05), order="center", delay=2, track=0.3, bold=0.02,
  shadow=("#000000", 0.03, 0.05, 0.1, 0.8), bevel=0.55, tex=("brushed metal", 0.3, 1), part=("dust", 1.0, 1, 2, "#E0B070"), grade=DESERT)
p("053 The Crown", "didot", "#F5D782", A("spread", 30, 0.2, 1, curve="easeoutquad"), A("float", 120, 0.004, 0.0), A("fade", 16), order="center", delay=3, track=0.2, ital=0.2,
  grad=("#FFE9A0", "#B8902C", "", "", 1), tex=("fine grain", 0.2, 1), shadow=("#000000", 0.0, 0.05, 0.2, 0.8), vig=(0.7, 0.5), grade=(0.9, 1.1, 0.5, "#1A0833", "#F5D782"))
p("054 Black Sails", "georgiab", "#281E14", A("slide", 24, E(250), 0, dir="down", curve="easeoutquad"), A("wave", 40, 0.017, 0.001), A("dissolve", 12, 0, 0.03), delay=1, track=0.1, ital=0.2,
  plate=("#D9C493", 0.95, 0.5), tex=("paper", 0.55, 1), fog=(0.12, "#8090A0"), grade=SEPIA)
p("055 The Queen's Gambit", "bodoni", "#F5F5F5", A("zoom", 20, 1.0, 1, curve="easeoutexpo"), "none", A("cut"), order="center", delay=1, track=0.3, vig=(0.6, 0.55), grade=NOIR)
p("056 The Menu", "bodoni", "#F0F0F0", A("spread", 24, 0.15, 1, curve="easeoutexpo"), "none", A("fade", 12, curve="linear"), order="center", delay=2, track=0.15, tex=("paper", 0.25, 1), vig=(0.85, 0.5))
p("057 Triangle of Sadness", "fut", "#F5DC96", A("wipe", 30, dir="left", curve="easeoutquad"), A("float", 60, 0.008, 0.0), A("fade", 14), delay=1, track=0.3, grad=("#FFF0B8", "#C8A24A", "", "", 1),
  bevel=0.4, flare=(0.45, "#FFE6A8"), part=("sparkle", 0.4, 0.7, 0.6, "#FFFFFF"))
p("058 Tar", "helv", "#FFFFFF", A("slide", 20, E(200), 0, dir="up", curve="easeoutexpo"), "none", A("cut"), order="all", track=0.25, bold=0.03, grade=NOIR, vig=(0.5, 0.55),
  tex=("fine grain", 0.0, 1), scan=(0.05, 8))
p("059 The Whale", "georgia", "#D2D2D2", A("zoom", 36, 1.0, 2, curve="easeoutquad"), A("pulse", 120, 0.005), A("fade", 20, curve="linear"), order="center", delay=3, track=0.1,
  shadow=("#000000", 0.0, 0.03, 0.15, 0.7), vig=(0.85, 0.5), flare=(0.18, "#FFE9C8"), grade=WARM)
p("060 Babylon", "bodoni", "#FFD700", A("strobe", 12, 0.15), A("jitter", 2, E(3), 5), A("scatter", 6, 3, 150), order="random", delay=1, fps=12, track=0.3, stroke=("#000000", 0.04),
  bevel=0.6, part=("confetti", 1.2, 1, 1.2, "#FFD700"), grade=AMBER, grainall=0.4, chroma=0.04, glow=("#FFD700", 0.1, 0.8))
# ------------------------------------------------------------------ 061 - 080
p("061 Senna", "helv", "#FFFFFF", A("slide", 10, E(800), 0, dir="right", curve="fastlinear"), A("jitter", 1, E(0.9), 0), A("slide", 4, E(800), 0, dir="left", curve="linear"), order="all", track=0.2, mb=2,
  bold=0.03, shadow=("#000000", 0.03, 0.03, 0.0, 1), stroke=("#00A65A", 0.012), grainall=0.5, scratch=0.4, grade=SEPIA)
p("062 Yellowjackets", "typew", "#D2D2C8", A("strobe", 16, 0.05), A("jitter", 2, E(1.5), 10), A("dissolve", 8, 0, 0.0), order="random", delay=2, track=0.2, fps=12, stroke=("#1A1408", 0.04),
  fog=(0.25, "#8090A0"), part=("static", 0.25, 1, 1, "#FFFFFF"), scan=(0.2, 2), grade=GREENGRAY, grainall=0.4)
p("063 The Bear (Forks)", "helvl", "#FFFFFF", A("zoom", 16, 1.02, 1, curve="easeoutexpo"), "none", A("fade", 10, curve="linear"), order="center", delay=1, track=0.3, bevel=0.35, flare=(0.2, "#FFFFFF"), vig=(0.3, 0.6))
p("064 Station Eleven", "garamond", "#EBEBF0", A("spread", 36, 0.2, 1, curve="easeoutquad"), A("pulse", 120, 0.004), A("whiteout", 20, 0.8), order="center", delay=3, track=0.2, ital=0.2, glow=("#FFFFFF", 0.1, 0.5),
  part=("dust", 0.7, 0.8, 0.4, "#FFF3D0"), flare=(0.25, "#FFF3D0"), grade=(0.6, 0.95, 0.5, "#90A8C8", "#FFF3D0"))
p("065 The Boys", "impact", "#FFFFFF", A("pop", 12, 1.2, 0, curve="steps"), A("glowpulse", 16, 0.7), A("burn", 4, 1, 1), delay=1, track=0.1, stroke=("#000000", 0.07), shadow=("#D0112B", 0.04, 0.05, 0, 1),
  glow=("#2C6BFF", 0.1, 0.8), tex=("halftone", 0.3, 1), grain=0.3)
p("066 Grand Budapest Hotel", "fut", "#F5E1C8", A("spread", 24, 0.2, 1, curve="easeoutexpo"), "none", A("squashx", 16), order="center", delay=2, track=0.2, stroke=("#E8A0B8", 0.03),
  grad=("#F8E8C8", "#E8C078", "", "", 1), plate=("#F4B6C8", 0.5, 0.5), vig=(0.3, 0.6))
p("067 Tinker Tailor Soldier Spy", "courierb", "#C8C3B4", A("slide", 30, E(250), 1, dir="left", curve="linear"), A("drift", 240, 0.004, 0, dir="right"), A("wipe", 12, dir="right", curve="linear"), order="all", track=0.1,
  tex=("paper", 0.4, 1), grade=(0.35, 1.1, 0.85, "#0F140E", "#C8C8A8"), grainall=0.55, vig=(0.5, 0.55))
p("068 The Life Aquatic", "fut", "#FFFFFF", A("slide", 20, E(120), 1, dir="left", curve="easeoutquad"), A("float", 48, 0.01, 0.2), A("fade", 10, curve="linear"), delay=1, track=0.25, bold=0.03,
  plate=("#1B5FA8", 1, 0.4), grade=(1.1, 1.05, 0.4, "#001A44", "#FFFFFF"))
p("069 Network", "mono", "#F0F0F0", A("strobe", 8, 0.1), A("jitter", 2, E(2), 0), A("crtoff", 4), order="all", fps=12, track=-0.02, chroma=0.06, scan=(0.35, 3), glow=("#7CFF9C", 0.08, 0.6), grainall=0.3)
p("070 Adaptation", "courier", "#1E1E1E", A("step"), A("none"), A("stepoff"), delay=2, odelay=1, outrev=1, tex=("paper", 0.15, 1), plate=("#F2EFE6", 1, 0.5))
p("071 The Hateful Eight (Opening)", "rock", "#F0F0F0", A("zoom", 36, 1.0, 2, curve="easeoutquad"), A("jitter", 1, E(1.5), 0, 1), A("fade", 20, curve="linear"), delay=2, track=0.35,
  stroke=("#000000", 0.04), part=("snow", 1.4, 1.2, 1.0, "#FFFFFF"), grade=(0.4, 1.15, 0.8, "#10151A", "#C8D0D8"), grainall=0.5, tex=("rust", 0.2, 1))
p("072 The Hateful Eight (Panavision)", "rock", "#EAE6DC", A("zoom", 36, 1.0, 2, curve="easeoutquad"), A("wave", 24, 0.015, 0.001), A("wipe", 20, dir="right", curve="linear"), delay=2, track=0.3,
  stroke=("#000000", 0.04), part=("snow", 1.0, 1.4, 0.8, "#FFFFFF"), fog=(0.15, "#DDE4EA"), grade=SEPIA, grainall=0.55, scratch=0.3)
p("073 Zodiac", "courierb", "#D2CDBE", A("step"), "none", A("dissolve", 8, 0, 0.12), delay=1, track=0.15, tex=("paper", 0.3, 1), grade=GREENGRAY, vig=(0.5, 0.55), grainall=0.3)
p("074 The French Dispatch", "didot", "#1E1E1E", A("slide", 24, E(300), 0, dir="up", curve="easeoutexpo"), "none", A("slide", 10, E(200), 0, dir="up", curve="linear"), order="center", delay=2, track=0.2, bold=0.02,
  plate=("#F4EAD2", 1, 0.5), tex=("paper", 0.3, 1), grade=SEPIA)
p("075 Whiplash", "black", "#FFFFFF", A("pop", 12, 0.9, 0, curve="steps"), A("pulse", 8, 0.03), A("cut"), order="all", track=-0.03, stroke=("#000000", 0.03), flashc="#E0A060",
  part=("dust", 0.5, 0.8, 0.5, "#E8C890"), grade=(0.7, 1.3, 0.8, "#000000", "#FFD0A0"), glow=("#D49A4A", 0.08, 0.6))
p("076 La La Land", "fut", "#FFF0C8", A("spread", 36, 0.2, 1, curve="easeoutquad"), A("float", 60, 0.0125, 0.0), A("fade", 18), order="center", delay=3, track=0.2, ital=0.2, glow=("#FFD27A", 0.15, 0.9),
  part=("bokeh", 0.8, 1, 0.6, "#FFD27A"), grade=(1.2, 1.1, 0.6, "#001040", "#FFD070"))
p("077 Moonlight", "georgia", "#DCE6F5", A("zoom", 40, 1.05, 2, curve="easeoutexpo"), A("flicker", 96, 0.15, 0, 1), A("fade", 20), order="all", track=0.25, glow=("#7A86F0", 0.12, 0.6),
  grain=0.2, grainall=0.3, vig=(0.6, 0.55), grade=INDIGO)
p("078 Rush Hour", "impact", "#FFFFFF", A("slide", 12, E(900), 0, dir="right", curve="easeoutquad"), A("jitter", 1, E(0.6), 0), A("slide", 5, E(900), 0, dir="left", curve="linear"), delay=1, track=0.15, mb=2,
  stroke=("#000000", 0.07), shadow=("#FFD400", 0.03, 0.05, 0, 1), part=("bokeh", 0.8, 1, 1.5, "#FF3B5C"), flare=(0.25, "#FF9A60"))
p("079 The Rider", "garamond", "#F5E6C8", A("zoom", 30, 1.0, 2, curve="easeoutquad"), A("float", 80, 0.007, 0), A("fade", 16), order="center", delay=2, track=0.2, ital=0.2, shadow=("#000000", 0.01, 0.03, 0.08, 0.5),
  flare=(0.3, "#FFD9A0"), grainall=0.4, grade=DESERT)
p("080 Arrival", "fut", "#D2DCE1", A("focus", 48, 1.1, 0.35, dir="upleft", curve="easeoutexpo"), A("float", 96, 0.02, 0.2), A("dissolve", 24, 0, 0.0), order="random", delay=3, track=0.4, glow=("#FFFFFF", 0.1, 0.5),
  fog=(0.4, "#A8B0B8"), grade=(0.3, 1.0, 0.7, "#202428", "#E0E6EA"), part=("dust", 0.4, 1, 0.4, "#FFFFFF"))
# ------------------------------------------------------------------ 081 - 100
p("081 Rush (Wet)", "euro", "#FFFFFF", A("slide", 9, E(950), 0, dir="right", curve="fastlinear"), A("jitter", 1, E(1.3), 0), A("slide", 4, E(950), 0, dir="left", curve="linear"), order="all", track=0.2, mb=2,
  part=("rain", 1.6, 1, 1.3, "#BFD0E0"), fog=(0.18, "#A0A8B0"), grade=COLD, glow=("#9FC4FF", 0.05, 0.4))
p("082 Jurassic World", "trajan", "#E6E6EB", A("zoom", 28, 1.15, 1, curve="easeoutexpo"), A("jitter", 1, E(0.7), 0), A("fade", 14), order="center", delay=2, track=0.3, bevel=0.55, tex=("brushed metal", 0.45, 1),
  fog=(0.25, "#A8C0A8"), grade=TEAL, flare=(0.2, "#C8E8FF"))
p("083 Master and Commander", "caslon", "#DCD2C3", A("slide", 36, E(60), 0, dir="down", curve="easeoutquad"), A("wave", 30, 0.018, 0.001), A("dissolve", 18, 0, 0.0), delay=2, track=0.15, bold=0.02, shadow=("#000000", 0.03, 0.04, 0.05, 0.8),
  fog=(0.3, "#E0E0E0"), part=("dust", 0.6, 1, 1, "#E8E0D0"), grade=(0.5, 1.15, 0.8, "#10181F", "#E0C89A"), tex=("paper", 0.25, 1))
p("084 Grand Budapest (Lobby)", "fut", "#FFE696", A("spread", 24, 0.2, 1, curve="easeoutexpo"), "none", A("squashx", 14), order="center", delay=2, track=0.25, stroke=("#7A1230", 0.03),
  grad=("#FFF0B0", "#D8A040", "", "", 1), plate=("#8A1A3A", 0.9, 0.45), bevel=0.5)
p("085 Ford v Ferrari (Pit)", "euro", "#F5F5FA", A("slide", 16, E(250), 0, dir="up", curve="easeoutexpo"), "none", A("cut"), order="all", track=0.2, bold=0.03, shadow=("#000000", 0.03, 0.04, 0.0, 1),
  bevel=0.5, tex=("brushed metal", 0.45, 1), flare=(0.25, "#FFFFFF"), grade=COLD)
p("086 Kill Bill", "impact", "#FFE600", A("pop", 6, 1.5, 0, curve="steps"), A("jitter", 1, E(1.8), 0.4, 1), A("tear", 3, 1.0, curve="linear"), order="all", track=-0.02, stroke=("#000000", 0.033), shadow=("#000000", 0.04, 0.04, 0, 1),
  scratch=1.0, grainall=0.6, grain=0.3, flashc="#FFFFFF", grade=(1.2, 1.4, 0.8, "#000000", "#FFE090"))
p("087 Midsommar", "trajan", "#FFFFFF", A("spread", 40, 0.2, 1, curve="easeoutexpo"), A("pulse", 144, 0.006), A("whiteout", 24, 1), order="center", delay=3, track=0.3, glow=("#FFFFFF", 0.18, 2.8),
  part=("sparkle", 0.4, 0.6, 0.4, "#FFE9F8"), grade=(1.1, 0.95, 0.6, "#8A9AD0", "#FFF3F0"), flare=(0.3, "#FFFFFF"))
p("088 First Man", "helv", "#EBEBE6", A("slide", 10, E(150), 0, dir="up", curve="linear"), A("jitter", 1, E(2.5), 0), A("cut"), order="all", track=0.1, grainall=0.7, grain=0.3, flare=(0.2, "#FFFFFF"),
  grade=(0.5, 1.2, 0.8, "#05060A", "#D8D8D0"))
p("089 Annihilation", "helvl", "#F0F5FF", A("chroma", 32, 0.12, 0, curve="easeoutquad"), A("chromajitter", 4, 0.1), A("dissolve", 16, 0, 0.0), order="random", delay=2, track=0.4,
  glow=("#7AF0FF", 0.08, 0.5), tex=("oil impasto", 0.25, 1.4), grad=("#FF9BE8", "#7AF0FF", "#FFE98A", "#8AFFB0", 1), chroma=0.1, fog=(0.12, "#B8A0FF"))
p("090 Oppenheimer (Trinity)", "courierb", "#FFFAF0", A("scramble", 12), A("jitter", 1, E(0.6), 0, 1), A("whiteout", 2, 1), delay=1, glow=("#FFB347", 0.25, 3.0), grainall=0.5, flashc="#FFF2D8",
  grade=NOIR, part=("embers", 0.5, 0.8, 1, "#FFB347"))
p("091 Rush Hour 2", "impact", "#FFFFFF", A("slide", 10, E(1000), 0, dir="right", curve="fastlinear"), A("jitter", 1, E(0.8), 0), A("slide", 4, E(1000), 0, dir="left", curve="linear"), order="all", track=0.2, mb=2,
  stroke=("#000000", 0.07), glow=("#FF2DCB", 0.3, 2.0), part=("bokeh", 1.0, 1, 2, "#00E5FF"), flare=(0.3, "#FF2DCB"))
p("092 The French Dispatch (Features)", "didot", "#231E19", A("zoom", 24, 1.0, 1, curve="easeoutexpo"), "none", A("wipe", 12, dir="right", curve="linear"), delay=1, track=0.12, ital=0.2, plate=("#F2EAD3", 1, 0.45),
  tex=("paper", 0.4, 1), grade=SEPIA)
p("093 Ad Astra", "fut", "#F0F5FF", A("spread", 48, 0.35, 1, curve="easeoutexpo"), A("float", 120, 0.008, 0), A("fade", 24, curve="linear"), order="center", delay=3, track=0.3, glow=("#FFFFFF", 0.15, 0.6),
  part=("dust", 0.9, 0.5, 0.2, "#FFFFFF"), flare=(0.2, "#C8E0FF"), grade=(0.5, 1.15, 0.7, "#000A1C", "#C8D8F0"))
p("094 There Will Be Blood", "caslon", "#D2B98C", A("slide", 30, E(250), 0, dir="down", curve="easeoutquad"), A("pulse", 120, 0.004), A("wipe", 16, dir="down", curve="linear"), order="all", track=0.2, bold=0.03,
  shadow=("#000000", 0.02, 0.04, 0.05, 0.9), tex=("rust", 0.4, 1), grade=SEPIA, grainall=0.5, vig=(0.6, 0.5))
p("095 Drive", "fut", "#FFFFFF", A("spread", 36, 0.2, 1, curve="easeoutexpo"), A("glowpulse", 72, 0.8), A("fade", 16), order="center", delay=2, track=0.2, ital=0.2, bold=0.03, stroke=("#FF2D95", 0.02),
  glow=("#FF2D95", 0.42, 2.5), part=("bokeh", 0.8, 1, 0.5, "#00E5FF"), grade=(1.1, 1.1, 0.5, "#14003A", "#FF8AD0"))
p("096 Apollo 13", "mono", "#FFAF32", A("scramble", 14), A("jitter", 1, E(0.5), 0), A("cut"), order="all", track=0.1, glow=("#FFAF32", 0.2, 1.8), scan=(0.3, 3), grade=(0.9, 1.1, 0.6, "#1A0F00", "#FFAF32"))
p("097 The Dark Knight", "black", "#D2D7DC", A("spread", 36, 0.2, 1, curve="easeoutexpo"), "none", A("fade", 16, curve="linear"), order="center", delay=3, track=0.2, bevel=0.4, tex=("concrete", 0.4, 1),
  shadow=("#000000", 0.0, 0.06, 0.2, 0.9), vig=(0.85, 0.5), grade=(0.4, 1.3, 0.85, "#000608", "#C8D2DC"))
p("098 The King's Speech", "caslon", "#F0D28C", A("zoom", 30, 1.04, 2, curve="easeoutquad"), A("float", 90, 0.004, 0), A("fade", 16), order="center", delay=2, track=0.2, bevel=0.45,
  grad=("#FFE6A0", "#B8902C", "", "", 1), shadow=("#000000", 0.0, 0.04, 0.12, 0.6), vig=(0.6, 0.5), grade=WARM)
p("099 Titanic", "didot", "#F5E6B4", A("spread", 48, 0.3, 1, curve="easeoutexpo"), A("float", 68, 0.015, 0), A("fade", 24), order="center", delay=3, track=0.3, bold=0.02, glow=("#FFD27A", 0.12, 0.7),
  grad=("#FFF0C8", "#C8A050", "", "", 1), part=("sparkle", 0.4, 0.7, 0.4, "#FFFFFF"), grade=(0.9, 1.1, 0.6, "#001833", "#FFD090"))
p("100 The Matrix", "mono", "#B4FFB4", A("scramble", 16), A("jitter", 1, E(1), 0), A("squashy", 6), order="ltr", delay=1, track=-0.05, bold=0.03, glow=("#00FF41", 0.25, 2.5),
  scan=(0.3, 3), part=("rain", 0.5, 0.8, 0.8, "#00FF41"), grade=(1.0, 1.2, 0.7, "#001A08", "#B4FFB4"))
# ------------------------------------------------------------------ 101 - 240 : variantes et familles
def race(name, fx, rain=False, fog_=None, night=False, flare_=None, grade_=None, color="#FFFFFF", sh=None, frames=9, dist=950, v=1.3, strokec="#C00000"):
    part = ("rain", 1.6, 1, 1.3, "#BFD0E0") if rain else (("dust", 0.35, 1, 3, "#FFFFFF") if not night else ("bokeh", 0.6, 1, 2.5, "#FFD7A0"))
    p(name, "euro", color, A("slide", frames, E(dist), 0, dir="right", curve="fastlinear"), A("jitter", 1, E(v), 0), A("slide", 4, E(dist), 0, dir="left", curve="linear"),
      order="all", track=0.2, mb=2, bold=0.03, stroke=(strokec, 0.012), shadow=sh or ("#000000", 0.03, 0.04, 0.0, 0.9), part=part, fog=fog_, flare=flare_,
      grade=grade_ or (0.85, 1.25, 0.6, "#0A0A0A", "#FFFFFF"), grainall=0.25, glow=("#FFFFFF", 0.04, 0.3) if night else None)
race("101 Rush: Monza", 0, flare_=(0.5, "#FFD9A0"), grade_=(1.0, 1.2, 0.5, "#2A1500", "#FFE0A0"), sh=("#000000", 0.03, 0.04, 0, 1), dist=900)
race("106 Le Mans '66: Pit Lane", 0, grade_=COLD, flare_=(0.2, "#FFFFFF"), dist=1000, frames=10)
race("121 Rush: Nurburgring", 0, rain=True, fog_=(0.35, "#B0B8C0"), grade_=COLD)
race("126 Le Mans '66: Ford GT40", 0, night=True, flare_=(0.4, "#9FC4FF"), grade_=(0.9, 1.3, 0.6, "#00102A", "#FFFFFF"), dist=1100, frames=8, strokec="#1F55C8", v=1.2)
race("141 Rush: Japanese Grand Prix", 0, rain=True, fog_=(0.4, "#9098A0"), grade_=(0.6, 1.2, 0.8, "#10080A", "#E0B0B0"), strokec="#E01020", v=1.4)
race("146 Le Mans '66: Circuit de la Sarthe", 0, night=True, flare_=(0.5, "#FFFFFF"), grade_=(0.85, 1.3, 0.7, "#000612", "#FFFFFF"), dist=1150, frames=8, v=1.5)
race("151 Rush: Silverstone GP", 0, rain=True, fog_=(0.3, "#A8B0B8"), grade_=COLD, strokec="#C8D0D8", v=1.3)
race("156 Le Mans '66: Mulsanne Straight", 0, night=True, flare_=(0.6, "#FFFFFF"), grade_=(0.8, 1.35, 0.8, "#000408", "#FFFFFF"), dist=1200, frames=8, v=1.6)
race("176 Ford v Ferrari: Daytona 24h", 0, night=True, flare_=(0.5, "#FFFFFF"), grade_=(0.85, 1.3, 0.7, "#00081A", "#FFFFFF"), dist=1100, frames=8, v=1.5)
race("191 Ford v Ferrari: Le Mans Pits", 0, night=True, fog_=(0.15, "#C8C8C8"), flare_=(0.45, "#FFFFFF"), grade_=(0.8, 1.25, 0.6, "#101010", "#FFFFFF"), dist=1100, frames=8, v=1.4)

def spy(name, stamp=False, dur=30, grade_=GREENGRAY, wipe=12, track=0.15):
    col = "#B41E1E" if stamp else "#2D2823"
    p(name, "courierb", col, A("zoom", 6, 1.2, 1, curve="easeoutexpo") if stamp else A("zoom", dur, 1.0, 2, curve="easeoutexpo"), "none", A("wipe", wipe, dir="right", curve="linear"),
      order="all", track=track, tex=("paper", 0.45, 1), plate=("#D8CCB0", 0.85, 0.5), grade=grade_, grainall=0.55, vig=(0.5, 0.55), fog=(0.08, "#9AA088"))
spy("102 Tinker Tailor: Archive", stamp=True, grade_=SEPIA, wipe=10, track=0.25)
spy("122 Tinker Tailor: Control"); spy("142 Tinker Tailor: Control Room"); spy("152 Tinker Tailor: Circus HQ")

def saphire(name, plate_alpha=1.0, label=False, roll=1.1, col="#FFFFFF"):
    p(name, "fut", "#141414" if label else col, A("slide", 20, E(200) if label else E(120), 1, dir="up" if label else "left", curve="easeoutquad"),
      A("float", 56, roll / 120.0, 0.2), A("wipe", 10, dir="right", curve="linear") if label else A("fade", 10, curve="linear"), delay=1, track=0.3 if label else 0.25, bold=0.03,
      plate=("#F2E6A0" if label else "#1B5FA8", plate_alpha, 0.35), grade=(1.1, 1.05, 0.4, "#001A44", "#FFFFFF"), part=("bubbles", 0.3, 0.7, 0.4, "#BFE6FF"))
saphire("103 Life Aquatic: Research", label=True, roll=1.0)
saphire("123 Life Aquatic: Belafonte"); saphire("143 Life Aquatic: Team Zissou", roll=1.1); saphire("153 Life Aquatic: Belafonte Sub", roll=1.2)

def network(name, jit=2, dur=8, rgb=0.06):
    p(name, "mono", "#F0F8F0", A("strobe", dur, 0.1), A("jitter", 2, E(jit), 0), A("crtoff", 4), order="random" if dur > 8 else "all", delay=1, fps=12, track=-0.02, chroma=rgb,
      scan=(0.35, 3), glow=("#7CFF9C", 0.08, 0.6), vig=(0.5, 0.6), grainall=0.3)
network("104 Network: Control Room", 1.5, 10, 0.07); network("124 Network: UBS News"); network("144 Network: Max Schumacher"); network("154 Network: UBS Control Room")

def adaptation(name, mist=0.15, dur=28):
    p(name, "courier", "#191919", A("step"), A("none"), A("stepoff"), delay=1, odelay=1, outrev=1, plate=("#F2EFE6", 1, 0.5), tex=("paper", 0.3, 1), fog=(mist, "#7A9A6A"), vig=(0.4, 0.6))
adaptation("105 Adaptation: Orchid", 0.18); adaptation("125 Adaptation: Ghost Orchid", 0.22); adaptation("145 Adaptation: Everglades", 0.25); adaptation("155 Adaptation: Orchid Swamp", 0.28)

def hateful(name, cabin=False, amt=1.4, dur=36):
    p(name, "rock", "#E6D2B9" if cabin else "#F0F0F0", A("zoom", dur, 1.0, 2, curve="easeoutquad"), A("wave", 56, 0.012, 0.001), A("wipe", 18, dir="right", curve="linear") if cabin else A("fade", 20, curve="linear"), delay=2,
      track=0.3, stroke=("#000000", 0.04), tex=("rust", 0.25, 1) if not cabin else ("wood", 0.5, 1), part=("snow", amt, 1.2, 0.9, "#FFFFFF"), fog=(0.15, "#DDE4EA"),
      grade=(0.4, 1.15, 0.8, "#10151A", "#C8D0D8") if not cabin else (0.6, 1.2, 0.8, "#1A0F06", "#F0C890"), grainall=0.5, scratch=0.2, flare=(0.15, "#FFB070") if cabin else None)
hateful("107 Hateful Eight: Blizzard"); hateful("127 Hateful Eight: Stagecoach"); hateful("147 Hateful Eight: Haberdashery", cabin=True, amt=0.7)
hateful("157 Hateful Eight: Red Rock", amt=1.6); hateful("177 Hateful Eight: Minnie's Haberdashery", cabin=True, amt=0.6); hateful("192 Hateful Eight: Snowbound", amt=2.2)

def zodiac(name):
    p(name, "courierb", "#D2CDBE", A("step"), "none", A("dissolve", 8, 0, 0.12), delay=1, track=0.13, bevel=0.1, tex=("paper", 0.45, 1), grade=GREENGRAY, vig=(0.5, 0.55), grainall=0.3, scratch=0.2)
for nm in ("108 Zodiac: Investigation", "128 Zodiac: Chronicle", "148 Zodiac: San Francisco", "158 Zodiac: San Francisco Chronicle", "178 Zodiac: Lake Berryessa", "193 Zodiac: Police Files"): zodiac(nm)

def dispatch(name, concrete=False, d=250):
    p(name, "didot", "#F0F0F0" if concrete else "#F5EBD2", A("slide", 24, E(d), 0, dir="up", curve="easeoutexpo"), "none", A("wipe", 12, dir="down", curve="linear"), order="center", delay=2,
      track=0.22, bold=0.02, ital=0 if concrete else 0.2, stroke=("#C9A24A", 0.012), plate=("#C9BFA8" if concrete else "#2E5E8C", 0.9, 0.5), tex=("concrete" if concrete else "paper", 0.4, 1),
      grade=GREENGRAY if concrete else (0.8, 1.05, 0.5, "#3A2A1A", "#F5EBD2"), grainall=0.3)
dispatch("109 French Dispatch: Art Section", d=300); dispatch("129 French Dispatch: Ennui"); dispatch("149 French Dispatch: Concrete Masterpiece", concrete=True)
dispatch("159 French Dispatch: Private Dining Room"); dispatch("179 French Dispatch: Commissioner's Dining Room"); dispatch("194 French Dispatch: Commissioner's Table")

def jazz(name, hz=3.5, big=1.18):
    p(name, "black", "#FFFFFF", A("pop", 10, 0.9, 0, curve="steps"), A("pulse", max(2, round(24 / hz)), 0.035), A("cut"), order="all", track=-0.03, stroke=("#000000", 0.03),
      flashc="#E0A060", glow=("#D49A4A", 0.08, 0.6), part=("dust", 0.5, 0.8, 0.5, "#E8C890"), flare=(0.2, "#FFD9A0"), grade=(0.7, 1.3, 0.8, "#000000", "#FFD0A0"))
jazz("110 Whiplash: Finale", 3.0); jazz("130 Whiplash: Caravan", 4.0); jazz("150 Whiplash: Studio Band", 3.5); jazz("160 Whiplash: Studio Rehearsal", 3.5)
jazz("180 Whiplash: Andrew's Solo", 3.5); jazz("195 Whiplash: Final Competition", 4.0)

def lalaland(name, dur=40, hue="#FFF0C8", sky="#001040"):
    p(name, "fut", hue, A("spread", dur, 0.2, 1, curve="easeoutquad"), A("float", 64, 0.0125, 0), A("fade", 20), order="center", delay=3, track=0.2, ital=0.2, glow=("#FFD27A", 0.15, 0.9),
      part=("bokeh", 0.8, 1, 0.6, "#FFD27A"), grade=(1.2, 1.1, 0.6, sky, "#FFD070"))
lalaland("111 La La Land: Epilogue"); lalaland("131 La La Land: Planetarium", hue="#FFF5D2", sky="#000A2A"); lalaland("161 La La Land: Griffith Observatory", hue="#FFF0D2")
lalaland("181 La La Land: Seb's Jazz Club", hue="#FFEBC3", sky="#1A0A00"); lalaland("196 La La Land: Epilogue Symphony", dur=48, hue="#FFF0CD")

def moonlight(name, dur=40):
    p(name, "georgia", "#DCE6F5", A("zoom", dur, 1.05, 2, curve="easeoutexpo"), A("flicker", 96, 0.15, 0, 1), A("fade", 20), order="all", track=0.25, glow=("#7A86F0", 0.12, 0.6),
      grain=0.2, grainall=0.3, vig=(0.6, 0.55), grade=INDIGO)
moonlight("112 Moonlight: Act III"); moonlight("132 Moonlight: Chiron"); moonlight("162 Moonlight: Little"); moonlight("182 Moonlight: Blue Diner"); moonlight("197 Moonlight: Epilogue Shores", 48)

def rushhour(name, neon="#FF2DCB", neon2="#00E5FF", gold=False):
    p(name, "impact", "#FFFFFF", A("slide", 12, E(900), 0, dir="right", curve="easeoutquad"), A("jitter", 1, E(0.7), 0), A("slide", 5, E(900), 0, dir="left", curve="linear"), delay=1, track=0.15, mb=2,
      stroke=("#000000", 0.07), shadow=("#FFD400", 0.03, 0.05, 0, 1) if gold else ("#000000", 0.03, 0.05, 0, 1), glow=(neon, 0.28, 1.8), part=("bokeh", 1.0, 1, 2, neon2), flare=(0.3, neon))
rushhour("113 Rush Hour 3", "#FF6A2D", "#2D6BFF", True); rushhour("133 Rush Hour: Los Angeles", "#FFB02D", "#FF2D5C", True); rushhour("163 Rush Hour: Hong Kong")
rushhour("183 Rush Hour 2: Hong Kong Nights"); rushhour("198 Rush Hour 3: Paris Finale", "#FFD27A", "#C8A0FF", True)

def rider(name, dur=30):
    p(name, "garamond", "#F5E6C8", A("zoom", dur, 1.0, 2, curve="easeoutquad"), A("float", 80, 0.007, 0), A("fade", 16), order="center", delay=2, track=0.2, ital=0.2,
      shadow=("#000000", 0.01, 0.03, 0.08, 0.5), flare=(0.35, "#FFD9A0"), part=("dust", 0.5, 1, 0.5, "#E8C890"), grainall=0.4, grade=DESERT)
rider("114 The Rider: Plains"); rider("134 The Rider: Dakota"); rider("164 The Rider: Horizon"); rider("184 The Rider: Sunset Trail"); rider("199 The Rider: Final Horizon", 36)

def arrival(name, dur=48, fogamt=0.4):
    p(name, "fut", "#D2DCE1", A("focus", dur, 1.1, 0.35, dir="upleft", curve="easeoutexpo"), A("float", 96, 0.02, 0.2), A("dissolve", 24, 0, 0.0), order="random", delay=3, track=0.4,
      glow=("#FFFFFF", 0.1, 0.5), fog=(fogamt, "#A8B0B8"), grade=(0.3, 1.0, 0.7, "#202428", "#E0E6EA"), part=("dust", 0.4, 1, 0.4, "#FFFFFF"))
arrival("115 Arrival: Heptapod"); arrival("135 Arrival: Hyper-Lingua"); arrival("165 Arrival: The Ship", fogamt=0.5); arrival("185 Arrival: The Monolith", fogamt=0.5); arrival("200 Arrival: Universal Language", 60, 0.3)

def killbill(name, blue=False):
    p(name, "impact", "#FFE600" if not blue else "#FFFFFF", A("pop", 6, 1.45, 0, curve="steps"), A("jitter", 1, E(1.7), 0.4, 1), A("tear", 3, 1.0, curve="linear"), order="all", track=-0.02,
      stroke=("#000000", 0.033), shadow=("#000000", 0.04, 0.04, 0, 1), glow=("#2D6BFF", 0.33, 2.2) if blue else None, scratch=1.0, grainall=0.6, grain=0.3, flashc="#FFFFFF",
      grade=(1.2, 1.4, 0.8, "#000000", "#FFE090"), part=("snow", 0.4, 1, 0.4, "#FFFFFF") if blue else None)
killbill("116 Kill Bill: Vol. 2"); killbill("136 Kill Bill: House of Blue Leaves", True)

def midsommar(name, dur=40):
    p(name, "trajan", "#FFFAF0", A("spread", dur, 0.2, 1, curve="easeoutexpo"), A("pulse", 144, 0.006), A("whiteout", 22, 1), order="center", delay=3, track=0.28, glow=("#FFFFFF", 0.18, 2.6),
      part=("sparkle", 0.4, 0.6, 0.4, "#FFE9F8"), grade=(1.1, 0.95, 0.6, "#8A9AD0", "#FFF3F0"), flare=(0.3, "#FFFFFF"), tex=("paper", 0.1, 1))
midsommar("117 Midsommar: Maypole"); midsommar("137 Midsommar: Harga")

def firstman(name, shake=2.8, grain=0.7):
    p(name, "helv", "#E6E6E1", A("slide", 10, E(170), 0, dir="up", curve="linear"), A("jitter", 1, E(shake), 0), A("cut"), order="all", track=0.12, grainall=grain, grain=0.3, flare=(0.2, "#FFFFFF"),
      grade=(0.5, 1.2, 0.8, "#05060A", "#D8D8D0"), scan=(0.1, 3))
firstman("118 First Man: Saturn V", 2.8); firstman("138 First Man: Gemini", 2.2)

def annihilation(name, d=13):
    p(name, "helvl", "#EBF0FF", A("chroma", 32, 0.12, 0, curve="easeoutquad"), A("chromajitter", 4, 0.09), A("dissolve", 16, 0, 0.0), order="random", delay=2, track=0.38, chroma=0.1,
      glow=("#7AF0FF", 0.08, 0.5), tex=("oil impasto", 0.25, 1.4), grad=("#FF9BE8", "#7AF0FF", "#FFE98A", "#8AFFB0", 1), fog=(0.12, "#B8A0FF"))
annihilation("119 Annihilation: Lighthouse"); annihilation("139 Annihilation: The Shimmer")

def oppenheimer(name, glowi=3.0):
    p(name, "courierb", "#FFFAF0", A("scramble", 12), A("jitter", 1, E(0.55), 0, 1), A("whiteout", 2, 1), delay=1, track=0.05, glow=("#FFB347", 0.22, glowi), grainall=0.5,
      flashc="#FFF2D8", grade=NOIR, part=("embers", 0.5, 0.8, 1, "#FFB347"))
oppenheimer("120 Oppenheimer: Los Alamos"); oppenheimer("140 Oppenheimer: Gadget", 3.1)

def sicario(name):
    p(name, "euro", "#F5DCB4", A("zoom", 36, 1.06, 2, curve="easeoutexpo"), A("wave", 36, 0.0067, 0.001), A("fade", 16), order="center", delay=2, track=0.22, bold=0.04,
      shadow=("#000000", 0.02, 0.04, 0.1, 0.8), part=("dust", 0.9, 1, 1.5, "#E8C890"), fog=(0.18, "#E0B070"), grade=DESERT, grainall=0.4)
sicario("166 Sicario: Border Crossing"); sicario("171 Sicario: Juarez Convoy")

def bladerunner(name, tealorange=False):
    p(name, "euro", "#B4F0FF" if tealorange else "#FFB464", A("spread", 48, 0.2, 1, curve="easeoutexpo"), A("float", 96, 0.015, 0.2), A("dissolve", 24, 0, 0.04), order="random", delay=3, track=0.35, bold=0.03,
      glow=("#FFB464" if not tealorange else "#00E5FF", 0.35, 2.0), fog=(0.32, "#FF8C3C" if not tealorange else "#2A6A7A"), flare=(0.35, "#FFB464"),
      part=("rain", 1.0, 1, 1, "#9FD8E8") if tealorange else ("ash", 0.8, 1, 0.8, "#E8A060"), grade=TEAL)
bladerunner("167 Blade Runner 2049: Orange Haze"); bladerunner("172 Blade Runner 2049: K's Spinner", True)

def social(name):
    p(name, "mono", "#E6F0FF", A("scramble", 14), A("flicker", 4, 0.1, 0, 1), A("cut"), order="ltr", delay=1, track=-0.01, bold=0.03, glow=("#2A6AFF", 0.1, 0.8), scan=(0.12, 3),
      grade=(0.7, 1.15, 0.7, "#000820", "#C8DCFF"), vig=(0.5, 0.55))
social("168 The Social Network: Harvard Binary"); social("173 The Social Network: Algorithm Wall")

def prisoners(name, rain=False):
    p(name, "caslon", "#D2DCE6", A("zoom", 40, 1.0, 2, curve="easeoutquad"), A("wave", 40, 0.0075, 0.001), A("fade", 20), delay=2, track=0.2, bold=0.03, shadow=("#000000", 0.02, 0.05, 0.15, 0.9),
      part=("rain", 1.4, 1, 1, "#9AA8B8") if rain else ("snow", 0.5, 1, 0.5, "#E8F0F8"), fog=(0.25, "#A8B8C0"), vig=(0.8, 0.5), grade=(0.4, 1.25, 0.85, "#050A10", "#C8D2DC"),
      flare=(0.12, "#FFFFFF") if rain else None)
prisoners("169 Prisoners: Pennsylvania Winter"); prisoners("174 Prisoners: Rainy Search", True)

def spiderverse(name, miles=False):
    p(name, "comic", "#FFEB00" if miles else "#FFFFFF", A("pop", 8, 1.4, 0, curve="fastlinear"), A("jitter", 2, E(1.6), 0), A("stretch", 2, 1.6, 1.4), order="all", fps=12, bold=0.03,
      stroke=("#000000", 0.06), shadow=("#FF00A8" if miles else "#00E5FF", 0.07, 0.07, 0, 1), chroma=0.07, tex=("halftone", 0.35, 1), part=("confetti", 0.3, 0.8, 1, "#FFFFFF") if miles else None)
spiderverse("170 Spider-Verse: Into the Spider-Verse"); spiderverse("175 Spider-Verse: Miles Morales", True)

p("186 Drive: Nightcall", "fut", "#00F5FF", A("spread", 36, 0.2, 1, curve="easeoutquad"), A("pulse", 48, 0.015), A("fade", 18), order="center", delay=2, track=0.25, bold=0.03, ital=0.2,
  stroke=("#FF2D95", 0.02), shadow=("#FF2D95", 0.04, 0.05, 0.1, 0.9), glow=("#FF2D95", 0.3, 2.4), part=("bokeh", 0.8, 1, 0.5, "#00E5FF"), grade=(1.1, 1.1, 0.5, "#14003A", "#FF8AD0"))
p("187 Interstellar: Gargantua", "helvl", "#F0F5FF", A("focus", 48, 0.9, 0.4, dir="upleft", curve="easeoutexpo"), A("wave", 48, 0.017, 0.001), A("wipe", 24, dir="right", curve="linear"), order="random", delay=3, track=0.4,
  glow=("#FFB347", 0.4, 2.0), chroma=0.04, flare=(0.4, "#FFB86A"), part=("dust", 0.6, 0.6, 0.4, "#FFE0B0"), grain=0.25, grainall=0.3, grade=AMBER)
p("188 Dune: Part Two", "futcond", "#FFD796", A("zoom", 40, 1.05, 1, curve="easeoutquad"), A("wave", 48, 0.006, 0.001), A("dissolve", 20, 0, 0.03), delay=2, track=0.3, bold=0.05,
  shadow=("#000000", 0.03, 0.05, 0.1, 0.9), tex=("sand", 0.5, 1), part=("dust", 1.4, 1, 2.0, "#E8C088"), fog=(0.22, "#E8B070"), grade=DESERT, flare=(0.25, "#FFC890"))
p("189 Baby Driver: Soundtrack Cut", "impact", "#FFFFFF", A("pop", 12, 1.2, 0, curve="steps"), A("pulse", 8, 0.03), A("cut"), order="all", track=0.15, shadow=("#000000", 0.05, 0.06, 0, 1),
  tex=("brushed metal", 0.25, 1), part=("rain", 0.2, 1, 1, "#FFFFFF"), flashc="#FFFFFF", glow=("#FFFFFF", 0.06, 0.3))
p("190 Parasite: The Flood", "helv", "#D2E1DC", A("slide", 36, E(60), 1, dir="up", curve="easeoutexpo"), A("wave", 12, 0.005, 0.001), A("fade", 18), delay=2, track=0.18, bold=0.04, glow=("#B8D8E0", 0.05, 0.3),
  part=("rain", 2.2, 1, 1.5, "#B8C8D0"), tex=("concrete", 0.4, 1), grade=GREENGRAY, vig=(0.6, 0.5))
# ------------------------------------------------------------------ 201 - 240
p("201 Malcolm: Opening Chaos", "impact", "#FFE600", A("strobe", 6, 0.1), A("jitter", 1, E(1.2), 4), A("stretch", 2, 1.4, 0.8), order="all", track=-0.02, stroke=("#000000", 0.07), scan=(0.12, 2), chroma=0.04, grainall=0.2,
  grade=(1.25, 1.1, 0.5, "#000000", "#FFFFFF"), fps=12)
p("202 Jurassic Park: Isla Nublar", "trajan", "#FFC832", A("zoom", 36, 1.06, 1, curve="easeoutquad"), A("wave", 48, 0.008, 0.001), A("fade", 18), order="center", delay=2, track=0.3, bold=0.03, shadow=("#000000", 0.03, 0.05, 0.1, 0.8),
  bevel=0.5, fog=(0.3, "#A8C0A0"), flare=(0.35, "#FFD070"), grade=(1.1, 1.1, 0.6, "#0A1A08", "#FFD070"), grainall=0.35)
p("203 The Matrix: Digital Rain", "mono", "#B4FFB4", A("scramble", 12), A("flicker", 4, 0.15, 0, 1), A("cut"), delay=1, track=0.0, bold=0.03, glow=("#00FF41", 0.25, 2.2), part=("rain", 0.6, 0.8, 0.8, "#00FF41"),
  scan=(0.3, 3), grade=(1.0, 1.2, 0.7, "#001A08", "#B4FFB4"))
p("204 Fight Club: Paper Street Soap", "impact", "#F0F5E6", A("flash", 10, 1), A("jitter", 2, E(1.5), 2), A("burn", 4, 1, 1), order="all", track=0.15, fps=12, stroke=("#000000", 0.04), grain=0.35, grainall=0.5, scratch=1.0,
  grade=(0.5, 1.25, 0.9, "#0A0E00", "#C8D090"), shadow=("#000000", 0.05, 0.06, 0.05, 0.9))
p("205 Titanic: North Atlantic", "didot", "#FFF0D2", A("spread", 48, 0.25, 1, curve="easeoutexpo"), A("wave", 96, 0.01, 0.001), A("fade", 24), order="center", delay=3, track=0.25, glow=("#FFD27A", 0.12, 0.6),
  grade=(0.8, 1.1, 0.6, "#001833", "#FFD090"), part=("sparkle", 0.5, 0.7, 0.4, "#FFFFFF"), vig=(0.5, 0.55))
p("206 The Matrix: Bullet Time", "fut", "#C8FFDC", A("spinx", 60, 70, 0, curve="easeoutexpo"), A("float", 120, 0.0125, 0.2), A("blur", 20, 0.2, 0.2, dir="left"), delay=3, track=0.25, bold=0.03,
  glow=("#00FF41", 0.18, 1.4), scan=(0.15, 3), grade=(0.9, 1.2, 0.8, "#001A08", "#C8FFDC"))
p("207 The Usual Suspects", "courierb", "#191914", A("step"), "none", A("cut"), delay=1, track=0.1, plate=("#D8CBA8", 0.95, 0.5), tex=("paper", 0.45, 1), grade=(0.4, 1.2, 0.8, "#1A1408", "#E0D6B8"), vig=(0.5, 0.55))
p("208 The Silence of the Lambs", "helv", "#D2DCD7", A("zoom", 40, 1.08, 2, curve="easeoutexpo"), A("flicker", 120, 0.1, 0, 1), A("fade", 20), order="all", track=0.25, shadow=("#000000", 0.0, 0.05, 0.2, 0.9),
  tex=("concrete", 0.25, 1), vig=(0.9, 0.45), grade=GREENGRAY, grain=0.2)
p("209 Pulp Fiction", "impact", "#F5F0DC", A("pop", 10, 1.3, 0, curve="fastlinear"), A("jitter", 1, E(0.5), 0), A("stretch", 3, 1.5, 0.8), order="all", track=-0.01, stroke=("#000000", 0.05), shadow=("#B01E1E", 0.05, 0.06, 0, 1),
  tex=("paper", 0.5, 1), grade=(1.15, 1.35, 0.7, "#100800", "#F0E0A0"), grainall=0.3)
p("210 Independence Day", "euro", "#FFFFFF", A("slide", 12, E(400), 0, dir="up", curve="fastlinear"), A("jitter", 1, E(1.5), 0), A("whiteout", 2, 1), order="all", track=0.2, bold=0.03, shadow=("#000000", 0.06, 0.08, 0.05, 1),
  glow=("#FF8C3C", 0.3, 1.8), part=("embers", 0.8, 1.2, 1.3, "#FF8C3C"), fog=(0.25, "#202020"), grade=(1.1, 1.3, 0.8, "#080500", "#FFD0A0"), flare=(0.4, "#FFB070"))
p("211 10 Things I Hate About You", "fut", "#FFEBF0", A("spread", 30, 0.15, 1, curve="easeoutquad"), A("wave", 34, 0.0067, 0.001), A("fade", 15), order="center", delay=2, track=0.18, glow=("#FF9BC8", 0.1, 0.7),
  part=("sparkle", 0.4, 0.7, 0.5, "#FFFFFF"), grain=0.1, grade=(1.1, 0.95, 0.5, "#D0A0E0", "#FFE8F0"))
p("212 Office Space", "helv", "#282828", A("step"), "none", A("wipe", 10, dir="right", curve="linear"), order="all", track=0.1, bold=0.03, plate=("#CFCBBF", 1, 0.5), tex=("paper", 0.3, 1), scan=(0.1, 4),
  grade=(0.4, 0.95, 0.7, "#8A877A", "#E8E4D8"))
p("213 Scream", "impact", "#FFFFFF", A("pop", 8, 1.2, 0, curve="fastlinear"), A("jitter", 1, E(0.8), 0), A("tear", 2, 0.6, curve="linear"), order="all", track=-0.02, shadow=("#C00000", 0.05, 0.06, 0.0, 1),
  glow=("#2D6BFF", 0.08, 0.4), grade=(0.8, 1.4, 0.8, "#000000", "#E8E8F0"), vig=(0.7, 0.5), flashc="#FFFFFF")
p("214 The X-Files: I Want to Believe", "futcond", "#BEDCC8", A("focus", 48, 1.1, 0.2, dir="upleft", curve="easeoutexpo"), A("float", 120, 0.0125, 0.2), A("fade", 24), delay=3, track=0.3, bold=0.03,
  glow=("#40FF90", 0.08, 0.5), fog=(0.4, "#506858"), scan=(0.2, 3), grain=0.2, grainall=0.4, grade=GREENGRAY)
p("215 The Fast and the Furious: Race Wars", "euro", "#FFFFFF", A("slide", 8, E(1000), 0, dir="right", curve="fastlinear"), A("jitter", 1, E(1.5), 0), A("slide", 3, E(1000), 0, dir="left", curve="linear"), order="all", track=0.2, mb=2,
  glow=("#A02DFF", 0.33, 2.2), stroke=("#00E5FF", 0.015), part=("bokeh", 0.8, 1, 3, "#FF2DCB"), grade=(1.15, 1.25, 0.6, "#10002A", "#FFFFFF"))
p("216 Matrix Reloaded: Freeway Chase", "black", "#DCFFE6", A("slide", 8, E(1200), 0, dir="right", curve="fastlinear"), A("jitter", 1, E(1.8), 0), A("cut"), order="all", track=0.22, mb=2, glow=("#00FF41", 0.15, 1.5),
  part=("dust", 0.4, 1, 3, "#C8FFE0"), scan=(0.1, 3), grade=(0.7, 1.25, 0.8, "#001A10", "#DCFFE6"))
p("217 Ronin: Nice Ambush", "helv", "#EBEBF0", A("zoom", 30, 1.0, 2, curve="easeoutquad"), "none", A("fade", 14), order="center", delay=2, track=0.15, bold=0.03, shadow=("#000000", 0.02, 0.03, 0.05, 0.8),
  grade=COLD, grain=0.15, grainall=0.2)
p("218 Chocolat", "didot", "#FFE1B4", A("spread", 36, 0.15, 1, curve="easeoutquad"), A("wave", 72, 0.0075, 0.001), A("fade", 18), order="center", delay=2, track=0.22, ital=0.2, glow=("#FFB060", 0.1, 0.6),
  tex=("oil impasto", 0.15, 1), grade=(1.0, 1.05, 0.6, "#2A0F00", "#FFD8A0"), vig=(0.5, 0.55), part=("dust", 0.4, 1, 0.4, "#FFD8A0"))
p("219 Cast Away", "georgiab", "#FAF0D2", A("zoom", 40, 1.0, 2, curve="easeoutquad"), A("wave", 50, 0.006, 0.001), A("fade", 20), order="ltr", delay=2, track=0.2, shadow=("#000000", 0.0, 0.03, 0.08, 0.5),
  flare=(0.4, "#FFFFFF"), fog=(0.12, "#E8F4FF"), grade=(1.1, 1.1, 0.5, "#003A5A", "#FFF0C8"))
p("220 Blade: Daywalker Bloodbath", "impact", "#FFFFFF", A("pop", 8, 1.35, 0, curve="fastlinear"), A("pulse", 7, 0.03), A("burn", 2, 0, 1), order="all", track=0.18, shadow=("#C00000", 0.05, 0.06, 0.0, 1),
  glow=("#FF0020", 0.15, 1.2), flashc="#FF1A1A", part=("embers", 0.5, 1, 1.3, "#FF1A1A"), grade=(0.7, 1.4, 0.8, "#000814", "#E8E8F0"), scan=(0.15, 3))
p("221 Fight Club: Insomnia", "courierb", "#D2DCCD", A("focus", 48, 1.0, 0.25, dir="up", curve="easeoutexpo"), A("flicker", 4, 0.3, 0, 1), A("dissolve", 16, 0, 0.1), order="random", delay=3, track=0.2,
  shadow=("#000000", 0.04, 0.05, 0.1, 0.9), grain=0.4, grainall=0.5, grade=(0.5, 1.15, 0.9, "#0A1400", "#C8D8A8"), scan=(0.1, 2))
p("222 Buffy: Sunnydale High", "impact", "#FFE6F0", A("slide", 30, E(100), 1, dir="left", curve="easeoutquad"), A("glowpulse", 72, 0.4), A("fade", 15), delay=2, track=0.15, stroke=("#000000", 0.04), glow=("#A02DFF", 0.15, 0.9),
  part=("sparkle", 0.3, 0.8, 0.5, "#FFFFFF"), grade=(1.1, 1.15, 0.6, "#05082A", "#FFB070"), fog=(0.1, "#6A70A0"))
p("223 Twin Peaks: Fire Walk With Me", "caslon", "#FFDCDC", A("spread", 60, 0.3, 1, curve="easeoutexpo"), A("wave", 120, 0.015, 0.001), A("wipe", 30, dir="right", curve="linear"), order="center", delay=4, track=0.35, ital=0.2,
  glow=("#C00010", 0.25, 2.0), plate=("#6A0010", 0.55, 0.5), fog=(0.2, "#10204A"), grade=(1.0, 1.2, 0.7, "#200000", "#FFD0D0"), grain=0.2)
p("224 Stargate SG-1: Cheyenne Mountain", "euro", "#B4E1FF", A("slide", 36, E(300), 1, dir="up", curve="easeoutexpo"), A("glowpulse", 48, 0.6), A("squashx", 12), delay=2, track=0.22, bold=0.03,
  glow=("#00B4FF", 0.25, 2.2), tex=("concrete", 0.3, 1), scan=(0.2, 3), grade=COLD, part=("sparkle", 0.3, 0.7, 0.8, "#B4E1FF"))
p("225 Friends: Central Perk", "frank", "#FFA03C", A("zoom", 24, 1.15, 1, curve="easeoutquad"), "none", A("fade", 10), order="center", delay=1, track=0.15, bold=0.05, stroke=("#000000", 0.04), grain=0.2, grainall=0.2,
  grade=(1.1, 1.05, 0.5, "#2A1000", "#FFE0A0"))
p("226 Seinfeld: New York Street", "black", "#FFE600", A("pop", 12, 0.9, 0, curve="steps"), A("pulse", 10, 0.02), A("cut"), order="all", track=-0.01, ital=0.2, stroke=("#000000", 0.04), shadow=("#000000", 0.05, 0.06, 0, 1),
  tex=("concrete", 0.25, 1), grade=(1.05, 1.1, 0.5, "#2A0A00", "#FFD890"))
p("227 Blade: Blood Rain", "impact", "#FFFFFF", A("pop", 8, 1.3, 0, curve="fastlinear"), A("jitter", 1, E(1.6), 0), A("burn", 2, 0, 1), order="all", track=0.2, shadow=("#C00000", 0.05, 0.06, 0, 1),
  part=("rain", 1.5, 1, 1.2, "#C00018"), glow=("#FF0020", 0.12, 1.0), grade=(0.7, 1.4, 0.8, "#000814", "#E8E8F0"), scan=(0.12, 3))
p("228 The Lost World: Jurassic Park", "trajan", "#FFD250", A("zoom", 36, 1.0, 2, curve="easeoutquad"), A("wave", 48, 0.009, 0.001), A("fade", 18), order="center", delay=2, track=0.3, bold=0.03, shadow=("#000000", 0.03, 0.05, 0.1, 0.8),
  bevel=0.45, fog=(0.35, "#90A890"), grade=(1.0, 1.15, 0.6, "#081408", "#FFC870"), grainall=0.35)
p("229 Mission: Impossible", "euro", "#FFFFFF", A("wipe", 10, dir="left", curve="linear"), A("jitter", 1, E(0.5), 0), A("wipe", 3, dir="right", curve="linear"), order="all", track=0.25, bold=0.03, glow=("#FF1A1A", 0.2, 1.8),
  scan=(0.2, 3), flare=(0.25, "#FF1A1A"), grade=(0.6, 1.35, 0.8, "#000810", "#E8E8F0"))
p("230 Men in Black", "fut", "#FFFFFF", A("exposure", 24, 1, 1.0, curve="easeoutexpo"), A("flicker", 60, 0.3, 0, 1), A("burn", 4, 0, 1), order="center", delay=2, track=0.2, bold=0.03, shadow=("#C8C8D0", 0.02, 0.03, 0.0, 1),
  glow=("#40FF70", 0.06, 0.4), grade=(0.5, 1.35, 0.9, "#000000", "#FFFFFF"))
p("231 The Sixth Sense", "caslon", "#D2E1EB", A("zoom", 48, 1.0, 2, curve="easeoutexpo"), A("wave", 60, 0.0083, 0.001), A("fade", 24), order="all", track=0.25, shadow=("#000000", 0.0, 0.04, 0.15, 0.9),
  fog=(0.2, "#B0C0D0"), part=("dust", 0.3, 1, 0.3, "#E8F0F8"), vig=(0.8, 0.5), grade=COLD, grain=0.2)
p("232 Armageddon", "impact", "#FFFFFF", A("slide", 10, E(400), 0, dir="up", curve="fastlinear"), A("jitter", 1, E(1.7), 0), A("burn", 3, 2, 1), order="all", track=0.2, shadow=("#000000", 0.06, 0.08, 0.05, 1),
  glow=("#FF8C3C", 0.3, 1.8), part=("embers", 1.0, 1.4, 1.5, "#FF8C3C"), fog=(0.25, "#301800"), flare=(0.4, "#FFB070"), grade=(1.15, 1.3, 0.8, "#001830", "#FFC890"))
p("233 The Blair Witch Project", "courierb", "#E6EBDC", A("step"), A("jitter", 2, E(2), 3), A("cut"), order="all", track=0.05, fps=12, chroma=0.07, scan=(0.3, 2), grain=0.5, grainall=0.7, scratch=0.4,
  part=("static", 0.35, 1, 1, "#FFFFFF"), grade=(0.55, 1.1, 0.8, "#06120A", "#C8D8B8"))
p("234 Fast and Furious: LA Street Race", "euro", "#FFFFFF", A("slide", 8, E(1100), 0, dir="right", curve="fastlinear"), A("jitter", 1, E(1.5), 0), A("slide", 3, E(1100), 0, dir="left", curve="linear"), order="all", track=0.2, mb=2,
  glow=("#00FF90", 0.33, 2.2), stroke=("#00E5FF", 0.015), part=("bokeh", 0.8, 1, 3, "#FF2D2D"), grade=(1.15, 1.25, 0.6, "#001A18", "#FFFFFF"))
p("235 Signs: Crop Circles", "georgiab", "#FFF0C8", A("zoom", 40, 1.0, 2, curve="easeoutexpo"), A("wave", 40, 0.0067, 0.001), A("fade", 20), order="center", delay=2, track=0.2, shadow=("#000000", 0.02, 0.04, 0.1, 0.6),
  fog=(0.1, "#F0E0B0"), part=("dust", 0.3, 1, 0.4, "#F0E0B0"), grade=(1.0, 1.1, 0.6, "#1A1400", "#FFE8A0"), grain=0.2)
p("236 Blade II: House of Pain", "impact", "#FFFFFF", A("pop", 8, 1.35, 0, curve="fastlinear"), A("jitter", 1, E(1.5), 0), A("cut"), order="all", track=0.18, shadow=("#C00000", 0.05, 0.06, 0, 1), glow=("#FF0030", 0.15, 1.2),
  tex=("rust", 0.35, 1), fog=(0.12, "#601010"), grade=(0.7, 1.4, 0.8, "#100000", "#E8E8F0"), scan=(0.12, 3))
p("237 Panic Room", "black", "#E1E6EB", A("zoom", 48, 1.06, 2, curve="easeoutexpo"), "none", A("wipe", 16, dir="right", curve="linear"), order="all", track=0.25, bevel=0.4, tex=("brushed metal", 0.4, 1),
  shadow=("#000000", 0.0, 0.06, 0.2, 0.9), vig=(0.6, 0.5), grade=(0.5, 1.3, 0.8, "#05080C", "#C8D2DC"))
p("238 Finding Nemo", "rounded", "#FFDC64", A("spread", 30, 0.15, 1, curve="easeoutquad"), A("wave", 36, 0.01, 0.001), A("dissolve", 15, 0, 0.01), order="center", delay=2, track=0.15, bold=0.03, stroke=("#FF7A00", 0.06),
  glow=("#FFFFFF", 0.06, 0.4), bevel=0.6, part=("bubbles", 0.9, 0.8, 0.8, "#E8FFFF"), flare=(0.25, "#C8F8FF"), grade=(1.25, 1.05, 0.5, "#006A8A", "#FFFFFF"))
p("239 Signs: Full Moon", "georgiab", "#DCE6F5", A("zoom", 36, 1.0, 2, curve="easeoutexpo"), A("wave", 48, 0.0067, 0.001), A("fade", 18), delay=2, track=0.2, shadow=("#000000", 0.02, 0.04, 0.15, 0.9),
  vig=(0.8, 0.5), fog=(0.12, "#8090B0"), grade=INDIGO, grain=0.2)
p("240 The Matrix Revolutions: Machine City", "euro", "#FFC878", A("scramble", 20), A("glowpulse", 60, 0.7), A("dissolve", 30, 0, 0.0), order="random", delay=3, track=0.35, bold=0.03, glow=("#FFB464", 0.25, 2.2),
  tex=("rust", 0.35, 1), bevel=0.4, scan=(0.15, 3), grade=(0.95, 1.2, 0.8, "#201000", "#FFD090"), part=("embers", 0.4, 0.8, 0.6, "#FFB464"))

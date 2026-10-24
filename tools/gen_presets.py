import os, sys
sys.path.insert(0, os.path.dirname(__file__))
from presets_src import PRESETS
out = os.path.join(os.path.dirname(__file__), "..", "src", "TitlePresetsData.inc")
with open(out, "w", encoding="utf-8") as f:
    for line in PRESETS:
        assert '"' not in line and "\\" not in line
        f.write('    "%s",\n' % line)
print(len(PRESETS), "presets ->", out)

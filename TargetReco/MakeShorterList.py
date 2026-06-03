#!/usr/bin/env python3
import sys
import re

FILE_RE = re.compile(r"_lvl\d_(\d{2})_(\d+)\.root")

def sort_key(path):
    m = FILE_RE.search(path)
    if not m:
        return (10**12, 10**12)

    i = int(m.group(1))   # 00..04
    j = int(m.group(2))   # 000..N
    return (j, i)

if len(sys.argv) != 3:
    print("Usage: RunNumber NPaths")
    sys.exit(1)

input_file  = f"ListDir/run_00{sys.argv[1]}.list"
output_file = f"ListDir/run_00{sys.argv[1]}_{sys.argv[2]}files.list"
N = int(sys.argv[2])

with open(input_file) as f:
    files = [l.strip() for l in f if l.strip()]

files_sorted = sorted(files, key=sort_key)

subset = files_sorted[:N]

with open(output_file, "w") as f:
    for p in subset:
        f.write(p + "\n")

print(f"Written {len(subset)} files to {output_file}")
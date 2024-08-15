#!/usr/bin/env python3
#
# This script takes commands described in human readable format and generates a commands.dat file
#

import sys
import lusee_commands

infile = sys.argv[1] if len(sys.argv)>1 else "data/commands.txt"
outfile = sys.argv[2] if len(sys.argv)>2 else "data/commands.dat"

of = open(outfile, "w")
for line in open(infile).readlines():
    if "#" in line:
        line = line[:line.index("#")]
    line = line.strip().split()
    if len(line)==0:
        continue
    if len(line)==4:
        rep, cmd, high, low = line
        rep = int(rep)
        cmd = lusee_commands.__dict__[cmd]
        high = lusee_commands.__dict__[high]
        low = int(low, 16)
        of.write(f"{rep} {cmd:02x} {high:02x} {low:02x}\n")
    else:
        print (f"Warning: skipping '{line}'")

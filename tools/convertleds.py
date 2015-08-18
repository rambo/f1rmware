#! /usr/bin/env python2
import sys
import binascii
import struct
import getopt
import re

def print_usage():
    sys.stderr.write('Usage:\n')
    sys.stderr.write('\tconvertleds.py [-d delay] input.led [output.l3d]\n')

options, remainder = getopt.getopt(sys.argv[1:], 'd:o:h', ['delay=', 'outdir=', 'help'])
delay = 50
outdir = "."

for opt, arg in options:
    if opt in ('-d', '--delay'):
        delay = int(arg)
    if opt in ('-o', '--outdir'):
        outdir = arg
    if opt in ('-h', '--help'):
        print_usage()
        sys.exit(0)

if len(remainder) == 0:
    sys.stderr.write('missing input file\n')
    print_usage()
    sys.exit(1)

elif len(remainder) > 2:
    sys.stderr.write('too many arguments\n')
    print_usage()
    sys.exit(1)

filename = remainder[0]

if len(remainder) == 1:
    outfilename = re.sub('\.[^.]*$','.l3d',filename)
    outfilename = outdir + "/" + re.sub('.*/','',outfilename)
    
else:
    outfilename = remainder[1]

with open(filename) as fp:
    contents = fp.read()
    contents = contents.replace(chr(10), '')
    contents = contents.replace(' ', '')
    with open(outfilename, 'w') as fpW:
        fpW.write(struct.pack('>H', delay))
        fpW.write(binascii.unhexlify(contents))

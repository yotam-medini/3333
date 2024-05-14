#!/usr/bin/env python
import sys
import argparse
import pprint
import datetime

ew = sys.stderr.write

C2S = [
    "tbls",
    "ntbl",
    "gnew",
    "try3",
    "add3",
    "nmor",
    "join",
    "clos",
]

S2C = [
    "TBLS",
    "NTBL",
    "JOIN",
    "GSTATE",
    "SET_FOUND",
    "CONNECTION_TAKEN",
    "TABLE_CLOSED",
]

def strnow():
    tnow = datetime.datetime.now()
    ret = tnow.strftime('%Y-%m-%d-%H%M')
    return ret

def gen_cpp_consts(fn):
    f = open(fn, "w")
    f.write(f"// DO NOT EDIT. Auto generated on {strnow()} by {sys.argv[0]}\n");
    magic = "CS_CONSTS_H"
    f.write(f"#if !defined({magic})\n")
    f.write(f"#define {magic} 1\n\n")

    f.write("#include <string>\n\n")
    
    for c2s in C2S:
        uc2s = c2s.upper()
        f.write(f'static const std::string S3333_C2S_{uc2s} = "{c2s}";\n');
    f.write("\n")
    for e, s2c in enumerate(S2C):
        f.write(f"static const unsigned E3333_S2C_{s2c} = {e};\n");
    f.write(f"static const unsigned E3333_S2C_N = {len(S2C)};\n");
        
    f.write(f"\n#endif /* {magic} */\n")
    f.close()
    
def gen_ts_consts(fn):
    f = open(fn, "w")
    f.write(f"// DO NOT EDIT. Auto generated on {strnow()} by {sys.argv[0]}\n");
    f.write("\n");
    for c2s in C2S:
        uc2s = c2s.upper()
        f.write(f'export let S3333_C2S_{uc2s} = "{c2s}";\n');
    f.write("\n")
    for e, s2c in enumerate(S2C):
        f.write(f"export let E3333_S2C_{s2c} = {e};\n");
    f.write(f"export let E3333_S2C_N = {len(S2C)};\n");
    f.close()
    
def main(argv):
    parser = argparse.ArgumentParser(description=
        "Generate C++/server & TypeScript/client constants for 3333");
    parser.add_argument("--cpp", type=str, default="server/cs_consts.h");
    parser.add_argument("--ts", type=str, default="client/src/cs_consts.ts");
    args_parsed = parser.parse_args(argv)
    # ew("args_parsed: %s\n" % pprint.pformat(args_parsed))
    gen_cpp_consts(args_parsed.cpp)
    gen_ts_consts(args_parsed.ts)
    return 0;

if __name__ == "__main__":
    rc = main(sys.argv[1:])
    sys.exit(rc)

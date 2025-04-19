#!/usr/bin/env python
#
# Watchdog for 3333-server.
# This script is expected to be called by crontab
#
# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2011/April/16
#

import os
import sys
import time


def strnow():
    "Return current time as yyyy/mm/dd:HH:MM:SS"
    now = int(time.time())
    nowlocal = time.localtime(now)
    lta = time.localtime(now)
    s = "%d/%02d/%02d:%02d:%02d:%02d" % lta[0:6]
    return s


def safe_open(fn):
    try:
        f = open(fn)
    except:
        f = None
    return f


def backup_if_big(fn: str, sfx: str, max_size: int):
    try:
        sz = os.stat(fn).st_size
        if sz > max_size:
            fn_back = f"{fn[:-len(sfx)]}-1.{sfx}"
            os.system("mv -f {fn} {fn_back}")
    except:
        pass

def dbglog(msg):
    fn = "/tmp/3333-watchdog.log"
    backup_if_big(fn, "log", 0x10000)
    flog = open(fn, "a")
    flog.write(f"{strnow()}: {msg}\n")
    flog.close()


home = os.getenv("HOME")
xserver = f"{home}/bin/3333-server"
pidfn = "/tmp/3333-server.pid"

f = safe_open(pidfn)
pid = None
if f is not None:
    line = f.readline()
    pid = int(line)
    f.close()
    # dbglog("pid=%d" % pid)

alive = False
if pid is not None:
    clfn = f"/proc/{pid}/cmdline"
    # dbglog("clfn='%s'" % clfn)
    f = safe_open(clfn)
    if f is not None:
        progname = ""
        cmdline = f.read()
        alive = cmdline.startswith(xserver)
        dbglog(f"alive={alive}")
        f.close()

rc = 0
if not alive:
    nohup_out = "/tmp/3333-nohup.out"
    backup_if_big(nohup_out, "out", 0x100000)
    cmd = f"nohup nice -n 18 {xserver} > {nohup_out} &"
    dbglog(cmd)
    rc = os.system(cmd)
    dbglog(f"rc={rc}")
sys.exit(rc)

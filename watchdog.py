#!/usr/bin/env python
#
# Watchdog for 3333-server.
# This script is expected to be called by crontab
#
# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2011/April/16
#

import os
import string
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


def dbglog(msg):
    flog = open("/tmp/3333-wd.log", "a")
    flog.write("%s: %s\n" % (strnow(), msg))
    flog.close()


home = os.getenv("HOME")
d3333 = "%s/3333" % home
xserver = "%s/bin/3333-server.py" % d3333
pidfn = "%s/var/3333.pid" % d3333


f = safe_open(pidfn)
pid = None
if f is not None:
    line = f.readline()
    pid = int(line)
    f.close()
    # dbglog("pid=%d" % pid)

alive = False
if pid is not None:
    clfn = "/proc/%d/cmdline" % pid
    # dbglog("clfn='%s'" % clfn)
    f = safe_open(clfn)
    if f is not None:
        progname = ""
        cmdline = f.read()
        # dbglog("cmdline=%s, len=%d" % (cmdline, len(cmdline)))
        ss = string.split(cmdline, '\0')
        if len(ss) > 2:
            progname = ss[1]
        alive = progname == xserver
        dbglog("progname=%s, alive=%d" % (progname, alive))
        f.close()

rc = 0
if not alive:
    dbglog("pyver: %s" % sys.version)
    precmd = "nohup nice -n 18"
    localpy = "/usr/local/bin/python"
    try:
        os.stat(localpy)
        precmd += (" %s" % localpy)
    except:
        dbglog("stat(%s) failed, will use default python" % localpy)
    # ${HOME}/3333/bin/3333-server.py -portfn ${HOME}/3333/var/3333.port 
    cmd = "%s %s/bin/3333-server.py -portfn %s/var/3333.port -pidfn %s &" % (
        precmd, d3333, d3333, pidfn)
    dbglog("%s" % cmd)
    rc = os.system(cmd)
sys.exit(rc)

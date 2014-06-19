import moos.logutils as logutils

for a in logutils.alogentries(open("test.alog")):
    print a.time, a.varname, a.value

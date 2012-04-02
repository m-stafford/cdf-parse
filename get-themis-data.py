#!/usr/local/bin/python2.7
import os
import os.path
import datetime

year = 2009
probes = ["thc"]
inst = ["fgm","esa"]
begin_year = datetime.date(year, 1, 1)
end_year = datetime.date(year, 12, 31)
one_day = datetime.timedelta(days=1)
next_day = begin_year

for day in range(0, 366):  # includes potential leap year
    if next_day > end_year:
        break
    for pname in probes:
        for i in inst:
            file_string = "%(name)s_l2_%(instrument)s_%(date)s_v01.cdf" % {'name' : pname, 'instrument' : i, 'date' : next_day.strftime('%Y%m%d')}
            addr_string = "%(name)s/l2/%(instrument)s/%(year)s/" % {'name' : pname, 'instrument' : i, 'year' : next_day.strftime('%Y')}
            tot_string = "wget -e robots=off http://themis.ssl.berkeley.edu/data/themis/" + addr_string + file_string
            if os.path.isfile(file_string):
                print "File exists!"
            else:
                os.system(tot_string)
    next_day += one_day


#!/usr/bin/env python

# script for plotting energy/error tradeoff for app/kernel

# inputs: app, kernel, base directory
# output directory: directory where all experiment output directories are created

import sys
import os
import re
import matplotlib.pyplot as plt
import csv

def getErrorValues(app, kernel, basedir):
    res = {}
    path = os.path.join(basedir,'{0}/{1}/outputs'.format(app,kernel))    
    for (dirpath, dirnames, filenames) in os.walk(path):
        for f in sorted(filenames):
            b,e = os.path.splitext(f)
            #print('{0} : {1}'.format(b,e))
            fin = open(os.path.join(dirpath,f),'r')
            try:
                val = float(fin.readline())
                res[b] = (val,0.0)
            except ValueError:
                pass
            fin.close()
    return res

def fillEnergyValues(app, kernel, basedir, errvals):
    path = os.path.join(basedir,'{0}/{1}/energy_results'.format(app,kernel))    
    for (dirpath, dirnames, filenames) in os.walk(path):
        for f in sorted(filenames):
            b,e = os.path.splitext(f)
            b = re.sub('-usage', '', b)
            #print('${0}$'.format(b))
            if b in errvals:
                #print('found b')
                #print('{0} : {1}'.format(b,e))
                fin = open(os.path.join(dirpath,f),'r')
                try:
                    reader = csv.reader(fin)
                    energydata = {}
                    for row in reader:
                        #print(row)
                        energydata[row[0]] = row[1]
                    pe = float(energydata['precise_total'][:-2])
                    ae = float(energydata['approx_total'][:-2])
                    #print('{0} : {1}'.format(pe, ae))
                    errvals[b] = (errvals[b][0],1.0-(ae/pe))
                except ValueError:
                    pass
                fin.close()
    return errvals

# main routine: run all experiments
# args:
# 1. app
# 2. kernel
# 3. base directory containing results (expect: basedir/app/kernel/[energy_results,outputs]
def main(app, kernel, basedir):
    
    #print('{0}/{1}: {2}'.format(app,kernel,basedir))

    errvals = getErrorValues(app, kernel, basedir)
    res = fillEnergyValues(app, kernel, basedir, errvals)

    keys = sorted(res.keys())
    errvals = []
    energyvals = []
    for k in keys:
        (x,y) = res[k]
        errvals.append(x)
        energyvals.append(y)
    
    plt.plot(errvals,energyvals,'ro')
    plt.xlabel('error')
    plt.ylabel('energy savings')
    plt.axis([0.0,1.0,0.0,1.0])
    plt.show()


def usage():
    print('usage: python plot.py app kernel basedir')
    exit(0)

if __name__ == "__main__":
    if len(sys.argv) == 4:
        main(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        usage()


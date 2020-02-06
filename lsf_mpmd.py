import sys
import os

project = "CSC143"

ranksPerNode = 40
cpusPerRank = 4

params={}
params["overlapping_rs"] = "warn"
params["oversubscribe_cpu"] = "warn"
params["oversubscribe_mem"] = "allow"
params["oversubscribe_gpu"] = "allow"
params["launch_distribution"] = "packed"

rankCurrent = 0
appCurrent = 0
cpuCurrent = 0
nodes = 0

appsFullPath = []
apps = []
ranks = []

filename = ""

for arg in range(1, len(sys.argv), 2):
    appsFullPath.append(os.getcwd() + "/" + sys.argv[arg])
    apps.append(sys.argv[arg])
    ranks.append(int(sys.argv[arg+1]))
    filename = filename + sys.argv[arg] + sys.argv[arg+1]

ranksTotal = sum(ranks)

fileErf = open(filename + ".erf","w")
fileJob = open(filename + ".job","w")

s = 0
for app in appsFullPath:
    fileErf.write("app {0}: {1}".format(s, app) + "\n")
    s = s + 1

for key, value in params.items():
    fileErf.write(key + ": " + value + "\n")

for app in ranks:
    for rank in range(app):
        if rankCurrent % ranksPerNode == 0:
            cpuCurrent = 0
            nodes = nodes + 1
        line = "rank: {0}: {{ host: {1}; cpu: {{{2}-{3}}} }} : app {4}".format(rankCurrent, nodes, cpuCurrent, cpuCurrent + 3, appCurrent)
        fileErf.write(line + "\n")
        cpuCurrent = cpuCurrent + cpusPerRank
        rankCurrent = rankCurrent + 1
    appCurrent = appCurrent + 1

fileJob.write("#!/bin/bash" + "\n")
fileJob.write("#BSUB -P {0}".format(project) + "\n")
fileJob.write("#BSUB -J job_{0}".format(filename) + "\n")
fileJob.write("#BSUB -W 2:00" + "\n")
fileJob.write("#BSUB -nnodes {0}".format(nodes) + "\n")
fileJob.write("cd {0}".format(os.getcwd()) + "\n")
fileJob.write("jsrun --erf_input {0}/{1}.erf".format(os.getcwd(),filename) + "\n")

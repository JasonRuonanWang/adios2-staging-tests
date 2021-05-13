import sys
import os

project = "CSC303"
walltime = 10

ranksPerNode = 40
cpusPerRank = 4

params={}
params["overlapping_rs"] = "warn"
params["oversubscribe_cpu"] = "warn"
params["oversubscribe_mem"] = "allow"
params["oversubscribe_gpu"] = "allow"
params["launch_distribution"] = "packed"

rankCurrent = 0
rankbCurrent = 0
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

SscErf = open(filename + ".ssc.erf","w")
SstErf = open(filename + ".sst.erf","w")
ImpiErf = open(filename + ".impi.erf","w")

SscJob = open(filename + ".ssc.job","w")
SstJob = open(filename + ".sst.job","w")
ImpiJob = open(filename + ".impi.job","w")

s = 0
for app in appsFullPath:
    SscErf.write("app {0}: {1} ssc".format(s, app) + "\n")
    ImpiErf.write("app {0}: {1} insitumpi".format(s, app) + "\n")
    SstErf.write("app {0}: {1} sst".format(s, app) + "\n")
    s = s + 1

for key, value in params.items():
    SscErf.write(key + ": " + value + "\n")
    SstErf.write(key + ": " + value + "\n")
    ImpiErf.write(key + ": " + value + "\n")

for app in ranks:
    for rank in range(app):
        if rankbCurrent % ranksPerNode == 0:
            cpuCurrent = 0
            nodes = nodes + 1
        line = "rank: {0}: {{ host: {1}; cpu: {{{2}-{3}}} }} : app {4}".format(rankCurrent, nodes, cpuCurrent, cpuCurrent + 3, appCurrent)
        SscErf.write(line + "\n")
        SstErf.write(line + "\n")
        ImpiErf.write(line + "\n")
        cpuCurrent = cpuCurrent + cpusPerRank
        if cpuCurrent == 84:
            cpuCurrent = 88
        rankCurrent = rankCurrent + 1
        rankbCurrent = rankbCurrent + 1
    appCurrent = appCurrent + 1
    cpuCurrent = 0
    rankbCurrent = 0

SscJob.write("#!/bin/bash" + "\n")
SscJob.write("#BSUB -P {0}".format(project) + "\n")
SscJob.write("#BSUB -J job_ssc_{0}".format(filename) + "\n")
SscJob.write("#BSUB -W {0}".format(walltime) + "\n")
SscJob.write("#BSUB -nnodes {0}".format(nodes) + "\n")
SscJob.write("cd {0}".format(os.getcwd()) + "\n")
SscJob.write("jsrun --erf_input {0}/{1}.ssc.erf".format(os.getcwd(),filename) + "\n")

SstJob.write("#!/bin/bash" + "\n")
SstJob.write("#BSUB -P {0}".format(project) + "\n")
SstJob.write("#BSUB -J job_sst_{0}".format(filename) + "\n")
SstJob.write("#BSUB -W {0}".format(walltime) + "\n")
SstJob.write("#BSUB -nnodes {0}".format(nodes) + "\n")
SstJob.write("cd {0}".format(os.getcwd()) + "\n")
SstJob.write("jsrun --erf_input {0}/{1}.sst.erf".format(os.getcwd(),filename) + "\n")

ImpiJob.write("#!/bin/bash" + "\n")
ImpiJob.write("#BSUB -P {0}".format(project) + "\n")
ImpiJob.write("#BSUB -J job_impi_{0}".format(filename) + "\n")
ImpiJob.write("#BSUB -W {0}".format(walltime) + "\n")
ImpiJob.write("#BSUB -nnodes {0}".format(nodes) + "\n")
ImpiJob.write("cd {0}".format(os.getcwd()) + "\n")
ImpiJob.write("jsrun --erf_input {0}/{1}.impi.erf".format(os.getcwd(),filename) + "\n")




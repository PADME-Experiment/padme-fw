#!/usr/bin/env python3

import sys
import os.path

usage= """Usage: \n
PrepareUserAnalysis.py AnalysisName\n
Please respect the analysis naming schema: 
\t - Start with capital letter
\t - Use capital letter for every logical part of the analysis name
\t - Try to provide a meaningful name, it may become an official one
\t - Good examples: PVetoFineTimeCalibration, TargetPositionExtraction
\t - Bad examples: pvetoTest, EventSelection
"""



def ChangeStringInFile(old,new,oldstring,newstring):
    with open(old,"r") as fin:
        with open(new,"w") as fout:
            for line in fin:
                newline = line.replace(oldstring,newstring)
                fout.write(newline)



if len(sys.argv) != 2:
    print(usage)
    exit()
    
#exit()

AnalysisName = sys.argv[1]

print("Preparing infrastructure for analysis  ", AnalysisName)
    
TemplateHeaderFile = "UserAnalysis/include/UserTemplateAnalyser.hh"
TemplateSourceFile = "UserAnalysis/src/UserTemplateAnalyser.cc"
OutputHeaderFile = "UserAnalysis/include/"+AnalysisName+".hh"
OutputSourceFile = "UserAnalysis/src/"+AnalysisName+".cc"

ConfigFile = "config/AnalysersChain.cfg"

print("Output header file: ",OutputHeaderFile)
print("Output source file: ",OutputSourceFile)

if os.path.isfile(OutputHeaderFile) or os.path.isfile(OutputSourceFile):
    print("Analyser with name ",AnalysisName, " seem to exist. Better exit")
    exit()


ChangeStringInFile(TemplateHeaderFile,OutputHeaderFile,"UserTemplateAnalyser",AnalysisName)
ChangeStringInFile(TemplateSourceFile,OutputSourceFile,"UserTemplateAnalyser",AnalysisName)

with open(ConfigFile,"a") as fout:
    fout.write(AnalysisName+"\n")

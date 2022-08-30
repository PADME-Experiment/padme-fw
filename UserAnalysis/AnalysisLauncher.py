import os
import sys
import commands
import math

#compile PadmeReco
os.system("make -j8")

#ask which run you want to analyse
DataToRead = raw_input("Enter the address of the reconstructed data in format /data05/padme/beth/Runxxx_Conditions/:")

print("\n-----------------\n")

#find data from selected run
print("Total raw files reconstructed in "+DataToRead+":")
os.system("less "+DataToRead+"*List.txt | wc -l")
print("Data files have 1e3 events each, MC files probably have 1e2 events each.")

#ask how many files total you want to analyse
TotFiles = int(input("Total raw files to analyse:"))

if TotFiles<1:
    TotFiles = 1
print "Analysing",TotFiles,"files"

#work out how to distribute the jobs
NoCoresToUse = float(commands.getstatusoutput("ls -d "+DataToRead+"SubList* | wc -l")[1])

MinFilesPerCore = int(math.floor(TotFiles/NoCoresToUse))
MaxFilesPerCore = int(math.ceil(TotFiles/NoCoresToUse))

NoCoresWithMin = int(NoCoresToUse - TotFiles%NoCoresToUse)
NoCoresWithMax = int(NoCoresToUse - NoCoresWithMin)

print "MinFilesPerCore",MinFilesPerCore,"MaxFilesPerCore",MaxFilesPerCore,"NoCoresWithMin",NoCoresWithMin,"NoCoresWithMax",NoCoresWithMax

#check which version of UserAnalysis you want to use
PadmeFWToUse = commands.getstatusoutput("(cd "+DataToRead+" && ls -d *padme-fw*)")[1]

PadmeAnalysisToUse = PadmeFWToUse+"/UserAnalysis/Analyzers"
PadmeAnalysisCheckString = "PadmeAnalysisToUse set to " +PadmeAnalysisToUse+". Is this what you want? Type y for yes, n for no: "
PadmeAnalysisCheck = raw_input(PadmeAnalysisCheckString)
if PadmeAnalysisCheck=="n":
    PadmeAnalysisToUse = raw_input("Insert your desired output directory here: ")

#if a version of UserAnalysis already exists there then rm it
AnalysisReplaceCheckString = "about to rm everything in "+DataToRead+PadmeAnalysisToUse+". Proceed? Type y for yes, n for no:"
AnalysisReplaceCheck = raw_input(AnalysisReplaceCheckString)

if AnalysisReplaceCheck=="y":
    os.system("rm -rf "+DataToRead+PadmeAnalysisToUse+"*")
else:
    sys.exit()

#copy correct version of UserAnalysis to PadmeFW directory
CopyCommand = "cp -r /home/long/"+PadmeAnalysisToUse+" "+DataToRead+PadmeAnalysisToUse
print(CopyCommand)
os.system(CopyCommand)

print DataToRead+PadmeFWToUse

#check number of events per file
MCorData = raw_input("Are the reco files from data? Type d for data, m for MC:")
if MCorData == "d":
    EventChecker = raw_input("Assuming 1000 events per reco file. Type y if this is correct, n if this is incorrect:")
    if EventChecker == 'y':
        EventsPerFile =1000
    else:
        EventsPerFile = raw_input("Type the correct number of events per file here:")

elif MCorData == "m":
    EventChecker = raw_input("Assuming 100 events per reco file. Type y if this is correct, n if this is incorrect:")
    if EventChecker == 'y':
        EventsPerFile =100
    else:
        EventsPerFile = raw_input("Type the correct number of events per file here:")

else:
    print "Data type unclear. Exiting"
    sys.exit()

print(type(EventsPerFile))
    
#put runs to be used into a file list
ListFileName = "AnalysisRuns.txt"

lsCommand = "ls -d "+DataToRead+"SubList*"
SubDirList = commands.getstatusoutput(lsCommand)[1]
SubDirList = SubDirList.split("\n")

with open(ListFileName,"w") as fulllist: 
   # nwritten=0
    for SubDir in SubDirList:
        print("MySubDir "+SubDir)
        #which subdirectory are we talking about?
        SubDirNo = int(SubDir.split("_")[-1])
        #create symlinks to PadmeAnalysis and config file
        if os.path.exists(SubDir+"/PadmeAnalysis"):
            os.system("rm "+SubDir+"/PadmeAnalysis")
        exesymlink = "ln -s "+DataToRead+PadmeFWToUse+"/UserAnalysis/PadmeAnalysis "+SubDir+"\n"
        os.system(exesymlink)
        if os.path.exists(SubDir+"/AnalysisConfig.conf"):
            os.system("rm "+SubDir+"/AnalysisConfig.conf")
        configsymlink = "ln -s "+DataToRead+PadmeFWToUse+"/UserAnalysis/config/UserAnalysis.conf "+SubDir+"/AnalysisConfig.conf\n"
        os.system(configsymlink)

        #write source file to run analysis
        if SubDirNo<NoCoresWithMax:
            print "SubDirNo ",SubDirNo,"NoCoresWithMax",NoCoresWithMax,"MinFilesPerCore",MinFilesPerCore,"MaxFilesPerCore",MaxFilesPerCore,"EventsPerFile",EventsPerFile,"int(MaxFilesPerCore)*int(EventsPerFile)",int(MaxFilesPerCore)*int(EventsPerFile)
            fulllist.write("nohup ./PadmeAnalysis -i "+SubDir+"/RecoOutputFile.root -n "+str(int(MaxFilesPerCore)*int(EventsPerFile))+" -o "+SubDir+"/AnalysisOutputFile.root -v 1 -c "+SubDir+"/AnalysisConfig.conf &> "+SubDir+"/logAnalysis.txt &")
        else:
            print "SubDirNo ",SubDirNo,"NoCoresWithMin",NoCoresWithMin,"MinFilesPerCore",MinFilesPerCore,"int(MaxFilesPerCore)",MaxFilesPerCore,"EventsPerFile",EventsPerFile,"int(MinFilesPerCore)*EventsPerFile",int(MinFilesPerCore)*int(EventsPerFile)
            fulllist.write("nohup ./PadmeAnalysis -i "+SubDir+"/RecoOutputFile.root -n "+str(int(MinFilesPerCore)*int(EventsPerFile))+" -o "+SubDir+"/AnalysisOutputFile.root -v 1 -c "+SubDir+"/AnalysisConfig.conf &> "+SubDir+"/logAnalysis.txt &")
        fulllist.write("\n")
    fulllist.write("cd /home/long/"+PadmeFWToUse+"/UserAnalysis")

print("AnalysisRuns.txt")

import os
import sys
import math
import glob
#compile PadmeReco
os.system("make -j16")

#ask which run you want to analyse
RecoDataSearchAddress = input("Enter the address of the reconstructed data in format /data05/padme/beth/Runxxx_Conditions/:")
RecoDataToAnalyse = glob.glob(os.path.join(RecoDataSearchAddress,"**","*SubList*","*RecoOutput*.root"), recursive = True)
RecoDataAddress = RecoDataToAnalyse[0].split("SubList")[0]
RecoDataAddress = glob.glob(RecoDataAddress)[0]
print("\n-----------------\n")

#find data from selected run

#check number of events per file
MCorData = input("Are the reco files from data? Type d for data, m for MC:")
if MCorData == "d":
    print("Total raw files reconstructed in "+RecoDataAddress+":")
    ListFile = glob.glob(os.path.join(RecoDataSearchAddress,"**",'Run*List.txt'), recursive = True)[0]
    RawFiles = sum(1 for _ in open(ListFile))
    print(RawFiles)
    print("Data files have 1e3 events each, MC files probably have 1e2 events each.")
    EventChecker = input("Assuming 1000 events per raw file. Type y if this is correct, n if this is incorrect:")
    if EventChecker == 'y':
        EventsPerFile =1000
    else:
        EventsPerFile = input("Type the correct number of events per file here:")

elif MCorData == "m":
    print("Total raw files reconstructed in "+RecoDataAddress+":")
    ListFile = glob.glob(os.path.join(RecoDataSearchAddress,"**",'MCList.txt'), recursive = True)[0]
    RawFiles = sum(1 for _ in open(ListFile))
    print(RawFiles)
    print("Data files have 1e3 events each, MC files probably have 1e2 events each.")
    EventChecker = input("Assuming 100 events per raw file. Type y if this is correct, n if this is incorrect:")
    if EventChecker == 'y':
        EventsPerFile =100
    else:
        EventsPerFile = input("Type the correct number of events per file here:")

else:
    print ("Data type unclear. Exiting")
    sys.exit()

#ask how many files total you want to analyse
TotFiles = int(input("Total reco files to analyse:"))

if TotFiles<1:
    TotFiles = 1
print("Analysing",TotFiles,"files")

#work out how to distribute the jobs
NoCoresToUse = len(RecoDataToAnalyse)

MinFilesPerCore = int(math.floor(TotFiles/NoCoresToUse))
MaxFilesPerCore = int(math.ceil(TotFiles/NoCoresToUse))

NoCoresWithMin = int(NoCoresToUse - TotFiles%NoCoresToUse)
NoCoresWithMax = int(NoCoresToUse - NoCoresWithMin)

print("MinFilesPerCore",MinFilesPerCore,"MaxFilesPerCore",MaxFilesPerCore,"NoCoresWithMin",NoCoresWithMin,"NoCoresWithMax",NoCoresWithMax)

#check which version of UserAnalysis you want to use
if(RecoDataAddress.endswith("Reco_and_Analysis/")):
    RawDataAddress=RecoDataAddress.split("Reco_and_Analysis/")[0]
    print("RecoDataAddress is",RecoDataAddress," RawDataAddress is",RawDataAddress)
else:
    RawDataAddress = RecoDataAddress
    print("RawDataAddress is",RawDataAddress)

PadmeFWToUse = glob.glob(os.path.join(RawDataAddress,"*padme-fw*"))[0]
PadmeFWToUse = PadmeFWToUse.split(RawDataAddress)[1]

PadmeAnalysisToUse = os.getcwd()
PadmeAnalysisCheckString = "PadmeAnalysisToUse set to " +PadmeAnalysisToUse+". Is this what you want? Type y for yes, n for no: "
PadmeAnalysisCheck = input(PadmeAnalysisCheckString)
if PadmeAnalysisCheck=="n":
    PadmeAnalysisToUse = input("Insert your desired analysis directory here: ")

#if a version of UserAnalysis already exists there then rm it
AnalysisRemovePath = os.path.join(RawDataAddress,PadmeFWToUse,"UserAnalysis")
AnalysisReplaceCheckString = "about to rm everything in "+AnalysisRemovePath+". Proceed? Type y for yes, n for no:"
AnalysisReplaceCheck = input(AnalysisReplaceCheckString)

if AnalysisReplaceCheck=="y":
    os.system("rm -rf "+AnalysisRemovePath+"*")
else:
    sys.exit()

#copy correct version of UserAnalysis to PadmeFW directory
CopyCommand = 'rsync -r --exclude "*.root" --exclude "tmp/" --exclude "*.list" ' + os.path.join('~/',PadmeAnalysisToUse)+' '+ os.path.join(RawDataAddress,PadmeFWToUse)
#"cp -r /home/long/"+PadmeAnalysisToUse+" "+RecoDataAddress+PadmeAnalysisToUse
print(CopyCommand)
os.system(CopyCommand)
    
#put runs to be used into a file list
ListFileName = "AnalysisRuns.txt"

SubDirList = glob.glob(os.path.join(RecoDataAddress,"SubList*"))

with open(ListFileName,"w") as fulllist: 
    for SubDir in SubDirList:
        SubDirNo = int(SubDir.split("_")[-1])
        #create symlinks to PadmeAnalysis and config file
        if os.path.exists(os.path.join(SubDir,"PadmeAnalysis")):
            os.system("rm "+os.path.join(SubDir,"PadmeAnalysis"))
        exesymlink = "ln -sf "+os.path.join(RawDataAddress,PadmeFWToUse,"UserAnalysis/PadmeAnalysis ")+SubDir+"\n"
        os.system(exesymlink)
        if os.path.exists(os.path.join(SubDir+"AnalysisConfig.conf")):
            os.system("rm "+os.path.join(SubDir+"AnalysisConfig.conf"))
        configsymlink = "ln -sf "+os.path.join(RawDataAddress,PadmeFWToUse,"UserAnalysis/config/UserAnalysis.conf")+" "+os.path.join(SubDir,"AnalysisConfig.conf")+"\n"
        os.system(configsymlink)

        #write source file to run analysis
        if SubDirNo<NoCoresWithMax:
            print("nohup ./PadmeAnalysis -i "+os.path.join(SubDir,"RecoOutputFile.root")+" -n "+str(int(MaxFilesPerCore)*int(EventsPerFile))+" -o "+os.path.join(SubDir,"AnalysisOutputFile.root")+" -v 1 -c "+os.path.join(SubDir,"AnalysisConfig.conf")+" &> "+os.path.join(SubDir,"logAnalysis.txt")+" &")
            fulllist.write("nohup ./PadmeAnalysis -i "+os.path.join(SubDir,"RecoOutputFile.root")+" -n "+str(int(MaxFilesPerCore)*int(EventsPerFile))+" -o "+os.path.join(SubDir,"AnalysisOutputFile.root")+" -v 1 -c "+os.path.join(SubDir,"AnalysisConfig.conf")+" &> "+os.path.join(SubDir,"logAnalysis.txt")+" &")
        else:
            print("nohup ./PadmeAnalysis -i "+os.path.join(SubDir,"RecoOutputFile.root")+" -n "+str(int(MinFilesPerCore)*int(EventsPerFile))+" -o "+os.path.join(SubDir,"AnalysisOutputFile.root")+" -v 1 -c "+os.path.join(SubDir,"AnalysisConfig.conf")+" &> "+os.path.join(SubDir,"logAnalysis.txt")+" &")
            fulllist.write("nohup ./PadmeAnalysis -i "+os.path.join(SubDir,"RecoOutputFile.root")+" -n "+str(int(MinFilesPerCore)*int(EventsPerFile))+" -o "+os.path.join(SubDir,"AnalysisOutputFile.root")+" -v 1 -c "+os.path.join(SubDir,"AnalysisConfig.conf")+" &> "+os.path.join(SubDir,"logAnalysis.txt")+" &")
        fulllist.write("\n")

print("AnalysisRuns.txt")

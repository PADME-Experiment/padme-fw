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
NoCoresToUse = 16.
MinFilesPerCore = math.floor(TotFiles/NoCoresToUse)
MaxFilesPerCore = math.ceil(TotFiles/NoCoresToUse)

NoCoresWithMin = NoCoresToUse - TotFiles%NoCoresToUse
NoCoresWithMax = NoCoresToUse - NoCoresWithMin

#put runs to be used into a file list
ListFileName = "AnalysisList.txt"

lsCommand = "ls -d "+DataToRead+"SubList*"
SubDirList = commands.getstatusoutput(lsCommand)[1]
SubDirList = SubDirList.split("\n")

#check which version of UserAnalysis you want to use
PadmeFWToUse = commands.getstatusoutput("ls -d "+DataToRead+"*padme-fw*")[1]

PadmeAnalysisToUse = "220407padme-fw/UserAnalysis/Analyzers"
PadmeAnalysisCheckString = "PadmeAnalysisToUse set to " +PadmeAnalysisToUse+". Is this what you want? Type y for yes, n for no: "
PadmeAnalysisCheck = raw_input(PadmeAnalysisCheckString)
if PadmeAnalysisCheck=="n":
    PadmeAnalysisToUse = raw_input("Insert your desired output directory here: ")

#if a version of UserAnalysis already exists there then rm it
ProceedCheckString = "about to rm everything in "+PadmeAnalysisToUse+". Proceed? Type y for yes, n for no:"
ProceedCheck = raw_input(ProceedCheckString)

if ProceedCheck=="y":
    os.system("rm -rf "+PadmeAnalysisToUse+"*")
else:
    sys.exit()

#copy correct version of UserAnalysis to PadmeFW directory
CopyCommand = "cp -r /home/long/"+PadmeAnalysisToUse+" "+PadmeFWToUse
print(CopyCommand)
#os.system(CopyCommand)


#

with open(ListFileName,"w") as fulllist: 
    nwritten=0
    for SubDir in SubDirList:
        exesymlink = "ln -s "+DataToRead+PadmeFWToUse+"/UserAnalysis/PadmeAnalysis "+SubDir+"\n"
        
        fulllist.write("cd "+SubDir+"\n")
        if(nwritten<NoCoresWithMin):
            fulllist.write("nohup ./PadmeAnalysis -i OutputFile"+str(int(nwritten))+".root -n"+str(int(MinFilesPerCore))+" -o AnalysisOutputFile"+str(int(nwritten))+".root >logAnalysis.txt &\n")
        else:
            fulllist.write("nohup ./PadmeAnalysis -i OutputFile"+str(int(nwritten))+".root -n"+str(int(MaxFilesPerCore))+" -o AnalysisOutputFile"+str(int(nwritten))+".root >logAnalysis.txt &\n")
        nwritten+=1


#SubFiles=[]
#SubDirectories=[]
#SubFileNames=[]
#for ii in range(NoCoresToUse):
#    SubDirectories.append("SubListRunMC_"+str(ii))
#    SubFileNames.append(SubDirectories[ii]+".txt")
#    os.mkdir(MyDir+SubDirectories[ii])
#    os.chdir(MyDir+SubDirectories[ii])
#    #create the text files containing the list of files
#    SubFiles.append(open(MyDir+SubDirectories[ii]+"/"+SubFileNames[ii],"w"))
#
#with open(MyDir+ListFileName,"r") as fulllist:
#    for line in fulllist:
#        SubFiles[LinesPrinted%NoCoresToUse].write(line)
#        LinesPrinted+=1
#
##copy padme-fw
#PadmeFWToUse ="220407padme-fw/" 
#PadmeFWCheckString = "PadmeFWToUse set to " +PadmeFWToUse+". Is this what you want? Type y for yes, n for no: "
#PadmeFWCheck = raw_input(PadmeFWCheckString)
#
#if PadmeFWCheck=="n":
#    PadmeFWToUse = raw_input("Insert your desired output directory here: ")
#
#CopyCommand = "cp -r /home/long/"+PadmeFWToUse+" "+MyDir
#print(CopyCommand)
#os.system(CopyCommand)
#
##create symbolic links and launch reconstruction
#with open("/home/long/"+PadmeFWToUse+"PadmeReco/MCRecoLauncher.txt","w") as RecoLauncher:
#    for ii in range(len(SubFiles)):
#        print("cd "+MyDir+SubDirectories[ii])
#        os.chdir(MyDir+SubDirectories[ii])
#        RecoLauncher.write("cd "+MyDir+SubDirectories[ii]+"\n")
#
#        exesymlink = "ln -s "+MyDir+PadmeFWToUse+"PadmeReco/PadmeReco "+"./"
#        os.system(exesymlink)
#        configsymlink = "ln -s "+MyDir+PadmeFWToUse+"PadmeReco/config "+"./"
#        os.system(configsymlink)
#        
#        launchcomm = "nohup ./PadmeReco -l "+MyDir+SubDirectories[ii]+"/"+SubFileNames[ii]+" -o OutputFile"+str(ii)+".root &> log"+str(ii)+".txt &"
#        print launchcomm
#        RecoLauncher.write(launchcomm+"\n")
#        #os.system(launchcomm)
#    RecoLauncher.write("cd /home/long/"+PadmeFWToUse+"PadmeReco")

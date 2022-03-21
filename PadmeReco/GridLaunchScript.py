import os
import sys
import commands

#compile PadmeReco
os.system("make -j8")

#ask which run you want to reconstruct
ThreeDigitRunNumber = raw_input("Enter the three digit run number:")

#define output directory and make sure it exists
MyDir = "/data05/padme/beth/Run"+ThreeDigitRunNumber+"_NewDigitizer/"
MyDirCheckString = "Output directory set to " +MyDir+". Is this what you want? Type y for yes, n for no: "
MyDirCheck = raw_input(MyDirCheckString)

if MyDirCheck=="n":
    MyDir = raw_input("Insert your desired output directory here: ")

if not os.path.exists(MyDir):
    os.mkdir(MyDir)
   
#clear output directory
ProceedCheckString = "about to rm everything in "+MyDir+". Proceed? Type y for yes, n for no:"
ProceedCheck = raw_input(ProceedCheckString)

if ProceedCheck=="y":
    os.system("rm -r "+MyDir+"*")
else:
    sys.exit()

#find data from selected run
print("\n-----------------\n")
FullRunNumber = "run_0030"+ThreeDigitRunNumber
Address = "root://atlasse.lnf.infn.it//dpm/lnf.infn.it/home/vo.padme.org/daq/2020/rawdata/"
FullRunName = (commands.getstatusoutput("gfal-ls "+Address+"| grep "+FullRunNumber))[1]
DataToRead = Address+FullRunName

print("Total files available in "+DataToRead+":")
lscommand = "gfal-ls "+DataToRead+" | wc -l"
os.system("gfal-ls "+DataToRead+" | wc -l")
print("Files have 1e3 events each.")

#ask how many events total you want to reconstruct
TotEvents = int(input("Total events to reconstruct:"))

#each file contains 10^3 events, so total files to reconstruct = TotEvents/10^3
FullFiles = int(TotEvents/1e3)
if TotEvents<1e3:
    FullFiles = 1
print "Reconstructing",FullFiles,"files =",int(FullFiles*1e3),"events"

#put runs to be used into a file list
ListFileName = "Run"+ThreeDigitRunNumber+"List.txt"
lsCommand = "gfal-ls "+DataToRead+" | head -"+str(FullFiles)
RootFileList = commands.getstatusoutput(lsCommand)[1]
RootFileList=RootFileList.split('\n')

with open(MyDir+ListFileName,"w") as fulllist: 
    for RootFile in RootFileList:
        fulllist.write(DataToRead+"/"+RootFile+"\n")
        
#create sublists to run in parallel
NoCoresToUse = 16
LinesPrinted = 0

SubFiles=[]
SubDirectories=[]
SubFileNames=[]
for ii in range(NoCoresToUse):
    SubDirectories.append("SubListRun"+str(ThreeDigitRunNumber)+"_"+str(ii))
    SubFileNames.append(SubDirectories[ii]+".txt")
    os.mkdir(MyDir+SubDirectories[ii])
    os.chdir(MyDir+SubDirectories[ii])
    #create the text files containing the list of files
    SubFiles.append(open(MyDir+SubDirectories[ii]+"/"+SubFileNames[ii],"w"))

with open(MyDir+ListFileName,"r") as fulllist:
    for line in fulllist:
        SubFiles[LinesPrinted%NoCoresToUse].write(line)
        LinesPrinted+=1

#copy padme-fw
PadmeFWToUse ="220321padme-fw/" 
CopyCommand = "cp -r /home/long/"+PadmeFWToUse+" "+MyDir
print(CopyCommand)
os.system(CopyCommand)

#create symbolic links and launch reconstruction
with open("/home/long/"+PadmeFWToUse+"PadmeReco/RecoLauncher.txt","w") as RecoLauncher:
    for ii in range(len(SubFiles)):
        print("cd "+MyDir+SubDirectories[ii])
        os.chdir(MyDir+SubDirectories[ii])
        RecoLauncher.write("cd "+MyDir+SubDirectories[ii]+"\n")

        exesymlink = "ln -s "+MyDir+PadmeFWToUse+"PadmeReco/PadmeReco "+"./"
        os.system(exesymlink)
        configsymlink = "ln -s "+MyDir+PadmeFWToUse+"PadmeReco/config "+"./"
        os.system(configsymlink)
        
        launchcomm = "nohup ./PadmeReco -l "+MyDir+SubDirectories[ii]+"/"+SubFileNames[ii]+" -o OutputFile"+str(ii)+".root &> log"+str(ii)+".txt &"
        print launchcomm
        RecoLauncher.write(launchcomm+"\n")
        #os.system(launchcomm)
    RecoLauncher.write("cd /home/long/"+PadmeFWToUse+"PadmeReco")

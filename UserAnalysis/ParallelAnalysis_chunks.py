import os
import sys

if len(sys.argv) < 5:
    print("You should run like this --> python ParallelAnalysis.py 'listname_withoutext' 'runNumber' 'NoCoresToUse' 'path/to/your/list/ (mind the last /) or def'")
    sys.exit(-1)

print("########################## building  #############################\n\n")

os.system("make -j16")

runName = sys.argv[1]
runNumber = sys.argv[2]
NoCoresToUse = int(sys.argv[3])
InDir =  sys.argv[4]
if InDir=="def":
    InDir = "/data9Vd1/padme/dimeco/" #default
print(f"I'm looking in {InDir}")
fileList = InDir + runName + ".list"

if not os.path.exists(fileList):
    print("File not existing --> check it " + fileList)
    sys.exit(-1)

OutDir = "/data9Vd1/padme/dimeco/UserAn_out_" + runName
if not os.path.exists(OutDir):
    os.system("mkdir " + OutDir)
    print(OutDir + " folder was created")

Launcher = open("Launcher.txt", "w")

print("\n\n########################## creating Launcher.txt for " + runName + " #############################\n\n")

with open(fileList, "r") as fulllist:
    files = [l.strip() for l in fulllist.readlines()]
    nfiles = len(files)

    # CASE 1: files <= cores 
    if nfiles <= NoCoresToUse:
        for i, f in enumerate(files):
            launchcomm = (
                "nohup ./PadmeAnalysis -i {0} -r {1} -o {2}/{3}_{4}.root "
                "> {2}/logReco_{4}.txt &"
            ).format(f, runNumber, OutDir, runName, i)

            print(launchcomm)
            Launcher.write(launchcomm + "\n")

    # CASE 2: files > cores split into sub-lists
    else:
        print("More files than cores: splitting input list")

        chunk_size = int((nfiles + NoCoresToUse - 1) / NoCoresToUse)  # ceiling division
        print(chunk_size)
        sublists = []

        for i in range(NoCoresToUse):
            sub = files[i * chunk_size:(i + 1) * chunk_size]
            if not sub:
                break
            listname = "list_{0}.list".format(i)
            sublists.append(listname)

            with open(listname, "w") as lf:
                for f in sub:
                    lf.write(f + "\n")

            launchcomm = (
                "nohup ./PadmeAnalysis -l {0} -r {1} -o {2}/{3}_{4}.root "
                "> {2}/logReco_{4}.txt &"
            ).format(listname, runNumber, OutDir, runName, i)

            print(launchcomm)
            Launcher.write(launchcomm + "\n")

Launcher.close()
print("\n\n########################## send source Launcher.txt to start the analysis #############################\n\n")       

# print("\n\n########################## creating Hadder_" + runName + ".sh #############################\n\n")

# with open("/data9Vd1/padme/dimeco/Hadder_" + runName + ".sh", "w") as Hadder:
#     if os.path.exists(runName + "_full.root"):
#         MyTag = raw_input("This output file already exists, please add a tag: ")
#         outname = runName + "_full_" + MyTag + ".root"
#     else:
#         outname = runName + "_full.root"

#     HaddCommand = "hadd " + outname + " "
#     for i in range(min(NoCoresToUse, nfiles)):
#         HaddCommand += OutDir + "/" + runName + "_" + str(i) + ".root "

#     Hadder.write(HaddCommand + "\n")

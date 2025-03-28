README of the october 2024 PbGl test beam 

how to compile:
  g++ -o "file.exe name" leadglass.cpp  -I ../reco/PadmeRoot/include -L ../reco/PadmeRoot/lib/ -lPadmeRoot  `root-config --libs  --cflags`

this is a dedicated version of compiling the code for the TB data analysis. There would be a better and optimized one, but this works, so it's fine (for the moment at least!)

how to execute:
  ./LGRead -l "input" -o "output"

one can execute in the same way we do for the full PADME framework, using list, single inputs, giving names, deciding the number of events and so on.

the logbook is arranged here on google sheets:
https://docs.google.com/spreadsheets/d/1jyPMirJvQu8A0sOSfjZdSylwc1y-uNd6f03u48Yzj1E/edit?gid=0#gid=0

and the already analyzed runs are here listed.
Energy scan:
  311 to 316 (some ad hoc considerations can be found in the runLGEnergyScan.sh script) 
Multiplicity scan:
  385 to 395 (some ad hoc considerations can be found in the runLGMultiScan.sh script) 
Position scan:
  318 to 370 (some ad hoc considerations can be found in the runLGPositionScan.sh script)
Voltage Scan:
  301 to 310 (some ad hoc considerations can be found in the runLGVoltageScan.sh script)

The other files (plot###.cpp and fit###.cpp) do some precise operations. 
fitPosScan.cpp - here I take the charge as calculated in leadglass.cpp, then I plot it on a histogram and make a fit for the position scan runs.
Then in plotPosScanMCCorr.cpp I plot the mean charge Q as a function of X and Y positions (the MC correction is also included here).
fitMultiScan.cpp - same idea but for the multiplicity scan runs. In plotQNpot.cpp, I plot the mean charge as a function of the NPOT.
fitEnergyScan.cpp - here I plot and fit the charge distribution for the energy scan runs (with a sum of 6 Gaussians).
Then in plotChargeEnergy.cpp I plot the Q over E graph and make a fit.
fitHVScan.cpp - is the same but for the HV scan runs and in plotChargeHV.cpp I plot the Q/E over HV and make a fi
README of the Run4 target reco and analysis

how to compile:
  g++ -o "file.exe" TargetRun4.cpp  -I ../PadmeRoot/include -L ../PadmeRoot/lib/ -lPadmeRoot `root-config --libs  --cflags`

this is a dedicated version of compiling the code for the TB data analysis or a simplified framework still compatible with PADME. There would be a better and optimized one, but this works, so it's fine (for the moment at least!)

how to execute:
  ./file.exe -l "input" -o "output"

one can execute in the same way we do for the full PADME framework, using list, single inputs, giving names, deciding the number of events and so on.




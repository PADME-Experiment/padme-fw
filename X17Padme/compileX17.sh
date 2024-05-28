g++ `root-config --cflags` -fPIC -Wall -O3 -g -c ChannelModel.cxx -o build/ChannelModel.o
g++ `root-config --cflags` -fPIC -Wall -O3 -g -c GlobalParams.cxx -o build/GlobalParams.o
g++ `root-config --cflags` -fPIC -Wall -O3 -g -c MuMuBkgShape.cxx -o build/MuMuBkgShape.o
g++ `root-config --cflags` -fPIC -Wall -O3 -g -c MuMuSignalShape.cxx -o build/MuMuSignalShape.o
g++ `root-config --cflags` -fPIC -Wall -O3 -g -c StatisticsModel.cxx -o build/StatisticsModel.o
g++ `root-config --cflags` -fPIC -Wall -O3 -g -c roofit_full.cxx -o build/roofit_full.o
g++ -o bin/X17Stat build/roofit_full.o build/ChannelModel.o build/GlobalParams.o build/MuMuBkgShape.o build/MuMuSignalShape.o build/StatisticsModel.o `root-config --glibs`


RECOLA=
## RECOLA=-DRECOLA

COLLIER=
## COLLIER=-DCOLLIER

EXE = babayaga
PAW = -lpawlib -lpacklib -lmathlib
CERNLIB = -L/cern/pro/lib
PAW =
CERNLIB =
PAWyn = npaw
PAWyn = ypaw
# or 'ypaw' if you use ntuple PAW events (in which case comment the two lines above)

F77 = gfortran
# tune for your processor
FFLAGS = -O3
# -flto
# aggressive flags
#FFLAGS = -Ofast -march=native -mtune=native
#-v
#FFLAGS = -g
##FFLAGS = -g -fbounds-check

## C ranlux optimizations, AVX2 faster but only on recent CPUS, SSE2 slower but present on
## any neowulf node. If both are specified, -DSSE2 is ignored (and won't run on old neowulf nodes).
RLXOPT=-DSSE2
# -DAVX2


CC = gcc

FPCHECK = trapfpe.c
FPCHECK =

EXTRADEPS = #Makefile

VPHLMNT=vp_hlmnt_v2_1
VPHLMNT=vp_hlmnt_v2_1_1
VPHLMNT=vp_hlmnt_v2_2

ifeq ($(RECOLA),-DRECOLA)
#        RCLDIR = recolas/recola2-collier-2.1.6
#        RCLDIR = recolas/recola-collier-1.3.7
        RCLVERSION = 1.4.0
        RCLDIR = recolas/recola-collier-$(RCLVERSION)
        RCLMOD = -I$(RCLDIR)/include/ -I$(RCLDIR)/recola-$(RCLVERSION)/include/
        RCLLIB = -L$(RCLDIR)/install/lib/ -lrecola -lcollier
# the following only for recola2
# -lmodelfile
endif

ifeq ($(COLLIER),-DCOLLIER)
  CLLDIR = collier/COLLIER-1.2.5/
  CLLMOD = -I$(CLLDIR)include/
  CLLLIB = -L$(CLLDIR)lib -lcollier
endif


OBJECTS = main.o cuts.o sv.o matrix_model.o mapmomenta.o\
          routines.o sampling.o phasespace.o distributions.o $(VPHLMNT).o\
          hadr5n16.o hadr5n09.o userinterface.o intpl.o Rteubner.o $(PAWyn).o\
          hadr5n17.o hadr5n12.o c_rnlx_interface.o ranlux_common.o ranlxd.o ranlxs.o ranlux.o recola_int.o
#\
#          Acp.o Aint.o Asu3.o interface.o mtx_eeenudbb.o 

F77 += $(FFLAGS)

default: $(EXE)

SAVEDIR = release
RELEASEDIR = BabaYaga
pack: # use only to release BABAYAGA
	mkdir -p $(RELEASEDIR)/form &&\
	mkdir -p $(RELEASEDIR)/c_ranlux &&\
        cp -r Makefile README input-test-pack input-gg-test shared.inc trapfpe.c\
        main.f cuts.f sv.F matrix_model.f mapmomenta.f vpol_novosibirsk.dat\
        routines.F sampling.f phasespace.f distributions.f $(VPHLMNT).f\
        hadr5n16.f hadr5n09.f ranlux.f userinterface.F intpl.f Rteubner.f\
        hadr5n17.f hadr5n12.f Acp.f Aint.f Asu3.f interface.f mtx_eeenudbb.f\
        recola_int.F npaw.f ypaw.f dalhadshigh17.f dalhadslow17.f dalhadt17.f dalhadthigh17.f\
        $(RELEASEDIR) &&\
	cp form/*.f $(RELEASEDIR)/form/ &&\
	cp c_ranlux/* $(RELEASEDIR)/c_ranlux/ &&\
	mkdir $(SAVEDIR) ;\
	tar cjvf $(SAVEDIR)/babayaga.tar.bz2 $(RELEASEDIR)/ &&\
	rm -rf $(RELEASEDIR)

clean:
	rm -f $(OBJECTS) *.a
deepclean:
	rm -f $(OBJECTS) *.a $(EXE) *~ form/*~

# C version of ranlux by Martin Luscher
# http://luscher.web.cern.ch/luscher/ranlux/index.html
c_rnlx_interface.o: c_ranlux/c_rnlx_interface.c $(EXTRADEPS) Makefile
	$(CC) $(FFLAGS) $(RLXOPT) -std=c99 -Ic_ranlux/ -c c_ranlux/c_rnlx_interface.c
ranlxd.o: c_ranlux/ranlxd.c $(EXTRADEPS) Makefile 
	$(CC) $(FFLAGS) $(RLXOPT) -std=c99 -Ic_ranlux/ -c c_ranlux/ranlxd.c
ranlxs.o: c_ranlux/ranlxs.c $(EXTRADEPS) Makefile
	$(CC) $(FFLAGS) $(RLXOPT) -std=c99 -Ic_ranlux/ -c c_ranlux/ranlxs.c
ranlux_common.o: c_ranlux/ranlux_common.c $(EXTRADEPS) Makefile
	$(CC) $(FFLAGS) $(RLXOPT) -std=c99 -Ic_ranlux/ -c c_ranlux/ranlux_common.c
####


# source files
main.o: main.f $(EXTRADEPS)
	$(F77) -c main.f
cuts.o: cuts.f $(EXTRADEPS)
	$(F77) -c cuts.f
matrix_model.o: matrix_model.f  form/formme.f form/ee3gexact.f form/borngg.f form/formmemm.f $(EXTRADEPS)
	$(F77) -c matrix_model.f
sv.o: sv.F $(EXTRADEPS)
	$(F77) -c $(COLLIER) $(CLLMOD) sv.F
2loop.o: 2loop.f $(EXTRADEPS)
	$(F77) -c 2loop.f
hplog.o: 2loop/hplog.f $(EXTRADEPS)
	$(F77) -c 2loop/hplog.f
userinterface.o: userinterface.F $(EXTRADEPS)
	$(F77) $(RECOLA) -c userinterface.F
phasespace.o: phasespace.f $(EXTRADEPS)
	$(F77) -c phasespace.f
hadr5n16.o: hadr5n16.f $(EXTRADEPS)
	$(F77) -c hadr5n16.f
hadr5n17.o: hadr5n17.f $(EXTRADEPS)
	$(F77) -c hadr5n17.f
hadr5n12.o: hadr5n12.f $(EXTRADEPS)
	$(F77) -c hadr5n12.f
# this is the old one
hadr5n09.o: hadr5n09.f $(EXTRADEPS)
	$(F77) -c hadr5n09.f
####
intpl.o: intpl.f $(EXTRADEPS)
	$(F77) -c intpl.f
$(VPHLMNT).o: $(VPHLMNT).f $(EXTRADEPS)
	$(F77) -c $(VPHLMNT).f
Rteubner.o: Rteubner.f $(EXTRADEPS)
	$(F77) -c Rteubner.f
mapmomenta.o: mapmomenta.f $(EXTRADEPS)
	$(F77) -c mapmomenta.f
sampling.o: sampling.f $(EXTRADEPS)
	$(F77) -c sampling.f
routines.o: routines.F $(EXTRADEPS)
	$(F77) $(COLLIER) $(CLLMOD) -c routines.F
distributions.o: distributions.f shared.inc $(EXTRADEPS)
	$(F77) -c distributions.f
ranlux.o: ranlux.f $(EXTRADEPS)
	$(F77) -c ranlux.f
$(PAWyn).o: $(PAWyn).f $(EXTRADEPS)
	$(F77) -c $(PAWyn).f
recola_int.o: recola_int.F $(EXTRADEPS) Makefile
	$(F77) $(RECOLA) -c $(RCLMOD) recola_int.F

### BEGIN ALPHA
# Acp.o: Acp.f $(EXTRADEPS)
# 	$(F77) -fno-automatic -c Acp.f
# Aint.o: Aint.f $(EXTRADEPS)
# 	$(F77) -fno-automatic -c Aint.f
# Asu3.o: Asu3.f $(EXTRADEPS)
# 	$(F77) -fno-automatic -c Asu3.f
# mtx_eeenudbb.o: mtx_eeenudbb.f $(EXTRADEPS)
# 	$(F77) -fno-automatic -c mtx_eeenudbb.f
# interface.o: interface.f $(EXTRADEPS)
# 	$(F77) -fno-automatic -c interface.f
### END ALPHA

# babayaga library
libbabayaga.a: $(OBJECTS)
	ar cr libbabayaga.a $(OBJECTS)

# executable
$(EXE): libbabayaga.a
	$(F77) main.o $(FPCHECK) -o $(EXE) -L. -lbabayaga $(CLLLIB) $(RCLLIB) $(RCLEXTRA)
# -L/home/carlo2/babayaga-fixed/LoopTools-2.14/lib64/ -looptools

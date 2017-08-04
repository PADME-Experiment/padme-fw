#!/bin/zsh

# cd /cvmfs/sft.cern.ch/lcg/releases/; ls -1d LCG_88/{gcc,ROOT,clhep,Geant4,XercesC}/*/*slc6*|grep -v "dbg$"

OPTPref=/opt

LCGPREF=/cvmfs/sft.cern.ch/lcg/releases
lxplusUserName=ggeorge

PACKS=(
#LCG_88/clhep/2.3.1.1/x86_64-slc6-gcc49-opt
LCG_88/clhep/2.3.1.1/x86_64-slc6-gcc62-opt
#LCG_88/gcc/4.9.1/x86_64-slc6
#LCG_88/gcc/4.9.3/x86_64-slc6
LCG_88/gcc/6.2.0/x86_64-slc6
#LCG_88/Geant4/10.02.p02/x86_64-slc6-gcc49-opt
LCG_88/Geant4/10.02.p02/x86_64-slc6-gcc62-opt
#LCG_88/ROOT/6.08.06/x86_64-slc6-gcc49-opt
LCG_88/ROOT/6.08.06/x86_64-slc6-gcc62-opt
#LCG_88/XercesC/3.1.3/x86_64-slc6-gcc49-opt
LCG_88/XercesC/3.1.3/x86_64-slc6-gcc62-opt
)


yumPackages=(
tree.x86_64
sqlite-devel.x86_64
mysql-server.x86_64
mysql-devel.x86_64
python-ipython.noarch
)

function LCG(){
FollowLCGLinks=( ${PACKS[@]/%//.} )
rsync -auvvRessh --del ${FollowLCGLinks[@]/#/$lxplusUserName@lxplus.cern.ch:$LCGPREF/./} $OPTPref
}

function yuminstall(){
yum install -y ${yumPackages[@]}
}


function envvar(){
typeset -Ux PATH # remove duplicated elements
typeset -Ux CPATH # remove duplicated elements
typeset -Ux LD_LIBRARY_PATH # remove duplicated elements
for i in $PACKS; {
dir=$OPTPref/$i
[ -d $dir/bin ]     && PATH=$dir/bin:$PATH
[ -d $dir/lib ]     && LD_LIBRARY_PATH=$dir/lib:$LD_LIBRARY_PATH
[ -d $dir/lib64 ]   && LD_LIBRARY_PATH=$dir/lib64:$LD_LIBRARY_PATH
[ -d $dir/include ] && CPATH=$dir/include:$CPATH
}
}


function shenv(){
envvar
cat << EOF
export PATH=$PATH
export CPATH=$CPATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH
EOF
}

function cshenv(){
envvar
cat << EOF
setenv PATH = $PATH
setenv CPATH = $CPATH
setenv LD_LIBRARY_PATH = $LD_LIBRARY_PATH
EOF
}

function help(){
cat << EOF

Synopsis:
$0 --LCG | --yuminstall | --shenv | --cshenv

 --LCG rsync LCG from lxplus

 --yuminstall installs some packages using yum 

 --shenv prints the new environments for sh

 --cshenv prints the new environments for csh

Example:
$0 --LCG
$0 --yuminstall
eval \$($0 --shenv)

EOF
exit
}

case $1 in
--LCG ) LCG ;;
--yuminstall ) yuminstall ;;
--cshenv  ) cshenv ;;
--shenv   ) shenv ;;
*) help;;
esac

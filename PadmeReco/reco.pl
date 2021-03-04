#!/usr/bin/perl
$opt      = $ARGV[0];
$RUN      = $ARGV[1];
$LISTNAME = $ARGV[2];
$GREP     = $ARGV[3];

if($opt eq "?") {
    print "reco.pl <Path to NRUN>\n";
#    print "         l:  do list check status\n";
#    print "Arg 1 :  Run \n";
#    print "Arg 2 :  Listname\n";
#    print "Arg 3 :  GREP\n";

    exit(1);
}

#system("voms-proxy-init --voms vo.padme.org");
#$RUN="run_0030106_20200709_233056";
$RUNN = $ARGV[0];
print "number of run insterted $RUNN\n";
#my $RUN=`gfal-ls root://atlasse.lnf.infn.it//dpm/lnf.infn.it/home/vo.padme.org/daq/2020/rawdata/ |grep $RUNN`;
my $RUN=`gfal-ls davs://atlasse.lnf.infn.it:443/dpm/lnf.infn.it/home/vo.padme.org/daq/2020/rawdata/ | grep $RUNN`;
#my $RUNSIZE=`gfal-ls root://atlasse.lnf.infn.it//dpm/lnf.infn.it/home/vo.padme.org/daq/2020/rawdata/ |grep $RUNN |wc -l`;
my $RUNSIZE=`gfal-ls davs://atlasse.lnf.infn.it:443/dpm/lnf.infn.it/home/vo.padme.org/daq/2020/rawdata/ |grep $RUNN |wc -l`;
chomp($RUN);
chomp($RUNSIZE);
#$length = @RUN;
print "Il run  $RUN size $RUNSIZE\n";
if($RUNSIZE>1) {
    print "selecting more than one run size 3\n"; 
    exit(1);
}
#$PATH= "root://atlasse.lnf.infn.it//dpm/lnf.infn.it/home/vo.padme.org/daq/2020/rawdata/$RUN/";
$PATH="davs://atlasse.lnf.infn.it:443/dpm/lnf.infn.it/home/vo.padme.org/daq/2020/rawdata/$RUN/";
$OUTDIR="/nfs/kloe/einstein3/padme/isabella/PresaDatiFrascati072020/$RUN";
system("mkdir $OUTDIR");
print "PATH $PATH GREP $GREP\n";

#my @files = `gfal-ls root://atlasse.lnf.infn.it//dpm/lnf.infn.it/home/vo.padme.org/daq/2020/rawdata/$RUN/`;
my @files = `gfal-ls davs://atlasse.lnf.infn.it:443/dpm/lnf.infn.it/home/vo.padme.org/daq/2020/rawdata/$RUN/`;
print "You have ";
print scalar @files;

print " files @files[1] \n";

$Lasti=scalar @files;
$NperFile=int($Lasti/16);
$NFile=0;

my $OUTFILE="Run$NFile.sh";	
print "Numero files $Lasti $OUT\n";
open(my $OUT,'>',$OUTFILE) or die "failed opening file";

my $SCRIPTFILE="RunAll.sh";
open(my $SCRIPT,'>',$SCRIPTFILE) or die "failed opening script file ";
#$NFile++;
for($ii=0;$ii<$Lasti;$ii++){
    $INFILE="@files[$ii]";
    chomp($INFILE);
    $COMMO="./PadmeReco -i $PATH$INFILE -o $OUTDIR/Reco$INFILE \n";
    print $OUT "$COMMO";   
}
close $OUTFILE;
$COMMO="split -l $NperFile -d $OUTFILE List$RUN\_ \n";
print $COMMO;
system("$COMMO");

print $SCRIPT "#!/bin/bash \n";
$NFiles=`ls -l Listrun_* |wc -l`;
system("chmod 7777 Listrun_*");
system("chmod 7777 RunAll.sh");
print $NFiles;
for($kk=0;$kk<$NFiles;$kk++){
    $string= sprintf("./List$RUN\_%02d",$kk);
#    print $string;
    print $SCRIPT "$string  &>/dev/null& \n";
 }

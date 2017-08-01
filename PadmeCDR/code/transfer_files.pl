#!/usr/bin/perl

use Getopt::Std;

printf "Running on node $ENV{'HOSTNAME'} as user $ENV{'USER'} in dir $ENV{'HOME'}\n";

$YEAR = 1900+(localtime)[5];
getopts('y:');
if ($opt_y) {
    $YEAR = $opt_y;
}
printf "Will transfer files for year $YEAR\n";

$DATA_DIR = "rawdata/$YEAR";

$DAQ_SRV = "padmesrv1";
$DAQ_USER = "daq";
$DAQ_PATH = "/home/daq/DAQ";

$DAQ_ADLER32_CMD = "/home/daq/DAQ/tools/adler32";

$DAQ_SFTP = "sftp://$DAQ_SRV$DAQ_PATH";
$LNF_SRM = "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org/testbeam";
$CNAF_SRM = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape/testbeam";

print "Getting list of raw data files for year $YEAR on $DAQ_SRV\n";
my @daq_list = `ssh -l $DAQ_USER $DAQ_SRV ls $DAQ_PATH/$DATA_DIR`;
chomp(@daq_list);
my %daq_list = map { $_ => 1 } @daq_list;

print "Getting list of raw data files for year $YEAR on LNF disks\n";
my @lnf_list = `gfal-ls $LNF_SRM/$DATA_DIR`;
chomp(@lnf_list);
my %lnf_list = map { $_ => 1 } @lnf_list;

print "Getting list of raw data files for year $YEAR on CNAF tape library\n";
my @cnaf_list = `gfal-ls $CNAF_SRM/$DATA_DIR`;
chomp(@cnaf_list);
my %cnaf_list = map { $_ => 1 } @cnaf_list;

# Make a full list and show which file is located where
@full_list = ();
push(@full_list,@daq_list);
push(@full_list,@lnf_list);
push(@full_list,@cnaf_list);
@full_list = sort @full_list;
$i = 0;
$uniq_list[0] = $full_list[0];
foreach my $file (@full_list) {
    if ($file ne $uniq_list[$i]) {
	push(@uniq_list,$file);
	$i++;
    }
}
foreach my $file (@uniq_list) {
    printf("%-30s ",$file);
    if ( exists($daq_list{$file})  ) { printf "D"; } else { printf "-"; }
    if ( exists($lnf_list{$file})  ) { printf "L"; } else { printf "-"; }
    if ( exists($cnaf_list{$file}) ) { printf "C"; } else { printf "-"; }
    printf "\n";
}

#foreach my $file ("run_628.root") {
foreach my $file (@uniq_list) {

    printf "\n=== Checking file $file ===\n";

    # If file exists on DAQ and not at LNF, copy it
    if ( exists($daq_list{$file}) and not exists($lnf_list{$file}) ) {

	printf "Starting copy of $file from DAQ to LNF\n";

	# Copy from PADME on-line server to LNF storage system
	$cmd = "gfal-copy -t 3600 -T 3600 -D\"SFTP PLUGIN:USER=$DAQ_USER\" -D\"SFTP PLUGIN:PRIVKEY=/home/$ENV{'USER'}/.ssh/id_rsa\" $DAQ_SFTP/$DATA_DIR/$file $LNF_SRM/$DATA_DIR/$file";
	printf "> $cmd\n";
	#system(split(/ /,$cmd));
	system($cmd);

	# Verify ADLER32 checksum

	$a32_daq_out = `ssh -l $DAQ_USER $DAQ_SRV $DAQ_ADLER32_CMD $DAQ_PATH/$DATA_DIR/$file`;
	chomp $a32_daq_out;
	$a32_daq = (split(/ /,$a32_daq_out))[0];

	$a32_lnf_out = `gfal-sum $LNF_SRM/$DATA_DIR/$file ADLER32`;
	chomp $a32_lnf_out;
	$a32_lnf = (split(/ /,$a32_lnf_out))[1];

	if ( $a32_daq ne $a32_lnf ) {
	    printf "ADLER32 CRC ***ERROR***: DAQ $a32_daq LNF $a32_lnf\n";
	} else {
	    printf "ADLER32 CRC OK: DAQ $a32_daq LNF $a32_lnf\n";
	    # Copy was successful: add file to list at LNF
	    $lnf_list{$file} = 1;
	    # Add DAQ->LNF post-transfer processing here
	}

    }

    # If file exist at LNF but not at CNAF, copy it
    if ( exists($lnf_list{$file}) and not exists($cnaf_list{$file}) ) {

	printf "Starting copy of $file from LNF to CNAF\n";

	# Copy from LNF storage system to CNAF tape library using gfal-copy
	$cmd = "gfal-copy -t 3600 -T 3600 $LNF_SRM/$DATA_DIR/$file $CNAF_SRM/$DATA_DIR/$file";
	printf "> $cmd\n";
	system(split(/ /,$cmd));

	# Verify ADLER32 checksum

	$a32_lnf_out = `gfal-sum $LNF_SRM/$DATA_DIR/$file ADLER32`;
	chomp $a32_lnf_out;
	$a32_lnf = (split(/ /,$a32_lnf_out))[1];

	$a32_cnaf_out = `gfal-sum $CNAF_SRM/$DATA_DIR/$file ADLER32`;
	chomp $a32_cnaf_out;
	$a32_cnaf = (split(/ /,$a32_cnaf_out))[1];

	if ( $a32_lnf ne $a32_cnaf ) {
	    printf "ADLER32 CRC ***ERROR***: LNF $a32_lnf CNAF $a32_cnaf\n";
	} else {
	    printf "ADLER32 CRC OK: LNF $a32_lnf CNAF $a32_cnaf\n";
	    # Copy was successful: add file to list at LNF
	    $cnaf_list{$file} = 1;
	    # Add LNF->CNAF post-transfer processing here
	}

    }

}

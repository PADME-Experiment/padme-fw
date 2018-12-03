#!/usr/bin/perl -w
use Compress::Zlib;        # for adler32()

my $crc;
my ($file) = @ARGV;
if (not defined $file) {
    print "Usage: adler32 <file>\n";
    exit(1);
}
open(my $fh,$file) or die "Could not open file $file";
while(<$fh>){
    $crc = adler32($_,$crc);
}
printf("%08x %s\n",$crc,$file);

exit(0);

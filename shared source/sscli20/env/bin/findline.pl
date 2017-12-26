# ==++==
# 
#   
#    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
#   
#    The use and distribution terms for this software are contained in the file
#    named license.txt, which can be found in the root of this distribution.
#    By using this software in any fashion, you are agreeing to be bound by the
#    terms of this license.
#   
#    You must not remove this notice, or any other, from this software.
#   
# 
# ==--==

my $ScriptName = "findline.pl";

# Capture the command line for reporting
my @CommandLine = @ARGV;

my $StringToFind = quotemeta($CommandLine[0]);
# print "StringToFind:$StringToFind\n";

my $InputFilename = $CommandLine[1]; 
my $retvalue = 1; # Return 1 when failed.

if (!open(INPUTFILE, $InputFilename)) {
    printf "$ScriptName: Cannot open $InputFilename\n";
}
else {
    while (<INPUTFILE>) {
        $thisline = $_; 
        # Match exactly a line case-insensitively. (Ok to have trailing white spaces)
        if ($thisline =~ /^$StringToFind\s*$/i) {
            print $thisline;
            $retvalue = 0;
        }
    }

    close INPUTFILE;
}
exit $retvalue;


sub Usage() {
    print "Usage: $ScriptName [line to find] [file to search]\n";
    print "       This script matches exactly a line case-insensitively (Ok to have trailing white spaces).\n";
}




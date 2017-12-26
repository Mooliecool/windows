#!/usr/bin/perl -w
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
#
# baseline.pl
#
# Custom driver for test:
#     Run the test and compare the output with baseline.

use strict;

sub Usage() {
    print "Usage: perl baseline.pl [testname] [commandline]\n";
}

# Global Variables
my(
   @CommandLine,        # Copy of the original command line
   $TestName,           # name of the test we want to run
   $retval,             # return value
   $results,            # captured output
   @baselinelist,   
   $baseline,
   );


@CommandLine=@ARGV;
$TestName = shift(@CommandLine);

$results = `@CommandLine`;
$retval = $? >> 8;

if ($retval != 0)
{
    printf "****Return Value=$retval\n";
    exit($retval);
}

open(BASELINE, $TestName . ".bsl");
@baselinelist = <BASELINE>;
close(BASELINE);

$baseline = join "", @baselinelist;

if ($results ne $baseline)
{
    printf "****Expected:*****\n".$baseline."\n****Results:****\n".$results."\n****End of Results****\n";
    exit(1);
} 

exit($retval);

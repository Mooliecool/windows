#!/usr/bin/perl

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

###############################################################################
# Purpose: simple perl script for running PAL verification tests
# Usage: On UNIX:
#           cd ROTOR_DIR
#           . env.sh (or equivalent)
#           cd tests
#           perl pvtrun.pl
#        On Windows:
#           cd ROTOR_DIR
#           env.bat
#           cd tests
#           pvtrun.pl
# Notes:
#    - palverify.dat is a subset of the test suite,
#      consisting of tests that should pass in all circumstances
#    - tests that FAIL under any circumstance (e.g. root user, reg user, etc.)
#      are not included.
#    - running this script should yield 0 failures
#

sub printUsage {    
    print "\n\n";
    print "====================================================================\n\n";
    print "*** PAL Verification Test Usage Information ***\n\n";
    print "On Windows:\n";
    print "    1. run: env.bat to set your dev environment\n";
    print "    2. run: pvtrun.pl [sub-test]\n";
    print "On UNIX:\n";
    print "    1. run: . env.sh  (or equivalent to set your dev environment)\n";
    print "    2. run: pvtrun.pl [sub-test]\n";
    print "\n";
    print "This test is intended to always pass and is maintained such that\n";
    print "it should yield zero failures whenever it is run.\n\n";
    print "====================================================================";
    print "\n\n";
    exit 1;
}


########################################################################################
# MAIN

use FindBin;

# Make the environment local
foreach (keys %ENV) {
    ${$_} = $ENV{$_};
}

unless (defined $BASEDIR) {
    print "pvtrun.pl: BASEDIR is not defined.\n";
    printUsage();
}

if ( defined $ARGV[0]) {
    my $subtest = $ARGV[0];
    $subtest =~ s/\\/\//g;
    $ENV{'TH_SUBTEST'} = lc $subtest;
}

if ( defined $ARGV[1]) {
    print "pvtrun.pl: only accept one argument.\n";
    printUsage();
}

my $mathLib = "";
if ( defined $WINDIR ) {

    $PLATFORM="WIN32";
    $SEP  = "\\";
    $pSEP = ";";
    $cmdSEP = " & ";
    $execEXT = ".exe";
    $exectest = "exectest_win32.c";
    # For building the harness and tests (PAL library location) respectively
    $ENV{'LIB'}             =   $SDK_LIB_PATH . ";"  # So 4 tests can build dll's
                              . $CRT_LIB_PATH . ";"  # So 4 tests can build dll's
                              . $_NTTREE  . "\\sdk\\pal\\lib\\" . $_BUILDTYPE . "\\" . $_BUILDARCH . $DDKBUILDENV; # Find rotor_pal.lib

    # Required for building the test harness
    $ENV{'INCLUDE'} =    $VCINSTALLDIR . "\\PlatformSDK\\Include;"
                      .  $VCINSTALLDIR . "\\Include";

    $COMPILEANDLINK =   "cl /nologo /DWIN32 /wd4996 /Zi /Od /W3 /WX /Fe";

} elsif ( defined $PLATFORM_UNIX ) {

    $PLATFORM = $^O;
    $SEP  = "\/";
    $pSEP = ":";
    $cmdSEP = "; ";
    $execEXT = "";
    $exectest = "exectest_unix.c";
    $mathLib = " -lm";
    if ($PLATFORM eq "freebsd") {
        $COMPILEANDLINK = "gcc -pthread -g -Wall $ENV{'C_DEFINES'} -o ";
    } else {
        if ($ENV{'_BIT64'} eq '1' && $PLATFORM eq "hpux") {
            $COMPILEANDLINK = "gcc -g -Wall $ENV{'C_DEFINES'} -mlp64 -o ";
        }
        else {
            $COMPILEANDLINK = "gcc -g -Wall $ENV{'C_DEFINES'} -o ";
        }
    }
} else {
    print "pvtrun.pl: unsupported platform or platform not detected.\n";
    printUsage();

}



$HARNBASE_DIR = $ROTOR_DIR . $SEP . "tests" . $SEP. "harness";
$HARNBIN_DIR  = $HARNBASE_DIR . $SEP . "test_harness";

# Vars for running the harness
$TH_DIR = $ROTOR_DIR . $SEP . "tests" 
                     . $SEP . "palsuite";
$ENV{'TH_DIR'}     = $TH_DIR;
$ENV{'TH_XRUN'}    = $ROTOR_DIR . $SEP . "tests" 
                                . $SEP . "harness" 
                                . $SEP . "xrun" 
                                . $SEP . "xrun" . $execEXT;
$TH_RESULTS = $FindBin::Bin . $SEP . "pvtresults.log";
$ENV{'TH_RESULTS'} = $TH_RESULTS;
$TH_SUMRES = $FindBin::Bin . $SEP . "pvtresults.sum";
$ENV{'TH_SUMRES'} = $TH_SUMRES;

$ENV{'TH_CONFIG'}  = $TH_DIR . $SEP . "palverify.dat";

#
# Read from buildinfo file, the current build number and type
#
$OS = uc($^O);
$EOLN = "\015\012";

if ($OS eq "MSWIN32"){
  $EOLN = "\n";
}
if ($OS eq "DARWIN"){
 $OS = "MAC OS"; # For QA integration issues
}

$ENV{'PAL_PLATFORM'} = $OS;

my($buildinfofile);
if (uc($^O) ne "MSWIN32") {
    $buildinfofile = $ENV{ _NTTREE }.'/buildinfo.txt';    
}else{
    $buildinfofile = $ENV{ _NTTREE }.'\\buildinfo.txt';
}

my(%info) = ();
my($key, $value);
$key   = "1";
$value = "1";
$buildnumber = "nobuild.number";
$buildtype   = "nobuildtype";

if(open(BUILDINFO,$buildinfofile )){
  while (<BUILDINFO>){
   chomp;
   ($key, $value) = (split /:/)[0,1];
   $key =~ s/\s//g;
   $value  =~ s/\s//g;
   $info{$key} = $value;
}
$buildnumber = $info{"Build-Number"};
$buildtype   = $info{"Build-Type"};
close(BUILDINFO);
}

$ENV{'PAL_BUILDTYPE'} = $buildtype;
$ENV{'PAL_BUILDNUMBER'} = $buildnumber;

# Build the harness and xrun component

printf "===================================================================\n";
printf "Building xrun and test_harness:\n\n";
chdir $HARNBASE_DIR . $SEP . "xrun";
my $compileLine = $COMPILEANDLINK . "xrun xrun.c";
$retCode = system $compileLine;
if ($retCode) {
    printf "Error!  xrun failed to build.\n";
    exit 1;
}
chdir $HARNBIN_DIR;
$retCode = system $COMPILEANDLINK . "testharness configparser.c error.c " 
                  . $exectest . " main.c params.c results.c testinfo.c util.c" . $mathLib;
if ($retCode) {
    printf "Error!  test harness failed to build.\n";
    exit 1;
}


# Run the harness
printf "===================================================================\n";
printf "Running the suites:\n\n";

if ($PLATFORM eq "WIN32") {

    $retCode = system $HARNBIN_DIR . $SEP . "testharness" . $execEXT;

    # Not sure how to capture harness ret code on Windows,
    # so just report errors if system returns non-zero

    printf "===================================================================\n\n";
    printf "\n\nPAL Verification Test Summary:\n";
    if ($retCode) {
        printf "Regression detected. Check for PAL test case\n" 
              ."failures in %s\n\n", $TH_RESULTS;
    } else {
        printf "\n\nNo PAL test case failures detected.\n\n";
    }
    printf "===================================================================\n\n";

} else {

    $summaryMsg = "echo ===================================================================;"
                 ."echo\; echo PAL Verification Test Summary\:\;"
                 ."echo \$FAILS test case failures were detected.\;"
                 ."echo Check for PAL test case failures in ".$TH_RESULTS
                 ."; echo; "
                 ."echo ===================================================================;"
                 ."echo";
    system $HARNBIN_DIR . $SEP . "testharness; "
          ."FAILS=\$\?; " . $summaryMsg;
}
 
# Go back where you came from!       

chdir $ROTOR_DIR;






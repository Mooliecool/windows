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
# Rotor Test Driver
#

sub Usage() {
    print "Usage: perl rrun.pl [options] [tests]\n";
    print "\n\t[options]\n";
    print "\n-b\tRuns only tests in the BVT group\n";
    print "\t-c\tRun with Micrsoft V1 .NET Framework, not Rotor (only useful on Windows)\n";
    print "\t-d\tDisplay only - don't execute the tests\n";
    print "\t-g\tRun with GCStress on\n";
    print "\t-h\tThis message\n";
    print "\t-i\tIgnore the NOTON* attribute for this platform (not used on Windows)\n";
    print "\t-l\tLong Running mode - execute all tests, including those marked as LONGRUNNING\n";
    print "\t-n\tDon't print the banner\n";
    print "\t-o\tForce the verifier off and don't run tests marked VERIFIERMUSTBEON\n";
    print "\t-p\tRun tests with debug heap\n";
    print "\t-t\tRun Test Lab suite combinations\n";
    print "\t-v\tVerbose output\n";

    print "\n\t[tests]\tNames of tests to execute\n";
}

# Feature Notes:
#   1) rrun.log and rrun.err generated in your starting directory
#   2) To run a specific test, you may specify either the short name or 
#      the fully qualified name.  For example, to run the test named
#        C:\rotor\tests\bvt\switch
#      You may use either 
#        perl rrun.pl switch
#        perl rrun.pl C:\rotor\tests\bvt\switch
#      When using the short version, all tests matching that name found
#      will be executed.
#
# Test File Syntax (rsources)
#
#   All non blank, non comment lines are of the form:
#      TESTNAME = FILE1 [FILE2...][, <ATTRIB1>...]
#   Where
#      TESTNAME = arbitrary name to identify the tests - by convention
#      the base name of the first file.
#      FILE1 = name of the source file for the main test - compiled to
#              an EXE (if not an EXE already)
#      FILEN = name of support files, compiled to DLL
#        Recognized file extensions - .CS, .IL, .ILF and .EXE
#      <ATTRIB> - attributes of the test. Currently recognized
#        attributes:
#        <VERIFIERMUSTBEON> - The verifier must be on for this test to
#           run
#        <VERIFIERMUSTBEOFF> - The verifier must be off for this test
#           to run
#        <CORDBGDRIVER> - Run the test with the cordbgdriver
#        <LOADNEG> - Run the test with the $LoadNegUtility
#        <LOADPOS> - Run the test with the $LoadPosUtility
#        <COMPILEONLY> - Only compile the source - don't execute
#        <DOFIRST> - Make sure this test is run "first" (all first tests
#           are grouped at the front - primarily used to build the 
#           utilities subdir)
#        <LOADNEGDEL> - Run the test with the $LoadNegDelUtility
#        <PERLDRIVER> - Run the test with the perl driver. The driver
#           must have the same base name as the first source file: <FILE1>.pl
#        <BASELINEDRIVER> - Compare the output of the test with baseline.
#        <NOTONFREEBSD> - Causes the test to be skipped if running on FreeBSD
#        <NOTONMACOSX> - Causes the test to be skipped if running on Mac OS X
#        <NOTONSOLARIS> - Causes the test to be skipped if running on Solaris
#        <UNSAFEOPTION> - Causes csharp source to be compile with /unsafe
#        <LONGRUNNING> - Tests only run when specificying the long running mode
#        <HASRESOURCE> - Test has a resource file of the same base name
#        <DLLOUTPUT> - force the first target to be a library
#        <USESGENSTRINGS> - test uses the genstrings library
#        <USESFXTEST> - test uses the fxtest library
#        <ONEOUTPUT> - combines all files into on output
#        <BVT> - Marks test as belonging to the BVT group
#        <NOGCSTRESS> - Don't run these tests under GCStress
#
# Directory File Syntax (rdirs)
#
#   All non blank, non comment lines are of the form:
#      DIRNAME [, <ATTRIB1>...]
#   Where
#      DIRNAME = name of the subdirectory to search for other rdirs or rsources files
#      <ATTRIB> - attributes of the sub-directory. Currently recognized
#        <TESTPASSONLY> - sub-directory tree is only searched if the -t switch is specificed

#
# General Strategy of test driver 
#
# 1) Traverse directory tree from current directory down, looking for 
#    RDIRS files and building a list of fully qualified directories
# 2) Open the RSOURCES file in each directory, building a 
#    list of tests to run
# 3) Traverse the list of tests and execute each test.  If specific
#    tests are specified, only execute those that match the name 

use Cwd;
use File::Basename;
use File::Compare;
use File::Copy;
use Getopt::Std;
use strict;

# Global Variables
my(
   $DriverName,               # Name of the driver - used for log entries
   @DirectoriesToProcess,     # Working list of directories
   @Directories,              # Full list of directories      
   $Directory,                # Current directory being visited
   @Tests,                    # Array of all tests
   $LogFile,                  # Name of the log file
   $ErrFile,                  # Name of the error file
   $PassCount,                # Total number of suites that passed
   $FailCount,                # Total number of suites that failed
   %options,                  # Hash of command line options
   $StartTime,                # Starting time
   @IndividualTests,          # Individual tests to run
   $CSCFLAGS,                 # Flags to the CSC compiler
   $JSCFLAGS,                 # Flags to the JSC compiler
   $CLIXName,                 # Name of the clix driver
   $CASScriptName,            # Name of the CAS script
   $VerifierLastState,        # Last set state for Verifier
   $CordbgUtility,            # Name of the cordbg utility for cordbg tests
   $BaselineUtility,          # Name of the baseline utility
   $LoadNegUtility,           # Name of the verifier utility for negative tests
   $LoadPosUtility,           # Name of the verifier utility for positive tests
   $LoadNegDelUtility,        # Name of the verifier utility for negative delegate tests
   $JSCCOMPILER,              # Path to JSC compiler
   $NoLogoOption,             # Command line for compilers - unset under -v
   $GenStringsLocation,       # Path to genstrings.dll and debug file
   $FxTestLocation,           # Path to fxtest.dll and debug file
   $LogFileBase,              # Base name for the log file - build dependent
   $OutputDir,                # Ouput directory name - build dependent
   $OutputDirWithSeperator,   # Same as above, but with the path seperator appended
   @CommandLine,              # Copy of the original command line
   @VerifyState               # Possible states of the verifier for this run
   );

# A mapping from $^O names to NOTON* names.
my %SupportedPlatforms = (
        'FreeBSD' => 'FREEBSD',
        'Darwin' => 'MACOSX',
        'Solaris' => 'SOLARIS',
        'Win32' => 'WIN32'
        );

# IsSupportedPlatform - Returns whether this platform is supported.
sub IsSupportedPlatform($)
{
    my $platform = shift;
    for (values %SupportedPlatforms) {
        return 1 if $platform eq $_;
    }
    return 0;
}

#
# LogMessage - Logs a message to the console and logfile
#
sub LogMessage
{
    my $Message = shift(@_);
    
    print $DriverName . $Message . "\n"; 
    print LOGFILE $DriverName . $Message . "\n"; 
}

#
# LogWarning - Logs a warning to the console and logfile
#
sub LogWarning
{
    my $WarningMessage = shift(@_);
    
    print $DriverName . "WARNING:" . $WarningMessage . "\n";     
    print LOGFILE $DriverName . "WARNING:" . $WarningMessage . "\n"; 
}

#
# LogError - Logs a error to the console and logfile
#
sub LogError
{
    my $ErrorMessage = shift(@_);
    
    print $DriverName . "ERROR: " . $ErrorMessage . "\n";
    print ERRFILE $DriverName . "ERROR: " . $ErrorMessage . "\n";
    print LOGFILE $DriverName . "ERROR: " . $ErrorMessage . "\n"; 
    $FailCount++;
}

#
# LogResults
#
# Log the test results
#
sub LogResults
{
    my($test, $CompTime, $TestTime, $TotalTime, $RetVal) = @_;
    
    my $PrintLine = "NAME=[$test->{NAME}],TARGET=[$test->{TARGET}],Dir=[$test->{DIRECTORY}],CompTime=[$CompTime sec],TestTime=[$TestTime sec],TotalTime=[$TotalTime sec],RetVal=[$RetVal]\n";
    if($RetVal) {
        print $DriverName . "FAILED:" . $PrintLine;
        print LOGFILE $DriverName . "FAILED:" . $PrintLine;
        print ERRFILE $DriverName . "FAILED:" . $PrintLine;
        $FailCount++;
    } else {
        print $DriverName . "PASSED:" . $PrintLine;
        print LOGFILE $DriverName . "PASSED:" . $PrintLine;
        $PassCount++;
    }
}


#
# cmd_redirect -- execute a cmd, redirecting stdout, stderr.
#
# Redirects STDERR to STDOUT, and then redirects STDOUT to the
# argument named in $redirect.  It is done this way since
# invoking system() with i/o redirection under Win9x masks
# the return code, always yielding a 0.
#
# The return value is the actual return value from the test.
#
sub cmd_redirect
{
    my ($cmd) = @_;
    my $retval;

    LogMessage("cmd == [" . $cmd . "]");

    open SAVEOUT, ">&STDOUT";
    open SAVEERR, ">&STDERR";

    # This gets perl to shut up about only using SAVEOUT and SAVEERR
    # once each.
    print SAVEOUT "";
    print SAVEERR "";
    
    open STDOUT, ">>$LogFile" or die "Can't redirect stdout to ($LogFile)";
    open STDERR, ">&STDOUT" or die "Can't dup stdout";

    select STDERR; $| = 1;
    select STDOUT; $| = 1;

    system($cmd);
    $retval = $?;
    if (($retval >> 8) != 0) {
        # The program exited with a non-zero result.
        $retval >>= 8;
    } elsif (($retval & 127) != 0) {
        # The program exited due to a signal.
        $retval &= 127;
    } elsif (($retval & 128) != 0) {
        # The program dumped core.
        $retval &= 128;
    } else {
        $retval = 0;
    }

    close STDOUT;
    close STDERR;

    open STDOUT, ">&SAVEOUT";
    open STDERR, ">&SAVEERR";

    return $retval;
}

# 
# Verifier - turns the verifier on and off
sub verifier
{
    my ($state) = @_;
    my $cmdline;

    if(not $options{c}) {
        if(uc($state) eq uc($VerifierLastState)) {
            return 0;
        } else {
            # Cache the last known state - time optimization
            $VerifierLastState = $state;
            $cmdline = $CASScriptName . " " . $state;
            if($options{d}) {
                LogMessage($cmdline);
                return 0;
            } else {
                $? = cmd_redirect( $cmdline );
                if($?) {
                    LogError("[" . $cmdline . "] returned [" . $? . "]");
                    die "Processing cannot continue";
                }
                return $?;
            }
        }
    }
}

sub UnrollArray
{
    my (@LocalArray) = @_;
    my $UnRolledArray = pop(@LocalArray);
    while(@LocalArray) {
        $UnRolledArray = $UnRolledArray . " " . pop(@LocalArray);
    }
    return $UnRolledArray;
}

sub CopyFile
{
    my $sourcefile = shift(@_);
    my $targetfile = shift(@_);

    if(not -e $sourcefile) {
        LogError("Source file [" . $sourcefile . "] does not exist");
        return 1;
    }

    if(not -e $targetfile or -M $sourcefile > -M $targetfile) {
        LogMessage("Copying [$sourcefile] to [$targetfile]");
        if(not copy($sourcefile, $targetfile)) {
            #if we fail, sleep for 3 and try again
            sleep 3;
            if(not copy($sourcefile, $targetfile) ) {
                LogError("Could not copy [$sourcefile] to [$targetfile] - error [" . $! . "]");
                return 1;
            }
        }
    }
    return 0;
}

################
# Main Program #
################

$StartTime = time();

$DriverName = "RRUN: ";

# Capture the command line for reporting
@CommandLine = @ARGV;

# Parse the command line
my $retval = getopts("bcdghilnopstv", \%options);
if(not $retval) {
    die $DriverName . "Unknown option\n";
}

if(not $options{n}) {
    print "Rotor Test Driver - Version 1.0\n";
    print "Microsoft Corp. - All Rights Reserved\n\n";
}

if($options{h}) {
    Usage();
    exit 0;
} 

# Grab the tests to run - that's what's left
@IndividualTests = @ARGV;

# Initialization

# Take a quick look around to see if we are running in an known environement
if(not $options{c}) {
    if(not $ENV{ROTOR_DIR}) {
        die $DriverName . "Environement variable ROTOR_DIR not set - cannot continue\n";
    }
}

# Setup log files and output directories
if($ENV{BUILD_ALT_DIR}) {
    $LogFileBase = "rrun" . $ENV{BUILD_ALT_DIR};
    $OutputDir = "obj" . $ENV{BUILD_ALT_DIR};
} else {
    $LogFileBase = "rrun";
    $OutputDir = "obj";
}

# Open the log and error files

#
# Note: we unlink the files and open append because we want the 
# "seek to end of file and write" behaviour on each write.  This
# ensures that the output we capture from each test run folds into the
# proper place in the output stream
#
$LogFile = Cwd::cwd() . "/" . $LogFileBase . ".log";
unlink $LogFile;
open(LOGFILE, ">>" . $LogFile) 
    or die "Can't open " . $LogFile . "\n" ;

$ErrFile = Cwd::cwd() . "/". $LogFileBase . ".err";
unlink $ErrFile;
open(ERRFILE, ">>" . $ErrFile)
    or die "Can't open " . $ErrFile . "\n";

# Disable buffering
my $old_fh;
$old_fh = select(LOGFILE);
$| = 1;
select($old_fh);
$old_fh = select(ERRFILE);
$| = 1;
select($old_fh);


# Set up globals
$PassCount = 0;
$FailCount = 0;

$JSCFLAGS = "";

# OS Specific Initialization
if (uc($^O) ne "MSWIN32") {
    $OutputDirWithSeperator = $OutputDir . "/";
    if($options{v}) {
        $NoLogoOption = "";
    } else {
        $NoLogoOption = " -nologo";
    }

    if(not $options{c}) {
        $CLIXName="clix "; 
    }
    $CASScriptName=$ENV{ROTOR_DIR} . "/tests/cas";
    $CSCFLAGS = "/debug $ENV{CSC_DEFINES}" . $NoLogoOption;

    $CordbgUtility = $ENV{ROTOR_DIR} . "/tests/cordbg/utilities/cordbg.pl";
    $BaselineUtility = $ENV{ROTOR_DIR} . "/tests/utilities/baseline.pl";
    $LoadNegUtility = $ENV{ROTOR_DIR} . "/tests/utilities/" . $OutputDirWithSeperator . "loaderneg.exe";
    $LoadPosUtility = $ENV{ROTOR_DIR} . "/tests/utilities/" . $OutputDirWithSeperator . "loaderpos.exe";
    $LoadNegDelUtility = $ENV{ROTOR_DIR} . "/tests/utilities/" . $OutputDirWithSeperator . "loadernegdel.exe";   
    $JSCCOMPILER = $CLIXName . $ENV{_NTTREE} . "/jsc.exe" .  $NoLogoOption;
    $ENV{'JSCCOMP'} = $ENV{_NTTREE} . "/jsc.exe";
    $JSCFLAGS = "-debug -codepage:1252";
    $GenStringsLocation = $ENV{ROTOR_DIR} . "/tests/utilities/" . $OutputDirWithSeperator . "genstrings";
    $FxTestLocation = $ENV{ROTOR_DIR} . "/tests/utilities/" . $OutputDirWithSeperator . "fxtest";
} else {
    $OutputDirWithSeperator = $OutputDir . "\\";
    # Defaults to Unix style - override
    fileparse_set_fstype("MSWin32");
    if($options{v}) {
        $NoLogoOption = "";
    } else {
        $NoLogoOption = " /nologo";
    }
    if(not $options{c}) {
        $CLIXName="clix.exe ";
    }
    $CASScriptName=$ENV{ROTOR_DIR} . "\\tests\\cas.bat";
    $CSCFLAGS = "/debug $ENV{CSC_DEFINES}"  . $NoLogoOption;
    $CordbgUtility = $ENV{ROTOR_DIR} . "\\tests\\cordbg\\utilities\\cordbg.pl";
    $BaselineUtility = $ENV{ROTOR_DIR} . "\\tests\\utilities\\baseline.pl";
    $LoadNegUtility = $ENV{ROTOR_DIR} . "\\tests\\utilities\\" . $OutputDirWithSeperator . "loaderneg.exe";
    $LoadPosUtility = $ENV{ROTOR_DIR} . "\\tests\\utilities\\" . $OutputDirWithSeperator . "loaderpos.exe";
    $LoadNegDelUtility = $ENV{ROTOR_DIR} . "\\tests\\utilities\\" . $OutputDirWithSeperator . "loadernegdel.exe";
    if($ENV{_NTTREE}) {
        $JSCCOMPILER = $CLIXName . $ENV{_NTTREE} . "\\jsc.exe" . $NoLogoOption;
        $ENV{'JSCCOMP'} = $ENV{_NTTREE} . "\\jsc.exe";
    } else {
        $JSCCOMPILER = $CLIXName . "jsc.exe" . $NoLogoOption;
        $ENV{'JSCCOMP'} = "jsc.exe";
    }
    $JSCFLAGS = "-debug -codepage:1252";
    $GenStringsLocation = $ENV{ROTOR_DIR} . "\\tests\\utilities\\" . $OutputDirWithSeperator . "genstrings";
    $FxTestLocation = $ENV{ROTOR_DIR} . "\\tests\\utilities\\" . $OutputDirWithSeperator . "fxtest";    
}

if(not $options{c}) {
    $JSCFLAGS = $JSCFLAGS . " -d:rotor";
}

# Turn of the assertion - rely on throwing the exception to fail the tests
# Required for the tests in verifier\delegate

$ENV{COMPlus_AssertOnBadImageFormat} = "0";

# Set GCStress, if appropriate
if($options{g}) {
    $ENV{COMPlus_GCStress} = "4";
}

# Debug heap option
if($options{p}) { 
    if((uc($^O) eq "DARWIN")) {
        $ENV{MallocScribble} = "1";
        $ENV{MallocGuardEdges} = "1";
        $ENV{MallocCheckHeapStart} = "1";
        $ENV{MallocCheckHeapEach} = "1";
    } elsif (uc($^O) eq "FREEBSD") {
        $ENV{MALLOC_OPTIONS} = "AJR";
    } else {
        LogWarning("Debug heap option not supported on platform [" . uc($^O) . "]");
    }
}

#
# Start Processing
#

LogMessage("Run Started " . scalar(localtime));

# Display details on the run
push(@CommandLine, $0);
LogMessage("Command line [" . UnrollArray(@CommandLine)  . "]");
LogMessage("Current Working Directory [" . Cwd::cwd() . "]");

# Always build the utilities directory
my $UtilitiesDirectory;
if (uc($^O) ne "MSWIN32") {
    $UtilitiesDirectory = $ENV{ROTOR_DIR} . "/tests/utilities";
} else {
    $UtilitiesDirectory = $ENV{ROTOR_DIR} . "\\tests\\utilities";
}
push(@DirectoriesToProcess, $UtilitiesDirectory);

# Find all the directories to process, by walking the rdirs files
push(@DirectoriesToProcess, Cwd::cwd());

while(@DirectoriesToProcess) 
{
    my $Directory = pop(@DirectoriesToProcess);
    chomp($Directory);
    if(-e $Directory . "/rsources") {
        push(@Directories, $Directory);
    }
    my $DirFileName = $Directory . "/rdirs";

    open(RDIRS_FILE, $DirFileName) or next;
    DIRPROCESSING : while(<RDIRS_FILE>) {
        my $dirattribute;
    
        chomp;
        s/#.*//;    # No comments
        s/^\s+//;   # No leading whitespace
        s/\s+$//;   # No trailing whitespace
        next unless length;
#split(/\s*=\s*|\s*,\s*/, $_);
        my($subdirname, @dirattributes) = split(/\s*,\s*/, $_);
        foreach $dirattribute (@dirattributes) {
            $dirattribute =~ s/^\s+//;   # No leading whitespace
            $dirattribute =~ s/\s+$//;   # No trailing whitespace
            next unless length;
            if(uc($dirattribute) eq "<TESTPASSONLY>") {
                if( not $options{t} ) {
                    next DIRPROCESSING;
                }
            } else {
                die "Unknown directory attribute [" . $dirattribute . "] in " . $DirFileName . "\n";
            }
        }
        # Check that the directory exists
        my $FullPathToDirectory = $Directory . "/" . $subdirname;
        if(-d $FullPathToDirectory) {
            push @DirectoriesToProcess, $FullPathToDirectory;
        } else {
            LogError("Could not open directory [" . $FullPathToDirectory . "] for processing");
        }
    }
    close(RDIRS_FILE);
}

# Find the name of the NOTON* attribute for this platform.
my $notName = undef;
for (keys %SupportedPlatforms) {
    if (/^$^O$/i) {
        $notName = "NOTON$SupportedPlatforms{$_}";
        last;
    }
}

# Walk the directories, looking for rsource files
# Process the rsource file, building up the list of tests
foreach $Directory (@Directories) 
{
    my $SourceFileName = $Directory . "/rsources";

    if($options{v}) {
        LogMessage( "Looking for " . $SourceFileName);
    }

    open(RDIRS_FILE, $SourceFileName) or 
        (LogMessage("No RSOURCES file found in " . $Directory) and next);
    while(<RDIRS_FILE>) {
        chomp;
        s/#.*//;    # No comments
        s/^\s+//;   # No leading whitespace
        s/\s+$//;   # No trailing whitespace
        next unless length;
        # Pick apart lines - form is NAME = TEST
        my($testname, $testtarget, @testattributes) = split(/\s*=\s*|\s*,\s*/, $_);
        # Process the attributes
        my $attribute;
        my $verifierattribute = "doesntcare";
        my $driver = "none";
        my $compileonly = 0;
        my $dofirst = 0;
        my $perldriver = 0;
        my $baselinedriver = 0;
        my $cordbgdriver = 0;
        my $fNotOnPlatform = 0;
        my $fLongRunningTest = 0;
        my $CSharpUnSafeOption = "";
        my $fHasResource = 0;
        my $fDLLOutput = 0;
        my $fUsesGenstrings = 0;
        my $fUsesFxTest = 0;    
        my $fOneOutput = 0;
        my $fInBVT = 0;
        my $fNoGCStress = 0;
        foreach $attribute (@testattributes) {
            $attribute =~ s/^\s+//;   # No leading whitespace
            $attribute =~ s/\s+$//;   # No trailing whitespace
            next unless length;
            if(uc($attribute) eq "<VERIFIERMUSTBEON>") {
                $verifierattribute = "verifiermustbeon";
            } elsif (uc($attribute) eq "<VERIFIERMUSTBEOFF>") {
                $verifierattribute = "verifiermustbeoff";
            } elsif (uc($attribute) eq "<LOADNEG>") {
                $driver = $LoadNegUtility;
            } elsif (uc($attribute) eq "<LOADPOS>") {
               $driver = $LoadPosUtility;
            } elsif (uc($attribute) eq "<LOADNEGDEL>") {
                $driver = $LoadNegDelUtility;
            } elsif (uc($attribute) eq "<COMPILEONLY>") {       
                $compileonly = 1;
            } elsif (uc($attribute) eq "<DOFIRST>") {       
                $dofirst = 1;
            } elsif (uc($attribute) eq "<PERLDRIVER>") {        
                $perldriver = 1;        
            } elsif (uc($attribute) eq "<BASELINEDRIVER>") {        
                $baselinedriver = 1;                       
            } elsif (uc($attribute) eq "<CORDBGDRIVER>") {
                $cordbgdriver = 1;        
            } elsif (defined($notName) and uc($attribute) eq "<$notName>") {      
                $fNotOnPlatform = 1;
            } elsif (uc($attribute) eq "<UNSAFEOPTION>") {      
                $CSharpUnSafeOption = " /unsafe";
            } elsif (uc($attribute) eq "<LONGRUNNING>") {       
                $fLongRunningTest = 1;
            } elsif (uc($attribute) eq "<HASRESOURCE>") {       
                $fHasResource = 1;
            } elsif (uc($attribute) eq "<DLLOUTPUT>") {     
                $fDLLOutput = 1;
            } elsif (uc($attribute) eq "<USESGENSTRINGS>") {        
                $fUsesGenstrings = 1;
            } elsif (uc($attribute) eq "<USESFXTEST>") {        
                $fUsesFxTest = 1;                
            } elsif (uc($attribute) eq "<ONEOUTPUT>") {     
                $fOneOutput = 1;
            } elsif (uc($attribute) eq "<BVT>") {     
                $fInBVT = 1;
            } elsif (uc($attribute) eq "<NOGCSTRESS>") {     
                $fNoGCStress = 1;
            } elsif ($attribute =~ /^<NOTON(\w+)>$/ and IsSupportedPlatform($1)) {
                next;
            } else {
                die "Unknown attribute [" . $attribute . "] in " . $SourceFileName . "\n";
            }
        }

        my $test = {
            DIRECTORY => $Directory,
            TARGET => $testtarget,
            NAME => $testname,
            VERIFERATTRIBUTE => $verifierattribute,
            DRIVER => $driver,
            COMPILEONLY => $compileonly,
            DOFIRST => $dofirst,
            PERLDRIVER => $perldriver,
            BASELINEDRIVER => $baselinedriver,
            CORDBGDRIVER => $cordbgdriver,
            NOTONPLATFORM => $fNotOnPlatform,
            CSHARPUNSAFEOPTION => $CSharpUnSafeOption,
            LONGRUNNINGTEST => $fLongRunningTest,
            HASRESOURCE => $fHasResource,
            DLLOUTPUT => $fDLLOutput,
            USESGENSTRINGS => $fUsesGenstrings,
            USESFXTEST => $fUsesFxTest,
            ONEOUTPUT => $fOneOutput,
            INBVT => $fInBVT,
            NOGCSTRESS => $fNoGCStress
        };

        push @Tests, $test;
    }
    close(RDIRS_FILE);
}

#
# Run the tests
# 

# Sorts tests based on various criteria - order of sorts matters

# Sort the tests based on the verifier attribute - performance optimization
@Tests = sort {$a->{VERIFERATTRIBUTE} cmp $b->{VERIFERATTRIBUTE}} @Tests;

# Sort tests so those being done first are.
@Tests = sort {$b->{DOFIRST} <=> $a->{DOFIRST}} @Tests;

# Set up the verification states
if($options{t}) {
    push @VerifyState, "veron";
    push @VerifyState, "veroff";
} elsif ($options{o}) {
    push @VerifyState, "veroff";
} else {
    push @VerifyState, "veron";
}

foreach my $verifierstate (@VerifyState) 
{
    LogMessage("Verifier State = [" . $verifierstate . "]");

    TESTPROCESSING : foreach my $test (@Tests)
    {
        # If individual tests are specified, then see if this is one of them
        if(@IndividualTests) {
            my $GotMatch = 0;
            my $SpecificTest;
            foreach $SpecificTest (@IndividualTests) {
                if((uc($test->{NAME}) eq uc($SpecificTest))  or 
                (uc($test->{DIRECTORY} . "/" . $test->{NAME}) eq uc($SpecificTest))) {
                    $GotMatch = 1;
                }
            }
            next unless $GotMatch;
        }

        if($options{v}) {
            LogMessage("Possible Test [$test->{DIRECTORY}/$test->{NAME}]");
        }

        # Attribute filtering
        if(($verifierstate eq "veroff") and ($test->{VERIFERATTRIBUTE} eq "verifiermustbeon")) {
            next;
        }
        if(($verifierstate eq "veron") and ($test->{VERIFERATTRIBUTE} eq "verifiermustbeoff")) {
            next;
        }
        if((not $options{i}) and $test->{NOTONPLATFORM}) {
            next;
        }
        if($test->{LONGRUNNINGTEST} and (not $options{l})) {
            next;
        }
        if($options{b} and not ($test->{INBVT} or $test->{DOFIRST})) {
            next;
        }
        if($options{g} and $test->{NOGCSTRESS}) {
            next;
        }

        # Set up the current working directory
        chdir($test->{DIRECTORY}) or die "Couldn't change dir to $test->{DIRECTORY}\n";

        # Set up output directory
        if(not -e $OutputDir) {
            mkdir($OutputDir, 0777) or die "Couldn't create the output directory $test->{DIRECTORY}\$OutputDir\n";
        }

        LogMessage("Starting [$test->{DIRECTORY}/$test->{NAME}] " . scalar(localtime));
        my($TestStartTime) = time();

        #verifier
        verifier($verifierstate);

        # Process the test files
        my @files = split(" ", $test->{TARGET});

        for (my $index = 0; $index < @files; $index++) {
            my $file = $files[$index];

            my($base, $dir, $ext) = fileparse($file,'\..*' );
            {
                # Used to bypass compile for checked in EXE tests
                my $fDoCompile = 1;

                # If not already specified, the test command line is the derived from the first 
                # file in the list.
                if(not $test->{CMDLINE}) {
                    if($test->{DRIVER} ne "none") {
                        $test->{CMDLINE} = $CLIXName . $test->{DRIVER} . " " . $OutputDirWithSeperator . $base . ".exe";
                    } elsif ($test->{CORDBGDRIVER}) {
                        $test->{CMDLINE} = "perl " . $CordbgUtility . " " . $test->{NAME};
                    } else {
                        $test->{CMDLINE} = $CLIXName . $OutputDirWithSeperator . $base . ".exe";
                    }

                    if ($test->{PERLDRIVER}) {
                        $test->{CMDLINE} = "perl " . $base . ".pl " . $test->{CMDLINE};
                    }

                    if ($test->{BASELINEDRIVER}) {
                        $test->{CMDLINE} = "perl " . $BaselineUtility . " " . $test->{NAME} . " " . $test->{CMDLINE};
                    }
                }

                my $cmdline;
                my $GenStringsReference = "";

                # Assume that first file on files list is compiled to exe, all others to dll

                # If the test requires genstrings, copy it over and build in the reference
                if($test->{USESGENSTRINGS}) {
                    my $sourcefile = $GenStringsLocation . ".dll";   
                    my $targetfile = $OutputDirWithSeperator . "genstrings.dll";

                    if(CopyFile($sourcefile, $targetfile)) {
                        next TESTPROCESSING;
                    }

                    $sourcefile = $GenStringsLocation . ".ildb";   
                    $targetfile = $OutputDirWithSeperator . "genstrings.ildb";

                    if(CopyFile($sourcefile, $targetfile)) {
                        next TESTPROCESSING;
                    }
                    $GenStringsReference = " -r:" . $OutputDirWithSeperator . "genstrings.dll";
                }

                my $FxTestReference = "";

                # If the test requires fxtest, copy it over and build in the refence
                if($test->{USESFXTEST}) {
                    my $sourcefile = $FxTestLocation . ".dll";   
                    my $targetfile = $OutputDirWithSeperator . "fxtest.dll";

                    if(CopyFile($sourcefile, $targetfile)) {
                        next TESTPROCESSING;
                    }
                    
                    $sourcefile = $FxTestLocation . ".ildb";   
                    $targetfile = $OutputDirWithSeperator . "fxtest.ildb";

                    if(CopyFile($sourcefile, $targetfile)) {
                        next TESTPROCESSING;
                    }
                    $FxTestReference = " -r:" . $OutputDirWithSeperator . "fxtest.dll";
                }
                
                if((uc($ext) eq ".IL") or (uc($ext) eq ".IFL") ) {
                    if($index eq 0) {
                        $cmdline = "ilasm /debug /err" . $NoLogoOption . " /output:" . $OutputDirWithSeperator . "$base.exe $file";
                    } else {
                        $cmdline = "ilasm /debug /err /dll" . $NoLogoOption . " /output:" . $OutputDirWithSeperator . "$base.dll $file";
                    }
                } elsif (uc($ext) eq ".CS") {
                    my $Resource = "";
                    if($test->{HASRESOURCE}) {
                        $Resource = " /resource:" . $base . ".resources";
                    }
                    if($index eq 0 and not $test->{DLLOUTPUT}) {
                        $cmdline =  "csc " . $CSCFLAGS . $test->{CSHARPUNSAFEOPTION} . $Resource . $GenStringsReference . $FxTestReference . " /out:" . $OutputDirWithSeperator . "$base.exe $file";
                    } else {
                        $cmdline =  "csc " . $CSCFLAGS . $test->{CSHARPUNSAFEOPTION} . $Resource . $GenStringsReference . $FxTestReference . " /out:" . $OutputDirWithSeperator . "$base.dll /target:library $file";
                    }
                } elsif (uc($ext) eq ".JS") {
                    if($index eq 0 and not $test->{DLLOUTPUT}) {
                        $cmdline =   $JSCCOMPILER . " " . $JSCFLAGS . " -out:" . $OutputDirWithSeperator . "$base.exe $file";
                    } else {
                        $cmdline =   $JSCCOMPILER . " " . $JSCFLAGS . " -out:" . $OutputDirWithSeperator . "$base.dll /target:library $file";
                        #LogError("Do no yet support multiple .JS files in a tests");
                        #next TESTPROCESSING;
                    }
                } elsif ((uc($ext) eq ".EXE") or (uc($ext) eq ".PL")) {
                    $fDoCompile = 0;
                } else {
                    LogError("Unknown file extension [" . $ext . "]  - can't compile [" . $file . "]");
                    next TESTPROCESSING;
                }

                # If we have only one output, just append the rest of the files
                if($test->{ONEOUTPUT}) {
                    while(@files > 1) {
                        $cmdline = $cmdline . " " . pop(@files);
                    }
                    pop(@files);
                }

                # Compile the file

                if($fDoCompile) {
                    if($options{d}) {
                        # Just display the command line
                        LogMessage($cmdline);
                    } else {
                        LogMessage("Compiling [". $cmdline . "]");
                        $? = cmd_redirect ($cmdline);
                        if($?) {
                            LogError("Compile returned [" . $? . "] for " . $cmdline);
                            next TESTPROCESSING;
                        }
                    }
                }
            }
        }

        my($TestMidTime) = time();
            

        # Run the test
        if(not $test->{COMPILEONLY}) {
            if($options{d}) {
                # Just display the command line
                LogMessage($test->{CMDLINE});
            } else {
                #Execute test
                my $TestOutputFile = "rrun.test.log";
                LogMessage("Running [$test->{CMDLINE}]...");
                $? = cmd_redirect ("$test->{CMDLINE}", $TestOutputFile);

                LogResults($test, $TestMidTime - $TestStartTime, time() - $TestMidTime, time() - $TestStartTime, $?);
            }
        }
        
        LogMessage("Ending [$test->{NAME}] " . scalar(localtime));
    }
} 

# Calculate run time
my ($elapsedtime, $hours, $mins, $secs);
$elapsedtime = time() - $StartTime;
$secs = $elapsedtime % 60;
$elapsedtime = ($elapsedtime - $secs) / 60;
$mins = $elapsedtime % 60;
$elapsedtime = ($elapsedtime - $mins) / 60;
$hours = $elapsedtime;

# Set the verifier to a known state
verifier("veroff");

# Print the summary information.
LogMessage("Run Completed " . scalar(localtime) . "\n");
LogMessage("Summary:");
LogMessage("   Run time - " . $hours . "h " . $mins ."m " . $secs . "s");
LogMessage("   Processed - " . ($PassCount + $FailCount));
LogMessage("   Passed - " . $PassCount);
LogMessage("   Failed - " . $FailCount);

# Return an error if any tests failed
exit $FailCount if $FailCount > 0;

# No errors. Delete the error file if it's empty.
# If deletion fails, wait three seconds and try again.
if (-e $ErrFile and -z $ErrFile) {
    (sleep 3 and unlink $ErrFile) unless unlink $ErrFile;
}
exit 0; # Even if unlink fails, we have no errors from the tests
###############
# End of Main #
###############

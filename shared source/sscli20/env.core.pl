#!/usr/bin/env perl
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
# Sets up portable .NET Framework build environment
#
# This requires perl 5.6 or newer, just like the rest of the rotor build.
#
############################################################################
#
# Basic structure - we have a hash of variables which, if we don't hit an
# error, we print out to stdout at the end, in either sh or csh syntax,
# depending.
#
# Use the Get() and Set() functions to set and get the values from this hash.
# These just do a simple read/write. Since these writes are destructive, they
# don't read from the current environment (%ENV).
#
# Use Prepend(), PrependOnce(), and Append() to modify a variable. These copy
# the variable if necessary from the inherited environment (%ENV) before
# modification.
#
# Since the unix wrapper shell scripts "eval" the output of this program, for
# that to work, stdout must contain only the commands you want the parent
# shell to execute. So, stdout contains the commands which set the environment
# variables. Everything else (warnings, errors, ...) must go to stderr.
#
# The bourne shell wrapper (env.sh) redirects stderr to stdout after the eval.
# The csh wrapper (env.csh) does not.
#
# But, cmd.exe doesn't have the same sort of eval, so there we write the
# output to a file. So, for that scenario, stdout gets the regular output,
# and stderr gets the errors.
#
# There are three printing functions: CorOutput, CorPrint, CorPrintError.
# Depending on the value of the --output param, these write to:
#
#                --output=file   --output=- (or missing)
# CorOutput        file            stdout
# CorPrint         stdout          stderr
# CorPrintError    stderr          stderr
#
# Irregardless of which shell, use CorOutput for the text to be executed by
# the calling shell, CorPrint for information messages, and
# CorPrintError for error messages. Don't directly call print.
#
# Finally, when possible, use the modules perl provides to do things portably.
# For example, use the File::Spec functions to manipulate path names, and
# minimize the number of external programs you call.

use 5.6.0;
use strict;

use Cwd;
use Time::localtime;
use File::Spec::Functions ":ALL";
use Sys::Hostname;
use Getopt::Long;
use File::stat;

#
# Globals set from command line arguments
#


my $arg_nobrowse = 0;
my $arg_shell_type = 'sh';
my $arg_shell_type = ($^O eq "MSWin32") ? 'cmd' : 'sh';
my $arg_compiler = ($^O eq "MSWin32") ? 'msvc' : 'gcc';
my $arg_64 = 0;
my $arg_buildtype = 'checked';
my $arg_project = '';
my $arg_help = 0;
my $arg_output = '';

my $shell_handle = \*STDOUT;
my $output_handle = \*STDERR;
my $error_handle = \*STDERR;
my $output_to_file = 0;

#
# Platform functions
#

sub Windows
{
    return ($^O =~ /^MSWin/);
}

#
# Output Functions
#

sub CorOutput
{
    print $shell_handle "@_\n";
}

sub CorPrintError
{
    print $error_handle "Error: @_\n";
}

sub CorFail
{
    CorPrintError @_;
    exit 1;
}

sub CorPrint
{
    print $output_handle "@_\n";
}

sub SetOutputHandles
{
    my ($outfilename) = @_;
    if (!$outfilename || $outfilename eq "-") {
        # use defaults
    }
    else {
        open (EXPORT, ">", $outfilename) || CorFail "Could not write to $outfilename";
        $shell_handle  = \*EXPORT;
        $output_handle = \*STDOUT;
        $error_handle  = \*STDERR;
        $output_to_file = 1;
    }
}

sub CloseOutputHandles
{
    if ($output_to_file) {
        close $shell_handle;
    }
}


#
# Our table of vars
#

my %VarTable = ();

sub Set
{
    my ($varname, $value) = @_;
    $VarTable{$varname} = $value;
}

sub Get
{
    my ($varname) = @_;
    return $VarTable{$varname} || $ENV{$varname};
}

sub Prepend
{
    my ($varname, $sep, $value) = @_;
    if (!$VarTable{$varname}) {
        $VarTable{$varname} = $ENV{$varname};
    }
    if ($VarTable{$varname}) {
        $VarTable{$varname} = join $sep, $value, $VarTable{$varname};
    }
    else {
        $VarTable{$varname} = $value;
    }
}

sub PrependOnce
{
    my ($varname, $sep, $value) = @_;
    if (!$VarTable{$varname}) {
        $VarTable{$varname} = $ENV{$varname};
    }
    if ($VarTable{$varname}) {
        if ($VarTable{$varname} !~ /^$value/) {
            $VarTable{$varname} = join $sep, $value, $VarTable{$varname};
        }
    }
    else {
        $VarTable{$varname} = $value;
    }
}

sub Append
{
    my ($varname, $sep, $value) = @_;
    if (!$VarTable{$varname}) {
        $VarTable{$varname} = $ENV{$varname};
    }
    if ($VarTable{$varname}) {
        $VarTable{$varname} = join $sep, $VarTable{$varname}, $value;
    }
    else {
        $VarTable{$varname} = $value;
    }
}

#
# Variable exporting
#

sub CorExport
{
    my ($varname, $value) = @_;
    if ($arg_shell_type eq 'sh') {
        CorOutput ("$varname=\"$value\";");
        CorOutput ("export $varname;");
    }
    elsif ($arg_shell_type eq 'csh') {
        CorOutput ("setenv $varname \"$value\";");
    }
    elsif ($arg_shell_type eq 'cmd') {
        CorOutput ("set $varname=$value");
    }
    elsif ($arg_shell_type eq 'emacs') {
        CorOutput ("(setenv \"$varname\" \"$value\")");
    }
    else {
        CorPrintError "Invalid shell type: $arg_shell_type";
    }
}

sub OutputAll
{
    map {
        my $var = $_;
        CorExport ($var, $VarTable{$var})

    } sort keys %VarTable;
}

#
# Initialization
#

sub SanityCheck
{
    my $pwd = getcwd();
    if ($pwd =~ /\s/) {
        CorFail ("You cannot have spaces in the path to the SSCLI directory.");
    }
    if ($pwd =~ /[\x80-\xff]/) {
        CorFail ("You cannot have high-bit characters in the path to the SSCLI directory.");
    }
    if (localtime->year() < 100) {
        CorFail ("You cannot have your system's clock set to a year earlier than 2000.");
    }
    if ($ENV{"_NTROOT"}) {
        CorFail ("You cannot run env.[bat|csh|sh] more than once");
    }
}

sub Help
{
    my ($retval) = @_;
    print STDERR <<EOM;
Usage: env.core.pl [--help] [--project=[rotor]]
                   [--shell=cmd|csh|sh] [--compiler=gcc|lkgvc|mingw|msvc]
                   [--output=outfile]
EOM
    print STDERR <<EOM;
                   [chk|check|checked|dbg|debug|fre|free]
                   [nobrowse]
EOM
    print STDERR <<EOM;

Description:
    Prints shell commands for setting all the environment variables used by
    the rotor build process. To use, either capture the output to a
    file to "source" later, or "eval" the output directly in your shell.

Option Summary:
        --help:          prints this message

        --compiler:      one of: gcc (default on unix),
                                 msvc (default on windows),
                                 mingw, or lkgvc

EOM
    print STDERR <<EOM;
        --output:        output file name (- for stdout)

        --project:       choose a rotor build

        --shell:         whether to output settings for
                            cmd (default on windows), csh, or 
                            sh (default on unix)

        If the build type is:
            chk, check, or checked, then do a check build
            dbg, or debug, then do a debug build
            fre, or free, then do a free build

        If this option is omitted, the default build type is checked.
EOM
    print STDERR <<EOM;

        64: do a 64 bit build.
EOM
    exit ($retval);
}

sub ParseArgs
{
    my $result = GetOptions ("help"            => \$arg_help,
                             "output=s"        => \$arg_output,
                             "project=s"       => \$arg_project,
                             "shell=s"         => \$arg_shell_type,
                             "compiler=s"      => \$arg_compiler);
    if (!$result) {
        Help(1);
    }
    if ($arg_help) {
        Help(0);
    }
    if ($arg_shell_type !~ /^(csh|sh|cmd|emacs)$/) {
        CorPrintError "unknown shell: \"$arg_shell_type\"\n";
        Help(1);
    }
    if ($arg_project eq "") {
        $arg_project = "rotor";
    }
    if ($arg_project ne "rotor") {
        CorPrintError "unknown project: \"$arg_project\"\n";
        Help(1);
    }
    if ($arg_compiler !~ /^(gcc|lkgvc|mingw|msvc)$/) {
        CorPrintError "unknown compiler: \"$arg_compiler\"\n";
        Help(1);
    }
    SetOutputHandles ($arg_output);

    if ($#ARGV >= 0) {
        if ($ARGV[0] =~ /^(chk|check|checked)$/) {
            $arg_buildtype = "checked";
            shift @ARGV;
        }
        elsif ($ARGV[0] =~ /^(dbg|debug)$/) {
            $arg_buildtype = "debug";
            shift @ARGV;
        }
        elsif ($ARGV[0] =~ /^(fre|free)$/) {
            $arg_buildtype = "free";
            shift @ARGV;
        }

        if ($ARGV[0] eq "nobrowse") {
            $arg_nobrowse = 1;
            shift @ARGV;
        }
    }
    if ($#ARGV != -1) {
        Help(1);
    }
}

#
# Funtions which figure our exported environment variables
#

sub RunInEnv
{
    my ($command) = @_;
    my @vars = map { my $value=Get($_); $value ? "$_='" . $value . "' " : ""}
        ("PATH", "LD_LIBRARY_PATH", "DYLD_LIBRARY_PATH");
    my $full_command = "/bin/sh -c \"@vars $command\"";

    return `$full_command`;
}

sub Get_GCC_LIB_DIR
{
    my ($default_dir) = @_;
    my $gcc_output = RunInEnv ("gcc -v 2>&1 | head -1");
    if ($gcc_output =~ /Using built(-)?in specs/) {
        return $default_dir;
    }
    elsif ($gcc_output =~ m|^Reading specs from (.*)/specs$|) {
        return $1;
    }
    else {
        CorFail ("Could not parse gcc output");
    }
}

sub SetGccVars
{
    Set ("GCC_LIB_DIR", Get_GCC_LIB_DIR("/usr/lib"));
    my $gcc_eh_lib = catfile (Get("GCC_LIB_DIR"), "libgcc_eh.a");
    if (-f $gcc_eh_lib) {
        Set("GCC_EH_LIB", $gcc_eh_lib);
    }
    Set ("ROTOR_TOOLSET_VERSION", `gcc -dumpversion`);
}

sub UseCheckedInManagedTools
{
    Set("COMPLUS_InstallRoot", catdir (Get("LKGVC_DIR"), "tools", "x86", "managed"));
    Set("COMPLUS_Version", "v2.0");
    Set("MANAGED_TOOLS_ROOT", Get("COMPLUS_InstallRoot"));
    Set("MANAGED_TOOLS_VERSION", Get("COMPLUS_Version"));
}

sub SetMinGWVars
{
    # This variable is useful in sources files and other build files
    # to simply test for MinGW.
    Set ("MINGW_BUILD", "1");
    if (!Get("MINGW_PATH")) {
        Set ("MINGW_PATH", catdir (Get("ROTOR_DIR"), "tools", "mingw"));
    }
    Prepend ("PATH", ";", catdir (Get("MINGW_PATH"), "bin"));
    Set ("ROTOR_TOOLSET", "GCC");
    if (!Get("ROTOR_TOOLSET_VERSION")) {
        my $gcc_command_name = catfile (catdir(Get("MINGW_PATH"), "bin"), "gcc -dumpversion");
        my $gcc_command_result = `$gcc_command_name`;
        chomp($gcc_command_result);
        Set("ROTOR_TOOLSET_VERSION", $gcc_command_result);
    }
    Set("MINGW_LIB_DIR", catdir (Get("MINGW_PATH"), "lib"));
    Set("GCC_LIB_DIR", catdir (Get("MINGW_LIB_DIR"), "gcc-lib", "mingw32",
                               Get("ROTOR_TOOLSET_VERSION")));
    Set("MINGW_WIN32_INC_PATH", catdir (Get("MINGW_PATH"), "include"));
    Set("MINGW_WIN32_GCC_LIB_INC_PATH", catdir (Get("GCC_LIB_DIR"), "include"));
    UseCheckedInManagedTools();
}

sub SetLastKnownGoodVCVars
{
    # Empty MSVCDIR means we use LKG.
    Set("MSVCDIR", "");
    Set("ROTOR_TOOLSET", "MSVC");
    Set("ROTOR_TOOLSET_VERSION", "80");
    UseCheckedInManagedTools();
}

sub SetCompilerVars
{
    if ($arg_compiler eq "gcc") {
        SetGccVars();
    }
    elsif ($arg_compiler eq "msvc") {
       # nada
    }
    elsif ($arg_compiler eq "mingw") {
        SetMinGWVars();
    }
    elsif ($arg_compiler eq "lkgvc") {
        SetLastKnownGoodVCVars();
    }
    else {
        CorFail "Unknown compiler: $arg_compiler";
    }
}

sub eval_file
{
    my ($file) = @_;
    if ( -f $file) {
        open (FILE, $file) || CorFail ("Failed to open $file");
        my @lines = <FILE>;
        close ($file);
        eval ("@lines");
        if ($@) {
            CorFail ("Failed to eval() file $file:\n$@");
        }
    }
}

sub SetDebug
{
    my ($debug_type) = @_;
    if ($debug_type ne "PDB_ONLY") {
        Set("USE_PDB", "1");
    }
}

sub SetVars
{
    my $pwd = getcwd();

    if ($arg_nobrowse) {
        Set("BROWSER_INFO", "");
        Set("NO_BROWSER_INFO", "1");
        Set("NO_BROWSER_FILE", "1");
    }

    # Set a few environment variables for the SSCLI build process.
    # This file must be run from the root of the SSCLI distribution.
    Set("ROTOR_DIR", canonpath ("$pwd"));
    Set("NTMAKEENV", catdir (Get("ROTOR_DIR"), "env", "bin"));
    Set("FEATURE_PAL", 1);
    Set("CRT_INC_PATH", catdir (Get("ROTOR_DIR"), "palrt", "inc"));
    Set("SDK_INC_PATH", catdir (Get("ROTOR_DIR"), "pal", "inc"));

    # Set PAL_UNIX_DIR to point to the location of Unix PAL sources
    if (Get("PAL_UNIX_DIR") eq "") {
       Set("PAL_UNIX_DIR", "unix");
    }
    Set("C_DEFINES", "-DFEATURE_PAL");
    Set("CSC_DEFINES", "/d:FEATURE_PAL");
    if (!Windows()) {
        Set("PLATFORM_UNIX", 1);

        # This isn't a perfect test, but the likelihood of both /TMP and /ETC
        # existing on a case-sensitive system is very, very low.
        Set("FEATURE_CASE_SENSITIVE_FS", (-d "/TMP" && -d "/ETC" ) ? 0 : 1);

        Append("C_DEFINES", " ", "-DPLATFORM_UNIX");
        Append("CSC_DEFINES", " ", "/d:PLATFORM_UNIX");
        # set the default make command to "make"
        # if some platforms requires using other make it will be overwritten
        # this makes sure the the MAKE variable is defined

        Set("MAKE", "make");
    }

    if ($arg_project ne "rotor") {
        Append("C_DEFINES", " ", "-D" . uc($arg_project));
        Append("CSC_DEFINES", " ", "/d:" . uc($arg_project));
    }

    #

    my $platform_os = lc(Windows() ? $^O : `uname -s`);
    my $platform_os_version = "";

    if (Windows()) {
        # The output of ver looks like:
        #
        # Microsoft Windows XP [Version 5.1.2600]
        #
        # (including the empty line before)

    	$platform_os_version = `ver`;
        # we keep the first two fields of the version number
	if (! ($platform_os_version =~ s/\nMicrosoft.*\[Version +([0-9]+\.[0-9]+)\..*\]/$1/g)) {
            $platform_os_version = "";
        }

	if ($platform_os_version =~ /^5\..*/) {
            $platform_os_version = "5.1";
        }
    } else  {
	$platform_os_version = `uname -r`;
        # we keep all fields of the version number
        if (! ($platform_os_version =~ s/^[^0-9]*([0-9.]+).*$/$1/s)) {
            $platform_os_version = "";
        }

    }
    chomp ($platform_os);
    chomp ($platform_os_version);
    if (! $platform_os_version ) {
        CorFail ("Could not get platform OS version");
    }
    $platform_os_version = lc($platform_os_version);

    my $architecture='';
    if ($platform_os =~ /^(darwin|freebsd)$/) {
        $architecture=`uname -p`;
        chomp ($architecture);
    }
    elsif ($platform_os eq "mswin32") {
        $architecture = Get("PROCESSOR_ARCHITECTURE");
        if ($architecture eq "x86") {
            $architecture = "i386";
        }
    }
    else {
        $architecture = "unknown";
    }
    Set("PLATFORM_OS", $platform_os);
    Set("PLATFORM_OS_VERSION", $platform_os_version);

    if ($platform_os =~ /^freebsd$/) {
        Append("C_DEFINES", " ", "-DPTHREAD_RESTRICTS_INITIAL_THREAD_STACKSIZE=1 -DPTHREAD_INITIAL_THREAD_MAX_STACKSIZE=0x100000");
    }

    if ($arg_64) {
        CorPrint "64-bit build";
        Set("_BIT64", "1");
    }
    else {
        CorPrint "32-bit build";
        Set("_BIT64", "0");
    }

    my $dir_startup=catdir (Get("ROTOR_DIR"), "dir.startup.pl");
    if (-f $dir_startup)
    {
        eval_file ($dir_startup);
    }
    if (Get("BASEDIR") eq "") {
        Set("BASEDIR", Get("ROTOR_DIR"));
    }
    if (Get("NDPDIR") eq "") {
        Set("NDPDIR", Get("ROTOR_DIR"));
    }
    if (Get("VSADIR") eq "") {
        Set("VSADIR", catdir(Get("ROTOR_DIR"), "jscript"));
    }
    
    my $basedir = Get("BASEDIR");
    if (Windows()) {
        # BUILD.EXE insists on having some particular
        # environment variables pointing to our root
        my ($volume, $directories, $file) = splitpath($basedir, 1);
        Set("_NTDRIVE", $volume);
        Set("_NTROOT", $directories);
    }
    else {
        Set("_NTROOT", $basedir);
    }

    # Since different platform or host may require different setup for
    # PATH, LD_LIBRARY_PATH, or other environment variables,
    # we'll check and run the platform startup file first,
    # and then check and run the platform.host startup file.

    my $platform_startup=catdir (Get("ROTOR_DIR"), "$platform_os.startup.pl");
    my $hostname = hostname();
    $hostname =~ s/\..*$//;
    my $host_startup=catdir (Get("ROTOR_DIR"), "$platform_os.$hostname.startup.pl");

    if (-f $platform_startup) {
        eval_file ($platform_startup);
        # do we really only want to eval $host_startup when we have
        # a $platform_startup?
        if (-f $host_startup) {
            eval_file ($host_startup);
        }
    }

    if ($architecture =~ /i[3-6]86/) {
        Set("PROCESSOR_ARCHITECTURE", "x86");
        Set("_BUILDARCH", "rotor_x86");
        if (!Windows()) {
            Set("ROTOR_X86", "1");
            Set("PLATFORM", $platform_os);
        }
        SetCompilerVars();
    }
    elsif ($architecture eq "powerpc") {
        Set("PROCESSOR_ARCHITECTURE", "ppc");
        Set("PPC", "1");
        Append("CSC_DEFINES", " ", "/d:BIGENDIAN");

        if (Get("PLATFORM_OS") eq "darwin") {
            Set("_BUILDARCH", "ppc");
            Append("CSC_DEFINES", " ", "/d:__APPLE__");
            # ROTORTODO: This needs to be fixed to work with the current Apple compilers
            # if ($arg_buildtype ne "debug") {
            #     Set("FEATURE_PRECOMPILED_HEADERS", "1");
            # }
        }
        else {
            CorFail ("Unknown platform with $architecture: " . Get("PLATFORM_OS"));
        }
    }
    else {
        CorFail ("$architecture is not a supported processor");
    }

    Set("BUILD_DEFAULT_TARGETS", "-dynamic " . Get("_BUILDARCH"));
    if (Windows()) {
        Set("BUILD_DEFAULT", "-mwe -a -M");
    }
    else {
        Set("BUILD_DEFAULT", "-iwe -a -M");
    }
    
    # Treat warnings as errors
    Append ("BUILD_DEFAULT", " ", "-wx");

    if (Get("FEATURE_CASE_SENSITIVE_FS") eq "1") {
        Append ("C_DEFINES", " -DFEATURE_CASE_SENSITIVE_FILESYSTEM");
        Append ("CSC_DEFINES", " ", "/d:FEATURE_CASE_SENSITIVE_FILESYSTEM");
    }
    Append ("C_DEFINES", " ", "-DPAL_PORTABLE_SEH");

    if ($arg_project eq "rotor") {
        Append ("C_DEFINES", " ", "-DGC_SMP");
        Append ("C_DEFINES", " ", "-DFJITONLY");
    }

    Set("PAL_LIB_PERF", "");

    if ($arg_buildtype eq "free") {
        CorPrint "Free Environment";
        Set("_BUILDTYPE", "fre");
        Append("C_DEFINES", " ", "-DNTMAKEENV -DNDEBUG");
        Set("NTDEBUG", "ntsdnodbg");
        if (Windows()) {
            Set("_BUILDOPT", "full opt");
            Set("NTDBGFILES", "");
            Prepend ("MSC_OPTIMIZATION", " ", "/O1");
            Set("BUILD_ALT_DIR", "");
            Set("BROWSER_INFO", "");
            Set("DEBUG_CRTS", "");
            if (my $color = Get("COLOR_FREE")) {
                CorOutput ("color $color");
            }
        }
        else {
            Set("BUILD_ALT_DIR", "r");
        }
    }
    elsif ($arg_buildtype eq "debug") {
        CorPrint "Debug Environment";
        Set("_BUILDTYPE", "dbg");
        Append("C_DEFINES", " ", "-DNTMAKEENV -D_DEBUG");
        Set("BUILD_ALT_DIR", "d");
        Set("NTDEBUG", "ntsd");
        if (Windows()) {
            Set("_BUILDOPT", "no opt");
            Set("FPO_OPT", "");
            Set("NTDBGFILES", "");
            Prepend("MSC_OPTIMIZATION", " ", "/Od /Oi");
            if (Get("NO_BROWSER_INFO") ne "1") {
                Set("BROWSER_INFO", "1");
            }
            Set("DEBUG_CRTS", "1");
            if (my $color = Get("COLOR_CHECKED")) {
                CorOutput ("color $color");
            }
            SetDebug(Get("DEBUG_TYPE"));
        }
        else {
            Set("MSC_OPTIMIZATION", "");
        }
    }
    elsif ($arg_buildtype eq "checked") {
        CorPrint "Checked Environment";
        Set("_BUILDTYPE", "chk");
        Append("C_DEFINES", " ", "-DNTMAKEENV -D_DEBUG");
        Set("BUILD_ALT_DIR", "c");
        Set("NTDEBUG", "ntsd");
        if (Windows()) {
            Set("DUMP_CRT_LEAKS", "");
            Set("_BUILDOPT", "full opt");
            Set("NTDBGFILES", "");
            Set("FPO_OPT", "1");
            Prepend("MSC_OPTIMIZATION", " ", "/O1");
            if (Get("NO_BROWSER_INFO") ne "1") {
                Set("BROWSER_INFO", "1");
            }
            if (my $color = Get("COLOR_CHECKED")) {
                CorOutput ("color $color");
            }
            Set("DEBUG_CRTS", "1");
            SetDebug(Get("DEBUG_TYPE"));
        }
    }
    else {
        CorFail ("Invalid buildtype $arg_buildtype");
    }

    if (Windows()) {
        if (Get("ROTOR_TOOLSET") ne "MSVC") {
            Set("MSC_OPTIMIZATION", "");
        }
        if (!Get("_NTTREE")) {
            Set("_NTTREE",
                catdir (Get("BASEDIR"),
                        "binaries." . Get("_TGTCPUTYPE") . Get("_BUILDTYPE") . ".rotor"));
        }
        Prepend("_NT_SYMBOL_PATH", ";", Get("_NTTREE"));
        Prepend("_NT_SYMBOL_PATH", ";", catdir(Get("_NTTREE"), "Symbols"));
        Prepend("_NT_DEBUGGER_EXTENSION_PATH", ";", catdir(Get("_NTTREE")));

        Prepend("PATH", ";", catdir (Get("_NTTREE"), "int_tools"));
        Prepend("PATH", ";", catdir (Get("_NTTREE"), "sdk", "bin"));
        Prepend("PATH", ";", Get("_NTTREE"));

        Set("NTDEBUGTYPE", "vc6");
    }
    else {
        # Set the build log filename to include build type and architecture
        Set("LOGBASENAME", "build" . Get("BUILD_ALT_DIR") . "_" . Get("_BUILDARCH"));
        Append("BUILD_DEFAULT", " ", "-j " . Get("LOGBASENAME"));

        my $archsuffix = Get("PROCESSOR_ARCHITECTURE");

        #

        Set("_NTTREE", catdir (Get("BASEDIR"), "binaries." . $archsuffix  . Get("_BUILDTYPE") . ".rotor"));

        Append("PATH", ":", Get("_NTTREE"));
        Append("PATH", ":", catdir (Get("_NTTREE"), "sdk", "bin"));
        Append("PATH", ":", catdir (Get("_NTTREE"), "int_tools"));

        Append(Get("PLATFORM_OS") eq "darwin" ? "DYLD_LIBRARY_PATH" : "LD_LIBRARY_PATH", ":", Get("_NTTREE"));
        Set("LD_LIB_DIRS", "-L" . Get("_NTTREE"));
    }
}

sub Finish
{
    if (Windows()) {
        if (!Get("ROTOR_TOOLSET") || !Get("ROTOR_TOOLSET_VERSION")) {
            CorFail "unknown toolset version";
        }

        CorPrint ("Building for Operating System - " . Get("_TGTOS"));
        CorPrint ("             Processor Family - " . Get("_TGTCPUTYPE"));
        CorPrint ("                    Processor - " . Get("_TGTCPU"));
        CorPrint ("                   Build Type - " . Get("_BUILDTYPE"));
        CorOutput ("title " . ucfirst($arg_project) . " Window: %t% %_BUILDARCH%/%_BUILDTYPE%/%_BUILDOPT%/binaries in: %_NTTREE%");
        if ($arg_compiler eq "lkgvc" || $arg_compiler eq "mingw") {
            CorOutput ("perl " .
                       catfile (Get("MANAGED_TOOLS_ROOT"),
                                Get("MANAGED_TOOLS_VERSION"),
                                "shim", "InstallShim.pl"));
        }
    }
    CloseOutputHandles();
}

#
# Do It
#

ParseArgs();
SanityCheck();
SetVars();
OutputAll();
Finish();

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

# add this directory to @INC to bring in parsefiles
my $path_to_me;
BEGIN {
    ($path_to_me) = ($0 =~ m@^(.+)[\/\\][^\/\\]+@);
    $path_to_me = "." unless $path_to_me;    
}
use lib $path_to_me;

use strict;

my $errmsg = "spp.pl : error :";

sub main();
main();
exit(0);

sub usage {
    $? = 1;

    my $msg = shift;
    if (defined($msg)) {
        warn "$errmsg $msg\n\n";
    }

    die <<"USAGE" . "\n";
spp : Simple Pre-Processor

Usage: spp.pl /i:<infile> /o:<outfile> [/s:<subsfile>] [/d:<delimiter>] [var[=expr]]*
    -?  : Display this message.
    -i  : Input file
    -o  : Output file
    -s  : Substitution file. Each line is in the format <var>[=<expr>]
    -d  : Delimiter, default is '%'.
USAGE
}

sub main() {
    my $infile = "";
    my $outfile = "";
    my $subsfile = "";
    my $delim = '%';
    my @vars = ();  # substitution variables and defines

    # process args
    my $options = 'iosd';
    foreach my $arg (@ARGV) {
        if ($arg =~ m{^[/-]}o) {
            my ($optionName,$optionValue) = ($arg =~ /^[\/-]([$options])\:(.+)$/i);
            if (!defined($optionName)) {
                usage("Invalid argument $arg.");
            }

            if (!defined($optionValue)) {
                usage("Argument $arg requires a value.");
            }

            $optionName = lc($optionName);
            if ($optionName eq 'i') {
                $infile = $optionValue;
            }
            elsif ($optionName eq 'o') {
                $outfile = $optionValue;
            }
            elsif ($optionName eq 's') {
                $subsfile = $optionValue;
            }
            elsif ($optionName eq 'd') {
                $delim = $optionValue;
                if (length($optionValue) != 1) {
                    usage("Argument /d requires a single character value.");
                }
            }
            else {
                usage("Unrecognized argument $optionName");
            }
    
            $options =~ s/$optionName//;
        }
        else {
            push @vars, $arg;
        }
    }

    if (length($infile) == 0 or length($outfile) == 0) {
        usage("command is missing required parameters.");
    }

    my %defines;
    my %subs;

    # read substitution file, add contents to vars
    if (length($subsfile) > 0) {
        open(SUBS, $subsfile) || die "$errmsg Can't open $subsfile: $!, stopped";
        while (my $line = <SUBS>) {
            if ($line !~ m/^\s*$/) {
                push @vars, $line;
            }
        }

        close(SUBS) ||  die "$errmsg Can't close $subsfile: $!, stopped";
    }

    # populate %defines & %subs from @vars
    foreach my $var (@vars) {
        # get key and value, trim whitespace
        my ($k, $v) = split(/=/, $var, 2);
        $k =~ s/^\s*\b(\w+)\s*$/$1/;
        if (defined($v)) {
            $v =~ s/^\s*(\S+)\s*$/$1/;
        }

        next if length($k) == 0;

        if (defined($defines{$k}) or defined($subs{$k})) {
            die "$errmsg: substitution value for $k specified multiple times, stopped";
        }

        # if there is no value, or the value is not zero, treat as defined
        if (!defined($v) || $v) {
            $defines{$k} = 1;
        }

        # if there is a value, add it subs with the key delimited
        if (defined($v) && length($v) > 0) {
            $subs{$delim . $k . $delim} =  $v;
        }
    }

    # current state, represented in a stack
    # possible values are:
    #   1: use lines that follow
    #   0: omit lines that follow
    #  -1: omit lines that follow, and all further elseif and else clauses
    my @states = (1);
        
    open (INFILE, "<$infile") || die("$errmsg Cannot open $infile: $!, stopped");
    open (OUTFILE, ">$outfile") || die("$errmsg Cannot open $outfile: $!, stopped");

    while (my $line = <INFILE>) {
        # process if/else/elseif/endif
        if ($line =~ m/^\s*\#if\b/) {
            my $state;
            my ($expr) = ($line =~ m/^\s*\#if\s+(\w+)\s*$/);
            if ($expr) {
                $state = $defines{$expr} ? 1 : 0;
            }
            else {
                ($expr) = ($line =~ m/^\s*\#if\s+\!\s*(\w+)\s*$/);
                if ($expr) {
                    $state = $defines{$expr} ? 0 : 1;
                }
                else {
                    die "$errmsg Syntax error in #if statement in $infile line $., stopped";
                }
            }

            push @states, $state;
            next;
        }
        elsif ($line =~ m/^\s*\#elseif\b/) {
            my $state;
            my ($expr) = ($line =~ m/^\s*\#elseif\s+(\w+)\s*$/);
            if ($expr) {
                $state = $defines{$expr} ? 1 : 0;
            }
            else {
                ($expr) = ($line =~ m/^\s*\#elseif\s+\!\s*(\w+)\s*$/);
                if ($expr) {
                    $state = $defines{$expr} ? 0 : 1;
                }
                else {
                    die "$errmsg Syntax error in #elseif statement in $infile line $., stopped";
                }
            }

            if (@states <= 1) {
                die "$errmsg No matching #if for #elseif statement in $infile line $., stopped";
            }
    
            my $lastState = pop @states;
            if ($lastState != 0) {
                $state = -1;
            }
            push @states, $state;
            next;
        }
        elsif ($line =~ m/^\s*\#else\b/) {
            if ($line !~ m/^\s*\#else\s*$/) {
                die "$errmsg Syntax error in #else statement in $infile line $., stopped";
            }

            if (@states <= 1) {
                die "$errmsg No matching #if for #else statement in $infile line $., stopped";
            }
    
            my $state;
            my $lastState = pop @states;
            if ($lastState == 0) {
                $state = 1;
            }
            else {
                $state = -1;
            }

            push @states, $state;
            next;
        }
        elsif ($line =~ m/^\s*\#endif\b/) {
            if ($line !~ m/^\s*\#endif\s*$/) {
                die "$errmsg Syntax error in #else statement in $infile line $., stopped";
            }

            if (@states <= 1) {
                die "$errmsg No matching #if for #endif statement in $infile line $., stopped";
            }
    
            pop @states;
            next;
        }
    
        # check current state at top of state stack
        if ($states[$#states] != 1) {
            next;
        }
    
        # process substitutions
        foreach my $k (keys %subs) {
            my $v = $subs{$k};
            $line =~ s/$k/$v/gi;
        }
    
        print OUTFILE $line;
    }

    close (OUTFILE) || die("$errmsg Cannot close $outfile: $!, stopped");
    close (INFILE) || die("$errmsg Cannot close $infile: $!, stopped");

    print("spp.pl: $outfile created successfully.\n");
}


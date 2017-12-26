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

# misc stuff
require 5;
use Getopt::Std;
use strict;

############################################################################
# globals

use vars qw( $opt_v $opt_h $opt_o $opt_s $opt_l $opt_f );
my $g_seen_exports = 0;
my $g_linenum = 0;
my @g_symbols = ();
my @g_libs = ();
my $g_progname="def2ld.pl";
my $g_format = "gnu";

############################################################################
# Usage

sub usage {
    print STDERR "$g_progname: Generates a ld linker script file from a .def file.\n";
    print STDERR "Arguments: [-h] [-v] [-s exported sym list] [-l exported lib list]\n";
    print STDERR "           [-o output] [-f output_format] [def_file]\n";
    print STDERR "Details:\n";
    print STDERR "        -h: prints this message\n";
    print STDERR "        -f: [default=gnu]. either gnu or apple\n";
    print STDERR "        -v: prints verbose output\n";
    print STDERR "        -s: takes a comma seperated list of symbols to export\n";
    print STDERR "        -l: takes a comma seperated list of libraries.\n";
    print STDERR "            All global symbols from each library are exported.\n";
    print STDERR "        -o: specifies output file. If none, it prints to stdout.\n";
    print STDERR "  def_file: the input .def file. If none, it reads from stdin.\n";
    exit 1;
}

############################################################################
# parse command line

getopts ('vho:s:l:f:');

if ($opt_h) {
    usage();
}

if ($opt_f) {
    if ($opt_f eq "gnu") {
        $g_format = "gnu";
    }
    elsif ($opt_f eq "apple") {
        $g_format = "apple";
    }
    else {
        usage();
    }
}

if ($opt_o) {
    print "$g_progname: Writing to output file: \"$opt_o\"\n" if ($opt_v);
    if (!open (OUTPUT, ">$opt_o")) {
        die "$g_progname: FATAL ERROR: Could not open output file: \"$opt_o\"\n";
    }
}
else {
    open (OUTPUT, ">-") || die "Failed to open stdout!";
}

if ($opt_l) {
    @g_libs = split /,+/, $opt_l;
    add_syms_from_libs (@g_libs);
}

if ($opt_s) {
    push @g_symbols, split /,+/, $opt_s;
}

if ($#ARGV > 0) {
    usage();
}

# Process input def file

while (my $line = <>) {
    chomp $line;
    $g_linenum++;
    $line =~ s/[#;].*//;
    print "$g_progname: read line: $line\n" if ($opt_v);
    if ($g_seen_exports) {
        process_symbol_line ($line)
    }
    else {
        $g_seen_exports = look_for_exports ($line);
        if ($opt_v && $g_seen_exports) {
            print "$g_progname: EXPORTS found at line $g_linenum\n";
        }
    }
}

if (!$g_seen_exports) {
    die "$g_progname: FATAL ERROR: Never found EXPORTS";
}

my @sorted_symbols = sort @g_symbols;

if ($g_format eq "gnu") {
    print_ld_script_gnu();
}
elsif ($g_format eq "apple") {
    print_ld_script_apple();
}
else {
    die "Invalid output format: $g_format";
}

# finish up

close OUTPUT;

############################################################################
# parsing functions

sub look_for_exports {
    my $line = shift;
    return $line =~ /EXPORTS/;
}

sub clean_symbol {
    # removes the @ORDINAL blobs that can occur at the end of a .def symbol
    # entry, and checks for other syntax that we don't like
    my $symbol = shift;
    if ($symbol =~ /=/) {
        die "$g_progname: FATAL ERROR: illegal '=' at line $g_linenum";
    }
    my @pieces = split "@", $symbol;
    return $pieces[0];
}

sub process_symbol_line {
    my $line = shift;
    my @fields = split " ", $line;
    my $add_symbol = 0;
    if ($#fields == 0) {
        $add_symbol = 1;
        print "$g_progname: Added symbol: $fields[0]\n" if ($opt_v);
    }
    elsif ($#fields == 1) {
        if ($fields[1] =~ /^private$/i) {
            $add_symbol = 1;
            print "$g_progname: Added private symbol $fields[0]\n" if ($opt_v);
        }
        elsif ($fields[1] =~ /^data$/i) {
            $add_symbol = 1;
            print "$g_progname: Added data symbol: $fields[0]\n" if ($opt_v);
        }
        elsif ($fields[1] =~ /^@[0-9]+/) {
            $add_symbol = 1;
            print "$g_progname: Added ordinal symbol: $fields[0]\n" if ($opt_v);
        }
        else {
            die "$g_progname: FATAL ERROR Line $g_linenum: Unexpected symbol modifier: \"$fields[1]\"";
        }
    }
    elsif ($#fields > 1) {
        die "$g_progname: FATAL ERROR Line $g_linenum: too many fields";
    }
    if ($add_symbol) {
        push @g_symbols, clean_symbol ($fields[0]);
    }
}

############################################################################
# Library reading functions

sub get_libs_global_syms {
    my $lib_name = shift;
    my $command = "nm $lib_name";
    my @syms = ();
    if (!open (NM_HANDLE, "$command|")) {
        die "$g_progname: FATAL_ERROR: could not run: \"$command\"";
    }
    LINE: while (my $line = <NM_HANDLE>) {
        chomp $line;
        # nm prints out four kinds of lines:
        #   1. blank
        #   2. ^objectname.o:
        #   3. ^address type name
        #   4.          type name
        #
        # Since nm can vary a bit from machine to machine, sanity check the
        # output a good deal to try and forstall surprises.

        # first, screen out any ignorable lines
        next LINE if ($line =~ /^\s*$/);
        next LINE if ($line =~ /^\S+:\s*$/);

        # have an actual symbol line
        my @fields = split " ", $line;
        if ($#fields >= 2) {
            die "$g_progname: bad nm line: \"$line\"" unless ($fields[0] =~ /^[0-9a-fA-F]+$/);
            shift @fields;
        }
        die "$g_progname: bad nm line: \"$line\"" unless ($#fields == 1);
        my ($type, $name) = @fields;
        print "$g_progname: \"$lib_name:$name\" has type \"$type\"\n" if ($opt_v);
        if ($type =~ /^[ABDGNRSTUVW]+$/) {
            # this catches global undefined and weak symbols too. Don't know
            # if we want to filter those. We exclude global common symbols,
            # since we don't want to export those if there's only a 'C' entry
            # for it. 
            print "$g_progname: $lib_name:$name is global\n" if ($opt_v);
            push @syms, $name;
        }
    }
    close NM_HANDLE;
    return @syms;
}

sub add_syms_from_lib {
    my $lib_name = shift;
    print "$g_progname: Adding symbols from lib \"$lib_name\"\n" if ($opt_v);
    my @syms = get_libs_global_syms ($lib_name);
    foreach my $sym (@syms) {
        push @g_symbols, $sym;
        print "$g_progname: Adding symbol: $sym to exported symbol list\n" if ($opt_v);
    }
}

sub add_syms_from_libs {
    foreach my $lib (@_) {
        add_syms_from_lib ($lib);
    }
}

############################################################################
# Output functions

sub print_ld_script_header_gnu {
    print OUTPUT <<"EOF";
VERS_1.1 {
        global:
EOF
}

sub print_ld_script_footer_gnu {
    print OUTPUT <<"EOF";
        local:
                *;
};
EOF
}

sub print_ld_script_gnu {
    print_ld_script_header_gnu();
    # gnu ld script files must have at least one exported symbol, and gnu ld quietly
    # ignores symbols which aren't in the library.
    if ($#sorted_symbols == -1) {
        print OUTPUT "                PLACEHOLDER_NONEXISTANT_SYMBOL;\n";
    }
    for my $sym (@sorted_symbols) {
        print OUTPUT "                $sym;\n";
    }
    print_ld_script_footer_gnu();
}


sub print_ld_script_apple {
    # Apple's ld complains if we export a nonexistant symbol, but doesn't mind
    # an empty exported symbols file.
    for my $sym (@sorted_symbols) {
        print OUTPUT "_$sym\n";
    }
}

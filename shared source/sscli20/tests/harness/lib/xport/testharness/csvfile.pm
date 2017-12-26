#
# 
#  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
# 
#  The use and distribution terms for this software are contained in the file
#  named license.txt, which can be found in the root of this distribution.
#  By using this software in any fashion, you are agreeing to be bound by the
#  terms of this license.
# 
#  You must not remove this notice, or any other, from this software.
# 
#

package xport::testharness::csvfile;

use vars qw($VERSION @ISA @EXPORT);
use strict;

# This perl module understands how to parse out the comma separated
# variables in a test result file from the Rotor on Unix 'testharness'
# program.

$VERSION = "1.1";

require 5.000;

use Exporter;

@ISA = qw(Exporter);
@EXPORT = qw(parseEscapedCSVLine slurpFile makeEscapedCSVLine );

use xport::testharness::record;


# 'parseEscapedCSVLine' breaks up a line according to the output standards
# in testharness comma delimiters with literal characters
# (e.g. commas and backslashes) in values quoted with backslashes.

sub parseEscapedCSVLine {
  my($delimiter, $quotechar, $line) = @_;
  my($qdelimiter,  $qquotechar, $piece, @valueparts, @chunks, @values);

  $qdelimiter = quotemeta $delimiter;
  $qquotechar = quotemeta $quotechar;

  @chunks = split "$delimiter", $line;

  @valueparts = ();
  my $chunk;
  foreach $chunk (@chunks) {
    if ( $chunk =~ m/(.*)$qquotechar$/ ) {
      push @valueparts, $1;
    } else {
      my $value;
      if ( @valueparts ) {
	push @valueparts, $chunk;
	$value = join $delimiter, @valueparts;
	@valueparts = ();
      } else {
	$value = $chunk;
      }
      $value =~ s/$qquotechar(.)/$1/g ;

      push @values, $value;
    }
  }

  return(@values);
}

sub escapify {
  my ($delimiter, $qquotechar, $subject) = @_;
  if ( defined $subject ) {
    $subject =~ s/\([$qquotechar$delimiter]\)/$qquotechar$1/g;
  } else {
    $subject = "";
  }
  return $subject;
}
    
sub makeEscapedCSVLine {
  my $record = shift;
  my ($delimiter, $qquotechar) = @_;
  return join $delimiter, map { escapify($delimiter, $qquotechar, $_) }
                       (
			$record->directory(),
			$record->category(),
			$record->function(),
			$record->description(),
			$record->flags(),
			$record->type(),
			$record->result(),
			$record->comment(),
			$record->unknown(),
		   );
}

# Slurp in the file named into the hash referenced.  Watch for
# duplicate keys in the process.

sub slurpFile ($$\%\@) {

  my ($filename, $idTypeRequested, $hashref, $duplistref) = @_;

  open(THEFILE,$filename) or die;
  my @file = <THEFILE>;
  close(THEFILE);

  my ($entry, $CurrentRecord);
  foreach $entry (@file) {

    $CurrentRecord = new Xport::TestHarness::Record();
    chomp $entry;

    my($dir,$cat,$func,$desc,$flags,$type,$res,$comment,@leftovers) = parseEscapedCSVLine(',','\\',$entry);

    # Insert the data into the Record object
    $CurrentRecord->directory($dir);
    $CurrentRecord->category($cat);
    $CurrentRecord->function($func);
    $CurrentRecord->description($desc);
    $CurrentRecord->flags($flags);
    $CurrentRecord->type($type);
    $CurrentRecord->result($res);
    $CurrentRecord->comment($comment);
    $CurrentRecord->unknown(@leftovers);

    if ( $$hashref{$CurrentRecord->id()} ) {
      printf "Duplicate record ID detected: \n key: '%s' \n new: '$entry' \n previous: '%s' \n", 
	$CurrentRecord->id(),
	makeEscapedCSVLine($$hashref{$CurrentRecord->id()}, ',', '\\');
      push @{$duplistref}, $CurrentRecord;
    } else {
      if   ( $idTypeRequested eq '2' ) {
          $$hashref{$CurrentRecord->idx2()} = $CurrentRecord;;
      } if ( $idTypeRequested eq '3' ) {
          $$hashref{$CurrentRecord->id()}   = $CurrentRecord;;
      }
    }
  }

  return $hashref;
}

1; # end the module properly

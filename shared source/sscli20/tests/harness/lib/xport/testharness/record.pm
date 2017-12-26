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

package Xport::TestHarness::Record;

# This perl module understands the structure of a test result from the Rotor
# on Unix testharness program.


## Constructor
sub new {

  my $proto = shift;
  my $class = ref($proto) || $proto;

  my $record  = {};

  $record->{ID}            = undef;
  $record->{DIRECTORY}     = undef;
  $record->{CATEGORY}      = undef;
  $record->{FUNCTION}      = undef;
  $record->{DESCRIPTION}   = undef;
  $record->{FLAGS}         = undef;
  $record->{TYPE}          = undef;
  $record->{RESULT}        = undef;
  $record->{COMMENT}       = undef;
  $record->{UNKNOWN}       = undef;

  bless($record,$class);
  return $record;
}

## Object Methods

## Reset the record's unique identifier given its current state.  Perl will complain
## about undefined values if used before the data for 'complete' record have all
## been specified!
sub SetUniqueId() {

  my $record = shift;
  $record->{ID} = join '|', ( $record->{DIRECTORY},
			      $record->{TYPE},
			      $record->{FLAGS}
			    );
}

sub SetUniqueIdx2() {

  my $record = shift;
  $record->{ID} = join '|', ( $record->{DIRECTORY},
                              $record->{TYPE},
                            );
}

## Return an array of the data in a record
sub arrayData {

  my $record = shift;

  my @temp;
  $temp[0] = $record->{DIRECTORY};
  $temp[1] = $record->{CATEGORY};
  $temp[2] = $record->{FUNCTION};
  $temp[3] = $record->{DESCRIPTION};
  $temp[4] = $record->{FLAGS};
  $temp[5] = $record->{TYPE};
  $temp[6] = $record->{RESULT};
  $temp[7] = $record->{COMMENT};
  $temp[8] = $record->{UNKNOWN};
  return @temp;
}

sub noundefs {
  my $record = shift;
  my $key;
  foreach $key (keys %$record) {
    $record->{$key} = "" if (! defined $record->{$key} );
  }
}

## Get methods
sub id {
  my $record = shift;
  if ( ! defined $record->{ID}) {
    $record->SetUniqueId();
  }
  return $record->{ID};
}

sub idx2 {
  my $record = shift;
  if ( ! defined $record->{ID}) {
    $record->SetUniqueIdx2();
  }
  return $record->{ID};
}

## Get/Set methods
sub directory {
  my $record = shift;
  if (@_) { $record->{DIRECTORY} = shift }
  return $record->{DIRECTORY};
}

sub category {
  my $record = shift;
  if (@_) { $record->{CATEGORY} = shift }
  return $record->{CATEGORY};
}

sub function {
  my $record = shift;
  if (@_) { $record->{FUNCTION} = shift }
  return $record->{FUNCTION};
}

sub description {
  my $record = shift;
  if (@_) { $record->{DESCRIPTION} = shift }
  return $record->{DESCRIPTION};
}

sub flags {
  my $record = shift;
  if (@_) { $record->{FLAGS} = shift }
  return $record->{FLAGS};
}

sub type {
  my $record = shift;
  if (@_) { $record->{TYPE} = shift }
  return $record->{TYPE};
}

sub result {
  my $record = shift;
  if (@_) { $record->{RESULT} = shift }
  return $record->{RESULT};
}

sub comment {
  my $record = shift;
  if (@_) { $record->{COMMENT} = shift }
  return $record->{COMMENT};
}

sub unknown {
  my $record = shift;
  if (@_) { $record->{UNKNOWN} = shift }
  if (! defined  $record->{UNKNOWN}) { $record->{UNKNOWN} = () }
  return $record->{UNKNOWN};
}

1;

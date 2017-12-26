#! /usr/bin/perl -w

# This script takes the results recorded by the test
# harness in a comma separated values file and writes out a summary
# HTML page that consolidates the data.
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

# USAGE:  $0: resultsFilename.csv newHTMLfile.html

## INSTALLATION INSTRUCTIONS
# The HTML::AsSubs perl module is usually not part of a default perl installation.

use vars qw($VERSION);
use strict;

$VERSION = "1.1";

require 5.000;

use FindBin;
use lib "$FindBin::Bin/lib";
use xport::testharness::record;
use xport::testharness::csvfile;
use HTML::AsSubs;
use POSIX qw(strftime);
use Sys::Hostname;

my $Id = $0;

sub cmpTestIDs {
  my ($a, $b) = @_;

  my ($adir, $bdir) = ($a->directory(), $b->directory());

  # use the directory name first
  my $res1 = $adir cmp $bdir;    # assume the simple case

  my ($aprefix, $adigits, $bprefix, $bdigits);  # look for the both test# case
  my ($fAisCandidate, $fBisCandidate);
  $fAisCandidate = ($adir =~ m/^(.*\D)(\d+)$/);
  ($aprefix, $adigits) = ($1, $2);
  $fBisCandidate = ($bdir =~ m/^(.*\D)(\d+)$/);
  ($bprefix, $bdigits) = ($1, $2);
  if ($fAisCandidate and $fBisCandidate ) {
    if ($aprefix eq $bprefix) {
      $res1 = $adigits <=> $bdigits;
    }
  }

  # then the type of test
  if ( $res1 == 0 ) {
    $res1 = $a->type() cmp $b->type();
  }

  return $res1;
}

############ Some interesting constants ############
my @lineBackgroundColors = ( 'white', '#ccccee' );

############ Storage for chunks of marked up content ############
my @statsAsHTML;
my %resultcounts;
my %resulttypecounts;
my %EnumResults;

# determine the results seen
sub enumerateResults {
    foreach my $result (keys %resultcounts) {
      foreach my $type (keys %{$resulttypecounts{$result}}) {
	$EnumResults{$type} = 1;
      }
    }
    
}

# generate the summary statistics table rows for the result values
# in the order they are listed.
sub summaryTableRows {

  my @resultValues = sort keys %EnumResults;

  foreach my $result ( @_ ) {
    my @thisLineAsHTML=();
    my $currcolor = shift @lineBackgroundColors;
    push @lineBackgroundColors, $currcolor;
    push @thisLineAsHTML, (td($result),);
    $resultcounts{$result} = 0 if ( ! defined $resultcounts{$result} );
    push @thisLineAsHTML, (td({'align' => 'right'}, $resultcounts{$result}),);
    foreach my $type (@resultValues) {
      if (defined $resulttypecounts{$result}{$type}) {
	push @thisLineAsHTML, td({'align' => 'right'}, $resulttypecounts{$result}{$type});
      } else {
	push @thisLineAsHTML, td({'align' => 'right'}, '.');
      }
    }
    push @statsAsHTML, HTML::AsSubs::tr({'bgcolor'=>$currcolor}, @thisLineAsHTML);
  }
}

# generate a table for the results seen
sub summaryTableHeaders {

    my $currcolor = shift @lineBackgroundColors;
    push @lineBackgroundColors, $currcolor;


    my @thisLineAsHTML = (
			  th("Result Type"),
			  th("Number of results"));

    my @resultValues = sort keys %EnumResults;

    foreach my $type (@resultValues) {
      push @thisLineAsHTML, th({'align' => 'right'}, $type);
    }

    return @thisLineAsHTML;
}


################# mainline ######################

if (@ARGV != 2) {
  print "$0 requires two filename arguments:\n";
  print "    <logfile> the name of the log file to interpret\n";
  print "    <htmlfile> the name of the file to dump the generated html in\n";
  print "$0 <logfile> <htmlfile>";
  exit 1;
}

my %bigHash;
my @duplicatelist;

slurpFile($ARGV[0], '3', %bigHash, @duplicatelist);

printf "Slurped %d records\n", 0 + (keys %bigHash);
printf "(with  %d duplicates detected)\n", 0 + @duplicatelist;


# Summarize the data so we can render summaries.

my %resultsSummaryTree;

my $result;
foreach $result ( values %bigHash ) {
  $result->directory() =~ m!(^.+/.+/)! ;
  my $branchname = $1;
  if ( ! defined $resultsSummaryTree{$branchname} ) {
    $resultsSummaryTree{$branchname} = [ $result ];
  } else {
    push @{$resultsSummaryTree{$branchname}}, $result;
  }
}


# render the branches/leaves summary
my @branchColumnHeaders = 
  ( HTML::AsSubs::tr( { 'align' => 'left', 'bgcolor' => '#88ff88' },
		      th("API / Function"),
		      th(p("Summary of API test results"),
			 ul( li("BUILD(pass,fail,disabled)"),
			     li("EXEC(Pass,Fail,Disabled)")
			   )
			)
		    )
  );
my %summaryTexts = (
		    "*unknown*type*" => 
		        { '*not*recorded*' => {'color' => 'green', 
					       'bgcolor' => 'pink',
					       'text' => '?' ,
					       'makelink' => 'yes'  
				    },
					},
		    "BUILD" => { '*not*recorded*' => {'color' => 'green', 
					       'bgcolor' => 'pink',
					       'text' => '?' ,
					       'makelink' => 'yes'  
				    },
				 'PASS' => {'color' => 'green', 
					    'bgcolor' => 'green',
					    'text' => 'p' 
					   },
				 'FAIL' => {'color' => 'red', 
					    'bgcolor' => 'red', 
					    'text' => 'f',
					    'makelink' => 'yes'  
					   },
				 'NO_INFO' => {'color' => 'red', 
					    'bgcolor' => 'red', 
					    'text' => 'noinfo ',
					    'makelink' => 'yes'  
					   },
				 'DIR_ERROR' => {'color' => 'red', 
					    'bgcolor' => 'red', 
					    'text' => 'direrr ',
					    'makelink' => 'yes'  
					   },
				 'DISABLED' => {'color' => 'black', 
						'bgcolor' => 'blue', 
						'text' => 'd'},
			       },
		    "EXEC" => { '*not*recorded*' => {'color' => 'green', 
					       'bgcolor' => 'pink',
					       'text' => '?' ,
					       'makelink' => 'yes'  
				    },
				'PASS' => {'color'=> 'green', 
					   'bgcolor' => 'green',
					   'text' => 'P'},
				'FAIL' => {'color'=> 'red', 
					   'bgcolor' => 'red', 
					   'text' => 'F',
					   'makelink' => 'yes' 
					  },
				'NO_INFO' => {'color'=> 'red', 
					   'bgcolor' => 'red', 
					   'text' => 'NOINFO ',
					   'makelink' => 'yes' 
					  },
				 'DIR_ERROR' => {'color' => 'red', 
					    'bgcolor' => 'red', 
					    'text' => 'DIRERR ',
					    'makelink' => 'yes'  
					   },
				'DISABLED' => {'color'=> 'black', 
					       'bgcolor' => 'blue', 
					       'text' => 'D'},
			      }
);
# render the stuff from the file in a few interesting ways

my (@branchesAsHTML, $branchPrefix);
foreach $branchPrefix ( sort keys %resultsSummaryTree ) {
  my (@thisBranchAsHTML, @leafnumbers, $testRecord);
  my $branchRecords = $resultsSummaryTree{$branchPrefix};
  foreach $testRecord ( sort { cmpTestIDs($a, $b) } @$branchRecords ) {
    my $trIdx = $testRecord->id();
    $trIdx =~ tr%/|#%_% ;
    my $result = $testRecord->result();
    my $type = $testRecord->type();
    if ( defined $summaryTexts{$type} && 
	 defined $summaryTexts{$type}{$result} ) {
      my $renderhash = $summaryTexts{$type}{$result};
      if ($renderhash->{'makelink'}) {
	push @thisBranchAsHTML, a({'href' => "#$trIdx"}, $renderhash->{'text'});
      } else {
	push @thisBranchAsHTML, font({'color'=>$renderhash->{'color'}},
				     $renderhash->{'text'});
      }
    } else {
      push @thisBranchAsHTML, font( {'color'=>'red'}, 
				    a({'href' => "#$trIdx"}, 
				      " '$result' " ));
    }
  }
  my $currcolor = shift @lineBackgroundColors;
  push @lineBackgroundColors, $currcolor;
  push @branchesAsHTML, HTML::AsSubs::tr( {'bgcolor'=>$currcolor}, (td($branchPrefix, ": "), td(@thisBranchAsHTML)))
}

# render the comprehensive list.
# and capture the counts per result type
my @compColumnHeaders = 
  ( HTML::AsSubs::tr( { 'align' => 'left', 'bgcolor' => '#88ff88' },
		     th("Result"),
		     th("Directory"),
		     th("Category"),
		     th("Function"),
		     th("Description"),
		     th("Flags"),
		     th("Type"),
		     th("Comment")
		      )
  );
@lineBackgroundColors = ( 'white', '#ccccee' ); # reset the line colors
my @fullTestResTableRows;
foreach $branchPrefix ( sort keys %resultsSummaryTree ) {
  my $testRecord;
  my $branchRecords = $resultsSummaryTree{$branchPrefix};
  foreach $testRecord ( sort { cmpTestIDs($a, $b) } @$branchRecords ) {
    my $currcolor = shift @lineBackgroundColors;
    push @lineBackgroundColors, $currcolor;

    my @thisLineAsHTML;
    my $trIdx = $testRecord->id();
    $trIdx =~ tr%/|#%_% ;
    my $result = $testRecord->result();
    my $type = $testRecord->type();
    my ($typekey, $resultkey) = ($type, $result);

    if ( ! defined $summaryTexts{$type} ) {
      $typekey = '*unknown*type*';
    }
    if ( ! defined $summaryTexts{$typekey}{$result} ) {
      $resultkey = '*not*recorded*';
    }

    my $renderhash = $summaryTexts{$typekey}{$resultkey};
    push @thisLineAsHTML, td({'bgcolor'=>$renderhash->{'bgcolor'}}, $result);
    if (defined $resultcounts{$result}) {
      $resultcounts{$result}++;
    } else {
      $resultcounts{$result} = 1;
    }
    if (defined $resulttypecounts{$result}{$type}) {
      $resulttypecounts{$result}{$type}++;
    } else {
      $resulttypecounts{$result}{$type} = 1;
    }
    push @thisLineAsHTML, ( td($testRecord->directory()),
			   td($testRecord->category()),
			   td($testRecord->function()),
			   td(a({'name' => $trIdx },
				$testRecord->description())),
			   td($testRecord->flags()),
			   td($type),
			 );
    if ( defined $testRecord->comment() ) {
      push @thisLineAsHTML, td( { 'bgcolor'=>$currcolor },
				p($testRecord->comment()));
    }

    # insert the table row(s)
    push @fullTestResTableRows, 
      HTML::AsSubs::tr( {'bgcolor'=>$currcolor }, 
			@thisLineAsHTML);
  }
}

# determine the types of results seen
enumerateResults();

# get the summary table of 'regular' test results out of the way
my @regularResults = ('PASS', 'FAIL');
summaryTableRows( @regularResults );

# NOW WRITE OUT A SEPARATOR ROW B/W PASS/FAIL AND MORE SERIOUS ERROR COUNTS
my $currcolor = shift @lineBackgroundColors;
push @lineBackgroundColors, $currcolor;

push @statsAsHTML, HTML::AsSubs::tr({'bgcolor'=>'#88ff88'},
				    map td('----'), 
				        ('-', '-', keys %EnumResults)
				   );

# NOW WRITE OUT THE MORE SERIOUS ERROR COUNTS
my @otherResults = ();
{
  my %count = ();
  my $e;
  foreach $e (@regularResults, @regularResults, keys %resultcounts) { $count{$e}++ }
  foreach $e (keys %count) {
    if ($count{$e} < 2) {
      push @otherResults, $e;
    }
  }
}
summaryTableRows( sort @otherResults );
if ( 0 + @otherResults) {
  push @statsAsHTML, HTML::AsSubs::tr({'bgcolor'=>'#88ff88'},
				      map td('----'), 
				      ('-', '-', keys %EnumResults)
				     );
}

# Assume this script is always run on the same host the log file was generated on.
# and by the same user that ran the tests. 
my $HOSTNAME = hostname;
my $USERNAME = getlogin || getpwuid($<) || "No info on user.";

# Get changed time on log file
my $LOGTIME = (stat($ARGV[0]))[10];
# Convert time elapsed since epoch to human readable format
# use POSIX qw(strftime) permits this
$LOGTIME = strftime "%a %b %e, %Y @ %H:%M:%S", localtime($LOGTIME); 
 
my $h = 
		       html(
       head( title("Test Results for ", $ARGV[0])),
       body( {'text'=>'black', 'bgcolor'=>'white'}, 
	     h1("Test Results for ", $ARGV[0], " on $HOSTNAME"),
	     h3("by user: ", $USERNAME),
	     h3("Log file dated: ", $LOGTIME),
	     em("This  HTML page generated with $Id."),
	     h2("Summary Numbers Per Type Of Result."),
	     table(  HTML::AsSubs::tr(td("# of unique test results : "), 
				      td({'align' => 'right'}, 
					 0 + (keys %bigHash))),
		     HTML::AsSubs::tr(td("# of duplicated test results : "),
				      td({'align' => 'right'},
					 0  + @duplicatelist)),
		     HTML::AsSubs::tr(td("Distinct API / function categories:"), 
				      td({'align' => 'right'}, 
					 0 + keys %resultsSummaryTree)),
		     HTML::AsSubs::tr( { 'align' => 'left', 
					 'bgcolor' => '#88ff88' },
				       summaryTableHeaders(),
				     ),
		     @statsAsHTML
		  ),
	     h2("Summary: Per API/function area"),
	     table( {'width'=>'"100&#37;"',
		     'border'=>'0',
		     'cellspacing'=>'1',
		     'cellpadding'=>'2'},
		    @branchColumnHeaders,
		    @branchesAsHTML),
	     h2("Full list"),
	     table( {'width'=>'"100&#37;"',
		     'border'=>'0',
		     'cellspacing'=>'1',
		     'cellpadding'=>'2'},
		    @compColumnHeaders,
		    @fullTestResTableRows),
	   ),
      );

open OUTFILE, ">$ARGV[1]" or die;
print OUTFILE $h->as_HTML;
close OUTFILE;

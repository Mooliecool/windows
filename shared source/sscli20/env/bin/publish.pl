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

# FileName: publish.pl
#
# Function: Given a file, publish it to the requested locations.
#
# Usage: publish.pl <logfile> <publishspec>
#    where: <logfile> is the name of the log file to be generated
#           <publishspec> is of the form:
#                  {<filename>=<location>;<location>}{...}
#
#           <filename> is a single filename to publish.
#           <location> is where to publish the file.  Multiple locations
#                          are delimited with semicolon
#
#    or  publish <logfile> -f <specfile>
#    where: <logfile> is the name of the logfile to be generated
#           <specfile> contains one or more <publishspec> entries
#
# Example:
#    publish.pl publish.log {kernel32.lib=\public\sdk\lib\amd64\kernel32.lib;\mypub\_kernel32.lib}
#

use File::Basename;
use File::Copy;
use File::Path;
use File::Spec;

if ($ENV{'PLATFORM_UNIX'} != "1") {
    $Path_Separator = '\\';
} else {
    $Path_Separator = '/';
}

$currenttime = time;
open (CWD, 'cd 2>&1|');
$PublishDir = <CWD>;
close (CWD);
chop $PublishDir;

# strip the logfile name out of the arguments array.

$logfilename = $ARGV[0];
shift;

print "PUBLISH: logging to $logfilename\n";

if ($ARGV[0] =~ /^[\/-][fF]$/) {
    shift;
    $indirname = shift;
    if (@ARGV || !$indirname) {
        die "Build_Status PUBLISH() : error p1000: Invalid syntax - Expected:  publish -f FILE\n";
    }

    print "PUBLISH: getting input from $indirname\n";
    open INDIR, $indirname   or die "Build_Status PUBLISH(): error p1005: Could not open: $indirname\n";

    @ARGV=<INDIR>;
    close INDIR;
} elsif ($ARGV[0] =~ /^[\/-][iI]$/) {
    shift;
    print "PUBLISH: getting input from STDIN\n";
    @ARGV=<STDIN>;
}

for (@ARGV) {

   s/\s//g;                # Remove spaces, tabs, newlines, etc

   $NextSpec = $_;

   print "PUBLISH: NextSpec = $_\n";

   while ($NextSpec) {

      $SaveSpec = $NextSpec;

      $Spec1 = "";
      $PublishSpec = "";

      # Filter out the current publish spec
      ($PreCurly,$Spec1) = split (/{/, $NextSpec,2);

      # See if there's another one
      ($PublishSpec,$NextSpec) = split (/}/, $Spec1,2);

      # Break out the filename
      ($SourceSpec,$LocationSpec) = split (/=/, $PublishSpec,2);

      @Files = split (/\:\:/, $SourceSpec);
      $FileName = $Files[0];

      # Create the location list
      @Location = split ((/\;/), $LocationSpec);

      die "PUBLISH(80) : error p1003: Bad input: $SaveSpec\n"  unless($PublishSpec && $FileName && $#Location >= 0);

      # See if the source exists.
      if (!stat($FileName)) {
         print "Build_Status PUBLISH() : error p1001: $PublishDir - $FileName does not exist\n";
      } else {

         ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;

         $TimeDate = sprintf "%04d/%02d/%02d-%02d:%02d:%02d", 1900+$year, 1+$mon, $mday, $hour, $min, $sec;

         # Run pcopy for every location listed.

         for (@Location) {
            print "PUBLISH: Copying $FileName to $_\n";
            mkpath(dirname($_));
            if (copy($SourceSpec, $_)) {
               $ReturnCode = 0;
            } else {
               $ReturnCode = $!;
            }
            if ($ReturnCode == 0) {

               $CopiedFile=$_;

               $PUBLISH_LOG = $ENV{'_NTTREE'};
               $PUBLISH_LOG = File::Spec->catfile($PUBLISH_LOG, $logfilename);

               $FileCount = 0;

               for (@Files) {
                   # RC == 0 means success.
                   $ReturnCode = -1;
                   $LoopCount = 0;

                  $AltCopiedFile = $CopiedFile;
                  $SlashLocation = $RIndex2 = rindex($_, $Path_Separator);
                  if (($FileCount != 0) && ($SlashLocation > -1)) {
                     substr($AltCopiedFile, rindex($AltCopiedFile, $Path_Separator)) = substr($_, rindex($_, $Path_Separator));
                  }
                  while ($ReturnCode) {
                     system ("echo $AltCopiedFile $PublishDir $_ $currenttime >> $PUBLISH_LOG");
                     $ReturnCode = $?;
                     $LoopCount = $LoopCount + 1;
                     # Retry a max of 100 times to log the change.
                     if ($LoopCount == 100) {
                        print "Build_Status PUBLISH() : warning p1002: Unable to log \"$AltCopiedFile $PublishDir $_\" to $PUBLISH_LOG\n";
                        $ReturnCode = 0;
                     }
                  }

                  print "PUBLISHLOG: $PublishDir, $_, $AltCopiedFile, Updated, $TimeDate\n";
                  $FileCount = $FileCount + 1;
               }
            } else {
               # Problem copying (bad source, missing dest, out of mem, etc).

               print "Build_Status PUBLISH() : error p1004: ERROR($ReturnCode) copying $FileName to $_\n";
            }
         }
      }
   }
}

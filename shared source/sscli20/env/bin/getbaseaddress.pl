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

# getbaseaddress: echo the base address of a file as a parameter to
# csc.exe, based on its coffbase entry or a default value 
# if the entry doesn't exist.

$Name = $ARGV[0];
$Ext = $ARGV[1];
$CoffBase = $ARGV[2];
$Addr = $ARGV[3];
$Output = $ARGV[4];
$Baseopt = $ARGV[5];

$BaseAddress = "";

open(VERSION1,$CoffBase) || print "Unable to open version file ".$CoffBase && exit 1;
while(<VERSION1>)
{
   if ( /\b$Name\.$Ext\s+(0x\S*).*/i )
   {
      $BaseAddress = $1;
      last;
   }
}
close(VERSION1);

if ($BaseAddress eq "")
{
    #$Name.$Ext not found, so look for $Name
    open(VERSION1,$CoffBase) || print "Unable to open version file ".$CoffBase && exit 1;
    while(<VERSION1>)
    {
       if ( /\b$Name\s+(0x\S*).*/i )
       {
          $BaseAddress = $1;
          last;
       }
    }
    close(VERSION1);
}

if ($BaseAddress eq "")
{
    $BaseAddress = $Addr;
}

if ($Baseopt eq "")
{
    $Baseopt = "/baseaddress:";
}

if ($Output eq "")
{
    print $Baseopt.$BaseAddress;
}
else
{
    $Output = ">>".$Output;
    open(OUTPUT, $Output) || print "Unable to open output file" && exit 1;
    print OUTPUT "$Baseopt$BaseAddress\n";
    close(OUTPUT);
}

exit 0;






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

# perl genver.pl <verfile> [<namespace>]
# 
# This script extracts the version info from a slm version file and emits an
# assembly version command line option for the C# compiler.
# 
#  e.g. 
#     perl genver.pl D:\vs\src\common\inc\version.h Microsoft.JScript
#   The following is written to stdout.
#     
#     namespace Microsoft.JScript {
#     class BuildVersionInfo {
#         public const int MajorVersion=7;
#         public const int MinorVersion=0;
#         public const int Revision=0;
#         public const int Build=8903;
#     }
#     }
#     
#   The <verfile> is a standard SLM version file and should have info like the following.
#     #define rmj     7
#     #define rmm     0
#     #define rup     8903
#     #define szVerName   ""
#     #define szVerUser   "ROKYU01-DEV"

$verfile = null;
$namespace = null;
$rmj = 0;
$rmm = 0;
$rup = 0;

if (@ARGV < 1) {
    die "genver.pl : () USAGE: perl genver.pl <verfile> [<namespace>]\n";
}

$verfile = $ARGV[0];

if (@ARGV == 2) {
    $namespace = $ARGV[1];
}

open (VERFILE, $verfile) or die "genver.pl : () Unable to open $verfile\n;";

while (<VERFILE>) {
    if (/#define rmj\b(.*)/) {$rmj = $1;}
    if (/#define rmm\b(.*)/) {$rmm = $1;}
    if (/#define rup\b(.*)/) {$rup = $1;}
}

if ($namespace ne null) {
    print "namespace ".$namespace." {\n";
}

print "class BuildVersionInfo {\n";
print "    public const int MajorVersion=".$rmj.";\n";
print "    public const int MinorVersion=".$rmm.";\n";
print "    public const int Revision=0;\n";
print "    public const int Build=".$rup.";\n";
print "}\n";

if ($namespace ne null) {
    print "}\n";
}


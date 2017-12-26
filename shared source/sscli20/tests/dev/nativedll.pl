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
# nativedll.pl
#
# Custom driver for nativedll:
#     Build nativedll.dll

my $retval = 0;

chdir("nativedll");
$retval = system("build -cM") >> 8;
chdir("..");

exit($retval);

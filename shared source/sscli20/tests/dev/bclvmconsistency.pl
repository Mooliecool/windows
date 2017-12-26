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
# bclvmconsistency.pl
#
# Custom driver for bclvmconsistency check:
#     Sets COMPlus_ConsistencyCheck=1 and executes dummy test

$ENV{COMPlus_ConsistencyCheck}=1;

my $retval = system (@ARGV) >> 8;
exit($retval);

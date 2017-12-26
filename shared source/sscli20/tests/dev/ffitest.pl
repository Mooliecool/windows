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
# ffitest.pl
#
# Custom driver for ffitest check:
#     Build ffitest.exe and execute it

my $retval = 0;

chdir("ffitest");
$retval = system("build -cM") >> 8;
chdir("..");

if (!$retval) {
    $retval = system("./ffi_test") >> 8;
}

exit($retval);

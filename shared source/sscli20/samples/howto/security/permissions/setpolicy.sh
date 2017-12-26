#!/bin/sh

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

# Install myperm.dll to the GAC
gacutil -i myperm.dll

# *** use caspol -reset to reset to default settings

# Set local machine code to grant Everything, but not FullTrust
echo clix caspol -cg 1.1 Everything
clix ${_NTTREE}/caspol -q -cg 1.1 Everything
echo

# At this point strongnamed assemblies must have the AllowPartiallyTrustedCallers attribute
# to be able to call other strongnamed assemblies.
# You will want to reset this.

# Add the myperm.dll perm assembly to the full trust list,
# so that we can then use -ap to add this permission.
echo clix caspol -q -af myperm.dll
clix ${_NTTREE}/caspol -q -af myperm.dll
echo

# Add the myperm perm set
echo clix caspol -q -ap myperm.xml
clix ${_NTTREE}/caspol -q -ap myperm.xml
echo

# Grant myperm to the authorized sample
echo clix caspol -q -ag 1.1 -strong -file authorized.exe -noname -noversion myperm -name "allowed"
clix ${_NTTREE}/caspol -q -ag 1.1 -strong -file authorized.exe -noname -noversion myperm -name "allowed"
echo

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


# *** use caspol -reset to reset to default settings
echo clix caspol -q -reset
clix ${_NTTREE}/caspol -q -reset
echo

# Uninstall myperm from GAC
echo gacutil -u myperm
gacutil -u myperm
echo


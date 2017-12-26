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
# WARNING: This script is a thin wrapper around env.core.pl. All the 
# intelligence for setting up the build environment is encapsulated 
# in env.core.pl. If you want to modify the environment setup, please 
# change env.core.pl directly.
#
# For a description of options, run either this script or env.core.pl
# with the "--help" flag.

if (! -f "./env.core.pl") then
  echo "env.csh must be run from the root of the SSCLI directory."
  exit 1
endif

# Warn that the arguments to source command may not be passed to
# this script if the shell is csh.
set SH=`basename $shell`
if ($SH == "csh") then
  echo "WARNING: Additional arguments to 'source' are ignored by csh."
endif


eval `perl ./env.core.pl --shell=csh $*`
if ($status == "0") then
    exit 0
else
    exit 1
endif

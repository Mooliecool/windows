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

ExecEnvCore()
{
    if [ ! -f "./env.core.pl" ]; then
        echo "env.sh must be run from the root of the SSCLI directory."
        return 1
    fi

    envcore=`perl ./env.core.pl $*`
    if [ $? -eq 0 ]; then
        eval $envcore 2>&1
        return 0
    else
        return 1
    fi
}

ExecEnvCore $*
#

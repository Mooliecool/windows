#! /bin/sh

#
# 
#  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
# 
#  The use and distribution terms for this software are contained in the file
#  named license.txt, which can be found in the root of this distribution.
#  By using this software in any fashion, you are agreeing to be bound by the
#  terms of this license.
# 
#  You must not remove this notice, or any other, from this software.
# 
#

if [ "$1x" = "x" ] 
then
    execname=`basename $0`
    cat <<EOF
$execname: print some statistics about a test harness configuration file to stdout.
USAGE:
    $execname: [testconfig.dat]
EOF
    exit 1;
fi

if [ ! -e $1 ]
then
    echo "$0: file '$1' not found"
    exit 1;
fi

echo "Summary statistics for '$1'"

echo "Number of lines in the file"
cat "$1" | wc -l 

echo "Lines without test implementations"
cut -d, -f1 "$1" | xargs ls -d 2>&1 >/dev/null | wc -l

echo "Lines marked as disabled (0) or enabled (1)"
cut -d, -f2 "$1" | tr  -d " \t" | sort | uniq -c
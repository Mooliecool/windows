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

MAJORCOMP=fusion
MINORCOMP=utils

TARGETNAME=fusionutils
TARGETTYPE=LIBRARY
TARGETPATH=$(CLR_LIB_DEST)

SOURCES= \
    dbglog.cpp \
    disk.cpp \
    helpers.cpp \
    fstream.cpp \
    xmlns.cpp \
    mstream.cpp

SOURCES_USED=sources.inc

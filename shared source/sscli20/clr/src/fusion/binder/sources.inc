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
MINORCOMP=binder

TARGETNAME=fusionbind
TARGETTYPE=LIBRARY
TARGETPATH=$(CLR_LIB_DEST)

SOURCES= \
    actasm.cpp \
    appctx.cpp \
    asm.cpp    \
    naming.cpp \
    parse.cpp  \
    policy.cpp \
    pcycache.cpp \
    nodefact.cpp \
    bindhelpers.cpp  \
    fusconfig.cpp \
    simplesink.cpp \
    hostasm.cpp \
    hostmodimprt.cpp \
    fxretarget.cpp \
    asmreflist.cpp \
    asmidmgr.cpp \
    hostpcymgr.cpp \
    bindresult.cpp


SOURCES_USED=sources.inc

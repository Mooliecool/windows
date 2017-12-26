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

INCLUDES        = $(INCLUDES);..\..\inc

USER_C_FLAGS    = $(USER_C_FLAGS) -DUNICODE -D_UNICODE -DNO_CRT

# Take out the CRT.
NO_NTDLL			=1
!undef USE_LIBCMT

TARGETPATH      =$(CLR_LIB_DEST)
TARGETTYPE      =LIBRARY

PRECOMPILED_INCLUDE     = ..\stdafx.h
PRECOMPILED_CXX         = 1
PRECOMPILED_SOURCEFILE  = ..\stdafx.cpp
PRECOMPILED_OBJ         = stdafx_compiler.obj

    
SOURCES=                \
    ..\AssemblyMD.cpp      \
    ..\ClassFactory.cpp    \
    ..\CustAttr.cpp        \
    ..\Disp.cpp            \
    ..\emit.cpp            \
    ..\helper.cpp          \
    ..\import.cpp          \
    ..\importhelper.cpp    \
    ..\MdPerf.cpp          \
    ..\MDUtil.cpp          \
    ..\NewMerger.cpp       \
    ..\RegMeta.cpp         \
    ..\TokenMapper.cpp     \
    ..\FilterManager.cpp   \
    ..\MDValidator.cpp

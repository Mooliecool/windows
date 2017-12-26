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

!include $(CLRBASE)\dac.mk

INCLUDES = $(INCLUDES);..;..\..\..\vm;..\..\..\vm\$(_TGTCPU);..\..\inc;..\..\inc\$(_TGTCPU);..\..\ee;..\..\..\inc;..\..\..\inc\$(O);$(VCTOOLS_INC_PATH)
C_DEFINES = $(C_DEFINES) -DUNICODE -D_UNICODE -DJIT_OR_NATIVE_SUPPORTED=1 $(USER_SPECIFIC_C_DEFINES) -DFEATURE_NO_HOST -DDAC_PERF_CHANGES=1

TARGETNAME = dac_$(_CORFLAVOR)
TARGETPATH = $(CLR_LIB_DEST)
TARGETTYPE = LIBRARY

PRECOMPILED_INCLUDE = ..\stdafx.h
PRECOMPILED_CXX = 1
PRECOMPILED_SOURCEFILE = ..\stdafx.cpp

SOURCES = \
    ..\daccess.cpp\
    ..\dacfn.cpp\
    ..\enummem.cpp\
    ..\fntableaccess.cpp\
    ..\inspect.cpp\
    ..\reimpl.cpp\
    ..\request.cpp\
    ..\request_svr.cpp\
    ..\stack.cpp\
    ..\task.cpp

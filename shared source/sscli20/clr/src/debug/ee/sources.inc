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

INCLUDES        =$(INCLUDES);..;$(CLRBASE)\src\vm;$(CLRBASE)\src\vm\$(_TGTCPU);..\..\inc;..\..\inc\$(_TGTCPU);$(VCTOOLS_INC_PATH)

USER_C_FLAGS    = $(USER_C_FLAGS) -DUNICODE -D_UNICODE -DFEATURE_NO_HOST

TARGETNAME      =cordbee_$(_CORFLAVOR)
TARGETPATH      =$(CLR_LIB_DEST)
TARGETTYPE      =LIBRARY

### CCover, for some reason, has a problem instrumenting
### source in the precompiled headers for this directory.
PRECOMPILED_INCLUDE     = ..\stdafx.h
PRECOMPILED_CXX         = 1
PRECOMPILED_SOURCEFILE  = ..\stdafx.cpp

# This is just a sources-include file.
# The real sources file is in some subdirectory and will include this one.

# Debugger code is compiled under both normal + DAC
# Some files are BOTH, and some are only non-dac.
SOURCES_BOTH=  \
    ..\Debugger.cpp        \
    ..\DebuggerModule.cpp  \
    ..\functioninfo.cpp

# These source files are not included in DAC builds.
SOURCES_NODAC=             \
    ..\funceval.cpp        \
    ..\RCThread.cpp        \
    ..\Canary.cpp          \
    ..\Thread.cpp          \
    ..\controller.cpp      \
    ..\shared.cpp          \
    ..\frameinfo.cpp


ROTOR_X86_SOURCES=..\rotor_x86\dbghelpers.asm \
                  ..\rotor_x86\x86walker.cpp \
                  ..\rotor_x86\primitives.cpp \
                  ..\rotor_x86\DebuggerRegDisplayHelper.cpp

PPC_SOURCES=..\ppc\dbghelpers.s \
            ..\ppc\ppcwalker.cpp \
            ..\ppc\primitives.cpp \
            ..\ppc\DebuggerRegDisplayHelper.cpp


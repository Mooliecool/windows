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

# Take out the CRT.
NO_NTDLL			=1
!undef USE_LIBCMT

!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"

# WarningControl.h disables too many warnings.  classlibnative/float will not use it.
COMPILER_WARNINGS	=/W4

# Get the FPU intrinics for sin, cos, etc.
!if $(FREEBUILD)
MSC_OPTIMIZATION=/Ox
!endif

!endif


INCLUDES        = ..;..\..\inc;$(INCLUDES);$(CLRBASE)\src\vm;$(CLRBASE)\src\vm\$(_TGTCPU);$(VCTOOLS_INC_PATH)

TARGETNAME      =comfloat_$(_CORFLAVOR)
TARGETPATH      =$(CLR_LIB_DEST)
TARGETTYPE      =LIBRARY

C_DEFINES       = $(C_DEFINES) -DUNICODE -D_UNICODE -DCRTDLL

USE_STL         = 1

SOURCES=        \
    ..\COMFloat.cpp
   

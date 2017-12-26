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
# WarningControl.h disables too many warnings.  classlibnative/nls folder will not use it.
COMPILER_WARNINGS	=
!endif

INCLUDES = $(INCLUDES);..;..\..\inc;$(CLRBASE)\src\vm;$(CLRBASE)\src\vm\$(_TGTCPU);$(VCTOOLS_INC_PATH)

TARGETNAME = comnls_$(_CORFLAVOR)
TARGETPATH = $(CLR_LIB_DEST)
TARGETTYPE = LIBRARY

USER_C_FLAGS = $(USER_C_FLAGS) -DUNICODE -D_UNICODE -DCRTDLL

USE_STL = 1


SOURCES =                 \
    ..\COMNlsInfo.cpp        \
    ..\NLSTable.cpp          \
    ..\GlobalizationAssembly.cpp   \
    ..\CasingTable.cpp       \
    ..\SortingTable.cpp \
    ..\SortingTableFile.cpp \
    ..\UnicodeCatTable.cpp \
    ..\jamo.cpp \
    ..\NativeTextInfo.cpp \
    ..\EncodingData.cpp \
    ..\COMNlsInfoData.cpp

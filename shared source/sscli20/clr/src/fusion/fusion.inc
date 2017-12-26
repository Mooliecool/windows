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

MSC_WARNING_LEVEL = /W3
NO_NTDLL=1
USE_MSVCRT=1

C_DEFINES = $(C_DEFINES) -DFUSION_QUALIFYASSEMBLY_ENABLED -DFUSION_PARTIAL_BIND_DEBUG -DFUSION_RETAIL_LOGGING -DFUSION_REDEFINE_ASSERT=1 -DMSOERT_NO_PROTSTOR=1



#
# Set private include paths
#

NO_BROWSER_FILE=1

FUSION_PATH = $(CLRBASE)\src\fusion

FUSION_ALL_INC_PATH = $(FUSION_PATH)\inc

FUSION_WRAPPER_INC_PATH=$(FUSION_PATH)\wrappers

FUSION_INC_PATH=$(FUSION_ALL_INC_PATH)
FUSION_BUILD_TYPE_PREFIX=

INCLUDES        = .; \
    $(FUSION_PATH)\inc; \
    $(FUSION_PATH)\idl\$(O); \
    $(INCLUDES); \
    $(CLRBASE)\src\inc; \
    $(CLRBASE)\src\inc\$(O); \
    $(CLRBASE)\src\vm; \
    $(CLRBASE)\src\vm\$(TARGET_DIRECTORY); \
    $(WINDOWS_INC_PATH);\
    $(BASE_INC_PATH);\
    $(SDK_INC_PATH);\
    $(NET_INC_PATH)

!ifndef FUSION_MBCS
C_DEFINES = $(C_DEFINES) -DUNICODE -D_UNICODE
!endif

#USE_FUSION_WRAPPERS=1
!ifdef USE_FUSION_WRAPPERS
C_DEFINES = $(C_DEFINES) -D_WIN32_IE=0x0500 -DUSE_FUSWRAPPERS
INCLUDES        = \
    $(CLRBASE)\src\fusion\wrappers; \
    $(INCLUDES)
!endif


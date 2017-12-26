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

# Need to wait on a link library building in pass 1.
SYNCHRONIZE_DRAIN = 1

!include $(CLRBASE)\dac.mk

C_DEFINES = $(C_DEFINES) -DUNICODE -D_UNICODE -DJIT_OR_NATIVE_SUPPORTED=1 $(USER_SPECIFIC_C_DEFINES) -DFEATURE_NO_HOST

TARGETNAME = cordac$(_CORFLAVOR)
TARGETPATH = $(_OBJ_DIR)
TARGETTYPE = DYNLINK

UMTYPE = windows
DLLDEF = ..\mscordac.def
DLLENTRY                                = _DllMainCRTStartup

NO_BROWSER_FILE=1

# If this is a full debug build turn on VS_FF_SPECIALBUILD in the
# version VS_FIXEDFILEINFO so that a debugger can distinguish it
# from a regular checked build.
!if "$(_BUILDTYPE)" == "dbg"
C_DEFINES = $(C_DEFINES) -DSPECIAL_BUILD
!endif
        
SOURCES = \
    ..\native.rc \
    ..\mscordac.cpp


!if "$(DELAYLOAD)" != ""
DELAYLOAD = $(DELAYLOAD);$(DELAYLOADLIST)
!else
DELAYLOAD = $(DELAYLOADLIST)
!endif

USE_DELAYIMP_LIB = 1

LINKLIBS = $(LINKLIBS) \
    $(CLR_LIB_PATH)\dac_$(_CORFLAVOR).lib

WIN32DLL_LIBS = \
    $(CLR_LIB_PATH)\utilcode_dac.lib \
    $(WIN32DLL_LIBS)

TARGETLIBS = $(TARGETLIBS) \
    $(CLR_LIB_PATH)\cee_$(_CORFLAVOR)_dac.lib \
    $(CLR_LIB_PATH)\cordbee_$(_CORFLAVOR)_dac.lib \
    $(CLR_LIB_PATH)\mdcompiler_dac.lib \
    $(CLR_LIB_PATH)\mdruntime_dac.lib \
    $(CLR_LIB_PATH)\mdruntimerw_dac.lib \
    $(CLR_LIB_PATH)\ipcmanager_dac.lib \


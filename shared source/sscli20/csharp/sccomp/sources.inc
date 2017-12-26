# ==++==
# 
#   
#    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
#   
#    The use and distribution terms for this software are contained in the file
#    named license.txt, which can be found in the root of this distribution.
#    By using this software in any fashion, you are agreeing to be bound by the
#    terms of this license.
#   
#    You must not remove this notice, or any other, from this software.
#   
# 
# ==--==

BUILD_USING_VC7PDBS=1

PROJECT_ROOT=$(BASEDIR)\csharp
!include $(DEVDIV_TOOLS)\sources.inc

UMTYPE=windows
USE_STATIC_ATL=1

USE_NATIVE_EH=ASYNC

# Get the XML includes first so we pick-up the msxml2.h
USER_INCLUDES=$(MFC_INCLUDES);\
	..;\
	..\..\inc;\
	$(_NTTREE)\inc\$(TARGET_DIRECTORY);\
	$(VSCOMMON_INC_PATH);\
	$(SDK_INC_PATH);$(DEBUGGER_INC_PATH);\
	$(VB_INC_PATH);\
	$(ENV_INC_PATH);\
	$(VCTOOLS_INC_PATH);\

USER_INCLUDES=$(USER_INCLUDES);\
	$(BASEDIR)\csharp\alink\inc;\
	$(ROTOR_DIR)\palrt\idl

C_DEFINES=$(C_DEFINES) -DX86 -D_MBCS -DTESTCMD -D_WINDOWS -DSTRICT -DNT -D_USRDLL -DSCCOMP_EXPORTS


USE_MSVCRT=1

!if !$(FREEBUILD)
LINKER_NOREF=1
!endif

!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"

# Enable 64-bit warnings
USER_C_FLAGS=$(USER_C_FLAGS) -Wp64
#USER_C_FLAGS=$(USER_C_FLAGS) /EP

!if !$(FREEBUILD)
# add runtime checks in debug build
# USER_C_FLAGS=$(USER_C_FLAGS) -RTCs -RTCc
!else
# The compiler is very performance intensive. Optimize for PURE SPEED!
MSC_OPTIMIZATION=/O2
!endif

# The compiler doesn't do any fp-calculations except what comes from source, which must be precise
!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET_VERSION)" >= "80"
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /fp:precise
!else
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /Op
!endif

!endif

!if "$(BUILD_BROWSE)"!="no"
BROWSER_INFO=1
NO_BROWSER_FILE=1
!endif

# These files are listed in alphabetical order.  Keep them that way!
SOURCES= \
    ..\alloc.cpp \
    ..\bitset.cpp \
    ..\clsdrec.cpp \
    ..\compiler.cpp \
    ..\complus.cpp \
    ..\controller.cpp \
    ..\cscmgd.cpp \
    ..\emitter.cpp \
    ..\error.cpp \
    ..\factory.cpp \
    ..\fncbind.cpp \
    ..\nullable.cpp \
    ..\operators.cpp \
    ..\fncpbind.cpp \
    ..\ilgen.cpp \
    ..\import.cpp \
    ..\inputset.cpp \
    ..\inttree.cpp \
    ..\lexer.cpp \
    ..\map.cpp \
    ..\memlook.cpp \
    ..\metaattr.cpp \
    ..\metahelp.cpp \
    ..\namemgr.cpp \
    ..\options.cpp \
    ..\parser.cpp \
    ..\pefile.cpp \
    ..\sccomp.cpp \
    ..\SimpleParser.cpp \
    ..\SourceParser.cpp \
    ..\srcdata.cpp \
    ..\srcmod.cpp \
    ..\symmgr.cpp \
    ..\TextParser.cpp \
    ..\typebind.cpp \


PRECOMPILED_INCLUDE=..\stdafx.h
PRECOMPILED_SOURCEFILE=..\stdafx.cpp
PRECOMPILED_PCH=stdafx.pch
PRECOMPILED_OBJ=stdafx.obj
PRECOMPILED_CXX=1

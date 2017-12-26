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

# Need to wait for all LINKLIBS to be built in pass 1.
SYNCHRONIZE_DRAIN = 1

!undef USE_LIBCMT

INCLUDES                                = $(INCLUDES);.;..;..\..\inc;..\..\inc\$(O);..\..\..\inc;..\..\..\inc\$(O);..\..\..\fusion\inc

INCLUDES                                = $(INCLUDES);..\shim;$(CLRBASE)\src\xmlparser
NO_NTDLL                                = 1

TARGETNAME                              = mscor$(_CORFLAVOR)

DLLENTRY                                = _DllMainCRTStartup

TARGETPATH                              = $(_OBJ_DIR)
TARGETTYPE                              = DYNLINK
UMTYPE                                  = windows

USER_C_FLAGS                            = $(USER_C_FLAGS) -DUNICODE -D_UNICODE

PRECOMPILED_INCLUDE                     = ..\stdafx.h
PRECOMPILED_CXX                         = 1
PRECOMPILED_SOURCEFILE                  = ..\stdafx.cpp

# Enable POGO for mscorwks.dll
POGO_OPTIMIZED                          = 1

# If this is a full debug build turn on VS_FF_SPECIALBUILD in the
# version VS_FIXEDFILEINFO so that a debugger can distinguish it
# from a regular checked build.
!if "$(_BUILDTYPE)" == "dbg"
C_DEFINES = $(C_DEFINES) -DSPECIAL_BUILD
!endif

DLLDEF                          = $(O)\mscor$(_CORFLAVOR)_ntdef.def

LINKLIBS = \
    $(CLR_LIB_PATH)\cee_$(_CORFLAVOR)1.lib \
    $(CLR_LIB_PATH)\cee_$(_CORFLAVOR)2.lib \
    $(CLR_LIB_PATH)\cee_$(_CORFLAVOR)3.lib \
    $(CLR_LIB_PATH)\cee_$(_CORFLAVOR)4.lib \
    $(CLR_LIB_PATH)\utilcode.lib                 \
    $(CLR_LIB_PATH)\fusionasmc.lib               \
    $(CLR_LIB_PATH)\fusionbind.lib               \
    $(CLR_LIB_PATH)\fusiondl.lib                 \
    $(CLR_LIB_PATH)\fusionmparse.lib             \
    $(CLR_LIB_PATH)\fusionutils.lib              \
    $(CLR_LIB_PATH)\xmlparser.lib                \
!IF "$(PROFILING_SUPPORTED_BUILD)"=="1"
    $(CLR_LIB_PATH)\perfcounters.lib \
!ENDIF

USE_DELAYIMP_LIB = 1

TARGETLIBS =    $(TARGETLIBS)                                        \
                $(CLR_LIB_PATH)\ipcmanager.lib                       \
                $(CLR_LIB_PATH)\mdcompiler.lib                       \
                $(CLR_LIB_PATH)\mdruntime.lib                        \
                $(CLR_LIB_PATH)\mdruntimerw.lib                      \
                $(CLR_LIB_PATH)\comfloat_$(_CORFLAVOR).lib           \
                $(CLR_LIB_PATH)\comnls_$(_CORFLAVOR).lib             \
                $(CLR_LIB_PATH)\ceefgen.lib                          \
!IF "$(DEBUGGING_SUPPORTED_BUILD)"=="1"
                $(CLR_LIB_PATH)\cordbee_$(_CORFLAVOR).lib            \
!ENDIF

!IF "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"
LINKER_FLAGS=$(LINKER_FLAGS) -MAP:$(O)\mscorwks.map
!ENDIF

# LINKER_FLAGS=$(LINKER_FLAGS) /linkrepro:C:\VBL\Lab21S\bugs\repro_issue



SOURCES=                        \
        ..\MSCoree.cpp          \
        ..\Native.rc            \


SOURCES_PAL =                      \
        ..\xmlreader.cpp           \
        ..\xmlstream.cpp

ROTOR_X86_SOURCES=$(SOURCES_PAL)
PPC_SOURCES=$(SOURCES_PAL)


!if "$(LINKONLY)" == "1" && "$(FEATURE_PAL)" != "1"
MISCFILES= ..\mscoree20_shared_neutral.vrg ..\mscoree.vrg ..\mscoreeVersioned.vrg ..\mscoreedonoteverremove.vrg ..\chk.reg ..\fstchk.reg ..\shim.reg  ..\dw20.msi ..\dw20_ia64.msi ..\dw20_amd64.msi
!endif

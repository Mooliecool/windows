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

_PROJECT_=ndp

!CMDSWITCHES -I


ORCAS=0

ORCAS=0

ASM_DEFINES=$(ASM_DEFINES) -DORCAS=$(ORCAS)
C_DEFINES=$(C_DEFINES) -DORCAS=$(ORCAS)
LIBC_DEFINES=$(LIBC_DEFINES) -DORCAS=$(ORCAS)
MCPP_DEFINES=$(MCPP_DEFINES) -DORCAS=$(ORCAS)
RC_DEFINES=$(RC_DEFINES) -DORCAS=$(ORCAS)



!ifndef CRT_INC_PATH
CRT_INC_PATH    = $(BASEDIR)\not_present\CRT_INC_PATH
!endif
!ifndef ATL_INC_PATH
ATL_INC_PATH    = $(BASEDIR)\not_present\ATL_INC_PATH
!endif

NDP_COMMON_INCLUDES=$(NDPDIR)\inc\version


INCLUDES=$(INCLUDES);$(NDP_COMMON_INCLUDES)

#############
# MIDL Options - put output files into obj directory 
# default in makefile.def is . which mixes generated and checked-in sources
PASS0_HEADERDIR=$(O)
PASS0_SOURCEDIR=$(O)

#
# Always use MSVCRT instead of NT C runtimes
#
USE_MSVCRT=1

NT_UP=0
NO_X86_CPU_OPT=1
NOT_LEAN_AND_MEAN=1

#
# filelist.xml and related files
#

BINPLACE_ROOT=$(_NTTREE)

# coffbase
!if "$(_BUILDTYPE)" == "chk"
NDP_COFFBASE_EXT=c
!else if "$(_BUILDTYPE)" == "dbg" 
NDP_COFFBASE_EXT=d
!else
NDP_COFFBASE_EXT=
!endif


#moving 4198 ignore from tools\makefile.def to here,
#since we want to do this for pretty much everything except ndp
LINKER_FLAGS=$(LINKER_FLAGS) -IGNORE:4198

NDPFX_GENERIC_APP_CONFIG=$(PROJECT_ROOT)\inc\version\app.config

# if the user wants to publish ndp binaries on this machine turn on BINPUBLISH
!if "$(BINARY_PUBLISH_PROJECTS:ndp=)" != "$(BINARY_PUBLISH_PROJECTS)"
BINPUBLISH=1
!endif 

# Publishing shorthands (no internal ones yet, I don't think we have any
# internal targets).
NDP_PASS0_PUBLISH_PUBLIC=\
    {$(O)\$(TARGETNAME).h=$(SDK_INC_PATH)\$(TARGETNAME).h} \
    {$(TARGETNAME).h=$(SDK_INC_PATH)\$(TARGETNAME).h}
    
NDP_PASS1_PUBLISH_PUBLIC=\
    {$(O)\$(TARGETNAME).lib=$(SDK_LIB_PATH)\$(TARGETNAME).lib}

NDP_REF_PUBLISH_PUBLIC=\
    {$(O)\$(TARGETNAME).dll=$(SDK_REF_PATH)\$(TARGETNAME).dll}

NDP_TLB_PUBLISH_PUBLIC=\
    {$(O)\$(TARGETNAME).tlb=$(SDK_LIB_PATH)\$(TARGETNAME).tlb}


# Define some common macros for Rotor support (so they can switch come common
# command line tools on the unix build).

!ifndef COPY_NAME
COPY_NAME=copy
!endif

!ifndef XCOPY
XCOPY=xcopy /r
!endif

!ifndef MD
MD=mkdir
!endif

!ifndef DELETER
DELETER=erase /q
!endif

# Developer division common tools.


!if "$(ROTOR_TOOLSET)" == "MSVC" && "$(MSVCDIR)" == ""  
# We are building with LKGVC toolset
DACTABLEGEN=cmd /c $(NTMAKEENV)\clrmanagedtoolwrapper.cmd $(LKGVC_DIR)\tools\devdiv\$(PROCESSOR_ARCHITECTURE)\dactablegen.exe
!else
DACTABLEGEN=clix $(_NTTREE)\int_tools\dactablegen.exe
!endif



#NO_AUTOMATIC_MANIFEST=1

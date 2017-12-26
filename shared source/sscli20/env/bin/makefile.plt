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
#
# If not defined, specify where to get incs and libs.
#

!IFNDEF _NTROOT
_NTROOT=\nt
!ENDIF

!IFNDEF BASEDIR
BASEDIR=$(_NTDRIVE)$(_NTROOT)
!ENDIF

CLRBASE=$(NDPDIR)\clr

# A couple of overrides

!ifndef _NT_TARGET_VERSION # Default to current OS version
_NT_TARGET_VERSION = 0x502
!endif

!ifndef _NT_TOOLS_VERSION # Default to current VC version
_NT_TOOLS_VERSION = 0x700
!endif

!ifndef SDK_PATH
SDK_PATH = $(LKGVC_DIR)\public\sdk
!endif

!ifndef SDK_INC_PATH
SDK_INC_PATH = $(SDK_PATH)\inc
!endif

!ifndef SDK_INC16_PATH
SDK_INC16_PATH = $(SDK_PATH)\inc16
!endif

!ifndef SDK_LIB_DEST
SDK_LIB_DEST = $(SDK_PATH)\lib
!endif

!ifndef SDK_LIB_PATH
SDK_LIB_PATH = $(SDK_LIB_DEST)\*
!endif

!ifndef SDK_LIB16_PATH
SDK_LIB16_PATH=$(SDK_PATH)\lib16
!endif

!ifndef DDK_PATH
DDK_PATH = $(LKGVC_DIR)\public\ddk
!endif

!ifndef DDK_INC_PATH
DDK_INC_PATH = $(DDK_PATH)\inc
!endif

!ifndef DDK_LIB_DEST
DDK_LIB_DEST = $(DDK_PATH)\lib
!endif

!ifndef DDK_LIB_PATH
DDK_LIB_PATH = $(DDK_LIB_DEST)\*
!endif

!ifndef IFSKIT_PATH
IFSKIT_PATH = $(LKGVC_DIR)\public\ifskit
!endif

!ifndef IFSKIT_INC_PATH
IFSKIT_INC_PATH = $(IFSKIT_PATH)\inc
!endif

!ifndef IFSKIT_LIB_DEST
IFSKIT_LIB_DEST = $(IFSKIT_PATH)\lib
!endif

!ifndef IFSKIT_LIB_PATH
IFSKIT_LIB_PATH = $(IFSKIT_LIB_DEST)\*
!endif

!ifndef HALKIT_PATH
HALKIT_PATH = $(LKGVC_DIR)\public\halkit
!endif

!ifndef HALKIT_INC_PATH
HALKIT_INC_PATH = $(HALKIT_PATH)\inc
!endif

!ifndef HALKIT_LIB_DEST
HALKIT_LIB_DEST = $(HALKIT_PATH)\lib
!endif

!ifndef HALKIT_LIB_PATH
HALKIT_LIB_PATH = $(HALKIT_LIB_DEST)\*
!endif

!ifndef PROCESSOR_PATH
PROCESSOR_PATH = $(LKGVC_DIR)\public\processorkit
!endif

!ifndef PROCESSOR_INC_PATH
PROCESSOR_INC_PATH = $(PROCESSOR_PATH)\inc
!endif

!ifndef PROCESSOR_LIB_DEST
PROCESSOR_LIB_DEST = $(PROCESSOR_PATH)\lib
!endif

!ifndef PROCESSOR_LIB_PATH
PROCESSOR_LIB_PATH = $(PROCESSOR_LIB_DEST)\*
!endif

!ifndef WDM_INC_PATH
WDM_INC_PATH = $(DDK_INC_PATH)\wdm
!endif

!ifndef CRT_INC_PATH
CRT_INC_PATH = $(SDK_INC_PATH)\crt
!endif

!ifndef IOSTREAMS_INC_PATH
IOSTREAMS_INC_PATH = $(SDK_INC_PATH)\crt\iostreams
!endif

!ifndef STL6_INC_PATH
STL6_INC_PATH = $(SDK_INC_PATH)\crt\stl60
!endif

!ifndef CRT_LIB_PATH
CRT_LIB_PATH = $(SDK_LIB_PATH)
!endif


OAK_INC_PATH = $(BASEDIR)\not_present\OAK_INC_PATH
OAK_BIN_PATH = $(BASEDIR)\not_present\OAK_BIN_PATH
WPP_CONFIG_PATH = $(BASEDIR)\not_present\WPP_CONFIG_PATH
PUBLIC_INTERNAL_PATH = $(BASEDIR)\not_present\PUBLIC_INTERNAL_PATH


COPYRIGHT_STRING = Copyright (c) Microsoft Corporation. All rights reserved.

!ifndef PUBLISH_CMD
PUBLISH_CMD=perl $(ROTOR_DIR)\env\bin\publish.pl publish.log
!endif

!ifndef BINDROP_CMD
! ifdef NOLINK
# Only drop binaries if we are linking in this pass.
BINDROP_CMD=rem No bindrop pass during link
!else
BINDROP_CMD=perl $(ROTOR_DIR)\env\bin\publish.pl $(_PROJECT_)_publish.log
!endif
!endif

!ifndef BUILD_PASS
# Old build.exe.  Guess the correct pass.
!ifdef PASS0ONLY
BUILD_PASS=PASS0
!elseif defined(LINKONLY)
BUILD_PASS=PASS2
!elseif defined(NOLINK) && !defined(PASS0ONLY)
BUILD_PASS=PASS1
!else
# catch all - someone used build -z or build -2
BUILD_PASS=PASSALL
!endif
!endif

#
# Set the flag which indicates whether we should be publishing binaries
# to 0 by default.  the project.mk file is responsible for parsing
# BINARY_PUBLISH_PROJECTS to determine if its value should be changed.
#

BINPUBLISH=0

#
# Find and include the project configuration file.
#

!if exists(.\project.mk)
_PROJECT_MK_PATH=.
!elseif exists(..\project.mk)
_PROJECT_MK_PATH=..
!elseif exists(..\..\project.mk)
_PROJECT_MK_PATH=..\..
!elseif exists(..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..
!elseif exists(..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..
!elseif exists(..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..
!elseif exists(..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..\..\..\..\..
!endif

!if "$(_PROJECT_MK_PATH)" != ""
#!message "Including $(_PROJECT_MK_PATH)\project.mk"
!include $(_PROJECT_MK_PATH)\project.mk
!if exists($(_PROJECT_MK_PATH)\myproject.mk)
!include $(_PROJECT_MK_PATH)\myproject.mk
!endif
!else
#!message "Unable to find project.mk. Update makefile.plt or create project.mk."
!endif

!IFDEF _PROJECT_

PROJECT_ROOT=$(BASEDIR)\$(_PROJECT_)
PROJECT_PUBLIC_PATH=$(PUBLIC_INTERNAL_PATH)\$(_PROJECT_)
PROJECT_INC_PATH=$(PROJECT_PUBLIC_PATH)\inc
PRIVATE_INC_PATH=$(PROJECT_INC_PATH)
PROJECT_LIB_DEST=$(PROJECT_PUBLIC_PATH)\lib
PROJECT_LIB_PATH=$(PROJECT_LIB_DEST)\$(TARGET_DIRECTORY)
PROJECT_INF_PATH=$(PROJECT_PUBLIC_PATH)\inf

!ELSE
#!message "ERROR: _PROJECT_ is not defined. Should be defined in project.mk."
!ENDIF

#
# If not defined, define the build message banner.
#

!IFNDEF BUILDMSG
BUILDMSG=
!ENDIF

!if ("$(NTDEBUG)" == "") || ("$(NTDEBUG)" == "ntsdnodbg")
FREEBUILD=1
! ifndef BUILD_TYPE
BUILD_TYPE=fre
! endif
!else
FREEBUILD=0
! ifndef BUILD_TYPE
BUILD_TYPE=chk
! endif
!endif


# Allow alternate object directories.

!if !defined(BUILD_ALT_DIR) && defined(CHECKED_ALT_DIR) && !$(FREEBUILD)
BUILD_ALT_DIR=d
!endif

_OBJ_DIR = obj$(BUILD_PHASE)$(BUILD_ALT_DIR)
_OBJ_DIR_PHASE0 = obj0$(BUILD_ALT_DIR)
_OBJ_DIR_PHASE1 = obj1$(BUILD_ALT_DIR)
_OBJ_DIR_PHASE2 = obj2$(BUILD_ALT_DIR)
_OBJ_DIR_PHASEDEF = obj$(BUILD_ALT_DIR)


#
# Determine which target is being built (i386 or ia64) and define
# the appropriate target variables.
#

!IFNDEF ROTOR_X86
ROTOR_X86=0
!ENDIF

386=0

AMD64=0

IA64=0

!IFNDEF PPC
PPC=0
!ENDIF

!IFNDEF MPPC
MPPC=0
!ENDIF

#
# Define the target platform specific information.
#

!ifdef PLATFORM_UNIX
SHARED_LIB_PRE=lib
!else
SHARED_LIB_PRE=
!endif

!if $(ROTOR_X86)

ASM_SUFFIX=asm
ASM_INCLUDE_SUFFIX=inc

TARGET_BRACES=

!ifdef SUBSTITUTE_386_CC
TARGET_CPP=$(SUBSTITUTE_386_CC)
!else
TARGET_CPP=cl
!endif

TARGET_DEFINES=-Di386 -D_X86_
TARGET_DIRECTORY=rotor_x86
TARGET_NTTREE=$(_NT386TREE)

!ifdef PLATFORM_UNIX
SHARED_LIB_EXT=so
!else
SHARED_LIB_EXT=dll
!endif

VCCOM_SUPPORTED=1
WIN64=0

!elseif $(386)

ASM_SUFFIX=asm
ASM_INCLUDE_SUFFIX=inc

TARGET_BRACES=

!ifdef SUBSTITUTE_386_CC
TARGET_CPP=$(SUBSTITUTE_386_CC)
!else
TARGET_CPP=cl
!endif

TARGET_DEFINES=-Di386 -D_X86_
TARGET_DIRECTORY=i386
TLB_SWITCHES=/tlb
!ifndef _NTTREE
! ifdef _NTX86TREE
_NTTREE=$(_NTX86TREE)
! elseif defined(_NT386TREE)

_NTTREE=$(_NT386TREE)
! endif
!endif

VCCOM_SUPPORTED=1
SCP_SUPPORTED=1
WIN64=0
PLATFORM_MFC_VER=0x0600
MACHINE_TYPE=ix86
ANSI_ANNOTATION=0
LTCG_DRIVER=0
LTCG_DRIVER_LIBRARY=0
LTCG_DYNLINK=0
LTCG_EXPORT_DRIVER=0
LTCG_GDI_DRIVER=0
LTCG_HAL=0
LTCG_LIBRARY=0
LTCG_MINIPORT=0
LTCG_PROGRAM=0
LTCG_PROGLIB=0
LTCG_UMAPPL_NOLIB=0
TARGET_CSC=csc

!elseif $(PPC)

ASM_SUFFIX=s
ASM_INCLUDE_SUFFIX=h

SHARED_LIB_EXT  =dylib

TARGET_DEFINES=-DPPC -D_PPC_
TARGET_DIRECTORY=ppc

WIN64=0

!elseif $(MPPC)

ASM_SUFFIX=s
ASM_INCLUDE_SUFFIX=h

TARGET_BRACES=-B
TARGET_CPP=cl
TARGET_DEFINES=-DMPPC -D_MPPC_
TARGET_DIRECTORY=mppc
TLB_SWITCHES=/tlb
WIN64=0
PLATFORM_MFC_VER=0x0421
MACHINE_TYPE=mppc

!ifndef _NTTREE
! ifdef _NTMPPCTREE
_NTTREE=$(_NTMPPCTREE)
! endif
!endif
ANSI_ANNOTATION=1
LTCG_DRIVER=0
LTCG_DRIVER_LIBRARY=0
LTCG_DYNLINK=0
LTCG_EXPORT_DRIVER=0
LTCG_GDI_DRIVER=0
LTCG_HAL=0
LTCG_LIBRARY=0
LTCG_MINIPORT=0
LTCG_PROGRAM=0
LTCG_PROGLIB=0
LTCG_UMAPPL_NOLIB=0

!else
!error Unknown target platform type.
!endif

!ifndef _NTBINDIR
_NTBINDIR=$(_NTTREE)
!endif

#
#  These flags don't depend on i386 etc. however have to be in this file.
#
#  MIDL_OPTIMIZATION is the optimization flag set for the current NT.
#  MIDL_OPTIMIZATION_NO_ROBUST is the current optimization without robust.
#
!ifdef MIDL_PROTOCOL
MIDL_PROTOCOL_DEFAULT=-protocol $(MIDL_PROTOCOL)
!else
# MIDL_PROTOCOL_DEFAULT=-protocol all
!endif

!IFNDEF MIDL_OPTIMIZATION
MIDL_OPTIMIZATION=-Oicf -robust -error all $(MIDL_PROTOCOL_DEFAULT)
!ENDIF
MIDL_OPTIMIZATION_NO_ROBUST=-Oicf -error all -no_robust
MIDL_OPTIMIZATION_NT4=-Oicf -error all -no_robust
MIDL_OPTIMIZATION_NT5=-Oicf -robust -error all $(MIDL_PROTOCOL_DEFAULT)
!ifdef SUBSTITUTE_MIDL_CC
MIDL_CPP=$(SUBSTITUTE_MIDL_CC)
!else
MIDL_CPP=$(TARGET_CPP)
!endif
MIDL_FLAGS=$(TARGET_DEFINES) -D_WCHAR_T_DEFINED

#
# If not defined, simply set to default
#

!IFNDEF HOST_TARGETCPU
HOST_TARGETCPU=$(TARGET_DIRECTORY)
!ENDIF

MIDL_ALWAYS_GENERATE_STUBS=0

CLEANSE_PUBLISHED_HDR=copy

PATH_TOOLS16=$(LKGVC_DIR)\tools\tools16

# If a build path is defined, use it.

!ifdef BUILD_PATH
PATH=$(BUILD_PATH)
!endif

BINPLACE_PLACEFILE_DIR=$(NTMAKEENV)

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

!include $(NDPDIR)\ndpmk.inc
CLRBASE=$(NDPDIR)\clr

# Build all internal static libraries into a common location so the rest of the
# CLR tree can remain as location independent as possible.
CLR_LIB_DEST=$(CLRBASE)\lib\$(_BUILDTYPE)
CLR_LIB_PATH=$(CLR_LIB_DEST)\$(TARGET_DIRECTORY)

USE_PDB_TO_COMPILE  =1
USE_MSVCRT          =1
USE_INCREMENTAL_COMPILING  =1

CONFORMING_FORSCOPE=1

!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"
COMPILER_WARNINGS   =/FIWarningControl.h
!endif

#                                                                                              


!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"
FULLCXXEH           = /EHs- /EHa  /EHc-
SYNCCXXEH           = /EHs  /EHa- /EHc-
SEHONLY             = /EHs- /EHa- /EHc-
!ELSE
FULLCXXEH           =
SYNCCXXEH           =
SEHONLY             =
!endif

#                                                                                 
EXCEPTION_HANDLING  = $(FULLCXXEH)
# EXCEPTION_HANDLING  = $(SYNCCXXEH)
# EXCEPTION_HANDLING = $(SEHONLY)


CLR_BUILD_ERROR=*** CLR Build Error: 
CLR_BUILD_TRACE=*** CLR Trace: 


###############################################################################
# The CLR will be shipped using /GS support for security purposes.
#
!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"
USER_C_FLAGS=$(USER_C_FLAGS) /GS
MCPP_FLAGS=$(MCPP_FLAGS) /GS 
!endif


###############################################################################
#
# The CLR allows you to turn off features of the build like edit & continue.
# These macros are currently based on the platform you are building.
#
!if "$(_BUILDARCH)" == "x86"
_TGTCPU=i386
DEBUGGING_SUPPORTED_BUILD=1
PROFILING_SUPPORTED_BUILD=1
FEATURE_COMINTEROP=1
FEATURE_PREJIT=1
C_DEFINES=$(C_DEFINES) -DDEBUGGING_SUPPORTED -DEnC_SUPPORTED -DPROFILING_SUPPORTED -DFEATURE_COMINTEROP -DFEATURE_PREJIT -DMDA_SUPPORTED
CSC_DEFINES=$(CSC_DEFINES) /D:FEATURE_COMINTEROP

!elseif "$(_BUILDARCH)" == "ia64"
_TGTCPU=ia64
DEBUGGING_SUPPORTED_BUILD=1
PROFILING_SUPPORTED_BUILD=1
FEATURE_COMINTEROP=1
FEATURE_PREJIT=1
C_DEFINES=$(C_DEFINES) -DDEBUGGING_SUPPORTED -DEnC_SUPPORTED -DPROFILING_SUPPORTED -DFEATURE_COMINTEROP -DFEATURE_PREJIT -DMDA_SUPPORTED
CSC_DEFINES=$(CSC_DEFINES) /D:FEATURE_COMINTEROP

!elseif  ("$(_BUILDARCH)" == "amd64") || ("$(_BUILDARCH)" == "AMD64")
_TGTCPU=amd64
DEBUGGING_SUPPORTED_BUILD=1
PROFILING_SUPPORTED_BUILD=1
FEATURE_COMINTEROP=1
FEATURE_PREJIT=1
C_DEFINES=$(C_DEFINES) -DDEBUGGING_SUPPORTED -DEnC_SUPPORTED -DPROFILING_SUPPORTED -DFEATURE_COMINTEROP -DFEATURE_PREJIT -DMDA_SUPPORTED
CSC_DEFINES=$(CSC_DEFINES) /D:FEATURE_COMINTEROP

!elseif  "$(_BUILDARCH)" == "rotor_x86"
_TGTCPU=i386
DEBUGGING_SUPPORTED_BUILD=1
PROFILING_SUPPORTED_BUILD=1
C_DEFINES=$(C_DEFINES) -DDEBUGGING_SUPPORTED -DCONTRACTS_DISABLED -DPROFILING_SUPPORTED

!elseif  "$(_BUILDARCH)" == "ppc"
_TGTCPU=ppc
DEBUGGING_SUPPORTED_BUILD=1
PROFILING_SUPPORTED_BUILD=1
C_DEFINES=$(C_DEFINES) -DDEBUGGING_SUPPORTED -DCONTRACTS_DISABLED -DPROFILING_SUPPORTED

!else
!message $(CLR_BUILD_ERROR) unsupported platform in ndp\clr\project.mk!
!endif


###############################################################################
#
# these are meant to take the place of items that were set in corenv.bat.
#
!if ("$(NTDEBUG)" == "") || ("$(NTDEBUG)" == "ntsdnodbg")
FREEBUILD=1
!else
FREEBUILD=0
!endif

#
# For builds that will do asserts, set the right defines.
#
!if !$(FREEBUILD)

C_DEFINES           = $(C_DEFINES) -DNTMAKEENV -D_DEBUG
DEBUG_CRTS          = 1

# for slow debug, do all checking.  very slooooow.
!if ("$(_BUILDOPT)" == "no opt")
C_DEFINES           = $(C_DEFINES) -DWRITE_BARRIER_CHECK=1 
USE_COMPILER_OPTIMIZATIONS=0
!endif

#
# Builds with no asserts get these macros.
#

!else

C_DEFINES           = $(C_DEFINES) -DNTMAKEENV -DNDEBUG -DPERF_TRACKING

!if "$(FEATURE_PAL)" != "1"
C_DEFINES           = $(C_DEFINES)
!endif

!endif

#
# 'full opt' is for checked and free builds.  anything else get's no
# optimizations.  debug macros are orthoganal.
#
!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"

MSC_OPTIMIZATION    = $(MSC_OPTIMIZATION) /Oi
!if ("$(_BUILDOPT)" == "full opt")
MSC_OPTIMIZATION    = $(MSC_OPTIMIZATION) /O1
!else
MSC_OPTIMIZATION    = $(MSC_OPTIMIZATION) /Od 
#                                                                
!endif

!endif


# A few public header files need to know that they're being used from a CLR
# build environment.
C_DEFINES           = $(C_DEFINES) -D_BLD_CLR

#                                                                                      
USER_C_FLAGS    = $(EXCEPTION_HANDLING) $(USER_C_FLAGS)

#
# Adding /we4640 to make this warning an error...
#
# warning C4640: 'X' : construction of local static object is not thread-safe
#

!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"
USER_C_FLAGS    = $(EXCEPTION_HANDLING) $(USER_C_FLAGS) /we4640
!endif


!if "$(_BUILDARCH)" == "x86"
C_DEFINES       = $(C_DEFINES) /D_WIN32_WINNT=0x0400
LINK_LIB_IGNORE = 4013
!else
C_DEFINES       = $(C_DEFINES) -DWIN32_LEAN_AND_MEAN=1
LINK_LIB_IGNORE = 4197,4013
!endif

#                                                            
INCLUDES        = $(CLRBASE)\src\inc\$(O_PHASEDEF);$(INCLUDES)
INCLUDES        = $(CLRBASE)\src\inc\$(O_PHASE2);$(INCLUDES)
INCLUDES        = $(CLRBASE)\src\inc\$(O_PHASE1);$(INCLUDES)
INCLUDES        = $(CLRBASE)\src\inc\$(O_PHASE0);$(INCLUDES)
INCLUDES        = $(CLRBASE)\src\inc;$(INCLUDES)
INCLUDES        = $(INCLUDES);$(VCTOOLS_INC_PATH)


###############################################################################
# Handle prefast build
###############################################################################
!if ("$(CLR_PREFAST_BUILD)" == "TRUE")
!message $(INCLUDES) Doing CLR prefast build!
!endif


###########################################
## LTCG and POGO related settings for CLR
###########################################
!if "$(_BUILDARCH)" == "ia64" || "$(_BUILDARCH)" == "amd64"
! if "$(_LTCG)" == "1"
LTCG = 1
! endif

! if "$(_POGO_INS)" == "1"
POGO_INS = 1
! endif

! if "$(_POGO_OPT)" == "1"
POGO_OPT = 1
! endif
!endif

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

###############################################################################
# Compiler options
###############################################################################

## Due to name space conflicts; when the x86jit fe is 
## built we need to turn off the pre-compiled header. 
PRECOMPILED_INCLUDE    =
PRECOMPILED_CXX	       =

INCLUDES            = $(CLRBASE)\src\vm;..;..\inc;..\$(_BUILDARCH)\inc;$(INCLUDES);$(O)

NTTARGETFILE0=          \
   $(NTTARGETFILE0)     \
   $(O)\globals_defines.h


##MSC_OPTIMIZATION = /O2

###############################################################################
## To switch to a "C" compilation comment out the following lines
##  NOTE: p2.h is being compiled as C++, but is only being attached
##        to files that end in *.c
##
USECXX_FLAG            = /TP
C_DEFINES              = $(C_DEFINES) -DCC_SKCPP
##
###############################################################################

###############################################################################
## Allow the usage of os functions such as VirtualAlloc and VirtualFree
##
C_DEFINES              = $(C_DEFINES) -DFEATURE_NO_HOST
##
###############################################################################

###############################################################################
## Enable new reader
##
C_DEFINES              = $(C_DEFINES) -DCC_NEWREADER
##
###############################################################################

###############################################################################
## The flags to do ICECAP
##
#USER_C_FLAGS     = $(USER_C_FLAGS) /O1 /Zi
#LINKER_FLAGS    =$(LINKER_FLAGS) -nodefaultlib /dbgimplib /debug /debugtype:cv,fixup /opt:ref
##
###############################################################################

###############################################################################
# Default build env turns on OPT:REF and OPT:ICF which makes debugging crappy
# since it removes all the non-referenced debugging functions
!if !$(FREEBUILD)
LINKER_NOREF=1
!endif
#
###############################################################################

###############################################################################
## The flags for a doing a CodeMarkers instrumented build
##
USER_C_FLAGS     = $(USER_C_FLAGS) -DCC_CMTIMING
##
###############################################################################

###############################################################################
## The flags for a free build
##
!if $(FREEBUILD)
MSC_OPTIMIZATION = -O2
!endif
##
###############################################################################

###############################################################################
# Ensure we get -D_MT defined in our build
###############################################################################


###############################################################################
# The build system normally sucks in a couple of standard libs which
# we dont want. So force it to only pick up the libs which the JIT needs.
###############################################################################


NO_NTDLL            = 1

###############################################################################
# POGO - link in pogort.lib for retail builds
###############################################################################


###############################################################################
# Source files
###############################################################################

JIT64_BASE_SOURCES=    \
    ..\native.rc       \
    ..\newverify.cpp   \
    ..\reader.cpp      \
    ..\vtypeinfo.cpp


DLLENTRY        =_DllMainCRTStartup

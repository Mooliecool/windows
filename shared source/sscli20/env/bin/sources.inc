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

VCS_ROOT=$(BASEDIR)\vscommon
VSCOMMON_INC_PATH=$(VCS_ROOT)\inc;$(VCS_ROOT)\unilib;$(VCS_ROOT)\util

INCLUDES=$(INCLUDES);$(LD_LIBRARY_PATH);$(VSCOMMON_INC_PATH);$(ROTOR_DIR)\prebuilt\idl\obj$(BUILD_ALT_DIR)\$(_BUILDARCH);$(NDPDIR)\clr\src\inc

C_DEFINES=$(C_DEFINES) -DUNIAPI_NOATL


!if $(FREEBUILD)
C_DEFINES = $(C_DEFINES) -DNDEBUG
!else
C_DEFINES = $(C_DEFINES) -DDEBUG
!endif

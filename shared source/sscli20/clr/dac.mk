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
# Global dac build directives.
# 

# Turn PROFILING_SUPPORTED into PROFILING_SUPPORTED_DATA if it's there.
C_DEFINES = $(C_DEFINES:-DPROFILING_SUPPORTED=-DPROFILING_SUPPORTED_DATA)
ASM_DEFINES = $(ASM_DEFINES:-DPROFILING_SUPPORTED=-DPROFILING_SUPPORTED_DATA)

C_DEFINES = $(C_DEFINES) -DDACCESS_COMPILE
ASM_DEFINES = $(ASM_DEFINES) -DDACCESS_COMPILE

EXCEPTION_HANDLING = $(FULLCXXEH)

USE_NOLIBS=1
!undef USE_MSVCRT
USE_LIBCMT=1

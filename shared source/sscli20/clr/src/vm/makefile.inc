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

STD_DEFINES = $(STD_DEFINES) $(C_DEFINES)

!if "$(ASM_SUFFIX)" == "asm"


$(O)\asmconstants.inc: ..\$(_TGTCPU)\asmconstants.h
    $(PERL) ../h2inc.pl ..\$(_TGTCPU)\asmconstants.h > $(O)\asmconstants.tmp
    cl /EP $(C_DEFINES) $(ASM_DEFINES) $(TARGET_DEFINES) $(O)\asmconstants.tmp > $(O)\asmconstants.inc
!else
# Nothing to do
$(O)\asmconstants.inc:
!endif


..\gceewks.cpp ..\gceesvr.cpp : ..\gcee.cpp
..\AMD64\StubLinkerAMD64.cpp: ..\i386\StubLinkerX86.cpp

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


prep_tlbs:


$(O)\opcode.refops: opcode.def genrops.pl
    $(PERL) genrops.pl < opcode.def > $(O)\opcode.refops

!if "$(PLATFORM_UNIX)" == "1"
COPY = cp -p -f
!else
COPY = copy
!endif


doscript:



copyheaders:
!if "$(PASS0ONLY)"=="1"
    $(COPY) $(ROTOR_DIR)\prebuilt\idl\obj$(BUILD_ALT_DIR)\$(_BUILDARCH)\*.h $(PASS0_HEADERDIR)
    $(COPY) $(ROTOR_DIR)\prebuilt\idl\obj$(BUILD_ALT_DIR)\$(_BUILDARCH)\*_i.c $(PASS0_SOURCEDIR)
!endif

grabcorsym:
    $(COPY) $(SDK_INC_PATH)\corsym.h $(O)
    $(COPY) $(SDK_INC_PATH)\corsym_i.c $(O)

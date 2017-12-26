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

OBJECTS = $(OBJECTS) $O\asmparse.obj 

!if "$(PLATFORM_UNIX)" == "1"
COPY = cp -p -f
!else
COPY = copy
!endif

$O\asmparse.obj: asmparse.h $O\asmparse.cpp

$O\asmparse.cpp: $(ROTOR_DIR)\prebuilt\yacc\obj$(BUILD_ALT_DIR)\$(_BUILDARCH)\asmparse.c
	$(COPY) $(ROTOR_DIR)\prebuilt\yacc\obj$(BUILD_ALT_DIR)\$(_BUILDARCH)\asmparse.c $O\asmparse.cpp

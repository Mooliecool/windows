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

VERDIR = ..\..\..\inc\version
INCDIR = ..\inc
DACDIR = ..\debug\daccess
BINDIR = $(_NTTREE)
HOSTMACH = $(MACHINE_TYPE:ix86=x86)
# No cross-platform daccess support at the moment.
TARGMACH = $(HOSTMACH)

!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"
$(O)/daccess.i: ..\inc\daccess.h ..\inc\jithelpers.h ..\inc\vptr_list.h
    $(CXX_COMPILER_NAME) @<<$(CL_RSP) /E $(CL_OPT_FC) -DDACCESS_COMPILE ..\inc\daccess.h > $@
$(CXX_COMPILER_FLAGS: =
)
<<NOKEEP
!else
$(O)/daccess.i: ..\inc\daccess.h ..\inc\jithelpers.h ..\inc\vptr_list.h
    $(C_PREPROCESSOR_NAME) $(C_PREPROCESSOR_FLAGS) -DDACCESS_COMPILE ..\inc\daccess.h > $@
!endif


CORDACSRC=$(BINDIR)\$(SHARED_LIB_PRE)cordacwks.$(SHARED_LIB_EXT)

!if "$(ROTOR_TOOLSET)" == "MSVC"

$O\wks.bin: $(O)\daccess.i $(CLRBASE)\src\dlls\mscoree\wks\$(O_PHASE1)\mscorwks.map
    $(DACTABLEGEN) /dac:$(O)\daccess.i \
        /map:$(CLRBASE)\src\dlls\mscoree\wks\$(O_PHASE1)\mscorwks.map /bin:$O\wks.bin

mscordacwks_upd: $(CORDACSRC) $O\wks.bin
    $(COPY_NAME) $O\wks.bin $(BINDIR)\mscordacwks.bin
    $(COPY_NAME) $(CORDACSRC) $(BINDIR)\$(SHARED_LIB_PRE)mscordacwks.$(SHARED_LIB_EXT)

!else

mscordacwks_upd: $(CORDACSRC)
    $(COPY_NAME) $(CORDACSRC) $(BINDIR)\$(SHARED_LIB_PRE)mscordacwks.$(SHARED_LIB_EXT)

!endif

RunTheScript: mscordacwks_upd

clean:


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

$(O)\BuildVersionInfo.cs:
    $(PERL) $(DEVDIV_TOOLS)\genver.pl $(VCS_ROOT)\inc\version.h Microsoft.JScript >$(O)\BuildVersionInfo.cs


$(_NTTREE)\bin\$(TARGET_DIRECTORY)\Microsoft.JScript.reg : Microsoft.JScript.reg
	copy /y $** $@

$(_NTTREE)\Microsoft.JScript_versioned_neutral.vrg : Microsoft.JScript_versioned_neutral.vrg
	copy /y $** $@

$(_NTTREE)\Microsoft.JScript_tlb_versioned_neutral.vrg : Microsoft.JScript_tlb_versioned_neutral.vrg
	copy /y $** $@

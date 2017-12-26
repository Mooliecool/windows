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

!if "$(PASS2)" == "1"

!if "$(BIGENDIAN)" == "1"
NLPSUFFIX=be.nlp
!else
NLPSUFFIX=.nlp
!endif
NLPDIR=system\globalization\tables


$(O)\appdomainsetup.cs: system\appdomainsetup.cs
        $(PERL) -w $(DEVDIV_TOOLS)\finddefines.pl $(CLRBASE)\src\inc\$(O)\fusion.h > $(O)\fdefines.h
        $(PERL) -w $(DEVDIV_TOOLS)\finddefines.pl $(CLRBASE)\src\inc\$(O)\fusionpriv.h > $(O)\fprivdefines.h
        $(PERL) -w $(DEVDIV_TOOLS)\finddefines.pl $(CLRBASE)\src\inc\fusionsetup.h > $(O)\fusionsetup.h
!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"
        cl /EP /C /I. /FI $(O)\fprivdefines.h /FI $(O)\fdefines.h /FI $(O)\fusionsetup.h $(C_DEFINES) system\appdomainsetup.cs > $(O)\appdomainsetup.cs
!else
        $(CC_NAME) -x c++ -E -C -P -nostdinc -I. -include $(O)\fprivdefines.h -include $(O)\fdefines.h -include $(O)\fusionsetup.h $(C_DEFINES) system\appdomainsetup.cs > $(O)\appdomainsetup.cs
        perl -p -ics_ -e "s/^# /#line /;" $(O)/appdomainsetup.cs
!endif

$(O)\compatibilityflags.cs:
!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"
        cl /EP /I$(CLRBASE)\src\inc /C system\compatibilityflags.cs > $(O)\compatibilityflags.cs
!else
        $(CC_NAME) -x c++ -E -C -P -nostdinc -I$(CLRBASE)\src\inc system\compatibilityflags.cs > $(O)\compatibilityflags.cs
!endif

pre_mscorlib_steps:
   $(PERL) $(NTMAKEENV)$(DIR_SEP)setup.pl DelaySign
   $(COPY_NAME) $(NLPDIR)\big5$(NLPSUFFIX)      $(O)\big5.nlp
   $(COPY_NAME) $(NLPDIR)\bopomofo$(NLPSUFFIX)  $(O)\bopomofo.nlp
   $(COPY_NAME) $(NLPDIR)\charinfo$(NLPSUFFIX)  $(O)\charinfo.nlp
   $(COPY_NAME) $(NLPDIR)\codepages$(NLPSUFFIX) $(O)\codepages.nlp
   $(COPY_NAME) $(NLPDIR)\culture$(NLPSUFFIX)   $(O)\culture.nlp
   $(COPY_NAME) $(NLPDIR)\ksc$(NLPSUFFIX)       $(O)\ksc.nlp
   $(COPY_NAME) $(NLPDIR)\l_except$(NLPSUFFIX)  $(O)\l_except.nlp
   $(COPY_NAME) $(NLPDIR)\l_intl$(NLPSUFFIX)    $(O)\l_intl.nlp
   $(COPY_NAME) $(NLPDIR)\normnfc$(NLPSUFFIX)   $(O)\normnfc.nlp
   $(COPY_NAME) $(NLPDIR)\normnfd$(NLPSUFFIX)   $(O)\normnfd.nlp
   $(COPY_NAME) $(NLPDIR)\normnfkc$(NLPSUFFIX)  $(O)\normnfkc.nlp
   $(COPY_NAME) $(NLPDIR)\normnfkd$(NLPSUFFIX)  $(O)\normnfkd.nlp
   $(COPY_NAME) $(NLPDIR)\normidna$(NLPSUFFIX)  $(O)\normidna.nlp
   $(COPY_NAME) $(NLPDIR)\prc$(NLPSUFFIX)       $(O)\prc.nlp
   $(COPY_NAME) $(NLPDIR)\prcp$(NLPSUFFIX)      $(O)\prcp.nlp
   $(COPY_NAME) $(NLPDIR)\sortkey$(NLPSUFFIX)   $(O)\sortkey.nlp
   $(COPY_NAME) $(NLPDIR)\sorttbls$(NLPSUFFIX)  $(O)\sorttbls.nlp
   $(COPY_NAME) $(NLPDIR)\xjis$(NLPSUFFIX)      $(O)\xjis.nlp


$(CLR_TARGET): pre_mscorlib_steps

post_mscorlib_steps: $(CLR_TARGET)
!if "$(PLATFORM_UNIX)" != "1"
    set __SECURITY_BOOTSTRAP_DB=
    if exist SecurityDB.raw ( \
        attrib -r SecurityDB.*.vss & \
        copy SecurityDB.idx SecurityDB.idx.vss & \
        copy SecurityDB.db SecurityDB.db.vss & \
        echo BUILD : warning SEC0001: Security bootstrap database altered, check in new versions of SecurityDB.*.vss & \
        build -cC3)
!endif
!if "$(USE_US_ENGLISH_SATELLITE)" == "1"
	echo [assembly:AssemblyCulture("en-US")]  // Produce a US English Satellite > $(O)\AssemblyCulture.cs
	copy $(O)\AssemblyAttributes.cs + /a $(O)\AssemblyCulture.cs $(O)\SatAssemblyAttributes.cs
	$(CSC_NAME) /noconfig /nostdlib+ /target:library /out:$(O)\mscorlib.resources.dll $(O)\SatAssemblyAttributes.cs /resource:$(O)\mscorlib.resources,mscorlib.en-US.resources /r:$(O)\mscorlib.dll
!endif


copy_stuff:

!else

pre_mscorlib_steps post_mscorlib_steps copy_stuff:

!endif


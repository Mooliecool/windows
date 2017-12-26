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

# ------------------------------------------
# Clear out variables to make sure we don't conflict
# These variables may already be set depending if and how VS was installed
# ------------------------------------------
Set("LIB", "");
Set("INCLUDE", "");

Set("_TGTOS", "NT32");
Set("_TGTCPUTYPE", "x86");
Set("_TGTCPU", "i386");

# ------------------------------------------
# Clear up the PERF environment variables
# ------------------------------------------
Set("PAL_LIB_PERF", "");

Set("NTMAKEENV", catdir(Get("ROTOR_DIR"), "env", "bin"));
if (!Get("tmp")) {
    Set("tmp", "\\");
}
Set("COPYCMD", "/Y");

Set ("NTDEBUGTYPE", "vc6");
Set ("COFFBASE_TXT_FILE", catfile (Get("ROTOR_DIR"), "env", "*", "coffbase.txt"));

if (my $msvcdir = Get("MSVCDir")) {
    # These are the INCLUDE paths used during the primary bootstrap.
    Set("SDK_INC_PATH_BOOT", catdir ($msvcdir, "PlatformSDK", "include"));
    Set("CRT_INC_PATH_BOOT", catdir ($msvcdir, "Include"));
    Set("SDK_LIB_PATH", catdir ($msvcdir, "PlatformSDK", "lib"));
    Set("CRT_LIB_PATH", catdir ($msvcdir, "lib"));
    Set("VC_BIN_PATH", catdir ($msvcdir, "bin"));
}
else {
    my $lkgvc_dir = Get("LKGVC_DIR");
    # These are the INCLUDEs path used during the primary bootstrap.
    Set("SDK_INC_PATH_BOOT", catdir($lkgvc_dir, "public", "sdk", "inc"));
    Set("CRT_INC_PATH_BOOT", catdir($lkgvc_dir, "public", "vc", "inc"));
    Set("SDK_LIB_PATH", catdir($lkgvc_dir, "public", "sdk", "lib", "i386"));
    Set("CRT_LIB_PATH", catdir($lkgvc_dir, "public", "vc", "lib", "i386"));
    Set("VC_BIN_PATH", catdir($lkgvc_dir, "tools", "x86", "vc", "bin"));
    Prepend("PATH", ";", Get("VC_BIN_PATH"));
}

1;

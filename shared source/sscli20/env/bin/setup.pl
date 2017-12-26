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

use File::Copy;

# verification skipping
@StrongNameKeys = (
    "b03f5f7f11d50a3a",
    "b77a5c561934e089"
);

# global assembly cache
@GACAssemblies = (
    "ISymWrapper.dll",
    "System.dll",
    "System.Configuration.dll",
    "System.Data.SqlXml.dll",
    "System.Runtime.Remoting.dll",
    "System.Runtime.Serialization.Formatters.Soap.dll",
    "System.Xml.dll",
    "Microsoft.Vsa.dll",
    "Microsoft.JScript.dll",
);

#default C# response file - subset of the GAC assemblies
@CSharpAssemblies = (
    "System.dll",
    "System.Configuration.dll",
    "System.Runtime.Remoting.dll",
    "System.Runtime.Serialization.Formatters.Soap.dll",
    "System.Xml.dll",
    "Microsoft.Vsa.dll",
);

@CommandLine = @ARGV;

if (uc($^O) eq "MSWIN32") {
    $Separator = '\\';
    $ExeSuffix = ".exe";
} else {
    $Separator = '/';
    $ExeSuffix = "";
}

$BinDir = $ENV{'_NTTREE'};

if (!@CommandLine || @CommandLine[0] eq "CSharp") {    
    open(RESPONSEFILE, ">" . $BinDir . $Separator . "csc.rsp") || die "Can't create csc.rsp\n";
    print RESPONSEFILE "# This file contains command-line options that the C#\n";
    print RESPONSEFILE "# command line compiler (csc) will process as part\n";
    print RESPONSEFILE "# of every compilation, unless the \"/noconfig\" option\n";
    print RESPONSEFILE "# is specified.\n";
    print RESPONSEFILE "\n";

    while (@CSharpAssemblies) {
        my $assembly = pop(@CSharpAssemblies);
        print RESPONSEFILE "/r:" . $assembly . "\n";
    }
    close(RESPONSEFILE);
    print("Default C# response file created succesfully.\n\n");
}

if (!@CommandLine || @CommandLine[0] eq "DelaySign") {
    while (@StrongNameKeys) {
        my $key = pop(@StrongNameKeys);
        if (@CommandLine > 1 && @CommandLine[1] ne $key) {
            next;
        }
        my $cmdline = $BinDir . $Separator . "sdk" . $Separator . "bin" . $Separator . "sn" . $ExeSuffix . " -Vr *," . $key;
        print $cmdline . "\n";
        if (system($cmdline) != 0) {
            die "Failed to register verification skipping for " . $key . "\n";
        }
    }
}

if (!@CommandLine || @CommandLine[0] eq "GAC") {
    while (@GACAssemblies) {
        my $assembly = pop(@GACAssemblies);
        if (@CommandLine > 1 && @CommandLine[1] ne $assembly) {
            next;
        }
        my $cmdline = $BinDir . $Separator . "gacutil" . $ExeSuffix . " /i " . $BinDir . $Separator . $assembly;
        print $cmdline . "\n";
        if (system($cmdline) != 0) {
            die "Failed to install " . $assembly . " to the GAC\n";
        }
    }
}

exit 0;

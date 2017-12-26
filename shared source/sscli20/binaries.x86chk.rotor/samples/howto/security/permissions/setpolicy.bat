REM ==++==
REM 
REM 
REM  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
REM 
REM  The use and distribution terms for this software are contained in the file
REM  named license.txt, which can be found in the root of this distribution.
REM  By using this software in any fashion, you are agreeing to be bound by the
REM  terms of this license.
REM 
REM  You must not remove this notice, or any other, from this software.
REM 
REM 
REM ==--==

REM Install myperm.dll to the GAC
gacutil -i myperm.dll

REM *** use caspol -reset to reset to default settings

REM Set local machine code to grant Everything, but not FullTrust
clix %_NTTREE%\caspol -q -cg 1.1 Everything

REM At this point strongnamed assemblies must have the AllowPartiallyTrustedCallers attribute
REM to be able to call other strongnamed assemblies.  You will want to reset this.

REM Add the myperm.dll perm assembly to the full trust list,
REM so that we can then use -ap to add this permission.
clix %_NTTREE%\caspol -q -af myperm.dll

REM Add the myperm perm set
clix %_NTTREE%\caspol -q -ap myperm.xml

REM Grant myperm to the authorized sample
clix %_NTTREE%\caspol -q -ag 1.1 -strong -file authorized.exe -noname -noversion myperm -name "allowed"

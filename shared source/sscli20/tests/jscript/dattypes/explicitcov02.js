//# ==++== 
//# 
//#   
//#    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//#   
//#    The use and distribution terms for this software are contained in the file
//#    named license.txt, which can be found in the root of this distribution.
//#    By using this software in any fashion, you are agreeing to be bound by the
//#    terms of this license.
//#   
//#    You must not remove this notice, or any other, from this software.
//#   
//# 
//# ==--== 
//####################################################################################
@cc_on


import System;

var NULL_DISPATCH = null;
var apGlobalObj;
var apPlatform;
var lFailCount;


var iTestID = 228257;

/* -------------------------------------------------------------------------
  Test: 	explicitcov02
   
  Component:	JScript
 
  Test Area:	Type Conversion
 
 ---------------------------------------------------------------------------
  
 ---------------------------------------------------------------------------
  
 
	[00]	7-Dec-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/




function explicitcov02() {

    apInitTest("explicitcov02"); 

	
	apInitScenario("1.int()");
@if (! @_fast)
	var i : int = int(null);
	if ( i!= 0) 
		apLogFailInfo ("1.",0,i, "")
@end
	apInitScenario ("2. byte()") ;
@if (!@_fast)
	var b: byte = byte(null);
	if (b != 0)
		apLogFailInfo ("2.",0,b ,"");
@end
	apInitScenario ("3. long() ");
@if (!@_fast)

	var l: long = long(null);
	if (l != 0)
		apLogFailInfo ("3.", 0,l ,"");
@end	
	apInitScenario ("4.sbyte () ")
@if (!@_fast)

	var sb: sbyte = sbyte(null);
	if (sb != 0 )
		apLogFailInfo ("4.",0, sb,"");
@end
	apInitScenario ("5. short () ")
@if (!@_fast)
	var s5: short = short(null);
	if (s5 != 0) 
		apLogFailInfo ("5.",0,s5,"");
@end
	apInitScenario ("6. uint()")
@if (!@_fast)
	var ui : uint = uint(null);
	if ( ui != 0) 
		apLogFailInfo("6.",0,ui,"");
@end
	apInitScenario ("7. ulong()")
@if (!@_fast)
	var ul: ulong = ulong(null);
	if (ul != 0)
		apLogFailInfo("7.",0,ul,"");
@end
	apInitScenario ("8. ushort()" )
@if (!@_fast)
	var us: ushort = ushort(null);
	if (us != 0)
		apLogFailInfo("7.",0,us,"");
@end 
	apInitScenario ("9. boolean() ");
@if (!@_fast)
	var b9: boolean = boolean(null);
	if ( b9 != false) 
		apLogFailInfo("9.",false,b9,"");
@end	
	apInitScenario("10. char() ");
@if (!@_fast)
	var c : char = char(null);
	if ( c != 0) 
		apLogFailInfo("10.",0,c,"");
@end	
	apInitScenario ("11. float() ");
@if (!@_fast)
	var f11: float = float(null);
	if ( f11 != 0) 
		apLogFailInfo("11.",0, f11, "");
@end
	apInitScenario("12. double() ");
@if (!@_fast)
	var d : double = double(null);
	if ( d!= 0) 
		apLogFailInfo ("12.",0,d, "");
@end	
	apInitScenario("13.Number() ");
@if (!@_fast)
	var n : Number = Number (null);
	if ( n != 0) 
		apLogFailInfo ("13.",0,n ,"");
@end
	apInitScenario("14. deciaml()");
@if (!@_fast)
	var d14 : decimal = decimal(null);
	if (d14 != 0) 
		apLogFailInfo( "14.",0, d14, "");
@end
//apLogFailInfo ("errorMessage, expected, actual, bugNum)
	


	apEndTest();
}




explicitcov02();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 1;

function apInitTest(stTestName) {
    lFailCount = 0;

    apGlobalObj = new Object();
    apGlobalObj.apGetPlatform = function Funca() { return "Rotor" }
    apGlobalObj.LangHost = function Funcb() { return 1033;}
    apGlobalObj.apGetLangExt = function Funcc(num) { return "EN"; }

    apPlatform = apGlobalObj.apGetPlatform();
    var sVer = "1.0";  //navigator.appVersion.toUpperCase().charAt(navigator.appVersion.toUpperCase().indexOf("MSIE")+5);
    apGlobalObj.apGetHost = function Funcp() { return "Rotor " + sVer; }
    print ("apInitTest: " + stTestName);
}

function apInitScenario(stScenarioName) {print( "\tapInitScenario: " + stScenarioName);}

function apLogFailInfo(stMessage, stExpected, stActual, stBugNum) {
    lFailCount = lFailCount + 1;
    print ("***** FAILED:");
    print ("\t\t" + stMessage);
    print ("\t\tExpected: " + stExpected);
    print ("\t\tActual: " + stActual);
}

function apGetLocale(){ return 1033; }
function apWriteDebug(s) { print("dbg ---> " + s) }
function apEndTest() {}

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


var iTestID = 228260;

/* -------------------------------------------------------------------------
  Test: 	explicitcov05
   
  Component:	JScript
 
  Test Area:	Type Conversion
 
 ---------------------------------------------------------------------------
  
 ---------------------------------------------------------------------------
  
 
	[00]	7-Dec-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/




function explicitcov05() {

    apInitTest("explicitcov05"); 

	
	/*apInitScenario("1.int()");
	var i : int = int(Infinity);
	if ( i!= 0) 
		apLogFailInfo ("1.",0,i, "")

	apInitScenario ("2. byte()") ;
	var b: byte = byte(Infinity);
	if (b != 0)
		apLogFailInfo ("2.",0,b ,"");

	apInitScenario ("3. long() ");
	var l: long = long(Infinity);
	//if (l != 0)
	//	apLogFailInfo ("3.", 0,l ,"");
	
	apInitScenario ("4.sbyte () ")
	var sb: sbyte = sbyte(Infinity);
	if (sb != 0 )
		apLogFailInfo ("4.",0, sb,"");

	apInitScenario ("5. short () ")
	var s5: short = short(Infinity);
	if (s5 != 0) 
		apLogFailInfo ("5.",0,s5,"");

	apInitScenario ("6. uint()")
	var ui : uint = uint(Infinity);
	if ( ui != 0) 
		apLogFailInfo("6.",0,ui,"");

	apInitScenario ("7. ulong()")
	var ul: ulong = ulong(Infinity);
	if (ul != 0)
		apLogFailInfo("7.",0,ul,"");

	apInitScenario ("8. ushort()" )
	var us: ushort = ushort(Infinity);
	if (us != 0)
		apLogFailInfo("7.",0,us,"");
 
	apInitScenario ("9. boolean() ");
	var b9: boolean = boolean(Infinity);
	if ( b9 != true) 
		apLogFailInfo("9.",true,b9,"");
	
	apInitScenario("10. char() ");
	var c : char = char(Infinity);
	if ( c != 0) 
		apLogFailInfo("10.",0,c,"");*/
	
	apInitScenario ("11. float() ");
	var f11: float = float(Infinity);
	if ( isFinite(f11)) 
		apLogFailInfo("11.","Infinity", f11, "");

	apInitScenario("12. double() ");
	var d : double = double(Infinity);
	if ( isFinite(d)) 
		apLogFailInfo ("12.","Infinity",d, "");
	
	apInitScenario("13.Number() ");


	var n : Number = Number (Infinity);
	if ( isFinite(n)) 
		apLogFailInfo ("13.","Infinity",n ,"");

	apInitScenario("14. deciaml()");
/*
	var d14 : decimal = decimal(Infinity);
	if (isFinity(d14)) 
		apLogFailInfo( "14.","Infinity", d14, "");
*/
//apLogFailInfo ("errorMessage, expected, actual, bugNum)
	


	apEndTest();
}




explicitcov05();


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

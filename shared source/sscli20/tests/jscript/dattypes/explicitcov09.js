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


var iTestID = 228270;

/* -------------------------------------------------------------------------
  Test: 	explicitcov09
   
  Component:	JScript
 
  Test Area:	Type Conversion
 
 ---------------------------------------------------------------------------
  
 ---------------------------------------------------------------------------
  
 
	[00]	10-Dec-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/


var sAct = "";
var sExp = "";


function explicitcov09() {

    apInitTest("explicitcov09"); 

	
	apInitScenario("1.int ");
	var i1 : int = int.MinValue;
	sAct = String(i1);
	sExp = int.MinValue.ToString();
	if (sAct != sExp)
		apLogFailInfo("1.",sExp,sAct,"");

	apInitScenario("2. short ");
	var s2: short = short(short.MinValue);
	sAct = String(s2);
	sExp = short.MinValue.ToString();
	if (sAct != sExp)
		apLogFailInfo("2.",sExp,sAct,"");

	apInitScenario ("3. byte ") ;
	var b3: byte = byte(255);
	sAct = String(b3);
	sExp = "255";
	if (sAct != sExp)
		apLogFailInfo("2.",sExp,sAct,"");
	

	apInitScenario ("4. long ");
	var l4: long = long(long.MinValue);
	sAct = String(l4);
	sExp = "-9223372036854775808";
	if (sAct != sExp)
		apLogFailInfo("4.",sExp,sAct,"");	

	apInitScenario ("5. uint ")
	var ui5: uint = uint(uint.MaxValue);
	sAct = String(ui5);
	sExp = uint.MaxValue.ToString();
	if (sAct != sExp)
		apLogFailInfo("5.",sExp,sAct,"");
		
	apInitScenario ("6. ulong ")
	var ul6 : ulong= ulong(ulong.MaxValue);
	sAct = String(ul6);
	sExp = ulong.MaxValue.ToString();
	if (sAct != sExp)
		apLogFailInfo("6.",sExp,sAct,"");
	
	apInitScenario ("7. ushort")
	var us7: ushort= ushort(ushort.MaxValue);
	sAct = String(us7);
	sExp = ushort.MaxValue.ToString();
	if (sAct != sExp)
		apLogFailInfo("7.",sExp,sAct,"");
	
	apInitScenario ("8. float  ");
	var f8: float = float(float.MaxValue);
	sAct = String(f8);
	sExp = 3.4028234663852886e+38;
	if (sAct != sExp)
		apLogFailInfo("8.",sExp,sAct,"");

	apInitScenario ("9. double");
	var d9: double = double(double.MaxValue);
	sAct = String(d9);
	sExp = 1.7976931348623157e+308;
	if (sAct != sExp)
		apLogFailInfo("9.",sExp,sAct,"");
	
	
	apInitScenario ("10. Number  ");
	var n10: Number = Number(double.MaxValue);
	sAct = String(n10);
	sExp = 1.7976931348623157e+308;
	if (sAct != sExp)
		apLogFailInfo("10.",sExp,sAct,"");
		
	apInitScenario ("11.sbyte  ")
	var sb11: sbyte = sbyte(sbyte.MaxValue);
	sAct = String(sb11);
	sExp = 127;
	if (sAct != sExp)
		apLogFailInfo("11.",sExp,sAct,"");

	apInitScenario ("12.char  ")
	var c12: char = char(char.MaxValue);
	sAct = String(c12);
	sExp = char.MaxValue.ToString();
	if (sAct != sExp)
		apLogFailInfo("12.",sExp,sAct,"");

	apInitScenario ("13.boolean  ")
	var b13: boolean= boolean(true);
	sAct = String(b13);
	sExp = "true";
	if (sAct != sExp)
		apLogFailInfo("13.",sExp,sAct,"");

	apInitScenario ("14.boolean  ")
	var b14: boolean= boolean(false);
	sAct = String(b14);
	sExp = boolean(false).toString();
	if (sAct != sExp)
		apLogFailInfo("14.",sExp,sAct,"");

	b14 = false;
	print(b14.toString());
	apEndTest();
}




explicitcov09();


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

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


var iTestID = 228271;

/* -------------------------------------------------------------------------
  Test: 	explicitcov10
   
  Component:	JScript
 
  Test Area:	Type Conversion
 
 ---------------------------------------------------------------------------
  
 ---------------------------------------------------------------------------
  
 
	[00]	10-Dec-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/


var sAct = "";
var sExp = "";


function explicitcov10() {

    apInitTest("explicitcov10"); 

	
	apInitScenario("1.int ");
	var i1 : int = int.MinValue;
	sAct = boolean(i1);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("1.",sExp,sAct,"");

	apInitScenario("2. short ");
	var s2: short = short(short.MinValue);
	sAct = boolean(s2);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("2.",sExp,sAct,"");

	apInitScenario ("3. byte ") ;
	var b3: byte = byte(255);
	sAct = boolean(b3);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("2.",sExp,sAct,"");
	

	apInitScenario ("4. long ");
	var l4: long = long(long.MinValue);
	sAct = boolean(l4);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("4.",sExp,sAct,"");	

	apInitScenario ("5. uint ")
	var ui5: uint = uint(uint.MaxValue);
	sAct = boolean(ui5);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("5.",sExp,sAct,"");
		
	apInitScenario ("6. ulong ")
	var ul6 : ulong= ulong(ulong.MaxValue);
	sAct = boolean(ul6);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("6.",sExp,sAct,"");
	
	apInitScenario ("7. ushort")
	var us7: ushort= ushort(ushort.MaxValue);
	sAct = boolean(us7);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("7.",sExp,sAct,"");
	
	apInitScenario ("8. float  ");
	var f8: float = float(float.MaxValue);
	sAct = boolean(f8);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("8.",sExp,sAct,"");

	apInitScenario ("9. double");
	var d9: double = double(double.MaxValue);
	sAct = boolean(d9);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("9.",sExp,sAct,"");
	
	
	apInitScenario ("10. Number  ");
	var n10: Number = Number(double.MaxValue);
	sAct = boolean(n10);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("10.",sExp,sAct,"");
		
	apInitScenario ("11.sbyte  ")
	var sb11: sbyte = sbyte(sbyte.MaxValue);
	sAct = boolean(sb11);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("11.",sExp,sAct,"");

	apInitScenario ("12.char  ")
	var c12: char = char(char.MaxValue);
	sAct = boolean(c12);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("12.",sExp,sAct,"");

	apInitScenario ("13.Boolean  ")
	var b13: Boolean= Boolean(true);
	sAct = boolean(b13);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("13.",sExp,sAct,"");

	apInitScenario ("14.boolean  ")
	var b14: boolean= boolean(false);
	sAct = boolean(b14);
	sExp = false;
	if (sAct != sExp)
		apLogFailInfo("14.",sExp,sAct,"");

	apInitScenario ("15.String ")
	var s15: String= "";
	sAct = boolean(s15);
	sExp = false;
	if (sAct != sExp)
		apLogFailInfo("15.",sExp,sAct,"");

	apEndTest();
}




explicitcov10();


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

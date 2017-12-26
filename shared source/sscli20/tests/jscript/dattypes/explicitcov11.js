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


var iTestID = 228283;

/* -------------------------------------------------------------------------
  Test: 	explicitcov11
   
  Component:	JScript
 
  Test Area:	Type Conversion
 
 ---------------------------------------------------------------------------
  
 ---------------------------------------------------------------------------
  
 
	[00]	10-Dec-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/


var sAct = "";
var sExp = "";


function explicitcov11() {

    apInitTest("explicitcov11"); 

	
	apInitScenario("1.int ");
	var i1 : int = int.MinValue;
	sAct = char(i1);
	sExp = char.MinValue;
	if (sAct != sExp)
		apLogFailInfo("1.",sExp,sAct,"");

	apInitScenario("2. short "); //321016
@if (!@_fast) 
	var s2: short = short(short.MinValue);
	sAct = char(s2);
	sExp = 32768;
	if (sAct != sExp)
		apLogFailInfo("2.",sExp,sAct,"");
@end


	apInitScenario ("3. byte ") ;
	var b3: byte = byte(255);
	sAct = char(b3);
	sExp = 255;
	if (sAct != sExp)
		apLogFailInfo("3.",sExp,sAct,"");
	
	apInitScenario ("4. long ");
	var l4: long = long(long.MinValue);
	sAct = char(l4);
	sExp = 0;
	if (sAct != sExp)
		apLogFailInfo("4.",sExp,sAct,"");	

	apInitScenario ("5. uint ")
	var ui5: uint = uint(uint.MaxValue);
	sAct = char(ui5);
	sExp = char.MaxValue;
	if (sAct != sExp)
		apLogFailInfo("5.",sExp,sAct,"");
	
	apInitScenario ("6. ulong ")
	var ul6 : ulong= ulong(ulong.MaxValue);
	sAct = char(ul6);
	sExp = char.MaxValue; //321026
	if (sAct != sExp)
		apLogFailInfo("6.",sExp,sAct,"");
	
	apInitScenario ("7. ushort")
	var us7: ushort= ushort(ushort.MaxValue);
	sAct = char(us7);
	sExp = char.MaxValue;
	if (sAct != sExp)
		apLogFailInfo("7.",sExp,sAct,"");
	
	/* These test depend on unspecified behavior of overflow during the conversion
           from floating point number to U2. x86 sets the result to 0 and PPC sets it to 0xFFFF
        apInitScenario ("8. float  ");
	var f8: float = float(float.MaxValue);
	sAct = char(f8);
	sExp = 0;
	if (sAct != sExp)
		apLogFailInfo("8.",sExp,sAct,"");

	apInitScenario ("9. double");
	var d9: double = double(double.MaxValue);
	sAct = char(d9);
	sExp = 0;
	if (sAct != sExp)
		apLogFailInfo("9.",sExp,sAct,"");

	
	apInitScenario ("10. Number  ");
	var n10: Number = Number(double.MaxValue);
	sAct = char(n10);
	sExp = 0;
	if (sAct != sExp)
		apLogFailInfo("10.",sExp,sAct,""); */
		
	apInitScenario ("11.sbyte  ") //321018
@if (!@_fast)
	var sb11: sbyte = sbyte(sbyte.MaxValue);
	sAct = char(sb11);
	sExp = 127;
	if (sAct != sExp)
		apLogFailInfo("11.",sExp,sAct,"");
@end

	apInitScenario ("12.char  ")
	var c12: char = char(char.MaxValue);
	sAct = char(c12);
	sExp = char.MaxValue;
	if (sAct != sExp)
		apLogFailInfo("12.",sExp,sAct,"");

	apInitScenario ("13.char  ")
	var b13: char= char(true);
	sAct = char(b13);
	sExp = true;
	if (sAct != sExp)
		apLogFailInfo("13.",sExp,sAct,"");

	apInitScenario ("14.char  ")
	var b14: char= char(false);
	sAct = char(b14);
	sExp = false;
	if (sAct != sExp)
		apLogFailInfo("14.",sExp,sAct,"");

	apInitScenario ("15.String ")
	var s15: String= "1";
	sAct = char(s15[0]);
	sExp = 49; 
	if (sAct != sExp)
		apLogFailInfo("15.",sExp,sAct,"");

	apInitScenario ("16.boolean ")
	var b16: boolean= true;
	sAct = char(b16);
	sExp = 1
	if (sAct != sExp)
		apLogFailInfo("16.",sExp,sAct,"");

	apInitScenario ("17.Boolean ")
	var b17: Boolean= true;
	sAct = char(b17);
	sExp = 1
	if (sAct != sExp)
		apLogFailInfo("17.",sExp,sAct,"");
	apEndTest();
}




explicitcov11();


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

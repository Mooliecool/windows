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


var iTestID = 228264;

/* -------------------------------------------------------------------------
  Test: 	explicitcov08
   
  Component:	JScript
 
  Test Area:	Type Conversion
 
 ---------------------------------------------------------------------------
  
 ---------------------------------------------------------------------------
  
 
	[00]	10-Dec-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, "");
}



function explicitcov08() {

    apInitTest("explicitcov08"); 

	
	apInitScenario("1.int(123.456)");
	const i : int = int(123.456);
	if ( i!= 123) 
		apLogFailInfo ("1.",123,i, "")

	apInitScenario ("2. byte(124.99)") ;
	const b: byte = byte(124.99);
	if (b != 124)
		apLogFailInfo ("2.", 124,b ,"");

	apInitScenario ("3. long(123.456) ");
	const l: long = long(123.456);
	if (l != 123)
		apLogFailInfo ("3.", 123,l ,"");
	
	apInitScenario ("4.sbyte (456.789) ")
	const sb: sbyte = sbyte(456.789);
	if (sb != -56 )
		apLogFailInfo ("4.",-56, sb,"");

	apInitScenario ("5. short (5.6789) ")
	const s5: short = short(5.6789);
	if (s5 != 5) 
		apLogFailInfo ("5.",5,s5,"");

	apInitScenario ("6. uint(1.02)")
	const ui : uint = uint(1.02);
	if ( ui != 1) 
		apLogFailInfo("6.",1,ui,"");

	apInitScenario ("7. ulong(23.45)")
	const ul: ulong = ulong(23.45);
	if (ul != 23)
		apLogFailInfo("7.",23,ul,"");

	apInitScenario ("8. ushort(55.66)" )
	const us: ushort = ushort(55.66);
	if (us != 55)
		apLogFailInfo("7.",55,us,"");
 
	apInitScenario ("9. boolean(55.66) ");
	const b9: boolean = boolean(55.66);
	if ( b9 != true) 
		apLogFailInfo("9.",true,b9,"");
	
	apInitScenario("10. char(101.99) ");
	const c : char = char(101.99);
	if ( c != 'e') 
		apLogFailInfo("10.", 'e',c,"");
	
	apInitScenario ("11. float(88.88) ");
	const f11: float = float(88.88);
	if ( f11 != 88.88) 
		apLogFailInfo("11.",88.88, f11, "");

	apInitScenario("12. double(44.99) ");
	const d : double = double(44.99);
	if ( d!= 44.99) 
		apLogFailInfo ("12.",44.99,d, "");
	
	apInitScenario("13.Number(123.456) ");
	const n : Number = Number (123.456);
	if ( n != 123.456) 
		apLogFailInfo ("13.", 123.456,n ,"");

	apInitScenario("14. deciaml(4321.4314)");
	const d14 : decimal = decimal(4321.4314);
	if (d14 != 4321.4314) 
		apLogFailInfo( "14.",4321.4314, d14, "");

//apLogFailInfo ("errorMessage, expected, actual, bugNum)
	


	apEndTest();
}




explicitcov08();


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

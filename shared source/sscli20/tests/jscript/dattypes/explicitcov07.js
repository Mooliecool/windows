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


var iTestID = 228263;

/* -------------------------------------------------------------------------
  Test: 	explicitcov07
   
  Component:	JScript
 
  Test Area:	Type Conversion
 
 ---------------------------------------------------------------------------
  
 ---------------------------------------------------------------------------
  
 
	[00]	10-Dec-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/


var sAct = "";
var sExp = "";

function explicitcov07() {

    apInitTest("explicitcov07"); 

	
	apInitScenario("1.int - short");
	var i : int = 2147483647;
	var s1: short = short(i); 
	if ( s1!= -1) 
		apLogFailInfo ("1.",-1,s1, "")

	apInitScenario("2. short - int");
	var s2: short = short(32767);
	var i2 : int = int(s2); 
	if ( i2!= 32767 ) 
		apLogFailInfo ("2.",32767,i2, "")


	apInitScenario ("3. byte - int") ;
	var b3: byte = byte(255);
	var i3 : int = int(b3);
	if ( i3 != 255)
		apLogFailInfo ("3.",255,i3 ,"");

	apInitScenario ("4. int -byte") ;
	var i4 : int = int(2147483647);
	var b4: byte = byte(i4);
	
	if ( b4 != 255)
		apLogFailInfo ("4.",255,b4 ,"");


	apInitScenario ("5. short -byte") ;
	var s5 : short = short(32767);
	var b5: byte = byte(s5);
	
	if ( b5 != 255)
		apLogFailInfo ("5.",255,b5 ,"");


	apInitScenario ("6. byte-short") ;
	var b6: byte = byte(255);
	var s6 : short = short(b6);
	
	if ( s6 != 255)
		apLogFailInfo ("6.",255,s6 ,"");

	apInitScenario ("7. long - int ");
	var l7: long = long(10000000000000000000);
	var i7: int = int(l7);
	if (i7 != -1981284352)
		apLogFailInfo ("7.", -1981284352,i7 ,"");

	apInitScenario ("8.int -long ");
	var i8: int = int(2147483647);
	var l8: long = long(i8);
	
	if (l8 != 2147483647)
		apLogFailInfo ("8.", 2147483647,l8 ,"");

	apInitScenario ("9. long - short ");
	var l9: long = long(10000000000000000000);
	var s9: short = short(l9);
	if (s9 != 0)
		apLogFailInfo ("9.",0,s9 ,"");

	apInitScenario ("10. short -long ");
	var s10:short= short(32767);
	var l10: long = long(s10);
	
	if (l10 != 32767)
		apLogFailInfo ("10.", 32767,l8 ,"");

	apInitScenario ("11. uint - ulong")
	var ui11 : uint = uint(uint.MinValue);
	sAct = ulong(ui11);
	sExp = uint.MinValue;
	if (sAct != sExp)
		apLogFailInfo("11.",sExp,sAct,"");
		
	apInitScenario ("12. ulong - uint")
	var ul12 : ulong= ulong(ulong.MinValue);
	sAct = uint(ul12);
	sExp = uint.MinValue //4294967295;
	if (sAct != sExp)
		apLogFailInfo("12.",sExp,sAct,"");
	
	apInitScenario ("13. ushort- uint")
	var us13 : ushort= ushort(ushort.MinValue);
	sAct = uint(us13);
	sExp = 0;
	if (sAct != sExp)
		apLogFailInfo("13.",sExp,sAct,"");
	
	apInitScenario ("14. uint-ushort ")
	var ui14 : uint= uint(uint.MinValue);
	sAct = ushort(ui14);
	sExp = ushort.MinValue;
	if (sAct != sExp)
		apLogFailInfo("14.",sExp,sAct,"");
	
	apInitScenario ("15. ushort-byte ")
	var us15: ushort= ushort(ushort.MinValue);
	sAct = byte(us15);
	sExp = byte.MinValue;
	if (sAct != sExp)
		apLogFailInfo("15.",sExp,sAct,"");
	
	apInitScenario ("16. uint-byte ")
	var ui16 : uint= uint(uint.MinValue);
	sAct = byte(ui16);
	sExp = byte.MinValue;
	if (sAct != sExp)
		apLogFailInfo("16.",sExp,sAct,"");

	apInitScenario ("17. float - double ");
	var f17: float = float(float.MinValue);
	sAct = double(f17);
	sExp = -3.4028234663852886e+38;
	if (sAct != sExp)
		apLogFailInfo("17.",sExp,sAct,"");

	apInitScenario ("18. float - Number ");
	var f18: float = float(float.MinValue);
	sAct = Number(f18);
	sExp = -3.4028234663852886e+38;
	if (sAct != sExp)
		apLogFailInfo("18.",sExp,sAct,"");

	apInitScenario ("19. double-float  ");
	var d19: double = double(double.MinValue);
	sAct = float(d19);
	sExp = -Infinity;
	if (sAct != sExp)
		apLogFailInfo("19.",sExp,sAct,"");
	
	
	apInitScenario ("20. Number-float  ");
	var n20: Number = Number(double.MinValue);
	sAct = float(n20);
	sExp = -Infinity;
	if (sAct != sExp)
		apLogFailInfo("20.",sExp,sAct,"");
		
	apInitScenario ("21.sbyte - byte ")
	var sb21: sbyte = sbyte(sbyte.MinValue);
	sAct = byte(sb21);
	sExp = 128;
	if (sAct != sExp)
		apLogFailInfo("21.",sExp,sAct,"");

	apInitScenario ("22.byte - sbyte ")
	var b21: byte = byte(byte.MinValue);
	sAct = sbyte(sb21);
	sExp = -128;
	if (sAct != sExp)
		apLogFailInfo("22.",sExp,sAct,"");

	apInitScenario ("23. uint-int ")
	var ui23 : uint= uint(uint.MinValue);
	sAct = int(ui23);
	sExp = 0;
	if (sAct != sExp)
		apLogFailInfo("23.",sExp,sAct,"");

	apInitScenario ("24. int-uint ")
	var i24 : int= int(int.MinValue);
	sAct = uint(i24);
	sExp = 2147483648;
	if (sAct != sExp)
		apLogFailInfo("24.",sExp,sAct,"");

	apInitScenario ("25. ushort-short ")
	var u25: ushort= ushort(ushort.MinValue);
	sAct = short(u25);
	sExp = 0;
	if (sAct != sExp)
		apLogFailInfo("25.",sExp,sAct,"");

	apInitScenario ("26. short-ushort ")
	var s26: short= short(short.MinValue);
	sAct = ushort(s26);
	sExp = -short.MinValue;
	if (sAct != sExp)
		apLogFailInfo("26.",sExp,sAct,"");

	apInitScenario ("27. ulong - long")
	var ul27 : ulong= ulong(ulong.MinValue);
	sAct = long(ul27);
	sExp = 0 ;
	if (sAct != sExp)
		apLogFailInfo("27.",sExp,sAct,"");

	apInitScenario ("28. long - ulong")
	var l28 : long= long(long.MinValue);
	sAct = ulong(l28);
	sExp =  9223372036854775808     //-long.MinValue ;
	if (sAct != sExp)
		apLogFailInfo("28.",sExp,sAct,"");

	apEndTest();
}




explicitcov07();


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

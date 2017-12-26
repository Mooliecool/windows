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


var iTestID = 204863;

/*----------------------------------------
Test: dotnet04 
Product:  JScript
Area: 	System
Purpose: using Boolean(), SByte() etc.
Notes: 

-----------------------------------

---------------------------------------------------*/


var sAct="";
var sExp = "";
var sActErr = "";
var sExpErr = "";
var sErrThr = "";

function verify(sRes, sExp, sMsg, sBug)
{
	if (sRes != sExp)
		apLogFailInfo(sMsg, sExp, sRes, sBug);
}


@if(!@aspx)
	import System
@end

function dotnet04() 
{
    	var _arr: Array = new Array();
    	var _boolean:Boolean = Boolean(true); 
    	var _byte: Byte = Byte(100);
	var _char: Char = Char("A");
	var _DateTime: DateTime = new DateTime(2001,1,1);
	var _decimal: Decimal = new Decimal(45.45);
	var _double : Double = Double(555.666);	
	var _int16: Int16 = Int16(-1234);
	var _int32: Int32 = Int32(-5678);
	var _int64: Int64 = Int64(-8765);
	var _object: Object = new Object();
	var _sbyte: SByte = SByte(-128);
	var _single: Single = Single(-0);
	var _strchar:System.String = new System.String(_char,2);
	var _Uint16: UInt16 = UInt16(1234);
	var _Uint32: UInt32 = UInt32(5678);
	var _Uint64: UInt64 = UInt64(8765);
	
	apInitTest (" dotnet04 : test using dotnet type");
	
	apInitScenario ("1.using Byte");
	verify(_byte,100,"1.1 using Byte","");
	_byte= Byte.MaxValue;
	verify(_byte,255,"1.2 using Byte","");
	_byte = Byte.MinValue;
	verify(_byte,0,"1.3 using Byte","");
	
	apInitScenario ("2. using Boolean");
	verify(_boolean,true,"2.1 using Boolean","");
	var s2: String = Boolean.FalseString;
	verify(s2,"False","2.2 using Boolean","");

	apInitScenario ("3. using Char");
	verify(_char,"A","3.1 using Char","");
	verify(Char.IsLetter(_char),true,"3.2 using Char","");
	_char = ' ';
	verify(Char.IsWhiteSpace(_char),true,"3.3 using Char","");
	verify(Char.MaxValue,0xFFFF,"3.4 using Char","");
	//print(Char.MaxValue);

	apInitScenario ("4. using DateTime");
	//verify(_DateTime.ToString(),"1/1/2001 12:00:00 AM","4.1 using DateTime","");
         if (_DateTime.ToString() != "1/1/2001 12:00:00 AM") {
                if (_DateTime.ToString() != "1/1/01 12:00:00 AM")
                       apLogFailInfo("4. using DateTime","1/1/01 12:00:00 AM or 1/1/2001" ,_DateTime.ToString(),"");
      }

	_DateTime = new DateTime(2001,1,1,1,1,1);
	//verify(_DateTime.ToString(),"1/1/2001 1:01:01 AM","4.2 using DateTime","");
       if (_DateTime.ToString() != "1/1/2001 1:01:01 AM") {
                if (_DateTime.ToString() != "1/1/01 1:01:01 AM")
                       apLogFailInfo("4. using DateTime","1/1/01 1:01:01 AM or 1/1/2001" ,_DateTime.ToString(),"");
      }
	
	apInitScenario ("5. using Decimal");
	verify(_decimal,45.45,"5.1 using Decimal","");
	var uint5 : UInt64 = UInt64.MaxValue;
	_decimal = Decimal(uint5);
	verify(_decimal,18446744073709551615,"5.2 using Decimal","");
	
	apInitScenario ("6. using Double");
	verify(_double,555.666,"6.1 using Double","");
	_double = Double.NaN;
	verify(isNaN(_double),true,"6.1 using Double","");
		
	apInitScenario ("7. using Int16");
	verify(_int16,-1234,"7.1 using Int16","");
	_int16 = Int16.MaxValue;
	verify(_int16,32767,"7.2 using Int16","");
	
	apInitScenario ("8. using Int32");
	verify(_int32,-5678,"8.1 using Int32","");
	_int32 = Int32.MinValue;
	verify(_int32,-2147483648,"8.2 using Int32","");

	apInitScenario ("9. using Int64");
	verify(_int64,-8765,"9.1 using Int64","");
	_int64 = Int64.MinValue;
	verify(_int64,-9223372036854775808,"9.2 using Int64","");

	apInitScenario ("10. using Object");
	_object = 1234;
	verify(_object,1234,"10.1 using Object","");
	_object = _DateTime;
	verify(_object,_DateTime,"10.2 using Object","");

	apInitScenario ("11. using SByte");
	verify(_sbyte,SByte.MinValue,"11. using SByte","");


	apInitScenario ("12. using Single");
	verify(_single,0,"12.1 using Single","");
	_single = Single.NegativeInfinity;	
	verify(_single,-Infinity,"12.1 using Single","");

	apInitScenario ("13. using String");
	verify(_strchar,"AA","13.1 using String","");
	_strchar = new System.String("-",5);
	verify(_strchar,"-----","13.2 using String","");

	apInitScenario ("14. using UInt16");
	verify(_Uint16,1234,"14.1 using UInt16","");
	_Uint16 = UInt16.MaxValue;
	verify(_Uint16,65535,"14.2 using UInt16","");
	
	apInitScenario ("15. using UInt32");
	verify(_Uint32,5678,"15.1 using UInt32","");
	_Uint32 = UInt32.MinValue;
	verify(_Uint32,0,"15.2 using UInt32","");

	apInitScenario ("16. using UInt64");
	verify(_Uint64,8765,"16.1 using UInt64","");
	_Uint64 = UInt64.MaxValue;
	verify(_Uint64,18446744073709551615,"16.2 using UInt64","");


	apEndTest();


}



dotnet04();


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

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


var iTestID = 204859;

/*----------------------------------------
Test: dotnet00 
Product:  JScript
Area: 	System
Purpose: assign a value to Array, Boolean, Byte, Char etc general type
Notes: 

-----------------------------------
[00] 31-7-2001
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

function dotnet00() 
{
    	var _arr: System.Array = [0,1,2];
   	//print(_arr.GetLength);
    	var _boolean:System.Boolean = true; 
    	var _byte: System.Byte = 100;
	var _char: System.Char = "A";
	var _DateTime: System.DateTime = new System.DateTime(2001,1,1);
	var _decimal: System.Decimal = new System.Decimal(45.45);
	var _double : System.Double = (555.666);	
	var _int16: System.Int16 = -1234;
	var _int32: System.Int32 = -5678;
	var _int64: System.Int64 = -8765;
	var _object: System.Object = new System.Object();
	var _sbyte: System.SByte = -128;
	var _single: System.Single = -0;
	var _strchar: System.String = new System.String(_char,2);
	var _Uint16: System.UInt16 = 1234;
	var _Uint32: System.UInt32 = 5678;
	var _Uint64: System.UInt64 = 8765;
	
	apInitTest (" dotnet00 : test using dotnet type");
	
	apInitScenario ("1.using System.Byte");
	verify(_byte,100,"1.1 using System.Byte","");
	_byte= System.Byte.MaxValue;
	verify(_byte,255,"1.2 using System.Byte","");
	_byte = System.Byte.MinValue;
	verify(_byte,0,"1.3 using System.Byte","");
	
	apInitScenario ("2. using System.Boolean");
	verify(_boolean,true,"2.1 using System.Boolean","");
	var s2: String = System.Boolean.FalseString;
	verify(s2,"False","2.2 using System.Boolean","");

	apInitScenario ("3. using System.Char");
	verify(_char,"A","3.1 using Char","");
	verify(Char.IsLetter(_char),true,"3.2 using Char","");
	_char = ' ';
	verify(Char.IsWhiteSpace(_char),true,"3.3 using Char","");
	verify(System.Char.MaxValue,0xFFFF,"3.4 using System.Char","");
	//print(Char.MaxValue);

	apInitScenario ("4. using System.DateTime");
      if (_DateTime.ToString() != "1/1/2001 12:00:00 AM") {
                if (_DateTime.ToString() != "1/1/01 12:00:00 AM")
                       apLogFailInfo("4.2 using DateTime","1/1/2001 12:00:00 AM or 1/1/01 " ,_DateTime.ToString(),"");
       }
	//verify(_DateTime.ToString(),"1/1/2001 12:00:00 AM","4.1 using DateTime","");
	_DateTime = new System.DateTime(2001,1,1,1,1,1);
       if (_DateTime.ToString() != "1/1/2001 1:01:01 AM") {
                if (_DateTime.ToString() != "1/1/01 1:01:01 AM")
                       apLogFailInfo("4.2 using DateTime","1/1/2001 1:01:01 AM or 1/1/01 " ,_DateTime.ToString(),"");
       }
	//verify(_DateTime.ToString(),"1/1/2001 1:01:01 AM","4.2 using DateTime","");
	
	apInitScenario ("5. using System.Decimal");
	verify(_decimal,45.45,"5.1 using System.Decimal","");
	var uint5 : System.UInt64 = System.UInt64.MaxValue;
	_decimal = Decimal(uint5);
	verify(_decimal,18446744073709551615,"5.2 using System.Decimal","");
	
	apInitScenario ("6. using System.Double");
	verify(_double,555.666,"6.1 using System.Double","");
	_double = System.Double.NaN;
	verify(isNaN(_double),true,"6.1 using System.Double","");
		
	apInitScenario ("7. using System.Int16");
	verify(_int16,-1234,"7.1 using System.Int16","");
	_int16 = System.Int16.MaxValue;
	verify(_int16,32767,"7.2 using System.Int16","");
	
	apInitScenario ("8. using System.Int32");
	verify(_int32,-5678,"8.1 using System.Int32","");
	_int32 = System.Int32.MinValue;
	verify(_int32,-2147483648,"8.2 using System.Int32","");

	apInitScenario ("9. using System.Int64");
	verify(_int64,-8765,"9.1 using System.Int64","");
	_int64 = System.Int64.MinValue;
	verify(_int64,-9223372036854775808,"9.2 using System.Int64","");

	apInitScenario ("10. using System.Object");
	_object = 1234;
	verify(_object,1234,"10.1 using System.Object","");
	_object = _DateTime;
	verify(_object,_DateTime,"10.2 using System.Object","");

	apInitScenario ("11. using System.SByte");
	verify(_sbyte,System.SByte.MinValue,"11. using System.SByte","");


	apInitScenario ("12. using System.Single");
	verify(_single,0,"12.1 using System.Single","");
	_single = System.Single.NegativeInfinity;	
	verify(_single,-Infinity,"12.1 using System.Single","");

	apInitScenario ("13. using System.String");
	verify(_strchar,"AA","13.1 using System.String","");
	_strchar = new System.String("-",5);
	verify(_strchar,"-----","13.2 using System.String","");

	apInitScenario ("14. using System.UInt16");
	verify(_Uint16,1234,"14.1 using System.UInt16","");
	_Uint16 = System.UInt16.MaxValue;
	verify(_Uint16,65535,"14.2 using System.UInt16","");
	
	apInitScenario ("15. using System.UInt32");
	verify(_Uint32,5678,"15.1 using System.UInt32","");
	_Uint32 = System.UInt32.MinValue;
	verify(_Uint32,0,"15.2 using System.UInt32","");

	apInitScenario ("16. using System.UInt64");
	verify(_Uint64,8765,"16.1 using System.UInt64","");
	_Uint64 = System.UInt64.MaxValue;
	verify(_Uint64,18446744073709551615,"16.2 using System.UInt64","");


	apEndTest();


}



dotnet00();


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

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


var iTestID = 204861;

/*----------------------------------------
Test: dotnet02 
Product:  JScript
Area: 	System
Purpose: assign/cast value between JScript type and System type.
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
	import System.IO
@else
	</script>
	<%@ import namespace="System.IO" %>
	<script language=jscript runat=server>
@end

function dotnet02() 
{
    	var _arr: System.Array = [0,1,2];
    	var _boolean:System.Boolean = true; 
    	var _byte: System.Byte = 100;
	var _char: System.Char = "A";
	var _DateTime: System.DateTime = new System.DateTime(2001,1,1);
	var _decimal: System.Decimal = new System.Decimal(555.666);
	var _double : System.Double = (555.666);	
	var _int16: System.Int16 = 1234;
	var _int32: System.Int32 = 5678;
	var _int64: System.Int64 = 8765;
	var _object: System.Object = new System.Object();
	var _sbyte: System.SByte = -128;
	var _single: System.Single = -0;
	var _strchar: System.String = new System.String(_char,2);
	var _Uint16: System.UInt16 = 1;
	var _Uint32: System.UInt32 = 5;
	var _Uint64: System.UInt64 = 8;
	
	var _arr1: Array = new Array();
	_arr1[0] = 0;
	_arr1[1] = 1;
	_arr1[2] = 2;
    	var _boolean1: Boolean = false; 
    	var _byte1: byte = 100;
	var _char1: char = "C";
	var _char2: Char = "B";
	var _Date1: Date = new Date(2001,1,1);
	var _decimal1: decimal = new decimal(45.45);
	var _double1 : double = 666.555;	
	var _int161: Int16 = -1234;
	var _int321: Int32 = -5678;
	var _int641: Int64 = -8765;
	var _object1: Object = new Object();
	var _sbyte1: sbyte = 127;
	var _single1: Single = 1.11;
	var _strchar1: String = new String("ABC");
	var _Uint161: UInt16 = 1;
	var _Uint321: UInt32 = 2;
	var _Uint641: UInt64 = 3;
	var _short: short = 4321;
	var _float: float = 33.44;
	var _long : long = 90;
	var _int: int = 4567;
	var _regexp: RegExp = /\d{2}-\d{2}/g;
	var _number: Number = new Number(4567);

	
	apInitTest (" dotnet02");
	
	apInitScenario ("1.assign between array");
	//var _arr00 : System.Array = _arr1;
	//verify(_arr00[0],3, "1.1 assign between array","");
		
	apInitScenario ("2. assign between Boolean");
	verify(_boolean1,false,"2.1 assign Boolean","");
	_boolean1 = _boolean
	verify(_boolean1,true,"2.2 assign Boolean","");
	_boolean1 = true;
	_boolean = _boolean1;
	verify(_boolean,true,"2.3 assign Boolean","");

	apInitScenario ("3. assign between char");
	verify(_char1,"C","3.1 assign between char","");
	_char1 = _char;
	verify(_char1,"A","3.2 assign between char","");
	_char1 = _char2;
	verify(_char1,"B","3.3 assign between char","");
	_char = _char1;
	verify(char.MaxValue,0xFFFF,"3.4 assign between char","");
	verify(Char.MaxValue,char.MaxValue,"3.5 assign between char","");	

        if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	apInitScenario ("4. assign between date and dateTime");
	verify(_Date1,"Thu Feb 1 00:00:00 PST 2001","4.1 assign between date and dateTime","");
	//_DateTime = new System.DateTime(2002,1,1);
	//_DateTime = _Date1;
	//verify(_DateTime,"Thu Feb 1 00:00:00 PST 2001","4.2 assign between date and dateTime","");
        }
	
	apInitScenario ("5. assign byte value");
	verify(_byte1,_byte,"5.1 assign byte value","");
	/*
	_byte = 99;
	_byte1 = byte;
	verify(_byte1, 99,"5.1 assign byte value","");
	_byte1 = 255;
	_byte = _byte1;
	verify(_byte,255,"5.2 assign byte value","");
	*/
	_byte = 99;
	_byte1 = byte(_byte);
	verify(_byte1,99,"5.2 assign byte value","");
	_byte1 = 199;
	_byte = Byte(_byte1);
	verify(_byte,199,"5.2 assign byte value","");

	apInitScenario ("6. assign decimal value");
	verify(_decimal1,45.45,"6.1 assign decimal value","");
	_decimal1 = _decimal;
	verify(_decimal1,555.666,"6.2 assign decimal value","");

	apInitScenario ("7. assign double value");
	verify(_double1, 666.555,"7.1 assign double value","");
	_double1 = _double;
	verify(_double1,555.666 ,"7.2 assign double value","");

	apInitScenario ("8. assign Int16 value");
	verify(_int161,-1234,"8.1. assign Int16 value","");
	_int161 = _int16;
	verify(_int161,1234,"8.2 assign Int16 value","");
	_int161 = 2;
	_int16 = _int161;
	verify(_int16,2,"8.3 assign Int16 value","");

	apInitScenario ("9. assign Int32 value");
	verify(_int321,-5678,"9.1. assign Int32 value","");
	_int321 = _int32;
	verify(_int321,5678,"9.2 assign Int32 value","");
	_int321 = 2;
	_int32 = _int321;
	verify(_int32,2,"9.3 assign Int32 value","");

	apInitScenario ("10. assign Int64 value");
	verify(_int641,-8765,"10.1. assign Int64 value","");
	_int641 = _int64;
	verify(_int641,8765,"10.2 assign Int64 value","");
	_int641 = 2;
	_int64 = _int641;
	verify(_int64,2,"10.3 assign Int64 value","");

	apInitScenario ("11. assign Object value");	
	_object1 = _object;
	verify(_object1,_object,"11.1 assign Object value","");

	apInitScenario ("12. assign sbyte value");
	verify(_sbyte1,127,"12. assign sbyte value","");
	_sbyte1 = _sbyte;
	verify(_sbyte1,-128,"12.1 assign sbyte value","");
	_sbyte1 = -99;
	_sbyte = _sbyte1;
	verify(_sbyte,-99,"12.2 assign sbyte value","");
	
	apInitScenario ("13. assign single value");
	verify(_single1,1.11,"13.1 assign single value","");
	_single1 = _single;
	verify(_single1,-0,"13.2 assign single value","");
	_single1 = -99.99;
	_single = _single1;
	verify(_single,-99.99,"13.3 assign single value","");


	apInitScenario ("14. assign string value");
	verify(_strchar1,"ABC","14.1 assign string value","");
	_strchar1 =_strchar;
	verify(_strchar1,"AA","14.2 assign string value","");
	_strchar1 = 'abc';
	_strchar = _strchar1;
	verify(_strchar,"abc","14.3 assign string value","");
	
	apInitScenario ("15. assign UInt16 value");
	verify(_Uint161,1,"15.1. assign UInt16 value","");
	_Uint161 = _Uint16+8;
	verify(_Uint161,9,"15.2 assign UInt16 value","");
	_Uint161 = 6;
	_Uint16 = _Uint161;
	verify(_Uint16,6,"15.3 assign UInt16 value","");

	apInitScenario ("16. assign UInt32 value");
	verify(_Uint321,2,"16.1. assign UInt32 value","");
	_Uint321 = _Uint32;
	verify(_Uint321,5,"16.2 assign UInt32 value","");
	_Uint321 = 22;
	_Uint32 = _Uint321;
	verify(_Uint32,22,"16.3 assign UInt32 value","");

	apInitScenario ("17. assign UInt64 value");
	verify(_Uint641,3,"17.1. assign UInt64 value","");
	_Uint641 = _Uint64;
	verify(_Uint641,8,"17.2 assign UInt64 value","");
	_Uint641 = 232;
	_Uint64 = _Uint641;
	verify(_Uint64,232,"17.3 assign UInt64 value","");

	apInitScenario ("18. assign short value");
	verify(_short,4321,"18.1 assign short value","");
	_byte = 99;
	_short = _byte;
	verify(_short,99,"18.2 assign short value","");
	_int16 = -100;
	_short = _int16;
	verify(_short,-100,"18.3 assign short value","");

	apInitScenario ("19. assign int value");
	verify(_int,4567, "19.1 assign int value","");
	_int32 = 32;	
	_int = _int32;
	verify(_int,32, "19.1 assign int value","");
	_Uint32 = 44;
	_int = int(_Uint32);
	verify(_int,44, "19.2 assign int value","");	

	
	apInitScenario ("20. assign float value");
	verify(_float,33.44,"20.1 assign float value","");
	_single = 99.01;
	_float = _single;
	verify(_float,99.01,"20.2 assign float value","");
	_float = float(_sbyte=1);
	verify(_float,1,"20.3 assign float value","");

	apInitScenario ("21. assign long value");
	verify(_long,90,"21.1 assign long value","");
	_int64 = 64;
	_long = _int64;
	verify(_long,64,"21.2 assign long value","");
	_single = 11.11;
	_long = long(_single);
	verify(_long,11,"21.3 assign long value","");
	
	apInitScenario ("22. assign RegExp value");
	
	apInitScenario ("23. assign Number value");
	verify(_number,4567,"23.1 assign Number value","");
	_number = _Uint16 = 13;
	verify(_number,13,"23.1 assign Number value","");
	_number = _double = 44.44;	
	verify(_number,44.44,"23.1 assign Number value","");

	apEndTest();


}



dotnet02();


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

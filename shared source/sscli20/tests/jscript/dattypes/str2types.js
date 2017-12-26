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


var iTestID = 228584;

/* -------------------------------------------------------------------------
  Test: 	str2types
   
  Component:	JScript
 
  Test Area:	Type Conversion
 
 ---------------------------------------------------------------------------
  
 ---------------------------------------------------------------------------
  
 
	[00]	10-Dec-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/


function verify(sAct, sExp, sMes){
	
	if (sAct != sExp){
		if ( isNaN(sExp)) {
			if (isNaN(sAct) != isNaN(sExp) )
        		apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, "");
        }
        else {
			apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, "");
		}
	}
}


function str2types() {

 apInitTest("str2types"); 

var empty : String = String(null);

var mid0: String = "A";
var max0 : String = "1234567890";
var min0 : String = -1234567890;
var zero : String = String(-0.89);

print(max0);
print(min0);
print(zero);
print(empty);
print(mid0);
 

////----------------String---------------
apInitScenario (" to String" )
var string0 : String = String(empty);
verify(string0, empty, "string0");

var strTypedmax0          : String     =     String(max0);
var strTypedmin0            : String     =     String(min0);
var strObjmax0            : Object     =     String(max0);
var strObjmin0             : Object     =     String(min0);
verify(strTypedmax0, max0, "string0");
verify(strTypedmin0, min0, "string0");
verify(strObjmax0, max0, "string0");
verify(strObjmin0, min0, "string0");



////------------------char ---------------------
apInitScenario (" to char");
//var char0 : char;
//char0 = char(empty);  //will get runtime exception
//verify(char0, 0, "char0");


var charTypedmid0		: char = char(mid0);

/*
var charTypedmax0		: char = char(max0);
var charTypedzero 		: char = char(zero);
var charTypedmin0		: char = char(min0);
var charObjmax0	: 	Object = char(max0);
var charObjmid0	: 	Object = char(mid0);
var charObjzero : 	Object = char(zero);
var charObjmin0	: 	Object = char(min0);

// var charlit	: char = char("4214"); //type mismatch error.

verify(charTypedmid0, mid0, "char0");
verify(charTypedzero, zero, "char0");
verify(charTypedmin0, min0, "char0");
//verify(charTypedmax0, max0, "char0");
verify(charObjmax0, max0, "char0");
verify(charObjmin0, min0, "char0");
verify(charObjmid0, mid0, "char0");
verify(charObjzero, zero, "char0");
*/


///----------boolean----------------
apInitScenario (" to boolean");
var boolean0 : boolean;
boolean0 = boolean(empty);
verify(boolean0, true, "boolean0");


var booleanTypedTrue     : boolean    =    boolean(max0);
var booleanTypedFalse    : boolean    =    boolean(min0);
var booleanObjTrue       : Object     =    boolean(mid0);
var booleanObjFalse      : Object     =    boolean(empty);
var booleanTypedTrue1     : Boolean    =    Boolean(max0);
var booleanTypedFalse1   : Boolean    =    Boolean(zero);
var booleanObjTrue1       : Object     =    Boolean(max0);
var booleanObjFalse1      : Object     =    Boolean(empty);


verify(booleanTypedTrue, true, "boolean");
verify(booleanTypedFalse,true, "boolean");
verify(booleanObjTrue, true, "boolean");
verify(booleanObjFalse, true, "boolean");
verify(booleanTypedTrue1, true, "boolean");
verify(booleanTypedFalse1, true, "boolean");
verify(booleanObjTrue1, true, "boolean");
verify(booleanObjFalse1, true, "boolean");



///-------------------sbyte-----------------
apInitScenario (" to sbyte");
var sbyte0 :sbyte;
sbyte0 = sbyte(empty);  
verify(sbyte0, 0, "sbyte0");


var sbyteTypedZero       : sbyte      =      sbyte(zero);
var sbyteTypedmid0        : sbyte      =      sbyte(mid0);
var sbyteTypedmax0      : sbyte      =      sbyte(max0);
var sbyteTypedmin0   : sbyte      =      sbyte(min0);
var sbyteObjZero         : Object     =      sbyte(0);
var sbyteObjmid0          : Object     =      sbyte(mid0);
var sbyteObjmax0        : Object     =      sbyte(max0);
var sbyteObjmin0     : Object     =      sbyte(min0);
verify(sbyteTypedZero, 0, "sbyte0");
verify(sbyteTypedmid0, 0, "sbyte0");
verify(sbyteTypedmax0, -46, "sbyte0");
verify(sbyteTypedmin0, 46, "sbyte0");
verify(sbyteObjZero, 0, "sbyte0");
verify(sbyteObjmid0, 0, "sbyte0");
verify(sbyteObjmax0, -46, "sbyte0");
verify(sbyteObjmin0, 46, "sbyte0");




////-------------byte ----------------------------
apInitScenario (" to byte");
var byte0 : byte = byte(empty);
verify(byte0, 0, "byte0");

var byteTypedZero        : byte       =       byte(zero);
var byteTypedmid0         : byte       =       byte(mid0);
var byteTypedmax0       : byte       =       byte(max0);
var byteTypedmin0		: byte =	byte(min0);
var byteObjZero          : Object     =       byte(zero);
var byteObjmid0           : Object     =       byte(mid0);
var byteObjmax0         : Object     =       byte(max0);
var byteObjmin0		: Object = byte(min0);
verify(byteTypedZero, 0, "byte0");
verify(byteTypedmid0, 0, "byte0");
verify(byteTypedmax0, 210, "byte0");
verify(byteTypedmin0, 46, "byte0");
verify(byteObjZero, 0, "byte0");
verify(byteObjmid0, 0, "byte0");
verify(byteObjmax0, 210, "byte0");
verify(byteObjmin0, 46, "byte0");

///--------------short---------------------------------
apInitScenario (" to short");
var short0 : short 

short0= short(empty); //bug 321016
verify(short0, 0, "short0");

var shortTypedZero       : short      =      short(zero);
var shortTypedmid0        : short      =      short(mid0);
var shortTypedmax0      : short      =      short(max0);
var shortTypedmin0   : short      =      short(min0);
var shortObjZero         : Object     =      short(zero);
var shortObjmid0          : Object     =      short(mid0);
var shortObjmax0        : Object     =      short(max0);
var shortObjmin0     : Object     =      short(min0);
verify(shortTypedZero, 0, "short0");
verify(shortTypedmid0, 0, "short0");
verify(shortTypedmax0, 722, "short0");
verify(shortTypedmin0, -722, "short0");
verify(shortObjZero, 0, "short0");
verify(shortObjmid0, 0, "short0");
verify(shortObjmax0, 722, "short0");
verify(shortObjmin0, -722, "short0");


////------------ushort------------------------------
apInitScenario (" to ushort");
var ushort0: ushort = ushort(empty);
verify(ushort0, 0, "short0");


var ushortTypedZero      : ushort     =     ushort(zero);
var ushortTypedmid0       : ushort     =     ushort(mid0);
var ushortTypedmax0     : ushort     =     ushort(max0);
var ushortTypedmin0     : ushort     =     ushort(min0);
var ushortObjZero        : Object     =     ushort(0);
var ushortObjmid0         : Object     =     ushort(mid0);
var ushortObjmax0       : Object     =     ushort(max0);
var ushortObjmin0       : Object     =     ushort(min0);
verify(ushortTypedZero, 0, "ushort0");
verify(ushortTypedmid0,  0, "ushort0");
verify(ushortTypedmax0, 722, "ushort0");
verify(ushortTypedmin0, 64814, "ushort0");
verify(ushortObjZero, 0, "ushort0");
verify(ushortObjmid0, 0, "ushort0");
verify(ushortObjmax0, 722, "usuhort0");
verify(ushortObjmin0, 64814, "usuhort0");



////----------------------float-------------------
apInitScenario ("to float")
var float0:float = float(empty);
verify(float0, NaN, "float0");

var floatTypedZero      : float     =     float(zero);
var floatTypedmid0       : float     =     float(mid0);
var floatTypedmax0     : float     =     float(max0);
var floatTypedmin0  : float     =     float(min0);
var floatObjZero        : Object     =     float(zero);
var floatObjmid0         : Object     =     float(mid0);
var floatObjmax0       : Object     =     float(max0);
var floatObjmin0    : Object     =     float(min0);
verify(floatTypedZero, -0.8899999856948853, "float0");
verify(floatTypedmid0, NaN, "float0");
verify(floatTypedmax0, max0, "float0");
verify(floatTypedmin0, min0, "float0");
verify(floatObjZero, -0.8899999856948853, "float0");
verify(floatObjmid0, NaN, "float0");
verify(floatObjmax0, max0, "float0");
verify(floatObjmin0, min0, "float0");


/////--------------------double ---------------------
apInitScenario (" to double");
var double0: double = double (empty);
verify(double0, NaN,"double0");

var doubleTypedZero      : double     =     double(zero);
var doubleTypedmid0       : double     =     double(mid0);
var doubleTypedmax0     : double     =     double(max0);
var doubleTypedmin0  : double     =     double(min0);
var doubleObjZero        : Object     =     double(zero);
var doubleObjmid0         : Object     =     double(mid0);
var doubleObjmax0       : Object     =     double(max0);
var doubleObjmin0    : Object     =     double(min0);
verify(doubleTypedZero, -0.89 ,"double0");
verify(doubleTypedmid0, NaN ,"double0");
verify(doubleTypedmax0, max0 ,"double0");
verify(doubleTypedmin0, min0 ,"double0");
verify(doubleObjZero, -0.89 ,"double0");
verify(doubleObjmid0, NaN ,"double0");
verify(doubleObjmax0, max0 ,"double0");
verify(doubleObjmin0, min0 ,"double0");


///--------------------Number------------------------
apInitScenario ("to Number")
var number0: Number = Number(empty);
verify(number0, NaN , "number0");

var numberTypedZero      : Number     =     Number(zero);
var numberTypedmid0       : Number     =     Number(mid0);
var numberTypedmax0     : Number     =     Number(max0);
var numberTypedmin0  : Number     =     Number(min0);
var numberObjZero        : Object     =     Number(zero);
var numberObjmid0         : Object     =     Number(mid0);
var numberObjmax0       : Object     =     Number(max0);
var numberObjmin0    : Object     =     Number(min0);
verify(numberTypedZero, -0.89 , "number0");
verify(numberTypedmid0, NaN, "number0");
verify(numberTypedmax0, max0 , "number0");
verify(numberTypedmin0, min0 , "number0");
verify(numberObjZero, -0.89 , "number0");
verify(numberObjmid0, NaN, "number0");
verify(numberObjmax0, max0 , "number0");
verify(numberObjmin0, min0 , "number0");


var numTypedZero      : Number     = new     Number(zero);
var numTypedmid0       : Number     = new     Number(mid0);
var numTypedmax0     : Number     = new     Number(max0);
var numTypedmin0  : Number     = new     Number(min0);
var numObjZero        : Object     = new     Number(zero);
var numObjmid0         : Object     = new     Number(mid0);
var numObjmax0       : Object     = new     Number(max0);
var numObjmin0    : Object     = new     Number(min0);
verify(numTypedZero, -0.89  , "num 0");
verify(numTypedmid0, NaN , "num 0");
verify(numTypedmax0, max0 , "num 0");
verify(numTypedmin0, min0 , "num 0");
verify(numObjZero, -0.89 , "num 0");
verify(numObjmid0, NaN, "num 0");
verify(numObjmax0, max0 , "num 0");
verify(numObjmin0, min0 , "num 0");


///---------------decimal------------------------------
apInitScenario (" to decimal");
/*
var decimal0 : decimal = decimal(empty);
verify(decimal0, 0, "decimal0");

var decimalTypedZero     : decimal    =    decimal(zero);
var decimalTypedmid0      : decimal    =    decimal(mid0);
var decimalTypedmax0    : decimal    =    decimal(max0);
var decimalTypedmin0 : decimal    =    decimal(min0);
var decimalObjZero       : Object     =    decimal(zero);
var decimalObjmid0        : Object     =    decimal(mid0);
var decimalObjmax0      : Object     =    decimal(max0);
var decimalObjmin0   : Object     =    decimal(min0);
verify(decimalTypedZero, 0, "decimal0");
verify(decimalTypedmid0, 32767, "decimal0");
verify(decimalTypedmax0, 65535, "decimal0");
verify(decimalTypedmin0, min0, "decimal0");
verify(decimalObjZero, 0, "decimal0");
verify(decimalObjmid0, 32767, "decimal0");
verify(decimalObjmax0, 65535, "decimal0");
verify(decimalObjmin0, min0, "decimal0");

var deciobjTypedZero     : decimal    = new    decimal(zero);
var deciobjTypedmid0      : decimal    = new    decimal(mid0);
var deciobjTypedmax0    : decimal    = new    decimal(max0);
var deciobjTypedmin0 : decimal    = new    decimal(min0);
var deciobjObjZero       : Object     = new    decimal(zero);
var deciobjObjmid0        : Object     = new    decimal(mid0);
var deciobjObjmax0      : Object     = new    decimal(max0);
var deciobjObjmin0   : Object     = new    decimal(min0);
verify(deciobjTypedZero, 0, "decimal0");
verify(deciobjTypedmid0, 32767, "decimal0");
verify(deciobjTypedmax0, 65535, "decimal0");
verify(deciobjTypedmin0, min0, "decimal0");
verify(deciobjObjZero, 0, "decimal0");
verify(deciobjObjmid0, 32767, "decimal0");
verify(deciobjObjmax0, 65535, "decimal0");
verify(deciobjObjmin0, min0, "decimal0");
*/




apEndTest();

}



str2types();


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

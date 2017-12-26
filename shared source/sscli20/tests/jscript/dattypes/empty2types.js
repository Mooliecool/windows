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


var iTestID = 228582;

/* -------------------------------------------------------------------------
  Test: 	empty2types
   
  Component:	JScript
 
  Test Area:	Type Conversion
 
 ---------------------------------------------------------------------------
  
 ---------------------------------------------------------------------------
  
 
	[00]	11-Dec-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/


function verify(sAct, sExp, sMes){
	if ( isNaN(sAct) ) 
	{
		if (isNaN(sAct) != isNaN(sExp) )
        		apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, "");
	}
	else if (sAct != sExp){
		apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, "");
	}
}


function empty2types() {

 apInitTest("empty2types"); 

var empty               ;

///----------boolean----------------
apInitScenario (" to boolean");
var boolean0 : boolean;
boolean0 = boolean(empty);
verify(boolean0, false, "boolean0");


var booleanTypedTrue     : boolean    =    boolean(true);
var booleanTypedFalse    : boolean    =    boolean(false);
var booleanObjTrue       : Object     =    boolean(true);
var booleanObjFalse      : Object     =    boolean(false);
var booleanTypedTrue1     : Boolean    =    Boolean(true);
var booleanTypedFalse1   : Boolean    =    Boolean(false);
var booleanObjTrue1       : Object     =    Boolean(true);
var booleanObjFalse1      : Object     =    Boolean(false);


verify(booleanTypedTrue, true, "boolean");
verify(booleanTypedFalse, false, "boolean");
verify(booleanObjTrue, true, "boolean");
verify(booleanObjFalse, false, "boolean");
verify(booleanTypedTrue1, true, "boolean");
verify(booleanTypedFalse1, false, "boolean");
verify(booleanObjTrue1, true, "boolean");
verify(booleanObjFalse1, false, "boolean");

////------------------char ---------------------
apInitScenario (" to char");
var char0 : char;
char0 = char(empty);
verify(char0, 0, "char0");

var charTypedOne		: char = char(1);
var charTypedc		: char = char('c');
var charObjTwo	: 	Object = char(2);
var charObjc	: 	Object = char('c');
verify(charTypedOne, 1, "char0");
verify(charTypedc, 'c', "char0");
verify(charObjTwo, 2, "char0");
verify(charObjc, 'c', "char0");

///-------------------sbyte-----------------
apInitScenario (" to sbyte");
var sbyte0 :sbyte;
sbyte0 = sbyte(empty);
verify(sbyte0, 0, "sbyte0");


var sbyteTypedZero       : sbyte      =      sbyte(0);
var sbyteTypedOne        : sbyte      =      sbyte(1);
var sbyteTypedFifty      : sbyte      =      sbyte(50);
var sbyteTypedNegFifty   : sbyte      =      sbyte(-50);
var sbyteObjZero         : Object     =      sbyte(0);
var sbyteObjOne          : Object     =      sbyte(1);
var sbyteObjFifty        : Object     =      sbyte(50);
var sbyteObjNegFifty     : Object     =      sbyte(-50);
verify(sbyteTypedZero, 0, "sbyte0");
verify(sbyteTypedOne, 1, "sbyte0");
verify(sbyteTypedFifty, 50, "sbyte0");
verify(sbyteTypedNegFifty, -50, "sbyte0");
verify(sbyteObjZero, 0, "sbyte0");
verify(sbyteObjOne, 1, "sbyte0");
verify(sbyteObjFifty, 50, "sbyte0");
verify(sbyteObjNegFifty, -50, "sbyte0");


////-------------byte ----------------------------
apInitScenario (" to byte");
var byte0 : byte = byte(empty);
verify(byte0, 0, "byte0");

var byteTypedZero        : byte       =       byte(0);
var byteTypedOne         : byte       =       byte(1);
var byteTypedForty       : byte       =       byte(40);
var byteObjZero          : Object     =       byte(0);
var byteObjOne           : Object     =       byte(1);
var byteObjForty         : Object     =       byte(40);
verify(byteTypedZero, 0, "byte0");
verify(byteTypedOne, 1, "byte0");
verify(byteTypedForty, 40, "byte0");
verify(byteObjZero, 0, "byte0");
verify(byteObjOne, 1, "byte0");
verify(byteObjForty, 40, "byte0");


///--------------short---------------------------------
apInitScenario (" to short");
var short0 : short = short(empty);
verify(short0, 0, "short0");

var shortTypedZero       : short      =      short(0);
var shortTypedOne        : short      =      short(1);
var shortTypedFifty      : short      =      short(50);
var shortTypedNegFifty   : short      =      short(-50);
var shortObjZero         : Object     =      short(0);
var shortObjOne          : Object     =      short(1);
var shortObjFifty        : Object     =      short(50);
var shortObjNegFifty     : Object     =      short(-50);
verify(shortTypedZero, 0, "short0");
verify(shortTypedOne, 1, "short0");
verify(shortTypedFifty, 50, "short0");
verify(shortTypedNegFifty, -50, "short0");
verify(shortObjZero, 0, "short0");
verify(shortObjOne, 1, "short0");
verify(shortObjFifty, 50, "short0");
verify(shortObjNegFifty, -50, "short0");


////------------ushort------------------------------
apInitScenario (" to ushort");
var ushort0: ushort = ushort(empty);
verify(ushort0, 0, "short0");


var ushortTypedZero      : ushort     =     ushort(0);
var ushortTypedOne       : ushort     =     ushort(1);
var ushortTypedForty     : ushort     =     ushort(40);
var ushortObjZero        : Object     =     ushort(0);
var ushortObjOne         : Object     =     ushort(1);
var ushortObjForty       : Object     =     ushort(40);
verify(ushortTypedZero, 0, "short0");
verify(ushortTypedOne, 1, "short0");
verify(ushortTypedForty, 40, "short0");
verify(ushortObjZero, 0, "short0");
verify(ushortObjOne, 1, "short0");
verify(ushortObjForty, 40, "short0");

////--------------int--------------------------------
apInitScenario (" to int");
var int0 : int = int(empty);
verify(int0,0,"int0");

var intTypedZero       : int      =      int(0);
var intTypedOne        : int      =      int(1);
var intTypedFifty      : int      =      int(50);
var intTypedNegFifty   : int      =      int(-50);
var intObjZero         : Object     =      int(0);
var intObjOne          : Object     =      int(1);
var intObjFifty        : Object     =      int(50);
var intObjNegFifty     : Object     =      int(-50);
verify(intTypedZero,0,"int0");
verify(intTypedOne,1,"int0");
verify(intTypedFifty,50,"int0");
verify(intTypedNegFifty,-50,"int0");
verify(intObjZero,0,"int0");
verify(intObjOne,1,"int0");
verify(intObjFifty,50,"int0");
verify(intObjNegFifty,-50,"int0");



////--------------uint--------------------------------
apInitScenario (" to uint");
var uint0 : uint = uint(empty);
verify(uint0, 0, "uint0");

var uintTypedZero      : uint     =     uint(0);
var uintTypedOne       : uint     =     uint(1);
var uintTypedForty     : uint     =     uint(40);
var uintObjZero        : Object     =     uint(0);
var uintObjOne         : Object     =     uint(1);
var uintObjForty       : Object     =     uint(40);
verify(uintTypedZero, 0, "uint0");
verify(uintTypedOne, 1, "uint0");
verify(uintTypedForty, 40, "uint0");
verify(uintObjZero, 0, "uint0");
verify(uintObjOne, 1, "uint0");
verify(uintObjForty, 40, "uint0");



/////---------------long----------------
apInitScenario (" to long");
var long0: long = long(empty);
verify(long0,0, "long0");

var longTypedZero       : long      =      long(0);
var longTypedOne        : long      =      long(1);
var longTypedFifty      : long      =      long(50);
var longTypedNegFifty   : long      =      long(-50);
var longObjZero         : Object     =      long(0);
var longObjOne          : Object     =      long(1);
var longObjFifty        : Object     =      long(50);
var longObjNegFifty     : Object     =      long(-50);
verify(longTypedZero,0, "long0");
verify(longTypedOne,1, "long0");
verify(longTypedFifty ,50, "long0");
verify(longTypedNegFifty,-50, "long0");
verify(longObjZero,0, "long0");
verify(longObjOne,1, "long0");
verify(longObjFifty,50, "long0");
verify(longObjNegFifty,-50, "long0");


/////---------------ulong-------------------
apInitScenario ("to ulong");
var ulong0 : ulong = ulong(empty);
verify(ulong0, 0, "ulong0");


var ulongTypedZero      : ulong     =     ulong(0);
var ulongTypedOne       : ulong     =     ulong(1);
var ulongTypedForty     : ulong     =     ulong(40);
var ulongObjZero        : Object     =     ulong(0);
var ulongObjOne         : Object     =     ulong(1);
var ulongObjForty       : Object     =     ulong(40);
verify(ulongTypedZero, 0, "ulong0");
verify(ulongTypedOne, 1, "ulong0");
verify(ulongTypedForty, 40, "ulong0");
verify(ulongObjZero, 0, "ulong0");
verify(ulongObjOne, 1, "ulong0");
verify(ulongObjForty, 40, "ulong0");


////----------------------float-------------------
apInitScenario ("to float")
var float0:float = float(empty);
verify(float0, NaN, "float0");

var floatTypedZero      : float     =     float(0);
var floatTypedOne       : float     =     float(1);
var floatTypedFifty     : float     =     float(50);
var floatTypedNegFifty  : float     =     float(-50);
var floatObjZero        : Object     =     float(0);
var floatObjOne         : Object     =     float(1);
var floatObjFifty       : Object     =     float(50);
var floatObjNegFifty    : Object     =     float(-50);
verify(floatTypedZero, 0, "float0");
verify(floatTypedOne, 1, "float0");
verify(floatTypedFifty, 50, "float0");
verify(floatTypedNegFifty, -50, "float0");
verify(floatObjZero, 0, "float0");
verify(floatObjOne, 1, "float0");
verify(floatObjFifty, 50, "float0");
verify(floatObjNegFifty, -50, "float0");


/////--------------------double ---------------------
apInitScenario (" to double");

var double0: double = double (empty);
verify(double0, NaN ,"double0");

var doubleTypedZero      : double     =     double(0);
var doubleTypedOne       : double     =     double(1);
var doubleTypedFifty     : double     =     double(50);
var doubleTypedNegFifty  : double     =     double(-50);
var doubleObjZero        : Object     =     double(0);
var doubleObjOne         : Object     =     double(1);
var doubleObjFifty       : Object     =     double(50);
var doubleObjNegFifty    : Object     =     double(-50);
verify(doubleTypedZero, 0 ,"double0");
verify(doubleTypedOne, 1 ,"double0");
verify(doubleTypedFifty, 50 ,"double0");
verify(doubleTypedNegFifty, -50 ,"double0");
verify(doubleObjZero, 0 ,"double0");
verify(doubleObjOne, 1 ,"double0");
verify(doubleObjFifty, 50 ,"double0");
verify(doubleObjNegFifty, -50 ,"double0");


///--------------------Number------------------------
apInitScenario ("to Number")
var number0: Number = Number(empty);
verify(number0, NaN , "number0");

var numberTypedZero      : Number     =     Number(0);
var numberTypedOne       : Number     =     Number(1);
var numberTypedFifty     : Number     =     Number(50);
var numberTypedNegFifty  : Number     =     Number(-50);
var numberObjZero        : Object     =     Number(0);
var numberObjOne         : Object     =     Number(1);
var numberObjFifty       : Object     =     Number(50);
var numberObjNegFifty    : Object     =     Number(-50);
verify(numberTypedZero, 0 , "number0");
verify(numberTypedOne, 1 , "number0");
verify(numberTypedFifty, 50 , "number0");
verify(numberTypedNegFifty, -50 , "number0");
verify(numberObjZero, 0, "number0");
verify(numberObjOne, 1, "number0");
verify(numberObjFifty, 50 , "number0");
verify(numberObjNegFifty, -50 , "number0");


var numTypedZero      : Number     = new     Number(0);
var numTypedOne       : Number     = new     Number(1);
var numTypedFifty     : Number     = new     Number(50);
var numTypedNegFifty  : Number     = new     Number(-50);
var numObjZero        : Object     = new     Number(0);
var numObjOne         : Object     = new     Number(1);
var numObjFifty       : Object     = new     Number(50);
var numObjNegFifty    : Object     = new     Number(-50);
verify(numTypedZero, 0 , "num 0");
verify(numTypedOne, 1 , "num 0");
verify(numTypedFifty, 50 , "num 0");
verify(numTypedNegFifty, -50 , "num 0");
verify(numObjZero, 0, "num 0");
verify(numObjOne, 1, "num 0");
verify(numObjFifty, 50 , "num 0");
verify(numObjNegFifty, -50 , "num 0");


///---------------decimal------------------------------
apInitScenario (" to decimal");
var decimal0 : decimal = decimal(empty);
verify(decimal0, 0, "decimal0");

var decimalTypedZero     : decimal    =    decimal(0);
var decimalTypedOne      : decimal    =    decimal(1);
var decimalTypedFifty    : decimal    =    decimal(50);
var decimalTypedNegFifty : decimal    =    decimal(-50);
var decimalObjZero       : Object     =    decimal(0);
var decimalObjOne        : Object     =    decimal(1);
var decimalObjFifty      : Object     =    decimal(50);
var decimalObjNegFifty   : Object     =    decimal(-50);
verify(decimalTypedZero, 0, "decimal0");
verify(decimalTypedOne, 1, "decimal0");
verify(decimalTypedFifty, 50, "decimal0");
verify(decimalTypedNegFifty, -50, "decimal0");
verify(decimalObjZero, 0, "decimal0");
verify(decimalObjOne, 1, "decimal0");
verify(decimalObjFifty, 50, "decimal0");
verify(decimalObjNegFifty, -50, "decimal0");

var deciobjTypedZero     : decimal    = new    decimal(0);
var deciobjTypedOne      : decimal    = new    decimal(1);
var deciobjTypedFifty    : decimal    = new    decimal(50);
var deciobjTypedNegFifty : decimal    = new    decimal(-50);
var deciobjObjZero       : Object     = new    decimal(0);
var deciobjObjOne        : Object     = new    decimal(1);
var deciobjObjFifty      : Object     = new    decimal(50);
var deciobjObjNegFifty   : Object     = new    decimal(-50);
verify(deciobjTypedZero, 0, "decimal0");
verify(deciobjTypedOne, 1, "decimal0");
verify(deciobjTypedFifty, 50, "decimal0");
verify(deciobjTypedNegFifty, -50, "decimal0");
verify(deciobjObjZero, 0, "decimal0");
verify(deciobjObjOne, 1, "decimal0");
verify(deciobjObjFifty, 50, "decimal0");
verify(deciobjObjNegFifty, -50, "decimal0");

apInitScenario (" to String" )
////----------------String---------------
var string0 : String = String(empty);
verify(string0, undefined, "string0");

var strTypedTwo          : String     =     String("2");
var strTypedc            : String     =     String("c");
var strObjTwo            : Object     =     String("2");
var strObjc              : Object     =     String("c");
verify(strTypedTwo, "2", "string0");
verify(strTypedc, "c", "string0");
verify(strObjTwo, "2", "string0");
verify(strObjc, "c", "string0");


apEndTest();

}




empty2types();


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

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


var iTestID = 228598;

/* -------------------------------------------------------------------------
  Test: 	double2types
   
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


function double2types() {

 apInitTest("double2types"); 

//var empty : double = double(null);
var empty: double = null;

var mid0: double = 10.08 ;
var max0 :double= double.MaxValue;
var min0 : double = double.MinValue;
var zero : double= 0x0000000;

print(max0);
print(min0);
print(zero);
print(empty);
print(mid0);
 
/////--------------------double ---------------------
apInitScenario (" to double");
var double0: double = double (empty);
verify(double0, empty,"double0");

var doubleTypedZero      : double     =     double(zero);
var doubleTypedmid0       : double     =     double(mid0);
var doubleTypedmax0     : double     =     double(max0);
var doubleTypedmin0  : double     =     double(min0);
var doubleObjZero        : Object     =     double(zero);
var doubleObjmid0         : Object     =     double(mid0);
var doubleObjmax0       : Object     =     double(max0);
var doubleObjmin0    : Object     =     double(min0);
verify(doubleTypedZero, zero ,"double0");
verify(doubleTypedmid0, mid0 ,"double0");
verify(doubleTypedmax0, max0 ,"double0");
verify(doubleTypedmin0, min0 ,"double0");
verify(doubleObjZero, zero ,"double0");
verify(doubleObjmid0, mid0 ,"double0");
verify(doubleObjmax0, max0 ,"double0");
verify(doubleObjmin0, min0 ,"double0");





////----------------------float-------------------
apInitScenario ("to float")
var float0:float = float(empty);
verify(float0, empty, "float0");

var floatTypedZero      : float     =     float(zero);
var floatTypedmid0       : float     =     float(mid0);
var floatTypedmax0     : float     =     float(max0);
var floatTypedmin0  : float     =     float(min0);
var floatObjZero        : Object     =     float(zero);
var floatObjmid0         : Object     =     float(mid0);
var floatObjmax0       : Object     =     float(max0);
var floatObjmin0    : Object     =     float(min0);
verify(floatTypedZero, zero, "float0");
verify(floatTypedmid0, mid0, "float0");
verify(floatTypedmax0, max0, "float0");
verify(floatTypedmin0, min0, "float0");
verify(floatObjZero, zero, "float0");
verify(floatObjmid0, mid0, "float0");
verify(floatObjmax0, max0, "float0");
verify(floatObjmin0, min0, "float0");



///----------boolean----------------
apInitScenario (" to boolean");
var boolean0 : boolean;
boolean0 = boolean(empty);
verify(boolean0, false, "boolean0");


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
verify(booleanObjFalse, false, "boolean");
verify(booleanTypedTrue1, true, "boolean");
verify(booleanTypedFalse1, false, "boolean");
verify(booleanObjTrue1, true, "boolean");
verify(booleanObjFalse1, false, "boolean");


////----------------String---------------
apInitScenario (" to String" )
var string0 : String = String(empty);
verify(string0, empty.ToString(), "string0");

var strTypedmax0          : String     =     String(max0);
var strTypedmin0            : String     =     String(min0);
var strObjmax0            : Object     =     String(max0);
var strObjmin0             : Object     =     String(min0);
verify(strTypedmax0, max0.toString(), "string0");
verify(strTypedmin0, min0.toString(), "string0");
verify(strObjmax0, max0.toString(), "string0");
verify(strObjmin0, min0.toString(), "string0");



////------------------char ---------------------
apInitScenario (" to char");
var char0 : char;
char0 = char(empty);  //will get runtime exception
verify(char0, 0, "char0");

var charTypedzero 		: char = char(zero);
var charObjzero : 	Object = char(zero);

verify(charTypedzero, '\0', "char0");
verify(charObjzero, '\0', "char0");

///-------------------sbyte-----------------
apInitScenario (" to sbyte");
var sbyte0 :sbyte;
sbyte0 = sbyte(empty);  
verify(sbyte0, 0, "sbyte0");


var sbyteTypedZero       : sbyte      =      sbyte(zero);
var sbyteObjZero         : Object     =      sbyte(0);

verify(sbyteTypedZero, 0, "sbyte0");
verify(sbyteObjZero, 0, "sbyte0");

////-------------byte ----------------------------
apInitScenario (" to byte");
var byte0 : byte = byte(empty);
verify(byte0, 0, "byte0");

var byteTypedZero        : byte       =       byte(zero);
var byteObjZero          : Object     =       byte(zero);

verify(byteTypedZero, 0, "byte0");
verify(byteObjZero, 0, "byte0");

///--------------short---------------------------------
apInitScenario (" to short");
var short0 : short 

short0= short(empty); //bug 321016
verify(short0, 0, "short0");

var shortTypedZero       : short      =      short(zero);
var shortObjZero         : Object     =      short(zero);

verify(shortTypedZero, 0, "short0");
verify(shortObjZero, zero, "short0");

////------------ushort------------------------------
apInitScenario (" to ushort");
var ushort0: ushort = ushort(empty);
verify(ushort0, 0, "short0");


var ushortTypedZero      : ushort     =     ushort(zero);
var ushortObjZero        : Object     =     ushort(0);

verify(ushortTypedZero, 0, "ushort0");
verify(ushortObjZero, 0, "ushort0");

////--------------int--------------------------------
apInitScenario (" to int");
var int0 : int = int(empty);
verify(int0,0,"int0");

var intTypedZero       : int      =      int(zero);
var intObjZero         : Object     =      int(0);

verify(intTypedZero,0,"int0");
verify(intObjZero,0,"int0");

////--------------uint--------------------------------
apInitScenario (" to uint");
var uint0 : uint = uint(empty);
verify(uint0, 0, "uint0");

var uintTypedZero      : uint     =     uint(zero);
var uintObjZero        : Object     =     uint(zero);

verify(uintTypedZero, 0, "uint0");
verify(uintObjZero, 0, "uint0");


/////---------------long----------------
apInitScenario (" to long");
var long0: long = long(empty);
//verify(long0,0, "long0");

var longTypedZero       : long      =      long(zero);
var longObjZero         : Object     =      long(zero);

verify(longTypedZero,0, "long0");
verify(longObjZero,0, "long0");

/////---------------ulong-------------------
apInitScenario ("to ulong");
var ulong0 : ulong = ulong(empty);
verify(ulong0, 0, "ulong0");

var ulongTypedZero      : ulong     =     ulong(zero);
var ulongObjZero        : Object     =     ulong(zero);

verify(ulongTypedZero, 0, "ulong0");
verify(ulongObjZero, 0, "ulong0");




///--------------------Number------------------------
apInitScenario ("to Number")
var number0: Number = Number(empty);
verify(number0, empty , "number0");

var numberTypedZero      : Number     =     Number(zero);
var numberTypedmid0       : Number     =     Number(mid0);
var numberTypedmax0     : Number     =     Number(max0);
var numberTypedmin0  : Number     =     Number(min0);
var numberObjZero        : Object     =     Number(zero);
var numberObjmid0         : Object     =     Number(mid0);
var numberObjmax0       : Object     =     Number(max0);
var numberObjmin0    : Object     =     Number(min0);
verify(numberTypedZero, zero , "number0");
verify(numberTypedmid0, mid0, "number0");
verify(numberTypedmax0, max0 , "number0");
verify(numberTypedmin0, min0 , "number0");
verify(numberObjZero, zero, "number0");
verify(numberObjmid0, mid0, "number0");
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
verify(numTypedZero, zero  , "num 0");
verify(numTypedmid0, mid0 , "num 0");
verify(numTypedmax0, max0 , "num 0");
verify(numTypedmin0, min0 , "num 0");
verify(numObjZero, zero, "num 0");
verify(numObjmid0, mid0, "num 0");
verify(numObjmax0, max0 , "num 0");
verify(numObjmin0, min0 , "num 0");



///---------------decimal------------------------------
apInitScenario (" to decimal");
var decimal0 : decimal = decimal(empty);
verify(decimal0, 0, "decimal0");

var decimalTypedZero     : decimal    =    decimal(zero);
var decimalTypedmid0      : decimal    =    decimal(mid0);
//var decimalTypedmax0    : decimal    =    decimal(max0);
//var decimalTypedmin0 : decimal    =    decimal(min0);
var decimalObjZero       : Object     =    decimal(zero);
var decimalObjmid0        : Object     =    decimal(mid0);
//var decimalObjmax0      : Object     =    decimal(max0);
//var decimalObjmin0   : Object     =    decimal(min0);

verify(decimalTypedZero, zero, "decimal0");
verify(decimalTypedmid0, 10.08, "decimal0");
//verify(decimalTypedmax0, max0, "decimal0");
//verify(decimalTypedmin0, min0, "decimal0");
verify(decimalObjZero, zero, "decimal0");
verify(decimalObjmid0, 10.08, "decimal0");
//verify(decimalObjmax0, max0, "decimal0");
//verify(decimalObjmin0, min0, "decimal0");


var deciobjTypedZero     : decimal    = new    decimal(zero);
var deciobjTypedmid0      : decimal    = new    decimal(mid0);
//var deciobjTypedmax0    : decimal    = new    decimal(max0);
//var deciobjTypedmin0 : decimal    = new    decimal(min0);
var deciobjObjZero       : Object     = new    decimal(zero);
var deciobjObjmid0        : Object     = new    decimal(mid0);
//var deciobjObjmax0      : Object     = new    decimal(max0);
//var deciobjObjmin0   : Object     = new    decimal(min0);
verify(deciobjTypedZero, 0, "decimal0");
verify(deciobjTypedmid0, 10.08, "decimal0");
//verify(deciobjTypedmax0, 65535, "decimal0");
//verify(deciobjTypedmin0, min0, "decimal0");
verify(deciobjObjZero, 0, "decimal0");
verify(deciobjObjmid0, 10.08, "decimal0");
//verify(deciobjObjmax0, 65535, "decimal0");
//verify(deciobjObjmin0, min0, "decimal0");



apEndTest();

}



double2types();


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

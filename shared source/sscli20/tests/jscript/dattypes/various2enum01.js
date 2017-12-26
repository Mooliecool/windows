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


var iTestID = 228739;

enum tint: int {
	t1
}

enum tsbyte : sbyte {
	t1
}

enum tlong: long {
	t1
}

function various2enum01 ()
{

apInitTest("various2enum01");
var sbyte0 : sbyte = sbyte.MaxValue;
var sbyte01 : sbyte = sbyte.MinValue;
var sbyte02 : sbyte = sbyte(-10);
apInitScenario ("scenario0")
if ( tsbyte(sbyte0) != sbyte.MaxValue)
apLogFailInfo("scenario tsbyte failed tsbyte(sbyte0)" ,sbyte.MaxValue,tsbyte(sbyte0),"");
apInitScenario ("scenario1")
if ( tsbyte(sbyte01) != sbyte.MinValue)
apLogFailInfo("scenario tsbyte failed tsbyte(sbyte01)" ,sbyte.MinValue,tsbyte(sbyte01),"");
apInitScenario ("scenario2")
if ( tsbyte(sbyte02) != sbyte02)
apLogFailInfo("scenario tsbyte failed tsbyte(sbyte02)" ,sbyte02,tsbyte(sbyte02),"");
apInitScenario ("scenario3")
if ( tint(sbyte0) != sbyte.MaxValue)
apLogFailInfo("scenario tint failed tint(sbyte0)" ,sbyte.MaxValue,tint(sbyte0),"");
apInitScenario ("scenario4")
if ( tint(sbyte01) != sbyte.MinValue)
apLogFailInfo("scenario tint failed tint(sbyte01)" ,sbyte.MinValue,tint(sbyte01),"");
apInitScenario ("scenario5")
if ( tint(sbyte02) != sbyte02)
apLogFailInfo("scenario tint failed tint(sbyte02)" ,sbyte02,tint(sbyte02),"");
apInitScenario ("scenario6")
if ( tlong(sbyte0) != sbyte.MaxValue)
apLogFailInfo("scenario tlong failed tlong(sbyte0)" ,sbyte.MaxValue,tlong(sbyte0),"");
apInitScenario ("scenario7")
if ( tlong(sbyte01) != sbyte.MinValue)
apLogFailInfo("scenario tlong failed tlong(sbyte01)" ,sbyte.MinValue,tlong(sbyte01),"");
apInitScenario ("scenario8")
if ( tlong(sbyte02) != sbyte02)
apLogFailInfo("scenario tlong failed tlong(sbyte02)" ,sbyte02,tlong(sbyte02),"");
var int1 : int = int.MaxValue;
var int11 : int = int.MinValue;
var int12 : int = int(-10);
apInitScenario ("scenario9")
if ( tsbyte(int1) != -1)
apLogFailInfo("scenario tsbyte failed tsbyte(int1)" ,int.MaxValue,tsbyte(int1),"");
apInitScenario ("scenario10")
if ( tsbyte(int11) != 0)
apLogFailInfo("scenario tsbyte failed tsbyte(int11)" ,int.MinValue,tsbyte(int11),"");
apInitScenario ("scenario11")
if ( tsbyte(int12) != int12)
apLogFailInfo("scenario tsbyte failed tsbyte(int12)" ,int12,tsbyte(int12),"");
apInitScenario ("scenario12")
if ( tint(int1) != int.MaxValue)
apLogFailInfo("scenario tint failed tint(int1)" ,int.MaxValue,tint(int1),"");
apInitScenario ("scenario13")
if ( tint(int11) != int.MinValue)
apLogFailInfo("scenario tint failed tint(int11)" ,int.MinValue,tint(int11),"");
apInitScenario ("scenario14")
if ( tint(int12) != int12)
apLogFailInfo("scenario tint failed tint(int12)" ,int12,tint(int12),"");
apInitScenario ("scenario15")
if ( tlong(int1) != int.MaxValue)
apLogFailInfo("scenario tlong failed tlong(int1)" ,int.MaxValue,tlong(int1),"");
apInitScenario ("scenario16")
if ( tlong(int11) != int.MinValue)
apLogFailInfo("scenario tlong failed tlong(int11)" ,int.MinValue,tlong(int11),"");
apInitScenario ("scenario17")
if ( tlong(int12) != int12)
apLogFailInfo("scenario tlong failed tlong(int12)" ,int12,tlong(int12),"");
var long2 : long = long.MaxValue;
var long21 : long = long.MinValue;
var long22 : long = long(-10);
apInitScenario ("scenario18")
if ( tsbyte(long2) != -1)
apLogFailInfo("scenario tsbyte failed tsbyte(long2)" ,-1,tsbyte(long2),"");
apInitScenario ("scenario19")
if ( tsbyte(long21) != 0)
apLogFailInfo("scenario tsbyte failed tsbyte(long21)" ,0,tsbyte(long21),"");
apInitScenario ("scenario20")
if ( tsbyte(long22) != long22)
apLogFailInfo("scenario tsbyte failed tsbyte(long22)" ,long22,tsbyte(long22),"");
apInitScenario ("scenario21")
if ( tint(long2) != -1)
apLogFailInfo("scenario tint failed tint(long2)" ,-1,tint(long2),"");
apInitScenario ("scenario22")
if ( tint(long21) != 0)
apLogFailInfo("scenario tint failed tint(long21)" ,0,tint(long21),"");
apInitScenario ("scenario23")
if ( tint(long22) != long22)
apLogFailInfo("scenario tint failed tint(long22)" ,long22,tint(long22),"");
apInitScenario ("scenario24")
if ( tlong(long2) != long.MaxValue)
apLogFailInfo("scenario tlong failed tlong(long2)" ,long.MaxValue,tlong(long2),"");
apInitScenario ("scenario25")
if ( tlong(long21) != long.MinValue)
apLogFailInfo("scenario tlong failed tlong(long21)" ,long.MinValue,tlong(long21),"");
apInitScenario ("scenario26")
if ( tlong(long22) != long22)
apLogFailInfo("scenario tlong failed tlong(long22)" ,long22,tlong(long22),"");
var float3 : float = float.MaxValue;
var float31 : float = float.MinValue;
var float32 : float = float(-10);

apInitScenario ("scenario28")
if ( tsbyte(float31) != 0)
apLogFailInfo("scenario tsbyte failed tsbyte(float31)" ,float.MinValue,tsbyte(float31),"");
apInitScenario ("scenario29")
if ( tsbyte(float32) != float32)
apLogFailInfo("scenario tsbyte failed tsbyte(float32)" ,float32,tsbyte(float32),"");
apInitScenario ("scenario30")
// float can not be converted to int&long type's enum but sbyte's enum
/*
if ( tint(float3) != float.MaxValue)
apLogFailInfo("scenario tint failed tint(float3)" ,float.MaxValue,tint(float3),"");
apInitScenario ("scenario31")
if ( tint(float31) != float.MinValue)
apLogFailInfo("scenario tint failed tint(float31)" ,float.MinValue,tint(float31),"");
apInitScenario ("scenario32")
if ( tint(float32) != float32)
apLogFailInfo("scenario tint failed tint(float32)" ,float32,tint(float32),"");
apInitScenario ("scenario33")
if ( tlong(float3) != float.MaxValue)
apLogFailInfo("scenario tlong failed tlong(float3)" ,float.MaxValue,tlong(float3),"");
apInitScenario ("scenario34")
if ( tlong(float31) != float.MinValue)
apLogFailInfo("scenario tlong failed tlong(float31)" ,float.MinValue,tlong(float31),"");
apInitScenario ("scenario35")
if ( tlong(float32) != float32)
apLogFailInfo("scenario tlong failed tlong(float32)" ,float32,tlong(float32),"");
*/
var double4 : double = double.MaxValue;
var double41 : double = double.MinValue;
var double42 : double = double(-10);
apInitScenario ("scenario37")
if ( tsbyte(double41) != 0)
apLogFailInfo("scenario tsbyte failed tsbyte(double41)" ,double.MinValue,tsbyte(double41),"")
apInitScenario ("scenario38")
if ( tsbyte(double42) != double42)
apLogFailInfo("scenario tsbyte failed tsbyte(double42)" ,double42,tsbyte(double42),"");
apInitScenario ("scenario41")
if ( tint(double42) != double42)
apLogFailInfo("scenario tint failed tint(double42)" ,double42,tint(double42),"");
apInitScenario ("scenario42")
// double can not be converted to long type's enum but int& sbyte
/*
if ( tlong(double4) != double.MaxValue)
apLogFailInfo("scenario tlong failed tlong(double4)" ,double.MaxValue,tlong(double4),"");
apInitScenario ("scenario43")
if ( tlong(double41) != double.MinValue)
apLogFailInfo("scenario tlong failed tlong(double41)" ,double.MinValue,tlong(double41),"");
apInitScenario ("scenario44")
if ( tlong(double42) != double42)
apLogFailInfo("scenario tlong failed tlong(double42)" ,double42,tlong(double42),"");
*/


}



various2enum01();


if(lFailCount > 0) System.Environment.ExitCode = 1;
else System.Environment.ExitCode = 0;

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

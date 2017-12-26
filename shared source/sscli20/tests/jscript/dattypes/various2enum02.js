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


var iTestID = 228744;

enum tuint: uint {
	t1
}

enum tbyte : byte {
	t1
}

enum tulong: ulong {
	t1
}

function various2enum02 ()
{
apInitTest("various2enum02");
var byte0 : byte = byte.MaxValue;
var byte01 : byte = byte.MinValue;
var byte02 : byte = byte(-10);
apInitScenario ("scenario0")
if ( tbyte(byte0) != byte.MaxValue)
apLogFailInfo("scenario tbyte failed tbyte(byte0)" ,byte.MaxValue,tbyte(byte0),"");
apInitScenario ("scenario1")
if ( tbyte(byte01) != byte.MinValue)
apLogFailInfo("scenario tbyte failed tbyte(byte01)" ,byte.MinValue,tbyte(byte01),"");
apInitScenario ("scenario2")
if ( tbyte(byte02) != byte02)
apLogFailInfo("scenario tbyte failed tbyte(byte02)" ,byte02,tbyte(byte02),"");
apInitScenario ("scenario3")
if ( tuint(byte0) != byte.MaxValue)
apLogFailInfo("scenario tuint failed tuint(byte0)" ,byte.MaxValue,tuint(byte0),"");
apInitScenario ("scenario4")
if ( tuint(byte01) != byte.MinValue)
apLogFailInfo("scenario tuint failed tuint(byte01)" ,byte.MinValue,tuint(byte01),"");
apInitScenario ("scenario5")
if ( tuint(byte02) != byte02)
apLogFailInfo("scenario tuint failed tuint(byte02)" ,byte02,tuint(byte02),"");
apInitScenario ("scenario6")
if ( tulong(byte0) != byte.MaxValue)
apLogFailInfo("scenario tulong failed tulong(byte0)" ,byte.MaxValue,tulong(byte0),"");
apInitScenario ("scenario7")
if ( tulong(byte01) != byte.MinValue)
apLogFailInfo("scenario tulong failed tulong(byte01)" ,byte.MinValue,tulong(byte01),"");
apInitScenario ("scenario8")
if ( tulong(byte02) != byte02)
apLogFailInfo("scenario tulong failed tulong(byte02)" ,byte02,tulong(byte02),"");
var uint1 : uint = uint.MaxValue;
var uint11 : uint = uint.MinValue;
var uint12 : uint = uint(-10);
apInitScenario ("scenario9")
if ( tbyte(uint1) != byte.MaxValue)
apLogFailInfo("scenario tbyte failed tbyte(uint1)" ,byte.MaxValue,tbyte(uint1),"");
apInitScenario ("scenario10")
if ( tbyte(uint11) != uint.MinValue)
apLogFailInfo("scenario tbyte failed tbyte(uint11)" ,uint.MinValue,tbyte(uint11),"");
apInitScenario ("scenario11")
if ( tbyte(uint12) != (byte.MaxValue - 9)) //uint12 
apLogFailInfo("scenario tbyte failed tbyte(uint12)" ,byte.MaxValue-9,tbyte(uint12),"");
apInitScenario ("scenario12")
if ( tuint(uint1) != uint.MaxValue)
apLogFailInfo("scenario tuint failed tuint(uint1)" ,uint.MaxValue,tuint(uint1),"");
apInitScenario ("scenario13")
if ( tuint(uint11) != uint.MinValue)
apLogFailInfo("scenario tuint failed tuint(uint11)" ,uint.MinValue,tuint(uint11),"");
apInitScenario ("scenario14")
if ( tuint(uint12) != uint12)
apLogFailInfo("scenario tuint failed tuint(uint12)" ,uint12,tuint(uint12),"");
apInitScenario ("scenario15")
if ( tulong(uint1) != uint.MaxValue)
apLogFailInfo("scenario tulong failed tulong(uint1)" ,uint.MaxValue,tulong(uint1),"");
apInitScenario ("scenario16")
if ( tulong(uint11) != uint.MinValue)
apLogFailInfo("scenario tulong failed tulong(uint11)" ,uint.MinValue,tulong(uint11),"");
apInitScenario ("scenario17")
if ( tulong(uint12) != uint12)
apLogFailInfo("scenario tulong failed tulong(uint12)" ,uint12,tulong(uint12),"");
var ulong2 : ulong = ulong.MaxValue;
var ulong21 : ulong = ulong.MinValue;
var ulong22 : ulong = ulong(-10);
apInitScenario ("scenario18")
if ( tbyte(ulong2) != byte.MaxValue)
apLogFailInfo("scenario tbyte failed tbyte(ulong2)" ,byte.MaxValue,tbyte(ulong2),"");
apInitScenario ("scenario19")
if ( tbyte(ulong21) != ulong.MinValue)
apLogFailInfo("scenario tbyte failed tbyte(ulong21)" ,ulong.MinValue,tbyte(ulong21),"");
apInitScenario ("scenario20")
if ( tbyte(ulong22) != (byte.MaxValue -9))
apLogFailInfo("scenario tbyte failed tbyte(ulong22)" ,byte.MaxValue-9,tbyte(ulong22),"");
apInitScenario ("scenario21")
if ( tuint(ulong2) != uint.MaxValue)
apLogFailInfo("scenario tuint failed tuint(ulong2)" ,uint.MaxValue,tuint(ulong2),"");
apInitScenario ("scenario22")
if ( tuint(ulong21) != ulong.MinValue)
apLogFailInfo("scenario tuint failed tuint(ulong21)" ,ulong.MinValue,tuint(ulong21),"");
apInitScenario ("scenario23")
if ( tuint(ulong22) != (uint.MaxValue - 9))
apLogFailInfo("scenario tuint failed tuint(ulong22)" ,uint.MaxValue-9,tuint(ulong22),"");
apInitScenario ("scenario24")
if ( tulong(ulong2) != ulong.MaxValue)
apLogFailInfo("scenario tulong failed tulong(ulong2)" ,ulong.MaxValue,tulong(ulong2),"");
apInitScenario ("scenario25")
if ( tulong(ulong21) != ulong.MinValue)
apLogFailInfo("scenario tulong failed tulong(ulong21)" ,ulong.MinValue,tulong(ulong21),"");
apInitScenario ("scenario26")
if ( tulong(ulong22) != ulong22)
apLogFailInfo("scenario tulong failed tulong(ulong22)" ,ulong22,tulong(ulong22),"");

}



various2enum02();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
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

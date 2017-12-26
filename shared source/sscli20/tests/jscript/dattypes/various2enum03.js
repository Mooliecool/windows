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


var iTestID = 228745;

enum tuint: uint {
	t1
}

enum tbyte : byte {
	t1
}

enum tulong: ulong {
	t1
}

function various2enum03 ()
{
apInitTest("various2enum03");
var float0 : float = float.MaxValue;
var float01 : float = float.MinValue;
var float02 : float = float(-10);

apInitScenario ("scenario1")
if ( tbyte(float01) != 0)
apLogFailInfo("scenario tbyte failed tbyte(float01)" ,0,tbyte(float01),"");
apInitScenario ("scenario2")
if ( tbyte(float02) != (byte.MaxValue-9))
apLogFailInfo("scenario tbyte failed tbyte(float02)" ,byte.MaxValue-9,tbyte(float02),"");
apInitScenario ("scenario3")
/*
if ( tuint(float0) != float.MaxValue)
apLogFailInfo("scenario tuint failed tuint(float0)" ,float.MaxValue,tuint(float0),"");
apInitScenario ("scenario4")
if ( tuint(float01) != float.MinValue)
apLogFailInfo("scenario tuint failed tuint(float01)" ,float.MinValue,tuint(float01),"");
apInitScenario ("scenario5")
if ( tuint(float02) != float02)
apLogFailInfo("scenario tuint failed tuint(float02)" ,float02,tuint(float02),"");
apInitScenario ("scenario6")
if ( tulong(float0) != float.MaxValue)
apLogFailInfo("scenario tulong failed tulong(float0)" ,float.MaxValue,tulong(float0),"");
apInitScenario ("scenario7")
if ( tulong(float01) != float.MinValue)
apLogFailInfo("scenario tulong failed tulong(float01)" ,float.MinValue,tulong(float01),"");
apInitScenario ("scenario8")
if ( tulong(float02) != float02)
apLogFailInfo("scenario tulong failed tulong(float02)" ,float02,tulong(float02),"");
*/
var double1 : double = double.MaxValue;
var double11 : double = double.MinValue;
var double12 : double = double(-10);
apInitScenario ("scenario10")
if ( tbyte(double11) != 0)
apLogFailInfo("scenario tbyte failed tbyte(double11)" ,0,tbyte(double11),"");
apInitScenario ("scenario11")
if ( tbyte(double12) != (byte.MaxValue -9))
apLogFailInfo("scenario tbyte failed tbyte(double12)" ,byte.MaxValue-9,tbyte(double12),"");

apInitScenario ("scenario14")
if ( tuint(double12) != (uint.MaxValue-9))
apLogFailInfo("scenario tuint failed tuint(double12)" ,uint.MaxValue-9,tuint(double12),"");
apInitScenario ("scenario15")
/*
if ( tulong(double1) != double.MaxValue)
apLogFailInfo("scenario tulong failed tulong(double1)" ,double.MaxValue,tulong(double1),"");
apInitScenario ("scenario16")
if ( tulong(double11) != double.MinValue)
apLogFailInfo("scenario tulong failed tulong(double11)" ,double.MinValue,tulong(double11),"");
apInitScenario ("scenario17")
if ( tulong(double12) != double12)
apLogFailInfo("scenario tulong failed tulong(double12)" ,double12,tulong(double12),"");
*/

}



various2enum03();


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

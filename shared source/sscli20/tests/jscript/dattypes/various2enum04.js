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


var iTestID = 228746;



enum tuint: uint {
	t1
}

enum tbyte: byte {
	t1
}

enum tulong: ulong {
	t1
}

function various2enum04 ()
{
apInitTest("various2enum04");
var int1 : int = int.MaxValue;
var int11 : int = int.MinValue;
var int12 : int = int(-10);
apInitScenario ("scenario9")
if ( tbyte(int1) != byte.MaxValue)
apLogFailInfo("scenario tbyte failed tbyte(int1)" ,byte.MaxValue,tbyte(int1),"");
apInitScenario ("scenario10")
if ( tbyte(int11) != byte.MinValue)
apLogFailInfo("scenario tbyte failed tbyte(int11)" ,byte.MinValue,tbyte(int11),"");
apInitScenario ("scenario11")
if ( tbyte(int12) != (byte.MaxValue -9))
apLogFailInfo("scenario tbyte failed tbyte(int12)" ,byte.MaxValue-9,tbyte(int12),"");
apInitScenario ("scenario12")
/* int-tuint, long
if ( tuint(int1) != int.MaxValue)
apLogFailInfo("scenario tuint failed tuint(int1)" ,int.MaxValue,tuint(int1),"");
apInitScenario ("scenario13")
if ( tuint(int11) != int.MinValue)
apLogFailInfo("scenario tuint failed tuint(int11)" ,int.MinValue,tuint(int11),"");
apInitScenario ("scenario14")
if ( tuint(int12) != int12)
apLogFailInfo("scenario tuint failed tuint(int12)" ,int12,tuint(int12),"");
apInitScenario ("scenario15")
if ( tulong(int1) != int.MaxValue)
apLogFailInfo("scenario tulong failed tulong(int1)" ,int.MaxValue,tulong(int1),"");
apInitScenario ("scenario16")
if ( tulong(int11) != int.MinValue)
apLogFailInfo("scenario tulong failed tulong(int11)" ,int.MinValue,tulong(int11),"");
apInitScenario ("scenario17")
if ( tulong(int12) != int12)
apLogFailInfo("scenario tulong failed tulong(int12)" ,int12,tulong(int12),"");
*/
var long2 : long = long.MaxValue;
var long21 : long = long.MinValue;
var long22 : long = long(-10);
apInitScenario ("scenario18")
if ( tbyte(long2) != byte.MaxValue)
apLogFailInfo("scenario tbyte failed tbyte(long2)" ,byte.MaxValue,tbyte(long2),"");
apInitScenario ("scenario19")
if ( tbyte(long21) != byte.MinValue)
apLogFailInfo("scenario tbyte failed tbyte(long21)" ,byte.MinValue,tbyte(long21),"");
apInitScenario ("scenario20")
if ( tbyte(long22) != (byte.MaxValue-9))
apLogFailInfo("scenario tbyte failed tbyte(long22)" ,byte.MaxValue-9,tbyte(long22),"");
apInitScenario ("scenario21")
if ( tuint(long2) != uint.MaxValue)
apLogFailInfo("scenario tuint failed tuint(long2)" ,uint.MaxValue,tuint(long2),"");
apInitScenario ("scenario22")
if ( tuint(long21) != uint.MinValue)
apLogFailInfo("scenario tuint failed tuint(long21)" ,uint.MinValue,tuint(long21),"");
apInitScenario ("scenario23")
if ( tuint(long22) != long22)
apLogFailInfo("scenario tuint failed tuint(long22)" ,long22,tuint(long22),"");
apInitScenario ("scenario24")
/* long->ulong
if ( tulong(long2) != long.MaxValue)
apLogFailInfo("scenario tulong failed tulong(long2)" ,long.MaxValue,tulong(long2),"");
apInitScenario ("scenario25")
if ( tulong(long21) != long.MinValue)
apLogFailInfo("scenario tulong failed tulong(long21)" ,long.MinValue,tulong(long21),"");
apInitScenario ("scenario26")
if ( tulong(long22) != long22)
apLogFailInfo("scenario tulong failed tulong(long22)" ,long22,tulong(long22),"");
*/

}



various2enum04();


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

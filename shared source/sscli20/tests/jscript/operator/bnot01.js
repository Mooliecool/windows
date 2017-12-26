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


var iTestID = 230615;

///////////////////////////////////////////////////////////////////////////////////
//



//        this testcase tests the ~ operator




              


function bnot01() {
var x
var _sbyte : sbyte
var _tempsbyte : sbyte
var _byte : byte
var _tempbyte : byte
var _short : short
var _tempshort : short
var _ushort : ushort
var _tempushort : ushort
var _int : int
var _tempint : int
var _uint : uint
var _tempuint : uint
var _long : long
var _templong : long
var _ulong : ulong
var _tempulong : ulong
var _char : char
var _tempchar : char

apInitTest("bNot01")

apInitScenario("~sbyte")
_sbyte = 127
_tempsbyte = -128
if (~_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, ~_sbyte, "")

_sbyte = 127
_tempsbyte = -128
if (~_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, ~_sbyte, "")

_sbyte = -128
_tempsbyte = 127
if (~_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, ~_sbyte, "")



apInitScenario("~short")
_short = 32767
_tempshort = -32768
if (~_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, ~_short, "")

_short = 32767
_tempshort = -32768
if (~_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, ~_short, "")

_short = -32768
_tempshort = 32767
if (~_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, ~_short, "")



apInitScenario("~int")
_int = 2147483647
_tempint = -2147483648
if (~_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, ~_int, "")

_int = 2147483647
_tempint = -2147483648
if (~_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, ~_int, "")

_int = -2147483648
_tempint = 2147483647
if (~_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, ~_int, "")



apInitScenario("~uint")
_uint = 4294967295
_tempuint = 0
if (~_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, ~_uint, "")

_uint = 2147483647
_tempuint = 2147483648
if (~_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, ~_uint, "")

_uint = 0
_tempuint = 4294967295
if (~_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, ~_uint, "")



apInitScenario("~long")
_long = 9223372036854775807
_templong = -9223372036854775808
if (~_long !== _templong) apLogFailInfo("Wrong Value", _templong, ~_long, "")

_long = 9223372036854775807
_templong = -9223372036854775808
if (~_long !== _templong) apLogFailInfo("Wrong Value", _templong, ~_long, "")

_long = -9223372036854775808
_templong = 9223372036854775807
if (~_long !== _templong) apLogFailInfo("Wrong Value", _templong, ~_long, "")



apInitScenario("~ulong")
_ulong = 18446744073709551615
_tempulong = 0
if (~_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, ~_ulong, "")

_ulong = 9223372036854775807
_tempulong = 9223372036854775808
if (~_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, ~_ulong, "")

_ulong = 0
_tempulong = 18446744073709551615
if (~_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, ~_ulong, "")




  apEndTest();
}


bnot01();


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

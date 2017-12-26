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


var iTestID = 191791;

//////////////////////////////////////////////////////////////////////////////

@if(!@aspx)
	import System
@end


function absh003() {

    var booleanTyped   : Boolean = Boolean(true);
    var booleanObj     : Object  = Boolean(true);

    var sbyteTyped     : SByte   = SByte(-50);
    var sbyteObj       : Object  = SByte(-50);
    var byteTyped      : Byte    = Byte(50);
    var byteObj        : Object  = Byte(50);

    var int16Typed     : Int16   = Int16(-50);
    var int16Obj       : Object  = Int16(-50);
    var uint16Typed    : UInt16  = UInt16(50);
    var uint16Obj      : Object  = UInt16(50);

    var int32Typed     : Int32   = Int32(-50);
    var int32Obj       : Object  = Int32(-50);
    var uint32Typed    : UInt32  = UInt32(50);
    var uint32Obj      : Object  = UInt32(50);

    var int64Typed     : Int64   = Int64(-50);
    var int64Obj       : Object  = Int64(-50);
    var uint64Typed    : UInt64  = UInt64(50);
    var uint64Obj      : Object  = UInt64(50);

    var singleTyped    : Single  = Single(-50);
    var singleObj      : Object  = Single(-50);

    var doubleTyped    : Double  = Double(-50);
    var doubleObj      : Object  = Double(-50);

    var numberTyped    : Number  = Number(-50);
    var numberObj      : Object  = Number(-50);

    var numobjTyped    : Number  = new Number(-50);
    var numobjObj      : Object  = new Number(-50);

    var strTyped       : String  = String("2");
    var strObj         : Object  = String("-2");

    var charTyped      : Char    = Char("\u0032");
    var charObj        : Object  = Char("\u0032");

    var decTyped       : decimal = decimal(-50)
    var decObj         : Object  = decimal(-50)

    var ptrTyped       : IntPtr  = IntPtr(-50)
    var ptrObj         : Object  = IntPtr(-50)

    apInitTest("absh003")
    apInitScenario("Testing for compilation errors")


    //apWriteDebug("section postponed, bug:283081")
    if (Math.abs(booleanTyped) != 1) apLogFailInfo("booleanTyped failed", "", "", "")
    if (Math.abs(booleanObj) != 1) apLogFailInfo("booleanObj failed", "", "", "") 
    if (Math.abs(sbyteTyped) != 50) apLogFailInfo("sbyteTyped failed", "", "", "")
    if (Math.abs(sbyteObj) != 50) apLogFailInfo("sbyteObj failed", "", "", "")
    if (Math.abs(byteTyped) != 50) apLogFailInfo("byteTyped failed", "", "", "")
    if (Math.abs(byteObj) != 50) apLogFailInfo("byteObj failed", "", "", "")
    if (Math.abs(int16Typed) != 50) apLogFailInfo("int16Typed failed", "", "", "")
    if (Math.abs(int16Obj) != 50) apLogFailInfo("int16Obj failed", "", "", "")
    if (Math.abs(uint16Typed) != 50) apLogFailInfo("uint16Typed failed", "", "", "")
    if (Math.abs(uint16Obj) != 50) apLogFailInfo("uint16Obj failed", "", "", "")

    if (Math.abs(int32Typed) != 50) apLogFailInfo("int32Typed failed", "", "", "")
    if (Math.abs(int32Obj) != 50) apLogFailInfo("int32Obj failed", "", "", "")
    if (Math.abs(uint32Typed) != 50) apLogFailInfo("uint32Typed failed", "", "", "")
    if (Math.abs(uint32Obj) != 50) apLogFailInfo("uint32Obj failed", "", "", "")

    //apWriteDebug("section postponed, bug:281928")
    if (Math.abs(int64Typed) != 50) apLogFailInfo("int64Typed failed", "", "", "")
    if (Math.abs(int64Obj) != 50) apLogFailInfo("int64Obj failed", "", "", "")
    if (Math.abs(uint64Typed) != 50) apLogFailInfo("uint64Typed failed", "", "", "")
    if (Math.abs(uint64Obj) != 50) apLogFailInfo("uint64Obj failed", "", "", "")

    if (Math.abs(singleTyped) != 50) apLogFailInfo("singleTyped failed", "", "", "")
    if (Math.abs(singleObj) != 50) apLogFailInfo("singleObj failed", "", "", "")
    if (Math.abs(doubleTyped) != 50) apLogFailInfo("doubleTyped failed", "", "", "")
    if (Math.abs(doubleObj) != 50) apLogFailInfo("doubleObj failed", "", "", "")

    if (Math.abs(numberTyped) != 50) apLogFailInfo("numberTyped failed", "", "", "")
    if (Math.abs(numberObj) != 50) apLogFailInfo("numberObj failed", "", "", "")
    if (Math.abs(numobjTyped) != 50) apLogFailInfo("numobjTyped failed", "", "", "")
    if (Math.abs(numobjObj) != 50) apLogFailInfo("numobjObj failed", "", "", "")

    if (Math.abs(strTyped) != 2) apLogFailInfo("strTyped failed", "", "", "")
    if (Math.abs(strObj) != 2) apLogFailInfo("strObj failed", "", "", "")
    if (Math.abs(charTyped) != 50) apLogFailInfo("charTyped failed", "", "", "")
    if (Math.abs(charObj) != 50) apLogFailInfo("charObj failed", "", "", "")

    if (Math.abs(decTyped) != 50) apLogFailInfo("decTyped failed", "", "", "")
    if (Math.abs(decObj) != 50) apLogFailInfo("decObj failed", "", "", "")

    //apWriteDebug("section postponed, bug:299863")
    //if (Math.abs(ptrTyped) != 50) apLogFailInfo("ptrTyped failed", "", "", "")
    if (Math.abs(ptrObj) != 50) apLogFailInfo("ptrObj failed", "", "", "")
    //if (Math.abs(uptrTyped) != 50) apLogFailInfo("uptrTyped failed", "", "", "")
    //if (Math.abs(uptrObj) != 50) apLogFailInfo("uptrObj failed", "", "", "")

    if (!isNaN(Math.abs()))  apLogFailInfo(" failed", "", "", "")



    apEndTest();

}


absh003();


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

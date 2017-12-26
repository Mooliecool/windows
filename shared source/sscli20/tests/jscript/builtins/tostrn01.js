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


var iTestID = 53717;

function bin(num)
{
    num = parseInt(num);
    var sBin = "01".charAt(num&0x01);
    if (num>>>1) sBin = bin(num>>>1) + sBin;
    
    return sBin;
}

function oct(num)
{
    num = parseInt(num);
    var sOct = "01234567".charAt(num&0x07);
    if (num>>>3) sOct = oct(num>>>3) + sOct;
    
    return sOct;
}

function hex (num)
{
    num = parseInt(num);
    
    var sHex = "0123456789abcdef".charAt(num&0x0f);

    if (num>>>4) sHex = hex(num>>>4) + sHex;
    
    return sHex;
}

function verify(sCat, base, num, bugNum){
 @if(@_fast)
    var sAct, sExp;
 @end
    if (bugNum == null) bugNum = "";

    sAct = num.toString(base);

	switch(base){
	
		case 2:
			sExp = bin(num);
			break;
		case 8:
			sExp = oct(num);
			break;
		case 10:
			sExp = num;
			break;
		case 16:
		    sExp = hex(num);
			break;
		default:
			sExp = "Error - invalid base";
	}

    if (sAct != sExp)
        apLogFailInfo( sCat+" failed", sExp, sAct, bugNum);

}

function tostrn01() {

var base;
    apInitTest("toStrN01 ");


    //----------------------------------------------------------------------------
    apInitScenario("numbers, pos integers");

	// base 16
	base =16;	
    verify("pos min",base, 1, null);
    verify("pos min < n < VT_UI1 pos max",base, 127, null);
    verify("VT_UI1 pos max",base, 255, null);
    verify("VT_I2 pos excl-min",base, 256, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max",base, 12869, null);
    verify("VT_I2 pos max",base, 32767, null);
    verify("VT_I4 pos excl-min",base, 32768, null);
    verify("VT_I4 pos excl-min < n < pos max",base, 1143483646, null);
    verify("pos max",base, 2147483647, null);
    verify("zero",base, 0, null);
	// base 10
	base =10;	
    verify("pos min",base, 1, null);
    verify("pos min < n < VT_UI1 pos max",base, 127, null);
    verify("VT_UI1 pos max",base, 255, null);
    verify("VT_I2 pos excl-min",base, 256, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max",base, 12869, null);
    verify("VT_I2 pos max",base, 32767, null);
    verify("VT_I4 pos excl-min",base, 32768, null);
    verify("VT_I4 pos excl-min < n < pos max",base, 1143483646, null);
    verify("pos max",base, 2147483647, null);
    verify("zero",base, 0, null);
	// base 8
    base =8;	
    verify("pos min",base, 1, null);
    verify("pos min < n < VT_UI1 pos max",base, 127, null);
    verify("VT_UI1 pos max",base, 255, null);
    verify("VT_I2 pos excl-min",base, 256, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max",base, 12869, null);
    verify("VT_I2 pos max",base, 32767, null);
    verify("VT_I4 pos excl-min",base, 32768, null);
    verify("VT_I4 pos excl-min < n < pos max",base, 1143483646, null);
    verify("pos max",base, 2147483647, null);
    verify("zero",base, 0, null);
	// base 2
	base =2;	
    verify("pos min",base, 1, null);
    verify("pos min < n < VT_UI1 pos max",base, 127, null);
    verify("VT_UI1 pos max",base, 255, null);
    verify("VT_I2 pos excl-min",base, 256, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max",base, 12869, null);
    verify("VT_I2 pos max",base, 32767, null);
    verify("VT_I4 pos excl-min",base, 32768, null);
@if (@_win32)
    verify("VT_I4 pos excl-min < n < pos max",base, 1143483646, null);
    verify("pos max",base, 2147483647, null);
@else
    verify("VT_I4 pos excl-min < n < pos max",base, 114348, null);
    // Not enough stack space on win16 machines to manually compute
    //-- verify("pos max",base, 214748, null);
@end
    verify("zero",base, 0, null);

    apEndTest();

}

tostrn01();


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

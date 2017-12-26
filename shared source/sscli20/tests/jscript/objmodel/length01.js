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


var iTestID = 52899;


var gcTest = 0;
function verify(fn, argcExpect, bugNum)
{
    if (null == bugNum) bugNum = "";
    gcTest++;

    if (fn.length != argcExpect)
        apLogFailInfo( gcTest+":"+fn+" failed", argcExpect, fn.length, bugNum);
}

function length01 ()
{

    apInitTest( "length01 ");

    //--------------------------------------------------------------------
    apInitScenario( "1: Verify all Array methods' argc" );

//        verify(Array.prototype.eval, 1, null);
        verify(Array.prototype.join, 1, null);
        verify(Array.prototype.reverse, 0, null);
        verify(Array.prototype.sort, 1, null);
//        verify(Array.prototype.toString, 1, null); // maps to join
        verify(Array.prototype.toString, 0, null); 
        verify(Array.prototype.valueOf, 0, null);

//        verify(Array.eval, 1, null);
        verify(Array.toString, 0, null);


    //--------------------------------------------------------------------
    apInitScenario( "2: Verify all Boolean methods' argc" );

//        verify(Boolean.prototype.eval, 1, null);
        verify(Boolean.prototype.valueOf, 0, null);
        verify(Boolean.prototype.toString, 0, null);

//        verify(Boolean.eval, 1, null);
        verify(Boolean.toString, 0, null);


    //--------------------------------------------------------------------
    apInitScenario( "3: Verify all Date methods' argc" );

        verify(Date.prototype.getDate, 0, null);
        verify(Date.prototype.getDay, 0, null);
        verify(Date.prototype.getHours, 0, null);
        verify(Date.prototype.getMinutes, 0, null);
        verify(Date.prototype.getMonth, 0, null);
        verify(Date.prototype.getSeconds, 0, null);
        verify(Date.prototype.getTime, 0, null);
        verify(Date.prototype.getTimezoneOffset, 0, null);
        verify(Date.prototype.getYear, 0, null);
        verify(Date.prototype.setDate, 1, null);
//        verify(Date.prototype.setHours, 1, null);
//        verify(Date.prototype.setMinutes, 1, null);
//        verify(Date.prototype.setMonth, 1, null);
//        verify(Date.prototype.setSeconds, 1, null);
        verify(Date.prototype.setHours, 4, null);
        verify(Date.prototype.setMinutes, 3, null);
        verify(Date.prototype.setMonth, 2, null);
        verify(Date.prototype.setSeconds, 2, null);
        verify(Date.prototype.setTime, 1, null);
        verify(Date.prototype.setYear, 1, null);
        verify(Date.prototype.toGMTString, 0, null);
        verify(Date.prototype.toLocaleString, 0, null);
        verify(Date.prototype.toString, 0, null);

//        verify(Date.eval, 1, null);
        verify(Date.parse, 1, null);

@if (@_jscript_version < 5.5)
        verify(Date.UTC, 6, null);
@else
        verify(Date.UTC, 7, null);
@end

        verify(Date.toString, 0, null);


    //--------------------------------------------------------------------
    apInitScenario( "4: Verify all Math methods' argc" );

        verify(Math.abs, 1, null);
        verify(Math.acos, 1, null);
        verify(Math.asin, 1, null);
        verify(Math.atan, 1, null);
        verify(Math.atan2, 2, null);
        verify(Math.ceil, 1, null);
        verify(Math.cos, 1, null);
//        verify(Math.eval, 1, null);
        verify(Math.exp, 1, null);
        verify(Math.floor, 1, null);
        verify(Math.log, 1, null);
        verify(Math.max, 2, null);
        verify(Math.min, 2, null);
        verify(Math.pow, 2, null);
        verify(Math.random, 0, null);
        verify(Math.round, 1, null);
        verify(Math.sin, 1, null);
        verify(Math.sqrt, 1, null);
        verify(Math.tan, 1, null);
        verify(Math.toString, 0, null);


    //--------------------------------------------------------------------
    apInitScenario( "5: Verify all Number methods' argc" );

//        verify(Number.prototype.eval, 1, null);
        verify(Number.prototype.toString, 1, null); // radix arg
        verify(Number.prototype.valueOf, 0, null);

//        verify(Number.eval, 1, null);
        verify(Number.toString, 0, null);


    //--------------------------------------------------------------------
    apInitScenario( "6: Verify all Object methods' argc" );

//        verify(Object.prototype.eval, 1, null);
        verify(Object.prototype.toString, 0, null);
        verify(Object.prototype.valueOf, 0, null);

//        verify(Object.eval, 1, null);
        verify(Object.toString, 0, null);


    //--------------------------------------------------------------------
    apInitScenario( "7: Verify all String methods' argc" );

        verify(String.prototype.anchor, 1, null);
        verify(String.prototype.big, 0, null);
        verify(String.prototype.blink, 0, null);
        verify(String.prototype.bold, 0, null);
        verify(String.prototype.charAt, 1, null);
//        verify(String.prototype.eval, 1, null);
        verify(String.prototype.fixed, 0, null);
        verify(String.prototype.fontcolor, 1, null);
        verify(String.prototype.fontsize, 1, null);
        verify(String.prototype.indexOf, 2, null);
        verify(String.prototype.italics, 0, null);
        verify(String.prototype.lastIndexOf, 2, null);
        verify(String.prototype.link, 1, null);
        verify(String.prototype.small, 0, null);
        verify(String.prototype.split, 2, 2261);
        verify(String.prototype.strike, 0, null);
        verify(String.prototype.sub, 0, null);
        verify(String.prototype.substring, 2, null);
        verify(String.prototype.sup, 0, null);
        verify(String.prototype.toLowerCase, 0, null);
        verify(String.prototype.toUpperCase, 0, null);
//        verify(String.prototype.toString, 2, null); // maps to substring
        verify(String.prototype.toString, 0, null); 

//        verify(String.eval, 1, null);
        verify(String.toString, 0, null);


    //--------------------------------------------------------------------
    apInitScenario( "8: Verify all intrinsic functions' argc" );

//        verify(escape, 2, null);
        verify(escape, 1, null);
        verify(eval, 1, null);
        verify(Function, 1, null);
        verify(isNaN, 1, null);
        verify(parseInt, 2, null);
        verify(parseFloat, 1, null);
        verify(unescape, 1, null);

    apEndTest();

}



length01();


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

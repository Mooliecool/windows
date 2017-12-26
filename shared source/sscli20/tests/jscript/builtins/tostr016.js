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


var iTestID = 53706;



@if(!@aspx)
	function obFoo() {};
	function fnRetStr() { return "function return string"; }
@else
	expando function obFoo() {};
	expando function fnRetStr() { return "function return string"; }
@end

function verify(scen, ob, type, bugNum) {
    if (bugNum == null) bugNum = "";
    var sExp="", sAct="";

    var arr = new Object();
    arr[1] = ob; 

    arr[1].toString = fnRetStr;

    // type 1 == scaler
    if (type == 1)
        sExp = ob+"";
    else
        sExp = "function return string";

    sAct = arr[1].toString();

    if (sAct != sExp)
        apLogFailInfo( scen+" failed ", sExp, sAct, bugNum);
}

function tostr016() {
 @if(@_fast)
    var vDate;
 @end

    apInitTest( "toStr016 " );

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");
@if(!@_fast)
    verify("built-in, non-exec--Math", Math, 0, null);
@end

    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");
@if(!@_fast)
    verify("built-in, exec, not instanciated--Array", Array, 0, null);
    verify("built-in, exec, not instanciated--Boolean", Boolean, 0, null);
    verify("built-in, exec, not instanciated--Date", Date, 0, null);  
    verify("built-in, exec, not instanciated--Number", Number, 0, null);
    verify("built-in, exec, not instanciated--Object", Object, 0, null);
    verify("built-in, exec, not instanciated--String", String, 0, null);
@end

    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    verify("built-in, exec, instanciated--Array",new Array(), 0, null);
    verify("built-in, exec, instanciated--Boolean",new Boolean(), 0, null);
    verify("built-in, exec, instanciated--Date",new Date(96,0,1), 0, null);
    verify("built-in, exec, instanciated--Number",new Number(), 0, null);
    verify("built-in, exec, instanciated--Object",new Object(), 0, null);
    verify("built-in, exec, instanciated--String",new String(), 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    verify("user-defined, not instanciated",obFoo, 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated", null);

    verify("user-defined, instanciated",new obFoo(), 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    verify("number, decimal, integer--min pos",1, 1, null);
    verify("number, decimal, integer--min pos < n < max pos",1234567890, 1, null);
    verify("number, decimal, integer--max pos",2147483647, 1, null);

    verify("number, decimal, integer--max neg",-1, 1, null);
    verify("number, decimal, integer--min neg < n < max neg",-1234567890, 1, null);
    verify("number, decimal, integer--min neg",-2147483647, 1, null);

    verify("number, decimal, integer--pos zero",0, 1, null);
    verify("number, decimal, integer--neg zero",0, 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    verify("number, decimal, float--min pos < n < max pos",1.2345678e90, 1, null);
    verify("number, decimal, float-- > max pos float (1.#INF)",1.797693134862315807e309, 1, null);

    verify("number, decimal, float--min neg < n < max neg",-1.2345678e90, 1, null);
    verify("number, decimal, float-- < min neg float (-1.#INF)",-1.797693134862315807e309, 1, null);

    verify("number, decimal, float--pos zero",0.0, 1, null);
    verify("number, decimal, float--neg zero",-0.0, 1, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("8. string");

    verify("literal, string--single space"," ", 1, 16045);
    verify("literal, string--multiple spaces","                                                                   ", 1, 16045);
    verify("literal, string--as false","false",1, 16045);
    verify("literal, string--ns 0","0", 1, 16045);
    verify("literal, string--ns > 0","1234567890", 1, 16045);
    verify("literal, string--ns > 0, leading space"," 1234567890", 1, 16045);
    verify("literal, string--ns > 0, trailing space","1234567890 ", 1, 16045);
    verify("literal, string--ns < 0","-1234567890", 1, 16045);
    verify("literal, string--as single word","obFoo", 1, 16045);
    verify("literal, string--as single word, leading space"," foo", 1, 16045);
    verify("literal, string--as single word, trailing space","foo ", 1, 16045);
    verify("literal, string--as multiple word","foo bar", 1, 16045);
    verify("literal, string--as multiple word, leading space"," foo bar", 1, 16045);
    verify("literal, string--as multiple word, trailing space","foo bar ", 1, 16045);
    verify("literal, string--zls","", 1, 16045);


    //----------------------------------------------------------------------------
    apInitScenario("9. constants");

    verify("constants",true, 1, 16045);
    verify("constants",false, 1, 16045);


    //----------------------------------------------------------------------------
    apInitScenario("12. built-in functions");

    verify("built-in functions--escape", escape, 0, null);
    verify("built-in functions--eval", eval, 0, null);
    verify("built-in functions--isNaN", isNaN, 0, null);
    verify("built-in functions--parseFloat", parseFloat, 0, null);
    verify("built-in functions--parseInt", parseInt, 0, null);
    verify("built-in functions--unescape", unescape, 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("13. Math methods");

    verify("Math methods--Math.abs", Math.abs, 0, null);
    verify("Math methods--Math.acos", Math.acos, 0, null);
    verify("Math methods--Math.asin", Math.asin, 0, null);
    verify("Math methods--Math.atan", Math.atan, 0, null);
    verify("Math methods--Math.ceil", Math.ceil, 0, null);
    verify("Math methods--Math.cos", Math.cos, 0, null);
    verify("Math methods--Math.exp", Math.exp, 0, null);
    verify("Math methods--Math.floor", Math.floor, 0, null);
    verify("Math methods--Math.log", Math.log, 0, null);
    verify("Math methods--Math.max", Math.max, 0, null);
    verify("Math methods--Math.min", Math.min, 0, null);
    verify("Math methods--Math.pow", Math.pow, 0, null);
    verify("Math methods--Math.random", Math.random, 0, null);
    verify("Math methods--Math.round", Math.round, 0, null);
    verify("Math methods--Math.sin", Math.sin, 0, null);
    verify("Math methods--Math.sqrt", Math.sqrt, 0, null);
    verify("Math methods--Math.tan", Math.tan, 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("14. string methods");

    verify("string methods--anchor", "somestring".anchor, 0, null);
    verify("string methods--big", "somestring".big, 0, null);
    verify("string methods--blink", "somestring".blink, 0, null);
    verify("string methods--bold", "somestring".bold, 0, null);
    verify("string methods--charAt", "somestring".charAt, 0, null);
    verify("string methods--fixed", "somestring".fixed, 0, null);
    verify("string methods--fontcolor", "somestring".fontcolor, 0, null);
    verify("string methods--fontsize", "somestring".fontsize, 0, null);
    verify("string methods--indexOf", "somestring".indexOf, 0, null);
    verify("string methods--italics", "somestring".italics, 0, null);
    verify("string methods--lastIndexOf", "somestring".lastIndexOf, 0, null);
    verify("string methods--link", "somestring".link, 0, null);
    verify("string methods--small", "somestring".small, 0, null);
    verify("string methods--strike", "somestring".strike, 0, null);
    verify("string methods--sub", "somestring".sub, 0, null);
    verify("string methods--substring", "somestring".substring, 0, null);
    verify("string methods--sup", "somestring".sup, 0, null);
    verify("string methods--toLowerCase", "somestring".toLowerCase, 0, null);
    verify("string methods--toUpperCase", "somestring".toUpperCase, 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("15. Built-in Date methods");

    verify("Built-in Date methods--parse", Date.parse, 0, null);
    verify("Built-in Date methods--UTC", Date.UTC, 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("16. Instantiated Date methods");

    vDate = new Date(96,0,1);

    verify("Instantiated Date methods--getDate", vDate.getDate, 0, null);
    verify("Instantiated Date methods--getDay", vDate.getDay, 0, null);
    verify("Instantiated Date methods--getHours", vDate.getHours, 0, null);
    verify("Instantiated Date methods--getMinutes", vDate.getMinutes, 0, null);
    verify("Instantiated Date methods--getMonth", vDate.getMonth, 0, null);
    verify("Instantiated Date methods--getSeconds", vDate.getSeconds, 0, null);
    verify("Instantiated Date methods--getTime", vDate.getTime, 0, null);
    verify("Instantiated Date methods--getTimezoneOffset", vDate.getTimezoneOffset, 0, null);
    verify("Instantiated Date methods--getYear", vDate.getYear, 0, null);
    verify("Instantiated Date methods--setDate", vDate.setDate, 0, null);
    verify("Instantiated Date methods--setHours", vDate.setHours, 0, null);
    verify("Instantiated Date methods--setMinutes", vDate.setMinutes, 0, null);
    verify("Instantiated Date methods--setMonth", vDate.setMonth, 0, null);
    verify("Instantiated Date methods--setSeconds", vDate.setSeconds, 0, null);
    verify("Instantiated Date methods--setTime", vDate.setTime, 0, null);
    verify("Instantiated Date methods--setYear", vDate.setYear, 0, null);
    verify("Instantiated Date methods--toGMTString", vDate.toGMTString, 0, null);
    verify("Instantiated Date methods--toLocaleString", vDate.toLocaleString, 0, null);


    /***** Browser only method tests
    //----------------------------------------------------------------------------
//    apInitScenario("17. Browser only methods");

    verify("Browser only methods--alert", window.alert, 0, null);
    verify("Browser only methods--back", history.back, 0, null);
    verify("Browser only methods--blur", text.blur, 0, null);
    verify("Browser only methods--clear", document.clear, 0, null);
    verify("Browser only methods--clearTimeout", frame.clearTimeout, 0, null);
    verify("Browser only methods--click", button.click, 0, null);
    verify("Browser only methods--close", document.close, 0, null);
    verify("Browser only methods--close", window.close, 0, null);
    verify("Browser only methods--confirm", window.confirm, 0, null);
    verify("Browser only methods--focus", text.focus, 0, null);
    verify("Browser only methods--forward", history.forward, 0, null);
    verify("Browser only methods--go", history.go, 0, null);
    verify("Browser only methods--open", document.open, 0, null);
    verify("Browser only methods--open", window.open, 0, null);
    verify("Browser only methods--prompt", window.prompt, 0, null);
    verify("Browser only methods--select", text.select, 0, null);
    verify("Browser only methods--setTimeout", window.setTimeout, 0, null);
    verify("Browser only methods--submit", form.submit, 0, null);
    verify("Browser only methods--write", document.write, 0, null);
    verify("Browser only methods--writeln", document.writeln, 0, null);
    */

    apEndTest();

}


tostr016();


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

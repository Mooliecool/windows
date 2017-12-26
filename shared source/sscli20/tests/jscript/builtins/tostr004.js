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


var iTestID = 53694;


function VerifyType(scen, ob, sUnq, type, bugNum)
{
    var VersionLF="\n";

    @cc_on
    if (parseFloat(@_jscript_version)>=7)
      VersionLF = "";
 
    if (bugNum == null) bugNum = "";
    var sExp="", sAct="";

    var arr = new Array();
    arr[13] = ob;

    // object types
    // 1 == scaler (misnomer-but ca va. This is anything that should return the object's value)
    // 2 == function
    // 3 == object

    if (type==1)
        sExp = sUnq;
    else if (type==2)
        sExp = VersionLF + "function "+sUnq+"() {\n    [native code]\n}" + VersionLF;
    else if (type==3)
        sExp = "[object "+sUnq+"]";
    else if (type==4)
	{

@if(!@aspx)
     	sExp = "function "+sUnq+"() {}";
@else
     	sExp = "expando function "+sUnq+"() {}";
@end
	}

    sAct = arr[13].toString();

    if (sAct != sExp)
        apLogFailInfo( scen+" failed ", sExp, sAct, bugNum);
}


@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function tostr004() {
 @if(@_fast)
    var vDate;
 @end

    apInitTest( "toStr004 " );

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    VerifyType("built-in, non-exec",Math,"Math",3);


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    VerifyType("built-in, exec, not instanciated--Array",Array,"Array",2);
    VerifyType("built-in, exec, not instanciated--Boolean",Boolean,"Boolean",2);
    VerifyType("built-in, exec, not instanciated--Date",Date,"Date",2);
    VerifyType("built-in, exec, not instanciated--Number",Number,"Number",2);
    VerifyType("built-in, exec, not instanciated--Object",Object,"Object",2);
    VerifyType("built-in, exec, not instanciated--String",String,"String",2);


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    VerifyType("built-in, exec, instanciated--Array",new Array(),"",1);
    VerifyType("built-in, exec, instanciated--Boolean",new Boolean(),"false",1);
    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    VerifyType("built-in, exec, instanciated--Date",new Date(96,0,1),"Mon Jan 1 00:00:00 PST 1996",1);
    }
    VerifyType("built-in, exec, instanciated--Number",new Number(),"0",1);
    VerifyType("built-in, exec, instanciated--Object",new Object(),"Object",3);
    VerifyType("built-in, exec, instanciated--String",new String(),"",1);


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    VerifyType("user-defined, not instanciated",obFoo,"obFoo",4);


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    VerifyType("user-defined, instanciated",new obFoo(),"Object",3);


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    VerifyType("number, decimal, integer--min pos",1,"1",1);
    VerifyType("number, decimal, integer--min pos < n < max pos",1234567890,"1234567890",1);
    VerifyType("number, decimal, integer--max pos",2147483647,"2147483647",1);

    VerifyType("number, decimal, integer--max neg",-1,"-1",1);
    VerifyType("number, decimal, integer--min neg < n < max neg",-1234567890,"-1234567890",1);
    VerifyType("number, decimal, integer--min neg",-2147483647,"-2147483647",1);

    VerifyType("number, decimal, integer--pos zero",0,"0",1);
    VerifyType("number, decimal, integer--neg zero",0,"0",1);


    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    VerifyType("number, decimal, float--min pos < n < max pos",1.2345678e90,"1.2345678e+90",1);
    VerifyType("number, decimal, float-- > max pos float (1.#INF)",1.797693134862315807e309,"Infinity",1);

    VerifyType("number, decimal, float--min neg < n < max neg",-1.2345678e90,"-1.2345678e+90",1);
    VerifyType("number, decimal, float-- < min neg float (-1.#INF)",-1.797693134862315807e309,"-Infinity",1);

    VerifyType("number, decimal, float--pos zero",0.0,"0",1);
    VerifyType("number, decimal, float--neg zero",-0.0,"0",1);

    
    //----------------------------------------------------------------------------
    apInitScenario("8. string");

    VerifyType("literal, string--single space"," "," ",1);
    VerifyType("literal, string--multiple spaces","                                                                   ",
        "                                                                   ",1);
    VerifyType("literal, string--as false","false","false",1);
	VerifyType("literal, string--ns 0","0","0",1);
    VerifyType("literal, string--ns > 0","1234567890","1234567890",1);
    VerifyType("literal, string--ns > 0, leading space"," 1234567890"," 1234567890",1);
    VerifyType("literal, string--ns > 0, trailing space","1234567890 ","1234567890 ",1);
    VerifyType("literal, string--ns < 0","-1234567890","-1234567890",1);
    VerifyType("literal, string--as single word","obFoo","obFoo",1);
    VerifyType("literal, string--as single word, leading space"," foo"," foo",1);
    VerifyType("literal, string--as single word, trailing space","foo ","foo ",1);
    VerifyType("literal, string--as multiple word","foo bar","foo bar",1);
    VerifyType("literal, string--as multiple word, leading space"," foo bar"," foo bar",1);
    VerifyType("literal, string--as multiple word, trailing space","foo bar ","foo bar ",1);
    VerifyType("literal, string--zls","","",1);


    //----------------------------------------------------------------------------
    apInitScenario("9. constants");

    VerifyType("constants",true, "true", 1);
    VerifyType("constants",false,"false",1);


    //----------------------------------------------------------------------------
    apInitScenario("12. built-in functions");

    VerifyType("built-in functions--escape", escape, "escape", 2);
    VerifyType("built-in functions--eval", eval, "eval", 2);
    VerifyType("built-in functions--isNaN", isNaN, "isNaN", 2);
    VerifyType("built-in functions--parseFloat", parseFloat, "parseFloat", 2);
    VerifyType("built-in functions--parseInt", parseInt, "parseInt", 2);
    VerifyType("built-in functions--unescape", unescape, "unescape", 2);


    //----------------------------------------------------------------------------
    apInitScenario("13. Math methods");

    VerifyType("Math methods--Math.abs", Math.abs, "abs", 2);
    VerifyType("Math methods--Math.acos", Math.acos, "acos", 2);
    VerifyType("Math methods--Math.asin", Math.asin, "asin", 2);
    VerifyType("Math methods--Math.atan", Math.atan, "atan", 2);
    VerifyType("Math methods--Math.ceil", Math.ceil, "ceil", 2);
    VerifyType("Math methods--Math.cos", Math.cos, "cos", 2);
    VerifyType("Math methods--Math.exp", Math.exp, "exp", 2);
    VerifyType("Math methods--Math.floor", Math.floor, "floor", 2);
    VerifyType("Math methods--Math.log", Math.log, "log", 2);
    VerifyType("Math methods--Math.max", Math.max, "max", 2);
    VerifyType("Math methods--Math.min", Math.min, "min", 2);
    VerifyType("Math methods--Math.pow", Math.pow, "pow", 2);
    VerifyType("Math methods--Math.random", Math.random, "random", 2);
    VerifyType("Math methods--Math.round", Math.round, "round", 2);
    VerifyType("Math methods--Math.sin", Math.sin, "sin", 2);
    VerifyType("Math methods--Math.sqrt", Math.sqrt, "sqrt", 2);
    VerifyType("Math methods--Math.tan", Math.tan, "tan", 2);


    //----------------------------------------------------------------------------
    apInitScenario("14. string methods");

    VerifyType("string methods--anchor", "somestring".anchor, "anchor", 2);
    VerifyType("string methods--big", "somestring".big, "big", 2);
    VerifyType("string methods--blink", "somestring".blink, "blink", 2);
    VerifyType("string methods--bold", "somestring".bold, "bold", 2);
    VerifyType("string methods--charAt", "somestring".charAt, "charAt", 2);
    VerifyType("string methods--fixed", "somestring".fixed, "fixed", 2);
    VerifyType("string methods--fontcolor", "somestring".fontcolor, "fontcolor", 2);
    VerifyType("string methods--fontsize", "somestring".fontsize, "fontsize", 2);
    VerifyType("string methods--indexOf", "somestring".indexOf, "indexOf", 2);
    VerifyType("string methods--italics", "somestring".italics, "italics", 2);
    VerifyType("string methods--lastIndexOf", "somestring".lastIndexOf, "lastIndexOf", 2);
    VerifyType("string methods--link", "somestring".link, "link", 2);
    VerifyType("string methods--small", "somestring".small, "small", 2);
    VerifyType("string methods--strike", "somestring".strike, "strike", 2);
    VerifyType("string methods--sub", "somestring".sub, "sub", 2);
    VerifyType("string methods--substring", "somestring".substring, "substring", 2);
    VerifyType("string methods--sup", "somestring".sup, "sup", 2);
    VerifyType("string methods--toLowerCase", "somestring".toLowerCase, "toLowerCase", 2);
    VerifyType("string methods--toUpperCase", "somestring".toUpperCase, "toUpperCase", 2);


    //----------------------------------------------------------------------------
    apInitScenario("15. Built-in Date methods");

    VerifyType("Built-in Date methods--parse", Date.parse, "parse", 2);
    VerifyType("Built-in Date methods--UTC", Date.UTC, "UTC", 2);


    //----------------------------------------------------------------------------
    apInitScenario("16. Instantiated Date methods");

    vDate = new Date(96,0,1);

    VerifyType("Instantiated Date methods--getDate", vDate.getDate, "getDate", 2);
    VerifyType("Instantiated Date methods--getDay", vDate.getDay, "getDay", 2);
    VerifyType("Instantiated Date methods--getHours", vDate.getHours, "getHours", 2);
    VerifyType("Instantiated Date methods--getMinutes", vDate.getMinutes, "getMinutes", 2);
    VerifyType("Instantiated Date methods--getMonth", vDate.getMonth, "getMonth", 2);
    VerifyType("Instantiated Date methods--getSeconds", vDate.getSeconds, "getSeconds", 2);
    VerifyType("Instantiated Date methods--getTime", vDate.getTime, "getTime", 2);
    VerifyType("Instantiated Date methods--getTimezoneOffset", vDate.getTimezoneOffset, "getTimezoneOffset", 2);
    VerifyType("Instantiated Date methods--getYear", vDate.getYear, "getYear", 2);
    VerifyType("Instantiated Date methods--setDate", vDate.setDate, "setDate", 2);
    VerifyType("Instantiated Date methods--setHours", vDate.setHours, "setHours", 2);
    VerifyType("Instantiated Date methods--setMinutes", vDate.setMinutes, "setMinutes", 2);
    VerifyType("Instantiated Date methods--setMonth", vDate.setMonth, "setMonth", 2);
    VerifyType("Instantiated Date methods--setSeconds", vDate.setSeconds, "setSeconds", 2);
    VerifyType("Instantiated Date methods--setTime", vDate.setTime, "setTime", 2);
    VerifyType("Instantiated Date methods--setYear", vDate.setYear, "setYear", 2);
    @cc_on
    //if (parseFloat(@_jscript_version)>=7)
      //VerifyType("Instantiated Date methods--toGMTString", vDate.toGMTString, "toUTCString", 2);
    //else
   //   VerifyType("Instantiated Date methods--toGMTString", vDate.toGMTString, "toGMTString", 2);
if (parseFloat(@_jscript_version)>=7) {
	@if (@_fast)
		VerifyType("Instantiated Date methods--toGMTString", vDate.toGMTString, "toGMTString", 2);
	@else
        	VerifyType("Instantiated Date methods--toGMTString", vDate.toGMTString, "toUTCString", 2);
	@end
	}
    else 
        VerifyType("Instantiated Date methods--toGMTString", vDate.toGMTString, "toGMTString", 2);
	
	   

 VerifyType("Instantiated Date methods--toLocaleString", vDate.toLocaleString, "toLocaleString", 2);


    apEndTest();

}


tostr004();


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

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


var iTestID = 65322;



@if(!@aspx)
	function UDObject() {this.myValue = 88.8;}
	function UDFunction() {return 55.6;}
@else
	expando function UDObject() {this.myValue = 88.8;}
	expando function UDFunction() {return 55.6;}
@end

function oper002() 
{
    // Added to handle strict mode in JScript 7.0
    @cc_on 
        @if (@_fast)  
            var expected, result;                 
         
    @end       

    var strVar = "13.2";
    var numVar = 17.3;
    var myDate = new Date();
    var emptyVar;
    var arrayVar = new Array(10);
    var myObj = new UDObject();
    var myUDF = new UDFunction();
    var myBool = true;

    arrayVar[3] = 13.1;
    arrayVar[4] = 14.2;
    arrayVar[5] = 15.3;
    myDate.setTime(0);

    apInitTest("oper002");

    // tokenizer output scenario #1
    apInitScenario("compare numeric variable < numeric variable");
    expected = false;
    result = (numVar < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #2
    apInitScenario("compare numeric variable < string variable");
    expected = false;
    result = (numVar < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #3
    apInitScenario("compare numeric variable < date variable");
    expected = false;
    result = (numVar < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #6
    apInitScenario("compare numeric variable < fn retval");
    expected = true;
    result = (numVar < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #7
    apInitScenario("compare numeric variable < intrinsic function");
    expected = false;
    result = (numVar < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #8
    apInitScenario("compare numeric variable < array element");
    expected = false;
    result = (numVar < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #9
    apInitScenario("compare numeric variable < object method");
    expected = true;
	@cc_on
	@if (@_jscript_version < 7)
    result = (numVar < myObj.myValue);
	@else
    result = (numVar < 88.8);  // expando props not allowed in js7 strict mode
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #10
    apInitScenario("compare numeric variable < string constant");
    expected = true;
    result = (numVar < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #11
    apInitScenario("compare numeric variable < numeric constant");
    expected = true;
    result = (numVar < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #12
    apInitScenario("compare numeric variable < boolean variable");
    expected = false;
    result = (numVar < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #13
    apInitScenario("compare numeric variable < null");
    expected = false;
    result = (numVar < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #14
    apInitScenario("compare numeric variable < boolean constant");
    expected = false;
    result = (numVar < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #15
    apInitScenario("compare string variable < numeric variable");
    expected = true;
    result = (strVar < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #16
    apInitScenario("compare string variable < string variable");
    expected = false;
    result = (strVar < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #17
    apInitScenario("compare string variable < date variable");
    expected = false;
    result = (strVar < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #20
    apInitScenario("compare string variable < fn retval");
    expected = true;
    result = (strVar < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #21
    apInitScenario("compare string variable < intrinsic function");
    expected = false;
    result = (strVar < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #22
    apInitScenario("compare string variable < array element");
    expected = true;
    result = (strVar < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #23
    apInitScenario("compare string variable < object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (strVar < myObj.myValue);
	@else
	result = (strVar < 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #24
    apInitScenario("compare string variable < string constant");
    expected = true;
    result = (strVar < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #25
    apInitScenario("compare string variable < numeric constant");
    expected = true;
    result = (strVar < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #26
    apInitScenario("compare string variable < boolean variable");
    expected = false;
    result = (strVar < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #27
    apInitScenario("compare string variable < null");
    expected = false;
    result = (strVar < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #28
    apInitScenario("compare string variable < boolean constant");
    expected = false;
    result = (strVar < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #29
    apInitScenario("compare date variable < numeric variable");
    expected = true;
    result = (myDate.getTime() < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #30
    apInitScenario("compare date variable < string variable");
    expected = true;
    result = (myDate.getTime() < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #31
    apInitScenario("compare date variable < date variable");
    expected = false;
    result = (myDate.getTime() < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #34
    apInitScenario("compare date variable < fn retval");
    expected = true;
    result = (myDate.getTime() < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #35
    apInitScenario("compare date variable < intrinsic function");
    expected = true;
    result = (myDate.getTime() < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #36
    apInitScenario("compare date variable < array element");
    expected = true;
    result = (myDate.getTime() < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #37
    apInitScenario("compare date variable < object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myDate.getTime() < myObj.myValue);
	@else
    result = (myDate.getTime() < 88.8);
	@end


    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #38
    apInitScenario("compare date variable < string constant");
    expected = true;
    result = (myDate.getTime() < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #39
    apInitScenario("compare date variable < numeric constant");
    expected = true;
    result = (myDate.getTime() < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #40
    apInitScenario("compare date variable < boolean variable");
    expected = true;
    result = (myDate.getTime() < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #41
    apInitScenario("compare date variable < null");
    expected = false;
    result = (myDate.getTime() < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #42
    apInitScenario("compare date variable < boolean constant");
    expected = true;
    result = (myDate.getTime() < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #71
    apInitScenario("compare fn retval < numeric variable");
    expected = false;
    result = (UDFunction() < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #72
    apInitScenario("compare fn retval < string variable");
    expected = false;
    result = (UDFunction() < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #73
    apInitScenario("compare fn retval < date variable");
    expected = false;
    result = (UDFunction() < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #76
    apInitScenario("compare fn retval < fn retval");
    expected = false;
    result = (UDFunction() < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #77
    apInitScenario("compare fn retval < intrinsic function");
    expected = false;
    result = (UDFunction() < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #78
    apInitScenario("compare fn retval < array element");
    expected = false;
    result = (UDFunction() < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #79
    apInitScenario("compare fn retval < object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (UDFunction() < myObj.myValue);
	@else
    result = (UDFunction() < 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #80
    apInitScenario("compare fn retval < string constant");
    expected = true;
    result = (UDFunction() < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #81
    apInitScenario("compare fn retval < numeric constant");
    expected = true;
    result = (UDFunction() < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #82
    apInitScenario("compare fn retval < boolean variable");
    expected = false;
    result = (UDFunction() < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #83
    apInitScenario("compare fn retval < null");
    expected = false;
    result = (UDFunction() < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #84
    apInitScenario("compare fn retval < boolean constant");
    expected = false;
    result = (UDFunction() < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #85
    apInitScenario("compare intrinsic function < numeric variable");
    expected = true;
    result = (Math.cos(0) < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #86
    apInitScenario("compare intrinsic function < string variable");
    expected = true;
    result = (Math.cos(0) < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #87
    apInitScenario("compare intrinsic function < date variable");
    expected = false;
    result = (Math.cos(0) < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #90
    apInitScenario("compare intrinsic function < fn retval");
    expected = true;
    result = (Math.cos(0) < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #91
    apInitScenario("compare intrinsic function < intrinsic function");
    expected = false;
    result = (Math.cos(0) < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #92
    apInitScenario("compare intrinsic function < array element");
    expected = true;
    result = (Math.cos(0) < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #93
    apInitScenario("compare intrinsic function < object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (Math.cos(0) < myObj.myValue);
	@else
    result = (Math.cos(0) < 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #94
    apInitScenario("compare intrinsic function < string constant");
    expected = true;
    result = (Math.cos(0) < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #95
    apInitScenario("compare intrinsic function < numeric constant");
    expected = true;
    result = (Math.cos(0) < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #96
    apInitScenario("compare intrinsic function < boolean variable");
    expected = false;
    result = (Math.cos(0) < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #97
    apInitScenario("compare intrinsic function < null");
    expected = false;
    result = (Math.cos(0) < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #98
    apInitScenario("compare intrinsic function < boolean constant");
    expected = false;
    result = (Math.cos(0) < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}

oper002();


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

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


var iTestID = 51784;



@if(!@aspx)
   function UDObject() {
       this.myValue = 88.8;
   }
   function UDFunction() {
       return 55.6;
   }
@else
   expando function UDObject(){
       this.myValue = 88.8;
   }
   expando function UDFunction() {
       return 55.6;
   }
@end

function coerce02() {
@if(@_fast)
    var result, expected;
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

    apInitTest("coerce02");

    // tokenizer output scenario #1
    apInitScenario("combine numeric variable and numeric variable");
    expected = "number";
    result = numVar + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #2
    apInitScenario("combine numeric variable and string variable");
    expected = "string";
    result = numVar + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #3
    apInitScenario("combine numeric variable and date variable");
    expected = "string";
    result = numVar + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #4
    apInitScenario("combine numeric variable and object");
    expected = "string";
    result = numVar + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #5
    apInitScenario("combine numeric variable and udf");
    expected = "string";
    result = numVar + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #6
    apInitScenario("combine numeric variable and fn retval");
    expected = "number";
    result = numVar + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #7
    apInitScenario("combine numeric variable and intrinsic function");
    expected = "number";
    result = numVar + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #8
    apInitScenario("combine numeric variable and array element");
    expected = "number";
    result = numVar + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #9
    apInitScenario("combine numeric variable and object method");
    expected = "number";
    result = numVar + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #10
    apInitScenario("combine numeric variable and string constant");
    expected = "string";
    result = numVar + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #11
    apInitScenario("combine numeric variable and numeric constant");
    expected = "number";
    result = numVar + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #12
    apInitScenario("combine numeric variable and boolean variable");
    expected = "number";
    result = numVar + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #13
    apInitScenario("combine numeric variable and null");
    expected = "number";
    result = numVar + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #14
    apInitScenario("combine numeric variable and boolean constant");
    expected = "number";
    result = numVar + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #15
    apInitScenario("combine string variable and numeric variable");
    expected = "string";
    result = strVar + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #16
    apInitScenario("combine string variable and string variable");
    expected = "string";
    result = strVar + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #17
    apInitScenario("combine string variable and date variable");
    expected = "string";
    result = strVar + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #18
    apInitScenario("combine string variable and object");
    expected = "string";
    result = strVar + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #19
    apInitScenario("combine string variable and udf");
    expected = "string";
    result = strVar + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #20
    apInitScenario("combine string variable and fn retval");
    expected = "string";
    result = strVar + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #21
    apInitScenario("combine string variable and intrinsic function");
    expected = "string";
    result = strVar + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #22
    apInitScenario("combine string variable and array element");
    expected = "string";
    result = strVar + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #23
    apInitScenario("combine string variable and object method");
    expected = "string";
    result = strVar + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #24
    apInitScenario("combine string variable and string constant");
    expected = "string";
    result = strVar + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #25
    apInitScenario("combine string variable and numeric constant");
    expected = "string";
    result = strVar + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #26
    apInitScenario("combine string variable and boolean variable");
    expected = "string";
    result = strVar + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #27
    apInitScenario("combine string variable and null");
    expected = "string";
    result = strVar + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #28
    apInitScenario("combine string variable and boolean constant");
    expected = "string";
    result = strVar + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #29
    apInitScenario("combine date variable and numeric variable");
    expected = "string";
    result = myDate + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #30
    apInitScenario("combine date variable and string variable");
    expected = "string";
    result = myDate + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #31
    apInitScenario("combine date variable and date variable");
    expected = "string";
    result = myDate + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #32
    apInitScenario("combine date variable and object");
    expected = "string";
    result = myDate + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #33
    apInitScenario("combine date variable and udf");
    expected = "string";
    result = myDate + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #34
    apInitScenario("combine date variable and fn retval");
    expected = "string";
    result = myDate + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #35
    apInitScenario("combine date variable and intrinsic function");
    expected = "string";
    result = myDate + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #36
    apInitScenario("combine date variable and array element");
    expected = "string";
    result = myDate + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #37
    apInitScenario("combine date variable and object method");
    expected = "string";
    result = myDate + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #38
    apInitScenario("combine date variable and string constant");
    expected = "string";
    result = myDate + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #39
    apInitScenario("combine date variable and numeric constant");
    expected = "string";
    result = myDate + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #40
    apInitScenario("combine date variable and boolean variable");
    expected = "string";
    result = myDate + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #41
    apInitScenario("combine date variable and null");
    expected = "string";
    result = myDate + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #42
    apInitScenario("combine date variable and boolean constant");
    expected = "string";
    result = myDate + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #43
    apInitScenario("combine object and numeric variable");
    expected = "string";
    result = myObj + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #44
    apInitScenario("combine object and string variable");
    expected = "string";
    result = myObj + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #45
    apInitScenario("combine object and date variable");
    expected = "string";
    result = myObj + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #46
    apInitScenario("combine object and object");
    expected = "string";
    result = myObj + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #47
    apInitScenario("combine object and udf");
    expected = "string";
    result = myObj + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #48
    apInitScenario("combine object and fn retval");
    expected = "string";
    result = myObj + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #49
    apInitScenario("combine object and intrinsic function");
    expected = "string";
    result = myObj + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #50
    apInitScenario("combine object and array element");
    expected = "string";
    result = myObj + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #51
    apInitScenario("combine object and object method");
    expected = "string";
    result = myObj + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #52
    apInitScenario("combine object and string constant");
    expected = "string";
    result = myObj + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #53
    apInitScenario("combine object and numeric constant");
    expected = "string";
    result = myObj + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #54
    apInitScenario("combine object and boolean variable");
    expected = "string";
    result = myObj + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #55
    apInitScenario("combine object and null");
    expected = "string";
    result = myObj + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #56
    apInitScenario("combine object and boolean constant");
    expected = "string";
    result = myObj + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #57
    apInitScenario("combine udf and numeric variable");
    expected = "string";
    result = myUDF + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #58
    apInitScenario("combine udf and string variable");
    expected = "string";
    result = myUDF + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #59
    apInitScenario("combine udf and date variable");
    expected = "string";
    result = myUDF + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #60
    apInitScenario("combine udf and object");
    expected = "string";
    result = myUDF + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #61
    apInitScenario("combine udf and udf");
    expected = "string";
    result = myUDF + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #62
    apInitScenario("combine udf and fn retval");
    expected = "string";
    result = myUDF + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #63
    apInitScenario("combine udf and intrinsic function");
    expected = "string";
    result = myUDF + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #64
    apInitScenario("combine udf and array element");
    expected = "string";
    result = myUDF + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #65
    apInitScenario("combine udf and object method");
    expected = "string";
    result = myUDF + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #66
    apInitScenario("combine udf and string constant");
    expected = "string";
    result = myUDF + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #67
    apInitScenario("combine udf and numeric constant");
    expected = "string";
    result = myUDF + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #68
    apInitScenario("combine udf and boolean variable");
    expected = "string";
    result = myUDF + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #69
    apInitScenario("combine udf and null");
    expected = "string";
    result = myUDF + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #70
    apInitScenario("combine udf and boolean constant");
    expected = "string";
    result = myUDF + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }


    apEndTest();
}


coerce02();


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

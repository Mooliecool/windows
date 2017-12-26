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


var iTestID = 51785;



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

function coerce03() {
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

    apInitTest("coerce03");

    // tokenizer output scenario #71
    apInitScenario("combine fn retval and numeric variable");
    expected = "number";
    result = UDFunction() + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #72
    apInitScenario("combine fn retval and string variable");
    expected = "string";
    result = UDFunction() + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #73
    apInitScenario("combine fn retval and date variable");
    expected = "string";
    result = UDFunction() + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #74
    apInitScenario("combine fn retval and object");
    expected = "string";
    result = UDFunction() + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #75
    apInitScenario("combine fn retval and udf");
    expected = "string";
    result = UDFunction() + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #76
    apInitScenario("combine fn retval and fn retval");
    expected = "number";
    result = UDFunction() + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #77
    apInitScenario("combine fn retval and intrinsic function");
    expected = "number";
    result = UDFunction() + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #78
    apInitScenario("combine fn retval and array element");
    expected = "number";
    result = UDFunction() + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #79
    apInitScenario("combine fn retval and object method");
    expected = "number";
    result = UDFunction() + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #80
    apInitScenario("combine fn retval and string constant");
    expected = "string";
    result = UDFunction() + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #81
    apInitScenario("combine fn retval and numeric constant");
    expected = "number";
    result = UDFunction() + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #82
    apInitScenario("combine fn retval and boolean variable");
    expected = "number";
    result = UDFunction() + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #83
    apInitScenario("combine fn retval and null");
    expected = "number";
    result = UDFunction() + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #84
    apInitScenario("combine fn retval and boolean constant");
    expected = "number";
    result = UDFunction() + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #85
    apInitScenario("combine intrinsic function and numeric variable");
    expected = "number";
    result = Math.cos(0) + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #86
    apInitScenario("combine intrinsic function and string variable");
    expected = "string";
    result = Math.cos(0) + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #87
    apInitScenario("combine intrinsic function and date variable");
    expected = "string";
    result = Math.cos(0) + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #88
    apInitScenario("combine intrinsic function and object");
    expected = "string";
    result = Math.cos(0) + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #89
    apInitScenario("combine intrinsic function and udf");
    expected = "string";
    result = Math.cos(0) + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #90
    apInitScenario("combine intrinsic function and fn retval");
    expected = "number";
    result = Math.cos(0) + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #91
    apInitScenario("combine intrinsic function and intrinsic function");
    expected = "number";
    result = Math.cos(0) + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #92
    apInitScenario("combine intrinsic function and array element");
    expected = "number";
    result = Math.cos(0) + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #93
    apInitScenario("combine intrinsic function and object method");
    expected = "number";
    result = Math.cos(0) + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #94
    apInitScenario("combine intrinsic function and string constant");
    expected = "string";
    result = Math.cos(0) + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #95
    apInitScenario("combine intrinsic function and numeric constant");
    expected = "number";
    result = Math.cos(0) + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #96
    apInitScenario("combine intrinsic function and boolean variable");
    expected = "number";
    result = Math.cos(0) + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #97
    apInitScenario("combine intrinsic function and null");
    expected = "number";
    result = Math.cos(0) + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #98
    apInitScenario("combine intrinsic function and boolean constant");
    expected = "number";
    result = Math.cos(0) + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #99
    apInitScenario("combine array element and numeric variable");
    expected = "number";
    result = arrayVar[4] + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #100
    apInitScenario("combine array element and string variable");
    expected = "string";
    result = arrayVar[4] + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #101
    apInitScenario("combine array element and date variable");
    expected = "string";
    result = arrayVar[4] + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #102
    apInitScenario("combine array element and object");
    expected = "string";
    result = arrayVar[4] + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #103
    apInitScenario("combine array element and udf");
    expected = "string";
    result = arrayVar[4] + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #104
    apInitScenario("combine array element and fn retval");
    expected = "number";
    result = arrayVar[4] + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #105
    apInitScenario("combine array element and intrinsic function");
    expected = "number";
    result = arrayVar[4] + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #106
    apInitScenario("combine array element and array element");
    expected = "number";
    result = arrayVar[4] + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #107
    apInitScenario("combine array element and object method");
    expected = "number";
    result = arrayVar[4] + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #108
    apInitScenario("combine array element and string constant");
    expected = "string";
    result = arrayVar[4] + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #109
    apInitScenario("combine array element and numeric constant");
    expected = "number";
    result = arrayVar[4] + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #110
    apInitScenario("combine array element and boolean variable");
    expected = "number";
    result = arrayVar[4] + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #111
    apInitScenario("combine array element and null");
    expected = "number";
    result = arrayVar[4] + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #112
    apInitScenario("combine array element and boolean constant");
    expected = "number";
    result = arrayVar[4] + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #113
    apInitScenario("combine object method and numeric variable");
    expected = "number";
    result = myObj.myValue + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #114
    apInitScenario("combine object method and string variable");
    expected = "string";
    result = myObj.myValue + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #115
    apInitScenario("combine object method and date variable");
    expected = "string";
    result = myObj.myValue + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #116
    apInitScenario("combine object method and object");
    expected = "string";
    result = myObj.myValue + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #117
    apInitScenario("combine object method and udf");
    expected = "string";
    result = myObj.myValue + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #118
    apInitScenario("combine object method and fn retval");
    expected = "number";
    result = myObj.myValue + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #119
    apInitScenario("combine object method and intrinsic function");
    expected = "number";
    result = myObj.myValue + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #120
    apInitScenario("combine object method and array element");
    expected = "number";
    result = myObj.myValue + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #121
    apInitScenario("combine object method and object method");
    expected = "number";
    result = myObj.myValue + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #122
    apInitScenario("combine object method and string constant");
    expected = "string";
    result = myObj.myValue + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #123
    apInitScenario("combine object method and numeric constant");
    expected = "number";
    result = myObj.myValue + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #124
    apInitScenario("combine object method and boolean variable");
    expected = "number";
    result = myObj.myValue + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #125
    apInitScenario("combine object method and null");
    expected = "number";
    result = myObj.myValue + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #126
    apInitScenario("combine object method and boolean constant");
    expected = "number";
    result = myObj.myValue + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #127
    apInitScenario("combine string constant and numeric variable");
    expected = "string";
    result = "200.3" + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #128
    apInitScenario("combine string constant and string variable");
    expected = "string";
    result = "200.3" + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #129
    apInitScenario("combine string constant and date variable");
    expected = "string";
    result = "200.3" + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #130
    apInitScenario("combine string constant and object");
    expected = "string";
    result = "200.3" + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #131
    apInitScenario("combine string constant and udf");
    expected = "string";
    result = "200.3" + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #132
    apInitScenario("combine string constant and fn retval");
    expected = "string";
    result = "200.3" + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #133
    apInitScenario("combine string constant and intrinsic function");
    expected = "string";
    result = "200.3" + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #134
    apInitScenario("combine string constant and array element");
    expected = "string";
    result = "200.3" + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #135
    apInitScenario("combine string constant and object method");
    expected = "string";
    result = "200.3" + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #136
    apInitScenario("combine string constant and string constant");
    expected = "string";
    result = "200.3" + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #137
    apInitScenario("combine string constant and numeric constant");
    expected = "string";
    result = "200.3" + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #138
    apInitScenario("combine string constant and boolean variable");
    expected = "string";
    result = "200.3" + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #139
    apInitScenario("combine string constant and null");
    expected = "string";
    result = "200.3" + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #140
    apInitScenario("combine string constant and boolean constant");
    expected = "string";
    result = "200.3" + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }


    apEndTest();
}


coerce03();


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

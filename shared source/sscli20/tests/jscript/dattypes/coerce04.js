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


var iTestID = 51786;



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

function coerce04() {
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

    apInitTest("coerce04");

    // tokenizer output scenario #141
    apInitScenario("combine numeric constant and numeric variable");
    expected = "number";
    result = 300.2 + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #142
    apInitScenario("combine numeric constant and string variable");
    expected = "string";
    result = 300.2 + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #143
    apInitScenario("combine numeric constant and date variable");
    expected = "string";
    result = 300.2 + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #144
    apInitScenario("combine numeric constant and object");
    expected = "string";
    result = 300.2 + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #145
    apInitScenario("combine numeric constant and udf");
    expected = "string";
    result = 300.2 + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #146
    apInitScenario("combine numeric constant and fn retval");
    expected = "number";
    result = 300.2 + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #147
    apInitScenario("combine numeric constant and intrinsic function");
    expected = "number";
    result = 300.2 + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #148
    apInitScenario("combine numeric constant and array element");
    expected = "number";
    result = 300.2 + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #149
    apInitScenario("combine numeric constant and object method");
    expected = "number";
    result = 300.2 + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #150
    apInitScenario("combine numeric constant and string constant");
    expected = "string";
    result = 300.2 + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #151
    apInitScenario("combine numeric constant and numeric constant");
    expected = "number";
    result = 300.2 + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #152
    apInitScenario("combine numeric constant and boolean variable");
    expected = "number";
    result = 300.2 + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #153
    apInitScenario("combine numeric constant and null");
    expected = "number";
    result = 300.2 + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #154
    apInitScenario("combine numeric constant and boolean constant");
    expected = "number";
    result = 300.2 + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #155
    apInitScenario("combine boolean variable and numeric variable");
    expected = "number";
    result = myBool + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #156
    apInitScenario("combine boolean variable and string variable");
    expected = "string";
    result = myBool + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #157
    apInitScenario("combine boolean variable and date variable");
    expected = "string";
    result = myBool + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #158
    apInitScenario("combine boolean variable and object");
    expected = "string";
    result = myBool + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #159
    apInitScenario("combine boolean variable and udf");
    expected = "string";
    result = myBool + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #160
    apInitScenario("combine boolean variable and fn retval");
    expected = "number";
    result = myBool + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #161
    apInitScenario("combine boolean variable and intrinsic function");
    expected = "number";
    result = myBool + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #162
    apInitScenario("combine boolean variable and array element");
    expected = "number";
    result = myBool + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #163
    apInitScenario("combine boolean variable and object method");
    expected = "number";
    result = myBool + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #164
    apInitScenario("combine boolean variable and string constant");
    expected = "string";
    result = myBool + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #165
    apInitScenario("combine boolean variable and numeric constant");
    expected = "number";
    result = myBool + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #166
    apInitScenario("combine boolean variable and boolean variable");
    expected = "number";
    result = myBool + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #167
    apInitScenario("combine boolean variable and null");
    expected = "number";
    result = myBool + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #168
    apInitScenario("combine boolean variable and boolean constant");
    expected = "number";
    result = myBool + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #169
    apInitScenario("combine null and numeric variable");
    expected = "number";
    result = null + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #170
    apInitScenario("combine null and string variable");
    expected = "string";
    result = null + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #171
    apInitScenario("combine null and date variable");
    expected = "string";
    result = null + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #172
    apInitScenario("combine null and object");
    expected = "string";
    result = null + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #173
    apInitScenario("combine null and udf");
    expected = "string";
    result = null + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #174
    apInitScenario("combine null and fn retval");
    expected = "number";
    result = null + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #175
    apInitScenario("combine null and intrinsic function");
    expected = "number";
    result = null + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #176
    apInitScenario("combine null and array element");
    expected = "number";
    result = null + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #177
    apInitScenario("combine null and object method");
    expected = "number";
    result = null + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #178
    apInitScenario("combine null and string constant");
    expected = "string";
    result = null + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #179
    apInitScenario("combine null and numeric constant");
    expected = "number";
    result = null + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #180
    apInitScenario("combine null and boolean variable");
    expected = "number";
    result = null + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #181
    apInitScenario("combine null and null");
    expected = "number";
    result = null + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #182
    apInitScenario("combine null and boolean constant");
    expected = "number";
    result = null + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #183
    apInitScenario("combine boolean constant and numeric variable");
    expected = "number";
    result = true + numVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #184
    apInitScenario("combine boolean constant and string variable");
    expected = "string";
    result = true + strVar;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #185
    apInitScenario("combine boolean constant and date variable");
    expected = "string";
    result = true + myDate;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #186
    apInitScenario("combine boolean constant and object");
    expected = "string";
    result = true + myObj;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #187
    apInitScenario("combine boolean constant and udf");
    expected = "string";
    result = true + myUDF;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #188
    apInitScenario("combine boolean constant and fn retval");
    expected = "number";
    result = true + UDFunction();

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #189
    apInitScenario("combine boolean constant and intrinsic function");
    expected = "number";
    result = true + Math.cos(0);

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #190
    apInitScenario("combine boolean constant and array element");
    expected = "number";
    result = true + arrayVar[4];

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #191
    apInitScenario("combine boolean constant and object method");
    expected = "number";
    result = true + myObj.myValue;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #192
    apInitScenario("combine boolean constant and string constant");
    expected = "string";
    result = true + "200.3";

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #193
    apInitScenario("combine boolean constant and numeric constant");
    expected = "number";
    result = true + 300.2;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #194
    apInitScenario("combine boolean constant and boolean variable");
    expected = "number";
    result = true + myBool;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #195
    apInitScenario("combine boolean constant and null");
    expected = "number";
    result = true + null;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    // tokenizer output scenario #196
    apInitScenario("combine boolean constant and boolean constant");
    expected = "number";
    result = true + true;

    if (typeof(result) != expected) {
        apLogFailInfo("wrong type",expected,typeof(result),"");
    }

    
    apInitScenario("Roundtrip numbers to strings, verify return");
    expected = 1234.432143214321;
    result = (expected.toString())-0;

    if (result != expected) {
        apLogFailInfo("wrong value",expected,result,"");
    }

    
    apInitScenario("Roundtrip strings to number, verify return");
    expected = "1234.432143214321";
    result = (expected-0).toString();

    if (result != expected) {
        apLogFailInfo("wrong value",expected,result,"");
    }

    apEndTest();
}


coerce04();


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

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


var iTestID = 65320;


@if(@aspx) expando @end function UDFunction() {
    return 55.6;
}


@if(!@aspx)
   function UDObject() {
      this.myValue = 88.8;
   }
@else
   expando function UDObject() {
      this.myValue = 88.8;
   }
@end


function equ002() 
{
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

    apInitTest("equ002");

    // tokenizer output scenario #197
    apInitScenario("compare numeric variable == numeric variable");

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var expected;
        var result;
     
@end       

    expected = true;
    result = (numVar == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #198
    apInitScenario("compare numeric variable == string variable");
    expected = false;
    result = (numVar == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #199
    apInitScenario("compare numeric variable == date variable");
    expected = false;
    result = (numVar == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #200
    apInitScenario("compare numeric variable == object");
    expected = false;
    result = (numVar == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #201
    apInitScenario("compare numeric variable == udf");
    expected = false;
    result = (numVar == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #202
    apInitScenario("compare numeric variable == fn retval");
    expected = false;
    result = (numVar == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #203
    apInitScenario("compare numeric variable == intrinsic function");
    expected = false;
    result = (numVar == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #204
    apInitScenario("compare numeric variable == array element");
    expected = false;
    result = (numVar == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #205
    apInitScenario("compare numeric variable == object method");
    expected = false;
    result = (numVar == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #206
    apInitScenario("compare numeric variable == string constant");
    expected = false;
    result = (numVar == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #207
    apInitScenario("compare numeric variable == numeric constant");
    expected = false;
    result = (numVar == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #208
    apInitScenario("compare numeric variable == boolean variable");
    expected = false;
    result = (numVar == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #209
    apInitScenario("compare numeric variable == null");
    expected = false;
    result = (numVar == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #210
    apInitScenario("compare numeric variable == boolean constant");
    expected = false;
    result = (numVar == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #211
    apInitScenario("compare string variable == numeric variable");
    expected = false;
    result = (strVar == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #212
    apInitScenario("compare string variable == string variable");
    expected = true;
    result = (strVar == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #213
    apInitScenario("compare string variable == date variable");
    expected = false;
    result = (strVar == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #214
    apInitScenario("compare string variable == object");
    expected = false;
    result = (strVar == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #215
    apInitScenario("compare string variable == udf");
    expected = false;
    result = (strVar == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #216
    apInitScenario("compare string variable == fn retval");
    expected = false;
    result = (strVar == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #217
    apInitScenario("compare string variable == intrinsic function");
    expected = false;
    result = (strVar == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #218
    apInitScenario("compare string variable == array element");
    expected = false;
    result = (strVar == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #219
    apInitScenario("compare string variable == object method");
    expected = false;
    result = (strVar == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #220
    apInitScenario("compare string variable == string constant");
    expected = false;
    result = (strVar == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #221
    apInitScenario("compare string variable == numeric constant");
    expected = false;
    result = (strVar == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #222
    apInitScenario("compare string variable == boolean variable");
    expected = false;
    result = (strVar == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #223
    apInitScenario("compare string variable == null");
    expected = false;
    result = (strVar == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #224
    apInitScenario("compare string variable == boolean constant");
    expected = false;
    result = (strVar == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #225
    apInitScenario("compare date variable == numeric variable");
    expected = false;
    result = (myDate.getTime == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #226
    apInitScenario("compare date variable == string variable");
    expected = false;
    result = (myDate.getTime == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #227
    apInitScenario("compare date variable == date variable");
    expected = true;
    result = (myDate.getTime == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #228
    apInitScenario("compare date variable == object");
    expected = false;
    result = (myDate.getTime == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #229
    apInitScenario("compare date variable == udf");
    expected = false;
    result = (myDate.getTime == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #230
    apInitScenario("compare date variable == fn retval");
    expected = false;
    result = (myDate.getTime == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #231
    apInitScenario("compare date variable == intrinsic function");
    expected = false;
    result = (myDate.getTime == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #232
    apInitScenario("compare date variable == array element");
    expected = false;
    result = (myDate.getTime == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #233
    apInitScenario("compare date variable == object method");
    expected = false;
    result = (myDate.getTime == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #234
    apInitScenario("compare date variable == string constant");
    expected = false;
    result = (myDate.getTime == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #235
    apInitScenario("compare date variable == numeric constant");
    expected = false;
    result = (myDate.getTime == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #236
    apInitScenario("compare date variable == boolean variable");
    expected = false;
    result = (myDate.getTime == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #237
    apInitScenario("compare date variable == null");
    expected = false;
    result = (myDate.getTime == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #238
    apInitScenario("compare date variable == boolean constant");
    expected = false;
    result = (myDate.getTime == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #239
    apInitScenario("compare object == numeric variable");
    expected = false;
    result = (myObj == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #240
    apInitScenario("compare object == string variable");
    expected = false;
    result = (myObj == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #241
    apInitScenario("compare object == date variable");
    expected = false;
    result = (myObj == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #242
    apInitScenario("compare object == object");
    expected = true;
    result = (myObj == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #243
    apInitScenario("compare object == udf");
    expected = false;
    result = (myObj == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #244
    apInitScenario("compare object == fn retval");
    expected = false;
    result = (myObj == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #245
    apInitScenario("compare object == intrinsic function");
    expected = false;
    result = (myObj == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #246
    apInitScenario("compare object == array element");
    expected = false;
    result = (myObj == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #247
    apInitScenario("compare object == object method");
    expected = false;
    result = (myObj == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #248
    apInitScenario("compare object == string constant");
    expected = false;
    result = (myObj == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #249
    apInitScenario("compare object == numeric constant");
    expected = false;
    result = (myObj == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #250
    apInitScenario("compare object == boolean variable");
    expected = false;
    result = (myObj == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #251
    apInitScenario("compare object == null");
    expected = false;
    result = (myObj == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #252
    apInitScenario("compare object == boolean constant");
    expected = false;
    result = (myObj == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #253
    apInitScenario("compare udf == numeric variable");
    expected = false;
    result = (myUDF == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #254
    apInitScenario("compare udf == string variable");
    expected = false;
    result = (myUDF == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #255
    apInitScenario("compare udf == date variable");
    expected = false;
    result = (myUDF == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #256
    apInitScenario("compare udf == object");
    expected = false;
    result = (myUDF == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #258
    apInitScenario("compare udf == fn retval");
    expected = false;
    result = (myUDF == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #259
    apInitScenario("compare udf == intrinsic function");
    expected = false;
    result = (myUDF == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #260
    apInitScenario("compare udf == array element");
    expected = false;
    result = (myUDF == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #261
    apInitScenario("compare udf == object method");
    expected = false;
    result = (myUDF == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #262
    apInitScenario("compare udf == string constant");
    expected = false;
    result = (myUDF == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #263
    apInitScenario("compare udf == numeric constant");
    expected = false;
    result = (myUDF == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #264
    apInitScenario("compare udf == boolean variable");
    expected = false;
    result = (myUDF == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #265
    apInitScenario("compare udf == null");
    expected = false;
    result = (myUDF == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #266
    apInitScenario("compare udf == boolean constant");
    expected = false;
    result = (myUDF == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #267
    apInitScenario("compare fn retval == numeric variable");
    expected = false;
    result = (UDFunction() == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #268
    apInitScenario("compare fn retval == string variable");
    expected = false;
    result = (UDFunction() == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #269
    apInitScenario("compare fn retval == date variable");
    expected = false;
    result = (UDFunction() == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #270
    apInitScenario("compare fn retval == object");
    expected = false;
    result = (UDFunction() == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #271
    apInitScenario("compare fn retval == udf");
    expected = false;
    result = (UDFunction() == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #272
    apInitScenario("compare fn retval == fn retval");
    expected = true;
    result = (UDFunction() == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #273
    apInitScenario("compare fn retval == intrinsic function");
    expected = false;
    result = (UDFunction() == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #274
    apInitScenario("compare fn retval == array element");
    expected = false;
    result = (UDFunction() == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #275
    apInitScenario("compare fn retval == object method");
    expected = false;
    result = (UDFunction() == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #276
    apInitScenario("compare fn retval == string constant");
    expected = false;
    result = (UDFunction() == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #277
    apInitScenario("compare fn retval == numeric constant");
    expected = false;
    result = (UDFunction() == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #278
    apInitScenario("compare fn retval == boolean variable");
    expected = false;
    result = (UDFunction() == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #279
    apInitScenario("compare fn retval == null");
    expected = false;
    result = (UDFunction() == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #280
    apInitScenario("compare fn retval == boolean constant");
    expected = false;
    result = (UDFunction() == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #281
    apInitScenario("compare intrinsic function == numeric variable");
    expected = false;
    result = (Math.cos(0) == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #282
    apInitScenario("compare intrinsic function == string variable");
    expected = false;
    result = (Math.cos(0) == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #283
    apInitScenario("compare intrinsic function == date variable");
    expected = false;
    result = (Math.cos(0) == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #284
    apInitScenario("compare intrinsic function == object");
    expected = false;
    result = (Math.cos(0) == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #285
    apInitScenario("compare intrinsic function == udf");
    expected = false;
    result = (Math.cos(0) == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #286
    apInitScenario("compare intrinsic function == fn retval");
    expected = false;
    result = (Math.cos(0) == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #287
    apInitScenario("compare intrinsic function == intrinsic function");
    expected = true;
    result = (Math.cos(0) == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #288
    apInitScenario("compare intrinsic function == array element");
    expected = false;
    result = (Math.cos(0) == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #289
    apInitScenario("compare intrinsic function == object method");
    expected = false;
    result = (Math.cos(0) == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #290
    apInitScenario("compare intrinsic function == string constant");
    expected = false;
    result = (Math.cos(0) == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #291
    apInitScenario("compare intrinsic function == numeric constant");
    expected = false;
    result = (Math.cos(0) == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #292
    apInitScenario("compare intrinsic function == boolean variable");
    expected = false;
    result = (Math.cos(1) == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #293
    apInitScenario("compare intrinsic function == null");
    expected = false;
    result = (Math.cos(0) == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #294
    apInitScenario("compare intrinsic function == boolean constant");
    expected = false;
    result = (Math.cos(1) == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #295
    apInitScenario("compare array element == numeric variable");
    expected = false;
    result = (arrayVar[4] == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #296
    apInitScenario("compare array element == string variable");
    expected = false;
    result = (arrayVar[4] == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}




equ002();


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

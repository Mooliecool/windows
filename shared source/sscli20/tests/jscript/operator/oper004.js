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


var iTestID = 65324;



@if(!@aspx)
	function UDObject() {this.myValue = 88.8;}
	function UDFunction() {return 55.6;}
@else
	expando function UDObject() {this.myValue = 88.8;}
	expando function UDFunction() {return 55.6;}
@end


function oper004() 
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

    apInitTest("oper004");

    // tokenizer output scenario #393
    apInitScenario("compare numeric variable > numeric variable");
    expected = false;
    result = (numVar > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #394
    apInitScenario("compare numeric variable > string variable");
    expected = true;
    result = (numVar > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #395
    apInitScenario("compare numeric variable > date variable");
    expected = true;
    result = (numVar > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #398
    apInitScenario("compare numeric variable > fn retval");
    expected = false;
    result = (numVar > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #399
    apInitScenario("compare numeric variable > intrinsic function");
    expected = true;
    result = (numVar > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #400
    apInitScenario("compare numeric variable > array element");
    expected = true;
    result = (numVar > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #401
    apInitScenario("compare numeric variable > object method");
    expected = false;
	@cc_on
	@if (@_jscript_version < 7)
    result = (numVar > myObj.myValue);
	@else
    result = (numVar > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #402
    apInitScenario("compare numeric variable > string constant");
    expected = false;
    result = (numVar > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #403
    apInitScenario("compare numeric variable > numeric constant");
    expected = false;
    result = (numVar > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #404
    apInitScenario("compare numeric variable > boolean variable");
    expected = true;
    result = (numVar > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #405
    apInitScenario("compare numeric variable > null");
    expected = true;
    result = (numVar > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #406
    apInitScenario("compare numeric variable > boolean constant");
    expected = true;
    result = (numVar > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #407
    apInitScenario("compare string variable > numeric variable");
    expected = false;
    result = (strVar > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #408
    apInitScenario("compare string variable > string variable");
    expected = false;
    result = (strVar > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #409
    apInitScenario("compare string variable > date variable");
    expected = true;
    result = (strVar > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #412
    apInitScenario("compare string variable > fn retval");
    expected = false;
    result = (strVar > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #413
    apInitScenario("compare string variable > intrinsic function");
    expected = true;
    result = (strVar > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #414
    apInitScenario("compare string variable > array element");
    expected = false;
    result = (strVar > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #415
    apInitScenario("compare string variable > object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (strVar > myObj.myValue);
	@else
    result = (strVar > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #416
    apInitScenario("compare string variable > string constant");
    expected = false;
    result = (strVar > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #417
    apInitScenario("compare string variable > numeric constant");
    expected = false;
    result = (strVar > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #418
    apInitScenario("compare string variable > boolean variable");
    expected = true;
    result = (strVar > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #419
    apInitScenario("compare string variable > null");
    expected = true;
    result = (strVar > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #420
    apInitScenario("compare string variable > boolean constant");
    expected = true;
    result = (strVar > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #421
    apInitScenario("compare date variable > numeric variable");
    expected = false;
    result = (myDate.getTime() > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #422
    apInitScenario("compare date variable > string variable");
    expected = false;
    result = (myDate.getTime() > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #423
    apInitScenario("compare date variable > date variable");
    expected = false;
    result = (myDate.getTime() > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #426
    apInitScenario("compare date variable > fn retval");
    expected = false;
    result = (myDate.getTime() > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #427
    apInitScenario("compare date variable > intrinsic function");
    expected = false;
    result = (myDate.getTime() > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #428
    apInitScenario("compare date variable > array element");
    expected = false;
    result = (myDate.getTime() > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #429
    apInitScenario("compare date variable > object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myDate.getTime() > myObj.myValue);
	@else
    result = (myDate.getTime() > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #430
    apInitScenario("compare date variable > string constant");
    expected = false;
    result = (myDate.getTime() > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #431
    apInitScenario("compare date variable > numeric constant");
    expected = false;
    result = (myDate.getTime() > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #432
    apInitScenario("compare date variable > boolean variable");
    expected = false;
    result = (myDate.getTime() > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #433
    apInitScenario("compare date variable > null");
    expected = false;
    result = (myDate.getTime() > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #434
    apInitScenario("compare date variable > boolean constant");
    expected = false;
    result = (myDate.getTime() > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #463
    apInitScenario("compare fn retval > numeric variable");
    expected = true;
    result = (UDFunction() > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #464
    apInitScenario("compare fn retval > string variable");
    expected = true;
    result = (UDFunction() > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #465
    apInitScenario("compare fn retval > date variable");
    expected = true;
    result = (UDFunction() > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #468
    apInitScenario("compare fn retval > fn retval");
    expected = false;
    result = (UDFunction() > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #469
    apInitScenario("compare fn retval > intrinsic function");
    expected = true;
    result = (UDFunction() > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #470
    apInitScenario("compare fn retval > array element");
    expected = true;
    result = (UDFunction() > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #471
    apInitScenario("compare fn retval > object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (UDFunction() > myObj.myValue);
	@else
    result = (UDFunction() > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #472
    apInitScenario("compare fn retval > string constant");
    expected = false;
    result = (UDFunction() > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #473
    apInitScenario("compare fn retval > numeric constant");
    expected = false;
    result = (UDFunction() > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #474
    apInitScenario("compare fn retval > boolean variable");
    expected = true;
    result = (UDFunction() > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #475
    apInitScenario("compare fn retval > null");
    expected = true;
    result = (UDFunction() > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #476
    apInitScenario("compare fn retval > boolean constant");
    expected = true;
    result = (UDFunction() > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #477
    apInitScenario("compare intrinsic function > numeric variable");
    expected = false;
    result = (Math.cos(0) > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #478
    apInitScenario("compare intrinsic function > string variable");
    expected = false;
    result = (Math.cos(0) > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #479
    apInitScenario("compare intrinsic function > date variable");
    expected = true;
    result = (Math.cos(0) > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #482
    apInitScenario("compare intrinsic function > fn retval");
    expected = false;
    result = (Math.cos(0) > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #483
    apInitScenario("compare intrinsic function > intrinsic function");
    expected = false;
    result = (Math.cos(0) > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #484
    apInitScenario("compare intrinsic function > array element");
    expected = false;
    result = (Math.cos(0) > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #485
    apInitScenario("compare intrinsic function > object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (Math.cos(0) > myObj.myValue);
	@else
    result = (Math.cos(0) > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #486
    apInitScenario("compare intrinsic function > string constant");
    expected = false;
    result = (Math.cos(0) > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #487
    apInitScenario("compare intrinsic function > numeric constant");
    expected = false;
    result = (Math.cos(0) > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #488
    apInitScenario("compare intrinsic function > boolean variable");
    expected = false;
    result = (Math.cos(0) > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #489
    apInitScenario("compare intrinsic function > null");
    expected = true;
    result = (Math.cos(0) > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #490
    apInitScenario("compare intrinsic function > boolean constant");
    expected = false;
    result = (Math.cos(0) > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #491
    apInitScenario("compare array element > numeric variable");
    expected = false;
    result = (arrayVar[4] > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}

oper004();


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

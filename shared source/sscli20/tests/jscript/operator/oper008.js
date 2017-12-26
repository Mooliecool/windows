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


var iTestID = 65328;



@if(!@aspx)
	function UDObject() {this.myValue = 88.8;}
	function UDFunction() {return 55.6;}
@else
	expando function UDObject() {this.myValue = 88.8;}
	expando function UDFunction() {return 55.6;}
@end


function oper008() 
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

    apInitTest("oper008");

    // tokenizer output scenario #785
    apInitScenario("compare numeric variable >= numeric variable");
    expected = true;
    result = (numVar >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #786
    apInitScenario("compare numeric variable >= string variable");
    expected = true;
    result = (numVar >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #787
    apInitScenario("compare numeric variable >= date variable");
    expected = true;
    result = (numVar >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #790
    apInitScenario("compare numeric variable >= fn retval");
    expected = false;
    result = (numVar >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #791
    apInitScenario("compare numeric variable >= intrinsic function");
    expected = true;
    result = (numVar >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #792
    apInitScenario("compare numeric variable >= array element");
    expected = true;
    result = (numVar >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #793
    apInitScenario("compare numeric variable >= object method");
    expected = false;
	@cc_on
	@if (@_jscript_version < 7)
    result = (numVar >= myObj.myValue);
	@else
    result = (numVar >= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #794
    apInitScenario("compare numeric variable >= string constant");
    expected = false;
    result = (numVar >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #795
    apInitScenario("compare numeric variable >= numeric constant");
    expected = false;
    result = (numVar >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #796
    apInitScenario("compare numeric variable >= boolean variable");
    expected = true;
    result = (numVar >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #797
    apInitScenario("compare numeric variable >= null");
    expected = true;
    result = (numVar >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #798
    apInitScenario("compare numeric variable >= boolean constant");
    expected = true;
    result = (numVar >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #799
    apInitScenario("compare string variable >= numeric variable");
    expected = false;
    result = (strVar >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #800
    apInitScenario("compare string variable >= string variable");
    expected = true;
    result = (strVar >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #801
    apInitScenario("compare string variable >= date variable");
    expected = true;
    result = (strVar >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #804
    apInitScenario("compare string variable >= fn retval");
    expected = false;
    result = (strVar >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #805
    apInitScenario("compare string variable >= intrinsic function");
    expected = true;
    result = (strVar >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #806
    apInitScenario("compare string variable >= array element");
    expected = false;
    result = (strVar >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #807
    apInitScenario("compare string variable >= object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (strVar >= myObj.myValue);
	@else
    result = (strVar >= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #808
    apInitScenario("compare string variable >= string constant");
    expected = false;
    result = (strVar >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #809
    apInitScenario("compare string variable >= numeric constant");
    expected = false;
    result = (strVar >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #810
    apInitScenario("compare string variable >= boolean variable");
    expected = true;
    result = (strVar >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #811
    apInitScenario("compare string variable >= null");
    expected = true;
    result = (strVar >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #812
    apInitScenario("compare string variable >= boolean constant");
    expected = true;
    result = (strVar >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #813
    apInitScenario("compare date variable >= numeric variable");
    expected = false;
    result = (myDate.getTime() >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #814
    apInitScenario("compare date variable >= string variable");
    expected = false;
    result = (myDate.getTime() >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #815
    apInitScenario("compare date variable >= date variable");
    expected = true;
    result = (myDate.getTime() >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #818
    apInitScenario("compare date variable >= fn retval");
    expected = false;
    result = (myDate.getTime() >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #819
    apInitScenario("compare date variable >= intrinsic function");
    expected = false;
    result = (myDate.getTime() >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #820
    apInitScenario("compare date variable >= array element");
    expected = false;
    result = (myDate.getTime() >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #821
    apInitScenario("compare date variable >= object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myDate.getTime() >= myObj.myValue);
	@else
    result = (myDate.getTime() >= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #822
    apInitScenario("compare date variable >= string constant");
    expected = false;
    result = (myDate.getTime() >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #823
    apInitScenario("compare date variable >= numeric constant");
    expected = false;
    result = (myDate.getTime() >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #824
    apInitScenario("compare date variable >= boolean variable");
    expected = false;
    result = (myDate.getTime() >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #825
    apInitScenario("compare date variable >= null");
    expected = true;
    result = (myDate.getTime() >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #826
    apInitScenario("compare date variable >= boolean constant");
    expected = false;
    result = (myDate.getTime() >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #855
    apInitScenario("compare fn retval >= numeric variable");
    expected = true;
    result = (UDFunction() >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #856
    apInitScenario("compare fn retval >= string variable");
    expected = true;
    result = (UDFunction() >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #857
    apInitScenario("compare fn retval >= date variable");
    expected = true;
    result = (UDFunction() >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #860
    apInitScenario("compare fn retval >= fn retval");
    expected = true;
    result = (UDFunction() >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #861
    apInitScenario("compare fn retval >= intrinsic function");
    expected = true;
    result = (UDFunction() >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #862
    apInitScenario("compare fn retval >= array element");
    expected = true;
    result = (UDFunction() >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #863
    apInitScenario("compare fn retval >= object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (UDFunction() >= myObj.myValue);
	@else
    result = (UDFunction() >= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #864
    apInitScenario("compare fn retval >= string constant");
    expected = false;
    result = (UDFunction() >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #865
    apInitScenario("compare fn retval >= numeric constant");
    expected = false;
    result = (UDFunction() >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #866
    apInitScenario("compare fn retval >= boolean variable");
    expected = true;
    result = (UDFunction() >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #867
    apInitScenario("compare fn retval >= null");
    expected = true;
    result = (UDFunction() >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #868
    apInitScenario("compare fn retval >= boolean constant");
    expected = true;
    result = (UDFunction() >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #869
    apInitScenario("compare intrinsic function >= numeric variable");
    expected = false;
    result = (Math.cos(0) >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #870
    apInitScenario("compare intrinsic function >= string variable");
    expected = false;
    result = (Math.cos(0) >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #871
    apInitScenario("compare intrinsic function >= date variable");
    expected = true;
    result = (Math.cos(0) >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #874
    apInitScenario("compare intrinsic function >= fn retval");
    expected = false;
    result = (Math.cos(0) >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #875
    apInitScenario("compare intrinsic function >= intrinsic function");
    expected = true;
    result = (Math.cos(0) >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #876
    apInitScenario("compare intrinsic function >= array element");
    expected = false;
    result = (Math.cos(0) >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #877
    apInitScenario("compare intrinsic function >= object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (Math.cos(0) >= myObj.myValue);
	@else
    result = (Math.cos(0) >= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #878
    apInitScenario("compare intrinsic function >= string constant");
    expected = false;
    result = (Math.cos(0) >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #879
    apInitScenario("compare intrinsic function >= numeric constant");
    expected = false;
    result = (Math.cos(0) >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #880
    apInitScenario("compare intrinsic function >= boolean variable");
    expected = true;
    result = (Math.cos(0) >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #881
    apInitScenario("compare intrinsic function >= null");
    expected = true;
    result = (Math.cos(0) >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #882
    apInitScenario("compare intrinsic function >= boolean constant");
    expected = true;
    result = (Math.cos(0) >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #883
    apInitScenario("compare array element >= numeric variable");
    expected = false;
    result = (arrayVar[4] >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}

oper008();


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

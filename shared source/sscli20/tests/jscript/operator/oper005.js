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


var iTestID = 65325;



@if(!@aspx)
	function UDObject() {this.myValue = 88.8;}
	function UDFunction() {return 55.6;}
@else
	expando function UDObject() {this.myValue = 88.8;}
	expando function UDFunction() {return 55.6;}
@end


function oper005() 
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

    apInitTest("oper005");

    // tokenizer output scenario #492
    apInitScenario("compare array element > string variable");
    expected = true;
    result = (arrayVar[4] > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #493
    apInitScenario("compare array element > date variable");
    expected = true;
    result = (arrayVar[4] > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #496
    apInitScenario("compare array element > fn retval");
    expected = false;
    result = (arrayVar[4] > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #497
    apInitScenario("compare array element > intrinsic function");
    expected = true;
    result = (arrayVar[4] > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #498
    apInitScenario("compare array element > array element");
    expected = false;
    result = (arrayVar[4] > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #499
    apInitScenario("compare array element > object method");
    expected = false;
	@cc_on
	@if (@_jscript_version < 7)
    result = (arrayVar[4] > myObj.myValue);
	@else
    result = (arrayVar[4] > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #500
    apInitScenario("compare array element > string constant");
    expected = false;
    result = (arrayVar[4] > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #501
    apInitScenario("compare array element > numeric constant");
    expected = false;
    result = (arrayVar[4] > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #502
    apInitScenario("compare array element > boolean variable");
    expected = true;
    result = (arrayVar[4] > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #503
    apInitScenario("compare array element > null");
    expected = true;
    result = (arrayVar[4] > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #504
    apInitScenario("compare array element > boolean constant");
    expected = true;
    result = (arrayVar[4] > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #505
    apInitScenario("compare object method > numeric variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > numVar);
	@else
    result = (88.8 > numVar);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #506
    apInitScenario("compare object method > string variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > strVar);
	@else
    result = (88.8 > strVar);
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #507
    apInitScenario("compare object method > date variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > myDate.getTime());
	@else
    result = (88.8 > myDate.getTime());
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #510
    apInitScenario("compare object method > fn retval");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > UDFunction());
	@else
    result = (88.8 > UDFunction());
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #511
    apInitScenario("compare object method > intrinsic function");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > Math.cos(0));
	@else
    result = (88.8 > Math.cos(0));
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #512
    apInitScenario("compare object method > array element");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > arrayVar[4]);
	@else
    result = (88.8 > arrayVar[4]);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #513
    apInitScenario("compare object method > object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > myObj.myValue);
	@else
    result = (88.8 > 88.8);
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #514
    apInitScenario("compare object method > string constant");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > "200.3");
	@else
    result = (88.8 > "200.3");
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #515
    apInitScenario("compare object method > numeric constant");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > 300.2);
	@else
    result = (88.8 > 300.2);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #516
    apInitScenario("compare object method > boolean variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > myBool);
	@else
    result = (88.8 > myBool);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #517
    apInitScenario("compare object method > null");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > null);
	@else
    result = (88.8 > null);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #518
    apInitScenario("compare object method > boolean constant");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue > true);
	@else
    result = (88.8 > true);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #519
    apInitScenario("compare string constant > numeric variable");
    expected = true;
    result = ("200.3" > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #520
    apInitScenario("compare string constant > string variable");
    expected = true;
    result = ("200.3" > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #521
    apInitScenario("compare string constant > date variable");
    expected = true;
    result = ("200.3" > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #524
    apInitScenario("compare string constant > fn retval");
    expected = true;
    result = ("200.3" > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #525
    apInitScenario("compare string constant > intrinsic function");
    expected = true;
    result = ("200.3" > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #526
    apInitScenario("compare string constant > array element");
    expected = true;
    result = ("200.3" > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #527
    apInitScenario("compare string constant > object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = ("200.3" > myObj.myValue);
	@else
    result = ("200.3" > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #528
    apInitScenario("compare string constant > string constant");
    expected = false;
    result = ("200.3" > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #529
    apInitScenario("compare string constant > numeric constant");
    expected = false;
    result = ("200.3" > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #530
    apInitScenario("compare string constant > boolean variable");
    expected = true;
    result = ("200.3" > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #531
    apInitScenario("compare string constant > null");
    expected = true;
    result = ("200.3" > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #532
    apInitScenario("compare string constant > boolean constant");
    expected = true;
    result = ("200.3" > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #533
    apInitScenario("compare numeric constant > numeric variable");
    expected = true;
    result = (300.2 > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #534
    apInitScenario("compare numeric constant > string variable");
    expected = true;
    result = (300.2 > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #535
    apInitScenario("compare numeric constant > date variable");
    expected = true;
    result = (300.2 > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #538
    apInitScenario("compare numeric constant > fn retval");
    expected = true;
    result = (300.2 > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #539
    apInitScenario("compare numeric constant > intrinsic function");
    expected = true;
    result = (300.2 > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #540
    apInitScenario("compare numeric constant > array element");
    expected = true;
    result = (300.2 > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #541
    apInitScenario("compare numeric constant > object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (300.2 > myObj.myValue);
	@else
    result = (300.2 > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #542
    apInitScenario("compare numeric constant > string constant");
    expected = true;
    result = (300.2 > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #543
    apInitScenario("compare numeric constant > numeric constant");
    expected = false;
    result = (300.2 > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #544
    apInitScenario("compare numeric constant > boolean variable");
    expected = true;
    result = (300.2 > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #545
    apInitScenario("compare numeric constant > null");
    expected = true;
    result = (300.2 > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #546
    apInitScenario("compare numeric constant > boolean constant");
    expected = true;
    result = (300.2 > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #547
    apInitScenario("compare boolean variable > numeric variable");
    expected = false;
    result = (myBool > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #548
    apInitScenario("compare boolean variable > string variable");
    expected = false;
    result = (myBool > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #549
    apInitScenario("compare boolean variable > date variable");
    expected = true;
    result = (myBool > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #552
    apInitScenario("compare boolean variable > fn retval");
    expected = false;
    result = (myBool > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #553
    apInitScenario("compare boolean variable > intrinsic function");
    expected = false;
    result = (myBool > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #554
    apInitScenario("compare boolean variable > array element");
    expected = false;
    result = (myBool > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #555
    apInitScenario("compare boolean variable > object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myBool > myObj.myValue);
	@else
    result = (myBool > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #556
    apInitScenario("compare boolean variable > string constant");
    expected = false;
    result = (myBool > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #557
    apInitScenario("compare boolean variable > numeric constant");
    expected = false;
    result = (myBool > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #558
    apInitScenario("compare boolean variable > boolean variable");
    expected = false;
    result = (myBool > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #559
    apInitScenario("compare boolean variable > null");
    expected = true;
    result = (myBool > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #560
    apInitScenario("compare boolean variable > boolean constant");
    expected = false;
    result = (myBool > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #561
    apInitScenario("compare null > numeric variable");
    expected = false;
    result = (null > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #562
    apInitScenario("compare null > string variable");
    expected = false;
    result = (null > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #563
    apInitScenario("compare null > date variable");
    expected = false;
    result = (null > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #566
    apInitScenario("compare null > fn retval");
    expected = false;
    result = (null > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #567
    apInitScenario("compare null > intrinsic function");
    expected = false;
    result = (null > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #568
    apInitScenario("compare null > array element");
    expected = false;
    result = (null > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #569
    apInitScenario("compare null > object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (null > myObj.myValue);
	@else
    result = (null > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #570
    apInitScenario("compare null > string constant");
    expected = false;
    result = (null > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #571
    apInitScenario("compare null > numeric constant");
    expected = false;
    result = (null > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #572
    apInitScenario("compare null > boolean variable");
    expected = false;
    result = (null > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #573
    apInitScenario("compare null > null");
    expected = false;
    result = (null > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #574
    apInitScenario("compare null > boolean constant");
    expected = false;
    result = (null > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #575
    apInitScenario("compare boolean constant > numeric variable");
    expected = false;
    result = (true > numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #576
    apInitScenario("compare boolean constant > string variable");
    expected = false;
    result = (true > strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #577
    apInitScenario("compare boolean constant > date variable");
    expected = true;
    result = (true > myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #580
    apInitScenario("compare boolean constant > fn retval");
    expected = false;
    result = (true > UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #581
    apInitScenario("compare boolean constant > intrinsic function");
    expected = false;
    result = (true > Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #582
    apInitScenario("compare boolean constant > array element");
    expected = false;
    result = (true > arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #583
    apInitScenario("compare boolean constant > object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (true > myObj.myValue);
	@else
    result = (true > 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #584
    apInitScenario("compare boolean constant > string constant");
    expected = false;
    result = (true > "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #585
    apInitScenario("compare boolean constant > numeric constant");
    expected = false;
    result = (true > 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #586
    apInitScenario("compare boolean constant > boolean variable");
    expected = false;
    result = (true > myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #587
    apInitScenario("compare boolean constant > null");
    expected = true;
    result = (true > null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #588
    apInitScenario("compare boolean constant > boolean constant");
    expected = false;
    result = (true > true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}

oper005();


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

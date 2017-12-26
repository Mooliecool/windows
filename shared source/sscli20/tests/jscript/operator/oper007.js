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


var iTestID = 65327;



@if(!@aspx)
	function UDObject() {this.myValue = 88.8;}
	function UDFunction() {return 55.6;}
@else
	expando function UDObject() {this.myValue = 88.8;}
	expando function UDFunction() {return 55.6;}
@end


function oper007() 
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

    apInitTest("oper007");

    // tokenizer output scenario #689
    apInitScenario("compare array element <= date variable");
    expected = false;
    result = (arrayVar[4] <= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #692
    apInitScenario("compare array element <= fn retval");
    expected = true;
    result = (arrayVar[4] <= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #693
    apInitScenario("compare array element <= intrinsic function");
    expected = false;
    result = (arrayVar[4] <= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #694
    apInitScenario("compare array element <= array element");
    expected = true;
    result = (arrayVar[4] <= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #695
    apInitScenario("compare array element <= object method");
    expected = true;
	@cc_on
	@if (@_jscript_version < 7)
    result = (arrayVar[4] <= myObj.myValue);
	@else
    result = (arrayVar[4] <= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #696
    apInitScenario("compare array element <= string constant");
    expected = true;
    result = (arrayVar[4] <= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #697
    apInitScenario("compare array element <= numeric constant");
    expected = true;
    result = (arrayVar[4] <= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #698
    apInitScenario("compare array element <= boolean variable");
    expected = false;
    result = (arrayVar[4] <= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #699
    apInitScenario("compare array element <= null");
    expected = false;
    result = (arrayVar[4] <= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #700
    apInitScenario("compare array element <= boolean constant");
    expected = false;
    result = (arrayVar[4] <= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #701
    apInitScenario("compare object method <= numeric variable");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= numVar);
	@else
    result = (88.8 <= numVar);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #702
    apInitScenario("compare object method <= string variable");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= strVar);
	@else
    result = (88.8 <= strVar);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #703
    apInitScenario("compare object method <= date variable");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= myDate.getTime());
	@else
    result = (88.8 <= myDate.getTime());
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #706
    apInitScenario("compare object method <= fn retval");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= UDFunction());
	@else
    result = (88.8 <= UDFunction());
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #707
    apInitScenario("compare object method <= intrinsic function");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= Math.cos(0));
	@else
    result = (88.8 <= Math.cos(0));
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #708
    apInitScenario("compare object method <= array element");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= arrayVar[4]);
	@else
    result = (88.8 <= arrayVar[4]);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #709
    apInitScenario("compare object method <= object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= myObj.myValue);
	@else
    result = (88.8 <= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #710
    apInitScenario("compare object method <= string constant");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= "200.3");
	@else
    result = (88.8 <= "200.3");
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #711
    apInitScenario("compare object method <= numeric constant");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= 300.2);
	@else
    result = (88.8 <= 300.2);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #712
    apInitScenario("compare object method <= boolean variable");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= myBool);
	@else
    result = (88.8 <= myBool);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #713
    apInitScenario("compare object method <= null");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= null);
	@else
    result = (88.8 <= null);
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #714
    apInitScenario("compare object method <= boolean constant");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue <= true);
	@else
    result = (88.8 <= true);
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #715
    apInitScenario("compare string constant <= numeric variable");
    expected = false;
    result = ("200.3" <= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #716
    apInitScenario("compare string constant <= string variable");
    expected = false;
    result = ("200.3" <= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #717
    apInitScenario("compare string constant <= date variable");
    expected = false;
    result = ("200.3" <= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #720
    apInitScenario("compare string constant <= fn retval");
    expected = false;
    result = ("200.3" <= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #721
    apInitScenario("compare string constant <= intrinsic function");
    expected = false;
    result = ("200.3" <= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #722
    apInitScenario("compare string constant <= array element");
    expected = false;
    result = ("200.3" <= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #723
    apInitScenario("compare string constant <= object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = ("200.3" <= myObj.myValue);
	@else
    result = ("200.3" <= 88.8);
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #724
    apInitScenario("compare string constant <= string constant");
    expected = true;
    result = ("200.3" <= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #725
    apInitScenario("compare string constant <= numeric constant");
    expected = true;
    result = ("200.3" <= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #726
    apInitScenario("compare string constant <= boolean variable");
    expected = false;
    result = ("200.3" <= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #727
    apInitScenario("compare string constant <= null");
    expected = false;
    result = ("200.3" <= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #728
    apInitScenario("compare string constant <= boolean constant");
    expected = false;
    result = ("200.3" <= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #729
    apInitScenario("compare numeric constant <= numeric variable");
    expected = false;
    result = (300.2 <= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #730
    apInitScenario("compare numeric constant <= string variable");
    expected = false;
    result = (300.2 <= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #731
    apInitScenario("compare numeric constant <= date variable");
    expected = false;
    result = (300.2 <= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #734
    apInitScenario("compare numeric constant <= fn retval");
    expected = false;
    result = (300.2 <= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #735
    apInitScenario("compare numeric constant <= intrinsic function");
    expected = false;
    result = (300.2 <= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #736
    apInitScenario("compare numeric constant <= array element");
    expected = false;
    result = (300.2 <= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #737
    apInitScenario("compare numeric constant <= object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (300.2 <= myObj.myValue);
	@else
    result = (300.2 <= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #738
    apInitScenario("compare numeric constant <= string constant");
    expected = true;
    result = (-300.2 <= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #739
    apInitScenario("compare numeric constant <= numeric constant");
    expected = true;
    result = (300.2 <= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #740
    apInitScenario("compare numeric constant <= boolean variable");
    expected = true;
    result = (-300.2 <= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #741
    apInitScenario("compare numeric constant <= null");
    expected = true;
    result = (-300.2 <= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #742
    apInitScenario("compare numeric constant <= boolean constant");
    expected = true;
    result = (-300.2 <= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #743
    apInitScenario("compare boolean variable <= numeric variable");
    expected = true;
    result = (myBool <= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #744
    apInitScenario("compare boolean variable <= string variable");
    expected = true;
    result = (myBool <= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #745
    apInitScenario("compare boolean variable <= date variable");
    expected = false;
    result = (myBool <= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #748
    apInitScenario("compare boolean variable <= fn retval");
    expected = true;
    result = (myBool <= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #749
    apInitScenario("compare boolean variable <= intrinsic function");
    expected = true;
    result = (myBool <= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #750
    apInitScenario("compare boolean variable <= array element");
    expected = true;
    result = (myBool <= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #751
    apInitScenario("compare boolean variable <= object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myBool <= myObj.myValue);
	@else
    result = (myBool <= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #752
    apInitScenario("compare boolean variable <= string constant");
    expected = true;
    result = (myBool <= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #753
    apInitScenario("compare boolean variable <= numeric constant");
    expected = true;
    result = (myBool <= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #754
    apInitScenario("compare boolean variable <= boolean variable");
    expected = true;
    result = (myBool <= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #755
    apInitScenario("compare boolean variable <= null");
    expected = false;
    result = (myBool <= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #756
    apInitScenario("compare boolean variable <= boolean constant");
    expected = true;
    result = (myBool <= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #757
    apInitScenario("compare null <= numeric variable");
    expected = true;
    result = (null <= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #758
    apInitScenario("compare null <= string variable");
    expected = true;
    result = (null <= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #759
    apInitScenario("compare null <= date variable");
    expected = true;
    result = (null <= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #762
    apInitScenario("compare null <= fn retval");
    expected = true;
    result = (null <= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #763
    apInitScenario("compare null <= intrinsic function");
    expected = true;
    result = (null <= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #764
    apInitScenario("compare null <= array element");
    expected = true;
    result = (null <= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #765
    apInitScenario("compare null <= object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (null <= myObj.myValue);
	@else
    result = (null <= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #766
    apInitScenario("compare null <= string constant");
    expected = true;
    result = (null <= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #767
    apInitScenario("compare null <= numeric constant");
    expected = true;
    result = (null <= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #768
    apInitScenario("compare null <= boolean variable");
    expected = true;
    result = (null <= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #769
    apInitScenario("compare null <= null");
    expected = true;
    result = (null <= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #770
    apInitScenario("compare null <= boolean constant");
    expected = true;
    result = (null <= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #771
    apInitScenario("compare boolean constant <= numeric variable");
    expected = true;
    result = (true <= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #772
    apInitScenario("compare boolean constant <= string variable");
    expected = true;
    result = (true <= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #773
    apInitScenario("compare boolean constant <= date variable");
    expected = false;
    result = (true <= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #776
    apInitScenario("compare boolean constant <= fn retval");
    expected = true;
    result = (true <= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #777
    apInitScenario("compare boolean constant <= intrinsic function");
    expected = true;
    result = (true <= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #778
    apInitScenario("compare boolean constant <= array element");
    expected = true;
    result = (true <= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #779
    apInitScenario("compare boolean constant <= object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (true <= myObj.myValue);
	@else
    result = (true <= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #780
    apInitScenario("compare boolean constant <= string constant");
    expected = true;
    result = (true <= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #781
    apInitScenario("compare boolean constant <= numeric constant");
    expected = true;
    result = (true <= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #782
    apInitScenario("compare boolean constant <= boolean variable");
    expected = true;
    result = (true <= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #783
    apInitScenario("compare boolean constant <= null");
    expected = false;
    result = (true <= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #784
    apInitScenario("compare boolean constant <= boolean constant");
    expected = true;
    result = (true <= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}

oper007();


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

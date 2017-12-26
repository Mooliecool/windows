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


var iTestID = 65323;



@if(!@aspx)
	function UDObject() {this.myValue = 88.8;}
	function UDFunction() {return 55.6;}
@else
	expando function UDObject() {this.myValue = 88.8;}
	expando function UDFunction() {return 55.6;}
@end


function oper003() {
    // Added to handle strict mode in JScript 7.0
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

    apInitTest("oper003");

    // tokenizer output scenario #99
    apInitScenario("compare array element < numeric variable");
    expected = true;
    result = (arrayVar[4] < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #100
    apInitScenario("compare array element < string variable");
    expected = false;
    result = (arrayVar[4] < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #101
    apInitScenario("compare array element < date variable");
    expected = false;
    result = (arrayVar[4] < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #104
    apInitScenario("compare array element < fn retval");
    expected = true;
    result = (arrayVar[4] < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #105
    apInitScenario("compare array element < intrinsic function");
    expected = false;
    result = (arrayVar[4] < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #106
    apInitScenario("compare array element < array element");
    expected = false;
    result = (arrayVar[4] < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #107
    apInitScenario("compare array element < object method");
    expected = true;
	@cc_on
	@if (@_jscript_version < 7)
    result = (arrayVar[4] < myObj.myValue);
	@else
    result = (arrayVar[4] < 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #108
    apInitScenario("compare array element < string constant");
    expected = true;
    result = (arrayVar[4] < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #109
    apInitScenario("compare array element < numeric constant");
    expected = true;
    result = (arrayVar[4] < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #110
    apInitScenario("compare array element < boolean variable");
    expected = false;
    result = (arrayVar[4] < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #111
    apInitScenario("compare array element < null");
    expected = false;
    result = (arrayVar[4] < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #112
    apInitScenario("compare array element < boolean constant");
    expected = false;
    result = (arrayVar[4] < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #113
    apInitScenario("compare object method < numeric variable");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < numVar);
	@else
    result = (88.8 < numVar);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #114
    apInitScenario("compare object method < string variable");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < strVar);
	@else
    result = (88.8 < strVar);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #115
    apInitScenario("compare object method < date variable");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < myDate.getTime());
	@else
    result = (88.8 < myDate.getTime());
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #118
    apInitScenario("compare object method < fn retval");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < UDFunction());
	@else
    result = (88.8 < UDFunction());
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #119
    apInitScenario("compare object method < intrinsic function");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < Math.cos(0));
	@else
    result = (88.8 < Math.cos(0));
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #120
    apInitScenario("compare object method < array element");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < arrayVar[4]);
	@else
    result = (88.8 < arrayVar[4]);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #121
    apInitScenario("compare object method < object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < myObj.myValue);
	@else
    result = (88.8 < 88.8);
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #122
    apInitScenario("compare object method < string constant");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < "200.3");
	@else
    result = (88.8 < "200.3");
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #123
    apInitScenario("compare object method < numeric constant");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < 300.2);
	@else
    result = (88.8 < 300.2);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #124
    apInitScenario("compare object method < boolean variable");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < myBool);
	@else
    result = (88.8 < myBool);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #125
    apInitScenario("compare object method < null");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < null);
	@else
    result = (88.8 < null);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #126
    apInitScenario("compare object method < boolean constant");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue < true);
	@else
    result = (88.8 < true);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #127
    apInitScenario("compare string constant < numeric variable");
    expected = false;
    result = ("200.3" < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #128
    apInitScenario("compare string constant < string variable");
    expected = false;
    result = ("200.3" < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #129
    apInitScenario("compare string constant < date variable");
    expected = false;
    result = ("200.3" < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #132
    apInitScenario("compare string constant < fn retval");
    expected = false;
    result = ("200.3" < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #133
    apInitScenario("compare string constant < intrinsic function");
    expected = false;
    result = ("200.3" < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #134
    apInitScenario("compare string constant < array element");
    expected = false;
    result = ("200.3" < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #135
    apInitScenario("compare string constant < object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = ("200.3" < myObj.myValue);
	@else
    result = ("200.3" < 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #136
    apInitScenario("compare string constant < string constant");
    expected = false;
    result = ("200.3" < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #137
    apInitScenario("compare string constant < numeric constant");
    expected = true;
    result = ("200.3" < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #138
    apInitScenario("compare string constant < boolean variable");
    expected = false;
    result = ("200.3" < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #139
    apInitScenario("compare string constant < null");
    expected = true;
    result = ("-200.3" < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #140
    apInitScenario("compare string constant < boolean constant");
    expected = false;
    result = ("200.3" < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #141
    apInitScenario("compare numeric constant < numeric variable");
    expected = true;
    result = (-300.2 < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #142
    apInitScenario("compare numeric constant < string variable");
    expected = true;
    result = (-300.2 < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #143
    apInitScenario("compare numeric constant < date variable");
    expected = true;
    result = (-300.2 < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #146
    apInitScenario("compare numeric constant < fn retval");
    expected = true;
    result = (-300.2 < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #147
    apInitScenario("compare numeric constant < intrinsic function");
    expected = true;
    result = (-300.2 < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #148
    apInitScenario("compare numeric constant < array element");
    expected = true;
    result = (-300.2 < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #149
    apInitScenario("compare numeric constant < object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (-300.2 < myObj.myValue);
	@else
    result = (-300.2 < 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #150
    apInitScenario("compare numeric constant < string constant");
    expected = false;
    result = (300.2 < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #151
    apInitScenario("compare numeric constant < numeric constant");
    expected = false;
    result = (300.2 < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #152
    apInitScenario("compare numeric constant < boolean variable");
    expected = true;
    result = (-300.2 < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #153
    apInitScenario("compare numeric constant < null");
    expected = true;
    result = (-300.2 < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #154
    apInitScenario("compare numeric constant < boolean constant");
    expected = true;
    result = (-300.2 < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #155
    apInitScenario("compare boolean variable < numeric variable");
    expected = true;
    result = (myBool < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #156
    apInitScenario("compare boolean variable < string variable");
    expected = true;
    result = (myBool < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #157
    apInitScenario("compare boolean variable < date variable");
    expected = false;
    result = (myBool < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #160
    apInitScenario("compare boolean variable < fn retval");
    expected = true;
    result = (myBool < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #161
    apInitScenario("compare boolean variable < intrinsic function");
    expected = false;
    result = (myBool < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #162
    apInitScenario("compare boolean variable < array element");
    expected = true;
    result = (myBool < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #163
    apInitScenario("compare boolean variable < object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myBool < myObj.myValue);
	@else
    result = (myBool < 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #164
    apInitScenario("compare boolean variable < string constant");
    expected = true;
    result = (myBool < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #165
    apInitScenario("compare boolean variable < numeric constant");
    expected = true;
    result = (myBool < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #166
    apInitScenario("compare boolean variable < boolean variable");
    expected = false;
    result = (myBool < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #167
    apInitScenario("compare boolean variable < null");
    expected = false;
    result = (myBool < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #168
    apInitScenario("compare boolean variable < boolean constant");
    expected = false;
    result = (myBool < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #169
    apInitScenario("compare null < numeric variable");
    expected = true;
    result = (null < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #170
    apInitScenario("compare null < string variable");
    expected = true;
    result = (null < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #171
    apInitScenario("compare null < date variable");
    expected = false;
    result = (null < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #174
    apInitScenario("compare null < fn retval");
    expected = true;
    result = (null < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #175
    apInitScenario("compare null < intrinsic function");
    expected = true;
    result = (null < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #176
    apInitScenario("compare null < array element");
    expected = true;
    result = (null < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #177
    apInitScenario("compare null < object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (null < myObj.myValue);
	@else
    result = (null < 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #178
    apInitScenario("compare null < string constant");
    expected = true;
    result = (null < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #179
    apInitScenario("compare null < numeric constant");
    expected = true;
    result = (null < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #180
    apInitScenario("compare null < boolean variable");
    expected = true;
    result = (null < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #181
    apInitScenario("compare null < null");
    expected = false;
    result = (null < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #182
    apInitScenario("compare null < boolean constant");
    expected = true;
    result = (null < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #183
    apInitScenario("compare boolean constant < numeric variable");
    expected = true;
    result = (true < numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #184
    apInitScenario("compare boolean constant < string variable");
    expected = true;
    result = (true < strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #185
    apInitScenario("compare boolean constant < date variable");
    expected = false;
    result = (true < myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #188
    apInitScenario("compare boolean constant < fn retval");
    expected = true;
    result = (true < UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #189
    apInitScenario("compare boolean constant < intrinsic function");
    expected = false;
    result = (true < Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #190
    apInitScenario("compare boolean constant < array element");
    expected = true;
    result = (true < arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #191
    apInitScenario("compare boolean constant < object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (true < myObj.myValue);
	@else
    result = (true < 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #192
    apInitScenario("compare boolean constant < string constant");
    expected = true;
    result = (true < "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #193
    apInitScenario("compare boolean constant < numeric constant");
    expected = true;
    result = (true < 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #194
    apInitScenario("compare boolean constant < boolean variable");
    expected = false;
    result = (true < myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #195
    apInitScenario("compare boolean constant < null");
    expected = false;
    result = (true < null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #196
    apInitScenario("compare boolean constant < boolean constant");
    expected = false;
    result = (true < true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}

oper003();


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

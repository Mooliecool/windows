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


var iTestID = 65329;



@if(!@aspx)
	function UDObject() {this.myValue = 88.8;}
	function UDFunction() {return 55.6;}
@else
	expando function UDObject() {this.myValue = 88.8;}
	expando function UDFunction() {return 55.6;}
@end


function oper009() 
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

    apInitTest("oper009");

    // tokenizer output scenario #884
    apInitScenario("compare array element >= string variable");
    expected = true;
    result = (arrayVar[4] >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #885
    apInitScenario("compare array element >= date variable");
    expected = true;
    result = (arrayVar[4] >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #888
    apInitScenario("compare array element >= fn retval");
    expected = false;
    result = (arrayVar[4] >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #889
    apInitScenario("compare array element >= intrinsic function");
    expected = true;
    result = (arrayVar[4] >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #890
    apInitScenario("compare array element >= array element");
    expected = true;
    result = (arrayVar[4] >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #892
    apInitScenario("compare array element >= string constant");
    expected = false;
    result = (arrayVar[4] >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #893
    apInitScenario("compare array element >= numeric constant");
    expected = false;
    result = (arrayVar[4] >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #894
    apInitScenario("compare array element >= boolean variable");
    expected = true;
    result = (arrayVar[4] >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #895
    apInitScenario("compare array element >= null");
    expected = true;
    result = (arrayVar[4] >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #896
    apInitScenario("compare array element >= boolean constant");
    expected = true;
    result = (arrayVar[4] >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #897
    apInitScenario("compare object method >= numeric variable");
    expected = true;
	@cc_on
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= numVar);
	@else
    result = (88.8 >= numVar);
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #898
    apInitScenario("compare object method >= string variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= strVar);
	@else
    result = (88.8 >= strVar);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #899
    apInitScenario("compare object method >= date variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= myDate.getTime());
	@else
    result = (88.8 >= myDate.getTime());
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #902
    apInitScenario("compare object method >= fn retval");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= UDFunction());
	@else
    result = (88.8 >= UDFunction());
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #903
    apInitScenario("compare object method >= intrinsic function");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= Math.cos(0));
	@else
    result = (88.8 >= Math.cos(0));
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #904
    apInitScenario("compare object method >= array element");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= arrayVar[4]);
	@else
    result = (88.8 >= arrayVar[4]);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #905
    apInitScenario("compare object method >= object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= myObj.myValue);
	@else
    result = (88.8 >= 88.8);
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #906
    apInitScenario("compare object method >= string constant");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= "200.3");
	@else
    result = (88.8 >= "200.3");
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #907
    apInitScenario("compare object method >= numeric constant");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= 300.2);
	@else
    result = (88.8 >= 300.2);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #908
    apInitScenario("compare object method >= boolean variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= myBool);
	@else
    result = (88.8 >= myBool);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #909
    apInitScenario("compare object method >= null");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= null);
	@else
    result = (88.8 >= null);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #910
    apInitScenario("compare object method >= boolean constant");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue >= true);
	@else
    result = (88.8 >= true);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #911
    apInitScenario("compare string constant >= numeric variable");
    expected = true;
    result = ("200.3" >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #912
    apInitScenario("compare string constant >= string variable");
    expected = true;
    result = ("200.3" >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #913
    apInitScenario("compare string constant >= date variable");
    expected = true;
    result = ("200.3" >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #916
    apInitScenario("compare string constant >= fn retval");
    expected = true;
    result = ("200.3" >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #917
    apInitScenario("compare string constant >= intrinsic function");
    expected = true;
    result = ("200.3" >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #918
    apInitScenario("compare string constant >= array element");
    expected = true;
    result = ("200.3" >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #920
    apInitScenario("compare string constant >= string constant");
    expected = true;
    result = ("200.3" >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #921
    apInitScenario("compare string constant >= numeric constant");
    expected = false;
    result = ("200.3" >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #922
    apInitScenario("compare string constant >= boolean variable");
    expected = true;
    result = ("200.3" >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #923
    apInitScenario("compare string constant >= null");
    expected = true;
    result = ("200.3" >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #924
    apInitScenario("compare string constant >= boolean constant");
    expected = true;
    result = ("200.3" >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #925
    apInitScenario("compare numeric constant >= numeric variable");
    expected = false;
    result = (-300.2 >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #926
    apInitScenario("compare numeric constant >= string variable");
    expected = false;
    result = (-300.2 >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #927
    apInitScenario("compare numeric constant >= date variable");
    expected = false;
    result = (-300.2 >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }


    // tokenizer output scenario #930
    apInitScenario("compare numeric constant >= fn retval");
    expected = false;
    result = (-300.2 >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #931
    apInitScenario("compare numeric constant >= intrinsic function");
    expected = false;
    result = (-300.2 >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #932
    apInitScenario("compare numeric constant >= array element");
    expected = false;
    result = (-300.2 >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #934
    apInitScenario("compare numeric constant >= string constant");
    expected = false;
    result = (-300.2 >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #935
    apInitScenario("compare numeric constant >= numeric constant");
    expected = true;
    result = (300.2 >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #936
    apInitScenario("compare numeric constant >= boolean variable");
    expected = true;
    result = (300.2 >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #937
    apInitScenario("compare numeric constant >= null");
    expected = true;
    result = (300.2 >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #938
    apInitScenario("compare numeric constant >= boolean constant");
    expected = true;
    result = (300.2 >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #939
    apInitScenario("compare boolean variable >= numeric variable");
    expected = false;
    result = (myBool >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #940
    apInitScenario("compare boolean variable >= string variable");
    expected = false;
    result = (myBool >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #941
    apInitScenario("compare boolean variable >= date variable");
    expected = true;
    result = (myBool >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #944
    apInitScenario("compare boolean variable >= fn retval");
    expected = false;
    result = (myBool >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #945
    apInitScenario("compare boolean variable >= intrinsic function");
    expected = true;
    result = (myBool >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #946
    apInitScenario("compare boolean variable >= array element");
    expected = false;
    result = (myBool >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #947
    apInitScenario("compare boolean variable >= object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myBool >= myObj.myValue);
	@else
    result = (myBool >= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #948
    apInitScenario("compare boolean variable >= string constant");
    expected = false;
    result = (myBool >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #949
    apInitScenario("compare boolean variable >= numeric constant");
    expected = false;
    result = (myBool >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #950
    apInitScenario("compare boolean variable >= boolean variable");
    expected = true;
    result = (myBool >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #951
    apInitScenario("compare boolean variable >= null");
    expected = true;
    result = (myBool >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #952
    apInitScenario("compare boolean variable >= boolean constant");
    expected = true;
    result = (myBool >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #953
    apInitScenario("compare null >= numeric variable");
    expected = false;
    result = (null >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #954
    apInitScenario("compare null >= string variable");
    expected = false;
    result = (null >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #955
    apInitScenario("compare null >= date variable");
    expected = true;
    result = (null >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #958
    apInitScenario("compare null >= fn retval");
    expected = false;
    result = (null >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #959
    apInitScenario("compare null >= intrinsic function");
    expected = false;
    result = (null >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #960
    apInitScenario("compare null >= array element");
    expected = false;
    result = (null >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #961
    apInitScenario("compare null >= object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (null >= myObj.myValue);
	@else
    result = (null >= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #962
    apInitScenario("compare null >= string constant");
    expected = false;
    result = (null >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #963
    apInitScenario("compare null >= numeric constant");
    expected = false;
    result = (null >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #964
    apInitScenario("compare null >= boolean variable");
    expected = false;
    result = (null >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #965
    apInitScenario("compare null >= null");
    expected = true;
    result = (null >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #966
    apInitScenario("compare null >= boolean constant");
    expected = false;
    result = (null >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #967
    apInitScenario("compare boolean constant >= numeric variable");
    expected = false;
    result = (true >= numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #968
    apInitScenario("compare boolean constant >= string variable");
    expected = false;
    result = (true >= strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #969
    apInitScenario("compare boolean constant >= date variable");
    expected = true;
    result = (true >= myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #972
    apInitScenario("compare boolean constant >= fn retval");
    expected = false;
    result = (true >= UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #973
    apInitScenario("compare boolean constant >= intrinsic function");
    expected = true;
    result = (true >= Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #974
    apInitScenario("compare boolean constant >= array element");
    expected = false;
    result = (true >= arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #975
    apInitScenario("compare boolean constant >= object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (true >= myObj.myValue);
	@else
    result = (true >= 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #976
    apInitScenario("compare boolean constant >= string constant");
    expected = false;
    result = (true >= "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #977
    apInitScenario("compare boolean constant >= numeric constant");
    expected = false;
    result = (true >= 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #978
    apInitScenario("compare boolean constant >= boolean variable");
    expected = true;
    result = (true >= myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #979
    apInitScenario("compare boolean constant >= null");
    expected = true;
    result = (true >= null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #980
    apInitScenario("compare boolean constant >= boolean constant");
    expected = true;
    result = (true >= true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}

oper009();


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

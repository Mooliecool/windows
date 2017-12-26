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


var iTestID = 65330;



@if(!@aspx)
	function UDObject() {this.myValue = 88.8;}
	function UDFunction() {return 55.6;}
@else
	expando function UDObject() {this.myValue = 88.8;}
	expando function UDFunction() {return 55.6;}
@end


function oper010() 
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

    apInitTest("oper010");

    // tokenizer output scenario #981
    apInitScenario("compare numeric variable != numeric variable");
    expected = false;
    result = (numVar != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #982
    apInitScenario("compare numeric variable != string variable");
    expected = true;
    result = (numVar != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #983
    apInitScenario("compare numeric variable != date variable");
    expected = true;
    result = (numVar != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #984
    apInitScenario("compare numeric variable != object");
    expected = true;
    result = (numVar != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #985
    apInitScenario("compare numeric variable != udf");
    expected = true;
    result = (numVar != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #986
    apInitScenario("compare numeric variable != fn retval");
    expected = true;
    result = (numVar != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #987
    apInitScenario("compare numeric variable != intrinsic function");
    expected = true;
    result = (numVar != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #988
    apInitScenario("compare numeric variable != array element");
    expected = true;
    result = (numVar != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #989
    apInitScenario("compare numeric variable != object method");
    expected = true;
	@cc_on
	@if (@_jscript_version < 7)
    result = (numVar != myObj.myValue);
	@else
    result = (numVar != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #990
    apInitScenario("compare numeric variable != string constant");
    expected = true;
    result = (numVar != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #991
    apInitScenario("compare numeric variable != numeric constant");
    expected = true;
    result = (numVar != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #992
    apInitScenario("compare numeric variable != boolean variable");
    expected = true;
    result = (numVar != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #993
    apInitScenario("compare numeric variable != null");
    expected = true;
    result = (numVar != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #994
    apInitScenario("compare numeric variable != boolean constant");
    expected = true;
    result = (numVar != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #995
    apInitScenario("compare string variable != numeric variable");
    expected = true;
    result = (strVar != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #996
    apInitScenario("compare string variable != string variable");
    expected = false;
    result = (strVar != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #997
    apInitScenario("compare string variable != date variable");
    expected = true;
    result = (strVar != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #998
    apInitScenario("compare string variable != object");
    expected = true;
    result = (strVar != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #999
    apInitScenario("compare string variable != udf");
    expected = true;
    result = (strVar != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1000
    apInitScenario("compare string variable != fn retval");
    expected = true;
    result = (strVar != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1001
    apInitScenario("compare string variable != intrinsic function");
    expected = true;
    result = (strVar != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1002
    apInitScenario("compare string variable != array element");
    expected = true;
    result = (strVar != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1003
    apInitScenario("compare string variable != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (strVar != myObj.myValue);
	@else
    result = (strVar != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1004
    apInitScenario("compare string variable != string constant");
    expected = true;
    result = (strVar != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1005
    apInitScenario("compare string variable != numeric constant");
    expected = true;
    result = (strVar != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1006
    apInitScenario("compare string variable != boolean variable");
    expected = true;
    result = (strVar != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1007
    apInitScenario("compare string variable != null");
    expected = true;
    result = (strVar != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1008
    apInitScenario("compare string variable != boolean constant");
    expected = true;
    result = (strVar != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1009
    apInitScenario("compare date variable != numeric variable");
    expected = true;
    result = (myDate.getTime() != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1010
    apInitScenario("compare date variable != string variable");
    expected = true;
    result = (myDate.getTime() != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1011
    apInitScenario("compare date variable != date variable");
    expected = false;
    result = (myDate.getTime() != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1012
    apInitScenario("compare date variable != object");
    expected = true;
    result = (myDate.getTime() != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1013
    apInitScenario("compare date variable != udf");
    expected = true;
    result = (myDate.getTime() != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1014
    apInitScenario("compare date variable != fn retval");
    expected = true;
    result = (myDate.getTime() != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1015
    apInitScenario("compare date variable != intrinsic function");
    expected = true;
    result = (myDate.getTime() != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1016
    apInitScenario("compare date variable != array element");
    expected = true;
    result = (myDate.getTime() != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1017
    apInitScenario("compare date variable != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myDate.getTime() != myObj.myValue);
	@else
    result = (myDate.getTime() != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1018
    apInitScenario("compare date variable != string constant");
    expected = true;
    result = (myDate.getTime() != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1019
    apInitScenario("compare date variable != numeric constant");
    expected = true;
    result = (myDate.getTime() != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1020
    apInitScenario("compare date variable != boolean variable");
    expected = true;
    result = (myDate.getTime() != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1021
    apInitScenario("compare date variable != null");
    expected = true;
    result = (myDate.getTime() != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1022
    apInitScenario("compare date variable != boolean constant");
    expected = true;
    result = (myDate.getTime() != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1023
    apInitScenario("compare object != numeric variable");
    expected = true;
    result = (myObj != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1024
    apInitScenario("compare object != string variable");
    expected = true;
    result = (myObj != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1025
    apInitScenario("compare object != date variable");
    expected = true;
    result = (myObj != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1026
    apInitScenario("compare object != object");
    expected = false;
    result = (myObj != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1027
    apInitScenario("compare object != udf");
    expected = true;
    result = (myObj != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1028
    apInitScenario("compare object != fn retval");
    expected = true;
    result = (myObj != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1029
    apInitScenario("compare object != intrinsic function");
    expected = true;
    result = (myObj != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1030
    apInitScenario("compare object != array element");
    expected = true;
    result = (myObj != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1031
    apInitScenario("compare object != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj != myObj.myValue);
	@else
    result = (myObj != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1032
    apInitScenario("compare object != string constant");
    expected = true;
    result = (myObj != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1033
    apInitScenario("compare object != numeric constant");
    expected = true;
    result = (myObj != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1034
    apInitScenario("compare object != boolean variable");
    expected = true;
    result = (myObj != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1035
    apInitScenario("compare object != null");
    expected = true;
    result = (myObj != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1036
    apInitScenario("compare object != boolean constant");
    expected = true;
    result = (myObj != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1037
    apInitScenario("compare udf != numeric variable");
    expected = true;
    result = (myUDF != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1038
    apInitScenario("compare udf != string variable");
    expected = true;
    result = (myUDF != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1039
    apInitScenario("compare udf != date variable");
    expected = true;
    result = (myUDF != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1040
    apInitScenario("compare udf != object");
    expected = true;
    result = (myUDF != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1041
    apInitScenario("compare udf != udf");
    expected = false;
    result = (myUDF != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1042
    apInitScenario("compare udf != fn retval");
    expected = true;
    result = (myUDF != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1043
    apInitScenario("compare udf != intrinsic function");
    expected = true;
    result = (myUDF != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1044
    apInitScenario("compare udf != array element");
    expected = true;
    result = (myUDF != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1045
    apInitScenario("compare udf != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myUDF != myObj.myValue);
	@else
    result = (myUDF != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1046
    apInitScenario("compare udf != string constant");
    expected = true;
    result = (myUDF != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1047
    apInitScenario("compare udf != numeric constant");
    expected = true;
    result = (myUDF != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1048
    apInitScenario("compare udf != boolean variable");
    expected = true;
    result = (myUDF != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1049
    apInitScenario("compare udf != null");
    expected = true;
    result = (myUDF != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1050
    apInitScenario("compare udf != boolean constant");
    expected = true;
    result = (myUDF != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1051
    apInitScenario("compare fn retval != numeric variable");
    expected = true;
    result = (UDFunction() != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1052
    apInitScenario("compare fn retval != string variable");
    expected = true;
    result = (UDFunction() != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1053
    apInitScenario("compare fn retval != date variable");
    expected = true;
    result = (UDFunction() != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1054
    apInitScenario("compare fn retval != object");
    expected = true;
    result = (UDFunction() != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1055
    apInitScenario("compare fn retval != udf");
    expected = true;
    result = (UDFunction() != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1056
    apInitScenario("compare fn retval != fn retval");
    expected = false;
    result = (UDFunction() != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1057
    apInitScenario("compare fn retval != intrinsic function");
    expected = true;
    result = (UDFunction() != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1058
    apInitScenario("compare fn retval != array element");
    expected = true;
    result = (UDFunction() != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1059
    apInitScenario("compare fn retval != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (UDFunction() != myObj.myValue);
	@else
    result = (UDFunction() != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1060
    apInitScenario("compare fn retval != string constant");
    expected = true;
    result = (UDFunction() != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1061
    apInitScenario("compare fn retval != numeric constant");
    expected = true;
    result = (UDFunction() != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1062
    apInitScenario("compare fn retval != boolean variable");
    expected = true;
    result = (UDFunction() != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1063
    apInitScenario("compare fn retval != null");
    expected = true;
    result = (UDFunction() != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1064
    apInitScenario("compare fn retval != boolean constant");
    expected = true;
    result = (UDFunction() != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1065
    apInitScenario("compare intrinsic function != numeric variable");
    expected = true;
    result = (Math.cos(0) != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1066
    apInitScenario("compare intrinsic function != string variable");
    expected = true;
    result = (Math.cos(0) != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1067
    apInitScenario("compare intrinsic function != date variable");
    expected = true;
    result = (Math.cos(0) != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1068
    apInitScenario("compare intrinsic function != object");
    expected = true;
    result = (Math.cos(0) != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1069
    apInitScenario("compare intrinsic function != udf");
    expected = true;
    result = (Math.cos(0) != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1070
    apInitScenario("compare intrinsic function != fn retval");
    expected = true;
    result = (Math.cos(0) != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1071
    apInitScenario("compare intrinsic function != intrinsic function");
    expected = false;
    result = (Math.cos(0) != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1072
    apInitScenario("compare intrinsic function != array element");
    expected = true;
    result = (Math.cos(0) != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1073
    apInitScenario("compare intrinsic function != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (Math.cos(0) != myObj.myValue);
	@else
    result = (Math.cos(0) != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1074
    apInitScenario("compare intrinsic function != string constant");
    expected = true;
    result = (Math.cos(0) != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1075
    apInitScenario("compare intrinsic function != numeric constant");
    expected = true;
    result = (Math.cos(0) != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1076
    apInitScenario("compare intrinsic function != boolean variable");
    expected = true;
    result = (Math.cos(1) != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1077
    apInitScenario("compare intrinsic function != null");
    expected = true;
    result = (Math.cos(0) != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1078
    apInitScenario("compare intrinsic function != boolean constant");
    expected = true;
    result = (Math.cos(2) != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1079
    apInitScenario("compare array element != numeric variable");
    expected = true;
    result = (arrayVar[4] != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1080
    apInitScenario("compare array element != string variable");
    expected = true;
    result = (arrayVar[4] != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1081
    apInitScenario("compare array element != date variable");
    expected = true;
    result = (arrayVar[4] != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}

oper010();


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

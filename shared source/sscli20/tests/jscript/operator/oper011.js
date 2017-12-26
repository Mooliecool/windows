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


var iTestID = 65331;



@if(!@aspx)
	function UDObject() {this.myValue = 88.8;}
	function UDFunction() {return 55.6;}
@else
	expando function UDObject() {this.myValue = 88.8;}
	expando function UDFunction() {return 55.6;}
@end


function oper011() 
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

    apInitTest("oper011");

    // tokenizer output scenario #1082
    apInitScenario("compare array element != object");
    expected = true;
    result = (arrayVar[4] != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1083
    apInitScenario("compare array element != udf");
    expected = true;
    result = (arrayVar[4] != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1084
    apInitScenario("compare array element != fn retval");
    expected = true;
    result = (arrayVar[4] != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1085
    apInitScenario("compare array element != intrinsic function");
    expected = true;
    result = (arrayVar[4] != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1086
    apInitScenario("compare array element != array element");
    expected = false;
    result = (arrayVar[4] != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1087
    apInitScenario("compare array element != object method");
    expected = true;
	@cc_on
	@if (@_jscript_version < 7)
    result = (arrayVar[4] != myObj.myValue);
	@else
    result = (arrayVar[4] != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1088
    apInitScenario("compare array element != string constant");
    expected = true;
    result = (arrayVar[4] != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1089
    apInitScenario("compare array element != numeric constant");
    expected = true;
    result = (arrayVar[4] != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1090
    apInitScenario("compare array element != boolean variable");
    expected = true;
    result = (arrayVar[4] != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1091
    apInitScenario("compare array element != null");
    expected = true;
    result = (arrayVar[4] != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1092
    apInitScenario("compare array element != boolean constant");
    expected = true;
    result = (arrayVar[4] != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1093
    apInitScenario("compare object method != numeric variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != numVar);
	@else
    result = (88.8 != numVar);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1094
    apInitScenario("compare object method != string variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != strVar);
	@else
    result = (88.8 != strVar);
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1095
    apInitScenario("compare object method != date variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != myDate.getTime());
	@else
    result = (88.8 != myDate.getTime());
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1096
    apInitScenario("compare object method != object");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != myObj);
	@else
    result = (88.8 != myObj);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1097
    apInitScenario("compare object method != udf");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != myUDF);
	@else
    result = (88.8 != myUDF);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1098
    apInitScenario("compare object method != fn retval");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != UDFunction());
	@else
    result = (88.8 != UDFunction());
	@end
	
    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1099
    apInitScenario("compare object method != intrinsic function");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != Math.cos(0));
	@else
    result = (88.8 != Math.cos(0));
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1100
    apInitScenario("compare object method != array element");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != arrayVar[4]);
	@else
    result = (88.8 != arrayVar[4]);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1101
    apInitScenario("compare object method != object method");
    expected = false;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != myObj.myValue);
	@else
    result = (88.8 != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1102
    apInitScenario("compare object method != string constant");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != "200.3");
	@else
    result = (88.8 != "200.3");
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1103
    apInitScenario("compare object method != numeric constant");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != 300.2);
	@else
    result = (88.8 != 300.2);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1104
    apInitScenario("compare object method != boolean variable");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != myBool);
	@else
    result = (88.8 != myBool);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1105
    apInitScenario("compare object method != null");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != null);
	@else
    result = (88.8 != null);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1106
    apInitScenario("compare object method != boolean constant");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myObj.myValue != true);
	@else
    result = (88.8 != true);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1107
    apInitScenario("compare string constant != numeric variable");
    expected = true;
    result = ("200.3" != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1108
    apInitScenario("compare string constant != string variable");
    expected = true;
    result = ("200.3" != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1109
    apInitScenario("compare string constant != date variable");
    expected = true;
    result = ("200.3" != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1110
    apInitScenario("compare string constant != object");
    expected = true;
    result = ("200.3" != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1111
    apInitScenario("compare string constant != udf");
    expected = true;
    result = ("200.3" != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1112
    apInitScenario("compare string constant != fn retval");
    expected = true;
    result = ("200.3" != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1113
    apInitScenario("compare string constant != intrinsic function");
    expected = true;
    result = ("200.3" != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1114
    apInitScenario("compare string constant != array element");
    expected = true;
    result = ("200.3" != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1115
    apInitScenario("compare string constant != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = ("200.3" != myObj.myValue);
	@else
    result = ("200.3" != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1116
    apInitScenario("compare string constant != string constant");
    expected = false;
    result = ("200.3" != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1117
    apInitScenario("compare string constant != numeric constant");
    expected = true;
    result = ("200.3" != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1118
    apInitScenario("compare string constant != boolean variable");
    expected = true;
    result = ("200.3" != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1119
    apInitScenario("compare string constant != null");
    expected = true;
    result = ("200.3" != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1120
    apInitScenario("compare string constant != boolean constant");
    expected = true;
    result = ("200.3" != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1121
    apInitScenario("compare numeric constant != numeric variable");
    expected = true;
    result = (300.2 != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1122
    apInitScenario("compare numeric constant != string variable");
    expected = true;
    result = (300.2 != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1123
    apInitScenario("compare numeric constant != date variable");
    expected = true;
    result = (300.2 != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1124
    apInitScenario("compare numeric constant != object");
    expected = true;
    result = (300.2 != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1125
    apInitScenario("compare numeric constant != udf");
    expected = true;
    result = (300.2 != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1126
    apInitScenario("compare numeric constant != fn retval");
    expected = true;
    result = (300.2 != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1127
    apInitScenario("compare numeric constant != intrinsic function");
    expected = true;
    result = (300.2 != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1128
    apInitScenario("compare numeric constant != array element");
    expected = true;
    result = (300.2 != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1129
    apInitScenario("compare numeric constant != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (300.2 != myObj.myValue);
	@else
    result = (300.2 != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1130
    apInitScenario("compare numeric constant != string constant");
    expected = true;
    result = (300.2 != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1131
    apInitScenario("compare numeric constant != numeric constant");
    expected = false;
    result = (300.2 != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1132
    apInitScenario("compare numeric constant != boolean variable");
    expected = true;
    result = (300.2 != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1133
    apInitScenario("compare numeric constant != null");
    expected = true;
    result = (300.2 != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1134
    apInitScenario("compare numeric constant != boolean constant");
    expected = true;
    result = (300.2 != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1135
    apInitScenario("compare boolean variable != numeric variable");
    expected = true;
    result = (myBool != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1136
    apInitScenario("compare boolean variable != string variable");
    expected = true;
    result = (myBool != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1137
    apInitScenario("compare boolean variable != date variable");
    expected = true;
    result = (myBool != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1138
    apInitScenario("compare boolean variable != object");
    expected = true;
    result = (myBool != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1139
    apInitScenario("compare boolean variable != udf");
    expected = true;
    result = (myBool != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1140
    apInitScenario("compare boolean variable != fn retval");
    expected = true;
    result = (myBool != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1141
    apInitScenario("compare boolean variable != intrinsic function");
    expected = false;
    result = (myBool != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1142
    apInitScenario("compare boolean variable != array element");
    expected = true;
    result = (myBool != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1143
    apInitScenario("compare boolean variable != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (myBool != myObj.myValue);
	@else
    result = (myBool != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1144
    apInitScenario("compare boolean variable != string constant");
    expected = true;
    result = (myBool != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1145
    apInitScenario("compare boolean variable != numeric constant");
    expected = true;
    result = (myBool != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1146
    apInitScenario("compare boolean variable != boolean variable");
    expected = false;
    result = (myBool != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1147
    apInitScenario("compare boolean variable != null");
    expected = true;
    result = (myBool != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1148
    apInitScenario("compare boolean variable != boolean constant");
    expected = true;
    result = (myBool != false);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1149
    apInitScenario("compare null != numeric variable");
    expected = true;
    result = (null != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1150
    apInitScenario("compare null != string variable");
    expected = true;
    result = (null != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1151
    apInitScenario("compare null != date variable");
    expected = true;
    result = (null != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1152
    apInitScenario("compare null != object");
    expected = true;
    result = (null != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1153
    apInitScenario("compare null != udf");
    expected = true;
    result = (null != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1154
    apInitScenario("compare null != fn retval");
    expected = true;
    result = (null != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1155
    apInitScenario("compare null != intrinsic function");
    expected = true;
    result = (null != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1156
    apInitScenario("compare null != array element");
    expected = true;
    result = (null != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1157
    apInitScenario("compare null != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (null != myObj.myValue);
	@else
    result = (null != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1158
    apInitScenario("compare null != string constant");
    expected = true;
    result = (null != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1159
    apInitScenario("compare null != numeric constant");
    expected = true;
    result = (null != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1160
    apInitScenario("compare null != boolean variable");
    expected = true;
    result = (null != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1161
    apInitScenario("compare null != null");
    expected = false;
    result = (null != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1162
    apInitScenario("compare null != boolean constant");
    expected = true;
    result = (null != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1163
    apInitScenario("compare boolean constant != numeric variable");
    expected = true;
    result = (true != numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1164
    apInitScenario("compare boolean constant != string variable");
    expected = true;
    result = (true != strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1165
    apInitScenario("compare boolean constant != date variable");
    expected = true;
    result = (true != myDate.getTime());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1166
    apInitScenario("compare boolean constant != object");
    expected = true;
    result = (true != myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1167
    apInitScenario("compare boolean constant != udf");
    expected = true;
    result = (true != myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1168
    apInitScenario("compare boolean constant != fn retval");
    expected = true;
    result = (true != UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1169
    apInitScenario("compare boolean constant != intrinsic function");
    expected = false;
    result = (true != Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1170
    apInitScenario("compare boolean constant != array element");
    expected = true;
    result = (true != arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1171
    apInitScenario("compare boolean constant != object method");
    expected = true;
	@if (@_jscript_version < 7)
    result = (true != myObj.myValue);
	@else
    result = (true != 88.8);
	@end

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1172
    apInitScenario("compare boolean constant != string constant");
    expected = true;
    result = (true != "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1173
    apInitScenario("compare boolean constant != numeric constant");
    expected = true;
    result = (true != 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1174
    apInitScenario("compare boolean constant != boolean variable");
    expected = false;
    result = (true != myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1175
    apInitScenario("compare boolean constant != null");
    expected = true;
    result = (true != null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #1176
    apInitScenario("compare boolean constant != boolean constant");
    expected = false;
    result = (true != true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    apEndTest();

}


oper011();


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

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


var iTestID = 179262;

//////////////////////////////////////////////////////////////////////////////

@if(!@aspx)
	import System
@end
var func
var type

function diff(actual, expected, delta){
    if (!isFinite(actual) || !isFinite(expected)){
        if (isNaN(actual) && isNaN(expected)) return false
        if (actual == expected) return false
        else return true
    }

    if ((actual - expected) <= delta && (expected - actual) <= delta)
        return false
    return true
}


function retvar(val){return val}
function rettyped1(val){return type(val)}
function rettyped2(val){
	return eval("   var ret:type = val; ret")
}

function verifyParams(errstr, input, expected, ... params:Object[]){
    var delta = .000001
    if (params.length>0) delta = params[0]


    if (typeof(func(input)) !== typeof(expected))
        apLogFailInfo(errstr + ": type failure", typeof(expected), typeof(func(input)), "");


    if (diff( func(type(input)), expected, delta))
        apLogFailInfo(errstr + ": immediate failure", expected, func(type(input)), "");


    //eval("	var insttype:type=input; if (diff( func(insttype), expected, delta)) apLogFailInfo(errstr + ': typed instance failure', expected, func(insttype), ''); ")


    var instob:Object = type(input)
    if (diff( func(instob), expected, delta))
        apLogFailInfo(errstr + ": object instance failure", expected, func(instob), "");


    var memob = new Object
    memob.mem = type(input)
    if (diff( func(memob.mem), expected, delta))
        apLogFailInfo(errstr + ": object member failure", expected, func(memob.mem), "");


    if (diff( func(  retvar(type(input))  ), expected, delta))
        apLogFailInfo(errstr + ": function return failure", expected, func(retvar(type(input))), "");


    if (diff( func(  rettyped1(input)  ), expected, delta))
        apLogFailInfo(errstr + ": function typed return failure 1", expected, func(  rettyped1(input)  ), "");


    //if (diff( func(  rettyped2(input)  ), expected, delta))
    //    apLogFailInfo(errstr + ": function typed return failure 2", expected, func(  rettyped2(input)  ), "");

}




function cosh001() {


    var i
    func = Math.cos


    apInitTest("cosh001 ");

    type = boolean
    apInitScenario("cos of a boolean");

    verifyParams("boolean: n = true", true, 0.54030230586813971740093660744298)
    verifyParams("boolean: n = false", false, 1)



    type = char;
    apInitScenario("cos of a char");

    verifyParams("char: n = 0", char.MinValue, 1)
    verifyParams("char: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("char: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("char: n = 100", 100, 0.86231887228768393410193851395084)
    verifyParams("char: n = char.max", char.MaxValue, 0.19234401860586396311221131652002)


    type = Single;
    apInitScenario("cos of a Single");


    verifyParams("single: n = NaN", NaN, NaN)
    verifyParams("single: n = -Infinity", -Infinity, NaN)
//    verifyParams("single: n = single.min", Single.MinValue, -0.67814304002235789763859745548629)
    verifyParams("single: n = -3Math.PI/2", -3*Math.PI/2, 0)
    verifyParams("single: n = -Math.PI", -Math.PI, -1)
    verifyParams("single: n = -Math.PI/2", -Math.PI/2, 0)
    verifyParams("single: n = -Math.PI/3", -Math.PI/3, .5)
    verifyParams("single: n = -Math.PI/6", -Math.PI/6, Math.sqrt(3)/2)
    verifyParams("single: n = -0", -0, 1)
    verifyParams("single: n = +0", +0, 1)
    verifyParams("single: n = Math.PI/6", Math.PI/6, Math.sqrt(3)/2)
    verifyParams("single: n = Math.PI/3", Math.PI/3, .5)
    verifyParams("single: n = Math.PI/2", Math.PI/2, 0)
    verifyParams("single: n = Math.PI", Math.PI, -1)
    verifyParams("single: n = 3Math.PI/2", 3*Math.PI/2, 0)
//    verifyParams("single: n = single.Max", Single.MaxValue, 0.67814304002235789763859745548629)
    verifyParams("single: n = Infinity", Infinity, NaN)


    type = double;
    apInitScenario("cos of a double");

    verifyParams("double: n = NaN", NaN, NaN)
    verifyParams("double: n = -Infinity", -Infinity, NaN)
//    verifyParams("double: n = double.min", double.MinValue, -0.67814304002235789763859745548629)
    verifyParams("double: n = -3Math.PI/2", -3*Math.PI/2, 0)
    verifyParams("double: n = -Math.PI", -Math.PI, -1)
    verifyParams("double: n = -Math.PI/2", -Math.PI/2, 0)
    verifyParams("double: n = -Math.PI/3", -Math.PI/3, .5)
    verifyParams("double: n = -Math.PI/6", -Math.PI/6, Math.sqrt(3)/2)
    verifyParams("double: n = -0", -0, 1)
    verifyParams("double: n = +0", +0, 1)
    verifyParams("double: n = Math.PI/6", Math.PI/6, Math.sqrt(3)/2)
    verifyParams("double: n = Math.PI/3", Math.PI/3, .5)
    verifyParams("double: n = Math.PI/2", Math.PI/2, 0)
    verifyParams("double: n = Math.PI", Math.PI, -1)
    verifyParams("double: n = 3Math.PI/2", 3*Math.PI/2, 0)
//    verifyParams("double: n = double.Max", double.MaxValue, 0.67814304002235789763859745548629)
    verifyParams("double: n = Infinity", Infinity, NaN)


    type = decimal;
    apInitScenario("cos of a decimal");

//    verifyParams("decimal: n = decimal.min", decimal.MinValue, -0.67814304002235789763859745548629)
    verifyParams("decimal: n = -3Math.PI/2", -3*Math.PI/2, 0)
    verifyParams("decimal: n = -Math.PI", -Math.PI, -1)
    verifyParams("decimal: n = -Math.PI/2", -Math.PI/2, 0)
    verifyParams("decimal: n = -Math.PI/3", -Math.PI/3, .5)
    verifyParams("decimal: n = -Math.PI/6", -Math.PI/6, Math.sqrt(3)/2)
    verifyParams("decimal: n = +0", +0, 1)
    verifyParams("decimal: n = Math.PI/6", Math.PI/6, Math.sqrt(3)/2)
    verifyParams("decimal: n = Math.PI/3", Math.PI/3, .5)
    verifyParams("decimal: n = Math.PI/2", Math.PI/2, 0)
    verifyParams("decimal: n = Math.PI", Math.PI, -1)
    verifyParams("decimal: n = 3Math.PI/2", 3*Math.PI/2, 0)
//    verifyParams("decimal: n = decimal.Max", decimal.MaxValue, 0.67814304002235789763859745548629)


    type = byte;
    apInitScenario("cos of a byte");

    verifyParams("byte: n = 0", byte.MinValue, 1)
    verifyParams("byte: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("byte: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("byte: n = 100", 100, 0.86231887228768393410193851395084)
    verifyParams("byte: n = byte.max", byte.MaxValue, -0.86230360783108238176471326502344)


    type = ushort;
    apInitScenario("cos of a ushort");

    verifyParams("ushort: n = 0", ushort.MinValue, 1)
    verifyParams("ushort: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("ushort: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("ushort: n = 100", 100, 0.86231887228768393410193851395084)
    verifyParams("ushort: n = ushort.max", ushort.MaxValue, 0.19234401860586396311221131652002)


    type = uint;
    apInitScenario("cos of a uint");

    verifyParams("uint: n = 0", uint.MinValue, 1)
    verifyParams("uint: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("uint: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("uint: n = 100", 100, 0.86231887228768393410193851395084)
    verifyParams("uint: n = uint.max", uint.MaxValue, -0.86793534735720228729619987845736)


    type = ulong;
    apInitScenario("cos of a ulong");

    verifyParams("ulong: n = 0", ulong.MinValue, 1)
    verifyParams("ulong: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("ulong: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("ulong: n = 100", 100, 0.86231887228768393410193851395084)
    verifyParams("ulong: n = 10*uint.max", 10*uint.MaxValue, 0.46644950484229241221597487911534)
    //verifyParams("ulong: n = ulong.max", ulong.MaxValue, -0.52029437916145757557525481470159)


    type = sbyte;
    apInitScenario("cos of a sbyte");

    verifyParams("sbyte: n = sbyte.min", sbyte.MinValue, -0.69289582192016509895436027724857)
    verifyParams("sbyte: n = -100", -100, 0.86231887228768393410193851395084)
    verifyParams("sbyte: n = -10", -10, -0.83907152907645245225886394782406)
    verifyParams("sbyte: n = -1", -1, 0.54030230586813971740093660744298)
    verifyParams("sbyte: n = 0", 0, 1)
    verifyParams("sbyte: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("sbyte: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("sbyte: n = 100", 100, 0.86231887228768393410193851395084)
    verifyParams("sbyte: n = sbyte.max", sbyte.MaxValue, 0.23235910202965792756573985157716)


    type = short;
    apInitScenario("cos of a short");

    verifyParams("short: n = short.min", short.MinValue, 0.37293782932771495587435101092088)
    verifyParams("short: n = -100", -100, 0.86231887228768393410193851395084)
    verifyParams("short: n = -10", -10, -0.83907152907645245225886394782406)
    verifyParams("short: n = -1", -1, 0.54030230586813971740093660744298)
    verifyParams("short: n = 0", 0, 1)
    verifyParams("short: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("short: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("short: n = 100", 100, 0.86231887228768393410193851395084)
    verifyParams("short: n = short.max", short.MaxValue, 0.98226335176928229845653841823194)


    type = int;
    apInitScenario("cos of a int");

    verifyParams("int: n = int.min", int.MinValue, 0.23781619457280335715368731740099)
    verifyParams("int: n = -100", -100, 0.86231887228768393410193851395084)
    verifyParams("int: n = -10", -10, -0.83907152907645245225886394782406)
    verifyParams("int: n = -1", -1, 0.54030230586813971740093660744298)
    verifyParams("int: n = 0", 0, 1)
    verifyParams("int: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("int: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("int: n = 100", 100, 0.86231887228768393410193851395084)
    verifyParams("int: n = int.max", int.MaxValue, -0.68883669187794383467975822303793)


    type = long;
    apInitScenario("cos of a long");

    //verifyParams("long: n = long.min", long.MinValue, 0.011800076512800236684420385060898)   fails by design
    verifyParams("long: n = -100", -100, 0.86231887228768393410193851395084)
    verifyParams("long: n = -10", -10, -0.83907152907645245225886394782406)
    verifyParams("long: n = -1", -1, 0.54030230586813971740093660744298)
    verifyParams("long: n = 0", 0, 1)
    verifyParams("long: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("long: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("long: n = 100", 100, 0.86231887228768393410193851395084)
    //verifyParams("long: n = long.max", long.MaxValue, 0.84778800734801878280815614874824)


    type = System.String;
    apInitScenario("cos of a String");

    verifyParams("string: n = -100", -100, 0.86231887228768393410193851395084)
    verifyParams("string: n = -10", -10, -0.83907152907645245225886394782406)
    verifyParams("string: n = -1", -1, 0.54030230586813971740093660744298)
    verifyParams("string: n = 0", 0, 1)
    verifyParams("string: n = 1", 1, 0.54030230586813971740093660744298)
    verifyParams("string: n = 10", 10, -0.83907152907645245225886394782406)
    verifyParams("string: n = 100", 100, 0.86231887228768393410193851395084)


    type = Date;
    apInitScenario("cos of a Date");

    verifyParams("date: n = 0", 0, NaN)


    type = Array;
    apInitScenario("cos of a Array");
    var arrvar:Array = new Array(1)
    arrvar[0] = -10
    verifyParams("Array: n = -10", arrvar, -0.83907152907645245225886394782406)
    arrvar[0] = -1
    verifyParams("Array: n = -1", arrvar, 0.54030230586813971740093660744298)
    arrvar[0] = -0
    verifyParams("Array: n = -0", arrvar, 1)
    arrvar[0] = 1
    verifyParams("Array: n = 1", arrvar, 0.54030230586813971740093660744298)


    apEndTest();

}

cosh001();


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

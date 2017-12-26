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


var iTestID = 181289;

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



function sinh002() {


    var i
    func = System.Math.Sin


    apInitTest("sinh002 ");

    type = boolean
    apInitScenario("sin of a boolean");

    verifyParams("boolean: n = true", true, 0.8414709848078965066525023216303)
    verifyParams("boolean: n = false", false, 0)



    type = char;
    apInitScenario("sin of a char");

    verifyParams("char: n = 0", char.MinValue, 0)
    verifyParams("char: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("char: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("char: n = 100", 100, -0.50636564110975879365655761045979)
    verifyParams("char: n = char.max", char.MaxValue, 0.98132755923114024077188380904771)


    type = Single;
    apInitScenario("sin of a Single");


    verifyParams("single: n = NaN", NaN, NaN)
    verifyParams("single: n = -Infinity", -Infinity, NaN)
//    verifyParams("single: n = single.min", Single.MinValue, -0.67814304002235789763859745548629)
    verifyParams("single: n = -3Math.PI/2", -3*Math.PI/2, 1)
    verifyParams("single: n = -Math.PI", -Math.PI, 0)
    verifyParams("single: n = -Math.PI/2", -Math.PI/2, -1)
    verifyParams("single: n = -Math.PI/3", -Math.PI/3, -Math.sqrt(3)/2)
    verifyParams("single: n = -Math.PI/6", -Math.PI/6, -.5)
    verifyParams("single: n = -0", -0, 0)
    verifyParams("single: n = +0", +0, 0)
    verifyParams("single: n = Math.PI/6", Math.PI/6, .5)
    verifyParams("single: n = Math.PI/3", Math.PI/3, Math.sqrt(3)/2)
    verifyParams("single: n = Math.PI/2", Math.PI/2, 1)
    verifyParams("single: n = Math.PI", Math.PI, 0)
    verifyParams("single: n = 3Math.PI/2", 3*Math.PI/2, -1)
//    verifyParams("single: n = single.Max", Single.MaxValue, 0.67814304002235789763859745548629)
    verifyParams("single: n = Infinity", Infinity, NaN)


    type = double;
    apInitScenario("sin of a double");

    verifyParams("double: n = NaN", NaN, NaN)
    verifyParams("double: n = -Infinity", -Infinity, NaN)
//    verifyParams("double: n = double.min", double.MinValue, -0.67814304002235789763859745548629)
    verifyParams("double: n = -3Math.PI/2", -3*Math.PI/2, 1)
    verifyParams("double: n = -Math.PI", -Math.PI, 0)
    verifyParams("double: n = -Math.PI/2", -Math.PI/2, -1)
    verifyParams("double: n = -Math.PI/3", -Math.PI/3, -Math.sqrt(3)/2)
    verifyParams("double: n = -Math.PI/6", -Math.PI/6, -.5)
    verifyParams("double: n = -0", -0, 0)
    verifyParams("double: n = +0", +0, 0)
    verifyParams("double: n = Math.PI/6", Math.PI/6, .5)
    verifyParams("double: n = Math.PI/3", Math.PI/3, Math.sqrt(3)/2)
    verifyParams("double: n = Math.PI/2", Math.PI/2, 1)
    verifyParams("double: n = Math.PI", Math.PI, 0)
    verifyParams("double: n = 3Math.PI/2", 3*Math.PI/2, -1)
//    verifyParams("double: n = double.Max", double.MaxValue, 0.67814304002235789763859745548629)
    verifyParams("double: n = Infinity", Infinity, NaN)


    type = decimal;
    apInitScenario("sin of a decimal");

//    verifyParams("decimal: n = decimal.min", decimal.MinValue, -0.67814304002235789763859745548629)
    verifyParams("decimal: n = -3Math.PI/2", -3*Math.PI/2, 1)
    verifyParams("decimal: n = -Math.PI", -Math.PI, 0)
    verifyParams("decimal: n = -Math.PI/2", -Math.PI/2, -1)
    verifyParams("decimal: n = -Math.PI/3", -Math.PI/3, -Math.sqrt(3)/2)
    verifyParams("decimal: n = -Math.PI/6", -Math.PI/6, -.5)
    verifyParams("decimal: n = +0", +0, 0)
    verifyParams("decimal: n = Math.PI/6", Math.PI/6, .5)
    verifyParams("decimal: n = Math.PI/3", Math.PI/3, Math.sqrt(3)/2)
    verifyParams("decimal: n = Math.PI/2", Math.PI/2, 1)
    verifyParams("decimal: n = Math.PI", Math.PI, 0)
    verifyParams("decimal: n = 3Math.PI/2", 3*Math.PI/2, -1)
//    verifyParams("decimal: n = decimal.Max", decimal.MaxValue, 0.67814304002235789763859745548629)


    type = byte;
    apInitScenario("sin of a byte");

    verifyParams("byte: n = 0", byte.MinValue, 0)
    verifyParams("byte: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("byte: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("byte: n = 100", 100, -0.50636564110975879365655761045979)
    verifyParams("byte: n = byte.max", byte.MaxValue, -0.50639163492449090740200902425491)


    type = ushort;
    apInitScenario("sin of a ushort");

    verifyParams("ushort: n = 0", ushort.MinValue, 0)
    verifyParams("ushort: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("ushort: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("ushort: n = 100", 100, -0.50636564110975879365655761045979)
    verifyParams("ushort: n = ushort.max", ushort.MaxValue, 0.98132755923114024077188380904771)


    type = uint;
    apInitScenario("sin of a uint");

    verifyParams("uint: n = 0", uint.MinValue, 0)
    verifyParams("uint: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("uint: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("uint: n = 100", 100, -0.50636564110975879365655761045979)
    verifyParams("uint: n = uint.max", uint.MaxValue, 0.49667719175328819252918094880462)


    type = ulong;
    apInitScenario("sin of a ulong");

    verifyParams("ulong: n = 0", ulong.MinValue, 0)
    verifyParams("ulong: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("ulong: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("ulong: n = 100", 100, -0.50636564110975879365655761045979)
    verifyParams("ulong: n = 10*uint.max", 10*uint.MaxValue, 0.88454782766811440939866589135696)
    //verifyParams("ulong: n = ulong.max", ulong.MaxValue, 0.85398697824556635386780047246434)


    type = sbyte;
    apInitScenario("sin of a sbyte");

    verifyParams("sbyte: n = sbyte.min", sbyte.MinValue, -0.72103771050173156304855831856512)
    verifyParams("sbyte: n = -100", -100, 0.50636564110975879365655761045979)
    verifyParams("sbyte: n = -10", -10, 0.54402111088936981340474766185138)
    verifyParams("sbyte: n = -1", -1, -0.8414709848078965066525023216303)
    verifyParams("sbyte: n = 0", 0, 0)
    verifyParams("sbyte: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("sbyte: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("sbyte: n = 100", 100, -0.50636564110975879365655761045979)
    verifyParams("sbyte: n = sbyte.max", sbyte.MaxValue, 0.9726300672424079534997791075157)


    type = short;
    apInitScenario("sin of a short");

    verifyParams("short: n = short.min", short.MinValue, -0.92785633341392467457138471990344)
    verifyParams("short: n = -100", -100, 0.50636564110975879365655761045979)
    verifyParams("short: n = -10", -10, 0.54402111088936981340474766185138)
    verifyParams("short: n = -1", -1, -0.8414709848078965066525023216303)
    verifyParams("short: n = 0", 0, 0)
    verifyParams("short: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("short: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("short: n = 100", 100, -0.50636564110975879365655761045979)
    verifyParams("short: n = short.max", short.MaxValue, 0.18750655394138942394239306576486)


    type = int;
    apInitScenario("sin of a int");

    verifyParams("int: n = int.min", int.MinValue, 0.97131017579293924282514516510681)
    verifyParams("int: n = -100", -100, 0.50636564110975879365655761045979)
    verifyParams("int: n = -10", -10, 0.54402111088936981340474766185138)
    verifyParams("int: n = -1", -1, -0.8414709848078965066525023216303)
    verifyParams("int: n = 0", 0, 0)
    verifyParams("int: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("int: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("int: n = 100", 100, -0.50636564110975879365655761045979)
    verifyParams("int: n = int.max", int.MaxValue, -0.7249165551445563905482932963398)


    type = long;
    apInitScenario("sin of a long");

    //verifyParams("long: n = long.min", long.MinValue, -0.99993037667344222961785650539353)   fails by design
    verifyParams("long: n = -100", -100, 0.50636564110975879365655761045979)
    verifyParams("long: n = -10", -10, 0.54402111088936981340474766185138)
    verifyParams("long: n = -1", -1, -0.8414709848078965066525023216303)
    verifyParams("long: n = 0", 0, 0)
    verifyParams("long: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("long: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("long: n = 100", 100, -0.50636564110975879365655761045979)
    //verifyParams("long: n = long.max", long.MaxValue, 0.53033526622022379922156569886211)


    type = System.String;
    apInitScenario("sin of a String");

    verifyParams("string: n = -100", -100, 0.50636564110975879365655761045979)
    verifyParams("string: n = -10", -10, 0.54402111088936981340474766185138)
    verifyParams("string: n = -1", -1, -0.8414709848078965066525023216303)
    verifyParams("string: n = 0", 0, 0)
    verifyParams("string: n = 1", 1, 0.8414709848078965066525023216303)
    verifyParams("string: n = 10", 10, -0.54402111088936981340474766185138)
    verifyParams("string: n = 100", 100, -0.50636564110975879365655761045979)


    type = Date;
    apInitScenario("sin of a Date");

    verifyParams("date: n = 0", 0, NaN)


    type = Array;
    apInitScenario("sin of a Array");
    var arrvar:Array = new Array(1)
    arrvar[0] = -10
    verifyParams("Array: n = -10", arrvar, 0.54402111088936981340474766185138)
    arrvar[0] = -1
    verifyParams("Array: n = -1", arrvar, -0.8414709848078965066525023216303)
    arrvar[0] = -0
    verifyParams("Array: n = -0", arrvar, 0)
    arrvar[0] = 1
    verifyParams("Array: n = 1", arrvar, 0.8414709848078965066525023216303)


    apEndTest();

}


sinh002();


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

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


var iTestID = 178210;

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


function tstRnd(errstr, min, max, ... info:Object[]){
    var i:int;
	var vNum:Number, rounds = 20

    if (info.length>0) rounds = info[0];
    for (i=0; i<rounds; i++)
    {
        vNum = Math.floor(Math.random()*(max+1-min))+min; // gives int num between min and max
        verifyParams(errstr,vNum,NaN);
    }
}



function asinh001() {


    var i
    func = Math.asin


    apInitTest("asinh001 ");

    type = boolean
    apInitScenario("asin of a boolean");

    verifyParams("boolean: n = true", true, Math.PI/2)
    verifyParams("boolean: n = false", false, 0)



    type = char;
    apInitScenario("asin of a char");

    verifyParams("char: n = 0", char.MinValue, 0)
    verifyParams("char: n = 1", 1, Math.PI/2)
    verifyParams("char: n = 2", 2, NaN)
    tstRnd("char: 2 < n < char.max", 3, char.MaxValue-1)
    verifyParams("char: n = char.max", char.MaxValue, NaN)


    type = Single;
    apInitScenario("asin of a Single");

    verifyParams("single: n = single.min", Single.MinValue, NaN)
    verifyParams("single: n = -2", -2, NaN)
    verifyParams("single: n = -1", -1, -Math.PI/2)
    verifyParams("single: n = -SQRT(3)/2", -Math.sqrt(3)/2, -Math.PI/3)
    verifyParams("single: n = -SQRT(2)/2", -Math.sqrt(2)/2, -Math.PI/4)
    verifyParams("single: n = -.5", -.5, -Math.PI/6)
    verifyParams("single: n = -0", -0, 0)
    verifyParams("single: n = +0", +0, 0)
    verifyParams("single: n = .5", .5, Math.PI/6)
    verifyParams("single: n = SQRT(2)/2", Math.sqrt(2)/2, Math.PI/4)
    verifyParams("single: n = SQRT(3)/2", Math.sqrt(3)/2, Math.PI/3)
    verifyParams("single: n = 1", 1, Math.PI/2)
    verifyParams("single: n = 2", 2, NaN)
    verifyParams("single: n = single.Max", Single.MaxValue, NaN)



    type = double;
    apInitScenario("asin of a double");

    verifyParams("double: n = double.min", double.MinValue, NaN)
    verifyParams("double: n = -2", -2, NaN)
    verifyParams("double: n = -1", -1, -Math.PI/2)
    verifyParams("double: n = -SQRT(3)/2", -Math.sqrt(3)/2, -Math.PI/3)
    verifyParams("double: n = -SQRT(2)/2", -Math.sqrt(2)/2, -Math.PI/4)
    verifyParams("double: n = -.5", -.5, -Math.PI/6)
    verifyParams("double: n = -0", -0, 0)
    verifyParams("double: n = +0", +0, 0)
    verifyParams("double: n = .5", .5, Math.PI/6)
    verifyParams("double: n = SQRT(2)/2", Math.sqrt(2)/2, Math.PI/4)
    verifyParams("double: n = SQRT(3)/2", Math.sqrt(3)/2, Math.PI/3)
    verifyParams("double: n = 1", 1, Math.PI/2)
    verifyParams("double: n = 2", 2, NaN)
    verifyParams("double: n = double.Max", double.MaxValue, NaN)


    type = decimal;
    apInitScenario("asin of a decimal");

    verifyParams("decimal: n = decimal.min", decimal.MinValue, NaN)
    verifyParams("decimal: n = -2", -2, NaN)
    verifyParams("decimal: n = -1", -1, -Math.PI/2)
    verifyParams("decimal: n = -SQRT(3)/2", -Math.sqrt(3)/2, -Math.PI/3)
    verifyParams("decimal: n = -SQRT(2)/2", -Math.sqrt(2)/2, -Math.PI/4)
    verifyParams("decimal: n = -.5", -.5, -Math.PI/6)
    verifyParams("decimal: n = -0", -0, 0)
    verifyParams("decimal: n = +0", +0, 0)
    verifyParams("decimal: n = .5", .5, Math.PI/6)
    verifyParams("decimal: n = SQRT(2)/2", Math.sqrt(2)/2, Math.PI/4)
    verifyParams("decimal: n = SQRT(3)/2", Math.sqrt(3)/2, Math.PI/3)
    verifyParams("decimal: n = 1", 1, Math.PI/2)
    verifyParams("decimal: n = 2", 2, NaN)
    verifyParams("decimal: n = decimal.Max", decimal.MaxValue, NaN)


    type = byte;
    apInitScenario("asin of a byte");

    verifyParams("byte: n = 0", byte.MinValue, 0)
    verifyParams("byte: n = 1", 1, Math.PI/2)
    verifyParams("byte: n = 2", 2, NaN)
    tstRnd("byte: 2 < n < byte.max", 3, byte.MaxValue-1)
    verifyParams("byte: n = byte.max", byte.MaxValue, NaN)


    type = ushort;
    apInitScenario("asin of a ushort");

    verifyParams("ushort: n = 0", ushort.MinValue, 0)
    verifyParams("ushort: n = 1", 1, Math.PI/2)
    verifyParams("ushort: n = 2", 2, NaN)
    tstRnd("ushort: 2 < n < ushort.max", 3, ushort.MaxValue-1)
    verifyParams("ushort: n = ushort.max", ushort.MaxValue, NaN)


    type = uint;
    apInitScenario("asin of a uint");

    verifyParams("uint: n = 0", uint.MinValue, 0)
    verifyParams("uint: n = 1", 1, Math.PI/2)
    verifyParams("uint: n = 2", 2, NaN)
    tstRnd("uint: 2 < n < uint.max", 3, uint.MaxValue-1)
    verifyParams("uint: n = uint.max", uint.MaxValue, NaN)


    type = ulong;
    apInitScenario("asin of a ulong");

    verifyParams("ulong: n = 0", ulong.MinValue, 0)
    verifyParams("ulong: n = 1", 1, Math.PI/2)
    verifyParams("ulong: n = 2", 2, NaN)
    tstRnd("ulong: 2 < n < ulong.max", 3, ulong.MaxValue-1)
    verifyParams("ulong: n = ulong.max", ulong.MaxValue, NaN)


    type = sbyte;
    apInitScenario("asin of a sbyte");

    verifyParams("sbyte: n = sbyte.min", sbyte.MinValue, NaN)
    tstRnd("sbyte: -2 > n > sbyte.max", sbyte.MinValue+1, -3)
    verifyParams("sbyte: n = -2", -2, NaN)
    verifyParams("sbyte: n = -1", -1, -Math.PI/2)
    verifyParams("sbyte: n = -0", -0, 0)
    verifyParams("sbyte: n = 0", 0, 0)
    verifyParams("sbyte: n = 1", 1, Math.PI/2)
    verifyParams("sbyte: n = 2", 2, NaN)
    tstRnd("sbyte: 2 < n < sbyte.max", 3, sbyte.MaxValue-1)
    verifyParams("sbyte: n = sbyte.max", sbyte.MaxValue, NaN)


    type = short;
    apInitScenario("asin of a short");

    verifyParams("short: n = short.min", short.MinValue, NaN)
    tstRnd("short: -2 > n > short.max", short.MinValue+1, -3)
    verifyParams("short: n = -2", -2, NaN)
    verifyParams("short: n = -1", -1, -Math.PI/2)
    verifyParams("short: n = -0", -0, 0)
    verifyParams("short: n = 0", 0, 0)
    verifyParams("short: n = 1", 1, Math.PI/2)
    verifyParams("short: n = 2", 2, NaN)
    tstRnd("short: 2 < n < short.max", 3, short.MaxValue-1)
    verifyParams("short: n = short.max", short.MaxValue, NaN)


    type = int;
    apInitScenario("asin of a int");

    verifyParams("int: n = int.min", int.MinValue, NaN)
    tstRnd("int: -2 > n > int.max", int.MinValue+1, -3)
    verifyParams("int: n = -2", -2, NaN)
    verifyParams("int: n = -1", -1, -Math.PI/2)
    verifyParams("int: n = -0", -0, 0)
    verifyParams("int: n = 0", 0, 0)
    verifyParams("int: n = 1", 1, Math.PI/2)
    verifyParams("int: n = 2", 2, NaN)
    tstRnd("int: 2 < n < int.max", 3, int.MaxValue-1)
    verifyParams("int: n = int.max", int.MaxValue, NaN)


    type = long;
    apInitScenario("asin of a long");

    verifyParams("long: n = long.min", long.MinValue, NaN)
    tstRnd("long: -2 > n > long.max", long.MinValue+1, -3)
    verifyParams("long: n = -2", -2, NaN)
    verifyParams("long: n = -1", -1, -Math.PI/2)
    verifyParams("long: n = -0", -0, 0)
    verifyParams("long: n = 0", 0, 0)
    verifyParams("long: n = 1", 1, Math.PI/2)
    verifyParams("long: n = 2", 2, NaN)
    tstRnd("long: 2 < n < long.max", 3, long.MaxValue-1)
    verifyParams("long: n = long.max", long.MaxValue, NaN)


    type = System.String;
    apInitScenario("asin of a String");

    verifyParams("string: n = -2", "-2", NaN)
    verifyParams("string: n = -1", "-1", -Math.PI/2)
    verifyParams("string: n = -0", "-0", 0)
    verifyParams("string: n = 0", "0", 0)
    verifyParams("string: n = 1", "1", Math.PI/2)
    verifyParams("string: n = 2", "2", NaN)


    type = Date;
    apInitScenario("asin of a Date");

    verifyParams("date: n = 0", 0, NaN)


    type = Array;
    apInitScenario("asin of a Array");
    var arrvar:Array = new Array(1)
    arrvar[0] = -2
    verifyParams("Array: n = -2", arrvar, NaN)
    arrvar[0] = -1
    verifyParams("Array: n = -1", arrvar, -Math.PI/2)
    arrvar[0] = -0
    verifyParams("Array: n = -0", arrvar, 0)
    arrvar[0] = 1
    verifyParams("Array: n = 1", arrvar, Math.PI/2)


    apEndTest();

}

asinh001();


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

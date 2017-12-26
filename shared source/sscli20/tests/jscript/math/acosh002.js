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


var iTestID = 181168;

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

function verify(errstr, input, expected, ... params:Object[]){
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
        verify(errstr,vNum,NaN);
    }
}



function acosh002() {


    var i
    func = System.Math.Acos


    apInitTest("acosh002 ");

    type = boolean
    apInitScenario("acos of a boolean");

    verify("boolean: n = true", true, 0)
    verify("boolean: n = false", false, Math.PI/2)



    type = char;
    apInitScenario("acos of a char");

    verify("char: n = 0", char.MinValue, Math.PI/2)
    verify("char: n = 1", 1, 0)
    verify("char: n = 2", 2, NaN)
    tstRnd("char: 2 < n < char.max", 3, char.MaxValue-1)
    verify("char: n = char.max", char.MaxValue, NaN)


    type = Single;
    apInitScenario("acos of a Single");

    verify("single: n = single.min", Single.MinValue, NaN)
    verify("single: n = -2", -2, NaN)
    verify("single: n = -1", -1, Math.PI)
    verify("single: n = -SQRT(3)/2", -Math.sqrt(3)/2, 5*Math.PI/6)
    verify("single: n = -SQRT(2)/2", -Math.sqrt(2)/2, 3*Math.PI/4)
    verify("single: n = -.5", -.5, 2*Math.PI/3)
    verify("single: n = -0", -0, Math.PI/2)
    verify("single: n = +0", +0, Math.PI/2)
    verify("single: n = .5", .5, Math.PI/3)
    verify("single: n = SQRT(2)/2", Math.sqrt(2)/2, Math.PI/4)
    verify("single: n = SQRT(3)/2", Math.sqrt(3)/2, Math.PI/6)
    verify("single: n = 1", 1, 0)
    verify("single: n = 2", 2, NaN)
    verify("single: n = single.Max", Single.MaxValue, NaN)



    type = double;
    apInitScenario("acos of a double");

    verify("double: n = double.min", double.MinValue, NaN)
    verify("double: n = -2", -2, NaN)
    verify("double: n = -1", -1, Math.PI)
    verify("double: n = -SQRT(3)/2", -Math.sqrt(3)/2, 5*Math.PI/6)
    verify("double: n = -SQRT(2)/2", -Math.sqrt(2)/2, 3*Math.PI/4)
    verify("double: n = -.5", -.5, 2*Math.PI/3)
    verify("double: n = -0", -0, Math.PI/2)
    verify("double: n = +0", +0, Math.PI/2)
    verify("double: n = .5", .5, Math.PI/3)
    verify("double: n = SQRT(2)/2", Math.sqrt(2)/2, Math.PI/4)
    verify("double: n = SQRT(3)/2", Math.sqrt(3)/2, Math.PI/6)
    verify("double: n = 1", 1, 0)
    verify("double: n = 2", 2, NaN)
    verify("double: n = double.Max", double.MaxValue, NaN)


    type = decimal;
    apInitScenario("acos of a decimal");

    verify("decimal: n = decimal.min", decimal.MinValue, NaN)
    verify("decimal: n = -2", -2, NaN)
    verify("decimal: n = -1", -1, Math.PI)
    verify("decimal: n = -SQRT(3)/2", -Math.sqrt(3)/2, 5*Math.PI/6)
    verify("decimal: n = -SQRT(2)/2", -Math.sqrt(2)/2, 3*Math.PI/4)
    verify("decimal: n = -.5", -.5, 2*Math.PI/3)
    verify("decimal: n = -0", -0, Math.PI/2)
    verify("decimal: n = +0", +0, Math.PI/2)
    verify("decimal: n = .5", .5, Math.PI/3)
    verify("decimal: n = SQRT(2)/2", Math.sqrt(2)/2, Math.PI/4)
    verify("decimal: n = SQRT(3)/2", Math.sqrt(3)/2, Math.PI/6)
    verify("decimal: n = 1", 1, 0)
    verify("decimal: n = 2", 2, NaN)
    verify("decimal: n = decimal.Max", decimal.MaxValue, NaN)


    type = byte;
    apInitScenario("acos of a byte");

    verify("byte: n = 0", byte.MinValue, Math.PI/2)
    verify("byte: n = 1", 1, 0)
    verify("byte: n = 2", 2, NaN)
    tstRnd("byte: 2 < n < byte.max", 3, byte.MaxValue-1)
    verify("byte: n = byte.max", byte.MaxValue, NaN)


    type = ushort;
    apInitScenario("acos of a ushort");

    verify("ushort: n = 0", ushort.MinValue, Math.PI/2)
    verify("ushort: n = 1", 1, 0)
    verify("ushort: n = 2", 2, NaN)
    tstRnd("ushort: 2 < n < ushort.max", 3, ushort.MaxValue-1)
    verify("ushort: n = ushort.max", ushort.MaxValue, NaN)


    type = uint;
    apInitScenario("acos of a uint");

    verify("uint: n = 0", uint.MinValue, Math.PI/2)
    verify("uint: n = 1", 1, 0)
    verify("uint: n = 2", 2, NaN)
    tstRnd("uint: 2 < n < uint.max", 3, uint.MaxValue-1)
    verify("uint: n = uint.max", uint.MaxValue, NaN)


    type = ulong;
    apInitScenario("acos of a ulong");

    verify("ulong: n = 0", ulong.MinValue, Math.PI/2)
    verify("ulong: n = 1", 1, 0)
    verify("ulong: n = 2", 2, NaN)
    tstRnd("ulong: 2 < n < ulong.max", 3, ulong.MaxValue-1)
    verify("ulong: n = ulong.max", ulong.MaxValue, NaN)


    type = sbyte;
    apInitScenario("acos of a sbyte");

    verify("sbyte: n = sbyte.min", sbyte.MinValue, NaN)
    tstRnd("sbyte: -2 > n > sbyte.max", sbyte.MinValue+1, -3)
    verify("sbyte: n = -2", -2, NaN)
    verify("sbyte: n = -1", -1, Math.PI)
    verify("sbyte: n = -0", -0, Math.PI/2)
    verify("sbyte: n = 0", 0, Math.PI/2)
    verify("sbyte: n = 1", 1, 0)
    verify("sbyte: n = 2", 2, NaN)
    tstRnd("sbyte: 2 < n < sbyte.max", 3, sbyte.MaxValue-1)
    verify("sbyte: n = sbyte.max", sbyte.MaxValue, NaN)


    type = short;
    apInitScenario("acos of a short");

    verify("short: n = short.min", short.MinValue, NaN)
    tstRnd("short: -2 > n > short.max", short.MinValue+1, -3)
    verify("short: n = -2", -2, NaN)
    verify("short: n = -1", -1, Math.PI)
    verify("short: n = -0", -0, Math.PI/2)
    verify("short: n = 0", 0, Math.PI/2)
    verify("short: n = 1", 1, 0)
    verify("short: n = 2", 2, NaN)
    tstRnd("short: 2 < n < short.max", 3, short.MaxValue-1)
    verify("short: n = short.max", short.MaxValue, NaN)


    type = int;
    apInitScenario("acos of a int");

    verify("int: n = int.min", int.MinValue, NaN)
    tstRnd("int: -2 > n > int.max", int.MinValue+1, -3)
    verify("int: n = -2", -2, NaN)
    verify("int: n = -1", -1, Math.PI)
    verify("int: n = -0", -0, Math.PI/2)
    verify("int: n = 0", 0, Math.PI/2)
    verify("int: n = 1", 1, 0)
    verify("int: n = 2", 2, NaN)
    tstRnd("int: 2 < n < int.max", 3, int.MaxValue-1)
    verify("int: n = int.max", int.MaxValue, NaN)


    type = long;
    apInitScenario("acos of a long");

    verify("long: n = long.min", long.MinValue, NaN)
    tstRnd("long: -2 > n > long.max", long.MinValue+1, -3)
    verify("long: n = -2", -2, NaN)
    verify("long: n = -1", -1, Math.PI)
    verify("long: n = -0", -0, Math.PI/2)
    verify("long: n = 0", 0, Math.PI/2)
    verify("long: n = 1", 1, 0)
    verify("long: n = 2", 2, NaN)
    tstRnd("long: 2 < n < long.max", 3, long.MaxValue-1)
    verify("long: n = long.max", long.MaxValue, NaN)


    type = System.String;
    apInitScenario("acos of a String");

    verify("string: n = -2", "-2", NaN)
    verify("string: n = -1", "-1", Math.PI)
    verify("string: n = -0", "-0", Math.PI/2)
    verify("string: n = 0", "0", Math.PI/2)
    verify("string: n = 1", "1", 0)
    verify("string: n = 2", "2", NaN)


    type = Date;
    apInitScenario("acos of a Date");

    verify("date: n = 0", 0, NaN)


    type = Array;
    apInitScenario("acos of a Array");
    var arrvar:Array = new Array(1)
    arrvar[0] = -2
    verify("Array: n = -2", arrvar, NaN)
    arrvar[0] = -1
    verify("Array: n = -1", arrvar, Math.PI)
    arrvar[0] = -0
    verify("Array: n = -0", arrvar, Math.PI/2)
    arrvar[0] = 1
    verify("Array: n = 1", arrvar, 0)


    apEndTest();

}


acosh002();


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

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


var iTestID = 179279;

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



function tanh001() {


    var i
    func = Math.tan


    apInitTest("tanh001 ");

    type = boolean
    apInitScenario("tan of a boolean");

    verifyParams("boolean: n = true", true, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("boolean: n = false", false, 0, 1e-10)



    type = char;
    apInitScenario("tan of a char");

    verifyParams("char: n = 0", char.MinValue, 0, 1e-10)
    verifyParams("char: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("char: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("char: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)
    verifyParams("char: n = char.max", char.MaxValue, 5.101939568196287312795332475991, 1e-10)


    type = Single;
    apInitScenario("tan of a Single");

    verifyParams("single: n = -Infinity", -Infinity, NaN, 1e-7)
    //verifyParams("single: n = single.min", Single.MinValue, -Math.PI/2, 1e-7)
    verifyParams("single: n = -100", -100, 0.58721391515692907667780963564459, 1e-7)
    verifyParams("single: n = -10", -10, -0.64836082745908667125912493300981, 1e-7)
    verifyParams("single: n = -SQRT(3)", -Math.PI/3, -Math.sqrt(3), 1e-6)
    verifyParams("single: n = -1", -1, -1.5574077246549022305069748074584, 1e-7)
    verifyParams("single: n = -1/SQRT(3)", -Math.PI/6, -1/Math.sqrt(3), 1e-6)
    verifyParams("single: n = -0", -0, 0, 1e-7)
    verifyParams("single: n = +0", +0, 0, 1e-7)
    verifyParams("single: n = 1/SQRT(3)", Math.PI/6, 1/Math.sqrt(3), 1e-6)
    verifyParams("single: n = 1", 1, 1.5574077246549022305069748074584, 1e-7)
    verifyParams("single: n = SQRT(3)", Math.PI/3, Math.sqrt(3), 1e-6)
    verifyParams("single: n = 10", 10, 0.64836082745908667125912493300981, 1e-7)
    verifyParams("single: n = 100", 100, -0.58721391515692907667780963564459, 1e-7)
    //verifyParams("single: n = single.Max", Single.MaxValue, Math.PI/2, 1e-7)
    verifyParams("single: n = Infinity", Infinity, NaN, 1e-7)



    type = double;
    apInitScenario("tan of a double");

    verifyParams("double: n = -Infinity", -Infinity, NaN, 1e-14)
    //verifyParams("double: n = double.min", double.MinValue, -Math.PI/2, 1e-14)
    verifyParams("double: n = -100", -100, 0.58721391515692907667780963564459, 1e-14)
    verifyParams("double: n = -10", -10, -0.64836082745908667125912493300981, 1e-14)
    verifyParams("double: n = -SQRT(3)", -Math.PI/3, -Math.sqrt(3), 1e-14)
    verifyParams("double: n = -1", -1, -1.5574077246549022305069748074584, 1e-14)
    verifyParams("double: n = -1/SQRT(3)", -Math.PI/6, -1/Math.sqrt(3), 1e-14)
    verifyParams("double: n = -0", -0, 0, 1e-14)
    verifyParams("double: n = +0", +0, 0, 1e-14)
    verifyParams("double: n = 1/SQRT(3)", Math.PI/6, 1/Math.sqrt(3), 1e-14)
    verifyParams("double: n = 1", 1, 1.5574077246549022305069748074584, 1e-14)
    verifyParams("double: n = SQRT(3)", Math.PI/3, Math.sqrt(3), 1e-14)
    verifyParams("double: n = 10", 10, 0.64836082745908667125912493300981, 1e-14)
    verifyParams("double: n = 100", 100, -0.58721391515692907667780963564459, 1e-14)
    //verifyParams("double: n = double.Max", double.MaxValue, Math.PI/2, 1e-14)
    verifyParams("double: n = Infinity", Infinity, NaN, 1e-14)


    type = decimal;
    apInitScenario("tan of a decimal");

    //verifyParams("decimal: n = decimal.min", decimal.MinValue, -Math.PI/2, 1e-14)
    verifyParams("decimal: n = -100", -100, 0.58721391515692907667780963564459, 1e-14)
    verifyParams("decimal: n = -10", -10, -0.64836082745908667125912493300981, 1e-14)
    verifyParams("decimal: n = -SQRT(3)", -Math.PI/3, -Math.sqrt(3), 1e-14)
    verifyParams("decimal: n = -1", -1, -1.5574077246549022305069748074584, 1e-14)
    verifyParams("decimal: n = -1/SQRT(3)", -Math.PI/6, -1/Math.sqrt(3), 1e-14)
    verifyParams("decimal: n = -0", -0, 0, 1e-14)
    verifyParams("decimal: n = +0", +0, 0, 1e-14)
    verifyParams("decimal: n = 1/SQRT(3)", Math.PI/6, 1/Math.sqrt(3), 1e-14)
    verifyParams("decimal: n = 1", 1, 1.5574077246549022305069748074584, 1e-14)
    verifyParams("decimal: n = SQRT(3)", Math.PI/3, Math.sqrt(3), 1e-14)
    verifyParams("decimal: n = 10", 10, 0.64836082745908667125912493300981, 1e-14)
    verifyParams("decimal: n = 100", 100, -0.58721391515692907667780963564459, 1e-14)
    //verifyParams("decimal: n = decimal.Max", decimal.MaxValue, Math.PI/2, 1e-14)


    type = byte;
    apInitScenario("tan of a byte");

    verifyParams("byte: n = 0", byte.MinValue, 0, 1e-10)
    verifyParams("byte: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("byte: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("byte: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)
    verifyParams("byte: n = byte.max", byte.MaxValue, 0.58725445460931961121131896830342, 1e-10)


    type = ushort;
    apInitScenario("tan of a ushort");

    verifyParams("ushort: n = 0", ushort.MinValue, 0, 1e-10)
    verifyParams("ushort: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("ushort: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("ushort: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)
    verifyParams("ushort: n = ushort.max", ushort.MaxValue, 5.101939568196287312795332475991, 1e-10)


    type = uint;
    apInitScenario("tan of a uint");

    verifyParams("uint: n = 0", uint.MinValue, 0, 1e-10)
    verifyParams("uint: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("uint: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("uint: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)
    verifyParams("uint: n = uint.max", uint.MaxValue, -0.57225137018054720865320985327688, 1e-10)


    type = ulong;
    apInitScenario("tan of a ulong");

    verifyParams("ulong: n = 0", ulong.MinValue, 0, 1e-10)
    verifyParams("ulong: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("ulong: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("ulong: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)
    //verifyParams("ulong: n = ulong.max", ulong.MaxValue, -1.6413534576750778311397455799875, 1e-10)


    type = sbyte;
    apInitScenario("tan of a sbyte");

    verifyParams("sbyte: n = sbyte.min", sbyte.MinValue, 1.0406148914328551831457275483397, 1e-10)
    verifyParams("sbyte: n = -100", -100, 0.58721391515692907667780963564459, 1e-10)
    verifyParams("sbyte: n = -10", -10, -0.64836082745908667125912493300981, 1e-10)
    verifyParams("sbyte: n = -1", -1, -1.5574077246549022305069748074584, 1e-10)
    verifyParams("sbyte: n = 0", 0, 0, 1e-10)
    verifyParams("sbyte: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("sbyte: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("sbyte: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)
    verifyParams("sbyte: n = sbyte.max", sbyte.MaxValue, 4.1858918318519886376247517518217, 1e-10)


    type = short;
    apInitScenario("tan of a short");

    verifyParams("short: n = short.min", short.MinValue, -2.4879651793076246915612969187096, 1e-10)
    verifyParams("short: n = -100", -100, 0.58721391515692907667780963564459, 1e-10)
    verifyParams("short: n = -10", -10, -0.64836082745908667125912493300981, 1e-10)
    verifyParams("short: n = -1", -1, -1.5574077246549022305069748074584, 1e-10)
    verifyParams("short: n = 0", 0, 0, 1e-10)
    verifyParams("short: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("short: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("short: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)
    verifyParams("short: n = short.max", short.MaxValue, 0.19089234430221485740826374865713, 1e-10)


    type = int;
    apInitScenario("tan of a int");

    verifyParams("int: n = int.min", int.MinValue, 4.0842894552985930438475781104297, 1e-10)
    verifyParams("int: n = -100", -100, 0.58721391515692907667780963564459, 1e-10)
    verifyParams("int: n = -10", -10, -0.64836082745908667125912493300981, 1e-10)
    verifyParams("int: n = -1", -1, -1.5574077246549022305069748074584, 1e-10)
    verifyParams("int: n = 0", 0, 0, 1e-10)
    verifyParams("int: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("int: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("int: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)
    verifyParams("int: n = int.max", int.MaxValue, 1.0523779637351339136698139284747, 1e-10)


    type = long;
    apInitScenario("tan of a long");

    //verifyParams("long: n = long.min", long.MinValue, -84.739312968755665875777173476351, 1e-10)
    verifyParams("long: n = -100", -100, 0.58721391515692907667780963564459, 1e-10)
    verifyParams("long: n = -10", -10, -0.64836082745908667125912493300981, 1e-10)
    verifyParams("long: n = -1", -1, -1.5574077246549022305069748074584, 1e-10)
    verifyParams("long: n = 0", 0, 0, 1e-10)
    verifyParams("long: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("long: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("long: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)
    //verifyParams("long: n = long.max", long.MaxValue, 0.62555174362418179716236325114354, 1e-10)


    type = System.String;
    apInitScenario("tan of a String");

    verifyParams("string: n = -100", -100, 0.58721391515692907667780963564459, 1e-10)
    verifyParams("string: n = -10", -10, -0.64836082745908667125912493300981, 1e-10)
    verifyParams("string: n = -1", -1, -1.5574077246549022305069748074584, 1e-10)
    verifyParams("string: n = 0", 0, 0, 1e-10)
    verifyParams("string: n = 1", 1, 1.5574077246549022305069748074584, 1e-10)
    verifyParams("string: n = 10", 10, 0.64836082745908667125912493300981, 1e-10)
    verifyParams("string: n = 100", 100, -0.58721391515692907667780963564459, 1e-10)


    type = Date;
    apInitScenario("tan of a Date");

    verifyParams("date: n = 0", 0, NaN)


    type = Array;
    apInitScenario("tan of a Array");
    var arrvar:Array = new Array(1)
    arrvar[0] = -10
    verifyParams("Array: n = -10", arrvar, -0.64836082745908667125912493300981, 1e-10)
    arrvar[0] = -1
    verifyParams("Array: n = -1", arrvar, -1.5574077246549022305069748074584, 1e-10)
    arrvar[0] = -0
    verifyParams("Array:1 n = -0", arrvar, 0, 1e-10)
    arrvar[0] = 1
    verifyParams("Array: n = 1", arrvar, 1.5574077246549022305069748074584, 1e-10)


    apEndTest();

}


tanh001();


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

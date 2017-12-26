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


var iTestID = 181173;

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


function verifyParams(errstr, input1, input2, expected, ... params:Object[]){
    var delta = .000001
    if (params.length>0) delta = params[0]


    if (typeof(func(input1, input2)) !== typeof(expected))
        apLogFailInfo(errstr + ": type failure", typeof(expected), typeof(func(input1, input2)), "");


    if (diff( func(type(input1), type(input2)), expected, delta))
        apLogFailInfo(errstr + ": immediate failure", expected, func(type(input1), type(input2)), "");


    //eval("	var insttype1:type=input1, insttype2:type = input2; if (diff( func(insttype1, insttype2), expected, delta)) apLogFailInfo(errstr + ': typed instance failure', expected, func(insttype1, insttype2), ''); ")


    var instob1:Object = type(input1)
    var instob2:Object = type(input2)
    if (diff( func(instob1, instob2), expected, delta))
        apLogFailInfo(errstr + ": object instance failure", expected, func(instob1, instob2), "");


    var memob1 = new Object
    var memob2 = new Object
    memob1.mem = type(input1)
    memob2.mem = type(input2)
    if (diff( func(memob1.mem, memob2.mem), expected, delta))
        apLogFailInfo(errstr + ": object member failure", expected, func(memob1.mem, memob2.mem), "");


    if (diff( func(  retvar(type(input1)), retvar(type(input2))  ), expected, delta))
        apLogFailInfo(errstr + ": function return failure", expected, func(  retvar(type(input1)), retvar(type(input2))  ), "");


    if (diff( func(  rettyped1(input1), rettyped1(input2)  ), expected, delta))
        apLogFailInfo(errstr + ": function typed return failure 1", expected, func(  rettyped1(input1), rettyped1(input2)  ), "");


    //if (diff( func(  rettyped2(input1), rettyped2(input2)  ), expected, delta))
    //    apLogFailInfo(errstr + ": function typed return failure 2", expected, func(  rettyped2(input1), rettyped2(input2)  ), "");

}



function atan2h002() {


    var i
    func = System.Math.Atan2


    apInitTest("atan2h002 ");

    type = boolean
    apInitScenario("atan2 of a boolean");

    verifyParams("boolean: n = true/true", true, true, Math.PI/4, 1e-10)
    verifyParams("boolean: n = false/true", false, true, 0, 1e-10)
    verifyParams("boolean: n = true/false", true, false, Math.PI/2, 1e-10)
    verifyParams("boolean: n = false/false", false, false, 0, 1e-10)



    type = char;
    apInitScenario("atan2 of a char");

    verifyParams("char: n = 0/1", char.MinValue, 0, 0, 1e-10)
    verifyParams("char: n = 1/100", 1, 100, 0.0099996666866652382063401162092795, 1e-10)
    verifyParams("char: n = 1/10", 1, 10, 0.099668652491162027378446119878021, 1e-10)
    verifyParams("char: n = 1/1", 1, 1, Math.PI/4, 1e-10)
    verifyParams("char: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-10)
    verifyParams("char: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-10)
    verifyParams("char: n = char.max/1", char.MaxValue, 1, 1.5707810677730011071016668574942, 1e-10)
    verifyParams("char: n = char.max/0", char.MaxValue, 0, Math.PI/2, 1e-10)


    type = Single;
    apInitScenario("atan2 of a Single");

    verifyParams("single: n = -Infinity/1e-10", -Infinity, 1e-10, -Math.PI/2, 1e-7)
    verifyParams("single: n = -Infinity/1", -Infinity, 1, -Math.PI/2, 1e-7)
    verifyParams("single: n = -Infinity/Single.max", -Infinity, Single.MaxValue, -Math.PI/2, 1e-7)
    verifyParams("single: n = -Infinity/Infinity", -Infinity, Infinity, NaN, 1e-7)
    verifyParams("single: n = -1/0", -1, 0, -Math.PI/2, 1e-7)

    verifyParams("single: n = single.min/1", Single.MinValue, 1, -Math.PI/2, 1e-7)
    verifyParams("single: n = -100/1", -100, 1, -1.5607966601082313810249815754305, 1e-7)
    verifyParams("single: n = -10/1", -10, 1, -1.4711276743037345918528755717617, 1e-7)
    verifyParams("single: n = -SQRT(3)/1", -Math.sqrt(3), 1, -Math.PI/3, 1e-7)
    verifyParams("single: n = 1/-1", 1, -1, 2.3561944901923449288469825374596, 1e-7)
    verifyParams("single: n = -1/SQRT(3)", -1, Math.sqrt(3), -Math.PI/6, 1e-7)
    verifyParams("single: n = -0/-0", -0, -0, -Math.PI, 0)
    verifyParams("single: n = +0/-0", +0, -0, Math.PI, 0)
    verifyParams("single: n = -0/+0", -0, +0, 0, 0)
    verifyParams("single: n = +0/+0", +0, +0, 0, 0)
    verifyParams("single: n = 1/SQRT(3)", 1, Math.sqrt(3), Math.PI/6, 1e-7)
    verifyParams("single: n = 1/1", 1, 1, Math.PI/4, 1e-7)
    verifyParams("single: n = SQRT(3)/1", Math.sqrt(3), 1, Math.PI/3, 1e-7)
    verifyParams("single: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-7)
    verifyParams("single: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-7)
    verifyParams("single: n = single.Max/1", Single.MaxValue, 1, Math.PI/2, 1e-7)

    verifyParams("single: n = 1/0", 1, 0, Math.PI/2, 1e-7)
    verifyParams("single: n = Infinity/Infinity", Infinity, Infinity, NaN, 1e-7)
    verifyParams("single: n = Infinity/Single.max", Infinity, Single.MaxValue, Math.PI/2, 1e-7)
    verifyParams("single: n = Infinity/1", Infinity, 1, Math.PI/2, 1e-7)
    verifyParams("single: n = Infinity/1e-10", Infinity, 1e-10, Math.PI/2, 1e-7)


    type = double;
    apInitScenario("atan2 of a double");

    verifyParams("double: n = -Infinity/1e-10", -Infinity, 1e-10, -Math.PI/2, 1e-7)
    verifyParams("double: n = -Infinity/1", -Infinity, 1, -Math.PI/2, 1e-7)
    verifyParams("double: n = -Infinity/double.max", -Infinity, double.MaxValue, -Math.PI/2, 1e-7)
    verifyParams("double: n = -Infinity/Infinity", -Infinity, Infinity, NaN, 1e-7)
    verifyParams("double: n = -1/0", -1, 0, -Math.PI/2, 1e-7)

    verifyParams("double: n = double.min/1", double.MinValue, 1, -Math.PI/2, 1e-7)
    verifyParams("double: n = -100/1", -100, 1, -1.5607966601082313810249815754305, 1e-7)
    verifyParams("double: n = -10/1", -10, 1, -1.4711276743037345918528755717617, 1e-7)
    verifyParams("double: n = -SQRT(3)/1", -Math.sqrt(3), 1, -Math.PI/3, 1e-7)
    verifyParams("double: n = 1/-1", 1, -1, 2.3561944901923449288469825374596, 1e-7)
    verifyParams("double: n = -1/SQRT(3)", -1, Math.sqrt(3), -Math.PI/6, 1e-7)
    verifyParams("double: n = -0/-0", -0, -0, -Math.PI, 0)
    verifyParams("double: n = +0/-0", +0, -0, Math.PI, 0)
    verifyParams("double: n = -0/+0", -0, +0, 0, 0)
    verifyParams("double: n = +0/+0", +0, +0, 0, 0)
    verifyParams("double: n = 1/SQRT(3)", 1, Math.sqrt(3), Math.PI/6, 1e-7)
    verifyParams("double: n = 1/1", 1, 1, Math.PI/4, 1e-7)
    verifyParams("double: n = SQRT(3)/1", Math.sqrt(3), 1, Math.PI/3, 1e-7)
    verifyParams("double: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-7)
    verifyParams("double: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-7)
    verifyParams("double: n = double.Max/1", double.MaxValue, 1, Math.PI/2, 1e-7)

    verifyParams("double: n = 1/0", 1, 0, Math.PI/2, 1e-7)
    verifyParams("double: n = Infinity/Infinity", Infinity, Infinity, NaN, 1e-7)
    verifyParams("double: n = Infinity/double.max", Infinity, double.MaxValue, Math.PI/2, 1e-7)
    verifyParams("double: n = Infinity/1", Infinity, 1, Math.PI/2, 1e-7)
    verifyParams("double: n = Infinity/1e-10", Infinity, 1e-10, Math.PI/2, 1e-7)


    type = decimal;
    apInitScenario("atan2 of a decimal");

    verifyParams("decimal: n = -1/0", -1, 0, -Math.PI/2, 1e-7)
    verifyParams("decimal: n = decimal.min/1", decimal.MinValue, 1, -Math.PI/2, 1e-7)
    verifyParams("decimal: n = -100/1", -100, 1, -1.5607966601082313810249815754305, 1e-7)
    verifyParams("decimal: n = -10/1", -10, 1, -1.4711276743037345918528755717617, 1e-7)
    verifyParams("decimal: n = -SQRT(3)/1", -Math.sqrt(3), 1, -Math.PI/3, 1e-7)
    verifyParams("decimal: n = 1/-1", 1, -1, 2.3561944901923449288469825374596, 1e-7)
    verifyParams("decimal: n = -1/SQRT(3)", -1, Math.sqrt(3), -Math.PI/6, 1e-7)
    verifyParams("decimal: n = +0/+0", +0, +0, 0, 0)
    verifyParams("decimal: n = 1/SQRT(3)", 1, Math.sqrt(3), Math.PI/6, 1e-7)
    verifyParams("decimal: n = 1/1", 1, 1, Math.PI/4, 1e-7)
    verifyParams("decimal: n = SQRT(3)/1", Math.sqrt(3), 1, Math.PI/3, 1e-7)
    verifyParams("decimal: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-7)
    verifyParams("decimal: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-7)
    verifyParams("decimal: n = decimal.Max/1", decimal.MaxValue, 1, Math.PI/2, 1e-7)
    verifyParams("decimal: n = 1/0", 1, 0, Math.PI/2, 1e-7)


    type = byte;
    apInitScenario("atan2 of a byte");

    verifyParams("byte: n = 0/1", byte.MinValue, 0, 0, 1e-10)
    verifyParams("byte: n = 1/100", 1, 100, 0.0099996666866652382063401162092795, 1e-10)
    verifyParams("byte: n = 1/10", 1, 10, 0.099668652491162027378446119878021, 1e-10)
    verifyParams("byte: n = 1/1", 1, 1, Math.PI/4, 1e-10)
    verifyParams("byte: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-10)
    verifyParams("byte: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-10)
    verifyParams("byte: n = byte.max/1", byte.MaxValue, 1, 1.566874778270136617543844909316, 1e-10)
    verifyParams("byte: n = byte.max/0", byte.MaxValue, 0, Math.PI/2, 1e-10)


    type = ushort;
    apInitScenario("atan2 of a ushort");

    verifyParams("ushort: n = 0/1", ushort.MinValue, 0, 0, 1e-10)
    verifyParams("ushort: n = 1/100", 1, 100, 0.0099996666866652382063401162092795, 1e-10)
    verifyParams("ushort: n = 1/10", 1, 10, 0.099668652491162027378446119878021, 1e-10)
    verifyParams("ushort: n = 1/1", 1, 1, Math.PI/4, 1e-10)
    verifyParams("ushort: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-10)
    verifyParams("ushort: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-10)
    verifyParams("ushort: n = ushort.max/1", ushort.MaxValue, 1, 1.5707810677730011071016668574942, 1e-10)
    verifyParams("ushort: n = ushort.max/0", ushort.MaxValue, 0, Math.PI/2, 1e-10)


    type = uint;
    apInitScenario("atan2 of a uint");

    verifyParams("uint: n = 0/1", uint.MinValue, 0, 0, 1e-10)
    verifyParams("uint: n = 1/100", 1, 100, 0.0099996666866652382063401162092795, 1e-10)
    verifyParams("uint: n = 1/10", 1, 10, 0.099668652491162027378446119878021, 1e-10)
    verifyParams("uint: n = 1/1", 1, 1, Math.PI/4, 1e-10)
    verifyParams("uint: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-10)
    verifyParams("uint: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-10)
    verifyParams("uint: n = uint.max/1", uint.MaxValue, 1, 1.5707963265620659755232419541008, 1e-10)
    verifyParams("uint: n = uint.max/0", uint.MaxValue, 0, Math.PI/2, 1e-10)


    type = ulong;
    apInitScenario("atan2 of a ulong");

    verifyParams("ulong: n = 0/1", ulong.MinValue, 0, 0, 1e-10)
    verifyParams("ulong: n = 1/100", 1, 100, 0.0099996666866652382063401162092795, 1e-10)
    verifyParams("ulong: n = 1/10", 1, 10, 0.099668652491162027378446119878021, 1e-10)
    verifyParams("ulong: n = 1/1", 1, 1, Math.PI/4, 1e-10)
    verifyParams("ulong: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-10)
    verifyParams("ulong: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-10)
    verifyParams("ulong: n = ulong.max/1", ulong.MaxValue, 1, 1.5707963267948966191771115830155, 1e-10)
    verifyParams("ulong: n = ulong.max/0", ulong.MaxValue, 0, Math.PI/2, 1e-10)


    type = sbyte;
    apInitScenario("atan2 of a sbyte");

    verifyParams("sbyte: n = -1/0", -1, 0, -Math.PI/2, 1e-14)
    verifyParams("sbyte: n = sbyte.min/1", sbyte.MinValue, 1, -1.5629839857347955079348582997976, 1e-14)
    verifyParams("sbyte: n = sbyte.min/-1", sbyte.MinValue, -1, -1.578608667854997730527785083482, 1e-14)
    verifyParams("sbyte: n = -100/1", -100, 1, -1.5607966601082313810249815754305, 1e-14)
    verifyParams("sbyte: n = -100/-1", -100, -1, -1.580795993481561857437661807849, 1e-14)
    verifyParams("sbyte: n = -10/1", -10, 1, -1.4711276743037345918528755717617, 1e-14)
    verifyParams("sbyte: n = -10/-1", -10, -1, -1.6704649792860586466097678115178, 1e-14)
    verifyParams("sbyte: n = -1/1", -1, 1, -Math.PI/4, 1e-14)
    verifyParams("sbyte: n = -1/-1", -1, -1, -3*Math.PI/4, 1e-14)
    verifyParams("sbyte: n = +0/+0", +0, +0, 0, 0)
    verifyParams("sbyte: n = 1/1", 1, 1, Math.PI/4, 1e-14)
    verifyParams("sbyte: n = 1/-1", 1, -1, 3*Math.PI/4, 1e-14)
    verifyParams("sbyte: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-14)
    verifyParams("sbyte: n = 10/-1", 10, -1, 1.6704649792860586466097678115178, 1e-14)
    verifyParams("sbyte: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-14)
    verifyParams("sbyte: n = 100/-1", 100, -1, 1.580795993481561857437661807849, 1e-14)
    verifyParams("sbyte: n = sbyte.Max/1", sbyte.MaxValue, 1, 1.5629224737707960763405916619423, 1e-14)
    verifyParams("sbyte: n = sbyte.Max/-1", sbyte.MaxValue, -1, 1.5786701798189971621220517213372, 1e-14)
    verifyParams("sbyte: n = 1/0", 1, 0, Math.PI/2, 1e-14)


    type = short;
    apInitScenario("atan2 of a short");

    verifyParams("short: n = -1/0", -1, 0, -Math.PI/2, 1e-14)
    verifyParams("short: n = short.min/1", short.MinValue, 1, -1.5707658092167810931344598656863, 1e-14)
    verifyParams("short: n = short.min/-1", short.MinValue, -1, -1.5708268443730121453281835175932, 1e-14)
    verifyParams("short: n = -100/1", -100, 1, -1.5607966601082313810249815754305, 1e-14)
    verifyParams("short: n = -100/-1", -100, -1, -1.580795993481561857437661807849, 1e-14)
    verifyParams("short: n = -10/1", -10, 1, -1.4711276743037345918528755717617, 1e-14)
    verifyParams("short: n = -10/-1", -10, -1, -1.6704649792860586466097678115178, 1e-14)
    verifyParams("short: n = -1/1", -1, 1, -Math.PI/4, 1e-14)
    verifyParams("short: n = -1/-1", -1, -1, -3*Math.PI/4, 1e-14)
    verifyParams("short: n = +0/+0", +0, +0, 0, 0)
    verifyParams("short: n = 1/1", 1, 1, Math.PI/4, 1e-14)
    verifyParams("short: n = 1/-1", 1, -1, 3*Math.PI/4, 1e-14)
    verifyParams("short: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-14)
    verifyParams("short: n = 10/-1", 10, -1, 1.6704649792860586466097678115178, 1e-14)
    verifyParams("short: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-14)
    verifyParams("short: n = 100/-1", 100, -1, 1.580795993481561857437661807849, 1e-14)
    verifyParams("short: n = short.Max/1", short.MaxValue, 1, 1.5707658082854300968095774169106, 1e-14)
    verifyParams("short: n = short.Max/-1", short.MaxValue, -1, 1.5708268453043631416530659663689, 1e-14)
    verifyParams("short: n = 1/0", 1, 0, Math.PI/2, 1e-14)


    type = int;
    apInitScenario("atan2 of a int");

    verifyParams("int: n = -1/0", -1, 0, -Math.PI/2, 1e-14)
    verifyParams("int: n = int.min/1", int.MinValue, 1, -1.5707963263292353319235824338609, 1e-14)
    verifyParams("int: n = int.min/-1", int.MinValue, -1, -1.5707963272605579065390609494186, 1e-14)
    verifyParams("int: n = -100/1", -100, 1, -1.5607966601082313810249815754305, 1e-14)
    verifyParams("int: n = -100/-1", -100, -1, -1.580795993481561857437661807849, 1e-14)
    verifyParams("int: n = -10/1", -10, 1, -1.4711276743037345918528755717617, 1e-14)
    verifyParams("int: n = -10/-1", -10, -1, -1.6704649792860586466097678115178, 1e-14)
    verifyParams("int: n = -1/1", -1, 1, -Math.PI/4, 1e-14)
    verifyParams("int: n = -1/-1", -1, -1, -3*Math.PI/4, 1e-14)
    verifyParams("int: n = +0/+0", +0, +0, 0, 0)
    verifyParams("int: n = 1/1", 1, 1, Math.PI/4, 1e-14)
    verifyParams("int: n = 1/-1", 1, -1, 3*Math.PI/4, 1e-14)
    verifyParams("int: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-14)
    verifyParams("int: n = 10/-1", 10, -1, 1.6704649792860586466097678115178, 1e-14)
    verifyParams("int: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-14)
    verifyParams("int: n = 100/-1", 100, -1, 1.580795993481561857437661807849, 1e-14)
    verifyParams("int: n = int.Max/1", int.MaxValue, 1, 1.5707963263292353317067419992628, 1e-14)
    verifyParams("int: n = int.Max/-1", int.MaxValue, -1, 1.5707963272605579067559013840167, 1e-14)
    verifyParams("int: n = 1/0", 1, 0, Math.PI/2, 1e-14)


    type = long;
    apInitScenario("atan2 of a long");

    verifyParams("long: n = -1/0", -1, 0, -Math.PI/2, 1e-14)
    verifyParams("long: n = long.min/1", long.MinValue, 1, -1.5707963267948966191229014743912, 1e-14)
    verifyParams("long: n = long.min/-1", long.MinValue, -1, -1.5707963267948966193397419088883, 1e-14)
    verifyParams("long: n = -100/1", -100, 1, -1.5607966601082313810249815754305, 1e-14)
    verifyParams("long: n = -100/-1", -100, -1, -1.580795993481561857437661807849, 1e-14)
    verifyParams("long: n = -10/1", -10, 1, -1.4711276743037345918528755717617, 1e-14)
    verifyParams("long: n = -10/-1", -10, -1, -1.6704649792860586466097678115178, 1e-14)
    verifyParams("long: n = -1/1", -1, 1, -Math.PI/4, 1e-14)
    verifyParams("long: n = -1/-1", -1, -1, -3*Math.PI/4, 1e-14)
    verifyParams("long: n = +0/+0", +0, +0, 0, 0)
    verifyParams("long: n = 1/1", 1, 1, Math.PI/4, 1e-14)
    verifyParams("long: n = 1/-1", 1, -1, 3*Math.PI/4, 1e-14)
    verifyParams("long: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-14)
    verifyParams("long: n = 10/-1", 10, -1, 1.6704649792860586466097678115178, 1e-14)
    verifyParams("long: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-14)
    verifyParams("long: n = 100/-1", 100, -1, 1.580795993481561857437661807849, 1e-14)
    verifyParams("long: n = long.Max/1", long.MaxValue, 1, 1.5707963267948966191229014743912, 1e-14)
    verifyParams("long: n = long.Max/-1", long.MaxValue, -1, 1.5707963267948966193397419088883, 1e-14)
    verifyParams("long: n = 1/0", 1, 0, Math.PI/2, 1e-14)


    type = System.String;
    apInitScenario("atan2 of a String");

    verifyParams("String: n = -100/1", -100, 1, -1.5607966601082313810249815754305, 1e-14)
    verifyParams("String: n = -100/-1", -100, -1, -1.580795993481561857437661807849, 1e-14)
    verifyParams("String: n = -10/1", -10, 1, -1.4711276743037345918528755717617, 1e-14)
    verifyParams("String: n = -10/-1", -10, -1, -1.6704649792860586466097678115178, 1e-14)
    verifyParams("String: n = -1/1", -1, 1, -Math.PI/4, 1e-14)
    verifyParams("String: n = -1/-1", -1, -1, -3*Math.PI/4, 1e-14)
    verifyParams("String: n = +0/+0", +0, +0, 0, 0)
    verifyParams("String: n = 1/1", 1, 1, Math.PI/4, 1e-14)
    verifyParams("String: n = 1/-1", 1, -1, 3*Math.PI/4, 1e-14)
    verifyParams("String: n = 10/1", 10, 1, 1.4711276743037345918528755717617, 1e-14)
    verifyParams("String: n = 10/-1", 10, -1, 1.6704649792860586466097678115178, 1e-14)
    verifyParams("String: n = 100/1", 100, 1, 1.5607966601082313810249815754305, 1e-14)
    verifyParams("String: n = 100/-1", 100, -1, 1.580795993481561857437661807849, 1e-14)


    type = Date;
    apInitScenario("atan2 of a Date");

    verifyParams("date: n = 0", 0, 1, NaN)


    type = Array;
    apInitScenario("atan2 of a Array");
    var arrvar1:Array = new Array(1), arrvar2:Array = new Array(1);
    arrvar1[0] = -10; arrvar2[0] = 1;
    verifyParams("Array: n = -10", arrvar1, arrvar2, -1.4711276743037345918528755717617, 1e-10)
    arrvar1[0] = -1; arrvar2[0] = 1;
    verifyParams("Array: n = -1", arrvar1, arrvar2, -Math.PI/4, 1e-10)
    arrvar1[0] = -0; arrvar2[0] = 1;
    verifyParams("Array:1 n = -0", arrvar1, arrvar2, 0, 1e-10)
    arrvar1[0] = 1; arrvar2[0] = 1;
    verifyParams("Array: n = 1", arrvar1, arrvar2, Math.PI/4, 1e-10)


    apEndTest();

}


atan2h002();


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

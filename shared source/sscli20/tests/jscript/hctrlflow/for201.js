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


var iTestID = 193518;

///////////////////////////////////////////////////////////////////////////////////
//import System

class go{

function start(){

    var booleanTyped   : boolean = boolean(true);
    var booleanObj     : Object  = boolean(true);

    var sbyteTyped     : sbyte   = sbyte(-50);
    var sbyteObj       : Object  = sbyte(-50);
    var byteTyped      : byte    = byte(50);
    var byteObj        : Object  = byte(50);

    var int16Typed     : short   = short(-50);
    var int16Obj       : Object  = short(-50);
    var uint16Typed    : ushort  = ushort(50);
    var uint16Obj      : Object  = ushort(50);

    var int32Typed     : int     = int(-50);
    var int32Obj       : Object  = int(-50);
    var uint32Typed    : uint    = uint(50);
    var uint32Obj      : Object  = uint(50);

    var int64Typed     : long    = long(-50);
    var int64Obj       : Object  = long(-50);
    var uint64Typed     : ulong   = ulong(50);
    var uint64Obj       : Object  = ulong(50);

    var singleTyped    : float   = float(-50);
    var singleObj      : Object  = float(-50);

    var doubleTyped    : double  = double(-50);
    var doubleObj      : Object  = double(-50);

    var numberTyped    : Number  = Number(-50);
    var numberObj      : Object  = Number(-50);

    var numobjTyped    : Number  = new Number(-50);
    var numobjObj      : Object  = new Number(-50);

    var strTyped       : String  = String("2");
    var strObj         : Object  = String("2");

    var charTyped      : char    = char("\u0032");
    var charObj        : Object  = char("\u0032");

    var DateTyped      : Date    = new Date(-50)
    var DateObj        : Object  = new Date(-50)

    var decimalTyped   : decimal = decimal(-50)
    var decimalObj     : Object  = decimal(-50)

    var count

////////////////////////////////////////////////////////////////////////////////////
    apInitScenario("for loop with a hard typed boolean")

    count = 0;
    
    for(booleanTyped = true; booleanTyped; count++){
        if (count >9) booleanTyped = false
    }
    if (count != 11) apLogFailInfo("Looped wrong number of times", 11, count, "")
    
    count = 0;
    for(booleanTyped = false; !booleanTyped; count++){
        if (count >9) booleanTyped = true
    }
    if (count != 11) apLogFailInfo("Looped wrong number of times", 11, count, "")




    apInitScenario("for loop with a hard typed Object/boolean")

    count = 0;
    for(booleanObj = true; booleanObj; count++){
        if (count >9) booleanObj = false
    }
    if (count != 11) apLogFailInfo("Looped wrong number of times", 11, count, "")
    
    count = 0;
    for(booleanObj = false; !booleanObj; count++){
        if (count >9) booleanObj = true
    }
    if (count != 11) apLogFailInfo("Looped wrong number of times", 11, count, "")






////////////////////////////////////////////////////////////////////////////////////
    apInitScenario("for loop with a hard typed sbyte")

    count = 0;
    for(sbyteTyped = -119; sbyteTyped; count++){
        if (sbyteTyped > sbyte.MinValue) sbyteTyped--
        else sbyteTyped = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    for(sbyteTyped = 118; sbyteTyped; count++){
        if (sbyteTyped < sbyte.MaxValue) sbyteTyped++
        else sbyteTyped = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("for loop with a hard typed Object/sbyte")
    count = 0;
    
    for(sbyteObj = -119; sbyteObj; count++){
        if (sbyteObj > sbyte.MinValue) sbyteObj--
        else sbyteObj = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    for(sbyteObj = 118; sbyteObj; count++){
        if (sbyteObj < sbyte.MaxValue) sbyteObj++
        else sbyteObj = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



    apInitScenario("for loop with a hard typed byte")
    count = 0;
    for(byteTyped = 246; byteTyped; count++){
        if (byteTyped < byte.MaxValue) byteTyped++
        else byteTyped = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    apInitScenario("for loop with a hard typed Object/byte")
    count = 0;
    for(byteObj = 246; byteObj; count++){
        if (byteObj < byte.MaxValue) byteObj++
        else byteObj = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



////////////////////////////////////////////////////////////////////////////////////
    apInitScenario("for loop with a hard typed Int16")
    count = 0;
    for(int16Typed = -32749; int16Typed; count++){
        if (int16Typed > short.MinValue) int16Typed--
        else int16Typed = 0
    }
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    for(int16Typed = 32748; int16Typed; count++){
        if (int16Typed < short.MaxValue) int16Typed++
        else int16Typed = 0
    }
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("for loop with a hard typed Object/Int16")
    count = 0;
    for(int16Obj = -32749; int16Obj; count++){
        if (int16Obj > short.MinValue) int16Obj--
        else int16Obj = 0
    }
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    for(int16Obj = 32748; int16Obj; count++){
        if (int16Obj < short.MaxValue) int16Obj++
        else int16Obj = 0
    }
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")




    apInitScenario("for loop with a hard typed UInt16")
    count = 0;
    for(uint16Typed = 65516; uint16Typed; count++){
        if (uint16Typed < ushort.MaxValue) uint16Typed++
        else uint16Typed = 0
    }
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("for loop with a hard typed Object/UInt16")
    count = 0;
    for(uint16Obj = 65516; uint16Obj; count++){
        if (uint16Obj < ushort.MaxValue) uint16Obj++
        else uint16Obj = 0
    }
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")


////////////////////////////////////////////////////////////////////////////////////

    apInitScenario("for loop with a hard typed Int32")
    count = 0;
    for(int32Typed = -2147483639; int32Typed; count++){
        if (int32Typed > int.MinValue) int32Typed--
        else int32Typed = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    for(int32Typed = 2147483638; int32Typed; count++){
        if (int32Typed < int.MaxValue) int32Typed++
        else int32Typed = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    apInitScenario("for loop with a hard typed Object/Int32")
    count = 0;
    for(int32Obj = -2147483639; int32Obj; count++){
        if (int32Obj > int.MinValue) int32Obj--
        else int32Obj = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    for(int32Obj = 2147483638; int32Obj; count++){
        if (int32Obj < int.MaxValue) int32Obj++
        else int32Obj = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")




    apInitScenario("for loop with a hard typed UInt32")
    count = 0;
    for(uint32Typed = 4294967286; uint32Typed; count++){
        if (uint32Typed < uint.MaxValue) uint32Typed++
        else uint32Typed = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("for loop with a hard typed Object/UInt32")
    count = 0;
    for(uint32Obj = 4294967286; uint32Obj; count++){
        if (uint32Obj < uint.MaxValue) uint32Obj++
        else uint32Obj = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



////////////////////////////////////////////////////////////////////////////////////

    apInitScenario("for loop with a hard typed Int64")
    count = 0;
    for(int64Typed = -9223372036854775799; int64Typed; count++){
        if (int64Typed > long.MinValue) int64Typed--
        else int64Typed = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    for(int64Typed = 9223372036854775798; int64Typed; count++){
        if (int64Typed < long.MaxValue) int64Typed++
        else int64Typed = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("for loop with a hard typed Object/Int64")
    count = 0;
    for(int64Obj = -9223372036854775799; int64Obj; count++){
        if (int64Obj > long.MinValue) int64Obj--
        else int64Obj = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    for(int64Obj = 9223372036854775798; int64Obj; count++){
        if (int64Obj < long.MaxValue) int64Obj++
        else int64Obj = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 0, count, "")




    apInitScenario("for loop with a hard typed UInt64")
    count = 0;
    for(uint64Typed = 18446744073709551606; uint64Typed; count++){
        if (uint64Typed < ulong.MaxValue) uint64Typed++
        else uint64Typed = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("for loop with a hard typed Object/UInt64")
    count = 0;
    for(uint64Obj = 18446744073709551606; uint64Obj; count++){
        if (uint64Obj < ulong.MaxValue) uint64Obj++
        else uint64Obj = 0
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

////////////////////////////////////////////////////////////////////////////////////
    apInitScenario("for loop with a hard typed single")
    for(count = 0; float.NegativeInfinity; count++){
        if (count == 10) break;
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    for(count = 0; float.PositiveInfinity; count++){
        if (count == 10) break;
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    for(singleTyped = float.MinValue; singleTyped < -float.Epsilon; count++){
        singleTyped /= 10
    }
    if (count != 84) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("for loop with a hard typed Object/single")
    for(count = 0; Object(float.NegativeInfinity); count++){
        if (count == 10) break;
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    for(count = 0; Object(float.PositiveInfinity); count++){
        if (count == 10) break;
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    for(singleObj = float.MinValue; singleObj < -float.Epsilon; count++){
        singleObj /= 10
    }
    if (count != 84) apLogFailInfo("Looped wrong number of times", 10, count, "")


////////////////////////////////////////////////////////////////////////////////////


    apInitScenario("for loop with a hard typed Double")
    for(count = 0; double.NegativeInfinity; count++){
        if (count == 10) break;
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    for(count = 0; double.PositiveInfinity; count++){
        if (count == 10) break;
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    for(doubleTyped = double.MinValue; doubleTyped; count++){
        doubleTyped /= 100
    }
    if (count != 316) apLogFailInfo("Looped wrong number of times", 10, count, "")

                       

    apInitScenario("for loop with a hard typed Object/Double")
    for(count = 0; Object(double.NegativeInfinity); count++){
        if (count == 10) break;
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    for(count = 0; Object(double.PositiveInfinity); count++){
        if (count == 10) break;
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    doubleObj = double.MinValue
    for(count = 0; doubleObj; count++){
        doubleObj /= 100
    }
    if (count != 316) apLogFailInfo("Looped wrong number of times", 10, count, "")


////////////////////////////////////////////////////////////////////////////////////



    apInitScenario("for loop with a hard typed Number")

    count = 0;
    for(numberTyped = double.MinValue; numberTyped; count++){
        numberTyped /= 100
    }
    if (count != 316) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    for(numberTyped = -5; numberTyped; count++){
        numberTyped += .5
        if (numberTyped >10) break
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



    apInitScenario("for loop with a hard typed Object/Number")

    count = 0;
    for(numberObj = double.MinValue; numberObj; count++){
        numberObj /= 100
    }
    if (count != 316) apLogFailInfo("Looped wrong number of times", 10, count, "")


    count = 0;
    for(numberTyped = 5; numberTyped; count++){
        numberTyped -= .5
        if (numberTyped >10) break
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


////////////////////////////////////////////////////////////////////////////////////



    apInitScenario("for loop with a hard typed Decimal")

    count = 0;
    for(decimalTyped = decimal.MinValue; decimalTyped; decimalTyped /= 100){
        count++
    }
    if (count != 29) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    for(decimalTyped = -5; decimalTyped; count++){
        decimalTyped += .5
        if (decimalTyped >10) break
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



    apInitScenario("for loop with a hard typed Object/Decimal")

    count = 0;
    for(decimalObj = decimal.MinValue; decimalObj; decimalObj /= 100){
        count++
    }
    if (count != 29) apLogFailInfo("Looped wrong number of times", 10, count, "")


    count = 0;
    for(numberTyped = 5; numberTyped; count++){
        numberTyped -= .5
        if (numberTyped >10) break
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

////////////////////////////////////////////////////////////////////////////////////

    apInitScenario("for loop with a hard typed String")

    count = 0;
    for(strTyped = null; strTyped; count++){
	if (count>10) break;
    }
    if (count != 0) apLogFailInfo("Looped wrong number of times", 0, count, "")

    count = 0;
    for(strTyped = ""; strTyped; count++){
	if (count>10) break;
    }
    if (count != 0) apLogFailInfo("Looped wrong number of times", 0, count, "")

    count = 0;
    for(strTyped = "-5"; strTyped; count++){
        strTyped++
	if (strTyped == "5") break;
    }
    if (count != 9) apLogFailInfo("Looped wrong number of times", 9, count, "")



    apInitScenario("for loop with a hard typed Object\String")

    count = 0;
    for(strObj = String(null); strObj; count++){
	if (count>9) break;
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    for(strObj = String(""); strObj; count++){
	if (count>10) break;
    }
    if (count != 0) apLogFailInfo("Looped wrong number of times", 0, count, "292755")


////////////////////////////////////////////////////////////////////////////////////





    apInitScenario("for loop with a hard typed char")
    count = 0;
    for(charTyped = 65516; charTyped; count++){
        if (charTyped < char.MaxValue) charTyped++
        else charTyped = 0
    }
    if (count != 20) apLogFailInfo("Looped wrong number of times", 20, count, "")


    apInitScenario("for loop with a hard typed Object/char")
    count = 0;
    for(charObj = 65516; charObj; count++){
        if (charObj < char.MaxValue) charObj++
        else charObj = 0
    }
    if (count != 20) apLogFailInfo("Looped wrong number of times", 20, count, "")


////////////////////////////////////////////////////////////////////////////////////

    apInitScenario("for loop with a hard typed Date")
    count = 0;
    for(DateTyped = new Date(-10); DateTyped.valueOf(); DateTyped = new Date(Number(DateTyped)+1))
        count++
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    count = 0;
    for(DateTyped = new Date(2147483638); DateTyped.valueOf(); count++){
        if (Number(DateTyped)<2147483658) DateTyped = new Date(Number(DateTyped)+1)
        else DateTyped = new Date(0)
    }
    if (count != 21) apLogFailInfo("Looped wrong number of times", 21, count, "")


    apInitScenario("for loop with a hard typed Object/Date")
    count = 0;
    for(DateObj = new Date(-10); DateObj.valueOf(); DateObj = new Date(Number(DateObj)+1)){
        count++
    }
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    count = 0;
    for(DateObj = new Date(4294967286); DateObj.valueOf(); count++){
        if (Number(DateObj)<4294967306) DateObj = new Date(Number(DateObj)+1)
        else DateObj = new Date(0)
    }
    if (count != 21) apLogFailInfo("Looped wrong number of times", 21, count, "")

}
}


function for201() {
    var x = new go

    apInitTest("for201 ");
    x.start()
    apEndTest();


}


for201();


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

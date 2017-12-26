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


var iTestID = 230539;

///////////////////////////////////////////////////////////////////////////////////
//



//         this tescase tests the === operator
// 	this TC is designed to force the algorithm down each branch of:

// If Type(x) is different from Type(y), return false.
// If Type(x) is Undefined, return true.
// If Type(x) is Null, return true.
// If Type(x) is not Number, go to step 11.
// If x is NaN, return false.
// If y is NaN, return false.
// If x is the same number value as y, return true.
// If x is +0 and y is -0, return true.
// If x is -0 and y is +0, return true.
// Return false.
// If Type(x) is String, then return true if x and y are exactly the same sequence of characters (same length and same characters in corresponding positions); otherwise, return false.
// If Type(x) is Boolean, return true if x and y are both true or both false; otherwise, return false.
// Return true if x and y refer to the same object or if they refer to objects joined to each other (section 13.1.2). Otherwise, return false.






function tripeq03() {

  var obj = new Object(1234)
  var nul1 = null
  var nul2 = null
  var nan1 = NaN
  var nan2 : Number = NaN
  var nan3 : Object = new Object(NaN)
  var num1 = 1234
  var num2 : Object = 1234
  var num3 : Object = new Object(1234)
  var num4 : Object = num3
  var zero1 = +0
  var zero2 = -0
  var str1 = "string"
  var str2 = "string" + char(0)
  var bool1 = true
  var bool2 = false


  apInitScenario("Strict equality sanity check")
  if (!(num1 === 1234)) apLogFailInfo("error in strict equality1", false, !(num1 === 1234), "")
  if (num1 !== 1234) apLogFailInfo("error in strict equality1", false, num1 !== 1234, "")

  if (obj === 1234) apLogFailInfo("error in strict equality2", false, obj === 1234, "")
  if (!(obj !== 1234)) apLogFailInfo("error in strict equality2", false, !(obj !== 1234), "")
  

  apInitScenario("Strict equality of undefined")
  if (!(obj.a === obj.b)) apLogFailInfo("error in strict equality3", false, !(obj.a === obj.b), "")
  if (obj.a !== obj.b) apLogFailInfo("error in strict equality3", false, obj.a !== obj.b, "")
  

  apInitScenario("Strict equality of null")
  if (!(nul1 === nul2)) apLogFailInfo("error in strict equality4", false, !(nul1 === nul2), "")
  if (nul1 !== nul2) apLogFailInfo("error in strict equality4", false, nul1 !== nul2, "")

  if (!(nul1 === null)) apLogFailInfo("error in strict equality5", false, !(nul1 === null), "")
  if (nul1 !== null) apLogFailInfo("error in strict equality5", false, nul1 !== null, "")


  apInitScenario("Strict equality of NaN")
  if (nan1 === NaN) apLogFailInfo("error in strict equality6", false, nan1 === NaN, "")
  if (!(nan1 !== NaN)) apLogFailInfo("error in strict equality6", false, !(nan1 !== NaN), "")

  if (nan2 === NaN) apLogFailInfo("error in strict equality7", false, nan2 === NaN, "")
  if (!(nan2 !== NaN)) apLogFailInfo("error in strict equality7", false, !(nan2 !== NaN), "")

  if (nan3 === NaN) apLogFailInfo("error in strict equality8", false, nan3 === NaN, "")
  if (!(nan3 !== NaN)) apLogFailInfo("error in strict equality8", false, !(nan3 !== NaN), "")

  if (1234 === nan1) apLogFailInfo("error in strict equality9", false, 1234 === nan1, "")
  if (!(1234 !== nan1)) apLogFailInfo("error in strict equality9", false, !(1234 !== nan1), "")

  if (1234 === nan2) apLogFailInfo("error in strict equality10", false, 1234 === nan2, "")
  if (!(1234 !== nan2)) apLogFailInfo("error in strict equality10", false, !(1234 !== nan2), "")

  if (1234 === nan2) apLogFailInfo("error in strict equality11", false, 1234 === nan2, "")
  if (!(1234 !== nan2)) apLogFailInfo("error in strict equality11", false, !(1234 !== nan2), "")


  apInitScenario("Strict equality of number with nonzero, finite number")
  if (!(num1 === num2)) apLogFailInfo("error in strict equality12", false, !(num1 === num2), "")
  if (num1 !== num2) apLogFailInfo("error in strict equality12", false, num1 !== num2, "")

  if (num1 === num3) apLogFailInfo("error in strict equality13", false, num1 === num3, "")
  if (!(num1 !== num3)) apLogFailInfo("error in strict equality13", false, !(num1 !== num3), "")

  if (num2 === num3) apLogFailInfo("error in strict equality14", false, num2 === num3, "")
  if (!(num2 !== num3)) apLogFailInfo("error in strict equality14", false, !(num2 !== num3), "")


  apInitScenario("Strict equality of zeros (+/-)")
  if (!(zero1 === zero2)) apLogFailInfo("error in strict equality15", false, !(zero1 === zero2), "")
  if (zero1 !== zero2) apLogFailInfo("error in strict equality15", false, zero1 !== zero2, "")

  if (!(zero2 === zero1)) apLogFailInfo("error in strict equality16", false, !(zero2 === zero1), "")
  if (zero2 !== zero1) apLogFailInfo("error in strict equality16", false, zero2 !== zero1, "")


  apInitScenario("Strict equality of strings")
  if (!(str1 === "string")) apLogFailInfo("error in strict equality17", false, !(str1 === "string"), "")
  if (str1 !== "string") apLogFailInfo("error in strict equality17", false, str1 !== "string", "")

  if (str1 === "String") apLogFailInfo("error in strict equality18", false, str1 === "String", "")
  if (!(str1 !== "String")) apLogFailInfo("error in strict equality18", false, !(str1 !== "String"), "")

  if (str2 === "string") apLogFailInfo("error in strict equality19", false, str2 === "string", "")
  if (!(str2 !== "string")) apLogFailInfo("error in strict equality19", false, !(str2 !== "string"), "")


  apInitScenario("Strict equality of booleans")
  if (!(bool1 === true)) apLogFailInfo("error in strict equality20", false, !(bool1 === true), "")
  if (bool1 !== true) apLogFailInfo("error in strict equality20", false, bool1 !== true, "")

  if (!(bool2 === false)) apLogFailInfo("error in strict equality21", false, !(bool2 === false), "")
  if (bool2 !== false) apLogFailInfo("error in strict equality21", false, bool2 !== false, "")

  if (bool1 === false) apLogFailInfo("error in strict equality22", false, bool1 === false, "")
  if (!(bool1 !== false)) apLogFailInfo("error in strict equality22", false, !(bool1 !== false), "")

  if (bool2 === true) apLogFailInfo("error in strict equality23", false, bool2 === true, "")
  if (!(bool2 !== true)) apLogFailInfo("error in strict equality23", false, !(bool2 !== true), "")


  apInitScenario("Strict equality of objects")
  if (num3 === new Object(1234)) apLogFailInfo("error in strict equality24", false, num3 === new Object(1234), "")
  if (!(num3 !== new Object(1234))) apLogFailInfo("error in strict equality24", false, !(num3 !== new Object(1234)), "")

  if (!(num3 === num4)) apLogFailInfo("error in strict equality25", false, !(num3 === num4), "")
  if (num3 !== num4) apLogFailInfo("error in strict equality25", false, num3 !== num4, "")

  apEndTest();
}



tripeq03();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 0;

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

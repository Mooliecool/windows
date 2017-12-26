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


var iTestID = 228908;

/*



	this TC is meant to increase the code coverage of the logical operator code.
	This TC does many logical operations on JScript types and test the result




*/
///////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function logicalop01() {
  var empty
  var Null = null
  var bool:Boolean
  var Bool:Object
  var num:Number
  var Num:Object
  var str:String
  var Str:Object


  apInitTest("LogicalOP01")



  apInitScenario("logical operations with an empty variable")

  if (!((true && empty) === undefined))  apLogFailInfo("wrong value/type", "", "", "")
  if (!((empty && true) === undefined))  apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || empty) === undefined)) apLogFailInfo("wrong value/type", "", "", "")
  if (!((empty || false) === false))     apLogFailInfo("wrong value/type", "", "", "")



  apInitScenario("logical operations with the Null constant")
  
  if (!((true && Null) === null))  	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Null && true) === null))  	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || Null) === null)) 	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Null || false) === false))     apLogFailInfo("wrong value/type", "", "", "")
  


  apInitScenario("logical operations with a null variable")

  if (!((true && null) === null))  	apLogFailInfo("wrong value/type", "", "", "")
  if (!((null && true) === null))  	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || null) === null)) 	apLogFailInfo("wrong value/type", "", "", "")
  if (!((null || false) === false))     apLogFailInfo("wrong value/type", "", "", "")



  apInitScenario("logical operations with a boolean variable")

  bool = true
  if (!((true && bool) === true))    	apLogFailInfo("wrong value/type", "", "", "")
  if (!((bool && true) === true))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || bool) === true))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((bool || false) === true)) 	apLogFailInfo("wrong value/type", "", "", "")

  bool = false
  if (!((true && bool) === false))    	apLogFailInfo("wrong value/type", "", "", "")
  if (!((bool && true) === false))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || bool) === false))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((bool || false) === false)) 	apLogFailInfo("wrong value/type", "", "", "")




  apInitScenario("logical operations with a String variable")

  str = ""
  if (!((true && str) === ""))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((str && true) === ""))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || str) === ""))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((str || false) === false))	apLogFailInfo("wrong value/type", "", "", "")

  str = undefined
  if (!((true && str) === undefined))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((str && true) === undefined))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || str) === undefined))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((str || false) === false))	apLogFailInfo("wrong value/type", "", "", "")

  str = null
  if (!((true && str) === undefined))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((str && true) === undefined))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || str) === undefined))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((str || false) === false))	apLogFailInfo("wrong value/type", "", "", "")

  str = "foo!"
  if (!((true && str) === "foo!"))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((str && true) === true))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || str) === "foo!"))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((str || false) === "foo!"))	apLogFailInfo("wrong value/type", "", "", "")



  apInitScenario("logical operations with a String Object")

  Str = ""
  if (!((true && Str) === ""))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str && true) === ""))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || Str) === ""))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str || false) === false))	apLogFailInfo("wrong value/type", "", "", "")

  Str = undefined
  if (!((true && Str) === undefined))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str && true) === undefined))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || Str) === undefined))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str || false) === false))	apLogFailInfo("wrong value/type", "", "", "")

  Str = null
  if (!((true && Str) === null))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str && true) === null))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || Str) === null))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str || false) === false))	apLogFailInfo("wrong value/type", "", "", "")

  Str = "foo!"
  if (!((true && Str) === "foo!"))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str && true) === true))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || Str) === "foo!"))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str || false) === "foo!"))	apLogFailInfo("wrong value/type", "", "", "")


  Str = new String("")
  if (!((true && Str) === Str))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str && true) === true))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || Str) === Str))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str || false) === Str))	apLogFailInfo("wrong value/type", "", "", "")

  Str = new String(undefined)
  if (!((true && Str) === Str))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str && true) === true))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || Str) === Str))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str || false) === Str))	apLogFailInfo("wrong value/type", "", "", "")

  Str = new String(null)
  if (!((true && Str) === Str))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str && true) === true))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || Str) === Str))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str || false) === Str))	apLogFailInfo("wrong value/type", "", "", "")

  Str = new String("foo!")
  if (!((true && Str) === Str))		apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str && true) === true))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((false || Str) === Str))	apLogFailInfo("wrong value/type", "", "", "")
  if (!((Str || false) === Str))	apLogFailInfo("wrong value/type", "", "", "")

  apEndTest();

}


logicalop01();


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

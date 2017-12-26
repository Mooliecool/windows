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


var iTestID = 228343;

/*






*/
///////////////////////////////////////////////////////////////////////////////////


@if(!@aspx)
  import System
@end

function populate(invar:Array){
  invar[0] = 2
  invar[1] = 4
  invar[2] = 1
  invar[3] = 3
}


///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function arraywrap01() {

  apInitTest("ArrayWrap01");
  var res

  var nativearray : System.Int32[]
  var secondarray : System.Int32[]
  var JSArray     : Array
  


  apInitScenario("JS properties of a native array")
  nativearray = new System.Int32[4]
  populate(nativearray)
  res = nativearray.length
  if (res != 4) apLogFailInfo("Wrong value returned", 4, res, "")

  

  apInitScenario("JS concat of a native array")

  nativearray = new System.Int32[4]
  secondarray = new System.Int32[4]
  JSArray = new Array()
  populate(nativearray)
  populate(secondarray)
  populate(JSArray)

  res = nativearray.concat(5)
  if (res.toString() != "2,4,1,3,5") apLogFailInfo("Wrong value returned", "2,4,1,3,5", res, "")
  
  res = nativearray.concat(secondarray)
  if (res.toString() != "2,4,1,3,2,4,1,3") apLogFailInfo("Wrong value returned", "2,4,1,3,2,4,1,3", res, "")

  res = nativearray.concat(JSArray)
  if (res.toString() != "2,4,1,3,2,4,1,3") apLogFailInfo("Wrong value returned", "2,4,1,3,2,4,1,3", res, "")
  
  

  apInitScenario("JS join of a native array")

  nativearray = new System.Int32[4]
  populate(nativearray)

  res = nativearray.join(".")
  if (res.toString() != "2.4.1.3") apLogFailInfo("Wrong value returned", "2.4.1.3", res, "")


 
  apInitScenario("JS reverse of a native array")

  nativearray = new System.Int32[4]
  populate(nativearray)

  res = nativearray.reverse()
  if (res.toString() != "3,1,4,2") apLogFailInfo("Wrong value returned", "3,1,4,2", res, "")
  


  apInitScenario("JS slice of a native array")

  nativearray = new System.Int32[4]
  populate(nativearray)

  res = nativearray.slice(2)
  if (res.toString() != "1,3") apLogFailInfo("Wrong value returned", "1,3", res, "")

  res = nativearray.slice(1,-1)
  if (res.toString() != "4,1") apLogFailInfo("Wrong value returned", "4,1", res, "")



  apInitScenario("JS sort of a native array")

  nativearray = new System.Int32[4]
  populate(nativearray)

  res = nativearray.sort()
  if (res.toString() != "1,2,3,4") apLogFailInfo("Wrong value returned", "1,2,3,4", res, "")

  function customSort(a,b){if (a==b) return 0; if (a<b) return 1; else return -1}
  res = nativearray.sort(customSort)
  if (res.toString() != "4,3,2,1") apLogFailInfo("Wrong value returned", "4,3,2,1", res, "")



  apInitScenario("JS splice of a native array")

  nativearray = new System.Int32[4]
  populate(nativearray)

  res = nativearray.splice(1,2,6,8)
  if (res.toString() != "4,1") apLogFailInfo("Wrong value returned", "4,1", res, "")
  if (nativearray.toString() != "2,6,8,3") apLogFailInfo("Wrong value returned", "2,6,8,3", res, "")



  apInitScenario("JS toLocaleString of a native array")

  nativearray = new System.Int32[4]
  populate(nativearray)

  res = nativearray.toLocaleString()
  if (res != "2.00, 4.00, 1.00, 3.00") apLogFailInfo("Wrong value returned", "2.00, 4.00, 1.00, 3.00", res, "")



  apInitScenario("JS valueOf of a native array")

  nativearray = new System.Int32[4]
  populate(nativearray)

  res = nativearray.valueOf()
  if (res != "2,4,1,3") apLogFailInfo("Wrong value returned", "2,4,1,3", res, "")


  apEndTest();

}


arraywrap01();


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

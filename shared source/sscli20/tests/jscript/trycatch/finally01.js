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


var iTestID = 222975;

//////////////////////////////////////////////////////////////////////////////////////
//

//
//
//      purpose checks the different ways of exiting out of a finally block      			
//
//////////////////////////////////////////////////////////////////////////////////////

function finally01() {

  apInitTest("finally01");


  apInitScenario("Enter Finally with break, continue, and throw from try block")


  var i:int
//  var i
  var count

  count = 0
  for (i = 4; i<8; i++){
    try{
      if (i == 6) break
    }catch(e){}
    finally{
      count ++
    }
  }
  if (count != 3) apLogFailInfo("finally block execed wrong number of times", 3, count, "")


  count = 0
  for (i = 4; i<8; i++){
    try{
      if (i == 6) continue
    }catch(e){}
    finally{
      count ++
    }
  }
  if (count != 4) apLogFailInfo("finally block execed wrong number of times", 4, count, "")


  count = 0
  for (i = 4; i<8; i++){
    try{
      if (i == 6) throw("JS is RoXeN your BoXeN")
    }catch(e){ count +=10}
    finally{
      count ++
    }
  }
  if (count != 14) apLogFailInfo("finally block execed wrong number of times", 14, count, "")


  count = 0
  for (i = 4; i<8; i++){
    try{
      if (i == 6) eval("i = 'fail'")
    }catch(e){ count +=10}
    finally{
      count ++
    }
  }
  if (count != 14) apLogFailInfo("finally block execed wrong number of times", 14, count, "")



  /////////////////////////////////////////////////////////////////////////
  apInitScenario("Enter Finally with break, continue, and throw from catch block")

  count = 0
  for (i = 4; i<8; i++){
    try{
      if (i == 6) throw("fail")
      }catch(e){ count +=10;break}
    finally{
      count ++
    }
  }
  if (count != 13) apLogFailInfo("finally block execed wrong number of times", 13, count, "")
  

  count = 0
  for (i = 4; i<8; i++){
    try{
      if (i == 6) throw("fail")
    }catch(e){ count +=10;continue}
    finally{
      count ++
    }
  }
  if (count != 14) apLogFailInfo("finally block execed wrong number of times", 14, count, "")


  try{
    count = 0
    for (i = 4; i<8; i++){
      try{
        if (i == 6) throw("fail")
      }catch(e){ count +=10;throw("fail again")}
      finally{
        count ++
      }
    }
  }catch(e){ if (e != "fail again") apLogFailInfo("throw from catch failed", "fail again", e, "")  }
  if (count != 13) apLogFailInfo("finally block execed wrong number of times", 14, count, "")


  /////////////////////////////////////////////////////////////////////////
  apInitScenario("leave Finally with break, continue, and throw")

  try{

  count = 0
  for (i = 4; i<8; i++){
    try{
      if (i == 6) throw("fail")
    }catch(e){ count +=10;throw(e)}
    finally{
      count ++
      continue
    }
  }
  if (count != 14) apLogFailInfo("finally block execed wrong number of times", 14, count, "")

  count = 0
  for (i = 4; i<8; i++){
    try{
      if (i == 6) throw("fail")
    }catch(e){ count +=10;eval("i='fail'")}
    finally{
      count ++
      continue
    }
  }
  if (count != 14) apLogFailInfo("finally block execed wrong number of times", 14, count, "")


  count = 0
  for (i = 4; i<9; i++){
    try{
      if (i > 5) throw("fail")
    }catch(e){ count +=10;throw(e)}
    finally{
      count ++
      if (i==7) break;
      else continue;
    }
  }
  if (count != 24) apLogFailInfo("finally block execed wrong number of times", 24, count, "")


  count = 0
  for (i = 4; i<9; i++){
    try{
      if (i > 5) throw("fail")
    }catch(e){ count +=10;eval("i='fail'")}
    finally{
      count ++
      if (i==7) break;
      else continue;
    }
  }
  if (count != 24) apLogFailInfo("finally block execed wrong number of times", 24, count, "")


  try{
    count = 0
    for (i = 4; i<8; i++){
      try{
        if (i > 5) throw("fail")
      }catch(e){ count +=10;throw(e)}
      finally{
        count ++
      if (i==7) throw("fail again");
      else continue;
      }
    }
  }catch(e){if (e != "fail again") apLogFailInfo("throw from catch failed", "fail again", e, "")}
  if (count != 24) apLogFailInfo("finally block execed wrong number of times", 24, count, "")


  try{
    count = 0
    for (i = 4; i<8; i++){
      try{
        if (i > 5) throw("fail")
      }catch(e){ count +=10;eval("i='fail'")}
      finally{
        count ++
      if (i==7) throw("fail again");
      else continue;
      }
    }
  }catch(e){if (e != "fail again") apLogFailInfo("throw from catch failed", "fail again", e, "")}
  if (count != 24) apLogFailInfo("finally block execed wrong number of times", 24, count, "")


  }catch(e){apLogFailInfo("unexpected error", "", "", "")}



  count = 0
  for (i = 4; i<8; i++){
    try{
      if (i == 6) throw (0)
    }catch(e){ 
      try{var x; x.foo()}catch(e){continue}finally{count +=10}
    }finally{
      count++
      continue
    }
  }
  if (count != 14) apLogFailInfo("finally block execed wrong number of times", 14, count, "")






  apEndTest()
}


finally01();


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

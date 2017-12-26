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


var iTestID = 51819;

function cc002() {
  apInitTest("cc002");

  // verify variable behavior

  @set @beforeccon = 1

  @cc_on

  var res;
  var exp;

  apInitScenario("1. Verify @set turns cc on");
  res = 0;
  res = @beforeccon;
  if (res != 1) {
    logfailinfo("@set did not turn on cc",1,res,"");
  }

  apInitScenario("2. Verify @set uses floats");
  res = 0;
  @set @fp = 123.456;
  res = @fp;
  if (res != 123.456) {
    logfailinfo("@set did not get float",123.456,res,"");
  }

  apInitScenario("3. Verify @set uses booleans");
  res = "nope";
  @set @bool = false;
  res = @bool;
  if (res != false) {
    logfailinfo("@set did not get boolean",false,res,"");
  }

// -------------- verify comparison operators ----------------

  @set @one = 1;
  @set @two = 2;

  apInitScenario("4. operator: <");
  exp = true;
  @if (@one < @two) res = true; @else res = false; @end
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("5. operator: !=");
  exp = true;
  @if (@one != @two) res = true; @else res = false; @end
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("6. operator: >");
  exp = false;
  @if (@one > @two) res = true; @else res = false; @end
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("7. operator: ==");
  exp = false;
  @if (@one == @two) res = true; @else res = false; @end
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("8. operator: <=");
  exp = true;
  @if (@one <= @two) res = true; @else res = false; @end
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("9. operator: >=");
  exp = false;
  @if (@one >= @two) res = true; @else res = false; @end
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

// ---------------- verify binary operators ------------

  apInitScenario("10. operator: +");
  exp = 3;
  @set @res = (@one + @two)
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("11. operator: -");
  exp = -1;
  @set @res = (@one - @two)
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("12. operator: *");
  exp = 2;
  @set @res = (@one * @two)
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("13. operator: /");
  exp = 1/2;
  @set @res = (@one / @two)
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("14. operator: %");
  exp = 1%2;
  @set @res = (@one % @two)
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("15. operator: >>");
  @set @one = 3
  exp = 3>>2;
  @set @res = (@one >> @two)
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("16. operator: <<");
  exp = 3<<2;
  @set @res = (@one << @two)
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("17. operator: >>>");
  exp = 3>>>2;
  @set @res = (@one >>> @two)
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

// --------------- unary operators -------------------------

  apInitScenario("18. unary operator: +");
  exp = 3;
  @set @res = +@one
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("19. unary operator: -");
  exp = -3;
  @set @res = -@one
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("20. unary operator: !");
  exp = false;
  @set @res = !@one
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }

  apInitScenario("21. unary operator: ~");
  exp = ~3;
  @set @res = ~@one
  res = @res;
  if (res != exp) {
    logfailinfo("operator comparison failed",exp,res,"");
  }



  apEndTest();

}

function logfailinfo(a,b,c,d) {
  if ((b==null) || isNaN(b)) {
    if ((c==null) || isNaN(c)) {
      apLogFailInfo(a,"null","null",d);
    } else {
      apLogFailInfo(a,"null",c,d);
    }
  } else if ((c==null) || isNaN(c)) {
    apLogFailInfo(a,b,"null",d);
  } else {
    apLogFailInfo(a,b,c,d);
  }
}


cc002();


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

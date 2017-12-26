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


var iTestID = 53690;

// test expressions in case and evaluator

function returnval() 
{
   switch( true ) 
   {
   	case false: apLogFailInfo ("wrong case", "true", "false", "");
   	case true: return 7;
   }
}


function switch04()
{
   apInitTest ("switch04 - expressions in case and being evaluated");

// Added to handle strict mode in JScript 7.0
@cc_on 
   @if (@_fast)  
      var a, i;
    
@end       
   
   //------------------------------------- expression in case
   apInitScenario ("1. Expression in case");
   a = 0;
   i = 0;
   switch( 3 ) 
   {
   	case i++: apLogFailInfo ("wrong case", "3", i-1, ""); break;
   	case i++: apLogFailInfo ("wrong case", "3", i-1, ""); break;
   	case i++: apLogFailInfo ("wrong case", "3", i-1, ""); break;
   	case i++: a++; break;
   	case i++: apLogFailInfo ("wrong case", "3", i-1, ""); break;
   	case i++: apLogFailInfo ("wrong case", "3", i-1, ""); break;
   }
   if (a != 1) 
      apLogFailInfo ("wrong count of 'a'", 1, a, "");


   //------------------------------------- expression being evaluated
   apInitScenario ("2. Expression being evaluated");
   a = 0;
   i = 0;
   switch (i++) 
   {
   	case 0: a++; break;
   	case 1: apLogFailInfo ("wrong case", i-1, 1, ""); break;
   	case 2: apLogFailInfo ("wrong case", i-1, 2, ""); break;
   	case 3: apLogFailInfo ("wrong case", i-1, 3, ""); break;
   }
   if (a != 1) 
      apLogFailInfo("wrong count of 'a'", 1, a, "");


   //------------------------------------ eval() being evaluated
   apInitScenario ("3. Eval() being evaluated");
   a = 0;
   switch (eval(38/2)) 
   {
   	case null: apLogFailInfo ("wrong case", "eval(38/2)", "null", "");
   	case 19: a++; break;
   }
   if (a != 1) 
      apLogFailInfo ("wrong count of 'a'", 1, a, "");


   //------------------------------------ return value in case
   apInitScenario ("4. Return value in case");
   a = 0;
   switch (4321) 
   {
      case 4321: a = returnval(); break;
   }
   if (a != 7) 
      apLogFailInfo ("wrong count of 'a'", 7, a, "");


   //------------------------------------ nested switches
   apInitScenario ("5. Nested switches");
   a = 0;
   switch (true) 
   {
   	case false: apLogFailInfo ("wrong case", "true", "false", ""); break;
   	case true: switch (4321) 
   	            {
   					   case 4: apLogFailInfo ("wrong case", 4321, 4, ""); break;
   					   case 4321: switch ("10") 
   					               {
   					   				   case "hello": apLogFailInfo ("wrong case", "'10'", "'hello'", ""); break;
   					   				   case "10": a++; break;
   					               } a++; break;
   	            } a++; break;
   }
   if ( a != 3 ) apLogFailInfo ("wrong count of 'a'", 3, a, "");


   apEndTest();
}



switch04();


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

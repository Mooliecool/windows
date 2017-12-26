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


var iTestID = 52922;

//one scenario
//check to make sure all unary + operations produce a typeof "number"

var m_scen = "";
	
function obFoo() {}

function numberTest(StringToPrint)
{
   /*	this function takes a parameter and tries to push
   it into a number using the unary + operator*/

   var strTmp;
         
   // Added to handle strict mode in JScript 7.0
   @cc_on 
   @if (@_fast)  
      var sCat;                       
    
   @end       

   if ((typeof StringToPrint) == "string")
      strTmp = "'" + StringToPrint + "'";
   else
      strTmp = StringToPrint;

   var stExp = "number";
   var Start = +StringToPrint;
   var stAct = typeof Start;
   		
   sCat = "+" + strTmp;
   if (stAct != stExp)
      apLogFailInfo (m_scen + (sCat.length ? "--" + sCat : "") + " failed", stExp, stAct, "");
}



function uplus001() 
{
   var p_infinity = Number.POSITIVE_INFINITY;
   var n_infinity = Number.NEGATIVE_INFINITY;

   apInitTest("uplus001 ");

   apInitScenario("1. Unary + with integer conversion");
   m_scen = "Unary + with integer conversion";
   //integers
   	
   numberTest(+3);
   numberTest(+0);
   numberTest(3);
   numberTest(0);
   numberTest(-0);
   numberTest(-3);

   apInitScenario("2. Unary + with string integer conversion");
   m_scen = "Unary + with string integer conversion";

   numberTest("+3");
   numberTest("+0");
   numberTest("3");
   numberTest("0");
   numberTest("-0");
   numberTest("-3");

   numberTest("   	 	 	 	 			  +3");
   numberTest("   	 	 	 	 			  +0");
   numberTest("   	 	 	 	 			  3");
   numberTest("   	 	 	 	 			  0");
   numberTest("   	 	 	 	 			  -0");
   numberTest("   	 	 	 	 			  -3");

   numberTest("-000000000000000000000000000000");
   numberTest("-000000000000000000000000000003");
   numberTest("+000000000000000000000000000000");
   numberTest("+000000000000000000000000000003");
   numberTest("000000000000000000000000000000");
   numberTest("000000000000000000000000000003");

   //float
   apInitScenario("3. Unary + with float conversion");
   m_scen = "Unary + with float conversion";

   numberTest(3.727);
   numberTest(.727);
   numberTest(0.727);	
   numberTest(+3.727);
   numberTest(+.727);
   numberTest(+0.727);	
   numberTest(-.727);
   numberTest(-0.727);	
   numberTest(-3.727);	

   apInitScenario("4. Unary + with string float conversion");
   m_scen = "Unary + with string float conversion";

   numberTest("3.727");
   numberTest(".727");
   numberTest("0.727");	
   numberTest("+3.727");
   numberTest("+.727");
   numberTest("+0.727");	
   numberTest("-.727");
   numberTest("-0.727");	
   numberTest("-3.727");

   numberTest("   	 	 	 	 			  3.727");
   numberTest("   	 	 	 	 			  .727");
   numberTest("   	 	 	 	 			  0.727");	
   numberTest("   	 	 	 	 			  +3.727");
   numberTest("   	 	 	 	 			  +.727");
   numberTest("   	 	 	 	 			  +0.727");	
   numberTest("   	 	 	 	 			  -.727");
   numberTest("   	 	 	 	 			  -0.727");	
   numberTest("   	 	 	 	 			  -3.727");


   numberTest("00000000000000000000000000003.727");
   numberTest("00000000000000000000000000000.727");
   numberTest("-0000000000000000000000000000.727");
   numberTest("-0000000000000000000000000003.727");
   numberTest("+0000000000000000000000000000.727");
   numberTest("+0000000000000000000000000003.727");

   //infinities
   apInitScenario("5. Unary + with Infinty conversion");
   m_scen = "Unary + with Infinity conversion";

   numberTest(Number.POSITIVE_INFINITY);
   numberTest(Number.NEGATIVE_INFINITY);

   	
   apInitScenario("6. Unary + with string Infinity conversion");
   m_scen = "Unary + with string Infinity conversion";

   numberTest(String(Number.POSITIVE_INFINITY));
   numberTest(String(Number.NEGATIVE_INFINITY));

   numberTest("   	 	 	 	 "+String(Number.POSITIVE_INFINITY));
   numberTest("   	 	 	 	 "+String(Number.NEGATIVE_INFINITY));
   	
   //NaN

   apInitScenario("7. Unary + with NaN conversion");
   m_scen = "Unary + with NaN conversion";

   numberTest('2n');
   numberTest('2.054r');
   numberTest('-2.054r');
   numberTest('-2n');
   numberTest('new arr');


   numberTest('000000000000000000000002n');
   numberTest('000000000000000000000002.054r');
   numberTest('-00000000000000000000002.054r');
   numberTest('-00000000000000000000002n');
   numberTest('00000000000000000000000new arr');

   numberTest(new Array(1,2,3));

   /*****************************************************************************/

   apEndTest();
}


uplus001();


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

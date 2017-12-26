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


var iTestID = 178205;

/* -------------------------------------------------------------------------
  Test: 	string17
   
  
 
  Component:	JScript
 
  Major Area:	String
 
  Test Area:	String methods using hard-typed variables
 
  Keywords:	
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  charAt

		2.  concat

		3.  indexOf

		4.  lastIndexOf

		5.  match

		6.  replace

		7.  search

		8.  slice

		9.  split

		10. substring

		11. toLowerCase

		12. toUpperCase

		13. length


  Abstract:	 Testing that scope works correctly with hard-typed variables.
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 

 -------------------------------------------------------------------------*/

import System

/*----------
/
/  Helper functions
/
----------*/


function verify(sAct, sExp, sMes,sBug){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp+" ", sAct, sBug);
}



/*----------
/
/  Global variables
/
----------*/



function string17() {

    apInitTest("string17: String tests involving hard-typed variables"); 

	var ts : String = "My dog has fleas.";

	apInitScenario("1.  charAt");
	verify(ts.charAt(3),"d","1.1 Wrong result.","");


	apInitScenario("2.  concat");
	verify(ts.concat("It needs a bath."),"My dog has fleas.It needs a bath.","2.1 Wrong result.","");


	apInitScenario("3.  indexOf");
	ts = "My dog has fleas.";
	verify(ts.indexOf("has"),7,"3.1 Wrong result.","");


	apInitScenario("4.  lastIndexOf");
	ts = "My dog has fleas.";
	verify(ts.lastIndexOf(" "),10,"4.1 Wrong result.","");


	apInitScenario("5.  match");
	ts = "My dog has fleas.";
	var obj5 : Array = ts.match(/ /gi);
	verify(obj5[0]," ","5.1 Wrong result.","");
	verify(obj5[1]," ","5.2 Wrong result.","");
	verify(obj5[2]," ","5.3 Wrong result.","");


	apInitScenario("6.  replace");
	verify(ts.replace(/ /gi, "."),"My.dog.has.fleas.","6.1 Wrong result.","");


	apInitScenario("7.  search");
	ts = "My dog has fleas.";
	verify(ts.search(/\./gi, "."),16,"7.1 Wrong result.","");


	apInitScenario("8.  slice");
	ts = "My dog has fleas.";
	verify(ts.slice(11),"fleas.","8.1 Wrong result.","");


	apInitScenario("9.  split");
	ts = "My dog has fleas.";
	var obj9 : Array = ts.split(" ");
	verify(obj9[0],"My","9.1 Wrong result.","");
	verify(obj9[1],"dog","9.2 Wrong result.","");
	verify(obj9[2],"has","9.3 Wrong result.","");
	verify(obj9[3],"fleas.","9.4 Wrong result.","");


	apInitScenario("10. substring");
	ts = "My dog has fleas.";
	verify(ts.substring(7,10),"has","10.1 Wrong result.","");


	apInitScenario("11. toLowerCase");
	ts = "My dog has fleas.";
	verify(ts.toLowerCase(),"my dog has fleas.","11.1 Wrong result.","");


	apInitScenario("12. toUpperCase");
	ts = "My dog has fleas.";
	verify(ts.toUpperCase(),"MY DOG HAS FLEAS.","12.1 Wrong result.","");


	apInitScenario("13. length");
	ts = "My dog has fleas.";
	verify(ts.length,17,"13.1 Wrong result.","");


	apEndTest();
}


string17();


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

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


var iTestID = 72437;

//////////
//
//  Test Case:
//
//     strlio01: String.prototype.lastIndexOf - second parameter is optional
//
//  Author:
//




//////////
//
//  Helper functions


function verify(sAct, sExp, sMes, sBUG){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}


	// function x and function y are defined here for scenario 20


function x(){ iCount = 3; }


function y() { iCount = -5; }	



//////////
//
// Global variables
//


var str1;
var str2;
var str3;

var iCount; // counting variable



function strlio01() {

    apInitTest("strlio01: String.prototype.lastIndexOf - second parameter is optional"); 


	str1 = "My dog has fleas.";


	apInitScenario("1 Zero arguments");
	verify (str1.lastIndexOf(), -1, "1 Wrong index returned", "");	


	apInitScenario("2 searchString argument, no pos argument");
	verify (str1.lastIndexOf("dog"), 3, "2 Wrong index returned", "");	


	apInitScenario("3 No searchString argument, pos argument");
	verify (str1.lastIndexOf(null,3), -1, "2 Wrong string returned", "");	


	apInitScenario("4 Pos == null");
	verify (str1.lastIndexOf(null,null), -1, "4.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog",null), -1, "4.2 Wrong string returned", "");	


	apInitScenario("5 Pos == undefined");
	verify (str1.lastIndexOf(null,undefined), -1, "5.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog",undefined), 3, "5.2 Wrong string returned", "");	


	apInitScenario("6 Pos == 0");
	verify (str1.lastIndexOf(null,0), -1, "6.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog",0), -1, "6.2 Wrong string returned", "");	


	apInitScenario("7 Pos == 1");
	verify (str1.indexOf(null,1), -1, "7.1 Wrong string returned", "");	
	verify (str1.indexOf("dog",1), 3, "7.2 Wrong string returned", "");	


	apInitScenario("8 Pos == -1");
	verify (str1.lastIndexOf(null,-1), -1, "8.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog",-1), -1, "8.2 Wrong string returned", "");	


	apInitScenario("9 Pos == String.length - 1");
	verify (str1.lastIndexOf(null,str1.length - 1), -1, "9.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog",str1.length - 1), 3, "9.2 Wrong string returned", "");	


	apInitScenario("10 Pos == String.length");
	verify (str1.lastIndexOf(null,str1.length), -1, "10.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog",str1.length), 3, "10.2 Wrong string returned", "");	


	apInitScenario("11 Pos == String.length + 1");
	verify (str1.lastIndexOf(null,str1.length + 1), -1, "11.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog",str1.length + 1), 3, "11.2 Wrong string returned", "");	


	apInitScenario("12 Pos == large positive number");
	verify (str1.lastIndexOf(null,Number.MAX_VALUE), -1, "12.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog",Number.MAX_VALUE), 3, "12.2 Wrong string returned", "VS7 #110980");	


	apInitScenario("13 Pos == large negative number");
	verify (str1.lastIndexOf(null, Number.MIN_VALUE), -1, "13.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog", Number.MIN_VALUE), -1, "13.2 Wrong string returned", "");	


	apInitScenario("14 Pos is float value");
	verify (str1.lastIndexOf(null, 3.14159), -1, "14.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog", 3.14159), 3, "14.2 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog", 4.14159), 3, "14.3 Wrong string returned", "");	


	apInitScenario("15 Pos is empty string");
	verify (str1.lastIndexOf(null, ""), -1, "15.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog", ""), -1, "15.2 Wrong string returned", "");	


	apInitScenario("16 Pos is non-empty string of letters");
	verify (str1.lastIndexOf(null, "abc"), -1, "16.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog", "abc"), 3, "16.2 Wrong string returned", "");	


	apInitScenario("17 Pos is non-empty string of numbers");
	verify (str1.lastIndexOf(null, "300"), -1, "17.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog", "300"), 3, "17.2 Wrong string returned", "");	


	apInitScenario("18 Pos is array");
	var arr1 = new Array(3);
	arr1[0] = 2;
	arr1[1] = 89;
	arr1[2] = -3;
	verify (str1.lastIndexOf(null, arr1), -1, "18.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog", arr1), 3, "18.2 Wrong string returned", "");	


	apInitScenario("19 Pos is object");
	var obj1 = new Object();
	obj1.mem = 3;
	verify (str1.lastIndexOf(null, obj1), -1, "19.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog", obj1), 3, "19.2 Wrong string returned", "");	


	apInitScenario("20 Pos is function");
	// x and y are functions defined above
	verify (str1.lastIndexOf(null, x), -1, "20.1 Wrong string returned", "");	
	verify (str1.lastIndexOf("dog", y), 3, "20.2 Wrong string returned", "");	



       //    expected values were incorrect failing the scenario
	apInitScenario("21 String is very long string, Pos is value near length (performance test)");
	str2 = "";
	for (iCount = 0; iCount < 1000; iCount++) {
		str2 = str2.concat("This is going to be a very long string once this sentence is concatenated 1000 times. ");
	}
	str2 = str2.concat("dog", "dog");
	verify (str2.lastIndexOf(null, str2.length - 100), -1, "21.1 Wrong string returned", "");	
	verify (str2.indexOf("dog", str2.length - 100), 86000, "21.2 Wrong string returned", "");	
	verify (str2.lastIndexOf("dog"), 86003, "21.3 Wrong string returned", "");	

	str3 = "This is going to be a very long string once this sentence is concatenated 1000 times. ";
	str3 = str3.concat("dog","dog");
	//str3 = "dog dog";
	verify (str3.indexOf("dog", str3.length - 30), str3.length - 6, "21.3 error", "");
	verify (str3.lastIndexOf("dog"), str3.length - 3, "21.4 error", "");


	apEndTest();
}



strlio01();


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

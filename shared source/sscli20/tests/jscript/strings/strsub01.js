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


var iTestID = 72449;


@cc_on
//////////
//
//  Test Case:
//
//     strsub01: String.prototype.substring -  Negative parameter values are treated as zero
//
//  Author:
//

//     07/05/2000 - Vernon W. Hui (VernonH) - fixed several testcases that were not compliant with v7.0 functionality
//     3/11/2002 -qiongou added scenarios etc.


//////////
//
//  Helper functions


function verify(sAct, sExp, sMes, SBUG){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}



//////////
//
//  Global variables
//


var str1;
var str2;
var reg1;
var iLargestNumber = Int32.MaxValue;



reg1 = new RegExp();



function strsub01() {


    apInitTest("strsub01: String.prototype.substring -  Negative parameter values are treated as zero"); 


	str1 = new String();
	str1 = "My dog has fleas.";


	apInitScenario("1 Zero arguments");
	str2 = str1.substring();
	verify (str2.length, 17, "1.1 Wrong result returned", "");
	verify (str2, "My dog has fleas.", "1.2 Wrong result returned", "");


	apInitScenario("2 Null start, no end argument");
	str2 = str1.substring(null);
	verify (str2.length, 17, "2.1 Wrong result returned", "");
	verify (str2, "My dog has fleas.", "2.2 Wrong result returned", "");


	apInitScenario("3 Null start, null end argument");
	str2 = str1.substring(null, null);
	verify (str2.length, 0, "3.1 Wrong result returned", "");
	verify (str2, "", "3.2 Wrong result returned", "");


	apInitScenario("4 Start == 0, null end argument");
	str2 = str1.substring(0, null);
	verify (str2.length, 0, "4.1 Wrong result returned", "");
	verify (str2, "", "4.2 Wrong result returned", "");


	apInitScenario("5 Undefined start, no end argument");
	str2 = str1.substring(undefined);
	verify (str2.length, 17, "5.1 Wrong result returned", "");
	verify (str2, "My dog has fleas.", "5.2 Wrong result returned", "");


	apInitScenario("6 Undefined start, null end argument");
	str2 = str1.substring(undefined, null);
	verify (str2.length, 0, "6.1 Wrong result returned", "");
	verify (str2, "", "6.2 Wrong result returned", "");

	apInitScenario("7 Undefined start, undefined end argument");
	str2 = str1.substring(undefined, undefined);
	verify (str2.length, 17, "7.1 Wrong result returned", "VS7 #126497");
	verify (str2, "My dog has fleas.", "7.2 Wrong result returned", "");

	apInitScenario("8 Start == 0, undefined end argument");
	str2 = str1.substring(0, undefined);
	verify (str2.length, 17, "8.1 Wrong result returned", "");
	verify (str2, "My dog has fleas.", "8.2 Wrong result returned", "");


	apInitScenario("9 Start == 0, null end argument");
	str2 = str1.substring(0, null);
	verify (str2.length, 0, "8.1 Wrong result returned", "");
	verify (str2, "", "8.2 Wrong result returned", "");


	apInitScenario("10 Start == 0, no end argument");
	str2 = str1.substring(0);
	verify (str2.length, 17, "10.1 Wrong result returned", "");
	verify (str2, "My dog has fleas.", "10.2 Wrong result returned", "");


	apInitScenario("11 Start == 0, end == 0");
	str2 = str1.substring(0,0);
	verify (str2.length, 0, "11.1 Wrong result returned", "");
	verify (str2, "", "11.2 Wrong result returned", "");


	apInitScenario("12 Positive start value, end == 0");
	str2 = str1.substring(3,0); // args are swapped
	verify (str2.length, 3, "12.1 Wrong result returned", "");
	verify (str2, "My ", "12.2 Wrong result returned", "");


	apInitScenario("13 Positive start value, negative end value");
	str2 = str1.substring(3,-10); // -10 forced to 0, then args are swapped
	verify (str2.length, 3, "13.1 Wrong result returned", "");
	verify (str2, "My ", "13.2 Wrong result returned", "");


	apInitScenario("14 Positive start value, large negative end value");
	str2 = str1.substring(3,-iLargestNumber); // -iLargestNumber forced to 0, then args are swapped
	verify (str2.length, 3, "14.1 Wrong result returned", "");
	verify (str2, "My ", "14.2 Wrong result returned", "");


	apInitScenario("15 Positive start value, large positive end value");
	str2 = str1.substring(3,iLargestNumber);
	verify (str2.length, 14, "15.1 Wrong result returned", "");
	verify (str2, "dog has fleas.", "15.2 Wrong result returned", "");


	apInitScenario("16 Negative start value, end == 0");
	str2 = str1.substring(-10,0);
	verify (str2.length, 0, "16.1 Wrong result returned", "");
	verify (str2, "", "16.2 Wrong result returned", "");


	apInitScenario("17 Negative start value, negative end value");
	str2 = str1.substring(-10,-5);
	verify (str2.length, 0, "17.1 Wrong result returned", "VS7 #111020");
	verify (str2, "", "17.2 Wrong result returned", "");


	apInitScenario("18 Negative start value, large negative end value");
	str2 = str1.substring(-10,-iLargestNumber);
	verify (str2.length, 0, "18.1 Wrong result returned", "");
	verify (str2, "", "18.2 Wrong result returned", "");


	apInitScenario("19 Negative start value, positive end value");
	str2 = str1.substring(-10,10);
	verify (str2.length, 10, "19.1 Wrong result returned", "");
	verify (str2, "My dog has", "19.2 Wrong result returned", "");


	apInitScenario("20 Negative start value, large positive end value");
	str2 = str1.substring(-10,iLargestNumber);
	verify (str2.length, 17, "20.1 Wrong result returned", "");
	verify (str2, "My dog has fleas.", "20.2 Wrong result returned", "");


	apInitScenario("21 Large negative start value, end == 0");
	str2 = str1.substring(-iLargestNumber,0);
	verify (str2.length, 0, "21.1 Wrong result returned", "");
	verify (str2, "", "21.2 Wrong result returned", "");


	apInitScenario("22 Large negative start value, negative end value");
	str2 = str1.substring(-iLargestNumber,-10);
	verify (str2.length, 0, "22.1 Wrong result returned", "");
	verify (str2, "", "22.2 Wrong result returned", "");


	apInitScenario("23 Large negative start value, large negative end value");
	str2 = str1.substring(-iLargestNumber,-iLargestNumber);
	verify (str2.length, 0, "23.1 Wrong result returned", "");
	verify (str2, "", "23.2 Wrong result returned", "");


	apInitScenario("24 Large negative start value, positive end value");
	str2 = str1.substring(-iLargestNumber,10);
	verify (str2.length, 10, "24.1 Wrong result returned", "");
	verify (str2, "My dog has", "24.2 Wrong result returned", "");


	apInitScenario("25 Large negative start value, large positive end value");
	str2 = str1.substring(-iLargestNumber,iLargestNumber);
	verify (str2.length, 17, "25.1 Wrong result returned", "");
	verify (str2, "My dog has fleas.", "25.2 Wrong result returned", "");

	apInitScenario("26 undefined as the end argument");
	str1 = "My dog has fleas.";
	verify (str1.substring(1, undefined), "y dog has fleas.", "26 Wrong result returned", "");

       apInitScenario("27 more than two arguments");
	str1 = "Go Trinity Group";
	verify (str1.substring(4,8,10,11), "rini", "27.1 Wrong result returned", "");
	verify (str1.substring(9,-1,2,3),"Go Trinit","27.2 Wrong result returned", "");

	apInitScenario("28 diff data type as arguments");
	str1 = "Go Trinity Group";
	verify(str1.substring(new String(),10),"Go Trinity","28.1 diff data type as arguments","");
	verify(str1.substring(new Number(1),10),"o Trinity","28.2 diff data type as arguments","");
	verify(str1.substring(10, new Object()),"Go Trinity","28.2 diff data type as arguments","");

	apInitScenario("29 function return data as arguments");
	function t1(x,y){return x+y}
	function t2(x) {return x-1}
	verify(str1.substring(t1(1,2),t2(0)),"Go ","29 function return data as arguments","");

	
	apInitScenario("30 deal with some control chars string");
	var str,str1,strall= "";
	var a = new Array("0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f");
	for (var j=0; j<16; j++)
	for (var i=0; i<16;i++){
		str = "\\"+"u00"+a[j]+a[i];
		eval("str1="+"\'"+str+"\'");
		try {
			strall = str1.concat(strall); 
		}
		catch (e) {
			apLogFailInfo("30 deal with some control chars error in creating string",strall,e.description,"");
		}
	}
	str1 = strall.substring(230,255);
	verify(str1.length,25,"30.1 deal with some control chars string","");
	verify(strall.substring(200,255).length,55,"30.2 deal with some control chars string","");

	apInitScenario("31 deal with some special case string");
	str1 = new String();
	verify(str1.substring(0,2),"","31.1 deal with some special case string","");
	verify("trinity".substring(6,8),"y","31.2 deal with some special case string","");

	str1 = "";
	verify(str1.substring(-2,2),"","31.3 deal with some special case string","");
	
	apEndTest();
}


strsub01();


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

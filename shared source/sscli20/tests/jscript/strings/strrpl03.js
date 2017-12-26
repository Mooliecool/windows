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


var iTestID = 72444;

//////////
//
//  Test Case:
//
//     strrpl03: String.prototype.replace -  There are several meta-characters that can be used in the replacement string, such as $1..$9 and $`
//
//  Author:
//




//////////
//
//  Helper functions


function verify(sAct, sExp, sMes){
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



reg1 = new RegExp();



function strrpl03() {


    apInitTest("strrpl03: String.prototype.replace -  There are several meta-characters that can be used in the replacement string, such as $1..$9 and $`"); 


	str1 = new String();
	str1 = "My dog cat has fleas.";


	apInitScenario("1 replaceValue contains $1");
	reg1 = /(dog)\s(cat)/ig;
	str2 = str1.replace(reg1, "$1");
	verify (str2, "My dog has fleas.", "1 Wrong result returned");


	apInitScenario("2 replaceValue contains $01");
	reg1 = /(dog)\s(cat)/ig;
	str2 = str1.replace(reg1, "$01");
	verify (str2, "My dog has fleas.", "2 Wrong result returned");


	apInitScenario("3 replaceValue contains $2 only");
	reg1 = /(dog)\s(cat)/ig;
	str2 = str1.replace(reg1, "$2");
	verify (str2, "My cat has fleas.", "3 Wrong result returned");


	apInitScenario("4 replaceValue contains $1-$9");
	reg1 = /(d)(o)(g)(\s)(c)(a)(t)(\s)(h)/ig;
	str2 = str1.replace(reg1, "$1$2$3$4$5$6$7$8$9");
	verify (str2, "My dog cat has fleas.", "4 Wrong result returned");


	apInitScenario("5 replaceValue contains some value in $11-$99");
	reg1 = /(d)(o)(g)(\s)(c)(a)(t)(\s)(h)(a)(s)/ig;
	str2 = str1.replace(reg1, "$01$02$03$04$05$06$07$08$09$10$11");
	verify (str2, "My dog cat has fleas.", "5 Wrong result returned");




	apEndTest();
}

strrpl03();


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

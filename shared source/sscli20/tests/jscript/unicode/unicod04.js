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


var iTestID = 77542;

/* -------------------------------------------------------------------------
  Test: 	UNICOD04
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	Unicode white space characters
 
  Keywords:	unicode literal escape eval white space characters
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  Tab (\u0009)

		2.  Linefeed (\u000A)

		3.  Carriage Return (\u000D)

		4.  Space (\u0020)

		5.  Non-breaking space (\u00A0)

		6.  Line Separator (\u2028)

		7.  Paragraph Separator (\u2029)

		8.  Ogham space mark (\u1680)

		9.  En quad (\u2000)

		10. Em quad (\u2001)

		11. En space (\u2002)

		12. Em space (\u2003)

		13. Three-per-em space (\u2004)

		14. Four-per-em space (\u2005)

		15. Six-per-em space (\u2006)

		16. Figure space (\u2007)

		17. Punctuation space (\u2008)

		18. Thin space (\u2009)

		19. Hair space (\u200A)

		20. Zero-width space (\u200B)

		21. Narrow no-break space (\u200F)

		22. Ideographic space (\u3000)


  Abstract:	 Unicode white space characters are legal in JScript code.  Like
			 other whitespace characters, they have no significance in and of
			 themselves except to separate identifiers and keywords.
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 

 -------------------------------------------------------------------------*/



//////////
//
//  Helper functions


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp+" ", sAct, "");
}



//////////
//
// Global variables




function unicod04() {

    apInitTest("unicod04: Unicode compliance -- Unicode white space characters"); 

	var iResult;

	apInitScenario("1.  Tab");
	iResult = undefined;
	iResult = "5\u00093";
	verify (typeof(iResult), "string", "1.1 Wrong data type");
	verify (iResult.length, 3, "1.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x0009, "1.3 Wrong character code returned");
	verify (iResult, "5	3", "1.4 Wrong value returned");
	iResult = parseInt("5\u00093");
	verify (typeof(iResult), "number", "1.5 Wrong data type");
	verify (iResult, 5, "1.6 Wrong string length");
	iResult = eval("5 +\u00093");
	verify (typeof(iResult), "number", "1.7 Wrong data type");
	verify (iResult, 8, "1.8 Wrong string length");


	apInitScenario("2.  Linefeed");
	iResult = undefined;
	iResult = "5\u000A3";
	verify (typeof(iResult), "string", "2.1 Wrong data type");
	verify (iResult.length, 3, "2.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x000A, "2.3 Wrong character code returned");
	verify (escape(iResult), "5%0A3", "2.4 Wrong value returned");
	iResult = parseInt("5\u000A3");
	verify (typeof(iResult), "number", "2.5 Wrong data type");
	verify (iResult, 5, "2.6 Wrong string length");
	iResult = eval("5 +\u000A3");
	verify (typeof(iResult), "number", "2.7 Wrong data type");
	verify (iResult, 8, "2.8 Wrong string length");


	apInitScenario("3.  Carriage Return");
	iResult = undefined;
	iResult = "5\u000D3";
	verify (typeof(iResult), "string", "3.1 Wrong data type");
	verify (iResult.length, 3, "3.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x000D, "3.3 Wrong character code returned");
	verify (escape(iResult), "5%0D3", "3.4 Wrong value returned");
	iResult = parseInt("5\u000D3");
	verify (typeof(iResult), "number", "3.5 Wrong data type");
	verify (iResult, 5, "3.6 Wrong string length");
	iResult = eval("5 +\u000D3");
	verify (typeof(iResult), "number", "3.7 Wrong data type");
	verify (iResult, 8, "3.8 Wrong string length");


	apInitScenario("4.  Space");
	iResult = undefined;
	iResult = "5\u00203";
	verify (typeof(iResult), "string", "4.1 Wrong data type");
	verify (iResult.length, 3, "4.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x0020, "4.3 Wrong character code returned");
	verify (escape(iResult), "5%203", "4.4 Wrong value returned");
	iResult = parseInt("5\u00203");
	verify (typeof(iResult), "number", "4.5 Wrong data type");
	verify (iResult, 5, "4.6 Wrong string length");
	iResult = eval("5 +\u00203");
	verify (typeof(iResult), "number", "4.7 Wrong data type");
	verify (iResult, 8, "4.8 Wrong string length");


	apInitScenario("5.  Non-breaking space");
	iResult = undefined;
	iResult = "5\u00A03";
	verify (typeof(iResult), "string", "5.1 Wrong data type");
	verify (iResult.length, 3, "5.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x00A0, "5.3 Wrong character code returned");
	verify (escape(iResult), "5%A03", "5.4 Wrong value returned");
	iResult = parseInt("5\u00A03");
	verify (typeof(iResult), "number", "5.5 Wrong data type");
	verify (iResult, 5, "5.6 Wrong string length");
	iResult = eval("5 +\u00A03");
	verify (typeof(iResult), "number", "5.7 Wrong data type");
	verify (iResult, 8, "5.8 Wrong string length");


	apInitScenario("6.  Line Separator");
	iResult = undefined;
	iResult = "5\u20283";
	verify (typeof(iResult), "string", "6.1 Wrong data type");
	verify (iResult.length, 3, "6.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2028, "6.3 Wrong character code returned");
	verify (escape(iResult), "5%u20283", "6.4 Wrong value returned");
	iResult = parseInt("5\u20283");
	verify (typeof(iResult), "number", "6.5 Wrong data type");
	verify (iResult, 5, "6.6 Wrong string length");
	iResult = eval("5 +\u20283");
	verify (typeof(iResult), "number", "6.7 Wrong data type");
	verify (iResult, 8, "6.8 Wrong string length");


	apInitScenario("7.  Paragraph Separator");
	iResult = undefined;
	iResult = "5\u20293";
	verify (typeof(iResult), "string", "7.1 Wrong data type");
	verify (iResult.length, 3, "7.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2029, "7.3 Wrong character code returned");
	verify (escape(iResult), "5%u20293", "7.4 Wrong value returned");
	iResult = parseInt("5\u20293");
	verify (typeof(iResult), "number", "7.5 Wrong data type");
	verify (iResult, 5, "7.6 Wrong string length");
	iResult = eval("5 +\u20293");
	verify (typeof(iResult), "number", "7.7 Wrong data type");
	verify (iResult, 8, "7.8 Wrong string length");


	apInitScenario("8.  Ogham space mark");
	iResult = undefined;
	iResult = "5\u16803";
	verify (typeof(iResult), "string", "8.1 Wrong data type");
	verify (iResult.length, 3, "8.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x1680, "8.3 Wrong character code returned");
	verify (escape(iResult), "5%u16803", "8.4 Wrong value returned");
	iResult = parseInt("5\u16803");
	verify (typeof(iResult), "number", "8.5 Wrong data type");
	verify (iResult, 5, "8.6 Wrong string length");


	apInitScenario("9.  En quad");
	iResult = undefined;
	iResult = "5\u20003";
	verify (typeof(iResult), "string", "9.1 Wrong data type");
	verify (iResult.length, 3, "9.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2000, "9.3 Wrong character code returned");
	verify (escape(iResult), "5%u20003", "9.4 Wrong value returned");
	iResult = parseInt("5\u20003");
	verify (typeof(iResult), "number", "9.5 Wrong data type");
	verify (iResult, 5, "9.6 Wrong string length");
	iResult = eval("5 +\u20003");
	verify (typeof(iResult), "number", "9.7 Wrong data type");
	verify (iResult, 8, "9.8 Wrong string length");


	apInitScenario("10. Em quad");
	iResult = undefined;
	iResult = "5\u20013";
	verify (typeof(iResult), "string", "10.1 Wrong data type");
	verify (iResult.length, 3, "10.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2001, "10.3 Wrong character code returned");
	verify (escape(iResult), "5%u20013", "10.4 Wrong value returned");
	iResult = parseInt("5\u20013");
	verify (typeof(iResult), "number", "10.5 Wrong data type");
	verify (iResult, 5, "10.6 Wrong string length");
	iResult = eval("5 +\u20013");
	verify (typeof(iResult), "number", "10.7 Wrong data type");
	verify (iResult, 8, "10.8 Wrong string length");


	apInitScenario("11. En space");
	iResult = undefined;
	iResult = "5\u20023";
	verify (typeof(iResult), "string", "11.1 Wrong data type");
	verify (iResult.length, 3, "11.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2002, "11.3 Wrong character code returned");
	verify (escape(iResult), "5%u20023", "11.4 Wrong value returned");
	iResult = parseInt("5\u20023");
	verify (typeof(iResult), "number", "11.5 Wrong data type");
	verify (iResult, 5, "11.6 Wrong string length");
	iResult = eval("5 +\u20023");
	verify (typeof(iResult), "number", "11.7 Wrong data type");
	verify (iResult, 8, "11.8 Wrong string length");


	apInitScenario("12. Em space");
	iResult = undefined;
	iResult = "5\u20033";
	verify (typeof(iResult), "string", "12.1 Wrong data type");
	verify (iResult.length, 3, "12.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2003, "12.3 Wrong character code returned");
	verify (escape(iResult), "5%u20033", "12.4 Wrong value returned");
	iResult = parseInt("5\u20033");
	verify (typeof(iResult), "number", "12.5 Wrong data type");
	verify (iResult, 5, "12.6 Wrong string length");
	iResult = eval("5 +\u20033");
	verify (typeof(iResult), "number", "12.7 Wrong data type");
	verify (iResult, 8, "12.8 Wrong string length");


	apInitScenario("13. Three-per-em space");
	iResult = undefined;
	iResult = "5\u20043";
	verify (typeof(iResult), "string", "13.1 Wrong data type");
	verify (iResult.length, 3, "13.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2004, "13.3 Wrong character code returned");
	verify (escape(iResult), "5%u20043", "13.4 Wrong value returned");
	iResult = parseInt("5\u20043");
	verify (typeof(iResult), "number", "13.5 Wrong data type");
	verify (iResult, 5, "13.6 Wrong string length");
	iResult = eval("5 +\u20043");
	verify (typeof(iResult), "number", "13.7 Wrong data type");
	verify (iResult, 8, "13.8 Wrong string length");


	apInitScenario("14. Four-per-em space");
	iResult = undefined;
	iResult = "5\u20053";
	verify (typeof(iResult), "string", "14.1 Wrong data type");
	verify (iResult.length, 3, "14.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2005, "14.3 Wrong character code returned");
	verify (escape(iResult), "5%u20053", "14.4 Wrong value returned");
	iResult = parseInt("5\u20053");
	verify (typeof(iResult), "number", "14.5 Wrong data type");
	verify (iResult, 5, "14.6 Wrong string length");
	iResult = eval("5 +\u20053");
	verify (typeof(iResult), "number", "14.7 Wrong data type");
	verify (iResult, 8, "14.8 Wrong string length");


	apInitScenario("15. Six-per-em space");
	iResult = undefined;
	iResult = "5\u20063";
	verify (typeof(iResult), "string", "15.1 Wrong data type");
	verify (iResult.length, 3, "15.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2006, "15.3 Wrong character code returned");
	verify (escape(iResult), "5%u20063", "15.4 Wrong value returned");
	iResult = parseInt("5\u20063");
	verify (typeof(iResult), "number", "15.5 Wrong data type");
	verify (iResult, 5, "15.6 Wrong string length");
	iResult = eval("5 +\u20063");
	verify (typeof(iResult), "number", "15.7 Wrong data type");
	verify (iResult, 8, "15.8 Wrong string length");


	apInitScenario("16. Figure space");

/*	iResult = undefined;
	iResult = "5\u20073";
	verify (typeof(iResult), "string", "16.1 Wrong data type");
	verify (iResult.length, 3, "16.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2007, "16.3 Wrong character code returned");
	verify (escape(iResult), "5%u20073", "16.4 Wrong value returned");
	iResult = parseInt("5\u20073");
	verify (typeof(iResult), "number", "16.5 Wrong data type");
	verify (iResult, 5, "16.6 Wrong string length");
	try {
		iResult = eval("5 +\u20073");
	}
	catch (e) {
		apLogFailInfo("*** Error: 16. Eval of escape char failed","","","");
	}
	verify (typeof(iResult), "number", "16.7 Wrong data type");
	verify (iResult, 8, "16.8 Wrong string length");
*/

	apInitScenario("17. Punctuation space");

/*	iResult = undefined;
	iResult = "5\u20083";
	verify (typeof(iResult), "string", "17.1 Wrong data type");
	verify (iResult.length, 3, "17.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2008, "17.3 Wrong character code returned");
	verify (escape(iResult), "5%u20083", "17.4 Wrong value returned");
	iResult = parseInt("5\u20083");
	verify (typeof(iResult), "number", "17.5 Wrong data type");
	verify (iResult, 5, "17.6 Wrong string length");
	try {
		iResult = eval("5 +\u20083");
	}
	catch (e) {
		apLogFailInfo("*** Error: 17. Eval of escape char failed","","","");
	}
	verify (typeof(iResult), "number", "17.7 Wrong data type");
	verify (iResult, 8, "17.8 Wrong string length");
*/

	apInitScenario("18. Thin space");

/*	iResult = undefined;
	iResult = "5\u20093";
	verify (typeof(iResult), "string", "18.1 Wrong data type");
	verify (iResult.length, 3, "18.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x2009, "18.3 Wrong character code returned");
	verify (escape(iResult), "5%u20093", "18.4 Wrong value returned");
	iResult = parseInt("5\u20093");
	verify (typeof(iResult), "number", "18.5 Wrong data type");
	verify (iResult, 5, "18.6 Wrong string length");
	try {
		iResult = eval("5 +\u20093");
	}
	catch (e) {
		apLogFailInfo("*** Error: 18. Eval of escape char failed","","","");
	}
	verify (typeof(iResult), "number", "18.7 Wrong data type");
	verify (iResult, 8, "18.8 Wrong string length");
*/

	apInitScenario("19. Hair space");

/*	iResult = undefined;
	iResult = "5\u200A3";
	verify (typeof(iResult), "string", "19.1 Wrong data type");
	verify (iResult.length, 3, "19.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x200A, "19.3 Wrong character code returned");
	verify (escape(iResult), "5%u200A3", "19.4 Wrong value returned");
	iResult = parseInt("5\u200A3");
	verify (typeof(iResult), "number", "19.5 Wrong data type");
	verify (iResult, 5, "19.6 Wrong string length");
	try {
		iResult = eval("5 +\u200A3");
	}
	catch (e) {
		apLogFailInfo("*** Error: 19. Eval of escape char failed","","","");
	}
	verify (typeof(iResult), "number", "19.7 Wrong data type");
	verify (iResult, 8, "19.8 Wrong string length");
*/

	apInitScenario("20. Zero-width space");

/*	iResult = undefined;
	iResult = "5\u200B3";
	verify (typeof(iResult), "string", "20.1 Wrong data type");
	verify (iResult.length, 3, "20.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x200B, "20.3 Wrong character code returned");
	verify (escape(iResult), "5%u200B3", "20.4 Wrong value returned");
	iResult = parseInt("5\u200B3");
	verify (typeof(iResult), "number", "20.5 Wrong data type");
	verify (iResult, 5, "20.6 Wrong string length");
	try {
		iResult = eval("5 +\u200B3");
	}
	catch (e) {
		apLogFailInfo("*** Error: 20. Eval of escape char failed","","","");
	}
	verify (typeof(iResult), "number", "20.7 Wrong data type");
	verify (iResult, 8, "20.8 Wrong string length");
*/

	apInitScenario("21. Narrow no-break space");
	iResult = undefined;
	iResult = "5\u200F3";
	verify (typeof(iResult), "string", "21.1 Wrong data type");
	verify (iResult.length, 3, "21.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x200F, "21.3 Wrong character code returned");
	verify (escape(iResult), "5%u200F3", "21.4 Wrong value returned");
	iResult = parseInt("5\u200F3");
	verify (typeof(iResult), "number", "21.5 Wrong data type");
	verify (iResult, 5, "21.6 Wrong value");


	apInitScenario("22. Ideographic space");
	iResult = undefined;
	iResult = "5\u30003";
	verify (typeof(iResult), "string", "22.1 Wrong data type");
	verify (iResult.length, 3, "22.2 Wrong string length");
	verify (iResult.charCodeAt(1), 0x3000, "22.3 Wrong character code returned");
	verify (escape(iResult), "5%u30003", "22.4 Wrong value returned");
	iResult = parseInt("5\u30003");
	verify (typeof(iResult), "number", "22.5 Wrong data type");
	verify (iResult, 5, "22.6 Wrong string length");
	try {
		iResult = eval("5 +\u30003");
	}
	catch (e) {
		apLogFailInfo("*** Error: 22. Eval of escape char failed","","","");
	}
	verify (typeof(iResult), "number", "22.7 Wrong data type");
	verify (iResult, 8, "22.8 Wrong string length");


	apEndTest();
}

unicod04();


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

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


var iTestID = 72445;

//////////
//
//  Test Case:
//
//     strslc01: String.prototype.slice -  first parameter, second parameter, or both can be negative
//
//  Author:
//

//		Added scenarios from 20 -qiongou 3/19/02



//////////
//
//  Helper functions


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}


function getUnicodeRangeStr() 
{
	var str,strall="";
	var str1 = "";
    var str2 = "";
	var a = new Array("0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f");
	for (var l=0; l<16; l++)
	for (var k=0; k<16; k++)
	for (var j=0; j<16; j++)
	for (var i=0; i<16;i++){
		str = "\\"+"u"+a[l]+a[k]+a[j]+a[i];
		eval("str2="+"\'"+str+"\'");
		try {
			//strall = str2.concat(strall); 
			strall = strall.concat(str2);
		}
		catch (e) {
			apLogFailInfo("getUnicodeRangeStr error occured",strall,e.description,"");
		}
	}
//	print(strall.length);
	if (strall.length != 65536){
		apLogFailInfo("getUnicodeRangeStr error occured lenth is incorrect",strall.length,65536,"");
		return null;
	}
	else
	{	
//		print(strall.length);
		return strall;
	}

	
}

function getLongStr(str,num) 
{
	var strall = "";
	if (num <=0)
		return null;
	else 
	{
		for (var i=0; i<num;i++)
			strall += str;
		return strall;
	}		
}
//////////
//
//  Global variables
//


var str1;
var str2;
var reg1;




reg1 = new RegExp();



function strslc01() {


    apInitTest("strslc01: String.prototype.slice -  first parameter, second parameter, or both can be negative"); 


	str1 = new String();
	str1 = "My dog has fleas.";


	apInitScenario("1 Neg start, no end arg");
	str2 = str1.slice(-3);
	verify (str2, "as.", "1 Wrong result returned");


	apInitScenario("2 Neg start, pos end, (start + length < end)");
	str2 = str1.slice(-13, 13);
	verify (str2, "og has fl", "2 Wrong result returned");


	apInitScenario("3 Neg start, pos end, (start + length = end)");
	str2 = str1.slice(-10, -10 + str1.length);
	verify (str2, "", "3 Wrong result returned");


	apInitScenario("4 Neg start, pos end, (start + length > end)");
	str2 = str1.slice(-10, 15);
	verify (str2, "has flea", "4 Wrong result returned");


	apInitScenario("5 Neg start, pos end > length");
	str2 = str1.slice(-10, 25);
	verify (str2, "has fleas.", "5 Wrong result returned");


	apInitScenario("6 Neg start, neg end, (start + length < end + length)");
	str2 = str1.slice(-10, -2);
	verify (str2, "has flea", "6 Wrong result returned");


	apInitScenario("7 Neg start, neg end, (start + length = end + length)");
	str2 = str1.slice(-10, -10);
	verify (str2, "", "7 Wrong result returned");


	apInitScenario("8 Neg start, neg end, (start + length > end + length)");
	str2 = str1.slice(-10, -12);
	verify (str2, "", "8 Wrong result returned");


	apInitScenario("9 Pos start, neg end, (start < end + length)");
	str2 = str1.slice(11, -2);
	verify (str2, "flea", "9 Wrong result returned");


	apInitScenario("10 Pos start, neg end, (start = end + length)");
	str2 = str1.slice(15, -2);
	verify (str2, "", "10 Wrong result returned");


	apInitScenario("11 Pos start, neg end, (start > end + length)");
	str2 = str1.slice(15, -5);
	verify (str2, "", "11 Wrong result returned");


	apInitScenario("12 start = 0, no end arg");
	str2 = str1.slice(0);
	verify (str2, "My dog has fleas.", "12 Wrong result returned");


	apInitScenario("13 start = 0, neg end");
	str2 = str1.slice(0, -2);
	verify (str2, "My dog has flea", "13 Wrong result returned");


	apInitScenario("14 Neg start, end = 0");
	str2 = str1.slice(-2, 0);
	verify (str2, "", "14 Wrong result returned");


	apInitScenario("15 Neg start with large negative value, pos end");
	str2 = str1.slice(Number.MIN_VALUE, 10);
	verify (str2, "My dog has", "15 Wrong result returned");


	apInitScenario("16 Pos start, neg end with large negative value");
	str2 = str1.slice(10, Number.MIN_VALUE);
	verify (str2, "", "16 Wrong result returned");


	apInitScenario("17 Pos start, pos end > length");
	str2 = str1.slice(10, Number.MAX_VALUE);
	verify (str2, " fleas.", "17 Wrong result returned", "VS7 #111007");


	apInitScenario("18 Pos start > length, pos end");
	str2 = str1.slice(Number.MAX_VALUE, 10);
	verify (str2, "", "18 Wrong result returned");


	apInitScenario("19 Pos start > length, pos end > length");
	str2 = str1.slice(Number.MAX_VALUE - 100, Number.MAX_VALUE);
	verify (str2, "", "19 Wrong result returned");

       apInitScenario("20 non-number start, no end");	
	str2 = str1.slice("fdafd");
	verify( str2,str1,"20  non-number start, no end");

	apInitScenario("21 non-number end, nomal start");
	str2 = str1.slice(1,"fdsafds");
	verify( str2,"","21. ");

	apInitScenario("22 null start, nomal end");
	str2 = str1.slice(null, str1.length);
	verify(str2,str1,"22. ");
	
	apInitScenario("23 nomal start, null end");
	str2 = str1.slice(1, null);
	verify(str2,"","23. ");

	apInitScenario("24 Object start, nomal end");
	str2 = str1.slice(new Object(),3);
	verify(str2, "My ","24. ");

	apInitScenario("25 undefined start, nomal end");
	str2 = str1.slice(undefined,3);
	verify(str2, "My ","25. ");

	apInitScenario("26 3 start, undefined end");
	str2 = str1.slice(3,undefined);
	verify(str2, "dog has fleas.","26. ");

	apInitScenario("27 from Unicode range chars");
	str1 = getUnicodeRangeStr() ;
	if (str1 != null) 
	{
		str2 = str1.slice(0,str1.length);
		verify(str2, str1,"27. ");
	}

	apInitScenario("28 from long string");
	str1 = getLongStr(" ",120000);
	if (str1 != null)
	{
		str2 = str1.slice(119999,120000);
		verify(str2, " ", "28. ");
	}
	apEndTest();
}

strslc01();


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

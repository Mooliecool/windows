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


var iTestID = 236479;

//
//     strcon02sec: String.prototype.concat with certain situations.
//
//  


var str1;
var str2;
var str3;

function strcon02sec() {

    apInitTest("strcon02: String.prototype.concat"); 


	str1 = "security";
	str2 = " test";
	

	apInitScenario("0. concat with normal string");
	str3 = str1.concat(str2);
	if (str3 != "security test")
		apLogFailInfo("0. concat with normal string","security test", str3, "");
		
 	apInitScenario("1. concat with null");
	str3 = str1.concat(null);
	if (str3 != "securitynull")
		apLogFailInfo("1. concat with null","securitynull", str3, "");

	apInitScenario("2. concat with undefined");
	str2 = undefined;
	str3 = str1.concat(str2);
	if ( str3 != "securityundefined")
		apLogFailInfo("2. concat with undefined","securityundefined", str3, "");
	
	
	apInitScenario("3. concat with \u0000");
	str2 = "\u0000";
	str3 = str1.concat(str2);
	if ( str3 != "security\u0000")
		apLogFailInfo("3. concat with \u0000","security\u0000", str3, "");

	apInitScenario("4. concat with NaN");
	str2 = NaN;
	str3 = str1.concat(str2);
	if (str3 != "securityNaN")
		apLogFailInfo("4. concat with NaN","securityNaN", str3, "");
	
	apInitScenario("5. concat with empty argument");
	str3 = str1.concat();
	if ( str3 != "security")
		apLogFailInfo("5. concat with empty","security", str3, "");

	apInitScenario("6. '\u0000' concat with others");
	str2 = "\u0000";
	str3 = str2.concat("abcdefg");
	if (str3.length != 8)
		apLogFailInfo("6. \u0000 concat with others","length = 8", str3.length, "");
	
	apInitScenario("7. string literal concat with literal");
	str3 = "abcd".concat("efg");
	if ( str3 != "abcdefg")
		apLogFailInfo("7. string literal concat with literal","abcdefg", str3, "");

	apInitScenario("8. concat with each & all characters in unicode range ");
	var str,strall="";
	str1 = "";
       str2 = "";
	var a = new Array("0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f");
	for (var l=0; l<16; l++)
	for (var k=0; k<16; k++)
	for (var j=0; j<16; j++)
	for (var i=0; i<16;i++){
		str = "\\"+"u"+a[l]+a[k]+a[j]+a[i];
		eval("str2="+"\'"+str+"\'");
		try {
			str3 = str1.concat(str2); 
			strall = str2.concat(strall); 
			//strall = strall.concat(str2);
		}
		catch (e) {
			apLogFailInfo("8. concat with full unicode range-error occured",strall,e.description,"");
		}
	}
	if (strall.length != 65536)
		apLogFailInfo("8. concat into full unicode range string failed.",strall.length,65536,"");
	
	
	apInitScenario("9. concat with large number of arguments with number contents ");
	var my_data = new Array();
	str = "";
	str2 = "";
	str3 = "";
	for (i=0;i <1000; i++){
		my_data[i] = i;
		//str = str+"my_data["+i.toString()+"]"+",";
		str = str + "my_data["+i+"]"+",";
	}	
	str = str+my_data[0];
	try {
		eval("str3= str2.concat("+str+");");
	}
	catch (e) {
		apLogFailInfo("9. concat with large number of arguments-error occured","",e.description,"");
	}
	

	apInitScenario("10. concat with same string multiple times as multiple arguments");
	var strtmp="";
	var strone = "1";
	str2 = "";
	str3 = "";
	for (i=0; i<10000; i++)
		strtmp = "strone,".concat(strtmp); 
	strtmp += "strone";
	try {
		eval("str3=str2.concat("+strtmp+");");
	}
	catch (e) 
	{
		apLogFailInfo("10.concat with same string multiple times as multiple arguments","",e.description,"");
	}
	
	
	apInitScenario("11. concat with Unicode range string multiple times as multiple arguments");
	strtmp="";
	str2 = "";
	str3 = "";
	for (i=0; i<20; i++)
		strtmp = "strall,".concat(strtmp); 
	strtmp += "strall";
	
	try {
		eval("str3=str2.concat("+strtmp+");");
	}
	catch (e) 
	{
		apLogFailInfo("11.concat with all unicode chars as multiple arguments","",e.description,"");
	}
	//print(str3.length);
	

	apInitScenario("12. concat with different data types / objects");
	var stra = "a";
	try {
		stra =stra.concat(new Date(2002,1,1));
	}
	catch (e)
	{
		apLogFailInfo("12.1 concat with Date type","no error",e.description,"");	
	}
	if (stra != "aFri Feb 1 00:00:00 PST 2002")
		apLogFailInfo("12.1 concat with Date type","aFri Feb 1 00:00:00 PST 2002",stra,"");
	stra = "A";
	try {
		stra = stra.concat(Math.PI);
	}
	catch (e)
	{
		apLogFailInfo("12.2 concat with Math type","no error",e.number,"");	
	}
	if (stra != "A3.141592653589793")
		apLogFailInfo("12.2 concat with Math type","A3.141592653589793",stra,"");	

	stra = "a";
	try {
		stra = stra.concat(new Number(1));
	}
	catch (e)
	{
		apLogFailInfo("12.3 concat with Number type","no error",e.descriptioin,"");	
	}
	if (stra != "a1")
		apLogFailInfo("12.3 concat with Number type","a1",stra,"");	

	stra = "a";
	try {
		stra = stra.concat(new String("bc"));
	}
	catch (e)
	{
		apLogFailInfo("12.4 concat with String object","no error",e.description,"");	
	}
	if (stra != "abc")
		apLogFailInfo("12.4 concat with String object","abc",stra,"");	

	stra = "a";
	try {
		stra = stra.concat([1,2,3]);
	}
	catch (e)
	{
		apLogFailInfo("12.5 concat with Array type","no error",e.description,"");	
	}
	if (stra != "a1,2,3")
		apLogFailInfo("12.5 concat with Array type","a1,2,3",stra,"");	
	stra = "a";
	try {
		stra = stra.concat(new RegExp("d(b+)(d)","ig"));
	}
	catch (e)
	{
		apLogFailInfo("12.6 concat with RegExp object","no error",e.description,"");	
	}
	if (stra != "a/d(b+)(d)/ig")
		apLogFailInfo("12.6 concat with RegExp object","a/d(b+)(d)/ig",stra,"");
	
	stra = "";
	

	apInitScenario("13. concat with function/Function");
	try {
		stra = stra.concat(new Function("x","y","return (x+y)"));
	}
	catch (e)
	{
		apLogFailInfo("13.1 concat with Function","no error",e.description,"");	
	}
	function test(x,y){
		return x+y;
	}

	stra = "";
	try {
		stra = stra.concat(test("a","b"));
	}
	catch (e)
	{
		apLogFailInfo("13.2 concat with function","no error",e.description,"");	
	}
	if (stra != "ab")
		apLogFailInfo("12.2 concat with function","ab",stra,"");	

	apInitScenario("14. with eval()");
	stra = "";
	stra = stra.concat(eval("stra.concat(1)"));
	if (stra != "1")
		apLogFailInfo("14. with eval()","1",stra,"");

	apEndTest();
}

strcon02sec();


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

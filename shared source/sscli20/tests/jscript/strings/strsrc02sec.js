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


var iTestID = 244434;

//////////
//
//  	3/15/2002 qiongou created
//
//     strsrc02sec: (i->case sensitive, g->global )
//		an instance of object or variable name or string literal
//		both RegExp object/String literal/RegExp variable
//		A little bit more RegExp 
//		1. control characters & chars in ASCII range
//		2. various regular expression pattern & sepcial chars
//		3. reference/back branches & bug regression 335916
//		4. search using literal string and regular expression
//		5. search unicode ranges chars.
//		6. long string match
//		7. invalid parameter feed in.



function verify(sCat1, sExp, sAct)
{
	//this function makes sure sAct and sExp are equal

	if (sExp != sAct)
		apLogFailInfo((sCat1.length?"--"+sCat1:"")+" failed", sExp,sAct, "");
}


function verifyStringObj(sCat1, sExp, sAct)
{
	//this function simply calls verify with the values of the string
	verify(sCat1, sExp.valueOf(), sAct.valueOf());
}


function ArrayEqual(sCat1, arrayExp, arrayAct)
{var i;
	//Makes Sure that Arrays are equal
	if (arrayAct == null)
		verify(sCat1 + ' NULL Err', arrayExp, arrayAct);
	else if (arrayExp.length != arrayAct.length)
		verify(sCat1 + ' Array length', arrayExp.length, arrayAct.length);
	else
	{
		for (i in arrayExp)
			verify(sCat1 + ' index '+i, arrayExp[i], arrayAct[i]);
	}
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

var str;
var arr;
var reg;



function strsrc02sec() 
{

    apInitTest("strsrc02sec");
	arr = new Array();

	apInitScenario("1. control chars & other chars in ASCII range");
	str = " \x61 b \x00 c \x00"; // with multiple \u0000
	reg = /\x00/i;
	arr = str.search(reg);
	verify ("1. control characters & chars in ASCII range",5,arr);

	arr = str.search(/\x00/g);
	verify ("1. control characters & chars in ASCII range",5,arr);
	
	arr = str.search(new RegExp("\x00","g"));
	verify ("1. control characters & chars in ASCII range",5,arr);


	var a = new Array("0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f");
	var tmp1,tmp2,tmp3 = "";
	for (var i=0;i<16;i++)
	for (var j=0;j<16;j++)
	{
		tmp1 = "\\"+"x"+a[i]+a[j];
		eval("tmp2="+"\'"+tmp1+"'");
		str = "         "+tmp2+" "+tmp2+" "+tmp2;
		eval("reg=/"+tmp1+"/g");
		//print("str = "+str+" reg= "+reg);
		if ((i==2) & (j==0))
			verify("1. with flage g all ASCII range chars",0,str.search(reg));
		else 
			verify("1. with flage g all ASCII range chars",9,str.search(reg));

	}

	str = new String()+'\x21'+"a";
	str="";
	reg = /'e'/;
	arr = str.search(reg);
	verify("1. no match return null failed. ",-1, arr);

	apInitScenario("2. various regular expression pattern & sepcial chars");
	str = "    *  (\t\t43214\t ";
	reg = /\cI/ig;
	verify("2. using \cX (control characters \cI", 8,str.search(reg));

	str = "1234\t5678 \n\n";
	reg = /\cJ/ig;
	verify("2. using \cJ (control characters \cI",10,str.search(reg));

	str = "\x0b\t\t\f\r\u000b";// \v=\x0b=\u000b
	reg = /\r/g;
	verify("2. \\r failed",4,str.search(reg));
	reg = /\v/g;
	verify("2. \\v failed",0,str.search(reg));

	str = "9 02  244wcde";
	reg = /\d/g;
	verify("2. \\d faied",0,str.search(reg));

	str = new String(" 1 ja1 va10java1 1");
	reg = /\w{3}\d?/g;
	arr = str.search(reg);
	ArrayEqual("2. \\w{3}\\d? failed",3,arr);

	str = new String("c de f cd e f");
	reg = /ab|cd|ef/g;
	arr = str.search(reg);
	ArrayEqual("2. //ab|cd|ef//g failed",7,arr);
	
	str = new String(" \x00JavaScript\r Java Javabaka");
	reg = new RegExp ("\\bJava\\b","g");
	arr = str.search(reg);
	verify("2. \\b failed", 14,arr);

	apInitScenario("3. parentheses/reference back branches");
	str = "\r   *() ab < title > This is a test </title> <html> </html> <body> nothing </body>";
	reg = /<\s*(\S+)(\s[^>]*)?>[\s\S]*<\s*\/\1\s*>/g;
	arr = str.search(reg);
	verify("3, html tag with g flag failed",11,arr);
	
	reg = /<\s*(\S+)(\s[^>]*)?>[\s\S]*<\s*\/\1\s*>/i;
	arr= str.search(reg);
	verify("3. html tag with i flag failed", 11,arr);

	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7)\w*/g;    // comment
	arr = str.search(reg);
	verify("3. reference back failed", 22,arr);

	str = new String("Jscript jscript is script cript ript ipt pt oo");
	arr = str.search(reg);
	verify("3. reference back failed", 8,arr);
	
	str = new String("\"test\",\'dev\',\"pm\'");
	reg = /(['"])[^'"]*\1/g; // /['"][^'"]*['"]/g; 
	arr = str.search(reg);
	verify("3. reference back failed",0,arr);
	
	// bug regression for 335916 somehow 9511 this build doesn't have fix yet.
	// need to verify the fix on this too.
	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7){1}\w*/g; // no work for (\6|\7)+ or * or {2},{1,4} 
	arr = str.search(reg);
	verify("3. reference back failed", 22,arr);	

//	need new build to verify the bug fix
	str = new String("\"test\"\",\'dev\',\"pm\'");
	reg = /(['"])[^'"]*\1{1,3}/g;
	arr = str.search(reg);
	verify("3. bug regression", 0, arr);
	
	str = " abaac bc";
	
	reg = new RegExp("(a?)b(\\1{2})+c","g");
	arr = str.search(reg);
	verify("3. bug regression ",1,arr);
	
	reg = new RegExp("(a?)b(\\1){2,4}c","g"); 
	arr = str.search(reg);
	verify("3. bug regression failed", 1,arr);

 	reg = new RegExp("(a?)b(\\1{2,4})c","g");
	reg = /(a?)b(\1{2,4})c/g;
	arr = str.search(reg);
	verify("3. bug regression failed", 1,arr);


	// this works
	str = "baac bc";
	reg =  new RegExp("(a?)b(\\1+)+c","g");
	arr = str.search(reg);
	verify("3. bug regression failed",5,arr);

	apInitScenario("4. search using literal string and literal regular expression");
	arr = "Visit our home page at http://www.microsoft.com/~msdn".search(/(\w+):\/\/([\w.]+)\/(\S*)/);
	verify("4. search using literal",23,arr);
	
	apInitScenario("5. search unicode ranges chars.");
	str = getUnicodeRangeStr();
	if (str != null) {
		reg = new RegExp("\u01ff","g")
		arr = str.search(reg);
		verify("5. search unicode ranges chars",511,arr);
		reg = new RegExp("\u10ff\u1100","g");
		arr = str.search(reg);
		verify("5. search unicode ranges chars",4351,arr);
		reg = /\ufffe\uffff/g;
		arr = str.search(reg);
		verify("5. match unicode ranges chars failed",65534,arr);
	}
	else 
		apLogFailInfo("5. search unicode ranges chars failed on creating str","","","");

	apInitScenario("6. long string search");
	str = getLongStr("*",120000);
	str = str+" ";
	arr = str.search(new RegExp("\\s","g"));
	verify("6. long string search", 120000,arr);
	arr = str.search(/\d/igm);
	verify("6. long string search",-1,arr);

	apInitScenario("7. other type parameter feed in");
	arr = "12345,6789 ".search(new Date());
	verify("7. other type parameter feed in Date",-1,arr)
	arr = "afdafd".search(new Object("f"));
	verify("7. other type parameter feed in Object",1,arr)
	
	arr = "".search("");
	verify("7. other type parameter feed in empty",0,arr)

	arr = "a+b".search(new Function("a","b","a+b"));
	verify("7. other type parameter feed in Function",-1,arr)

	apEndTest();

}


strsrc02sec();


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

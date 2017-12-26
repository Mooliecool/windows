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


var iTestID = 244433;

//////////
//
//  	3/15/2002 qiongou created
//
//     strrpl04sec:
//		We have about 100 TCs that deal with replace in RegExp area (group start rep_*, repa* etc)
//		Also we strrpl01-03 to deal with replace but with different type aguments.
//		So here we focus on:
//		1. control characters & chars in ASCII range
//		2. reference/back branches & bug regression 335916
//		3. unicode ranges chars.
//		4. long string replace.
//		5. take function as parameter


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
	for (var j=0; j<16; j++)	for (var i=0; i<16;i++){
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



function strrpl04sec() 
{

    apInitTest("strrpl04sec");
	arr = new Array();

	apInitScenario("1. control chars & other chars in ASCII range");
	str = "\x01 \x61 b \x01 c \x01"; // with multiple \u0000
	reg = /\x01/g; // need g flag to replace all matched pattern
	arr = str.replace(reg,"\u0000");
	verify("1. \\x01 replace with \\x00","\x00 \x61 b \x00 c \x00",arr);
	
	str = "\x00 \x61 b \x00 c \x00"
	reg = new RegExp("\x00","g");
	verify("1. \x00 replace with \\x01", "\x01 \x61 b \x01 c \x01",str.replace(reg,"\x01"));

	var a = new Array("0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f");
	var tmp1,tmp2,tmp3 = "";
	for (var i=0;i<16;i++)
	for (var j=0;j<16;j++)
	{
		tmp1 = "\\"+"x"+a[i]+a[j];
		eval("tmp2="+"\'"+tmp1+"'");
		str = tmp2+" "+tmp2+" "+tmp2 + " ";
		eval("reg=/"+tmp1+"/g");
		//print("str = "+str+" reg= "+reg);
		if ((i==2) && (j==0)) 
			verify("1. with flage g all ASCII range chars","\uffff\uffff\uffff\uffff\uffff\uffff",str.replace(reg,String.fromCharCode(65535)));
		else
			verify("1. with flage g all ASCII range chars","\uffff \uffff \uffff ",str.replace(reg,String.fromCharCode(65535)));
	}

	apInitScenario("2. parentheses/reference back branches");
	str = "< title > This is a test </title> <body> nothing </body>";
	reg = /<\s*(\S+)(\s[^>]*)?>[\s\S]*<\s*\/\1\s*>/g;
	arr = str.replace(reg,"a a");
	verify("2, html tag with g flag failed", "a a a a",arr);

	str = "*< title > This is a test </title> <body> nothing </body>*";
	reg = /<\s*(\S+)(\s[^>]*)?>[\s\S]*<\s*\/\1\s*>/i;
	arr= str.replace(reg,"b");
	verify("2. html tag with i flag failed","*b <body> nothing </body>*" ,arr);

	
	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7)\w*/g;    // comment
	arr = str.replace(reg,"$7");
	verify("2.1 reference back failed", "Jscript is Script and t",arr);

	arr = str.replace(reg,"$1 $2 $3 $4 $5 $6 $7");
	verify("2.2 refrence back failed","Jscript is Script and jscript script cript ript ipt pt t",arr);

	str = new String("\"test\",\'dev\',\"pm\'");
	reg = /(['"])([^'"]*)\1/g; // /['"][^'"]*['"]/g; 
	arr = str.replace(reg,"$2$1$2");
	verify("2.3 reference back failed","test\"test,dev\'dev,\"pm\'",arr);

	// bug regression for 335916 somehow 9511 this build doesn't have fix yet.
	// need to verify the fix on this too.

	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7){1}\w*/g; // no work for (\6|\7)+ or * or {2},{1,4} 
	arr = str.replace(reg,"$10"); //treated as $1=jscript + "0";
	verify("2.4 reference back failed", "Jscript is Script and jscript0",arr);	

	str = "abaac bc";
	reg =  new RegExp("(a?)b(\\1+)+c","g");
	arr = str.replace(reg,"*");
	verify("2.5 bug regression failed", "* *",arr);

// need new build & this part need re-write
	str = new String("\"test\"\",\'dev\',\"pm\'");
	reg = reg = /(['"])[^'"]*\1{1,3}/g;
	arr = str.replace(reg,"something to feed in");
	
	str = "abaac bc";
	
	reg = new RegExp("(a?)b(\\1{2})+c","g");
	arr = str.replace(reg,"I do");
	verify("3. bug regression ","I do I do",arr);
	
	reg = new RegExp("(a?)b(\\1){2,4}c","g"); 
	arr = str.replace(reg,"better");
	verify("3. bug regression failed","better better",arr);

 	reg = new RegExp("(a?)b(\\1{2,4})c","g");
	reg = /(a?)b(\1{2,4})c/g;
	arr = str.replace(reg,"for next");
	verify("3. bug regression failed","for next for next",arr);


	apInitScenario("3. replace unicode ranges chars.");
	str = getUnicodeRangeStr();
	if (str != null) {
		var t = "43214"+ str+"43214";
		arr = t.replace(str,"t");
		verify("3. unicode range chars","43214t43214",arr);
	}

	apInitScenario("4. long string replace");
	str = getLongStr("*",120000);
	arr = str.replace(new RegExp("\\*{120000}","g"),"_");
	verify("4. long string replace","_",arr);

	apInitScenario("5. take function as parameter");	
	reg = /(\d+(\.\d*)?)F\b/g;
	str = "Water freezes at 32F and boils at 212F.";
	arr = str.replace(reg,function($0,$1,$2) { return ((($1-32) * 5/9) + "C");});
	verify("5. ","Water freezes at 0C and boils at 100C.",arr);

	apEndTest();

}

strrpl04sec();


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

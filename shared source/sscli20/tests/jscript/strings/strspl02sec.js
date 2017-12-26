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


var iTestID = 244435;

//////////
//
//  	3/15/2002 qiongou created
//
//     strspl02sec: (i->case sensitive, g->global )
//		split([ separator : { String | RegExp } [, limit : Number]]) : Array
//		A little bit more RegExp 
//		1. control characters & chars in ASCII range
//		2. various regular expression pattern & sepcial chars
//		3. reference/back branches & bug regression 335916
//		4. split using literal string and regular expression
//		5. split unicode ranges chars.
//		6. long string split
//		7. invalid parameter feed in.
//		8  test limit number with -num, large number, 0, double etc



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



function strspl02sec() 
{

    apInitTest("strspl02sec");
	arr = new Array();

	apInitScenario("1. control chars & other chars in ASCII range");
	str = "\x00 \x61 b \x00 c \x00"; // with multiple \u0000
	reg = /\x00/i;
	arr = str.split(reg);
	ArrayEqual("1. \\x00 with i",new Array(" \x61 b "," c "),arr);
		
	arr = str.split(/\x00/g);
	ArrayEqual("1. \\x00 with g", new Array(" \x61 b "," c "),arr);
	
	reg = new RegExp("\x00","g");
	ArrayEqual("1. \x00 with RegExp", new Array(" \x61 b "," c "),str.split(reg));

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
			ArrayEqual("1. with flage g all ASCII range chars",new Array(),str.split(reg));
		else
			ArrayEqual("1. with flage g all ASCII range chars",new Array(" "," "," "),str.split(reg));
	}

	
	str = new String()+'\x21'+"a";
	reg = /'e'/;
	arr = str.split(reg);
	verify("1. if it omitted returns whole string. ",str, arr);

	str="abcded           efdfasfdaf    e fdsfde";
	arr = str.split("e"); //string pattern will add one more "" at the end.
	ArrayEqual("1. split by e",new Array("abcd","d           ","fdfasfdaf    "," fdsfd",""),arr);

	//check property of returned array
	verify("1. input property failed", undefined,arr.input);
	verify("1. index property failed",undefined, arr.index);
	verify("1. lastIndex property failed",undefined,arr.lastIndex);

	apInitScenario("2. various regular expression pattern & sepcial chars");
	str = "\t4314\t\t43214\t ";
	reg = /\cI/ig;
	arr = str.split(reg);
	ArrayEqual("2. using \cX (control characters \cI",new Array('4314','43214'," "),str.split(reg));


	str = "\n1234\n5678 \n\n";
	reg = /\cJ/ig;
	ArrayEqual("2. using \cJ (control characters \cI",new Array('1234','5678 '),str.split(reg));

	str = "\r\x0b\t\t\f\r\u000b";// \v=\x0b=\u000b
	reg = /\r/g;
	ArrayEqual("2. \\r failed",new Array('\x0b\t\t\f','\u000b'),str.split(reg));

	reg = /\v/g;
	ArrayEqual("2. \\v failed",new Array('\r','\t\t\f\r'),str.split(reg));

	str = "9 02  244wcde";
	reg = /\d/g;
	ArrayEqual("2. \\d faied",new Array(' ',"  ","wcde"),str.split(reg));


	str = new String(" 1 ja1 va10java1 1"); 
	reg = /\w{2}\d?/i;
	arr = str.split(reg);
	ArrayEqual("2. \\w{3}\\d? failed",new Array(" 1 "," "," 1"),arr);
	

	str = new String("abc de f cd e f");
	reg = /ab|cd|ef/g;
	arr = str.split(reg);
	ArrayEqual("2. //ab|cd|ef//g failed",new Array("c de f "," e f"),arr);

	str = new String(" JavaScript Java Javabaka");
	reg = new RegExp ("\\bJava\\b","g");
	arr = str.split(reg);
	ArrayEqual("2. \\b failed", new Array(" JavaScript "," Javabaka"),arr);

	
	apInitScenario("3. parentheses/reference back branches");
	str = "< title > This is a test </title> <body> nothing </body>";
	reg = /<\s*(\S+)(\s[^>]*)?>[\s\S]*<\s*\/\1\s*>/g;
	arr = str.split(reg);
	ArrayEqual("3, html tag with g flag failed", new Array(" "),arr);
	
	str = "*< title > This is a test </title> <body> nothing </body>*";
	reg = /<\s*(\S+)(\s[^>]*)?>[\s\S]*<\s*\/\1\s*>/i;
	arr= str.split(reg);
	ArrayEqual("3. html tag with i flag failed", new Array("*"," ","*"),arr);

	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7)\w*/g;    // comment
	arr = str.split(reg);
	ArrayEqual("3.1 reference back failed", new Array("Jscript is Script and "),arr);

	str = new String("Jscript is Script and jscript is script cript ript ipt pt oo");
	arr = str.split(reg);
	ArrayEqual("3.2 reference back failed", new Array("Jscript is Script and "," oo"),arr);
	
	str = new String("\"test\",\'dev\',\"pm\'");
	reg = /(['"])[^'"]*\1/g; // /['"][^'"]*['"]/g; 
	arr = str.split(reg);
	ArrayEqual("3.3 reference back failed",new Array(",",",\"pm\'"),arr);

	// bug regression for 335916 somehow 9511 this build doesn't have fix yet.
	// need to verify the fix on this too.

	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7){1}\w*/g; // no work for (\6|\7)+ or * or {2},{1,4} 
	arr = str.split(reg);
	ArrayEqual("3. reference back failed", new Array("Jscript is Script and "),arr);	

 //need new build & this part need re-write
	str = new String("\"test\"\",\'dev\',\"pm\'");
	reg = reg = /(['"])[^'"]*\1{1,3}/g;
	arr = str.split(reg);
	
	str = "abaac bc";
	
	reg = new RegExp("(a?)b(\\1{2})+c","g");
	arr = str.split(reg);
	ArrayEqual("3. bug regression ",new Array(" "),arr);
	
	reg = new RegExp("(a?)b(\\1){2,4}c","g"); 
	arr = str.split(reg);
	ArrayEqual("3. bug regression failed", new Array(" "),arr);

 	reg = new RegExp("(a?)b(\\1{2,4})c","g");
	reg = /(a?)b(\1{2,4})c/g;
	arr = str.split(reg);
	ArrayEqual("3. bug regression failed", new Array(" "),arr);


	// this works
	str = "abaac bc";
	reg =  new RegExp("(a?)b(\\1+)+c","g");
	arr = str.split(reg);
	ArrayEqual("3.4 bug regression failed", new Array(" "),arr);

	apInitScenario("4. split using literal string and literal regular expression");
	arr = "Visit our home page at http://www.microsoft.com/~msdn ".split(/(\w+):\/\/([\w.]+)\/(\S*)/);
	ArrayEqual("4. split using literal",new Array("Visit our home page at "," "),arr);
	
	apInitScenario("5. split unicode ranges chars.");
	//str = getUnicodeRangeStr();
	str = " 313 * \u01ff43143\u01ff\r\n\u01fff\f";
	reg = new RegExp("\u01ff","g")
	arr = str.split(reg);
	ArrayEqual("5. \\u01ff failed",new Array(" 313 * ","43143","\r\n","f\f"),arr);

	str = " 313 * \u10ff43143\u10ff\r\n\u10fff\f"
	reg = new RegExp("\u10ff(\u1100)*","g");
	arr = str.split(reg);
	ArrayEqual("5. \\u10fff failed",new Array(" 313 * ","43143","\r\n","f\f"),arr);

	str = " 313 * \uFFFF43143\uffff\r\n\ufffff\f"
	reg = /(\ufffe)?\uffff/g;
	arr = str.split(reg);
	ArrayEqual("5. \\uffff failed",new Array(" 313 * ","43143","\r\n","f\f"),arr);

	apInitScenario("6. long string split");

	str = getLongStr("*",120000);
	str = str + " " + str + "3"+ str;
	arr = str.split(new RegExp("\\*{120000}","g"));
	ArrayEqual("6. long string split",new Array(" ","3"),arr);
	
	apInitScenario("7. other type parameter feed in");
	arr = "12345,6789 ".split(new Date());
	verify("7. Date()","12345,6789 ",arr);

	arr = "afdafd".split(new Object("f"));
	ArrayEqual("7.feed in Ojbect",new Array("a","da","d"),arr);

	arr = "".split("");
	verify("7. empty ","",arr);

	arr = "a+b".split(new Function("return \"a+b\""));
	verify("7. Function","a+b",arr);

	reg = /c/g;
	arr = null;
	arr = str.split(new Number(2));
	verify("7. Number",str,arr);

	apInitScenario(" 8 test limit number with <actuall num ,> actuall num,-num, large number, 0, double");
	str = "\x00 \x61 b \x00 c \x00";
	reg = new RegExp("\x00","g");
	ArrayEqual("8. < actual num",new Array(" \x61 b "),str.split(reg,1));

	ArrayEqual("8 > actual num",new Array(" \x61 b "," c "),str.split(reg,10000));

	ArrayEqual("8 -num",new Array(" \x61 b "," c "),str.split(reg,-1));

	ArrayEqual("8. zero",new Array(),str.split(reg,0));

	ArrayEqual("8 double",new Array(" \x61 b "," c "),str.split(reg,2.64143));

	str = "abcdefg";
	reg = /c/g;
	arr = null;
	arr = str.split(2);
	ArrayEqual("8. change order of parameter",new Array(),str.split(2,reg)); //reg treated as 0

	ArrayEqual("8 undefined",new Array("ab","defg"),str.split(reg,undefined));

	ArrayEqual("8 null",new Array("ab","defg"),str.split(reg,null));
	
	ArrayEqual("8 Date",new Array("ab","defg"),str.split(reg,new Date()));
	
	ArrayEqual("8. Object",new Array(),str.split(reg,new Object())); 

	apEndTest();

}


strspl02sec();


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

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


var iTestID = 244432;

//////////
//
//  	3/15/2002 qiongou created
//
//     strmat02sec: (i->case sensitive, g->global )
//		an instance of object or variable name or string literal
//		both RegExp object/String literal/RegExp variable
//		A little bit more RegExp 
//		1. control characters & chars in ASCII range
//		2. various regular expression pattern & sepcial chars
//		3. reference/back branches & bug regression 335916
//		4. match using literal string and regular expression
//		5. match unicode ranges chars.
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



function strmat02sec() 
{

    apInitTest("strmat02sec");
	arr = new Array();

	apInitScenario("1. control chars & other chars in ASCII range");
	str = "\x00 \x61 b \x00 c \x00"; // with multiple \u0000
	reg = /\x00/i;
	arr = str.match(reg);
	verify ("1. control characters & chars in ASCII range","\x00",arr)
		
	reg = /\x00/g;
	arr = str.match(reg);
	ArrayEqual("1. control characters & chars in ASCII range", new Array('\x00','\x00','\x00'),arr);
	
	reg = new RegExp("\x00","g");
	ArrayEqual("1. control characters & chars in ASCII range", new Array('\x00','\x00','\x00'),str.match(reg));

	var a = new Array("0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f");
	var tmp1,tmp2,tmp3 = "";
	for (var i=0;i<16;i++)
	for (var j=0;j<16;j++)
	{
		tmp1 = "\\"+"x"+a[i]+a[j];
		eval("tmp2="+"\'"+tmp1+"'");
		str = tmp2+" "+tmp2+" "+tmp2;
		eval("reg=/"+tmp1+"/g");
		//print("str = "+str+" reg= "+reg);
		if ((i==2) && (j==0)) 
			ArrayEqual("1. with flage g all ASCII range chars",new Array(tmp2, tmp2,tmp2,tmp2,tmp2),str.match(reg));
		else
			ArrayEqual("1. with flage g all ASCII range chars",new Array(tmp2, tmp2,tmp2),str.match(reg));
		
		
	}
	//check no match return null
	str = new String()+'\x21'+"a";
	str="";
	reg = /'e'/;
	arr = str.match(reg);
	verify("1. no match return null failed. ",null, arr);
	
	str="abcded           efdfasfdaf    e fdsfde";
	arr = str.match("e");

	//check property of returned array
	verify("1. input property failed", str,arr.input);
	verify("1. index property failed",4, arr.index);
	verify("1. lastIndex property failed",5,arr.lastIndex);
	
	apInitScenario("2. various regular expression pattern & sepcial chars");
	str = "\t4314\t\t43214\t ";
	reg = /\cI/ig;
	arr = str.match(reg);
	ArrayEqual("2. using \cX (control characters \cI",new Array('\t','\t','\t','\t'),str.match(reg));
	str = "\n1234\n5678 \n\n";
	reg = /\cJ/ig;
	ArrayEqual("2. using \cJ (control characters \cI",new Array('\n','\n','\n','\n'),str.match(reg));

	//str = "\r\v\t\t\f\r\v"; //we can not directly give \v to string. It is not in ECMA spec.
	str = "\r\x0b\t\t\f\r\u000b";// \v=\x0b=\u000b
	reg = /\r/g;
	ArrayEqual("2. \\r failed",new Array('\r','\r'),str.match(reg));
	reg = /\v/g;
	ArrayEqual("2. \\v failed",new Array('\x0b','\u000b'),str.match(reg));

	str = "9 02  244wcde";
	reg = /\d/g;
	ArrayEqual("2. \\d faied",new Array('9',"0","2","2","4","4"),str.match(reg));

	str = new String(" 1 ja1 va10java1 1");
	reg = /\w{3}\d?/g;
	arr = str.match(reg);
	ArrayEqual("2. \\w{3}\\d? failed",new Array("ja1","va10","jav"),arr);

	str = new String("abc de f cd e f");
	reg = /ab|cd|ef/g;
	arr = str.match(reg);
	ArrayEqual("2. //ab|cd|ef//g failed",new Array("ab","cd"),arr);
	
	str = new String(" JavaScript Java Javabaka");
	reg = new RegExp ("\\bJava\\b","g");
	arr = str.match(reg);
	ArrayEqual("2. \\b failed", new Array("Java"),arr);

	apInitScenario("3. parentheses/reference back branches");
	str = "< title > This is a test </title> <body> nothing </body>";
	reg = /<\s*(\S+)(\s[^>]*)?>[\s\S]*<\s*\/\1\s*>/g;
	arr = str.match(reg);
	ArrayEqual("3, html tag with g flag failed", new Array("< title > This is a test </title>","<body> nothing </body>"),arr);
	
	reg = /<\s*(\S+)(\s[^>]*)?>[\s\S]*<\s*\/\1\s*>/;
	arr= str.match(reg);
	ArrayEqual("3. html tag with i flag failed", new Array("< title > This is a test </title>","title"," "),arr);

	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7)\w*/g;   // comment
	arr = str.match(reg);
	ArrayEqual("3. reference back failed", new Array("jscript is script cript ript ipt tpttoo"),arr);

	str = new String("Jscript is Script and jscript is script cript ript ipt pt oo");
	arr = str.match(reg);
	ArrayEqual("3. reference back failed", new Array("jscript is script cript ript ipt pt"),arr);
	
	str = new String("\"test\",\'dev\',\"pm\'");
	reg = /(['"])[^'"]*\1/g; // /['"][^'"]*['"]/g; 
	arr = str.match(reg);
	ArrayEqual("3. reference back failed",new Array("\"test\"","\'dev\'"),arr);
	
	// bug regression for 335916 somehow 9511 this build doesn't have fix yet.
	// need to verify the fix on this too.
	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7){1}\w*/g; // no work for (\6|\7)+ or * or {2},{1,4} 
	arr = str.match(reg);
	ArrayEqual("3. reference back failed", new Array("jscript is script cript ript ipt tpttoo"),arr);	

// need new build to verify the bug fix
	str = new String("\"test\"\",\'dev\',\"pm\'");
	reg = /(['"])[^'"]*\1{1,3}/g;
	arr = str.match(reg);
	
	str = "abaac bc";
	
	reg = new RegExp("(a?)b(\\1{2})+c","g");
	arr = str.match(reg);
	ArrayEqual("3. bug regression ",new Array("abaac","bc"),arr);
	
	reg = new RegExp("(a?)b(\\1){2,4}c","g"); 
	arr = str.match(reg);
	ArrayEqual("3. bug regression failed", new Array("abaac","bc"),arr);

 	reg = new RegExp("(a?)b(\\1{2,4})c","g");
	reg = /(a?)b(\1{2,4})c/g;
	arr = str.match(reg);
	ArrayEqual("3. bug regression failed", new Array("abaac","bc"),arr);


	str =  "a\r\nb\r\nc";
	reg = /a(.*\n)*b/i;
    ArrayEqual("3. bug regression failed", new Array("a\r\nb","\r\n"),reg.exec(str)); 
	

	// this works
	str = "abaac bc";
	reg =  new RegExp("(a?)b(\\1+)+c","g");
	arr = str.match(reg);
	ArrayEqual("3. bug regression failed", new Array("abaac","bc"),arr);

	apInitScenario("4. match using literal string and literal regular expression");
	arr = "Visit our home page at http://www.microsoft.com/~msdn".match(/(\w+):\/\/([\w.]+)\/(\S*)/);
	ArrayEqual("4. match using literal",new Array("http:\/\/www.microsoft.com\/~msdn","http","www.microsoft.com","~msdn"),arr);
	
	apInitScenario("5. match unicode ranges chars.");
	str = getUnicodeRangeStr();
	if (str != null) {
		reg = new RegExp("\u01ff","g")
		arr = str.match(reg);
		ArrayEqual("5. match unicode ranges chars",new Array("\u01ff"),arr);
		reg = new RegExp("\u10ff\u1100","g");
		arr = str.match(reg);
		ArrayEqual("5. match unicode ranges chars",new Array("\u10ff\u1100"),arr);
		reg = /\ufffe\uffff/g;
		arr = str.match(reg);
		ArrayEqual("5. match unicode ranges chars failed",new Array("\ufffe\uffff"),arr);
	}
	else 
//		print("5. match unicode ranges chars failed on creating str");

	apInitScenario("6. long string match");
	str = getLongStr("*",120000);
	arr = str.match(new RegExp("\\*{1,120000}","g"));
	if (arr.length != 1) 
	{
		apLogFailInfo("6. long string match",1,arr.length,"");
		if (arr[0].length != 120000)
			apLogFailInfo("6. long string match",120000,arr.length,"");
	}
	
	apInitScenario("7. other type parameter feed in");
	arr = "12345,6789 ".match(new Date());
	if (arr != null)
		apLogFailInfo("7.feed in Date object",null, arr,"");
	arr = "afdafd".match(new Object("f"));
	ArrayEqual("7.feed in Ojbect",new Array("f"),arr);
	
	arr = "".match("");
	ArrayEqual("empty string",new Array(""),arr);

	arr = "a+b".match(new Function("a","b","a+b"));
	if (arr != null)
		apLogFailInfo("7.feed in Function object",null, arr,"");
	
	apEndTest();

}

strmat02sec();


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

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


var iTestID = 244471;

//////////
//
//  	3/15/2002 qiongou created
//
//     regexp02sec.js: (i->case sensitive, g->global )
//	focus more on back referene and grouping & multiple lines.



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
	print(strall.length);
	if (strall.length != 65536){
		apLogFailInfo("getUnicodeRangeStr error occured lenth is incorrect",strall.length,65536,"");
		return null;
	}
	else
	{	
		print(strall.length);
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
var sExp;
var sAct;


function regexp02sec() 
{

    apInitTest("regexp02sec");
	
	apInitScenario("1. Reference back to 2-7 ");
	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7)\w*/g;	// comment
	sAct = reg.exec(str);
	sExp = new Array("jscript is script cript ript ipt tpttoo","jscript","script","cript","ript","ipt","pt","t","t");

	if (sAct != null)
    	ArrayEqual("1. 	Reference back to 7 exec failed", sExp,sAct);
 	else
		apLogFailInfo("1. Reference back to 7 exec failed",sExp, sAct);
	
	if (ScriptEngineMajorVersion()<7) reg.compile("([jJ]([Ss](c(r(i(p(t)))))))\\sis\\s\\2\\s\\3\\s\\4\\s\\5\\s","g");
       else reg.compile("([jJ]([Ss](c(r(i(p(t)))))))\\sis\\s\\2\\s\\3\\s\\4\\s\\5\\s","");

	verify("1.Reference back to 7 test failed",true, reg.test(str));
	
	apInitScenario("2. Reference back to 26 ");
	str = new String("abcdefghijklmnopqrstuvwxyz is z");
	reg = /(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z))))))))))))))))))))))))))\sis\s\26/g;
	sAct = reg.exec(str);
	sExp = "z";
	if (sAct != null){
		if( sAct[26] != sExp)
    		apLogFailInfo("2. Reference back to 26 exec failed", sExp, sAct[26]);
	}
 	else
		apLogFailInfo("2. Reference back to 26 exec failed",sExp, sAct);

	if (ScriptEngineMajorVersion()<7) reg.compile("(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z))))))))))))))))))))))))))\\sis\\s\\25","g");
       else reg.compile("(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z))))))))))))))))))))))))))\\sis\\s\\25","");

	str = new String("abcdefghijklmnopqrstuvwxyz is yz");
	verify("2. 2. Reference back to 26 test failed",true,reg.test(str));

	
	apInitScenario("3. Reference back to limit 99");
	str = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstu is u";
	reg = /(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))\sis\s\99/g;
	sAct = reg.exec(str);
	sExp = "u";
	if (sAct != null){
		if( sAct[99] != sExp)
    		apLogFailInfo("3. Reference back to limit 99 exec failed", sExp, sAct[99]);
	}
 	else
		apLogFailInfo("3. Reference back to limit 99 exec failed",sExp, sAct);

	if (ScriptEngineMajorVersion()<7) reg.compile("(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))\\sis\\s\\99","g");
       else reg.compile("(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))\\sis\\s\\99","");

	verify("3. Reference back to limit 99 exec failed",true,reg.test(str));

	apInitScenario("4. Reference back beyond limit 99");
	str = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuv is uv";
	reg = /(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v(w(x(y(z(a(b(c(d(e(f(g(h(i(j(k(l(m(n(o(p(q(r(s(t(u(v))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))\sis\s\100/g;
	sAct = reg.exec(str);
	if (sAct != null) 
		apLogFailInfo("4. Reference back beyond limit 99",null, sAct.length, "");

	apInitScenario("5. Reference back bug regressions ");
	// bug regression for 335916 somehow 9511 this build doesn't have fix yet.
	// need to verify the fix on this too.
	str = new String("Jscript is Script and jscript is script cript ript ipt tpttoo");
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7){1}\w*/g; // no work for (\6|\7)+ or * or {2},{1,4} 
	sAct = reg.exec(str);	
	//print(sAct);jscript is script cript ript ipt tpttoo,jscript,script,cript,ript,ipt,pt,t,t
	sExp = new Array( "jscript is script cript ript ipt tpttoo","jscript","script","cript","ript","ipt","pt","t","t");
	if (sAct != null)
		ArrayEqual("5.1 Reference back bug regressions ",sExp,sAct);
	else
		apLogFailInfo("5.1 Reference back bug regressions ","an array", null, ""); 


	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7)+\w*/g; // no work for (\6|\7)+ or * or {2},{1,4} 
	sAct = reg.exec(str);	
	if (sAct != null)
		ArrayEqual("5.2 Reference back bug regressions ",sExp,sAct);
	else
		apLogFailInfo("5.2 Reference back bug regressions ","an array", null, ""); 
	
	
	reg = /([jJ]([Ss](c(r(i(p(t)))))))\sis\s\2\s\3\s\4\s\5\s(\6|\7){2}\w*/i; // no work for (\6|\7)+ or * or {2},{1,4} 
	sAct = reg.exec(str);	
	//print(sAct); jscript is script cript ript ipt tpttoo,jscript,script,cript,ript,ipt,pt,t,pt
	sExp = new Array( "jscript is script cript ript ipt tpttoo","jscript","script","cript","ript","ipt","pt","t","pt");
	if (sAct != null)
		ArrayEqual("5.3 Reference back bug regressions ",sExp,sAct);
	else
		apLogFailInfo("5.3 Reference back bug regressions ","an array", null, ""); 
	
	//need new build to verify the bug fix
	str = new String("\"test\"\",\'dev\',\"pm\'");
	reg = reg = /(['"])[^'"]*\1{1,3}/g;
	sAct = reg.exec(str);
	if (sAct != null)
		ArrayEqual("5.4 ",new Array("\"test\"\"","\""),sAct);
	else 
		apLogFailInfo("5.4", "an array",null,"");

	//print(sAct);"test"","
	
	str = "abaac bc";
	
	reg = new RegExp("(a?)b(\\1{2})+c","");
	sAct = reg.exec(str);
	sExp = new Array("abaac","a","aa");
	if (sAct != null)
		ArrayEqual("5.5 ",sExp,sAct);
	else
		apLogFailInfo("5.5","an array",null,"");

	reg = /(a?)b(\1{2,4})c/;
	sAct = str.match(reg);
	if (sAct != null)
		ArrayEqual("5.6 ",sExp,sAct);
	else
		apLogFailInfo("5.6","an array",null,"");

	str = "abaac bc";
	reg =  new RegExp("(a?)b(\\1+)+c");
	sAct = reg.exec(str);
	if (sAct != null)
		ArrayEqual("5.7 ",sExp,sAct);
	else
		apLogFailInfo("5.7","an array",null,"");

	reg = new RegExp("(a?)b(\\1){2,4}c","g"); 
	sAct = reg.exec(str);
	sExp = new Array("abaac","a","a");
	if (sAct != null)
		ArrayEqual("5.8 ",sExp,sAct);
	else
		apLogFailInfo("5.8","an array",null,"");


	reg = new RegExp("(a?)b(\\1{2})+c","g");
	str = "bc";
	verify("5.9 bug 335916",true,reg.test(str));

	// syntax error -reg = new RegExp("(a?)b(\\1{2,0}){2,0}c","g");
	reg = new RegExp("(a?)b(\\1+)+c","g");
	verify("5.10 bug 335916",true,reg.test(str));

	str = "a\r\nb\r\nc";
	reg = /a(.*\n)*b/i
	sAct = reg.exec(str)
	if (sAct != null)
		ArrayEqual("5.11 bug 507944 ",new Array("a\r\nb","\r\n"),sAct);
	else
		apLogFailInfo("5.11 bug 507944","an array",null,"");

	apInitScenario("6. grouping 99 times");
	str = "JScriptjscript99";
	reg = /((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((JScriptjscript99))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))/g;
	verify("6. grouping 99 times",true,reg.test(str)); 


	str = "JScript7JScript7"; //JScript.NET-\99, but JScript5.6 -\98
	reg = /(((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((?=JScript)JScript(?=7)7))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))\98/g;
	verify("6. group  98 times", true, reg.test(str));


	apInitScenario("7 . grouping with multiple lines");
	str = "J\r\nJScript7\nJscript\n";
	reg = /(((((((((((((((((((((JScript7)))))))))))))))))))))/gm;
	verify("7. gouping with multiple lines ",true, reg.test(str));
	str = "\n\n\n\n\r\n\r\n\r\n\r\nJscript\r\n\r\n\JScript7";
	verify("7. gouping with multiple lines ",true, reg.test(str));

	apEndTest();

}

regexp02sec();


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

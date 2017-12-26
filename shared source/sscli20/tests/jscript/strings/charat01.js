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


var iTestID = 53676;

//////////
//
//  Test Case:
//
//		Added scenarios  from 18 -qiongou 3/19/02

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

// String.prototype.charAt( pos )
@cc_on
@if(@_fast)
	var foo, foo1, foo2, foo3, foo4, foo5 = "";
	var numobjvar = "";
	var a = "";
@end

foo = "abcde";
foo2 = String.fromCharCode(0, 97, 65535 );
foo3 = "abc\u0001\u0002\u0004\u0005def";
foo4 = "\u1000\u1001\u1002\u1003\u1004\u1005";
foo5 = "‚ ‚¦‚¢‚¨‚¤"



function bar() {
	return 2;
}

function test( res, exp, bug )
{
	if ( res != exp )
		apLogFailInfo( "wrong number", exp, res, bug );
}


function charat01()
{
apInitTest("charat01");


//----------------------------------- negative number
apInitScenario("1. negative number");
if ( foo.charAt(-1) != "" )
	apLogFailInfo( "wrong number", "undefined", foo.charAt(-1), "" );


//----------------------------------- string
apInitScenario("2. string");
test( foo.charAt("2"), "c" );


//----------------------------------- number past String.length-1
apInitScenario("3. number past String.length-1");
if ( foo.charAt(5) != "" )
	apLogFailInfo( "wrong number", "undefined", foo.charAt(5), "" );


//----------------------------------- float
apInitScenario("4. float");
test( foo.charAt(2.1), "c", "" );
test( foo.charAt(2.8), "c", "" );


//----------------------------------- boolean
apInitScenario("5. boolean");
test( foo.charAt(true), "b", "" );
test( foo.charAt(false), "a", "" );


//----------------------------------- Number
apInitScenario("6. Number");
numobjvar = new Number(2);
test( foo.charAt(numobjvar), "c", "" );


//----------------------------------- variable
apInitScenario("7. variable");
a = 2;
test( foo.charAt(a), "c", "" )


//----------------------------------- expression
apInitScenario("8. expression");
test( foo.charAt( 10/5 ), "c", "" );


//----------------------------------- eval
apInitScenario("9. eval");
test( foo.charAt( eval(1+1) ), "c", "" );


//----------------------------------- function
apInitScenario("10. function");
test( foo.charAt( bar() ), "c", "" );


//----------------------------------- first position
apInitScenario("11. first position");
test( foo.charAt(0), "a", "" );


//----------------------------------- last position
apInitScenario("12. last position");
test( foo.charAt(4), "e", "" );


//----------------------------------- no argument
apInitScenario("13. no argument");
test( foo.charAt(), "a", "" );


//----------------------------------- return 0
apInitScenario("14. return 0");
test( foo2.charAt(0), String.fromCharCode(0), "" );

//----------------------------------- return 2^16
apInitScenario("15. return 2^16");
@if (@_win16)
test( foo2.charAt(2), String.fromCharCode(63), "" );
@else
test( foo2.charAt(2), String.fromCharCode(65535), "" );
@end

//----------------------------------- CharAt -1
apInitScenario("16. Unicode strings");
@if(!@_win16)
if ( foo4.charAt(0) != "\u1000" )
	apLogFailInfo( "wrong number", "", foo3.charAt(0), "" );

if ( foo4.charAt(3) != "\u1003" )
	apLogFailInfo( "wrong number", "", foo3.charAt(3), "" );

if ( foo4.charAt(5) != "\u1005" )
	apLogFailInfo( "wrong number", "", foo3.charAt(5), "" );
@end

//-----------------------------------
apInitScenario("17. Unicode and ansi mixed strings");
@if(!@_win16)
if ( foo3.charAt(0) != "a" )
	apLogFailInfo( "wrong number", "", foo.charAt(0), "" );

if ( foo3.charAt(3) != "\u0001" )
	apLogFailInfo( "wrong number", "", foo.charAt(3), "" );

if ( foo3.charAt(5) != "\u0004" )
	apLogFailInfo( "wrong number", "", foo.charAt(5), "" );

if ( foo3.charAt(9) != "f" )
	apLogFailInfo( "wrong number", "", foo.charAt(9), "" );
@end

apInitScenario("18. null");
test(foo.charAt(null),"a","");

apInitScenario("19. whole range of unicode chars");
var str = getUnicodeRangeStr();
if (str != null)
	for (var i=0; i<str.length; i++)
		test(str.charAt(i),String.fromCharCode(i),"");

apInitScenario("20. long string");
str = getLongStr("*-",120000);
if (str != null)
	test(str.charAt(239999),"-",""); 

apEndTest();

}



charat01();


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

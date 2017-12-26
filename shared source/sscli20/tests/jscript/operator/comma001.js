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


var iTestID = 147983;


function verify(sCat, sAct, sExp){
	if (sExp != sAct)
		apLogFailInfo(sCat+" failed-",sExp,sAct, "");
}

var x = 0,y = 0;
function ret_true(){return true;}
function ret_false(){return false;}
function ret_test1(){return (2+8,1+3);}
function ret_test2(){return (ret_true(),ret_false());}
function ret_test3(){return (x = 5,y = 8);}
function ret_test4(){x=0;y=0;return (x = 5,y = x,y);}

function comma001() {
	var i, j;
	apInitTest( "Comma001 " );

	apInitScenario( "1. Comma in a for loop exit clause" );

		for (i=0, j=10; ret_true(), ret_false(), i<j; i++, j--){/*do nothing*/}
		verify("true,false,cond",i==j,true);

		for (i=0, j=10; ret_false(), ret_true(), i<j; i++, j--){/*do nothing*/}
		verify("false,true,cond",i==j,true);

		for (i=0, j=10; i++, i<10; j++){/*do nothing*/}
		verify("i++,cond",i,10);

		j = 10;
		for (i=0,j=i;i<10;i++){/*do nothing*/}
		verify("precedence test",j,0);

	apInitScenario( "2. Comma in a while loop exit clause" );

		i = 1;
		while (ret_true(),i<10){i++;}
		verify("true,cond",i,10);

		i = 1;
		while (ret_false(),i<10){i++;}
		verify("false,cond",i,10);

		i = 1;
		while (i++,i<10){/*do nothing*/}
		verify("i++,cond",i,10);

		j = 1;
		while(i=j,i<10){j++;i=0;}
		verify("precedence test",i,10);

	apInitScenario( "3. Comma in a do..while loop exit clause" );

		i = 1;
		do{	i++;
		}while (ret_true(),i<10);
		verify("true,cond",i,10);

		i = 1;
		do{	i++;
		}while (ret_false(),i<10);
		verify("false,cond",i,10);

		i = 1;
		do{	//nothing
		}while (i++,i<10);
		verify("i++,cond",i,10);

		j = 1;
		do{ j++; i=0;
		}while (i=j,i<10);
		verify("precedence test",i,10);

	apInitScenario( "4. Comma in an eval" );

		j = eval("ret_true(),ret_false();");
		verify("true,false",j,false);

		j = eval("2+5,1+2;");
		verify("some math",j,3);

		i = 10; j = 10;
		eval("i=1,i=5;");
		verify("reassign value to i",i,5);

		i = 10; j = 10;
		eval("j=1,i=5;");
		verify("assign to i and j",i,5);
		verify("assign to i and j",j,1);

		i = 1; j = 10;
		eval("j=5,i=j;");
		verify("precedence test",i,5);

	apInitScenario( "5. Comma in function return" );

		i = 0;
		i = ret_test1();
		verify("math in return",i,4);

		i = ret_test2();
		verify("function returns in return",i,false);

		i = ret_test3();
		verify("RHS of return",y,8);
		verify("LHS of return",x,5);

		i = 0;
		i = ret_test4();
		verify("precedence test",i,5);
		verify("precedence test",y,5);

	apEndTest();
}


comma001();


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

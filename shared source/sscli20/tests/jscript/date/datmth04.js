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


var iTestID = 51832;

function Test( a, desc )
{
	if ( a.valueOf() != Date.parse(a.toString()) )
		apLogFailInfo( desc+" failed", Date.parse(a.toString()), a.valueOf(), "Scripting:368");
}	

function datmth04()
{
	apInitTest( "datmth04: Date.parse must understand every Date.prototype.toString");

        if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	//---------------------------------------
	apInitScenario( "1. BC" );

	var a = new Date( "July 10, 1997BC" );
	Test(a, "BC");
	a = new Date( "July 10, 1997 BC" );
	Test(a, " BC");
	a = new Date( "July 10, 1997B.C." );
	Test(a, "B.C.");
	a = new Date( "July 10, 1997 B.C." );
	Test(a, " B.C.");
	a = new Date( "July 10, 1997B.C" );
	Test(a, "B.C");
	a = new Date( "July 10, 1997BC." );
	Test(a, "BC.");
	a = new Date( "July 10, 1997Bc" );
	Test(a, "Bc");
	a = new Date( "July 10, 1997bC" );
	Test(a, "bC");
	a = new Date( "July 10, 1997b.c." );
	Test(a, "b.c.");

	//----------------------------------------
	apInitScenario( "2. AD" );

	a = new Date( "July 10, 1997AD" );
	Test(a, "AD");
	a = new Date( "July 10, 1997 AD" );
	Test(a, " AD");
	a = new Date( "July 10, 1997A.D." );
	Test(a, "A.D.");
	a = new Date( "July 10, 1997 A.D." );
	Test(a, " A.D.");
	a = new Date( "July 10, 1997A.D" );
	Test(a, "A.D");
	a = new Date( "July 10, 1997AD." );
	Test(a, "AD.");
	a = new Date( "July 10, 1997Ad" );
	Test(a, "Ad");
	a = new Date( "July 10, 1997aD" );
	Test(a, "aD");
	a = new Date( "July 10, 1997a.d." );
	Test(a, "a.d.");

	//----------------------------------------
	apInitScenario( "3. AM" );

	a = new Date( "July 10, 1997 10:42AM" );
	Test(a, "AM");
	a = new Date( "July 10, 1997 10:42 AM" );
	Test(a, " AM");
	a = new Date( "July 10, 1997 10:42A.M." );
	Test(a, "A.M.");
	a = new Date( "July 10, 1997 10:42 A.M." );
	Test(a, " A.M.");
	a = new Date( "July 10, 1997 10:42A.M" );
	Test(a, "A.M");
	a = new Date( "July 10, 1997 10:42AM." );
	Test(a, "Am.");
	a = new Date( "July 10, 1997 10:42Am" );
	Test(a, "Am");
	a = new Date( "July 10, 1997 10:42aM" );
	Test(a, "aM");
	a = new Date( "July 10, 1997 10:42a.m." );
	Test(a, "a.m.");

	//------------------------------------------
	apInitScenario( "4. PM" );

	a = new Date( "July 10, 1997 10:42PM" );
	Test(a, "PM");
	a = new Date( "July 10, 1997 10:42 PM" );
	Test(a, " PM");
	a = new Date( "July 10, 1997 10:42P.M." );
	Test(a, "P.M.");
	a = new Date( "July 10, 1997 10:42 P.M." );
	Test(a, " P.M.");
	a = new Date( "July 10, 1997 10:42P.M" );
	Test(a, "P.M");
	a = new Date( "July 10, 1997 10:42PM." );
	Test(a, "Pm.");
	a = new Date( "July 10, 1997 10:42Pm" );
	Test(a, "Pm");
	a = new Date( "July 10, 1997 10:42pM" );
	Test(a, "pM");
	a = new Date( "July 10, 1997 10:42p.m." );
	Test(a, "p.m.");

	
	//------------------------------------------
	apInitScenario( "UTC aliases GMT" );

	if( Date.prototype.toUTCString() != Date.prototype.toGMTString())
		apLogFailInfo( "UTC aliases GMT failed",true,false , "Scripting:188");
        }
	apEndTest();
}

datmth04();


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

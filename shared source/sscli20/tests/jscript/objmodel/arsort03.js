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


var iTestID = 51737;


function invord(a,b){
	if(a > b)
		return -1;
	if(a==b)
		return 0;
	return 1;
}

function verify(vAct, vExp, bugNum){
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo("", vExp, vAct, bugNum);
}

//added for unfixed bug 314108
@if(@aspx) expando @end function cmpCaseInsensRev (s1, s2){
    s1 = s1.toUpperCase();
    s2 = s2.toUpperCase();
    if ( s1 < s2 )
        return 1;
    else if ( s1 > s2 )
        return -1;
    return 0;
}

function arsort03(){
@if(@_fast)
   var rg1, rg2;
@end

    apInitTest("arSort03 ");
    
    //----------------------------------------------------------------------------
    apInitScenario("test comparison routine useage");

    rg1 = new Array('d','Z','a','l','C');
    rg1.sort();

    rg2 = new Array('d','Z','a','l','C');
    rg2.sort(cmpCaseInsensRev);

    verify( rg1[ 0] == 'C' &&
            rg1[ 1] == 'Z' &&
            rg1[ 2] == 'a' &&
            rg1[ 3] == 'd' &&
            rg1[ 4] == 'l' &&
            rg2[ 0] == 'Z' &&
            rg2[ 1] == 'l' &&
            rg2[ 2] == 'd' &&
            rg2[ 3] == 'C' &&
            rg2[ 4] == 'a', true, null );

//----------------------------------------------------------------------------
//  sorting is not consistent if the sort fn 
//  does not account for mix in char and int

    apInitScenario("test custom sort fn, valid with numbers and chars");
/*	rg1 = new Array('z','q',1,"zsd");
	rg1.sort(invord);
	verify( rg1[0] == 1 &&
			rg1[1] == "zsd" &&
			rg1[2] == 'z' &&
			rg1[3] == 'q', true, null);
        for(var i=0;i<4;i++) print(rg1[i]);
*/
//----------------------------------------------------------------------

    apInitScenario("test standard sort with missing, undef elements");
	rg1 = new Array();
	rg1[0] = 0;
	rg1[2] = 2;
	rg1[4] = 4;
	rg1[6] = 6;
	rg1[8] = 8;
	rg1[9] = undefined;
	rg1[10] = 1;
	
	rg1.sort();
	verify( rg1[0] == 0 &&
			rg1[1] == 1 &&
			rg1[2] == 2 &&
			rg1[3] == 4 &&
			rg1[4] == 6 &&
			rg1[5] == 8 &&
			rg1[6] == undefined &&
			rg1[7] == undefined &&
			rg1[8] == undefined &&
			rg1[9] == undefined &&
			rg1[10] == undefined, true, null);

	//----------------------------------------------------------------------------
    apInitScenario("test custom sort with missing, undef elements");
	rg1 = new Array();
	rg1[0] = 0;
	rg1[2] = 2;
	rg1[4] = 4;
	rg1[6] = 6;
	rg1[8] = 8;
	rg1[9] = undefined;
	rg1[10] = 1;
	
	rg1.sort(invord);
	verify( rg1[0] == 8 &&
			rg1[1] == 6 &&
			rg1[2] == 4 &&
			rg1[3] == 2 &&
			rg1[4] == 1 &&
			rg1[5] == 0 &&
			rg1[6] == undefined &&
			rg1[7] == undefined &&
			rg1[8] == undefined &&
			rg1[9] == undefined &&
			rg1[10] == undefined, true, null);

    apEndTest();
}


arsort03();


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

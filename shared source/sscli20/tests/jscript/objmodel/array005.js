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


var iTestID = 242563;

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function array005() {

    apInitTest("array005");

    var strObj = new String("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq") ;
    var numObj = new Number(100) ;
    var funObj = new Function ("x", "y", "return(x+y)") ;
    var datObj = new Date("03/25/2002") ;
    var arr1 ;
    var arr2 ;
    var arr3 ;
    var arr4 ;
    var iTmp ;
    var s = "####################################################################################################" ;
    
    apInitScenario("Scenario 1: create Array with different types of items") ;

    arr1 = new Array(funObj, "One", strObj, Array(10, 20, 30), numObj, datObj, 2) ;
    verify (arr1.length, 7, "1) array not created correctly") ;
 
    verify (arr1[0](4, 5), 9, "1) arr1[0] incorrect") ;
    verify (arr1[1], "One", "1) arr1[1] incorrect") ;
    verify (arr1[2], strObj, "1) arr1[2] incorrect") ;
    verify (arr1[3][0], 10, "1) arr1[3][0] incorrect") ;
    verify (arr1[3][1], 20, "1) arr1[3][1] incorrect") ;
    verify (arr1[3][2], 30, "1) arr1[3][2] incorrect") ;
    verify (arr1[4], 100, "1) arr1[4] incorrect") ;
    verify (arr1[5], datObj, "1) arr1[5] incorrect") ;
    verify (arr1[6], 2, "1) arr1[6] incorrect") ;

    apInitScenario("Scenario 2: Array that contains long strings") ;
    
    for ( iTmp = 0 ; iTmp < 18 ; iTmp++ )
      s = s + s ;

    arr2 = new Array () ;
    for ( iTmp = 0 ; iTmp < 10 ; iTmp++ )
       arr2[iTmp] = strObj ;
    for ( iTmp = 62 ; iTmp < 77 ; iTmp++ )
       arr2[iTmp] = s ;
    for ( iTmp = 91 ; iTmp < 114 ; iTmp++ )
       arr2[iTmp] = s ;
    for ( iTmp = 138 ; iTmp < 154 ; iTmp++ )
       arr2[iTmp] = strObj ;
    for ( iTmp = 164 ; iTmp < 176 ; iTmp++ )
       arr2[iTmp] = strObj ;
    arr2[55] = strObj ;
    arr2[83] = s ;
    arr2[119] = s ;
    arr2[128] = s ;
    arr2[133] = s ;
    arr2[161] = strObj ;
    arr2[189] = strObj ;
    arr2[199] = s ;

    verify (arr2.length, 200, "2) array not created correctly") ;
    verify (arr2[5], strObj, "2) arr2[5] incorrect") ;
    verify (arr2[65], s, "2) arr2[65] incorrect") ;
    verify (arr2[105], s, "2) arr2[105] incorrect") ;
    verify (arr2[145], strObj, "2) arr2[145] incorrect") ;
    verify (arr2[175], strObj, "2) arr2[5] incorrect") ;
    verify (arr2[199], s, "2) arr2[199] incorrect") ;

    apInitScenario("Scenario 3: Nested Arrays that contains long strings") ;

    arr3 = new Array(arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2, arr2)
        
    verify (arr3.length, 22, "3.1) array not created correctly") ;   
    verify (arr3[0][9], strObj, "3.1) arr3[0][9] incorrect") ;
    verify (arr3[3][69], s, "3.1) arr3[3][69] incorrect") ;
    verify (arr3[6][99], s, "3.1) arr3[6][99] incorrect") ;
    verify (arr3[9][139], strObj, "3.1) arr3[9][139] incorrect") ;
    verify (arr3[12][169], strObj, "3.1) arr3[12][169] incorrect") ;
    verify (arr3[15][199], s, "3.1) arr3[15][199] incorrect") ;
    verify (arr3[18][0], strObj, "3.1) arr3[18][0] incorrect") ;
    verify (arr3[21][100], s, "3.1) arr3[21][100] incorrect") ;
 

    arr4 = new Array() ;
    arr4[0] = arr3 ;
    arr4[3] = arr3 ;
    arr4[7] = arr1 ;
    arr4[12] = arr2 ;
    arr4[18] = arr3 ;
    arr4[25] = s ;
    arr4[33] = arr3 ;
    arr4[42] = strObj ;
    arr4[52] = arr2 ;
    arr4[63] = s ;
    arr4[75] = arr2 ;
    arr4[88] = s ;
    arr4[102] = strObj ;
    arr4[127] = arr1 ;
    arr4[143] = arr1 ;
    arr4[160] = arr3 ;
    arr4[178] = s ;
    arr4[197] = s ;
    arr4[217] = s ;
    arr4[232] = arr2 ;
    arr4[999] = arr3 ;
 
    verify (arr4.length, 1000, "3.2) array not created correctly") ;    
    verify (arr4[0][1][2], strObj, "3.2) arr4[0][1][2] incorrect") ;
    verify (arr4[3][4][5], strObj, "3.2) arr4[3][4][5] incorrect") ;
    verify (arr4[7][0](3, 7), 10, "3.2) arr4[7][0] incorrect") ;
    verify (arr4[12][113], s, "3.2) arr4[12][113] incorrect") ;
    verify (arr4[25], s, "3.2) arr4[25] incorrect") ;
    verify (arr4[102], strObj, "3.2) arr4[102] incorrect") ;
    verify (arr4[143][1], "One", "3.2) arr4[143][1] incorrect") ;
    verify (arr4[197], s, "3.2) arr4[197] incorrect") ;
    verify (arr4[999][9][99], s, "3.2) arr4[999][9][99] incorrect") ;

    apInitScenario("Scenario 4: Recursive nested Arrays that contains long strings") ;

    for ( iTmp = 0 ; iTmp < 10000 ; iTmp++ )
      {
	arr4 = new Array(arr1, arr2, arr3, arr4) ;
	arr4[iTmp*2] = arr4 ;
      }

    verify (arr4.length, 19999, "4) array not created correctly") ;

    apEndTest();

}


array005();


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

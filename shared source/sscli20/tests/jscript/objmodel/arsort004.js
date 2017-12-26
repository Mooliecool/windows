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


var iTestID = 51738;


//This is a stress test on the Array.sort functionality
//using a custom sorting algorithm, and a "unsorting"
//algorithm to make sure that the array is constantly reshuffled
//Test one is a perpetually unsorted array that is always unsorted, and then sorted.
//Test two is a unsorted array that is sorted multiple times

var a = new Array(99,22,78,24,25,32,27,28);
var di = 0;
var BIG = 20000;

function makeAscending(a){	
	var i;
	for(i=0; i<BIG; i++)
		a[i] = i;
}
function makeDescending(a){
	var i;
	for(i=0; i<BIG; i++)
		a[i] = BIG - 1 - i;
}
function sortArray(){
	a.sort(sortorder);
}

//fix for ASPX bug 314108
@if(@aspx) expando @end function sortorder(a,b){
	return a-b
}
		
function unorder(length){
	var add1 = length;
	var add2 = length +3;
	var temp;
	if (add1> (a.length-1))
		add1 = add1%(a.length-1);
	if (add2> (a.length-1))
		add2 = add2%(a.length-1);

	if (add1 == 0){
		di++;
		if ((di%2) == 0){
			add1 = a.length-1;
			add2 = 2;
		}
	}
	temp = a[add1];
	a[add1] = a[add2];
	a[add2] = temp;
}

function ArrayEqual(Array1, Array2){
	var dIndex, Btemp = true;

	if (Array1.length != Array2.length)
		return false;
	else
	{
		for (dIndex in Array1)
			if (Array1[dIndex] != Array2[dIndex])
				Btemp = false;
		return Btemp;
	}
}

function arsort004(){
	var i;
	var a_Exp = new Array(22,24,25,27,28,32,78,99);
	var m_scen ='';
	var sCat = '';

	apInitTest("arSort004 ");

	apInitScenario("Perpetually unsorted list sorting");
	m_scen = "Perpetually unsorted list sorting";
	sCat = m_scen;
	for(i = 0; i<50; i++)
	{
		sortArray();			
		unorder(i);
	}
	sortArray();
	if (!ArrayEqual(a, a_Exp))
		apLogFailInfo(m_scen+(sCat.length?"--"+sCat:"")+" failed", a_Exp,a, "");

	apInitScenario("Perpetually Sorted list sorting");
	m_scen = "Perpetually Sorted list sorting";
	sCat = m_scen;
	for(i = 0; i<50; i++)
	{
		sortArray();			
	}
	sortArray();
	if (!ArrayEqual(a, a_Exp))
		apLogFailInfo(m_scen+(sCat.length?"--"+sCat:"")+" failed", a_Exp,a, "");

	apInitScenario("Sort 0 size array");
	var ar1 = new Array();
	ar1.sort();
	if(ar1.length != 0)
		apLogFailInfo("sort failed.",0,ar1.length(),null);

	apInitScenario("Sort BIG size array");
	var ar2 = new Array();
	makeAscending(ar1);
	makeDescending(ar2);
	ar2.sort(sortorder);
	if(!ArrayEqual(ar1,ar2))
		apLogFailInfo("sort failed.",0,ar1.length(),null);

/*****************************************************************************/
    apEndTest();
}



arsort004();


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

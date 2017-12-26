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


var iTestID = 204864;

/*----------------------------------------
Test: dotnet05
Product:  JScript
Area: 	System.Collections.
Purpose: make sure some basic functionality works.
Notes: 

-----------------------------------
---------------------------------------------------*/


var sAct="";
var sExp = "";
var sActErr = "";
var sExpErr = "";
var sErrThr = "";

function verify(sRes, sExp, sMsg, sBug)
{
	if (sRes != sExp)
		apLogFailInfo(sMsg, sExp, sRes, sBug);
}


@if(!@aspx)
	import System.Collections;
@else
	</script>
	<%@ import namespace="System.Collections" %>
	<script language=jscript runat=server>
@end

function dotnet05() 
{
    		
	apInitTest (" dotnet05 : System.Collections");

//////////////////// ArrayList//////////////////////////////////////	
	

	apInitScenario ("1.1 add key Array List");
	var arrlist: ArrayList = new ArrayList();
	arrlist.Add("Script");
	
	verify(arrlist.BinarySearch("Script"),0,"1.1 add key Array List","");
	
	apInitScenario ("1.2 check count of Array List");
	var str :String= new String("ABC");
	arrlist.Add(str);
	verify(arrlist.Count,2,"1.2 check count of Array List","");

	apInitScenario ("1.3 sort key Array List");
	arrlist.Sort();
	verify(arrlist.BinarySearch("Script"),1,"1.3 sort Array List","");

	apInitScenario ("1.4 remove key Array List");
	arrlist.RemoveAt(1);
	verify(arrlist.Count,1,"1.4 sort Array List","");

	
	apInitScenario ("1.5 default index to Array List");
	arrlist[0] = -99;
	verify(arrlist[0],-99,"1.5 default index to Array List","");
	
//////////////////Hashtable ///////////////////////////////////////////

	apInitScenario ("2.1 add key to Hashtable");
	var h: Hashtable = new Hashtable;
	h.Add("test","1");
	verify(h.Contains("test"),true,"2.1 add key to Hashtable","");
	
	h.Add("Dev","2");
	apInitScenario ("2.2 access Hashtable through default index ");
	h("PM") = 3;
	verify(h("Dev"),2,"2.2 access Hashtable through default index ","");

	apInitScenario ("2.3 access Hashtable through [] notation");
	h["LOC"] = 4;
	verify(h["LOC"],4,"2.3 access Hashtable through [] notation","");
	
	apInitScenario ("2.4 check containsvalue");
	verify(h.ContainsValue(3),true,"2.4 check containsvalue","");
	
	apInitScenario ("2.5 remove key from hashtable");
	h.Remove("LOC");
	verify(h.ContainsKey("LOC"),false, "2.5 remove key from hashtable","");
	
	apInitScenario ("2.6 check count of hashtable");
	verify(h.Count,3,"2.6 check count of hashtable","");	

//////////////////Queue ///////////////////////////////////////////

	apInitScenario ("3.1 Enqueue ");
	var q: Queue = new Queue(10);
	q.Enqueue(99);	
	verify(q.Contains(99),true,"3.1 Enqueue","");

	q.Enqueue("This is a test");
	verify(q.Contains("This is a test"),true,"3.1 Enqueue","");	

	apInitScenario ("3.2 Dequeue ");
	var x = q.Dequeue();
	verify(x,99,"3.2 Dequeue ","");

	
	apInitScenario ("3.3 Count of queue ");
	q.Enqueue(new Number(123.123));
	q.Enqueue(new RegExp(/^HTML/));
	verify(q.Count,3,"3.3 Count of queue","");

	apInitScenario ("3.4 ToArray");
	var arr: Array = new Array();
	arr = q.ToArray();
	verify(arr[0],"This is a test","3.4 ToArray","");
	verify(arr[1],123.123,"3.4 ToArray","");

//////////////////Stack ///////////////////////////////////////////	
	var stk: Stack = new Stack();
	
	apInitScenario ("4.1 push into Stack");
	stk.Push(NaN);
	var obj = new String("1234");
	stk.Push(obj);
	verify(stk.Contains(obj),true,"4.1 push into Stack","");
	verify(stk.Contains(NaN),true,"4.1 push into Stack","");
	
	apInitScenario ("4.2 pop up Stack");
	var x = stk.Pop();
	verify(x.ToString(),"1234","4.2 pop up Stack","");
	var y = stk.Pop();
	verify(isNaN(y), true,	"4.2 pop up Stack","");
	
	apInitScenario ("4.3 Clone stack");
	stk.Push(new Number(34));
	stk.Push(new Date(2001,11,25));
	var stk1: Stack = new Stack();
	stk1 = stk.Clone();
	verify(stk1.Count,2,"4.3 Clone stack","");

	apEndTest();

}



dotnet05();


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

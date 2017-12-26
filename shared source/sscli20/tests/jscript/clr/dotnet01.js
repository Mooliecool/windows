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


var iTestID = 204860;

/*-----------------------------------------
Test: dotnet01 
Product:  JScript
Area: 	System
Purpose: check some basic functionality for Buffer,Console,Convert etc.
Notes: 

-----------------------------------
[00] 31-7-2001
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
	import System
	import System.IO
@else
	</script>
	<%@ import namespace="System.IO" %>
	<script language=jscript runat=server>
@end

function dotnet01() {
	apInitTest (" dotnet01 : test using dotnet type");

	apInitScenario ("1.using System.Buffer");

@if(@aspx)
	var iarr: Byte[] = [0,1,2,3,4];
	var iarr1: Byte[] = [5,6,7,8,9,10];
	for (var i=0; i<5;i++) {
		sExp = i;
		sAct = System.Buffer.GetByte(iarr,i);
		verify(sAct,sExp,"1.using System.Buffer","");
	}
	var x = System.Buffer.BlockCopy(iarr,1,iarr1,1,3);
	for (i = 1; i<=3;i++){
		sExp = iarr[i];
		sAct = iarr1[i];
		verify (sAct,sExp,"1.using System.Buffer","");
	}
@else
	var iarr: Byte[] = [0,1,2,3,4];
	var iarr1: Byte[] = [5,6,7,8,9,10];
	for (var i=0; i<5;i++) {
		sExp = i;
		sAct = Buffer.GetByte(iarr,i);
		verify(sAct,sExp,"1.using System.Buffer","");
	}
	var x = Buffer.BlockCopy(iarr,1,iarr1,1,3);
	for (i = 1; i<=3;i++){
		sExp = iarr[i];
		sAct = iarr1[i];
		verify (sAct,sExp,"1.using System.Buffer","");
	}
@end
	apInitScenario ("2. using Console to call writeline");
	var _boolean : Boolean = true;
	var _char :char[]= ['a','b','c',123];
	//var _decimal :decimal = Decimal.MaxValue; 
       var _decimal : decimal = 123.456
	var _string:String = new String("test");
	var _single  :float= Single.MaxValue;
	var _double :double = Double.MaxValue;
	var _uint: uint = Char.MaxValue;
	var _date = new Date(2001,1,1);

	var fs1: FileStream = new FileStream("dot01reu.txt",FileMode.Create);
	var tmp : TextWriter = Console.Out;
	var sw1: StreamWriter = new StreamWriter(fs1);
	Console.SetOut(sw1);
	Console.WriteLine(_boolean);
	Console.WriteLine(_char);
	Console.WriteLine(_decimal);
	Console.WriteLine(_single);
	Console.WriteLine(_double);
	Console.WriteLine(_uint);
	Console.WriteLine(_date);
	sw1.Close();
	fs1.Close()
	Console.SetOut(tmp);
        
        if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {

	apInitScenario ("3. using Console to call readline");
	var fs2: FileStream = new FileStream("dot01reu.txt",FileMode.Open);
	var sr2 : StreamReader = new StreamReader(fs2);
	Console.SetIn(sr2);
	sExp = "True";
	sAct = Console.ReadLine();
	verify(sAct,sExp,"3.1 using Console to call readline","");
	sExp = "abc{";
	sAct = Console.ReadLine();
	verify(sAct,sExp,"3.2 using Console to call readline","");
	//sExp = Decimal.MaxValue.ToString(); //bug 313503
       sExp = _decimal.ToString();
	sAct = Console.ReadLine();
	verify(sAct,sExp,"3.3 using Console to call readline","");
	sExp = Single.MaxValue.ToString();
	sAct = Console.ReadLine();
	verify(sAct,sExp,"3.4 using Console to call readline","");
	sExp = Double.MaxValue.ToString();
	sAct = Console.ReadLine();
	verify(sAct,sExp,"3.5 using Console to call readline","");
	sExp = 65535;
	sAct = Console.ReadLine();
	verify(sAct,sExp, "3.6 using Console to call readline","");
	sExp = "Thu Feb 1 00:00:00 PST 2001";
	sAct = Console.ReadLine();
	verify(sAct,sExp, "3.7 using Console to call readline","");
	}

	apInitScenario ("4. using Convert");
	var _decimal4: decimal = decimal(3432123);
	var _sbyte4 = sbyte.MinValue +128.00;
	var _ushort: ushort = ushort.MinValue;
	sExp = true;
	sAct = Convert.ToBoolean(_decimal4);
	verify(sAct,sExp, "4.1 using Convert.ToBoolean","");
	sExp = false;
	sAct = Convert.ToBoolean(_sbyte4);
	verify(sAct,sExp, "4.2 using Convert.ToBoolean","");

	var _number = Number.NEGATIVE_INFINITY;
	sExp = true;
	sAct = Convert.ToBoolean(_number);
	verify(sAct,sExp, "4.3 using Convert.ToBoolean","")

	apInitScenario ("5. using DateTime");
	var jan1: DateTime = new DateTime(2001,1,1);
	var jan01 :Date= new Date(2001,1,1);
	var july5: Date = new Date(2001,6,5);
	sExp = true;
	sAct = (july5 >= jan1);
	verify(sAct,sExp,"5.2 using DateTime","");
	sExp = true;
	sAct = (jan01 <july5 );
	verify(sAct,sExp,"5.2 using DateTime","");

	apInitScenario ("6. using Environment");
	sExp = "at System.Environment.GetStackTrace(Exception e, Boolean needFileInfo)";
	sAct = Environment.StackTrace;
	if (sAct.indexOf(sExp)< 0)
		apLogFailInfo("6. using Environment",sExp,sAct,"");
	

	apInitScenario ("7.using Operating System");
@if(!@rotor)
	var _ope : OperatingSystem = new OperatingSystem(PlatformID.Win32NT, new Version("5.0"));
	sExp = "Win32NT";
	sAct = _ope.Platform;
	verify(sAct,sExp,"8.using OperatingSystem","");
@end
	apInitScenario ("8. using Random");
	var _ran : Random = new Random();
	_ran.Next(Int32.MaxValue);
        /* This depends on unspecied behavior
	var _ran1 = new Random(Int32(Number.POSITIVE_INFINITY)); */
	
	
	apEndTest();

}


dotnet01();


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

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


var iTestID = 51724;


function verify(sCat, ob, sExp, bugNum) {
    if (bugNum == null) bugNum = "";

    var obj = new Object();
    obj.mem = ob;

    var sAct = Math.abs(obj.mem);

    if (sAct + "" != sExp + "")
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct,bugNum);
}

function obFoo() {}

var m_scen = "";

function abs003() {

@cc_on
@if (@_fast)
    var vNum, iPow, nMin, sNum, nNum;
@end


    apInitTest("abs003 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("min pos: 1",1,1, null);

    for (var i=0; i<20; i++)
    {
        vNum = Math.floor(Math.random()*(32767-2))+2; // gives int num between 2 and 32766
        verify("min pos < n < max pos: VT_I2",vNum,vNum, null);
    }
    
    verify("min pos < n < max pos: VT_I2 max",32767,32767, null);

    verify("min pos < n < max pos: VT_I4 excl-min",32768,32768, null);

    for (i=0; i<20; i++)
    {
        vNum = Math.floor(Math.random()*(2147483647-32769))+32769; // gives int num between 32769 and 2147483646
        verify("min pos < n < max pos: VT_I4",vNum,vNum, null);
    }

    verify("max pos",2147483647,2147483647, null);


    verify("max neg: -1",-1,1, null);

    for (i=0; i<20; i++)
    {
        vNum = Math.floor(Math.random()*(-32768+2))-2; // gives int num between -2 and -32767
        verify("min neg < n < max neg: VT_I2",vNum,-vNum, null);
    }
    
    verify("min neg < n < max neg: VT_I2 min",-32768,32768, null);

    verify("min neg < n < max neg: VT_I4 excl-max",-32769,32769, null);

    for (i=0; i<20; i++)
    {
        vNum = Math.floor(Math.random()*(-2147483648+32770))-32770; // gives int num between -32770 and -2147483647
        verify("min pos < n < max pos: VT_I4",vNum,-vNum, null);
    }

    verify("min neg: -2147483648",-2147483648,2147483648, null);

    verify("zero",0,0, null);


    //----------------------------------------------------------------------------
    apInitScenario("2. number, decimal, float");

    m_scen = "number, decimal, float";

    verify("min pos: 2.2250738585072014e-308",2.2250738585072014e-308, 2.2250738585072014e-308, null);

    for (vNum=iPow=0;;iPow+=5)
    {
        nMin = iPow>=15 ? Math.pow(2,iPow-15) : 0;
        vNum = (Math.random()*(Math.pow(2,iPow)-nMin))+nMin;
	    if (vNum<1.7976931348623158e308)
		    verify("min pos < n < max pos",vNum,vNum, null);
	    else
		    break;
    }    

    verify("max pos",1.7976931348623158e308,1.7976931348623158e308, null);
    verify("> max pos float (1.#INF)",1.797693134862315807e309,1.797693134862315807e309, null);

    verify("max neg",-2.2250738585072012595e-308,2.2250738585072012595e-308, null);

    for (vNum=iPow=0;;iPow+=5)
    {
        nMin = iPow>=15 ? Math.pow(2,iPow-15) : 0;
        vNum = -(Math.random()*(Math.pow(2,iPow)-nMin))+nMin;
	    if (vNum>-1.797693134862315807e308 && vNum <= 0)
		    verify("max neg < n < min neg",vNum,-vNum, null);
	    else
		    break;
    }    

    verify("min neg",-1.797693134862315807e308, 1.797693134862315807e308, null);
    verify("< min neg float (-1.#INF)",-1.797693134862315807e309,1.797693134862315807e309, null);

    verify("zero",0.0,0, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. number, hexidecimal");

    m_scen = "number, hexidecimal";

    verify("min pos: 0x1",0x1,0x1, null);
    verify("min pos < n < max pos: 0xC",0xC,0xC, null);
    verify("min pos < n < max pos: 0x14",0x14,0x14, null);
    verify("min pos < n < max pos: 0x562",0x562,0x562, null);
    verify("min pos < n < max pos: 0x9a40",0x9a40,0x9a40, null);
    verify("min pos < n < max pos: 0xD4Bb9",0xD4Bb9,0xD4Bb9, null);
    verify("min pos < n < max pos: 0x17020a",0x17020a,0x17020a, null);
    verify("min pos < n < max pos: 0x269aaf0",0x269aaf0,0x269aaf0, null);
    verify("min pos < n < max pos: 0xA7a864cf",0xA7a864cf,0xA7a864cf, null);
    verify("max pos: 0xffffffff",0xffffffff,0xffffffff, null);

    verify("max neg: -0x1",-0x1,0x1, null);
    verify("min neg < n < max neg: -0x2",-0x2,0x2, null);
    verify("min neg < n < max neg: -0xfC",-0xfC,0xfC, null);
    verify("min neg < n < max neg: -0xC07",-0xC07,0xC07, null);
    verify("min neg < n < max neg: -0x54e6",-0x54e6,0x54e6, null);
    verify("min neg < n < max neg: -0x41061",-0x41061,0x41061, null);
    verify("min neg < n < max neg: -0xb15870",-0xb15870,0xb15870, null);
    verify("min neg < n < max neg: -0xf6BfE57",-0xf6BfE57,0xf6BfE57, null);
    verify("min neg < n < max neg: -0xA347599c",-0xA347599c,0xA347599c, null);
    verify("min neg: -0xffffffff",-0xffffffff,0xffffffff, null);

    verify("zero",0x0,0x0, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. number, octal");

    m_scen = "number, octal";

    verify("min pos",01,01, null);
    verify("min pos < n < max pos: 02",02, 02, null);
    verify("min pos < n < max pos: 072",072, 072, null);
    verify("min pos < n < max pos: 0461",0461, 0461, null);
    verify("min pos < n < max pos: 06706",06706, 06706, null);
    verify("min pos < n < max pos: 050661",050661, 050661, null);
    verify("min pos < n < max pos: 0302611",0302611, 0302611, null);
    verify("min pos < n < max pos: 02303116",02303116, 02303116, null);
    verify("min pos < n < max pos: 042152134",042152134, 042152134, null);
    verify("min pos < n < max pos: 0324413222",0324413222, 0324413222, null);
    verify("min pos < n < max pos: 05766757103",05766757103, 05766757103, null);
    verify("min pos < n < max pos: 021013604067",021013604067, 021013604067, null);
    verify("max pos: 037777777777",037777777777, 037777777777, null);

    verify("max neg: -01",-01, 01, null);
    verify("min neg < n < max neg: -07",-07, 07, null);
    verify("min neg < n < max neg: -021",-021, 021, null);
    verify("min neg < n < max neg: -0332",-0332, 0332, null);
    verify("min neg < n < max neg: -07777",-07777, 07777, null);
    verify("min neg < n < max neg: -044221",-044221, 044221, null);
    verify("min neg < n < max neg: -0201615",-0201615, 0201615, null);
    verify("min neg < n < max neg: -04506536",-04506536, 04506536, null);
    verify("min neg < n < max neg: -015257715",-015257715, 015257715, null);
    verify("min neg < n < max neg: -0521544023",-0521544023, 0521544023, null);
    verify("min neg < n < max neg: -02330412042",-02330412042, 02330412042, null);
    verify("min neg < n < max neg: -035164014173",-035164014173, 035164014173, null);
    verify("min neg: -037777777777",-037777777777, 037777777777, null);

    verify("pos zero",00, 00, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("5. num string, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("min pos: 1","1",1, null);

    for (i=0; i<20; i++)
    {
        vNum = ""+(Math.floor(Math.random()*(32767-2))+2); // gives int num between 2 and 32766
        verify("min pos < n < max pos: VT_I2",vNum,vNum, null);
    }
    
    verify("min pos < n < max pos: VT_I2 max","32767",32767, null);

    verify("min pos < n < max pos: VT_I4 excl-min","32768",32768, null);

    for (i=0; i<20; i++)
    {
        vNum = ""+(Math.floor(Math.random()*(2147483647-32769))+32769); // gives int num between 32769 and 2147483646
        verify("min pos < n < max pos: VT_I4",vNum,vNum, null);
    }

    verify("max pos","2147483647",2147483647, null);


    verify("max neg: -1","-1",1, null);

    for (i=0; i<20; i++)
    {
        vNum = ""+(Math.floor(Math.random()*(-32768+2))-2); // gives int num between -2 and -32767
        verify("min neg < n < max neg: VT_I2",vNum,-vNum, null);
    }
    
    verify("min neg < n < max neg: VT_I2 min","-32768",32768, null);

    verify("min neg < n < max neg: VT_I4 excl-max","-32769",32769, null);

    for (i=0; i<20; i++)
    {
        vNum = ""+(Math.floor(Math.random()*(-2147483648+32770))-32770); // gives int num between -32770 and -2147483647
        verify("min pos < n < max pos: VT_I4",vNum,-vNum, null);
    }

    verify("min neg: -2147483648","-2147483648",2147483648, null);

    verify("zero","0",0, null);


    //----------------------------------------------------------------------------
    apInitScenario("6. num string, decimal, float");

    m_scen = "num string, decimal, float";

    verify("min pos: 2.2250738585072014e-308","2.2250738585072014e-308", 2.2250738585072014e-308, null);

    for (vNum=iPow=0;;iPow+=5)
    {
        nMin = iPow>=15 ? Math.pow(2,iPow-15) : 0;
        // string conversion and return to num through eval is necessary because of how we've spec'd precision
        sNum = ""+((Math.random()*(Math.pow(2,iPow)-nMin))+nMin);
        nNum = eval(sNum);
	    if (nNum<1.7976931348623158e308)
		    verify("min pos < n < max pos",sNum,nNum, null);
	    else
		    break;
    }    

    verify("max pos","1.7976931348623158e308",1.7976931348623158e308, null);
    verify("> max pos float (1.#INF)","1.797693134862315807e309",1.797693134862315807e309, null);

    for (vNum=iPow=0;;iPow+=5)
    {
        nMin = iPow>=15 ? Math.pow(2,iPow-15) : 0;
        sNum = ""+ -((Math.random()*(Math.pow(2,iPow)-nMin))+nMin);
        nNum = eval(sNum);
	    if (nNum>-1.797693134862315807e308 && nNum <= 0)
		    verify("max neg < n < min neg",sNum,-nNum, null);
	    else
		    break;
    }    

    verify("min neg","-1.797693134862315807e308", 1.797693134862315807e308, null);
    verify("< min neg float (-1.#INF)","-1.797693134862315807e309",1.797693134862315807e309, null);

    verify("zero","0.0",0, null);


    //----------------------------------------------------------------------------
    apInitScenario("8. num string, octal");

    m_scen = "num string, octal";

    verify("min pos","01",1, null);
    verify("min pos < n < max pos: 02","02", 2, null);
    verify("min pos < n < max pos: 072","072", 72, null);
    verify("min pos < n < max pos: 0461","0461", 461, null);
    verify("min pos < n < max pos: 06706","06706", 6706, null);
    verify("min pos < n < max pos: 050661","050661", 50661, null);
    verify("min pos < n < max pos: 0302611","0302611", 302611, null);
    verify("min pos < n < max pos: 02303116","02303116", 2303116, null);
    verify("min pos < n < max pos: 042152134","042152134", 42152134, null);
    verify("min pos < n < max pos: 0324413222","0324413222", 324413222, null);
    verify("min pos < n < max pos: 05766757103","05766757103", 5766757103, null);
    verify("min pos < n < max pos: 021013604067","021013604067", 21013604067, null);
    verify("max pos: 037777777777","037777777777", 37777777777, null);

    verify("max neg: -01","-01", 1, null);
    verify("min neg < n < max neg: -07","-07", 7, null);
    verify("min neg < n < max neg: -021","-021", 21, null);
    verify("min neg < n < max neg: -0332","-0332", 332, null);
    verify("min neg < n < max neg: -07777","-07777", 7777, null);
    verify("min neg < n < max neg: -044221","-044221", 44221, null);
    verify("min neg < n < max neg: -0201615","-0201615", 201615, null);
    verify("min neg < n < max neg: -04506536","-04506536", 4506536, null);
    verify("min neg < n < max neg: -015257715","-015257715", 15257715, null);
    verify("min neg < n < max neg: -0521544023","-0521544023", 521544023, null);
    verify("min neg < n < max neg: -02330412042","-02330412042", 2330412042, null);
    verify("min neg < n < max neg: -035164014173","-035164014173", 35164014173, null);
    verify("min neg: -037777777777","-037777777777", 37777777777, null);

    verify("zero","00", 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("13. constants");

    m_scen = "constants";

    verify("true",true,1, null);
    verify("false",false,0, null);


    //----------------------------------------------------------------------------
    apInitScenario("14. null");

    m_scen = "null";

    verify("",null,0, null);


    /*****************************************************************************
    // All of the rest cause a runtime error (spec) of 'not a number'

    //----------------------------------------------------------------------------
    // apInitScenario("n. Alpha string");
    

    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, built-in, non-exec");


    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, built-in, exec, not inst");


    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, built-in, exec, inst");


    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, user-def, not inst");


    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, user-def, inst");


    //----------------------------------------------------------------------------
//    apInitScenario("n. undefined");
    *****************************************************************************/
    

    apEndTest();

}


abs003();


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

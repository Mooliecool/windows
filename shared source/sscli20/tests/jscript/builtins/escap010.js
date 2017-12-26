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


var iTestID = 52604;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function escap010() {

    apInitTest( "escap010 ");

    var sExp, sAct;
    var sMask, sEscRnd, afEscRnd, iPos;

    //--------------------------------------------------------------------
    apInitScenario( "1. zls, null mask");
    
    sExp = '';
    sAct = escape( eval("''") );

    if (sAct != sExp)
        apLogFailInfo( "zls, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "2. single space, null mask");
    
    sExp = "%20";
    sAct = escape( eval("' '") );

    if (sAct != sExp)
        apLogFailInfo( "single space, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "3. multiple spaces, null mask");
    
    sExp = "%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20";
    sAct = escape( eval("'                  '") );

    if (sAct != sExp)
        apLogFailInfo( "multiple spaces, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "4. single char uc alpha, null mask");

    sExp = 'M';
    sAct = escape( eval("'M'") );

    if (sAct != sExp)
        apLogFailInfo( "single char uc alpha, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "5. single char lc alpha, null mask");

    sExp = 's';
    sAct = escape( eval("'s'") );

    if (sAct != sExp)
        apLogFailInfo( "single char lc alpha, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "6. single char misc, null mask");

    sExp = '%23';
    sAct = escape( eval("'#'") );

    if (sAct != sExp)
        apLogFailInfo( "single char misc, null mask failed", sExp, sAct, "");

    sExp = '%21';
    sAct = escape( eval("'!'") );

    if (sAct != sExp)
        apLogFailInfo( "single char misc, null mask failed", sExp, sAct, "");

    sExp = '%5B';
    sAct = escape( eval("'['") );

    if (sAct != sExp)
        apLogFailInfo( "single char misc, null mask failed", sExp, sAct, "");

    sExp = '@';
    sAct = escape( eval("'@'") );

    if (sAct != sExp)
        apLogFailInfo( "single char misc, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "7. single char num, null mask");

    sExp = "4";
    sAct = escape( eval("'4'") );

    if (sAct != sExp)
        apLogFailInfo( "single char num, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "8. repeated char uc alpha, null mask");

    sExp = 'QQQQQQQQQQQQQQQQQQQQQQQQ';
    sAct = escape( eval("'QQQQQQQQQQQQQQQQQQQQQQQQ'") );

    if (sAct != sExp)
        apLogFailInfo( "repeated char uc alpha, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "9. repeated char lc alpha, null mask");

    sExp = 'pppppppppppppppppppppppppppppppp';
    sAct = escape( eval("'pppppppppppppppppppppppppppppppp'") );

    if (sAct != sExp)
        apLogFailInfo( "repeated char lc alpha, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "10. repeated char misc, null mask");

    sExp = '%23%23%23%23%23%23%23%23%23%23';
    sAct = escape( eval("'##########'") );

    if (sAct != sExp)
        apLogFailInfo( "repeated char misc, null mask failed", sExp, sAct, "");

    sExp = '%21%21%21%21%21%21%21%21%21%21%21%21%21%21';
    sAct = escape( eval("'!!!!!!!!!!!!!!'") );

    if (sAct != sExp)
        apLogFailInfo( "repeated char misc, null mask failed", sExp, sAct, "");

    sExp = '%5B%5B%5B%5B';
    sAct = escape( eval("'[[[['") );

    if (sAct != sExp)
        apLogFailInfo( "repeated char misc, null mask failed", sExp, sAct, "");

    sExp = '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@';
    sAct = escape( eval("'@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'") );

    if (sAct != sExp)
        apLogFailInfo( "repeated char misc, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "11. repeated char num, null mask");

    sExp = "11111111111111111111111111111111111";
    sAct = escape( eval("'11111111111111111111111111111111111'") );

    if (sAct != sExp)
        apLogFailInfo( "repeated char num, null mask failed", sExp, sAct, "");

    sExp = "0000000000000000000000000000";
    sAct = escape( eval("'0000000000000000000000000000'") );

    if (sAct != sExp)
        apLogFailInfo( "repeated char num, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "12. single num, null mask");

    sExp = 0+"";
    sAct = escape( eval("0") );

    if (sAct != sExp)
        apLogFailInfo( "single num, null mask failed", sExp, sAct, "");

    sExp = 9+"";
    sAct = escape( eval("9") );

    if (sAct != sExp)
        apLogFailInfo( "single num, null mask failed", sExp, sAct, "");

    sExp = -1234567890+"";
    sAct = escape( eval("-1234567890") );

    if (sAct != sExp)
        apLogFailInfo( "single num, null mask failed", sExp, sAct, "");

    sExp = -0xFFFFFFFF+"";
    sAct = escape( eval("-0xFFFFFFFF") );

    if (sAct != sExp)
        apLogFailInfo( "single num, null mask failed", sExp, sAct, "");

    sExp = 0x2468ACE+"";
    sAct = escape( eval("0x2468ACE") );

    if (sAct != sExp)
        apLogFailInfo( "single num, null mask failed", sExp, sAct, "");

    sExp = 037777777777+"";
    sAct = escape( eval("037777777777") );

    if (sAct != sExp)
        apLogFailInfo( "single num, null mask failed", sExp, sAct, "");

    sExp = -01234567+"";
    sAct = escape( eval("-01234567") );

    if (sAct != sExp)
        apLogFailInfo( "single num, null mask failed", sExp, sAct, "");

    sExp = 037777777777+"";
    sAct = escape( eval("037777777777") );

    if (sAct != sExp)
        apLogFailInfo( "single num, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "13. constant, null mask");

    sExp = "true";
    sAct = escape( eval("true") );

    if (sAct != sExp)
        apLogFailInfo( "constant true, null mask failed", sExp, sAct, "");

    sExp = "false";
    sAct = escape( eval("false") );

    if (sAct != sExp)
        apLogFailInfo( "constant false, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "14. null, null mask");

    sExp = "null";
    sAct = escape( eval("null") );

    if (sAct != sExp)
        apLogFailInfo( "null, null mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("15. built-in obj, non-exec, null mask");

//    sExp = "%0Afunction%20Math%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A";
    sExp = "%5Bobject%20Math%5D";
    sAct = escape( eval("Math") );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, non-exec, null mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("16. built-in obj, exec, not instanciated, null mask");

    @cc_on
    if (parseFloat(@_jscript_version)>=7)
      sExp = "function%20Array%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
    else
      sExp = "%0Afunction%20Array%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A";

    sAct = escape( eval("Array") );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, exec, not inst--Array, null mask failed", sExp, sAct, "");

    if (parseFloat(@_jscript_version)>=7)
      sExp = "function%20Date%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
    else
      sExp = "%0Afunction%20Date%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A";
    sAct = escape( Date );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, exec, not inst--Date, null mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("17. built-in obj, exec, instanciated, null mask");

    sExp = "false";
    sAct = escape( eval("new Boolean()") );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, exec inst--new Boolean(), null mask failed", sExp, sAct, "");

    sExp = "0";
    sAct = escape( eval("new Number()") );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, exec inst--new Number(), null mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("18. user-defined obj, not instanciated, null mask");


@if(!@aspx)
    sExp = "function%20obFoo%28%29%20%7B%7D"
@else
    sExp = "expando%20function%20obFoo%28%29%20%7B%7D"
@end

    sAct = escape( eval("obFoo") );

    if (sAct != sExp)
        apLogFailInfo( "user-defined obj, not inst, null mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("19. user-defined obj, instanciated, null mask");

    sExp = "%5Bobject%20Object%5D"
    sAct = escape( eval("new obFoo()") );

    if (sAct != sExp)
        apLogFailInfo( "user-defined obj, inst, null mask failed", sExp, sAct, "");


    apEndTest();

}



escap010();


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

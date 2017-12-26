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


var iTestID = 52600;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function escap006() {

    apInitTest( "escap006 ");

    var sExp, sAct;
    var sMask, sEscRnd, afEscRnd, iPos;

    //--------------------------------------------------------------------
    apInitScenario( "1. zls, 4 mask");
    
    sExp = '';
    sAct = escape( '', 4 );

    if (sAct != sExp)
        apLogFailInfo( "zls, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "2. single space, 4 mask");
    
    sExp = "%20";
    sAct = escape( ' ', 4 );

    if (sAct != sExp)
        apLogFailInfo( "single space, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "3. multiple spaces, 4 mask");
    
    sExp = "%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20";
    sAct = escape( "                  ", 4 );

    if (sAct != sExp)
        apLogFailInfo( "multiple spaces, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "4. single char uc alpha, 4 mask");

    sExp = 'M';
    sAct = escape( 'M', 4 );

    if (sAct != sExp)
        apLogFailInfo( "single char uc alpha, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "5. single char lc alpha, 4 mask");

    sExp = 's';
    sAct = escape( 's', 4 );

    if (sAct != sExp)
        apLogFailInfo( "single char lc alpha, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "6. single char misc, 4 mask");

    sExp = '%23';
    sAct = escape( '#', 4 );

    if (sAct != sExp)
        apLogFailInfo( "single char misc, 4 mask failed", sExp, sAct, "");

    sExp = '%21';
    sAct = escape( '!', 4 );

    if (sAct != sExp)
        apLogFailInfo( "single char misc, 4 mask failed", sExp, sAct, "");

    sExp = '%5B';
    sAct = escape( '[', 4 );

    if (sAct != sExp)
        apLogFailInfo( "single char misc, 4 mask failed", sExp, sAct, "");

    sExp = '@';
    sAct = escape( '@', 4 );

    if (sAct != sExp)
        apLogFailInfo( "single char misc, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "7. single char num, 4 mask");

    sExp = "4";
    sAct = escape( "4", 4 );

    if (sAct != sExp)
        apLogFailInfo( "single char num, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "8. repeated char uc alpha, 4 mask");

    sExp = 'QQQQQQQQQQQQQQQQQQQQQQQQ';
    sAct = escape( 'QQQQQQQQQQQQQQQQQQQQQQQQ', 4 );

    if (sAct != sExp)
        apLogFailInfo( "repeated char uc alpha, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "9. repeated char lc alpha, 4 mask");

    sExp = 'pppppppppppppppppppppppppppppppp';
    sAct = escape( 'pppppppppppppppppppppppppppppppp', 4 );

    if (sAct != sExp)
        apLogFailInfo( "repeated char lc alpha, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "10. repeated char misc, 4 mask");

    sExp = '%23%23%23%23%23%23%23%23%23%23';
    sAct = escape( '##########', 4 );

    if (sAct != sExp)
        apLogFailInfo( "repeated char misc, 4 mask failed", sExp, sAct, "");

    sExp = '%21%21%21%21%21%21%21%21%21%21%21%21%21%21';
    sAct = escape( '!!!!!!!!!!!!!!', 4 );

    if (sAct != sExp)
        apLogFailInfo( "repeated char misc, 4 mask failed", sExp, sAct, "");

    sExp = '%5B%5B%5B%5B';
    sAct = escape( '[[[[', 4 );

    if (sAct != sExp)
        apLogFailInfo( "repeated char misc, 4 mask failed", sExp, sAct, "");

    sExp = '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@';
    sAct = escape( '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@', 4 );

    if (sAct != sExp)
        apLogFailInfo( "repeated char misc, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "11. repeated char num, 4 mask");

    sExp = "11111111111111111111111111111111111";
    sAct = escape( "11111111111111111111111111111111111" );

    if (sAct != sExp)
        apLogFailInfo( "repeated char num, 4 mask failed", sExp, sAct, "");

    sExp = "0000000000000000000000000000";
    sAct = escape( "0000000000000000000000000000", 4 );

    if (sAct != sExp)
        apLogFailInfo( "repeated char num, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "12. single num, 4 mask");

    sExp = 0+"";
    sAct = escape( 0, 4 );

    if (sAct != sExp)
        apLogFailInfo( "single num, 4 mask failed", sExp, sAct, "");

    sExp = 9+"";
    sAct = escape( 9, 4 );

    if (sAct != sExp)
        apLogFailInfo( "single num, 4 mask failed", sExp, sAct, "");

    sExp = -1234567890+"";
    sAct = escape( -1234567890, 4 );

    if (sAct != sExp)
        apLogFailInfo( "single num, 4 mask failed", sExp, sAct, "");

    sExp = -0xFFFFFFFF+"";
    sAct = escape( -0xFFFFFFFF, 4 );

    if (sAct != sExp)
        apLogFailInfo( "single num, 4 mask failed", sExp, sAct, "");

    sExp = 0x2468ACE+"";
    sAct = escape( 0x2468ACE, 4 );

    if (sAct != sExp)
        apLogFailInfo( "single num, 4 mask failed", sExp, sAct, "");

    sExp = 037777777777+"";
    sAct = escape( 037777777777, 4 );

    if (sAct != sExp)
        apLogFailInfo( "single num, 4 mask failed", sExp, sAct, "");

    sExp = -01234567+"";
    sAct = escape( -01234567, 4 );

    if (sAct != sExp)
        apLogFailInfo( "single num, 4 mask failed", sExp, sAct, "");

    sExp = 037777777777+"";
    sAct = escape( 037777777777, 4 );

    if (sAct != sExp)
        apLogFailInfo( "single num, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "13. constant, 4 mask");

    sExp = "true";
    sAct = escape( true, 4 );

    if (sAct != sExp)
        apLogFailInfo( "constant true, 4 mask failed", sExp, sAct, "");

    sExp = "false";
    sAct = escape( false, 4 );

    if (sAct != sExp)
        apLogFailInfo( "constant false, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "14. null, 4 mask");

    sExp = "null";
    sAct = escape( null, 4 );

    if (sAct != sExp)
        apLogFailInfo( "null, 4 mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("15. built-in obj, non-exec, 4 mask");

//    sExp = "%0Afunction%20Math%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A";
    sExp = "%5Bobject%20Math%5D";
    sAct = escape( Math, 4 );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, non-exec, 4 mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("16. built-in obj, exec, not instanciated, 4 mask");

    @cc_on
    if (parseFloat(@_jscript_version)>=7)
      sExp = "function%20Array%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
    else
      sExp = "%0Afunction%20Array%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A";
    sAct = escape( Array, 4 );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, exec, not inst--Array, 4 mask failed", sExp, sAct, "");

    if (parseFloat(@_jscript_version)>=7)
      sExp = "function%20Date%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
    else
      sExp = "%0Afunction%20Date%28%29%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A";
    sAct = escape( Date, 4 );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, exec, not inst--Date, 4 mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("17. built-in obj, exec, instanciated, 4 mask");

    sExp = "false";
    sAct = escape( new Boolean(), 4 );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, exec inst--new Boolean(), 4 mask failed", sExp, sAct, "");

    sExp = "0";
    sAct = escape( new Number(), 4 );

    if (sAct != sExp)
        apLogFailInfo( "built-in obj, exec inst--new Number(), 4 mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("18. user-defined obj, not instanciated, 4 mask");

@if(!@aspx)
    sExp = "function%20obFoo%28%29%20%7B%7D"
@else
    sExp = "expando%20function%20obFoo%28%29%20%7B%7D"
@end
    sAct = escape( obFoo, 4 );

    if (sAct != sExp)
        apLogFailInfo( "user-defined obj, not inst, 4 mask failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("19. user-defined obj, instanciated, 4 mask");

    sExp = "%5Bobject%20Object%5D"
    sAct = escape( new obFoo(), 4 );

    if (sAct != sExp)
        apLogFailInfo( "user-defined obj, inst, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "20. chars 0x20-0x7f, ordered, 4 mask");

    sExp = "%20%21%22%23%24%25%26%27%28%29*+%2C-./0123456789%3A%3B%3C%3D%3E%3F@ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D%5E_%60abcdefghijklmnopqrstuvwxyz%7B%7C%7D%7E%7F"
    sAct = escape( " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 4 );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x20-0x7f, ordered, 4 mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "21. chars 0x20-0x7f, unordered, 4 mask");

    sExp = "on%5D0sdX%2CQBLW%5ElIDA@xUFwkR%3C%26hb%23za4E7G%5B%255cVS%3FN%7FMYiq9e/%241O%2162ZP%7E%28%5C.J8%20CjK*%3E3pHg%22ftr%27yu%29%60T%7C%3A%3D-%7Dvm%3B+_%7B"
    sAct = escape( "on]0sdX,QBLW^lIDA@xUFwkR<&hb#za4E7G[%5cVS?NMYiq9e/$1O!62ZP~(\\.J8 CjK*>3pHg\"ftr'yu)`T|:=-}vm;+_{", 4 );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x20-0x7f, unordered, 4 mask failed", sExp, sAct, "");


    apEndTest();

}



escap006();


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

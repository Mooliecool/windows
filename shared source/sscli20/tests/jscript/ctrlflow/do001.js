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


var iTestID = 52238;


//                             HTML tree


var  TestDesc;
var  Counter;

function CounterNOTONE() {
  return (Counter != 1);
}


function CounterONE() {
  return (Counter == 1);
}


function DECCounterNOTZERO() {
  Counter = Counter - 1;
  return (Counter != 0);
}


function DECCounterZERO() {
  Counter = Counter - 1;
  return (Counter == 0);
}


function DECCounter() {
  Counter = Counter - 1;
  return Counter;
}


function Different (AVarName , b) {
  return (AVarName != b);
}


function NumericEq (AVarName, b) {
  return (AVarName == b);
}


function do001() 
{
    apInitTest("do001 ");

   
    apInitScenario ("1a. do-while: single line in loop");
    TestDesc = "Set 1a: ";
    var i2 = 5;

    do  {
      i2 = i2 - 1;
    } while (i2 > 0);

    if (i2 != 0) 
      apLogFailInfo(TestDesc + "do {-} while ( fails with single line in loop" ,"","","");


    TestDesc = "Set 1b: ";
    Counter = 5;
    i2 = 0;

    do  {
      i2 = i2 + 1;
    } while (DECCounter() != 0);

    if (i2 != 5 || Counter != 0)
      apLogFailInfo( TestDesc + "do {-} while fails with single line in loop" ,"","","");


    TestDesc = "Set 1c: ";
    Counter = 5;
    i2 = 0;

    do {
      i2 = i2 + 1;
    } while ( DECCounterNOTZERO() == true);

    if( i2 != 5 || Counter != 0 )
      apLogFailInfo( TestDesc + "do {-} while fails with single line in loop" ,"","","");
    

    TestDesc = "Set 1d: ";
    Counter = 5;
    i2 = 0;

    do {
      i2 = i2 + 1;
    } while ( DECCounter() != 0 && i2 != 5);

    if( i2 != 5 || Counter != 0 )
      apLogFailInfo( TestDesc + "do {-} while  fails with single line in loop" ,"","","");
    

    TestDesc = "Set 1e: ";
    Counter = 5;
    i2 = 0;

    do {
      i2 = i2 + 1;
    } while ( DECCounterNOTZERO() == true && i2 != 6);

    if( i2 != 5 || Counter != 0 )
      apLogFailInfo( TestDesc + "do {-} while  fails with single line in loop" ,"","","");
    

    apInitScenario( "2a. do-while: multiple lines in loop");
    TestDesc = "Set 2a: ";

    Counter = 178;

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var TestFailed;        
     
@end       

    TestFailed = true;

    do {
      TestFailed = false;
      if( Counter % 2 == 0 )
            Counter = Counter / 2;
      else 
        Counter = 3 * Counter + 1;
      
    } while ( Counter != 1);

    if( TestFailed || Counter != 1 )
      apLogFailInfo( TestDesc + "do {-} while  fails with multiple line in loop" ,"","","");
    

    TestDesc = "Set 2b: ";
    apInitScenario( "2b. if-else inside do-while");

    Counter = 178;
    TestFailed = true;

    do {
      TestFailed = false;
      if( Counter % 2 == 0 )
           Counter = Counter / 2;
      else
          Counter = 3 * Counter + 1;
      
    } while ( CounterNOTONE() == true);

    if( TestFailed || Counter != 1 )
      apLogFailInfo( TestDesc + "do {-} while fails with multiple line in loop" ,"","","");
    

    TestDesc = "Set 2c: ";
    apInitScenario( "2c. if-else inside do-while with multiple conditions");

    Counter = 178;
    TestFailed = true;

    do {
      TestFailed = false;
      if( Counter % 2 == 0 )
         Counter = Counter / 2;
      else
        Counter = 3 * Counter + 1;
      
    } while ( CounterONE() != true && TestFailed == false);

    if( TestFailed || Counter != 1 )
      apLogFailInfo( TestDesc + "do {-} while fails with multiple line in loop" ,"","","");
    

    TestDesc = "Set 2d: ";
    apInitScenario( "2d. if-else inside do-while with multiple conditions");

    Counter = 178;
    TestFailed = true;

    do {
      TestFailed = false;
      if( Counter % 2 == 0 ) {
             Counter = Counter / 2;
           
      }
      else {
             Counter = 3 * Counter + 1;
      }
   
      
    } while ( CounterNOTONE() == true && TestFailed == false);

    if( TestFailed || Counter != 1 ) {
      apLogFailInfo( TestDesc + "do {-} while fails with multiple line in loop" ,"","","");
    }

    TestDesc = "Set 3: ";
    apInitScenario( "3. do-while: Nested 2 levels");

    Counter = 180;

    var tmp = 120000;
    do {
      do {
 if( tmp % 2 == 0 )
 tmp = tmp / 2;
 else
 tmp = 3 * tmp + 1;
 
 if( tmp == 1 )
    TestFailed = false;
 
 TestFailed = false;
 Counter = Counter - 1;
      } while ( Counter % 45 != 0);
    } while ( Counter != 0);

    if( tmp != 1 || Counter != 0 )
      apLogFailInfo( TestDesc + "do {-} while fails when nested two levels" ,"","","");
    

    TestDesc = "Set 4a: ";
    apInitScenario( "4a. Multiple do-whiles inside each other");

    tmp = 0;
    do {
      do {
 do {
 tmp = tmp + 1;
 } while ( tmp % 7 != 0);
      } while ( tmp % 5 != 0);
    } while ( tmp % 3 != 0);

    if( tmp != 105 )
      apLogFailInfo( TestDesc + "do {-} while fails with multiple do {-LOOPs in loop" ,"","","");
    

    TestDesc = "Set 4b: ";
    apInitScenario( "4b. Multiple do-whiles inside each other");

    tmp = 0;
    do {
      do {
 do {
 tmp = tmp + 1;
 } while ( Different(tmp % 7, 0) == true);
      } while ( Different(tmp % 5, 0) == true);
    } while ( Different(tmp % 3, 0) == true);

    if( tmp != 105 )
      apLogFailInfo( TestDesc + "do {-} while fails with multiple do -LOOPs in loop" ,"","","");
    

    TestDesc = "Set 4c: ";
    apInitScenario( "4c. Multiple do-whiles inside each other, 2 conditions on each");
    i2 = 42;

    tmp = 0;
    do {
      do {
 do {
 tmp = tmp + 1;
 } while ( tmp % 7 != 0 && i2 == 42);
      } while ( tmp % 5 != 0 && i2 < 52);
    } while ( tmp % 3 != 0 && i2 > 32);

    if( tmp != 105 ) {
      apLogFailInfo( TestDesc + "do {-} while fails with multiple do -LOOPs in loop" ,"","","");
    }

    TestDesc = "Set 4d: ";
    apInitScenario( "4d. Multiple do-whiles inside each other");

    tmp = 0;
    do {
      do {
 do {
 tmp = tmp + 1;
 } while ( Different(tmp % 7, 0) == true);
      } while ( Different(tmp % 5, 0) == true);
    } while ( Different(tmp % 3, 0) == true);

    if( tmp != 105 )
      apLogFailInfo( TestDesc + "do {-} while fails with multiple do LOOPs in loop" ,"","","");
    

    TestDesc = "Set 5a: ";
    apInitScenario( "5a. do-while nested 4 deep");

    tmp = 0;
    do {
      do {
 do {
 do {
 do {
 do {
 tmp = tmp + 1;
 } while ( tmp % 13 != 0);
 } while ( tmp % 11 != 0);
 } while ( tmp % 7 != 0);
 } while ( tmp % 5 != 0);
      } while ( tmp % 3 != 0);
    } while ( tmp % 2 != 0);

    if( tmp != 30030 )
      apLogFailInfo( TestDesc + "do {-} while  fails with deeply nested loops" ,"","","");
    
    TestDesc = "Set 5b: "
    apInitScenario( "5b. do-while nested 3 deep, long loops");

    tmp = 0;
    do {
      do {
        do {
          tmp = tmp + 1;
        } while ( tmp % 13 != 0 || tmp % 11 != 0);
      } while ( tmp % 7 != 0 || tmp % 5 != 0);
    } while ( tmp % 3 != 0 || tmp % 2 != 0);

    if( tmp != 30030 ) {
      apLogFailInfo( TestDesc + "do {-} while  fails with deeply nested loops" ,"","","");
    }

    TestDesc = "Set 11a: "
    apInitScenario( "11a. Single line in loop, simple conditional");
    i2 = 5;

    do {
      i2 = i2 - 1;
    } while (i2 != 0);

    if( i2 != 0 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with single line in loop" ,"","","");
    

    TestDesc = "Set 11b: ";
    apInitScenario( "11b. Single line in loop, function conditional !=");
    Counter = 5;
    i2 = 0;

    do {
      i2 = i2 + 1;
    } while (DECCounter() != 0);

    if( i2 != 5 || Counter != 0 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with single line in loop" ,"","","");
    

    TestDesc = "Set 11c: ";
    apInitScenario( "11c. Single line in loop, function conditional ==");
    Counter = 5;
    i2 = 0;

    do {
      i2 = i2 + 1;
    } while (DECCounterNOTZERO() == true);

    if( i2 != 5 || Counter != 0 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with single line in loop" ,"","","");
    

    TestDesc = "Set 11d: ";
    apInitScenario( "11d. Single line in loop, 2 conditionals");
    Counter = 5;
    i2 = 0;

    do {
      i2 = i2 + 1;
    } while (DECCounter() != 0 && i2 != 5);

    if( i2 != 5 || Counter != 0 )
      apLogFailInfo( TestDesc + "do LOOP while fails with single line in loop" ,"","","");
    

    TestDesc = "Set 11e: ";
    apInitScenario( "11e. Single line in loop, 2 conditionals");
    Counter = 5;
    i2 = 0;

    do {
      i2 = i2 + 1;
    } while (DECCounterNOTZERO() == true && i2 < 6);

    if( i2 != 5 || Counter != 0 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with single line in loop" ,"","","");
    

    TestDesc = "Set 12a: ";
    apInitScenario( "12a. Multiple lines in loop, if/else");

    Counter = 178;
    TestFailed = true;

    do {
      TestFailed = false;
      if( Counter % 2 == 0 )
 Counter = Counter / 2;
      else
 Counter = 3 * Counter + 1;
      
    }  while (Counter != 1);

    if( TestFailed || Counter != 1 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with multiple line in loop" ,"","","");
    

    TestDesc = "Set 12b: ";
    apInitScenario( "12b. Multiple lines in loop");

    Counter = 178;
    TestFailed = true;

    do {
      TestFailed = false;
      if( Counter % 2 == 0 )
 Counter = Counter / 2;
      else
 Counter = 3 * Counter + 1;
      
   } while (CounterNOTONE() == true);

    if( TestFailed || Counter != 1 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with multiple line in loop" ,"","","");
    

    TestDesc = "Set 12c: ";
    apInitScenario( "12c. Multiple lines in loop");

    Counter = 178;
    TestFailed = true;

    do {
      TestFailed = false;
      if( Counter % 2 == 0 )
 Counter = Counter / 2;
      else
 Counter = 3 * Counter + 1;
      
    } while (CounterONE() != true && TestFailed != true);

    if( TestFailed || Counter != 1 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with multiple line in loop" ,"","","");
    

    TestDesc = "Set 12d: ";
    apInitScenario( "12d. Multiple lines in loop");

    Counter = 178;
    TestFailed = true;

    do {
      TestFailed = false;
      if( Counter % 2 == 0 )
 Counter = Counter / 2;
      else
 Counter = 3 * Counter + 1;
      
    } while (CounterNOTONE() == true && TestFailed == false);

    if( TestFailed || Counter != 1 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with multiple line in loop" ,"","","");    

    TestDesc = "Set 13: ";
    apInitScenario( "13. Nested loops");

    Counter = 180;

    tmp = 120000;
    do {
      do {
 if( tmp % 2 == 0 )
 tmp = tmp / 2;
 else
 tmp = 3 * tmp + 1;
 
 if( tmp == 1 )
 TestFailed = false;
 
 TestFailed = false;
 Counter = Counter - 1;
      } while (Counter % 45 != 0);
    } while (Counter != 0);

    if( tmp != 1 || Counter != 0 )
      apLogFailInfo( TestDesc + "do LOOP while fails when nested two levels" ,"","","");
    

    TestDesc = "Set 14a: ";
    apInitScenario( "14a. Check everything having to do  with do  loops");

    tmp = 0;
    do {
      do {
 do {
 tmp = tmp + 1;
 } while (tmp % 7 != 0);
      } while (tmp % 5 != 0);
    } while (tmp % 3 != 0);

    if( tmp != 105 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with multiple do -LOOPs in loop" ,"","","");
    

    TestDesc = "Set 14b: ";
    apInitScenario( "14b. Nested 3 deep");

    tmp = 0;
    do {
      do {
 do {
 tmp = tmp + 1;
 } while (Different(tmp % 7, 0) == true)
      } while (Different(tmp % 5, 0) == true)
    } while (Different(tmp % 3, 0) == true)

    if( tmp != 105 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with multiple do -LOOPs in loop" ,"","","");
    

    TestDesc = "Set 14c: ";
    apInitScenario( "14c. Nested 3 deep, multiple conditionals");
    i2 = 42;

    tmp = 0;
    do {
      do {
 do {
 tmp = tmp + 1;
 } while (tmp % 7 != 0 && i2 == 42);
      } while (tmp % 5 != 0 && i2 < 52);
    } while (tmp % 3 != 0 && i2 > 32);

    if( tmp != 105 )
      apLogFailInfo( TestDesc + "do -LOOP while fails with multiple do -LOOPs in loop" ,"","","");
    

    TestDesc = "Set 14d: ";
    apInitScenario( "14d. Nested 3 deep");

    tmp = 0;
    do {
      do {
 do {
 tmp = tmp + 1;
 } while (Different(tmp % 7, 0) == true);
      } while (Different(tmp % 5, 0) == true);
    } while (Different(tmp % 3, 0) == true);

    if( tmp != 105 )
      apLogFailInfo( TestDesc + "do {-LOOP while fails with multiple do {-LOOPs in loop" ,"","","");
    

    TestDesc = "Set 15a: ";
    apInitScenario( "15a. Nested 6 deep");

    tmp = 0;
    do {
      do {
 do {
 do {
 do {
 do {
 tmp = tmp + 1;
 } while ( tmp % 13 != 0);
 } while ( tmp % 11 != 0);
 } while ( tmp % 7 != 0);
 } while ( tmp % 5 != 0);
      } while ( tmp % 3 != 0);
    } while ( tmp % 2 != 0);

    if( tmp != 30030 )
      apLogFailInfo( TestDesc + "do {-} while ( fails with deeply nested }s" ,"","","");
    

    TestDesc = "Set 15b: ";
    apInitScenario( "15b. Nested 3 deep, long loops");

    tmp = 0;
    do {
      do {
 do {
 tmp = tmp + 1;
 } while ( tmp % 13 != 0 || tmp % 11 != 0);
      } while ( tmp % 7 != 0 || tmp % 5 != 0);
    } while ( tmp % 3 != 0 || tmp % 2 != 0);

    if( tmp != 30030 )
      apLogFailInfo( TestDesc + "do {-} while fails with deeply nested }s" ,"","","");
    

    apEndTest()
} // end function do001






do001();


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

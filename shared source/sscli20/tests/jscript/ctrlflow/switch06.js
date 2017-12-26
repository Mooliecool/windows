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


var iTestID = 171837;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;



// -----------------------------------------------------------------------
function switch06()
{
   apInitTest ("Switch06");
   
   // -----------------------------------------------------------------------
   apInitScenario ("1.1 No type annotation; Lowecase letters (a,b,c)");
   
   var foo1_1;
   
   foo1_1 = "a";
   expected = 1;
   actual = 0;
   switch (foo1_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.1(1)", expected, actual, "");

   foo1_1 = "b";
   expected = 2;
   actual = 0;
   switch (foo1_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.1(2)", expected, actual, "");

   foo1_1 = "c";
   expected = 3;
   actual = 0;
   switch (foo1_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.1(3)", expected, actual, "");
      
    
   // -----------------------------------------------------------------------
   apInitScenario ("1.2 No type annotation; Uppercase letters (A,B,C)");
   
   var foo1_2;
   
   foo1_2 = "A";
   expected = 4;
   actual = 0;
   switch (foo1_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.2(1)", expected, actual, "");

   foo1_2 = "B";
   expected = 5;
   actual = 0;
   switch (foo1_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.2(2)", expected, actual, "");

   foo1_2 = "C";
   expected = 6;
   actual = 0;
   switch (foo1_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.2(3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("1.3 No type annotation; Numbers (1,2,3)");      
      
   var foo1_3;
   
   foo1_3 = 1;
   expected = -1;
   actual = 0;
   switch (foo1_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.3(1)", expected, actual, "");

   foo1_3 = 2;
   expected = -1;
   actual = 0;
   switch (foo1_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.3(2)", expected, actual, "");

   foo1_3 = 3;
   expected = -1;
   actual = 0;
   switch (foo1_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.3(3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("1.4 No type annotation; Whitespaces (' ', '\\n', '\\t')");      
   
   var foo1_4;
   
   foo1_4 = " ";
   expected = 7;
   actual = 0;
   switch (foo1_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.4(1)", expected, actual, "");

   foo1_4 = "\n";
   expected = 8;
   actual = 0;
   switch (foo1_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.4(2)", expected, actual, "");

   foo1_4 = "\t";
   expected = 9;
   actual = 0;
   switch (foo1_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.4(3)", expected, actual, "");         
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("1.5 No type annotation; Numeric equivalent");      
      
   var foo1_5;
   
   foo1_5 = "A";  // No type annotation; foo is assigned a string "A"
   expected = -1;
   actual = 0;
   switch (foo1_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.5(1)", expected, actual, "");
      
   foo1_5 = "B";  // No type annotation; foo is assigned a string "B"
   expected = -1;
   actual = 0;
   switch (foo1_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.5(2)", expected, actual, "");
      
   foo1_5 = "C";  // No type annotation; foo is assigned a string "C"
   expected = -1;
   actual = 0;
   switch (foo1_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.5(3)", expected, actual, "");      
      
   foo1_5 = " ";  // No type annotation; foo is assigned a string " "
   expected = -1;
   actual = 0;
   switch (foo1_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.5(4)", expected, actual, "");      
      
   foo1_5 = "\n";  // No type annotation; foo is assigned a string "\n"
   expected = -1;
   actual = 0;
   switch (foo1_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.5(5)", expected, actual, "");      
      
   foo1_5 = "\t";  // No type annotation; foo is assigned a string "\t"
   expected = -1;
   actual = 0;
   switch (foo1_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.5(6)", expected, actual, "");      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("1.6 No type annotation; Symbols (!,?,#)");      
      
   var foo1_6;
   
   foo1_6 = "!";
   expected = 1;
   actual = 0;
   switch (foo1_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.6(1)", expected, actual, "");

   foo1_6 = "?";
   expected = 2;
   actual = 0;
   switch (foo1_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.6(2)", expected, actual, "");

   foo1_6 = "#";
   expected = 3;
   actual = 0;
   switch (foo1_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.6(3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("1.7 No type annotation; Char(x)");   
   
   var foo1_7;
   
   foo1_7 = char(97);
   expected = 1;
   actual = 0;
   switch (foo1_7)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.7(1)", expected, actual, "");
      
   foo1_7 = char(65);
   expected = 4;
   actual = 0;
   switch (foo1_7)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.7(2)", expected, actual, "");

   foo1_7 = char(49);
   expected = 1;
   actual = 0;
   switch (foo1_7)
   {
      case "1":
         actual = 1;
         break;
      case "2":
         actual = 2;
         break;
      case "3":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.7(3)", expected, actual, "");          
      
   foo1_7 = char(32);
   expected = 1;
   actual = 0;
   switch (foo1_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.7(4)", expected, actual, "");          
      
   foo1_7 = char(10);
   expected = 2;
   actual = 0;
   switch (foo1_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.7(5)", expected, actual, "");          
      
   foo1_7 = char(9);
   expected = 3;
   actual = 0;
   switch (foo1_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.7(6)", expected, actual, "");          
      
   foo1_7 = char(33);
   expected = 4;
   actual = 0;
   switch (foo1_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      case "!":
         actual = 4;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 1.7(7)", expected, actual, "");          
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.1 Declared as char; Lowecase letters (a,b,c)");
   
   var foo2_1: char;
   
   foo2_1 = "a";
   expected = 1;
   actual = 0;
   switch (foo2_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.1(1)", expected, actual, "");

   foo2_1 = "b";
   expected = 2;
   actual = 0;
   switch (foo2_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.1(2)", expected, actual, "");

   foo2_1 = "c";
   expected = 3;
   actual = 0;
   switch (foo2_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.1(3)", expected, actual, "");       
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.2 Declared as char; Uppercase letters (A,B,C)");
   
   var foo2_2: char;
   
   foo2_2 = "A";
   expected = 4;
   actual = 0;
   switch (foo2_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.2(1)", expected, actual, "");

   foo2_2 = "B";
   expected = 5;
   actual = 0;
   switch (foo2_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.2(2)", expected, actual, "");

   foo2_2 = "C";
   expected = 6;
   actual = 0;
   switch (foo2_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.2(3)", expected, actual, "");         
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.3 Declared as char; Numbers (1,2,3)");      
      
   var foo2_3: char;
   
   foo2_3 = 1;
   expected = -1;
   actual = 0;
   switch (foo2_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.3(1)", expected, actual, "");

   foo2_3 = 2;
   expected = -1;
   actual = 0;
   switch (foo2_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.3(2)", expected, actual, "");

   foo2_3 = 3;
   expected = -1;
   actual = 0;
   switch (foo2_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.3(3)", expected, actual, "");         
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.4 Declared as char; Whitespaces (' ', '\\n', '\\t')");      
   
   var foo2_4: char;
   
   foo2_4 = " ";
   expected = 7;
   actual = 0;
   switch (foo2_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.4(1)", expected, actual, "");

   foo2_4 = "\n";
   expected = 8;
   actual = 0;
   switch (foo2_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.4(2)", expected, actual, "");

   foo2_4 = "\t";
   expected = 9;
   actual = 0;
   switch (foo2_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.4(3)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.5 Declared as char; Numeric equivalent");      
      
   var foo2_5: char;
   
   foo2_5 = "A";  
   expected = 65;
   actual = 0;
   switch (foo2_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.5(1)", expected, actual, "");
      
   foo2_5 = "B";  
   expected = 66;
   actual = 0;
   switch (foo2_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.5(2)", expected, actual, "");
      
   foo2_5 = "C";  
   expected = 67;
   actual = 0;
   switch (foo2_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.5(3)", expected, actual, "");      
      
   foo2_5 = " ";  
   expected = 32;
   actual = 0;
   switch (foo2_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.5(4)", expected, actual, "");      
      
   foo2_5 = "\n";  
   expected = 10;
   actual = 0;
   switch (foo2_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.5(5)", expected, actual, "");      
      
   foo2_5 = "\t";  
   expected = 9;
   actual = 0;
   switch (foo2_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.5(6)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.6 Declared as char; Symbols (!,?,#)");      
      
   var foo2_6: char;
   
   foo2_6 = "!";
   expected = 1;
   actual = 0;
   switch (foo2_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.6(1)", expected, actual, "");

   foo2_6 = "?";
   expected = 2;
   actual = 0;
   switch (foo2_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.6(2)", expected, actual, "");

   foo2_6 = "#";
   expected = 3;
   actual = 0;
   switch (foo2_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.6(3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.7 Declared as char; Char(x)");   
   
   var foo2_7: char;
   
   foo2_7 = char(97);
   expected = 1;
   actual = 0;
   switch (foo2_7)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.7(1)", expected, actual, "");
      
   foo2_7 = char(65);
   expected = 4;
   actual = 0;
   switch (foo2_7)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.7(2)", expected, actual, "");

   foo2_7 = char(49);
   expected = 1;
   actual = 0;
   switch (foo2_7)
   {
      case "1":
         actual = 1;
         break;
      case "2":
         actual = 2;
         break;
      case "3":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.7(3)", expected, actual, "");          
      
   foo2_7 = char(32);
   expected = 1;
   actual = 0;
   switch (foo2_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.7(4)", expected, actual, "");          
      
   foo2_7 = char(10);
   expected = 2;
   actual = 0;
   switch (foo2_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.7(5)", expected, actual, "");          
      
   foo2_7 = char(9);
   expected = 3;
   actual = 0;
   switch (foo2_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.7(6)", expected, actual, "");          
      
   foo2_7 = char(33);
   expected = 4;
   actual = 0;
   switch (foo2_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      case "!":
         actual = 4;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 2.7(7)", expected, actual, "");              
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Declared as String; Lowecase letters (a,b,c)");
   
   var foo3_1: String;
   
   foo3_1 = "a";
   expected = 1;
   actual = 0;
   switch (foo3_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.1(1)", expected, actual, "");

   foo3_1 = "b";
   expected = 2;
   actual = 0;
   switch (foo3_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.1(2)", expected, actual, "");

   foo3_1 = "c";
   expected = 3;
   actual = 0;
   switch (foo3_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.1(3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3.2 Declared as String; Uppercase letters (A,B,C)");
   
   var foo3_2: String;
   
   foo3_2 = "A";
   expected = 4;
   actual = 0;
   switch (foo3_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.2(1)", expected, actual, "");

   foo3_2 = "B";
   expected = 5;
   actual = 0;
   switch (foo3_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.2(2)", expected, actual, "");

   foo3_2 = "C";
   expected = 6;
   actual = 0;
   switch (foo3_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.2(3)", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Declared as String; Numbers (1,2,3)");      
      
   var foo3_3: String;
   
   foo3_3 = 1;
   expected = 11;
   actual = 0;
   switch (foo3_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.3(1)", expected, actual, "");

   foo3_3 = 2;
   expected = 22;
   actual = 0;
   switch (foo3_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.3(2)", expected, actual, "");

   foo3_3 = 3;
   expected = 33;
   actual = 0;
   switch (foo3_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.3(3)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3.4 Declared as String; Whitespaces (' ', '\\n', '\\t')");      
   
   var foo3_4: String;
   
   foo3_4 = " ";
   expected = 7;
   actual = 0;
   switch (foo3_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.4(1)", expected, actual, "");

   foo3_4 = "\n";
   expected = 8;
   actual = 0;
   switch (foo3_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.4(2)", expected, actual, "");

   foo3_4 = "\t";
   expected = 9;
   actual = 0;
   switch (foo3_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.4(3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3.5 Declared as String; Numeric equivalent");      
      
   var foo3_5: String;
   
   foo3_5 = "A";  
   expected = -1;
   actual = 0;
   switch (foo3_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.5(1)", expected, actual, "");
      
   foo3_5 = "B";  
   expected = -1;
   actual = 0;
   switch (foo3_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.5(2)", expected, actual, "");
      
   foo3_5 = "C";  
   expected = -1;
   actual = 0;
   switch (foo3_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.5(3)", expected, actual, "");      
      
   foo3_5 = " ";  
   expected = -1;
   actual = 0;
   switch (foo3_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.5(4)", expected, actual, "");      
      
   foo3_5 = "\n";  
   expected = -1;
   actual = 0;
   switch (foo3_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.5(5)", expected, actual, "");      
      
   foo3_5 = "\t";  
   expected = -1;
   actual = 0;
   switch (foo3_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.5(6)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3.6 Declared as String; Symbols (!,?,#)");      
      
   var foo3_6: String;
   
   foo3_6 = "!";
   expected = 1;
   actual = 0;
   switch (foo3_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.6(1)", expected, actual, "");

   foo3_6 = "?";
   expected = 2;
   actual = 0;
   switch (foo3_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.6(2)", expected, actual, "");

   foo3_6 = "#";
   expected = 3;
   actual = 0;
   switch (foo3_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.6(3)", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3.7 Declared as String; Char(x)");   
   
   var foo3_7: String;
   
   foo3_7 = char(97);
   expected = 1;
   actual = 0;
   switch (foo3_7)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.7(1)", expected, actual, "");
      
   foo3_7 = char(65);
   expected = 4;
   actual = 0;
   switch (foo3_7)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.7(2)", expected, actual, "");

   foo3_7 = char(49);
   expected = 1;
   actual = 0;
   switch (foo3_7)
   {
      case "1":
         actual = 1;
         break;
      case "2":
         actual = 2;
         break;
      case "3":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.7(3)", expected, actual, "");          
      
   foo3_7 = char(32);
   expected = 1;
   actual = 0;
   switch (foo3_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.7(4)", expected, actual, "");          
      
   foo3_7 = char(10);
   expected = 2;
   actual = 0;
   switch (foo3_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.7(5)", expected, actual, "");          
      
   foo3_7 = char(9);
   expected = 3;
   actual = 0;
   switch (foo3_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.7(6)", expected, actual, "");          
      
   foo3_7 = char(33);
   expected = 4;
   actual = 0;
   switch (foo3_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      case "!":
         actual = 4;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 3.7(7)", expected, actual, "");         
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.1 Declared as System.Char; Lowecase letters (a,b,c)");
   
   var foo4_1: System.Char;
   
   foo4_1 = "a";
   expected = 1;
   actual = 0;
   switch (foo4_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.1(1)", expected, actual, "");

   foo4_1 = "b";
   expected = 2;
   actual = 0;
   switch (foo4_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.1(2)", expected, actual, "");

   foo4_1 = "c";
   expected = 3;
   actual = 0;
   switch (foo4_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.1(3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.2 Declared as System.Char; Uppercase letters (A,B,C)");
   
   var foo4_2: System.Char;
   
   foo4_2 = "A";
   expected = 4;
   actual = 0;
   switch (foo4_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.2(1)", expected, actual, "");

   foo4_2 = "B";
   expected = 5;
   actual = 0;
   switch (foo4_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.2(2)", expected, actual, "");

   foo4_2 = "C";
   expected = 6;
   actual = 0;
   switch (foo4_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.2(3)", expected, actual, "");      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.3 Declared as System.Char; Numbers (1,2,3)");      
      
   var foo4_3: System.Char;
   
   foo4_3 = 1;
   expected = -1;
   actual = 0;
   switch (foo4_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.3(1)", expected, actual, "");

   foo4_3 = 2;
   expected = -1;
   actual = 0;
   switch (foo4_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.3(2)", expected, actual, "");

   foo4_3 = 3;
   expected = -1;
   actual = 0;
   switch (foo4_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.3(3)", expected, actual, "");                                                      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.4 Declared as System.Char; Whitespaces (' ', '\\n', '\\t')");      
   
   var foo4_4: System.Char;
   
   foo4_4 = " ";
   expected = 7;
   actual = 0;
   switch (foo4_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.4(1)", expected, actual, "");

   foo4_4 = "\n";
   expected = 8;
   actual = 0;
   switch (foo4_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.4(2)", expected, actual, "");

   foo4_4 = "\t";
   expected = 9;
   actual = 0;
   switch (foo4_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.4(3)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.5 Declared as System.Char; Numeric equivalent");      
      
   var foo4_5: System.Char;
   
   foo4_5 = "A";  
   expected = 65;
   actual = 0;
   switch (foo4_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.5(1)", expected, actual, "");
      
   foo4_5 = "B";  
   expected = 66;
   actual = 0;
   switch (foo4_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.5(2)", expected, actual, "");
      
   foo4_5 = "C";  
   expected = 67;
   actual = 0;
   switch (foo4_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.5(3)", expected, actual, "");      
      
   foo4_5 = " ";  
   expected = 32;
   actual = 0;
   switch (foo4_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.5(4)", expected, actual, "");      
      
   foo4_5 = "\n";  
   expected = 10;
   actual = 0;
   switch (foo4_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.5(5)", expected, actual, "");      
      
   foo4_5 = "\t";  
   expected = 9;
   actual = 0;
   switch (foo4_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.5(6)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.6 Declared as System.Char; Symbols (!,?,#)");      
      
   var foo4_6: System.Char;
   
   foo4_6 = "!";
   expected = 1;
   actual = 0;
   switch (foo4_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.6(1)", expected, actual, "");

   foo4_6 = "?";
   expected = 2;
   actual = 0;
   switch (foo4_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.6(2)", expected, actual, "");

   foo4_6 = "#";
   expected = 3;
   actual = 0;
   switch (foo4_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.6(3)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.7 Declared as System.Char; Char(x)");   
   
   var foo4_7: System.Char;
   
   foo4_7 = char(97);
   expected = 1;
   actual = 0;
   switch (foo4_7)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.7(1)", expected, actual, "");
      
   foo4_7 = char(65);
   expected = 4;
   actual = 0;
   switch (foo4_7)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.7(2)", expected, actual, "");

   foo4_7 = char(49);
   expected = 1;
   actual = 0;
   switch (foo4_7)
   {
      case "1":
         actual = 1;
         break;
      case "2":
         actual = 2;
         break;
      case "3":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.7(3)", expected, actual, "");          
      
   foo4_7 = char(32);
   expected = 1;
   actual = 0;
   switch (foo4_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.7(4)", expected, actual, "");          
      
   foo4_7 = char(10);
   expected = 2;
   actual = 0;
   switch (foo4_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.7(5)", expected, actual, "");          
      
   foo4_7 = char(9);
   expected = 3;
   actual = 0;
   switch (foo4_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.7(6)", expected, actual, "");          
      
   foo4_7 = char(33);
   expected = 4;
   actual = 0;
   switch (foo4_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      case "!":
         actual = 4;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 4.7(7)", expected, actual, "");         
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5.1 Declared as System.String; Lowecase letters (a,b,c)");
   
   var foo5_1: System.String;
   
   foo5_1 = "a";
   expected = 1;
   actual = 0;
   switch (foo5_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.1(1)", expected, actual, "");

   foo5_1 = "b";
   expected = 2;
   actual = 0;
   switch (foo5_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.1(2)", expected, actual, "");

   foo5_1 = "c";
   expected = 3;
   actual = 0;
   switch (foo5_1)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.1(3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5.2 Declared as System.String; Uppercase letters (A,B,C)");
   
   var foo5_2: System.String;
   
   foo5_2 = "A";
   expected = 4;
   actual = 0;
   switch (foo5_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.2(1)", expected, actual, "");

   foo5_2 = "B";
   expected = 5;
   actual = 0;
   switch (foo5_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.2(2)", expected, actual, "");

   foo5_2 = "C";
   expected = 6;
   actual = 0;
   switch (foo5_2)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.2(3)", expected, actual, "");     
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5.3 Declared as System.String; Numbers (1,2,3)");      
      
   var foo5_3: System.String;
   
   foo5_3 = 1;
   expected = 11;
   actual = 0;
   switch (foo5_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.3(1)", expected, actual, "");

   foo5_3 = 2;
   expected = 22;
   actual = 0;
   switch (foo5_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.3(2)", expected, actual, "");

   foo5_3 = 3;
   expected = 33;
   actual = 0;
   switch (foo5_3)
   {
      case "1":
         actual = 11;
         break;
      case "2":
         actual = 22;
         break;
      case "3":
         actual = 33;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.3(3)", expected, actual, "");     
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5.4 Declared as System.String; Whitespaces (' ', '\\n', '\\t')");      
   
   var foo5_4: System.String;
   
   foo5_4 = " ";
   expected = 7;
   actual = 0;
   switch (foo5_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.4(1)", expected, actual, "");

   foo5_4 = "\n";
   expected = 8;
   actual = 0;
   switch (foo5_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.4(2)", expected, actual, "");

   foo5_4 = "\t";
   expected = 9;
   actual = 0;
   switch (foo5_4)
   {
      case " ":
         actual = 7;
         break;
      case "\n":
         actual = 8;
         break;
      case "\t":
         actual = 9;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.4(3)", expected, actual, "");                                 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5.5 Declared as System.String; Numeric equivalent");      
      
   var foo5_5: System.String;
   
   foo5_5 = "A";  
   expected = -1;
   actual = 0;
   switch (foo5_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.5(1)", expected, actual, "");
      
   foo5_5 = "B";  
   expected = -1;
   actual = 0;
   switch (foo5_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.5(2)", expected, actual, "");
      
   foo5_5 = "C";  
   expected = -1;
   actual = 0;
   switch (foo5_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.5(3)", expected, actual, "");      
      
   foo5_5 = " ";  
   expected = -1;
   actual = 0;
   switch (foo5_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.5(4)", expected, actual, "");      
      
   foo5_5 = "\n";  
   expected = -1;
   actual = 0;
   switch (foo5_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.5(5)", expected, actual, "");      
      
   foo5_5 = "\t";  
   expected = -1;
   actual = 0;
   switch (foo5_5)
   {
      case 65:
         actual = 65;
         break;
      case 66:
         actual = 66;
         break;
      case 67:
         actual = 67;
         break;
      case 32:
         actual = 32;
         break;
      case 9:
         actual = 9;
         break;
      case 10:
         actual = 10;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.5(6)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5.6 Declared as System.String; Symbols (!,?,#)");      
      
   var foo5_6: System.String;
   
   foo5_6 = "!";
   expected = 1;
   actual = 0;
   switch (foo5_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.6(1)", expected, actual, "");

   foo5_6 = "?";
   expected = 2;
   actual = 0;
   switch (foo5_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.6(2)", expected, actual, "");

   foo5_6 = "#";
   expected = 3;
   actual = 0;
   switch (foo5_6)
   {
      case "!":
         actual = 1;
         break;
      case "?":
         actual = 2;
         break;
      case "#":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.6(3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5.7 Declared as System.String; Char(x)");   
   
   var foo5_7: System.String;
   
   foo5_7 = char(97);
   expected = 1;
   actual = 0;
   switch (foo5_7)
   {
      case "a":
         actual = 1;
         break;
      case "b":
         actual = 2;
         break;
      case "c":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.7(1)", expected, actual, "");
      
   foo5_7 = char(65);
   expected = 4;
   actual = 0;
   switch (foo5_7)
   {
      case "A":
         actual = 4;
         break;
      case "B":
         actual = 5;
         break;
      case "C":
         actual = 6;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.7(2)", expected, actual, "");

   foo5_7 = char(49);
   expected = 1;
   actual = 0;
   switch (foo5_7)
   {
      case "1":
         actual = 1;
         break;
      case "2":
         actual = 2;
         break;
      case "3":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.7(3)", expected, actual, "");          
      
   foo5_7 = char(32);
   expected = 1;
   actual = 0;
   switch (foo5_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.7(4)", expected, actual, "");          
      
   foo5_7 = char(10);
   expected = 2;
   actual = 0;
   switch (foo5_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.7(5)", expected, actual, "");          
      
   foo5_7 = char(9);
   expected = 3;
   actual = 0;
   switch (foo5_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.7(6)", expected, actual, "");          
      
   foo5_7 = char(33);
   expected = 4;
   actual = 0;
   switch (foo5_7)
   {
      case " ":
         actual = 1;
         break;
      case "\n":
         actual = 2;
         break;
      case "\t":
         actual = 3;
         break;
      case "!":
         actual = 4;
         break;
      default:
         actual = -1;
         break;
   }
   if (expected != actual)
      apLogFailInfo ("Error in 5.7(7)", expected, actual, "");                       
      
   apEndTest();
}



switch06();


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

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


var iTestID = 65321;



@if(!@aspx)
   function UDObject() {
      this.myValue = 88.8;
   }
@else
   expando function UDObject() {
      this.myValue = 88.8;
   }
@end


@if(@aspx) expando @end function UDFunction() {
    return 55.6;
}

function equ003() 
{
    var strVar = "13.2";
    var numVar = 17.3;
    var myDate = new Date();
    var emptyVar;
    var arrayVar = new Array(10);
    var myObj = new UDObject();
    var myUDF = new UDFunction();
    var myBool = true;

    arrayVar[3] = 13.1;
    arrayVar[4] = 14.2;
    arrayVar[5] = 15.3;
    myDate.setTime(0);

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
         var expected;
         var result;        
     
@end       

    apInitTest("equ003");
    
    // tokenizer output scenario #257
    apInitScenario("compare udf == udf");
    expected = true;
    result = (myUDF == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed", expected, result, "175435");
    }

    // tokenizer output scenario #297
    apInitScenario("compare array element == date variable");
    expected = false;
    result = (arrayVar[4] == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #298
    apInitScenario("compare array element == object");
    expected = false;
    result = (arrayVar[4] == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #299
    apInitScenario("compare array element == udf");
    expected = false;
    result = (arrayVar[4] == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #300
    apInitScenario("compare array element == fn retval");
    expected = false;
    result = (arrayVar[4] == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #301
    apInitScenario("compare array element == intrinsic function");
    expected = false;
    result = (arrayVar[4] == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #302
    apInitScenario("compare array element == array element");
    expected = true;
    result = (arrayVar[4] == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #303
    apInitScenario("compare array element == object method");
    expected = false;
    result = (arrayVar[4] == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #304
    apInitScenario("compare array element == string constant");
    expected = false;
    result = (arrayVar[4] == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #305
    apInitScenario("compare array element == numeric constant");
    expected = false;
    result = (arrayVar[4] == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #306
    apInitScenario("compare array element == boolean variable");
    expected = false;
    result = (arrayVar[4] == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #307
    apInitScenario("compare array element == null");
    expected = false;
    result = (arrayVar[4] == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #308
    apInitScenario("compare array element == boolean constant");
    expected = false;
    result = (arrayVar[4] == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #309
    apInitScenario("compare object method == numeric variable");
    expected = false;
    result = (myObj.myValue == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #310
    apInitScenario("compare object method == string variable");
    expected = false;
    result = (myObj.myValue == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #311
    apInitScenario("compare object method == date variable");
    expected = false;
    result = (myObj.myValue == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #312
    apInitScenario("compare object method == object");
    expected = false;
    result = (myObj.myValue == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #313
    apInitScenario("compare object method == udf");
    expected = false;
    result = (myObj.myValue == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #314
    apInitScenario("compare object method == fn retval");
    expected = false;
    result = (myObj.myValue == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #315
    apInitScenario("compare object method == intrinsic function");
    expected = false;
    result = (myObj.myValue == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #316
    apInitScenario("compare object method == array element");
    expected = false;
    result = (myObj.myValue == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #317
    apInitScenario("compare object method == object method");
    expected = true;
    result = (myObj.myValue == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #318
    apInitScenario("compare object method == string constant");
    expected = false;
    result = (myObj.myValue == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #319
    apInitScenario("compare object method == numeric constant");
    expected = false;
    result = (myObj.myValue == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #320
    apInitScenario("compare object method == boolean variable");
    expected = false;
    result = (myObj.myValue == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #321
    apInitScenario("compare object method == null");
    expected = false;
    result = (myObj.myValue == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #322
    apInitScenario("compare object method == boolean constant");
    expected = false;
    result = (myObj.myValue == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #323
    apInitScenario("compare string constant == numeric variable");
    expected = false;
    result = ("200.3" == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #324
    apInitScenario("compare string constant == string variable");
    expected = false;
    result = ("200.3" == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #325
    apInitScenario("compare string constant == date variable");
    expected = false;
    result = ("200.3" == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #326
    apInitScenario("compare string constant == object");
    expected = false;
    result = ("200.3" == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #327
    apInitScenario("compare string constant == udf");
    expected = false;
    result = ("200.3" == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #328
    apInitScenario("compare string constant == fn retval");
    expected = false;
    result = ("200.3" == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #329
    apInitScenario("compare string constant == intrinsic function");
    expected = false;
    result = ("200.3" == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #330
    apInitScenario("compare string constant == array element");
    expected = false;
    result = ("200.3" == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #331
    apInitScenario("compare string constant == object method");
    expected = false;
    result = ("200.3" == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #332
    apInitScenario("compare string constant == string constant");
    expected = true;
    result = ("200.3" == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #333
    apInitScenario("compare string constant == numeric constant");
    expected = false;
    result = ("200.3" == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #334
    apInitScenario("compare string constant == boolean variable");
    expected = false;
    result = ("200.3" == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #335
    apInitScenario("compare string constant == null");
    expected = false;
    result = ("200.3" == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #336
    apInitScenario("compare string constant == boolean constant");
    expected = false;
    result = ("200.3" == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #337
    apInitScenario("compare numeric constant == numeric variable");
    expected = false;
    result = (300.2 == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #338
    apInitScenario("compare numeric constant == string variable");
    expected = false;
    result = (300.2 == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #339
    apInitScenario("compare numeric constant == date variable");
    expected = false;
    result = (300.2 == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #340
    apInitScenario("compare numeric constant == object");
    expected = false;
    result = (300.2 == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #341
    apInitScenario("compare numeric constant == udf");
    expected = false;
    result = (300.2 == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #342
    apInitScenario("compare numeric constant == fn retval");
    expected = false;
    result = (300.2 == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #343
    apInitScenario("compare numeric constant == intrinsic function");
    expected = false;
    result = (300.2 == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #344
    apInitScenario("compare numeric constant == array element");
    expected = false;
    result = (300.2 == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #345
    apInitScenario("compare numeric constant == object method");
    expected = false;
    result = (300.2 == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #346
    apInitScenario("compare numeric constant == string constant");
    expected = false;
    result = (300.2 == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #347
    apInitScenario("compare numeric constant == numeric constant");
    expected = true;
    result = (300.2 == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #348
    apInitScenario("compare numeric constant == boolean variable");
    expected = false;
    result = (300.2 == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #349
    apInitScenario("compare numeric constant == null");
    expected = false;
    result = (300.2 == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #350
    apInitScenario("compare numeric constant == boolean constant");
    expected = false;
    result = (300.2 == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #351
    apInitScenario("compare boolean variable == numeric variable");
    expected = false;
    result = (myBool == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #352
    apInitScenario("compare boolean variable == string variable");
    expected = false;
    result = (myBool == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #353
    apInitScenario("compare boolean variable == date variable");
    expected = false;
    result = (myBool == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #354
    apInitScenario("compare boolean variable == object");
    expected = false;
    result = (myBool == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #355
    apInitScenario("compare boolean variable == udf");
    expected = false;
    result = (myBool == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #356
    apInitScenario("compare boolean variable == fn retval");
    expected = false;
    result = (myBool == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #357
    apInitScenario("compare boolean variable == intrinsic function");
    expected = false;
    result = (myBool == Math.cos(1));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #358
    apInitScenario("compare boolean variable == array element");
    expected = false;
    result = (myBool == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #359
    apInitScenario("compare boolean variable == object method");
    expected = false;
    result = (myBool == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #360
    apInitScenario("compare boolean variable == string constant");
    expected = false;
    result = (myBool == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #361
    apInitScenario("compare boolean variable == numeric constant");
    expected = false;
    result = (myBool == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #362
    apInitScenario("compare boolean variable == boolean variable");
    expected = true;
    result = (myBool == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #363
    apInitScenario("compare boolean variable == null");
    expected = false;
    result = (myBool == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #364
    apInitScenario("compare boolean variable == boolean constant");
    expected = true;
    result = (myBool == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #365
    apInitScenario("compare null == numeric variable");
    expected = false;
    result = (null == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #366
    apInitScenario("compare null == string variable");
    expected = false;
    result = (null == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #367
    apInitScenario("compare null == date variable");
    expected = false;
    result = (null == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #368
    apInitScenario("compare null == object");
    expected = false;
    result = (null == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #369
    apInitScenario("compare null == udf");
    expected = false;
    result = (null == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #370
    apInitScenario("compare null == fn retval");
    expected = false;
    result = (null == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #371
    apInitScenario("compare null == intrinsic function");
    expected = false;
    result = (null == Math.cos(0));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #372
    apInitScenario("compare null == array element");
    expected = false;
    result = (null == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #373
    apInitScenario("compare null == object method");
    expected = false;
    result = (null == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #374
    apInitScenario("compare null == string constant");
    expected = false;
    result = (null == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #375
    apInitScenario("compare null == numeric constant");
    expected = false;
    result = (null == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #376
    apInitScenario("compare null == boolean variable");
    expected = false;
    result = (null == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #377
    apInitScenario("compare null == null");
    expected = true;
    result = (null == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #378
    apInitScenario("compare null == boolean constant");
    expected = false;
    result = (null == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #379
    apInitScenario("compare boolean constant == numeric variable");
    expected = false;
    result = (true == numVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #380
    apInitScenario("compare boolean constant == string variable");
    expected = false;
    result = (true == strVar);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #381
    apInitScenario("compare boolean constant == date variable");
    expected = false;
    result = (true == myDate.getTime);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #382
    apInitScenario("compare boolean constant == object");
    expected = false;
    result = (true == myObj);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #383
    apInitScenario("compare boolean constant == udf");
    expected = false;
    result = (true == myUDF);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #384
    apInitScenario("compare boolean constant == fn retval");
    expected = false;
    result = (true == UDFunction());

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #385
    apInitScenario("compare boolean constant == intrinsic function");
    expected = false;
    result = (true == Math.cos(2));

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #386
    apInitScenario("compare boolean constant == array element");
    expected = false;
    result = (true == arrayVar[4]);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #387
    apInitScenario("compare boolean constant == object method");
    expected = false;
    result = (true == myObj.myValue);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #388
    apInitScenario("compare boolean constant == string constant");
    expected = false;
    result = (true == "200.3");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #389
    apInitScenario("compare boolean constant == numeric constant");
    expected = false;
    result = (true == 300.2);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #390
    apInitScenario("compare boolean constant == boolean variable");
    expected = true;
    result = (true == myBool);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #391
    apInitScenario("compare boolean constant == null");
    expected = false;
    result = (true == null);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

    // tokenizer output scenario #392
    apInitScenario("compare boolean constant == boolean constant");
    expected = true;
    result = (true == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

	apInitScenario("compare boolean constant == boolean constant");
    expected = true;
    result = (true == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

	apInitScenario("Booean object == boolean constant");
    expected = true;

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var x;        
     
@end       

	x = new Boolean();
	x = false;
    result = (x == false);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

	apInitScenario("Boolean object == string constant");
    expected = true;
	x = new Boolean();
	x = "true";	
    result = (x == "true");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

	apInitScenario("String object == string constant");
    expected = true;
	x = new String("test string");
    result = (x == "test string");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

	apInitScenario("String object == boolean constant");
    expected = false;
	x = new String();
	x = "true";	
    result = (x == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

	apInitScenario("Number object == number constant");
    expected = true;
	x = new Number();
	x = 123.456789;	
    result = (x == 123.456789);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

	apInitScenario("Number object == string constant");
    expected = true;
	x = new Number();
	x = 123.456789;	
    result = (x == "123.456789");

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

	
	apInitScenario();
    expected = true;
	x = new Boolean("true");
    result = (x == 1);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }
	expected = 1;
	x = new Boolean("true");
    result = (x == true);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }

	
	apInitScenario();
    expected = false;
	x = new Boolean("false");
    result = (x == 0);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }
	expected = 0;
	x = new Boolean("false");
    result = (x == false);

    if (result != expected) {
        apLogFailInfo("comparison failed",expected,result,"");
    }


    apEndTest();
}



equ003();


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

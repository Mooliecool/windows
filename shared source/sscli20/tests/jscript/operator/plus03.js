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


var iTestID = 176856;

//          this tescase tests the + compound assignment op using combinations
//          of hard typed variables

// Scenarios:

// 1) untyped + untyped
// 2) untyped + boolean
// 3) untyped + Number
// 4) untyped + String
// 5) untyped + Object
// 6) untyped + Date
// 7) untyped + Array
// 8) untyped + function
// 9) untyped + RegExp
// 10) untyped + Error
// 11) untyped + byte
// 12) untyped + char
// 13) untyped + short
// 14) untyped + int
// 15) untyped + long
// 16) untyped + float
// 17) boolean + untyped
// 18) boolean + boolean
// 19) boolean + Number
// 20) boolean + String
// 21) boolean + Object
// 22) boolean + Date
// 23) boolean + Array
// 24) boolean + function
// 25) boolean + RegExp
// 26) boolean + Error
// 27) boolean + byte
// 28) boolean + char
// 29) boolean + short
// 30) boolean + int
// 31) boolean + long
// 32) boolean + float
// 33) Number + untyped
// 34) Number + boolean
// 35) Number + Number
// 36) Number + String
// 37) Number + Object
// 38) Number + Date
// 39) Number + Array
// 40) Number + function
// 41) Number + RegExp
// 42) Number + Error
// 43) Number + byte
// 44) Number + char
// 45) Number + short
// 46) Number + int
// 47) Number + long
// 48) Number + float
// 49) String + untyped
// 50) String + boolean
// 51) String + Number
// 52) String + String
// 53) String + Object
// 54) String + Date
// 55) String + Array
// 56) String + function
// 57) String + RegExp
// 58) String + Error
// 59) String + byte
// 60) String + char
// 61) String + short
// 62) String + int
// 63) String + long
// 64) String + float
// 65) object + untyped
// 66) object + boolean
// 67) object + Number
// 68) object + String
// 69) object + Object
// 70) object + Date
// 71) object + Array
// 72) object + function
// 73) object + RegExp
// 74) object + Error
// 75) object + byte
// 76) object + char
// 77) object + short
// 78) object + int
// 79) object + long
// 80) object + float
// 81) Date + untyped
// 82) Date + boolean
// 83) Date + Number
// 84) Date + String
// 85) Date + Object
// 86) Date + Date
// 87) Date + Array
// 88) Date + function
// 89) Date + RegExp
// 90) Date + Error
// 91) Date + byte
// 92) Date + char
// 93) Date + short
// 94) Date + int
// 95) Date + long
// 96) Date + float
// 97) Array + untyped
// 98) Array + boolean
// 99) Array + Number
// 100) Array + String
// 101) Array + Object
// 102) Array + Date
// 103) Array + Array
// 104) Array + function
// 105) Array + RegExp
// 106) Array + Error
// 107) Array + byte
// 108) Array + char
// 109) Array + short
// 110) Array + int
// 111) Array + long
// 112) Array + float
// 113) Function + untyped
// 114) Function + boolean
// 115) Function + Number
// 116) Function + String
// 117) Function + Object
// 118) Function + Date
// 119) Function + Array
// 120) Function + function
// 121) Function + RegExp
// 122) Function + Error
// 123) Function + byte
// 124) Function + char
// 125) Function + short
// 126) Function + int
// 127) Function + long
// 128) Function + float
// 129) RegExp + untyped
// 130) RegExp + boolean
// 131) RegExp + Number
// 132) RegExp + String
// 133) RegExp + Object
// 134) RegExp + Date
// 135) RegExp + Array
// 136) RegExp + function
// 137) RegExp + RegExp
// 138) RegExp + Error
// 139) RegExp + byte
// 140) RegExp + char
// 141) RegExp + short
// 142) RegExp + int
// 143) RegExp + long
// 144) RegExp + float
// 145) Error + untyped
// 146) Error + boolean
// 147) Error + Number
// 148) Error + String
// 149) Error + Object
// 150) Error + Date
// 151) Error + Array
// 152) Error + function
// 153) Error + RegExp
// 154) Error + Error
// 155) Error + byte
// 156) Error + char
// 157) Error + short
// 158) Error + int
// 159) Error + long
// 160) Error + float
// 161) byte + untyped
// 162) byte + boolean
// 163) byte + Number
// 164) byte + String
// 165) byte + Object
// 166) byte + Date
// 167) byte + Array
// 168) byte + function
// 169) byte + RegExp
// 170) byte + Error
// 171) byte + byte
// 172) byte + char
// 173) byte + short
// 174) byte + int
// 175) byte + long
// 176) byte + float
// 177) Char + untyped
// 178) Char + boolean
// 179) Char + Number
// 180) Char + String
// 181) Char + Object
// 182) Char + Date
// 183) Char + Array
// 184) Char + function
// 185) Char + RegExp
// 186) Char + Error
// 187) Char + byte
// 188) Char + char
// 189) Char + short
// 190) Char + int
// 191) Char + long
// 192) Char + float
// 193) short + untyped
// 194) short + boolean
// 195) short + Number
// 196) short + String
// 197) short + Object
// 198) short + Date
// 199) short + Array
// 200) short + function
// 201) short + RegExp
// 202) short + Error
// 203) short + byte
// 204) short + char
// 205) short + short
// 206) short + int
// 207) short + long
// 208) short + float
// 209) int + untyped
// 210) int + boolean
// 211) int + Number
// 212) int + String
// 213) int + Object
// 214) int + Date
// 215) int + Array
// 216) int + function
// 217) int + RegExp
// 218) int + Error
// 219) int + byte
// 220) int + char
// 221) int + short
// 222) int + int
// 223) int + long
// 224) int + float
// 225) long + untyped
// 226) long + boolean
// 227) long + Number
// 228) long + String
// 229) long + Object
// 230) long + Date
// 231) long + Array
// 232) long + function
// 233) long + RegExp
// 234) long + Error
// 235) long + byte
// 236) long + char
// 237) long + short
// 238) long + int
// 239) long + long
// 240) long + float
// 241) float + untyped
// 242) float + boolean
// 243) float + Number
// 244) float + String
// 245) float + Object
// 246) float + Date
// 247) float + Array
// 248) float + function
// 249) float + RegExp
// 250) float + Error
// 251) float + byte
// 252) float + char
// 253) float + short
// 254) float + int
// 255) float + long
// 256) float + float

              

function FuncRetVal() { return 655.35; }
function myObjVal() { return 123321; }

@if(!@aspx)
	function myObject() { this.toString = myObjVal; }
@else
	expando function myObject() { this.toString = myObjVal; }
@end


function plus03() {
    var tmp,res,expected,emptyVar;

    var uvar = 4321;
    var boolvar:boolean = true;
    var numvar:Number = 3213.321;
    var strvar:String = "this is a string";
    var obvar:Object = new Object;
    var datevar:Date = new Date(0);
    var arrvar:Array = new Array;
	arrvar[0] = 2.2;
	//arrvar[3] = 3.3;
	//arrvar[4] = 4.4;
    function funvar(){var foo = 1};
    var revar:RegExp = /\d{2}-\d{2}/g;
    var errvar:Error = new Error;
    var bytevar:byte = 50;
    var charvar:char = "g";
    var shrtvar:short = 4321;
    var intvar:int = 1234;
    var longvar:long = 321321321321;
    var flovar:float = 1.25;
    var strTM:String = -2146828275;
    var strNE:String = "no errors...";

    apInitTest("AddAsgn01");


///////////////////////////////////////////////////////////////////
/////////////////  untyped  ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("untyped + untyped");
    uvar = 1234;
    res = uvar + 4321;
    expected = int(5555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + boolean");
    uvar = 1234;
    boolvar = true;
    res = uvar + boolvar;						// boolean changes to 1
	expected = int(1235);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + Number");
    uvar = 1234;
    numvar = 4321;
    res = uvar + numvar;
    expected = Number(5555);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    uvar = 1234;
    numvar = 4321.1;
    res = uvar + numvar;
    expected = Number(5555.1);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + String");
    uvar = 1234;
    strvar = "4321";
    res = uvar + strvar;
    expected = "12344321"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + Object");
    uvar = 1234;
    obvar = 4321;
    res = uvar + obvar;
    expected = int(5555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + Date");
    uvar = 1234;
    datevar = new Date(0);
    res = uvar + datevar;
    expected = "1234Wed Dec 31 16:00:00 PST 1969"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }

	
    apInitScenario("untyped + Array");
    uvar = 1234;
    // arrvar initialized above
    res = uvar + arrvar;
    expected = "12342.2"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + function");
    uvar = 1234;
    // funvar initialized above
    res = uvar + funvar;
    expected = "1234function funvar(){var foo = 1}"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + RegExp");
    uvar = 1234;
    revar = /\d{2}-\d{2}/g
    res = uvar + revar;
    expected = "1234/\\d{2}-\\d{2}/g"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + Error");
    uvar = 1234;
    errvar = new Error();
    res = uvar + errvar;
    expected = "1234Error"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + byte");
    uvar = 1234;
    bytevar = 21;
    res = uvar + bytevar;
    expected = 1255
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + char");
    uvar = 10;
	charvar = "A";
	res = uvar + charvar;
    expected = "K"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + short");
    uvar = -10000;
    shrtvar = -32000;
    res = uvar + shrtvar;
    expected = int(-42000)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + int");
    uvar = 1234;
	intvar = 1234567890
    res = uvar + intvar;
    expected = int(1234569124);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + long");
    uvar = 1234;
    longvar = 12344321;
    res = uvar + longvar;
    expected = int(12345555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    uvar = 1234;
    longvar = 2147482414;
    res = uvar + longvar;
    expected = 2147483648
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("untyped + float");
    uvar = 1234;
    flovar = 4321;
    res = uvar + flovar;
    expected = int(5555);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    uvar = 1234;
    flovar = 4321.1;
    res = uvar + flovar;
    expected = 5555.1;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


///////////////////////////////////////////////////////////////////
/////////////////  boolean  ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("boolean + untyped");
    boolvar = false;
    uvar = 4321;
    res = boolvar + uvar;
    expected = 4321;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    boolvar = false;
    uvar = 0;
    res = boolvar + uvar;
    expected = 0;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + boolean");
    boolvar = true;
    res = boolvar + true;					// boolean changes to 1
	expected = 2;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + Number");
    boolvar = true;
    numvar = -12;
    res = boolvar + numvar;
    expected = -11
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    boolvar = false;
    numvar = -.00000001;
    res = boolvar + numvar;
    expected = -.00000001;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + String");
    boolvar = false;
    strvar = "4321";
	res = boolvar + strvar;
    expected = "false4321"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + Object");
    boolvar = true;
    obvar = 4321;
    res = boolvar + obvar;
    expected = 4322;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + Date");
    boolvar = false;
    datevar = new Date(0);
    res = boolvar + datevar;
    expected = "falseWed Dec 31 16:00:00 PST 1969"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }

	
    apInitScenario("boolean + Array");
    boolvar = false;
    // arrvar initialized above
	res = boolvar + arrvar;
    expected = "false2.2"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + function");
    boolvar = false;
    // funvar initialized above
	res = boolvar + funvar;
    expected = "falsefunction funvar(){var foo = 1}"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + RegExp");
    boolvar = false;
    revar = /\d{2}-\d{2}/g
	res = boolvar + revar;
    expected = "false/\\d{2}-\\d{2}/g"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + Error");
    boolvar = false;
    errvar = new Error();
	res = boolvar + errvar;
    expected = "falseError"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + byte");
    boolvar = true;
    bytevar = 21;
    res = boolvar + bytevar;
    expected = 22
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + char");
    boolvar = false;
    charvar = "A";
    res = boolvar + charvar;
    expected = 65
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + short");
    boolvar = true;
    shrtvar = -32000;
    res = boolvar + shrtvar;
    expected = -31999
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + int");
    boolvar = true;
    intvar = 1234567890;
    res = boolvar + intvar;
    expected = 1234567891;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + long");
    boolvar = false;
    longvar = 0;
    res = boolvar + longvar;
    expected = 0
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    boolvar = true;
    longvar = 2147483647;
    res = boolvar + longvar;
    expected = 2147483648
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("boolean + float");
    boolvar = false;
    flovar = -Infinity;
    res = boolvar + flovar;
    expected = -Infinity
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    boolvar = true;
    flovar = 4321.1;
    res = boolvar + flovar;
    expected = 4322.1
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


///////////////////////////////////////////////////////////////////
/////////////////  Number   ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("Number + untyped");
    numvar = 1234;
    uvar = 4321;
    res = numvar + uvar;
    expected = int(5555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + boolean");
    numvar = 1234;
    boolvar = true;
    res = numvar + boolvar;						// boolean changes to 1
	expected = int(1235)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + Number");
    numvar = 1234;
    res = numvar + Number(4321);
    expected = int(5555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    numvar = 1234;
    res = numvar + Number(4321.1);
    expected = Number(5555.1)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + String");
    numvar = 1234;
    strvar = "4321";
    res = numvar + strvar;
    expected = "12344321"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + Object");
    numvar = 1234;
    obvar = 4321;
    res = numvar + obvar;
    expected = int(5555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + Date");
    numvar = 1234;
    datevar = new Date(0);
    res = numvar + datevar;
    expected = "1234Wed Dec 31 16:00:00 PST 1969"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }

	
    apInitScenario("Number + Array");
    numvar = 1234;
    // arrvar initialized above
    res = numvar + arrvar;
    expected = "12342.2"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + function");
    numvar = 1234;
    // funvar initialized above
    res = numvar + funvar;
    expected = "1234function funvar(){var foo = 1}"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + RegExp");
    numvar = 1234;
    revar = /\d{2}-\d{2}/g
    res = numvar + revar;
    expected = "1234/\\d{2}-\\d{2}/g"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + Error");
    numvar = 1234;
    errvar = new Error();
    res = numvar + errvar;
    expected = "1234Error"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + byte");
    numvar = 1234;
    bytevar = 21;
    res = numvar + bytevar;
    expected = int(1255)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + char");
    numvar = 1234;
    charvar = "A";
    res = numvar + charvar;
    expected = int(1299)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + short");
    numvar = -10000;
    shrtvar = -32000;
    res = numvar + shrtvar;
    expected = int(-42000)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + int");
    numvar = 1234;
    intvar = 1234567890;
    res = numvar + intvar;
    expected = Number(1234569124);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + long");
    numvar = 1234;
    longvar = 12344321;
    res = numvar + longvar;
    expected = Number(12345555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    numvar = 1234;
    longvar = 2147482414;
    res = numvar + longvar;
    expected = Number(2147483648)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Number + float");
    numvar = 1234;
    flovar = 4321;
    res = numvar + flovar;
    expected = Number(5555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    numvar = 1234;
    flovar = 4321.1;
    res = numvar + flovar;
    expected = Number(5555.1)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


///////////////////////////////////////////////////////////////////
/////////////////  String   ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("String + untyped");
    strvar = "1234";
    uvar = 4321;
    res = strvar + uvar;
    expected = "12344321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    strvar = "321";
    uvar = -321;
    res = strvar + uvar;
    expected = "321-321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + boolean");
    strvar = "321";
    boolvar = true;
    res = strvar + boolvar;					// boolean changes to 1
	expected = "321true"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + Number");
    strvar = "32";
    numvar = -12;
    res = strvar + numvar;
    expected = "32-12"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    strvar = "432";
    numvar = 1.1;
    res = strvar + numvar;
    expected = "4321.1"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + String");
    strvar = "4321";
	res = strvar + "1234";
    expected = "43211234"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + Object");
    strvar = "2";
    obvar = "2";
    res = strvar + obvar;
    expected = "22";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + Date");
    strvar = "";
    datevar = new Date(0);
    res = strvar + datevar;
    expected = "Wed Dec 31 16:00:00 PST 1969";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }

	
    apInitScenario("String + Array");
    strvar = "1";
    // arrvar initialized above
	res = strvar + arrvar;
    expected = "12.2";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + function");
    strvar = "\n";
    // funvar initialized above
	res = strvar + funvar;
    expected = "\nfunction funvar(){var foo = 1}"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + RegExp");
    strvar = "string";
    revar = /\d{2}-\d{2}/g
	res = strvar + revar;
    expected = "string/\\d{2}-\\d{2}/g"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + Error");
    strvar = "false";
    errvar = new Error();
	res = strvar + errvar;
    expected = "falseError"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + byte");
    strvar = "false";
    bytevar = 21;
    res = strvar + bytevar;
    expected = "false21"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + char");
    strvar = "1";
    charvar = "A";
    res = strvar + charvar;
    expected = "1A"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + short");
    strvar = 1000;
    shrtvar = -32000;
    res = strvar + shrtvar;
    expected = "1000-32000"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + int");
    strvar = "";
    intvar = 1234567890;
    res = strvar + intvar;
    expected = "1234567890"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + long");
    strvar = "0";
    longvar = 0;
    res = strvar + longvar;
    expected = "00"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    strvar = "1";
    longvar = 29147482414;
    res = strvar + longvar;
    expected = "129147482414"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("String + float");
    strvar = "bob";
    flovar = -Infinity;
    res = strvar + flovar;
    expected = "bob-Infinity"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    strvar = "true";
    flovar = 512;
    res = strvar + flovar;
    expected = "true512"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


///////////////////////////////////////////////////////////////////
/////////////////  untyped  ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("object + untyped");
    obvar = 1234;
	uvar = 4321;
    res = obvar + uvar;
    expected = int(5555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + boolean");
    obvar = 1234;
    boolvar = true;
    res = obvar + boolvar;						// boolean changes to 1
	expected = int(1235);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + Number");
    obvar = 1234;
    numvar = 4321;
    res = obvar + numvar;
    expected = Number(5555);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    obvar = 1234;
    numvar = 4321.1;
    res = obvar + numvar;
    expected = Number(5555.1);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + String");
    obvar = 1234;
    strvar = "4321";
    res = obvar + strvar;
    expected = "12344321"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + Object");
    obvar = 1234;
    res = obvar + Object(4321);
    expected = int(5555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + Date");
    obvar = 1234;
    datevar = new Date(0);
    res = obvar + datevar;
    expected = "1234Wed Dec 31 16:00:00 PST 1969"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }

	
    apInitScenario("object + Array");
    obvar = 1234;
    // arrvar initialized above
    res = obvar + arrvar;
    expected = "12342.2"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + function");
    obvar = 1234;
    // funvar initialized above
    res = obvar + funvar;
    expected = "1234function funvar(){var foo = 1}"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + RegExp");
    obvar = 1234;
    revar = /\d{2}-\d{2}/g
    res = obvar + revar;
    expected = "1234/\\d{2}-\\d{2}/g"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + Error");
    obvar = 1234;
    errvar = new Error();
    res = obvar + errvar;
    expected = "1234Error"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + byte");
    obvar = 1234;
    bytevar = 21;
    res = obvar + bytevar;
    expected = 1255
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + char");
    obvar = 10;
	charvar = "A";
	res = obvar + charvar;
    expected = "K"
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + short");
    obvar = -10000;
    shrtvar = -32000;
    res = obvar + shrtvar;
    expected = int(-42000)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + int");
    obvar = 1234;
	intvar = 1234567890
    res = obvar + intvar;
    expected = int(1234569124);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + long");
    obvar = 1234;
    longvar = 12344321;
    res = obvar + longvar;
    expected = int(12345555)
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    obvar = 1234;
    longvar = 2147482414;
    res = obvar + longvar;
    expected = 2147483648
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("object + float");
    obvar = 1234;
    flovar = 4321;
    res = obvar + flovar;
    expected = int(5555);
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    obvar = 1234;
    flovar = 4321.1;
    res = obvar + flovar;
    expected = 5555.1;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }

    
///////////////////////////////////////////////////////////////////
/////////////////  Date     ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("Date + untyped");
    datevar = new Date(0);
    uvar = 4321;
    res = datevar + uvar;
    expected = "Wed Dec 31 16:00:00 PST 19694321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + boolean");
    datevar = new Date(0);
    boolvar = true;
    res = datevar + boolvar;
    expected = "Wed Dec 31 16:00:00 PST 1969true";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + Number");
    datevar = new Date(0);
    numvar = NaN;
    res = datevar + numvar;
    expected = "Wed Dec 31 16:00:00 PST 1969NaN";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + String");
    datevar = new Date(0);
    strvar = "4321";
    res = datevar + strvar;
    expected = "Wed Dec 31 16:00:00 PST 19694321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + Object");
    datevar = new Date(0);
    obvar = 4321;
    res = datevar + obvar;
    expected = "Wed Dec 31 16:00:00 PST 19694321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + Date");
    datevar = new Date(0);
    res = datevar + new Date(0);
    expected = "Wed Dec 31 16:00:00 PST 1969Wed Dec 31 16:00:00 PST 1969";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }

	
    apInitScenario("Date + Array");
    datevar = new Date(0);
    // arrvar initialized above
    res = datevar + arrvar;
    expected = "Wed Dec 31 16:00:00 PST 19692.2";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + function");
    datevar = new Date(0);
    // funvar initialized above
    res = datevar + funvar;
    expected = "Wed Dec 31 16:00:00 PST 1969function funvar(){var foo = 1}";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + RegExp");
    datevar = new Date(0);
    revar = /\d{2}-\d{2}/g
    res = datevar + revar;
    expected = "Wed Dec 31 16:00:00 PST 1969/\\d{2}-\\d{2}/g";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + Error");
    datevar = new Date(0);
    errvar = new Error();
    res = datevar + errvar;
    expected = "Wed Dec 31 16:00:00 PST 1969Error";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + byte");
    datevar = new Date(0);
    bytevar = 21;
    res = datevar + bytevar;
    expected = "Wed Dec 31 16:00:00 PST 196921";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + char");
    datevar = new Date(0);
    charvar = "A";
    res = datevar + charvar;
    expected = "Wed Dec 31 16:00:00 PST 1969A";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + short");
    datevar = new Date(0);
    shrtvar = -32000;
    res = datevar + shrtvar;
    expected = "Wed Dec 31 16:00:00 PST 1969-32000";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + int");
    datevar = new Date(0);
    intvar = 4321;
    res = datevar + intvar;
    expected = "Wed Dec 31 16:00:00 PST 19694321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + long");
    datevar = new Date(0);
    longvar = 12344321;
    res = datevar + longvar;
    expected = "Wed Dec 31 16:00:00 PST 196912344321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Date + float");
    datevar = new Date(0);
    flovar = .125;
    res = datevar + flovar;
    expected = "Wed Dec 31 16:00:00 PST 19690.125";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


///////////////////////////////////////////////////////////////////
/////////////////  Array    ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("Array + untyped");
    uvar = 4321;
    res = arrvar + uvar;
    expected = "2.24321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + boolean");
    boolvar = true;
    res = arrvar + boolvar;
    expected = "2.2true";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + Number");
    numvar = 4321;
    res = arrvar + numvar;
    expected = "2.24321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + String");
    strvar = "4321";
    res = arrvar + strvar;
    expected = "2.24321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + Object");
    obvar = 4321;
    res = arrvar + obvar;
    expected = "2.24321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + Date");
    datevar = new Date(0);
    res = arrvar + datevar;
    expected = "2.2Wed Dec 31 16:00:00 PST 1969";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }

	
    apInitScenario("Array + Array");
    res = arrvar + arrvar;
    expected = "2.22.2";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + function");
    // funvar initialized above
    res = arrvar + funvar;
    expected = "2.2function funvar(){var foo = 1}";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + RegExp");
    revar = /\d{2}-\d{2}/g
    res = arrvar + revar;
    expected = "2.2/\\d{2}-\\d{2}/g";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + Error");
    errvar = new Error();
    res = arrvar + errvar;
    expected = "2.2Error";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + byte");
    bytevar = 21;
    res = arrvar + bytevar;
    expected = "2.221";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + char");
    charvar = "A";
    res = arrvar + charvar;
    expected = "2.2A";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + short");
    shrtvar = -32000;
    res = arrvar + shrtvar;
    expected = "2.2-32000";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + int");
    intvar = 4321;
    res = arrvar + intvar;
    expected = "2.24321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + long");
    longvar = 12344321;
    res = arrvar + longvar;
    expected = "2.212344321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Array + float");
    flovar = 4.125;
    res = arrvar + flovar;
    expected = "2.24.125";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


///////////////////////////////////////////////////////////////////
/////////////////  Function ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("Function + untyped");
    uvar = 4321;
    res = funvar + uvar;
    expected = "function funvar(){var foo = 1}4321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + boolean");
    boolvar = true;
    res = funvar + boolvar;
    expected = "function funvar(){var foo = 1}true";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + Number");
    numvar = 4321;
    res = funvar + numvar;
    expected = "function funvar(){var foo = 1}4321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + String");
    strvar = "+4321";
    res = funvar + strvar;
    expected = "function funvar(){var foo = 1}+4321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + Object");
    obvar = 4321;
    res = funvar + obvar;
    expected = "function funvar(){var foo = 1}4321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + Date");
    datevar = new Date(0);
    res = funvar + datevar;
    expected = "function funvar(){var foo = 1}Wed Dec 31 16:00:00 PST 1969";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }

    
    apInitScenario("Function + Array");
    res = funvar + arrvar;
    expected = "function funvar(){var foo = 1}2.2";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + function");
    // funvar initialized above
    res = funvar + funvar;
    expected = "function funvar(){var foo = 1}function funvar(){var foo = 1}";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + RegExp");
    revar = /\d{2}-\d{2}/g
    res = funvar + revar;
    expected = "function funvar(){var foo = 1}/\\d{2}-\\d{2}/g";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + Error");
    errvar = new Error();
    res = funvar + errvar;
    expected = "function funvar(){var foo = 1}Error";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + byte");
    bytevar = 21;
    res = funvar + bytevar;
    expected = "function funvar(){var foo = 1}21";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + char");
    charvar = "A";
    res = funvar + charvar;
    expected = "function funvar(){var foo = 1}A";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + short");
    shrtvar = -32000;
    res = funvar + shrtvar;
    expected = "function funvar(){var foo = 1}-32000";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + int");
    intvar = 4321;
    res = funvar + intvar;
    expected = "function funvar(){var foo = 1}4321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + long");
    longvar = 12344321;
    res = funvar + longvar;
    expected = "function funvar(){var foo = 1}12344321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Function + float");
    flovar = 4321;
    res = funvar + flovar;
    expected = "function funvar(){var foo = 1}4321";
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    }
    apEndTest();
}


function doVerify(a,b) {
  var delta = 1e-4;  
  if (a === b) { return 0; }

  if (typeof(a) == "number" && typeof(b) == "number"){
	if (a<b && (a+delta>b)) return 0;
	if (a>b && (a<b+delta)) return 0;
  }
  return 1;
}


plus03();


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

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


var iTestID = 177153;


//          this tescase tests the |= compound assignment op using combinations
//          of hard typed variables
// 
// Scenarios:
// 
// 1) untyped |= untyped
// 2) untyped |= boolean
// 3) untyped |= Number
// 4) untyped |= String
// 5) untyped |= Object
// 6) untyped |= Date
// 7) untyped |= Array
// 8) untyped |= function
// 9) untyped |= RegExp
// 10) untyped |= Error
// 11) untyped |= byte
// 12) untyped |= char
// 13) untyped |= short
// 14) untyped |= int
// 15) untyped |= long
// 16) untyped |= float
// 17) boolean |= untyped
// 18) boolean |= boolean
// 19) boolean |= Number
// 20) boolean |= String
// 21) boolean |= Object
// 22) boolean |= Date
// 23) boolean |= Array
// 24) boolean |= function
// 25) boolean |= RegExp
// 26) boolean |= Error
// 27) boolean |= byte
// 28) boolean |= char
// 29) boolean |= short
// 30) boolean |= int
// 31) boolean |= long
// 32) boolean |= float
// 33) Number |= untyped
// 34) Number |= boolean
// 35) Number |= Number
// 36) Number |= String
// 37) Number |= Object
// 38) Number |= Date
// 39) Number |= Array
// 40) Number |= function
// 41) Number |= RegExp
// 42) Number |= Error
// 43) Number |= byte
// 44) Number |= char
// 45) Number |= short
// 46) Number |= int
// 47) Number |= long
// 48) Number |= float
// 49) String |= untyped
// 50) String |= boolean
// 51) String |= Number
// 52) String |= String
// 53) String |= Object
// 54) String |= Date
// 55) String |= Array
// 56) String |= function
// 57) String |= RegExp
// 58) String |= Error
// 59) String |= byte
// 60) String |= char
// 61) String |= short
// 62) String |= int
// 63) String |= long
// 64) String |= float
// 65) object |= untyped
// 66) object |= boolean
// 67) object |= Number
// 68) object |= String
// 69) object |= Object
// 70) object |= Date
// 71) object |= Array
// 72) object |= function
// 73) object |= RegExp
// 74) object |= Error
// 75) object |= byte
// 76) object |= char
// 77) object |= short
// 78) object |= int
// 79) object |= long
// 80) object |= float
// 81) Date |= untyped
// 82) Date |= boolean
// 83) Date |= Number
// 84) Date |= String
// 85) Date |= Object
// 86) Date |= Date
// 87) Date |= Array
// 88) Date |= function
// 89) Date |= RegExp
// 90) Date |= Error
// 91) Date |= byte
// 92) Date |= char
// 93) Date |= short
// 94) Date |= int
// 95) Date |= long
// 96) Date |= float
// 97) Array |= untyped
// 98) Array |= boolean
// 99) Array |= Number
// 100) Array |= String
// 101) Array |= Object
// 102) Array |= Date
// 103) Array |= Array
// 104) Array |= function
// 105) Array |= RegExp
// 106) Array |= Error
// 107) Array |= byte
// 108) Array |= char
// 109) Array |= short
// 110) Array |= int
// 111) Array |= long
// 112) Array |= float
// 113) Function |= untyped
// 114) Function |= boolean
// 115) Function |= Number
// 116) Function |= String
// 117) Function |= Object
// 118) Function |= Date
// 119) Function |= Array
// 120) Function |= function
// 121) Function |= RegExp
// 122) Function |= Error
// 123) Function |= byte
// 124) Function |= char
// 125) Function |= short
// 126) Function |= int
// 127) Function |= long
// 128) Function |= float
// 129) RegExp |= untyped
// 130) RegExp |= boolean
// 131) RegExp |= Number
// 132) RegExp |= String
// 133) RegExp |= Object
// 134) RegExp |= Date
// 135) RegExp |= Array
// 136) RegExp |= function
// 137) RegExp |= RegExp
// 138) RegExp |= Error
// 139) RegExp |= byte
// 140) RegExp |= char
// 141) RegExp |= short
// 142) RegExp |= int
// 143) RegExp |= long
// 144) RegExp |= float
// 145) Error |= untyped
// 146) Error |= boolean
// 147) Error |= Number
// 148) Error |= String
// 149) Error |= Object
// 150) Error |= Date
// 151) Error |= Array
// 152) Error |= function
// 153) Error |= RegExp
// 154) Error |= Error
// 155) Error |= byte
// 156) Error |= char
// 157) Error |= short
// 158) Error |= int
// 159) Error |= long
// 160) Error |= float
// 161) byte |= untyped
// 162) byte |= boolean
// 163) byte |= Number
// 164) byte |= String
// 165) byte |= Object
// 166) byte |= Date
// 167) byte |= Array
// 168) byte |= function
// 169) byte |= RegExp
// 170) byte |= Error
// 171) byte |= byte
// 172) byte |= char
// 173) byte |= short
// 174) byte |= int
// 175) byte |= long
// 176) byte |= float
// 177) Char |= untyped
// 178) Char |= boolean
// 179) Char |= Number
// 180) Char |= String
// 181) Char |= Object
// 182) Char |= Date
// 183) Char |= Array
// 184) Char |= function
// 185) Char |= RegExp
// 186) Char |= Error
// 187) Char |= byte
// 188) Char |= char
// 189) Char |= short
// 190) Char |= int
// 191) Char |= long
// 192) Char |= float
// 193) short |= untyped
// 194) short |= boolean
// 195) short |= Number
// 196) short |= String
// 197) short |= Object
// 198) short |= Date
// 199) short |= Array
// 200) short |= function
// 201) short |= RegExp
// 202) short |= Error
// 203) short |= byte
// 204) short |= char
// 205) short |= short
// 206) short |= int
// 207) short |= long
// 208) short |= float
// 209) int |= untyped
// 210) int |= boolean
// 211) int |= Number
// 212) int |= String
// 213) int |= Object
// 214) int |= Date
// 215) int |= Array
// 216) int |= function
// 217) int |= RegExp
// 218) int |= Error
// 219) int |= byte
// 220) int |= char
// 221) int |= short
// 222) int |= int
// 223) int |= long
// 224) int |= float
// 225) long |= untyped
// 226) long |= boolean
// 227) long |= Number
// 228) long |= String
// 229) long |= Object
// 230) long |= Date
// 231) long |= Array
// 232) long |= function
// 233) long |= RegExp
// 234) long |= Error
// 235) long |= sbyte
// 236) long |= char
// 237) long |= short
// 238) long |= int
// 239) long |= long
// 240) long |= float
// 241) float |= untyped
// 242) float |= boolean
// 243) float |= Number
// 244) float |= String
// 245) float |= Object
// 246) float |= Date
// 247) float |= Array
// 248) float |= function
// 249) float |= RegExp
// 250) float |= Error
// 251) float |= byte
// 252) float |= char
// 253) float |= short
// 254) float |= int
// 255) float |= long
// 256) float |= float

              






function asgnor02() {
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
    function funvar(){return 4};
    var revar:RegExp = /\d{2}-\d{2}/g;
    var errvar:Error = new Error;
    var bytevar:byte = 50;
    var sbytevar:sbyte = -50
    var charvar:char = "g";
    var shrtvar:short = 4321;
    var intvar:int = 1234;
    var longvar:long = 321321321321;
    var flovar:float = 1.25;
    var strTM:String = -2146828275;
    var strOF:String = -2146823266;
    var strNE:String = "no errors...";

    apInitTest("AsgnOr02");



///////////////////////////////////////////////////////////////////
/////////////////  RegExp   ///////////////////////////////////////
///////////////////////////////////////////////////////////////////


    apInitScenario("RegExp |= untyped");
    uvar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("revar |= uvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= boolean");
    boolvar = true;
    res = strNE;
    expected = strTM;
    try{eval("revar |= boolvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= Number");
    numvar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("revar |= numvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= String");
    strvar = "4321";
    res = strNE;
    expected = strTM;
    try{eval("revar |= strvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= Object");
    obvar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("revar |= obvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= Date");
    datevar = new Date(0);
    res = strNE;
    expected = strTM;
    try{eval("revar |= datevar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }

    
    apInitScenario("RegExp |= Array");
    res = strNE;
    expected = strTM;
    try{eval("revar |= arrvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= function");
    // revar initialized above
    res = strNE;
    expected = strTM;
    try{eval("revar |= funvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }

// TODOPGM

    apInitScenario("RegExp |= RegExp");
    revar = /\d{2}-\d{2}/g
    res = strNE;
    expected = strOF;
    try{revar |= /d/;}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= Error");
    errvar = new Error();
    res = strNE;
    expected = strTM;
    try{eval("revar |= errvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= byte");
    bytevar = 21;
    res = strNE;
    expected = strTM;
    try{eval("revar |= bytevar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= char");
    charvar = "A";
    res = strNE;
    expected = strTM;
    try{eval("revar |= charvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= short");
    shrtvar = -32000;
    res = strNE;
    expected = strTM;
    try{eval("revar |= shrtvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= int");
    intvar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("revar |= intvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= long");
    longvar = 12344321;
    res = strNE;
    expected = strTM;
    try{eval("revar |= longvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("RegExp |= float");
    flovar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("revar |= flovar");}
    catch(e){res = e.number}
    if (expected != res) {
    apLogFailInfo("wrong return value",expected,res,"");
    }


///////////////////////////////////////////////////////////////////
/////////////////  Error    ///////////////////////////////////////
///////////////////////////////////////////////////////////////////



    apInitScenario("Error |= untyped");
    errvar = new Error();
    uvar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("errvar |= uvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= boolean");
    errvar = new Error();
    boolvar = true;
    res = strNE;
    expected = strTM;
    try{eval("errvar |= boolvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= Number");
    errvar = new Error();
    numvar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("errvar |= numvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= String");
    errvar = new Error();
    strvar = "4321";
    res = strNE;
    expected = strTM;
    try{eval("errvar |= strvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= Object");
    errvar = new Error();
    obvar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("errvar |= obvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= Date");
    errvar = new Error();
    datevar = new Date(0);
    res = strNE;
    expected = strTM;
    try{eval("errvar |= datevar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }

    
    apInitScenario("Error |= Array");
    errvar = new Error();
    // arrvar initialized above
    res = strNE;
    expected = strTM;
    try{eval("errvar |= arrvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= function");
    errvar = new Error();
    // funvar initialized above
    res = strNE;
    expected = strTM;
    try{eval("errvar |= funvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= RegExp");
    errvar = new Error();
    revar = /\d{2}-\d{2}/g
    res = strNE;
    expected = strTM;
    try{eval("errvar |= revar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= Error");
    errvar = new Error();
    res = strNE;
    expected = strTM;
    try{eval("errvar |= new Error()");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= byte");
    errvar = new Error();
    bytevar = 21;
    res = strNE;
    expected = strTM;
    try{eval("errvar |= bytevar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= char");
    errvar = new Error();
    charvar = "A";
    res = strNE;
    expected = strTM;
    try{eval("errvar |= charvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= short");
    errvar = new Error();
    shrtvar = -32000;
    res = strNE;
    expected = strTM;
    try{eval("errvar |= shrtvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= int");
    errvar = new Error();
    intvar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("errvar |= intvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= long");
    errvar = new Error();
    longvar = 12344321;
    res = strNE;
    expected = strTM;
    try{eval("errvar |= longvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Error |= float");
    errvar = new Error();
    flovar = 4321;
    res = strNE;
    expected = strTM;
    try{eval("errvar |= flovar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


///////////////////////////////////////////////////////////////////
/////////////////  Byte     ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("byte |= untyped");
    bytevar = 128;
    uvar = 127;
    bytevar |= uvar;
    expected = byte(255)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }
    bytevar = 123;
    uvar = 432;
    res = strNE;
    expected = strTM;
    try{eval("bytevar |= uvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }
    bytevar = 123;
    uvar = -200;
    res = strNE;
    expected = strTM;
    try{eval("bytevar |= uvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("byte |= boolean");
    bytevar = 122;
    boolvar = true;
    bytevar |= boolvar;                        // boolean changes to 1
    expected = byte(123)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= Number");
    bytevar = 128;
    numvar = 2;
    bytevar |= numvar;
    expected = byte(130)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }
    bytevar = 123;
    numvar = 4.1;
    bytevar |= numvar;
    expected = byte(127)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"285067");
    }


    apInitScenario("byte |= String");
    bytevar = 32;
    strvar = "16";
    bytevar |= strvar;
    expected = 48;
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= Object");
    bytevar = 0;
    obvar = 64;
    bytevar |= obvar;
    expected = byte(64)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= Date");
    bytevar = 48;
    datevar = new Date(24);
    bytevar |= datevar;
    expected = byte(56)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }

    
    apInitScenario("byte |= Array");
    bytevar = 128;
    // arrvar initialized above
    bytevar |= arrvar;
    expected = byte(130)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= function");
    bytevar = 128;
    // funvar initialized above
    bytevar |= funvar();
    expected = byte(132)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= RegExp");
    bytevar = 123;
    revar = /\d{2}-\d{2}/g
    bytevar |= datevar;
    expected = byte(123)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= Error");
    bytevar = 123;
    errvar = new Error();
    bytevar |= datevar;
    expected = byte(123)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= byte");
    bytevar = 128;
    bytevar |= byte(64);
    expected = byte(192)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= char");
    bytevar = 128;
    charvar = "A";
    bytevar |= charvar;
    expected = 193;
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= short");
    bytevar = 1;
    shrtvar = 2;
    bytevar |= shrtvar;
    expected = byte(3)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }
    bytevar = 10;
    shrtvar = 400;
    res = strNE;
    expected = strTM;
    try{eval("bytevar |= shrtvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("byte |= int");
    bytevar = 48;
    intvar = 24;
    bytevar |= intvar;
    expected = byte(56);
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }


    apInitScenario("byte |= long");
    bytevar = 128;
    longvar = 32;
    bytevar |= longvar;
    expected = byte(160)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }
    bytevar = 123;
    longvar = 2147482414;
    res = strNE;
    expected = strTM;
    try{eval("bytevar |= longvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("byte |= float");
    bytevar = 128;
    flovar = 8;
    bytevar |= flovar;
    expected = byte(136)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"");
    }
    bytevar = 128;
    flovar = 8.1;
    bytevar |= flovar;
    expected = byte(136)
    if (doVerify(expected,bytevar)) {
        apLogFailInfo("wrong return value",expected,bytevar,"285067");
    }


///////////////////////////////////////////////////////////////////
/////////////////  Char     ///////////////////////////////////////
///////////////////////////////////////////////////////////////////


    apInitScenario("Char |= untyped");
    charvar = "A";
    uvar = 2;
    charvar |= uvar;
    expected = "C";
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= boolean");
    charvar = "b";
    boolvar = true;
    charvar |= boolvar;                    // boolean changes to 1
    expected = "c"
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= Number");
    charvar = "A";
    numvar = char("B");
    charvar |= numvar;
    expected = char("C");
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }
    charvar = "4";
    numvar = 1.1;
    charvar |= numvar;
    expected = char("5");
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"285067");
    }


    apInitScenario("Char |= String");
    charvar = "a";
    strvar = "5"
    charvar |= strvar;
    expected = "e"
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= Object");
    charvar = "2";
    obvar = String("4");
    charvar |= obvar;
    expected = char(54);
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }
    charvar = 0;
    obvar = char(1);
    charvar |= obvar;
    expected = char(1);
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= Date");
    charvar = 0;
    datevar = new Date(65);
    charvar |= datevar
    expected = char(65);
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }
    
    apInitScenario("Char |= Array");
    charvar = char(64);
    // arrvar initialized above
    charvar |= arrvar
    expected = "B";
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= function");
    charvar = "A";
    // funvar initialized above
    charvar |= funvar()
    expected = "E";
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= RegExp");
    charvar = "s";
    revar = /\d{2}-\d{2}/g
    charvar |= revar
    expected = "s";
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= Error");
    charvar = "f";
    errvar = new Error();
    charvar |= errvar
    expected = "f";
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= byte");
    charvar = "A";
    bytevar = 4;
    charvar |= bytevar;
    expected = "E"
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= char");
    charvar = "A";
    charvar |= char(4);
    expected = "E"
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= short");
    charvar = char("\u0004");
    shrtvar = 65;
    charvar |= shrtvar;
    expected = "E"
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"284766");
    }


    apInitScenario("Char |= int");
    charvar = char("\u0004");
    intvar = 96;
    charvar |= intvar;
    expected = char("\u0064");
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }


    apInitScenario("Char |= long");
    charvar = "A";
    longvar = 4;
    charvar |= longvar;
    expected = "E"
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }
    charvar = "A";
    longvar = -2;
    res = strNE;
    expected = strTM;
    try{eval("charvar |= longvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("Char |= float");
    charvar = "b";
    flovar = 1;
    charvar |= flovar;
    expected = char("c")
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"");
    }
    charvar = char(64);
    flovar = 2.3;
    res = strNE;
    charvar |= flovar;
    expected = char("B")
    if (doVerify(expected,charvar)) {
        apLogFailInfo("wrong return value",expected,charvar,"285067");
    }


///////////////////////////////////////////////////////////////////
/////////////////  short    ///////////////////////////////////////
///////////////////////////////////////////////////////////////////


    apInitScenario("short |= untyped");
    shrtvar = 1234;
    uvar = 4321;
    shrtvar |= uvar;
    expected = short(5363)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= boolean");
    shrtvar = 1234;
    boolvar = true;
    shrtvar |= boolvar;                        // boolean changes to 1
    expected = short(1235)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= Number");
    shrtvar = 1234;
    numvar = 4321;
    shrtvar |= numvar;
    expected = short(5363)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }
    shrtvar = 1234;
    numvar = 4321.1;
    shrtvar |= numvar;
    expected = short(5363)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"285067");
    }


    apInitScenario("short |= String");
    shrtvar = 1234;
    strvar = "4321";
    shrtvar |= strvar;
    expected = short(5363)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= Object");
    shrtvar = 1234;
    obvar = 4321;
    shrtvar |= obvar;
    expected = short(5363)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= Date");
    shrtvar = 1234;
    datevar = new Date(4294971617);
    shrtvar |= datevar;
    expected = short(5363)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }

    
    apInitScenario("short |= Array");
    shrtvar = 512;
    // arrvar initialized above
    shrtvar |= arrvar;
    expected = short(514)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= function");
    shrtvar = 512;
    // funvar initialized above
    shrtvar |= funvar();
    expected = short(516)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= RegExp");
    shrtvar = 1234;
    revar = /\d{2}-\d{2}/g
    shrtvar |= revar;
    expected = short(1234)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= Error");
    shrtvar = 1234;
    errvar = new Error();
    shrtvar |= errvar;
    expected = short(1234)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= byte");
    shrtvar = 4096;
    bytevar = 34;
    shrtvar |= bytevar;
    expected = short(4130)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= char");
    shrtvar = 4096;
    charvar = "A";
    shrtvar |= charvar;
    expected = short(4161)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= short");
    shrtvar = 512;
    shrtvar |= short(256);
    expected = short(768)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }


    apInitScenario("short |= int");
    shrtvar = 1024;
    intvar = 512
    shrtvar |= intvar;
    expected = short(1536);
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }
    shrtvar = 1024;
    intvar = 123456789
    res = strNE;
    expected = strTM;
    try{eval("shrtvar |= intvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("short |= long");
    shrtvar = 1234;
    longvar = 4321;
    shrtvar |= longvar;
    expected = short(5363)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }
    shrtvar = 1234;
    longvar = 2147482414;
    res = strNE;
    expected = strTM;
    try{eval("shrtvar |= longvar");}
    catch(e){res = e.number}
    if (expected != res) {
        apLogFailInfo("wrong return value",expected,res,"");
    }


    apInitScenario("short |= float");
    shrtvar = 1234;
    flovar = 4321;
    shrtvar |= flovar;
    expected = int(5363)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"");
    }
    shrtvar = 1234;
    flovar = 4321.1;
    shrtvar |= flovar;
    expected = int(5363)
    if (doVerify(expected,shrtvar)) {
        apLogFailInfo("wrong return value",expected,shrtvar,"285067");
    }


///////////////////////////////////////////////////////////////////
/////////////////  int      ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("int |= untyped");
    intvar = 1234;
    uvar = 4321;
    intvar |= uvar;
    expected = int(5363)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= boolean");
    intvar = 1234;
    boolvar = true;
    intvar |= boolvar;                        // boolean changes to 1
    expected = int(1235)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= Number");
    intvar = 1234;
    numvar = 4321;
    intvar |= numvar;
    expected = int(5363)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }
    intvar = 1234;
    numvar = 4321.1;
    intvar |= numvar;
    expected = int(5363)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"285067");
    }


    apInitScenario("int |= String");
    intvar = 1234;
    strvar = "4321";
    intvar |= strvar;
    expected = int(5363)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= Object");
    intvar = 1234;
    obvar = 4321;
    intvar |= obvar;
    expected = int(5363)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= Date");
    intvar = 1234;
    datevar = new Date(4321);
    intvar |= datevar;
    expected = int(5363)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }

    
    apInitScenario("int |= Array");
    intvar = 512;
    // arrvar initialized above
    intvar |= arrvar;
    expected = int(514)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= function");
    intvar = 512;
    // funvar initialized above
    intvar |= funvar();
    expected = int(516)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= RegExp");
    intvar = 512;
    revar = /\d{2}-\d{2}/g
    intvar |= revar;
    expected = int(512)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= Error");
    intvar = 1234;
    errvar = new Error();
    intvar |= errvar;
    expected = int(1234)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= byte");
    intvar = 4096;
    bytevar = 4;
    intvar |= bytevar;
    expected = int(4100)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= char");
    intvar = 1073741824;
    charvar = "A";
    intvar |= charvar;
    expected = int(1073741889)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= short");
    intvar = 1073741824;
    shrtvar = 1024;
    intvar |= shrtvar;
    expected = int(1073742848)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= int");
    intvar = 1073742848;
    intvar |= -int(1073742849);
    expected = int(-1);
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }


    apInitScenario("int |= long");
    intvar = 1;
    longvar = 1024;
    intvar |= longvar;
    expected = int(1025)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"unknown expected");
    }
    intvar = -1025;
    longvar = 1024;
    expected = -1;
    intvar |= longvar
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,res,"301998");
    }
    intvar = 1234;
    longvar = 2147483649;
    expected = -2147482413;
    eval("intvar |= longvar");
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,res,"301998");
    }


    apInitScenario("int |= float");
    intvar = 1234;
    flovar = 4321;
    intvar |= flovar;
    expected = int(5363)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"");
    }
    intvar = 1234;
    flovar = 4321.1;
    intvar |= flovar;
    expected = int(5363)
    if (doVerify(expected,intvar)) {
        apLogFailInfo("wrong return value",expected,intvar,"285067");
    }


///////////////////////////////////////////////////////////////////
/////////////////  long     ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("long |= untyped");
    longvar = 1234;
    uvar = 4321;
    longvar |= uvar;
    expected = long(5363)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= boolean");
    longvar = 1234;
    boolvar = true;
    longvar |= boolvar;                        // boolean changes to 1
    expected = long(1235)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= Number");
    longvar = 1234;
    numvar = 4321;
    longvar |= numvar;
    expected = long(5363)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }
    longvar = 1234;
    numvar = 4321.1;
    longvar |= numvar;
    expected = long(5363)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"285067");
    }


    apInitScenario("long |= String");
    longvar = 1234;
    strvar = "4321";
    longvar |= strvar;
    expected = long(5363)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= Object");
    longvar = 1234;
    obvar = 4321;
    longvar |= obvar;
    expected = long(5363)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= Date");
    longvar = 1234;
    datevar = new Date(4321);
    longvar |= datevar;
    expected = long(5363)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }

    
    apInitScenario("long |= Array");
    longvar = 1099511627777;
    // arrvar initialized above
    longvar |= arrvar;
    expected = long(3)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= function");
    longvar = 1099511627778;
    // funvar initialized above
    longvar |= funvar();
    expected = long(1099511627782)
    @if(!@_fast)
      expected = 6
    @end
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= RegExp");
    longvar = 1234;
    revar = /\d{2}-\d{2}/g
    longvar |= revar
    expected = long(1234)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= Error");
    longvar = 1234;
    errvar = new Error();
    longvar |= errvar
    expected = long(1234)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= sbyte");
    longvar = -1029;
    sbytevar = 4;
    longvar |= sbytevar;
    expected = -1025
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= char");
    longvar = 1099511627776;
    charvar = "A";
    longvar |= charvar;
    expected = long(1099511627841)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= short");
    longvar = -3;
    shrtvar = -5;
    longvar |= shrtvar;
    expected = long(-1)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= int");
    longvar = 4294967298;
    intvar = 1024
    longvar |= intvar;
    expected = 4294968322;
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= long");
    longvar = 4294967298;
    longvar |= long(4294967297);
    expected = long(4294967299)
    @if(!@_fast)
      expected = 3
    @end
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }
    longvar = 9223372036854775552;
    longvar |= long(255);
    expected = long(9223372036854775807)
    @if(!@_fast)
      expected = 255
    @end
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }
    longvar = 9223372032559808256;
    longvar |= long(255);
    expected = long(9223372032559808511)
    @if(!@_fast)
      expected = 255
    @end
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }


    apInitScenario("long |= float");
    longvar = 1234;
    flovar = 4321;
    longvar |= flovar;
    expected = long(5363)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"");
    }
    longvar = 1234;
    flovar = 4321.1;
    longvar |= flovar;
    expected = long(5363)
    if (doVerify(expected,longvar)) {
        apLogFailInfo("wrong return value",expected,longvar,"285067");
    }


///////////////////////////////////////////////////////////////////
/////////////////  float    ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("float |= untyped");
    flovar = 1234;
    uvar = 4321;
    flovar |= uvar;
    expected = float(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }
    flovar = 1234.5;
    uvar = 4321;
    flovar |= uvar;
    expected = float(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }
    flovar = 1234;
    uvar = 4321.5;
    flovar |= uvar;
    expected = float(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }
    flovar = 1234.25;
    uvar = 4321.25;
    flovar |= uvar;
    expected = float(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= boolean");
    flovar = 1234;
    boolvar = true;
    flovar |= boolvar;                        // boolean changes to 1
    expected = float(1235)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= Number");
    flovar = 1234;
    numvar = 4321;
    flovar |= numvar;
    expected = float(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }
    flovar = 1234;
    numvar = 4321.1
    flovar |= numvar;
    expected = float(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= String");
    flovar = 1234;
    strvar = "4321";
    flovar |= strvar;
    expected = int(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= Object");
    flovar = 1234;
    obvar = 4321;
    flovar |= obvar;
    expected = int(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= Date");
    flovar = 1234;
    datevar = new Date(4321);
    flovar |= datevar;
    expected = int(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }

    
    apInitScenario("float |= Array");
    flovar = 1024;
    // arrvar initialized above
    flovar |= arrvar;
    expected = int(1026)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= function");
    flovar = 1024.4;
    // funvar initialized above
    flovar |= funvar();
    expected = int(1028)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= RegExp");
    flovar = 1234;
    revar = /\d{2}-\d{2}/g
    flovar |= revar;
    expected = int(1234)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= Error");
    flovar = 1234;
    errvar = new Error();
    flovar |= errvar;
    expected = int(1234)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= byte");
    flovar = 1024.5;
    bytevar = 30;
    flovar |= bytevar;
    expected = float(1054)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= char");
    flovar = 2.9;
    charvar = "A";
    flovar |= charvar;
    expected = float(67)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= short");
    flovar = -.9;
    shrtvar = 20;
    flovar |= shrtvar;
    expected = float(20)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= int");
    flovar = 1.5;
    intvar = 2
    flovar |= intvar;
    expected = float(3);
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= long");
    flovar = 128.128;
    longvar = 9000000512;
    flovar |= longvar;
    expected = Number(9000000512)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }
    flovar = 13;
    longvar = -2147482414;
    flovar |= longvar;
    expected = Number(-2147482401)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }


    apInitScenario("float |= float");
    flovar = 1234.321;
    flovar |= float(4321.321);
    expected = float(5363)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }
    flovar = -33.1;
    flovar |= float(32.9);
    expected = float(-1)
    if (doVerify(expected,flovar)) {
        apLogFailInfo("wrong return value",expected,flovar,"285067");
    }

    apEndTest();
}


function doVerify(a,b) {
  var delta = 1e-5;  
  if (a === b) { return 0; }

  if (typeof(a) == "number" && typeof(b) == "number"){
    if (a<b && (a+delta>b)) return 0;
    if (a>b && (a<b+delta)) return 0;
  }
  return 1;
}
//function FuncRetVal() { return 655.35; }
//function myObject() { this.retVal = myObjVal(); }
//function myObjVal() { return 123321; }


asgnor02();


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

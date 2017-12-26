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


var iTestID = 53109;


// Decimal number to binary string conversion
function bin (num, fAll)
{
	num = parseInt(num);
	if (fAll==null) fAll = false;
    var sBin="";

    for (var i=0; (num>>>i || fAll) && i<32; i++)
        sBin = (num>>>i&1) + sBin;

    return sBin;
}


// Decimal number to hexidecimal string conversion
function hex (num)
{
    num = parseInt(num);
    
    var sHex = "0123456789ABCDEF".charAt(num&0x0f);

    if (num>>>4) sHex = hex(num>>>4) + sHex;
    
    return sHex;
}


// Decimal to octal conversion
function oct (num)
{
    if (num < 0)
        return "-"+oct(Math.abs(num));

    if (num < 8)
        return ""+num;
    else
        return oct(Math.floor(num/8)) + num%8+"";
}


// Hex or oct to decimal conversion
function dec (num)
{
    if (typeof num != "string") return num;
    if (num.charAt(0)!= "0") num = "0x"+num;
    return eval(num);
}


// bin string to dec number conversion
function decBin (sNum)
{
    var val = 0, cb = sNum.length;
    for (var i=0; i<cb; i++)
        if (sNum.charAt(i)=="1") val |= 1<<(cb-1-i);
    return val;
}

// hex string to dec number conversion
// Handles signed int min (0x80000000) to signed int max (0x7fffffff), for
// everything else the behaviour is undefined.  The prefixes "0x"|"0X" are optional.
function decHex (sNum)
{
    var val = 0, cb = sNum.length;
    sNum = sNum.toLowerCase();
    for (var i=(sNum.substring(0,2)=="0x" ? 2 : 0); i<cb; i++)
        val |= "0123456789abcdef".indexOf(sNum.charAt(i))<<(4*(cb-1-i));
    return val;
}

// universal trim function
// defaults to trimming tabs, newlines, and spaces.
function trim (sIn, sTChars)
{
    if (!sIn.length) return sIn;
    if (sTChars == undefined) sTChars = "\t\n ";

    var iSt=-1, iEnd=sIn.length;

    while ( ++iSt<iEnd && sTChars.indexOf(sIn.charAt(iSt)) != -1 ) {}
    while ( --iEnd>iSt && sTChars.indexOf(sIn.charAt(iEnd)) != -1 ) {}

    return sIn.substring(iSt,iEnd+1);
}


// Equvilant to VBA's Chr function
function chr (num)
{
    if (num > 255 || num < 0) return null;

    var sHex = "0123456789abcdef";
    return unescape( "%"+sHex.charAt(num>>4&0x0f)+sHex.charAt(num&0x0f) );
}


// Equivilant to VBA's Asc function
function asc (str)
{
//    return parseInt( escape(str.charAt(0), 0).substring(1,3), 16 );
    return str.charCodeAt(0);
}


// Returns a "proper form" string from the input string sIn.
// The standard token delimiters are tab, newline, space, period, hypen,
// comma, & double/single quotes.  These can be overridden with the optional
// opDlm parameter and added to if the opDlm param contains a '+' char
// as the first char.
function proper (sIn, opDlm)
{
    var i;
    var std = "\t\n .-,\"\'";
    if (opDlm == undefined) opDlm = std;
    var sDlm = opDlm.charAt(0)=='+' ? std+opDlm.substring(1,opDlm.length) : opDlm;
    var ch, ich=0, cb=sIn.length, s=sIn.toLowerCase();

    for (i=0; i<cb; i++)
    {
        ch = s.charAt(i);
        if (sDlm.indexOf(ch)>=0)
            ich = i+1;
        else
            if (ich==i)
                s = s.substring(0,i)+ch.toUpperCase()+s.substring(i+1,cb);
    }
    
    return s;
}


// Equiv to VBA's String function
function bldStr (cb, str)
{
    if ( cb==undefined || 0 == cb || 0 == (""+str).length ) return "";
	if ( str==undefined ) str = " ";

    var sBld = (str + "");

    while (sBld.length * 2 < cb) sBld += sBld;

    if (sBld.length < cb)
        sBld += bldStr(cb - sBld.length, str);
    
    return sBld;
}


// Pads the base arg with as many pad arg's to make a string who's length is equal to n.
// sPad is optional, if not present, a space is used by default.
// Requires:  bldStr
function padStr (base, n, pad)
{
	var cb = n - ("" + base).length;

	if (pad == undefined) pad = " ";

	if (cb > 0 )
	    return bldStr(cb, pad) + base;

    return ""+base;
}


// Formats a string into cbBlk sized sections, delimited by sPad, from the right
// if fRight flag is set, from the left if not.  All args are optional except sSrc.
function fmtSBlock (sSrc, cbBlk, sPad, fRight)
{
    var i;
    // Default parameters
    if (fRight == undefined) fRight = false;
    if (sPad == undefined) sPad = " ";
    if (cbBlk == undefined || 0==cbBlk) cbBlk = sSrc.length;

    var cbMod = fRight ? sSrc.length%cbBlk : 0;

    var sRet = sSrc.substring(0,cbMod);
    
    for (i=0; i<sSrc.length-cbMod; i+=cbBlk)
        sRet += (sPad + sSrc.substring(i+cbMod,i+cbMod+cbBlk));

    return sRet.substring(!cbMod*sPad.length,sRet.length);
}


// Returns the string arg's chars randomly aranged in cbChunk sized chuncks
// cbChunk is optional, defaults to 1
function unsort (sIn, cbChunk)
{
    if (cbChunk == undefined) cbChunk = 1;
	var sOut="", iStrt=0, iEnd=0;
    while (sIn.length)
    {
        iStrt = Math.floor(Math.random()*Math.ceil(sIn.length/cbChunk))*cbChunk;
        iEnd = iStrt+cbChunk;
        sOut += sIn.substring(iStrt,iEnd);
        sIn = sIn.substring(0,iStrt) + (iEnd<sIn.length ? sIn.substring(iEnd, sIn.length) : "");
    }
    return sOut;
}


// Parameterized random string generator
// cb is the number of characters you want the string to return [optional, defaults to 100].
// bfType is a bit mask specifying the characters you want the string to contain [optional, 
// defaults to alphanumeric].  sDomain [optional] is a user supplied source string.
// Requires: chr
function genRndStr (cb, bfType, sDomain)
{
    var i;
    var sSource = "";
    if ( sDomain != undefined && typeof sDomain == "string")
        sSource = sDomain;

    if (bfType == undefined || bfType > 0)
    {
        if (cb == undefined) cb = 100;
        if (bfType == undefined) bfType = 0x01|0x02|0x04;

        var rgfWhichChars = new Array(256);
        for (i=1; i<rgfWhichChars.length; i++)
            rgfWhichChars[i] = 0;

        if (bfType & 0x01) // ucase alpha
            for (i=0x41; i<=0x5A; i++) rgfWhichChars[i]=1;

        if (bfType & 0x02) // lcase alpha
            for (i=0x61; i<=0x7A; i++) rgfWhichChars[i]=1;

        if (bfType & 0x04) // numbers
            for (i=0x30; i<=0x39; i++) rgfWhichChars[i]=1;

        if (bfType & 0x08) // all other printable chars (non-extended)
        {
            for (i=0x21; i<=0x2E; i++) rgfWhichChars[i]=1;
            for (i=0x3A; i<=0x40; i++) rgfWhichChars[i]=1;
            for (i=0x5B; i<=0x60; i++) rgfWhichChars[i]=1;
            for (i=0x7B; i<=0x7E; i++) rgfWhichChars[i]=1;
        }
        if (bfType & 0x10) // space
            rgfWhichChars[0x20]=1;

        if (bfType & 0x20) // tab
            rgfWhichChars[0x09]=1;

        if (bfType & 0x40) // newline
            rgfWhichChars[0x0A]=1;

        if (bfType & 0x80) // all other special chars
        {
            rgfWhichChars[0x08]=1; // backspace
            rgfWhichChars[0x0C]=1; // formfeed
            rgfWhichChars[0x0D]=1; // carriage return
        }
        if (bfType & 0x100) // non-printable non-extended chars
        {
            for (i=0x01; i<=0x07; i++) rgfWhichChars[i]=1;
                                           rgfWhichChars[0x0B]=1;
            for (i=0x0E; i<=0x1F; i++) rgfWhichChars[i]=1;
                                           rgfWhichChars[0x7F]=1;
        }
        if (bfType & 0x200) // extended chars (0x7f-0xff)
            for (i=0x80; i<=0xff; i++) rgfWhichChars[i]=1;

        // Create restricted char string
        for (i=0x00; i<0xff; i++)
            if (rgfWhichChars[i]) sSource += chr(i);
    }

    // Create random result string from restricted char string
    var sRet = "";
    for (i=0; i<cb; i++)
        sRet += sSource.charAt( Math.floor(Math.random()*sSource.length) );

    return sRet;
}

  @cc_on


//One Scenario

//This is truly a scenario which mimicks what a user
//might do with an Array and its length property.
//This test makes sure that the Array length functions properly 
//according to ECMA Spec
//param_Array: used as a Parameter name for the Array as it is passed to a function

var m_scen = "";

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "Scenario failed: "+sMes, sExp, sAct, "");
}

function verifyStats(param_Array, nExp)
{
	var nAct = param_Array.length;

	if (nExp < 0)
		nExp += Math.pow(2,32);
	else if (nExp>4294967295)
		nExp -= Math.pow(2,32);
	if (nAct != nExp)
			apLogFailInfo( m_scen+" failed", nExp, nAct, "");
}


function writeToIndex(param_Array, dIndex, valueToWrite)
{
	param_Array[dIndex] = valueToWrite;
}

function addElement(param_Array, value)
{
	var i_curIndex = param_Array.length++;
	param_Array[i_curIndex] = value;
	verifyStats(param_Array, (i_curIndex + 1));
}

function delElement(param_Array)
{
	var i_curIndex = param_Array.length--;
	verifyStats(param_Array, (i_curIndex - 1));
}


function decreaseLength(param_Array, d_numdecr)
{
	var i_curIndex = param_Array.length;
	param_Array.length-=d_numdecr;
	verifyStats(param_Array, (i_curIndex - d_numdecr));
}

function incrementLength(param_Array)
{
	var i_curIndex = param_Array.length++;
	verifyStats(param_Array, (i_curIndex +1));
}

function increaseLength(param_Array, d_numincr)
{
	var i_curIndex = param_Array.length;
	param_Array.length += d_numincr;

	verifyStats(param_Array, (i_curIndex + d_numincr));
}

function showIndex(param_Array, d_index, ExpectedContents, ExpectedLength)
{
	if (ExpectedLength < 0)
		ExpectedLength += Math.pow(2,32);
	else if (ExpectedLength>4294967295)
		ExpectedLength -= Math.pow(2,32);

	if (param_Array[d_index] != ExpectedContents)
			apLogFailInfo( m_scen+" incorrect contents failed", ExpectedContents, param_Array[d_index], "");

	if (param_Array.length != ExpectedLength)
			apLogFailInfo( m_scen+" incorrect length failed", ExpectedLength, param_Array.length, "");
}


function arlen001() {

    apInitTest("arLength001 ");

	var MyArray = new Array;

	apInitScenario("1. Array Length Tests that verify ECMA spec");

	m_scen = "new Array";
@if(@_fast)		//changes to prototype not allowed in fast mode
	writeToIndex(MyArray, 10, 7);
	showIndex(MyArray, 10, 7, 11);
@else
	Array.prototype[10] = 7;
	showIndex(MyArray, 10, 7, 0);
@end

	m_scen = "MyArray[10] = 15";
	writeToIndex(MyArray, 10, 15);
	showIndex(MyArray, 10, 15, 11);

	m_scen = "MyArray.length--";
	delElement(MyArray);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 10);
@else
	showIndex(MyArray, 10, 7, 10);
@end

	m_scen = "MyArray.length-= 7";
	decreaseLength(MyArray, 7);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 3);
@else
	showIndex(MyArray, 10, 7, 3);
@end

	m_scen = "MyArray[0] = 5";
	writeToIndex(MyArray, 0, 5);
	showIndex(MyArray, 0, 5, 3);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 3);
@else
	showIndex(MyArray, 10, 7, 3);
@end

	m_scen = "MyArray[1] = 3";
	writeToIndex(MyArray, 1, 3);
	showIndex(MyArray, 0, 5, 3);
	showIndex(MyArray, 1, 3, 3);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 3);
@else
	showIndex(MyArray, 10, 7, 3);
@end

	m_scen = "MyArray['3'] = 'test1'";
	writeToIndex(MyArray, '3', "test1");
	showIndex(MyArray, 0, 5, 4);
	showIndex(MyArray, 1, 3, 4);
	showIndex(MyArray, '3', "test1", 4);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 4);
@else
	showIndex(MyArray, 10, 7, 4);
@end

	m_scen = "MyArray['threed'] = 'notsothreed'";
	writeToIndex(MyArray, 'threed', 'notsothreed');
	showIndex(MyArray, 0, 5, 4);
	showIndex(MyArray, 1, 3, 4);
	showIndex(MyArray, '3', "test1", 4);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 4);
@else
	showIndex(MyArray, 10, 7, 4);
@end
	showIndex(MyArray, 'threed', "notsothreed", 4);

	m_scen = "MyArray[5] = 'test'";
	writeToIndex(MyArray, 5, "test");
	showIndex(MyArray, 0, 5, 6);
	showIndex(MyArray, 1, 3, 6);
	showIndex(MyArray, '3', "test1", 6);
	showIndex(MyArray, 5, 'test', 6);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 6);
@else
	showIndex(MyArray, 10, 7, 6);
@end
	showIndex(MyArray, 'threed', "notsothreed", 6);

	m_scen = "MyArray.length--";
	delElement(MyArray);
	showIndex(MyArray, 0, 5, 5);
	showIndex(MyArray, 1, 3, 5);
	showIndex(MyArray, '3', "test1", 5);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 5);
@else
	showIndex(MyArray, 10, 7, 5);
@end
	showIndex(MyArray, 'threed', "notsothreed", 5);

	m_scen = "MyArray.length++";
	incrementLength(MyArray);
	showIndex(MyArray, 0, 5, 6);
	showIndex(MyArray, 1, 3, 6);
	showIndex(MyArray, '3', "test1", 6);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 6);
@else
	showIndex(MyArray, 10, 7, 6);
@end
	showIndex(MyArray, 'threed', "notsothreed", 6);

	m_scen = "MyArray[5] = 45";
	writeToIndex(MyArray, 5, 45);
	showIndex(MyArray, 0, 5, 6);
	showIndex(MyArray, 1, 3, 6);
	showIndex(MyArray, '3', "test1", 6);
	showIndex(MyArray, 5, 45, 6);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 6);
@else
	showIndex(MyArray, 10, 7, 6);
@end
	showIndex(MyArray, 'threed', "notsothreed", 6);

	m_scen = "MyArray.length += 5";
	increaseLength(MyArray, 5);
	showIndex(MyArray, 0, 5, 11);
	showIndex(MyArray, 1, 3, 11);
	showIndex(MyArray, '3', "test1", 11);
	showIndex(MyArray, 5, 45, 11);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 11);
@else
	showIndex(MyArray, 10, 7, 11);
@end
	showIndex(MyArray, 'threed', "notsothreed", 11);
	
	m_scen = "MyArray[10] = 5";
	writeToIndex(MyArray, 10, 5);
	showIndex(MyArray, 0, 5, 11);
	showIndex(MyArray, 1, 3, 11);
	showIndex(MyArray, '3', "test1", 11);
	showIndex(MyArray, 5, 45, 11);
	showIndex(MyArray, 10, 5, 11);
	showIndex(MyArray, 'threed', "notsothreed", 11);
	
	m_scen = "MyArray.length++";
	incrementLength(MyArray);
	showIndex(MyArray, 0, 5, 12);
	showIndex(MyArray, 1, 3, 12);
	showIndex(MyArray, '3', "test1", 12);
	showIndex(MyArray, 5, 45, 12);
	showIndex(MyArray, 10, 5, 12);
	showIndex(MyArray, 'threed', "notsothreed", 12);
	
	m_scen = "MyArray.length -= 12";
	decreaseLength(MyArray, 12);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 0);
@else
	showIndex(MyArray, 10, 7, 0);
@end
	showIndex(MyArray, 'threed', "notsothreed", 0);
	
	m_scen = "MyArray.length -= 1";
	if (@_jscript_version < 5.5){
		decreaseLength(MyArray, 1);
		showIndex(MyArray, 10, 7, -1);
		showIndex(MyArray, 'threed', "notsothreed", -1);
	}else{
		try {MyArray.length -= 1;}
		catch(e) {verify (e.number, -2146823258, "Wrong e.number value");}
	}
	
	m_scen = "MyArray[Math.pow(2,32) -2] = 89";
	writeToIndex(MyArray, Math.pow(2,32) -2, 89);
@if(@_fast)
	showIndex(MyArray, 10, undefined, Math.pow(2,32) -1);
@else
	showIndex(MyArray, 10, 7, Math.pow(2,32) -1);
@end
	showIndex(MyArray, Math.pow(2,32) -2, 89, Math.pow(2,32) -1);
	showIndex(MyArray, 'threed', "notsothreed", Math.pow(2,32) -1);
	
	m_scen = "MyArray[Math.pow(2,33)] = 789";
	writeToIndex(MyArray, Math.pow(2,33), 789);
@if(@_fast)
	showIndex(MyArray, 10, undefined, Math.pow(2,32) -1);
@else
	showIndex(MyArray, 10, 7, Math.pow(2,32) -1);
@end
	showIndex(MyArray, Math.pow(2,32) -2, 89, Math.pow(2,32) -1);
	showIndex(MyArray, Math.pow(2,33), 789, Math.pow(2,32) -1);
	showIndex(MyArray, 'threed', "notsothreed", Math.pow(2,32) -1);

	m_scen = "MyArray.length++";
	if (@_jscript_version < 5.5){
		incrementLength(MyArray);
		showIndex(MyArray, 10, 7, 0);
		showIndex(MyArray, Math.pow(2,33), 789, 0);
		showIndex(MyArray, 'threed', "notsothreed", 0);
	}else{}

	m_scen = "MyArray.length++";
	if (@_jscript_version < 5.5){
		incrementLength(MyArray);
		showIndex(MyArray, 10, 7, 1);
		showIndex(MyArray, Math.pow(2,33), 789, 1);
		showIndex(MyArray, 'threed', "notsothreed", 1);
	}else{ 
              try {incrementLength(MyArray);}
		catch(e) {verify (e.number, -2146823258, "Wrong e.number value");}
	}

	m_scen = "MyArray.length = 0";
	MyArray.length = 0;	//change in functionality:
@if(@_fast)
	showIndex(MyArray, 10, undefined, 0);
@else
	showIndex(MyArray, 10, 7, 0);
@end
	showIndex(MyArray, Math.pow(2,33), 789, 0);
	showIndex(MyArray, 'threed', "notsothreed", 0);
	
	m_scen = "MyArray[10] = 15";
	writeToIndex(MyArray, 10, 15);
	showIndex(MyArray, 10, 15, 11);
	showIndex(MyArray, Math.pow(2,33), 789, 11);
	showIndex(MyArray, 'threed', "notsothreed", 11);

	m_scen = "MyArray.length = 0";
	MyArray.length = 0;
@if(@_fast)
	showIndex(MyArray, 10, undefined, 0);
@else
	showIndex(MyArray, 10, 7, 0);
@end
	showIndex(MyArray, Math.pow(2,33), 789, 0);
	showIndex(MyArray, 'threed', "notsothreed", 0);
	
	m_scen = "MyArray[-1] = 15";
	writeToIndex(MyArray, -1, 15);
	showIndex(MyArray, -1, 15, 0);
@if(@_fast)
	showIndex(MyArray, 10, undefined, 0);
@else
	showIndex(MyArray, 10, 7, 0);
@end
	showIndex(MyArray, Math.pow(2,33), 789, 0);
	showIndex(MyArray, 'threed', "notsothreed", 0);

    apInitScenario("2. bug 151740 - try to construct Array w/length not a positive int");

       m_scen = "MyArray = new Array(1.5)";
       try {MyArray = new Array(1.5);}
       catch(e) {if(e.number != -2146823259) apLogFailInfo(m_scen+" - wrong error thrown", -2146823259, e.number, 151740);}

       m_scen = "MyArray = new Array(-2)";
       try {MyArray = new Array(-2);}
       catch(e) {if(e.number != -2146823259) apLogFailInfo(m_scen+" - wrong error thrown", -2146823259, e.number, 151740);}

    apInitScenario("3. bug 154819 - try to construct Array w/length equal a string");
	//MyArray.length = 'a';  is no longer valid in any version
       m_scen = "MyArray.length = 'a'";
       try {MyArray.length = 'a';}
       catch(e) {if(e.number != -2146823258) apLogFailInfo(m_scen+" - wrong error thrown", -2146823258, e.number, 154819);}

/*****************************************************************************/


    apEndTest();

}


arlen001();


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

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


var iTestID = 73352;


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
/* -------------------------------------------------------------------------
  Lib:		hlpUri
   
		
 
  Component:	JScript
 
  Area: 	URI
 
  Purpose:	Library routines for testing URI encoding and decoding
		methods
 ---------------------------------------------------------------------------
  
 
	[00]	21-Oct-1999	    Angelach: Created
 -------------------------------------------------------------------------*/

// see if the incomming number is representing a uriReserved
// character or #

function isReserved (num)
  {
      if ( num == 35 || num == 36 || num == 38 || num == 43 ||
	   num == 44 || num == 47 || num == 58 || num == 59 ||
	   num == 61 || num == 63 || num == 64 )
	 return true ;
       else
	 return false ;
  }

// see if the incomming number is representing an English alphabet

function isAlpha (num)
  {
      if ( ( num >= 65 && num <= 90 ) || ( num >= 97 && num <= 122  ) )
	 return true ;
       else
	 return false ;
  }

// see if the incomming number is representing a decimal digit

function isNumber (num)
  {
      if ( num >= 48 && num <= 57 )
	 return true ;
       else
	 return false ;
  }

// see if the incomming number is representing a uriMark

function isMark (num)
  {
      if ( num == 33 || num == 39 || num == 40 || num == 41 ||
	   num == 42 || num == 45 || num == 46 || num == 95 || num == 126 )
	 return true ;
       else
	 return false ;
  }

// converting a (pair of) number(s) to an escape sequence.  This function is
// created through observing results from encodeURI:
// num1 is the primary number: expected to be in the range of [0x00, 0xDBFF]
// and [0xE000, 0xFFFF]
// num1 is the secondary number: when num1 is in the range of [0xD800, 0xDBFF],
// it is expected to be in the range of [0xDC00, 0xDFFF]; otherwise, this
// number is ignored
// if num3 is 1, num1 will be checked to see if it is a uriReserved number

function getEncoding (num1, num2, num3)
  {
      var iTmp ;
      var sTmp = "" ;
					    // these are characters that won't be
      if ( isAlpha (num1) || isNumber(num1) || isMark(num1) )
	   return chr(num1) ;		    // these are characters that won't be

      if ( num3 == 1 )
	if ( isReserved(num1) ) 	    // these are uriReserved characters
	  return chr(num1) ;		    // that won't be encoded by encodeURI

      if ( num1 < 0x10 )		    // char in [0, 0x7f] is
	  sTmp = "%0" + hex (num1) ;	    // represented by 0zzzzzzz
      else				    // which is its ASCII value
	if ( num1 < 0x80 )		    // in hex
	   sTmp = "%" + hex (num1) ;	    // char in [0x80, 07ff] is
	else				    // represented by 2 octets:
	  if ( num1 < 0x800 )		    // 110yyyyy 10zzzzzz
	    sTmp = "%" + hex (0xc0+Math.floor(num1/0x40)) + "%" + hex(0x80+(num1 % 0x40)) ;
	  else				    // char in [0x800, 0xd7ff] or [0xe000, 0xffff]
	    if ( num1 < 0xD800 || num1 >= 0xE000 )  // is represented by 3
	      { 			    // octets: 1110xxxx 10yyyyyy 10zzzzzz
		 iTmp = Math.floor(num1 / 0x40) ;
		 sTmp = "%" + hex (0xE0 + Math.floor(iTmp/0x40)) ;
		 sTmp = sTmp + "%" + hex (0x80 + (iTmp % 0x40)) ;
		 sTmp = sTmp + "%" + hex (0x80 + (num1 % 0x40)) ;
	       }			    // char in [0xd800, 0xdbff] followed by
	     else			    // char in [0xdc00, 0xdfff] are represented
	       {			    // by 4 octets: 11110uuu 10uuwwwww 10xxyyyy
		  iTmp = num1 - 0xD7C0 ;    // and 10zzzzzzzz
		  sTmp = "%F" + hex (Math.floor(iTmp / 0x100)) ;
		  sTmp = sTmp + "%" + hex (Math.floor((iTmp - Math.floor(iTmp/0x100) * 0x100)/0x04) + 0x80) ;
		  sTmp = sTmp + "%" + hex(((num1-0xD800) % 0x04)*0x10 + ((Math.floor((num2 - 0xDC00)/0x40)) % 0x10) + 0x80) ;
		  sTmp = sTmp + "%" + hex(num2 % 0x40 + 0x80)
               }

       return sTmp ;
  }

// return true if Exp and Act are identical; otherwise, false is returned
//  this is needed because of posponed bug 314108
@if(@aspx) expando @end   function Verify (Exp, Act, Desc, Bug){
      if ( Act != Exp )
	{
	  if ( Bug )
	    apLogFailInfo (Desc, Exp, Act, Bug) ;
	  else
	    apLogFailInfo (Desc, Exp, Act, "") ;

	  return false ;
	}
      else
	return true ;
  }
/* -------------------------------------------------------------------------
  Test: 	DURIC001
   
		
 
  Component:	JScript
 
  Major Area:	Global method
 
  Test Area:	decodeURIComponent
 
  Keyword:	decodeURIComponent, fromCharCode
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of decodeURIComponent
 
  Scenarios:	1.  decodeURIComponent with an empty string

		2.  decodeURIComponent with an English alphabet

		3.  decodeURIComponent with more than one alphabets

		4.  decodeURIComponent with a decimal digit

		5.  decodeURIComponent with more than one decimal digits

		6.  decodeURIComponent with a #

		7.  decodeURIComponent with more than one #'s

		8.  decodeURIComponent with a uriMark ( -|_|.|!|~|*|'|(|) )

		9.  decodeURIComponent with more than one uriMarks

		10. decodeURIComponent with a uriReserved Char ( ;|/|?|:|@|&|=|+|$|, )

		11. decodeURIComponent with more than one Reserved Chars

		12. decodeURIComponent with a charcter that needs to be encoded

		13. decodeURIComponent with more than one chars that need to be encoded

		14. decodeURIComponent with an 'escaped' character

		15. decodeURIComponent with more than one 'escaped' character

		16. decodeURIComponent with different types of characters

		17. decodeURIComponent with constant

		18. decodeURIComponent with build-in object - not instantiated

		19. decodeURIComponent with build-in object - instantiated

		20. decodeURIComponent with user-defined object

		21. decodeURIComponent with results from escape or encodeURIComponent

		22. decodeURIComponent with argument other than string

		23. decodeURIComponent repeatedly many times

  Abstract:	Call decodeURIComponent and compare its output with expected value;
		verify that decodeURIComponent returns corrected encoded string.  Also
		call decodeURIComponent with output from decodeURIComponent; verify that output
		from decodeURIComponent can be dcoded.
 ---------------------------------------------------------------------------
  Category:	Functionality
 
  Product:	JScript
 
  Related Files: lib.js, string.js, hlpUri.js
 
  Notes:
 ---------------------------------------------------------------------------
  
 
	[00]	01-Nov-1999	    Angelach: Created Test
	[01]	20-Oct-2000	    Angelach: moved scenario 15 to dURIc004
					      moved scenario 16 to dURIc005
 -------------------------------------------------------------------------*/

@cc_on

function duric001 ()
  {
     apInitTest( "dURIc001 ");

     var cPer = "%" ;

     var Str1 = "" ;
     var Str2 = "" ;
     var Str3 = "" ;
     var Str4 = "" ;
     var iTmp = 0 ;
     var jTmp = 0 ;

     var funcObj = new Function ("x", "y", "return(x+y)") ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 1: decodeURIComponent with an empty string");

     Str2 = decodeURIComponent ("") ;
     Verify ( "", Str2, "decodeURIComponent.1.1" ) ; // no decoding is expected

     Str1 = "" ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.1.2" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 2: decodeURIComponent with an alphabet letter");

     Str2 = decodeURIComponent ("h") ;
     Verify ( "h", Str2, "decodeURIComponent.2.1" ) ;// no decoding is expected

     Str2 = decodeURIComponent ("I") ;
     Verify ( "I", Str2, "decodeURIComponent.2.2" ) ;

     Str1 = "V" ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.2.3" ) ;

     Str1 = "m" ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.2.4" ) ;

     Str2 = decodeURIComponent ("n".toUpperCase()) ;
     Verify ( "N", Str2, "decodeURIComponent.2.5" ) ;

     Str2 = decodeURIComponent (String.fromCharCode(102)) ;
     Verify ( "f", Str2, "decodeURIComponent.2.6" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 3: decodeURIComponent with more than 1 alphabet letters");

     Str2 = decodeURIComponent ("decode") ;	     // no decoding is expected
     Verify ( "decode", Str2, "decodeURIComponent.3.1" ) ;

     Str2 = decodeURIComponent ("eeeeeeeeeeeeeeeeeeeee") ;
     Verify ( "eeeeeeeeeeeeeeeeeeeee", Str2, "decodeURIComponent.3.2" ) ;

     Str2 = decodeURIComponent ("URICOMPOMENT") ;
     Verify ( "URICOMPOMENT", Str2, "decodeURIComponent.3.3" ) ;

     Str2 = decodeURIComponent ("VVVVVVVVVVVVVVVVVVVVVV") ;
     Verify ( "VVVVVVVVVVVVVVVVVVVVVV", Str2, "decodeURIComponent.3.4" ) ;

     Str2 = decodeURIComponent ("jQkPlOmfUgThSiRLVKqJpNnModWerIwzByCHtGDxCaZbuFvuGtHsIufVeWdXcYbZEwYcXyBzqKpLrJPkQjRiWhTgoMnNmOlAxDwEvF") ;
     Verify ( "jQkPlOmfUgThSiRLVKqJpNnModWerIwzByCHtGDxCaZbuFvuGtHsIufVeWdXcYbZEwYcXyBzqKpLrJPkQjRiWhTgoMnNmOlAxDwEvF", Str2, "decodeURIComponent.3.5" ) ;

     Str1 = "" ;
     for ( iTmp = 97 ; iTmp < 123 ; iTmp++ )  // all alphabet
       {
	  Str1 = Str1 + String.fromCharCode(iTmp) + String.fromCharCode(iTmp-32) ;
	  Str2 = decodeURIComponent (Str1) ;

	  Verify ( Str1, Str2, "decodeURIComponent.3.6."+iTmp )
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 4: decodeURIComponent with a decimal digit");

     Str2 = decodeURIComponent ("7") ;		     // no encoding is expected
     Verify ( "7", Str2, "decodeURIComponent.4.1" ) ;

     Str1 = "3" ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.4.2" ) ;

     iTmp = 1 ;
     Str2 = decodeURIComponent (iTmp.toString ()) ;
     Verify ( "1", Str2, "decodeURIComponent.4.3" ) ;

     iTmp = 0 ;
     Str2 = decodeURIComponent (iTmp.toString ()) ;
     Verify ( "0", Str2, "decodeURIComponent.4.4" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 5: decodeURIComponent with more than 1 decimal digits");

     Str2 = decodeURIComponent ("2222") ;
     Verify ( "2222", Str2, "decodeURIComponent.5.1" ) ;

     Str2 = decodeURIComponent ("5081726493") ;
     Verify ( "5081726493", Str2, "decodeURIComponent.5.2" ) ;

     Str1 = "555555555555555555555555555555555555555555555555555"
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.5.3" ) ;

     Str1 = "745517645513764911111111111117349103467344444444434633798397349910103463777777777681610"
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.5.4" ) ;

     iTmp = 63481 ;
     Str2 = decodeURIComponent (iTmp) ;
     Verify ( "63481", Str2, "decodeURIComponent.5.5" ) ;

     iTmp = 0xE816 ;
     Str1 = 0xE816 + "" ;
     Str2 = decodeURIComponent (iTmp) ;
     Verify ( Str1, Str2, "decodeURIComponent.5.6" ) ;

     iTmp = 0x800A13A1 ;
     Str1 = 0x800A13A1 + "" ;
     Str2 = decodeURIComponent (iTmp) ;
     Verify ( Str1, Str2, "decodeURIComponent.5.7" ) ;

     Str1 = "" ;
     for ( iTmp = 267 ; iTmp >= 0 ; iTmp-- )
       {
	  jTmp = iTmp % 10 ;

	  Str1 = Str1 + jTmp.toString () ;
	  Str2 = decodeURIComponent (Str1) ;

	  Verify ( Str1, Str2, "decodeURIComponent.5.8."+iTmp )
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 6: decodeURIComponent with a #");

     Str2 = decodeURIComponent ("#") ;		     // no decoding is expected
     Verify ( "#", Str2, "decodeURIComponent.6.1" ) ;

     Str1 = "#" ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.6.2" ) ;

     Str2 = decodeURIComponent (unescape("#")) ;
     Verify ( "#", Str2, "decodeURIComponent.6.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 7: decodeURIComponent with more than 1 #'s");

     Str2 = decodeURIComponent ("##") ;
     Verify ( "##", Str2, "decodeURIComponent.7.1" ) ;

     Str1 = "########################"
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.7.2" ) ;

     Str1 = "#" ;
     for ( iTmp = 0 ; iTmp < 300 ; iTmp++ )
       {
	  Str1 = Str1.concat("#") ;
	  Str2 = decodeURIComponent (Str1) ;

	  Verify ( Str1, Str2, "decodeURIComponent.7.3."+iTmp )
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 8: decodeURIComponent with an uriMark");

     Str2 = decodeURIComponent ("*") ;		     // no decoding is expected
     Verify ( "*", Str2, "decodeURIComponent.8.1" ) ;

     Str1 = "!" ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.8.2" ) ;

     Str2 = decodeURIComponent ("-_.!~*'()".charAt(1)) ;
     Verify ( "_", Str2, "decodeURIComponent.8.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 9: decodeURIComponent with more than 1 uriMarks");

     Str2 = decodeURIComponent ("~~~~~~~~~~~~~~~~~~~~~~~~~~") ;
     Verify ( "~~~~~~~~~~~~~~~~~~~~~~~~~~", Str2, "decodeURIComponent.9.1" ) ;

     Str1 = decodeURIComponent ("(((") ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.9.2" ) ;

     Str2 = decodeURIComponent (" )))))))))))))  ".substring(1, 14)) ;
     Verify ( ")))))))))))))", Str2, "decodeURIComponent.9.3" ) ;

     Str2 = decodeURIComponent ("--_________*___('!.))'))))).!!!!!!!*'~") ;
     Verify ( "--_________*___('!.))'))))).!!!!!!!*'~", Str2, "decodeURIComponent.9.4" ) ;

     Str1 = "('~!._-*~!._--_.!~*'())-_*'().!~()'*~.!~*'(!._--_)-_.!~*)~!._-('*~!._-'(('*))"
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.9.5" ) ;

     Str2 = decodeURIComponent ("abcde'!))*...!-(_.(~__...??".substr(6, 5)) ;
     Verify ( "!))*.", Str2, "decodeURIComponent.9.6" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 10: decodeURIComponent with an uriReserved character");

     Str2 = decodeURIComponent (":") ;		     // no decoding is expected
     Verify ( ":", Str2, "decodeURIComponent.10.1" ) ;

     Str1 = "/" ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.10.2" ) ;

     Str2 = decodeURIComponent (";%^$=+ab?".charAt(8)) ;
     Verify ( "?", Str2, "decodeURIComponent.10.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 11: decodeURIComponent with more than 1 uriReserved characters");

     Str2 = decodeURIComponent ("============================") ;
     Verify ( "============================", Str2, "decodeURIComponent.11.1" ) ;

     Str1 = "&&&&&&" ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.11.2" ) ;

     Str2 = decodeURIComponent ("[[ //?,?@ ]]".substring(3, 9)) ;
     Verify ( "//?,?@", Str2, "decodeURIComponent.11.3" ) ;

     Str2 = decodeURIComponent ("/:;@&=&/$:+/,,,,,,?") ;
     Verify ( "/:;@&=&/$:+/,,,,,,?", Str2, "decodeURIComponent.11.4" ) ;

     Str1 = ";@????@,;=+$/??::::::::::@/??&=:/@::@?$$$$$$$+$," ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.11.5" ) ;

     Str1 = "%;:??&&-+=:&,///434*abe**"
     Str2 = decodeURIComponent (Str1.substr(2, 14)) ;
     Verify ( ":??&&-+=:&,///", Str2, "decodeURIComponent.11.6" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 12: decodeURIComponent with non alpha-numeric, #, uirMark, or uirReserved");

     Str2 = decodeURIComponent (" ") ;		     // no decoding is expected
     Verify ( " ", Str2, "decodeURIComponent.12.1" ) ;

     Str2 = decodeURIComponent ("{") ;
     Verify ( "{", Str2, "decodeURIComponent.12.2" ) ;

     Str2 = decodeURIComponent (String.fromCharCode(3)) ;  // unprintable character
     Str1 = String.fromCharCode(3) ;
     Verify ( Str1, Str2, "decodeURIComponent.12.3" ) ;

     Str2 = decodeURIComponent ('\b') ; 		    // unprintable character
     Verify ( '\b', Str2, "decodeURIComponent.12.4" ) ;

     Str1 = '\n' ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.12.5" ) ;

     Str1 = String.fromCharCode(127) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.12.6" ) ;

     for ( iTmp = 128 ; iTmp < 65536 ; iTmp += 28 )
       {
	 Str1 = String.fromCharCode(128) ;
	 Str2 = decodeURIComponent (Str1) ;
	 Verify ( Str1, Str2, "decodeURIComponent.12.6." + iTmp ) ;
       }

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 13: decodeURIComponent with more than 1 non alpha-numeric, #, uirMark, or uirReserved");

     Str1 = "{<{<{<{<{<{<{<{<" ;	    // in the range of [0, 127]
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.13.1" ) ;

     Str1 = String.fromCharCode(26, 22, 31, 28, 25, 2, 24, 5, 4, 14, 13, 20) ;
     Str1 = Str1 + String.fromCharCode(19, 30, 0, 29, 18, 17, 12, 11, 10, 9) ;
     Str1 = Str1 + String.fromCharCode(8, 15, 7, 23, 21, 16, 6, 27, 3, 1) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.13.2" ) ;

//     Str1 = "ã¿Ú±êÈõ" ;		      // 227, 191, 218, 234, 177, 142, 200, 245
     Str1 = String.fromCharCode(227, 191, 218, 234, 177, 142, 200, 245) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.13.3" ) ;

     Str1 = "" ;
     for ( iTmp = 65535 ; iTmp > 127 ; iTmp -= 30 )
       {
	 Str1 = Str1+ String.fromCharCode(iTmp, iTmp-3, iTmp-6, iTmp-9, iTmp-12, iTmp-15) ;
	 Str1 = Str1+ String.fromCharCode(iTmp-18, iTmp-21, iTmp-24, iTmp-27, iTmp-30) ;
	 Str2 = decodeURIComponent (Str1) ;
	 Verify ( Str1, Str2, "decodeURIComponent.13.4." + iTmp ) ;
       }

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 14: decodeURIComponent with 1 set of 'escaped' character");

     Str2 = decodeURIComponent ("%0C") ;	     // in the range of [0, 127]
     Str1 = String.fromCharCode(0x0C) ;
     Verify ( "\f", Str2, "decodeURIComponent.14.1" ) ;

     Str2 = decodeURIComponent ("%3F") ;
     Verify ( "?", Str2, "decodeURIComponent.14.2" ) ;
     Str3 = encodeURIComponent(Str2) ;
     Verify ( "%3F", Str3, "encodeURIComponent.14.2" ) ;

     Str2 = decodeURIComponent ("%2b") ;
     Verify ( "+", Str2, "encodeURIComponent.14.3" ) ;
     Str3 = encodeURIComponent(Str2) ;
     Verify ( "%2B", Str3, "encodeURIComponent.14.3" ) ;

     Str4 = "%7D" ;
     Str2 = decodeURIComponent (Str4) ;
     Verify ( "}", Str2, "decodeURIComponent.14.4" ) ;
     Str3 = encodeURIComponent(Str2) ;
     Verify ( Str4, Str3, "encodeURIComponent.14.4" ) ;

     Str1 = getEncoding (159) ;
     Str2 = decodeURIComponent (Str1) ; 	     // in the range of [128, 2047]
     Verify ( String.fromCharCode(159), Str2, "encodeURIComponent.14.5" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.5" ) ;

     Str1 = getEncoding (481) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(481), Str2, "encodeURIComponent.14.6" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.6" ) ;

     Str1 = getEncoding (736) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(736), Str2, "encodeURIComponent.14.7" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.7" ) ;

     Str1 = getEncoding (1104) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(1104), Str2, "encodeURIComponent.14.7" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.7" ) ;

     Str1 = getEncoding (1729) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(1729), Str2, "encodeURIComponent.14.8" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.8" ) ;

     Str1 = getEncoding (2040) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(2040), Str2, "encodeURIComponent.14.9" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.9" ) ;

     Str1 = getEncoding (2049) ;	    // in the range of [2048, 55295]
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(2049), Str2, "encodeURIComponent.14.10" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.10" ) ;

     Str1 = getEncoding (5887) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(5887), Str2, "encodeURIComponent.14.11" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.11" ) ;

     Str1 = getEncoding (8743) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(8743), Str2, "encodeURIComponent.14.12" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.12" ) ;

     Str1 = getEncoding (15072) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(15072), Str2, "encodeURIComponent.14.13" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.13" ) ;

     Str1 = getEncoding (26931) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(26931), Str2, "encodeURIComponent.14.14" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.14" ) ;

     Str1 = getEncoding (37538) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(37538), Str2, "encodeURIComponent.14.15" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.15" ) ;

     Str1 = getEncoding (55289) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(55289), Str2, "encodeURIComponent.14.16" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.16" ) ;

     Str1 = getEncoding (57347) ;	    // in the range of [57344, 65535]
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(57347), Str2, "encodeURIComponent.14.17" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.17" ) ;

     Str1 = getEncoding (59064) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(59064), Str2, "encodeURIComponent.14.18" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.18" ) ;

     Str1 = getEncoding (62885) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(62885), Str2, "encodeURIComponent.14.19" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.19" ) ;

     Str1 = getEncoding (65531) ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( String.fromCharCode(65531), Str2, "encodeURIComponent.14.20" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "decodeURIComponent.14.20" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 15: decodeURIComponent with more than 1 set of 'escaped' character");
/* [1]
     Str2 = decodeURIComponent ("%2F%1C%0B%40%2C") ;	 // in the range of [0, 127]
     Str1 = "/" + String.fromCharCode(0x1C, 0x0B) + "@," ;
     Verify ( Str1, Str2, "decodeURIComponent.15.1" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( "%2F%1C%0B%40%2C", Str3, "encodeURIComponent.15.1" ) ;

     Str2 = decodeURIComponent ("%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12") ;
     Str1 = String.fromCharCode(0x12, 0x12, 0x12, 0x12, 0x12, 0x012, 0x12, 0x12) ;
     Str1 = Str1 + String.fromCharCode(0x12, 0x12, 0x12, 0x12, 0x12, 0x12) ;
     Str1 = Str1 + String.fromCharCode(0x12, 0x12, 0x12, 0x12, 0x12, 0x12) ;
     Verify ( Str1, Str2, "decodeURIComponent.15.2" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( "%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12%12", Str3, "encodeURIComponent.15.2" ) ;
					    // in the range of [128, 2047]
     Str4 = getEncoding(0x82)+getEncoding(0xAD)+getEncoding(0x83)+getEncoding(0x47) ;
     Str4 = Str4+getEncoding(0x82)+getEncoding(0xAA)+getEncoding(0x81)+getEncoding(0xC5) ;
     Str2 = decodeURIComponent (Str4) ;
     Str1 = String.fromCharCode (0x82, 0xAD, 0x83, 0x47, 0x82, 0xAA, 0x81, 0xC5) ;
     Verify ( Str1, Str2, "decodeURIComponent.15.3" ) ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "encodeURIComponent.15.3" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 2047 ; iTmp > 127 ; iTmp -= 16 )
       {
	  Str4 = Str4+getEncoding(iTmp-5)+getEncoding(iTmp-11)+getEncoding(iTmp) ;
	  Str4 = Str4+getEncoding(iTmp-14)+getEncoding(iTmp-8)+getEncoding(iTmp-2) ;
	  Str1 = Str1+String.fromCharCode (iTmp-5, iTmp-11, iTmp, iTmp-14, iTmp-8, iTmp-2) ;

	  Str2 = decodeURIComponent (Str4) ;
	  Verify ( Str1, Str2, "decodeURIComponent.15.4."+iTmp ) ;
	  Str3 = encodeURIComponent (Str2) ;
	  Verify ( Str4, Str3, "encodeURIComponent.15.4."+iTmp ) ;
       }

     Str4 = "" ;			    // in the range of [2048, 55295]
     Str1 = "" ;
     for ( iTmp = 55295 ; iTmp > 2047 ; iTmp -= 418 )
       {
	  Str4 = Str4+getEncoding(iTmp-10)+getEncoding(iTmp-186)+getEncoding(iTmp-87) ;
	  Str4 = Str4+getEncoding(iTmp-43)+getEncoding(iTmp-153)+getEncoding(iTmp-373) ;
	  Str4 = Str4+getEncoding(iTmp)+getEncoding(iTmp-32)+getEncoding(iTmp-197) ;
	  Str4 = Str4+getEncoding(iTmp-21)+getEncoding(iTmp-319)+getEncoding(iTmp-131)
	  Str4 = Str4+getEncoding(iTmp-142)+getEncoding(iTmp-54)+getEncoding(iTmp-164) ;
	  Str4 = Str4+getEncoding(iTmp-175)+getEncoding(iTmp-109)+getEncoding(iTmp-98) ;
	  Str4 = Str4+getEncoding(iTmp-208)+getEncoding(iTmp-406)+getEncoding(iTmp-230) ;
	  Str4 = Str4+getEncoding(iTmp-241)+getEncoding(iTmp-384)+getEncoding(iTmp-263) ;
	  Str4 = Str4+getEncoding(iTmp-340)+getEncoding(iTmp-285)+getEncoding(iTmp-296) ;
	  Str4 = Str4+getEncoding(iTmp-307)+getEncoding(iTmp-120)+getEncoding(iTmp-395) ;
	  Str4 = Str4+getEncoding(iTmp-274)+getEncoding(iTmp-351)+getEncoding(iTmp-362) ;
	  Str4 = Str4+getEncoding(iTmp-76)+getEncoding(iTmp-252)+getEncoding(iTmp-329) ;
	  Str4 = Str4+getEncoding(iTmp-219)+getEncoding(iTmp-65)+getEncoding(iTmp-241) ;

	  Str1 = Str1+String.fromCharCode (iTmp-10, iTmp-186, iTmp-87, iTmp-43, iTmp-153) ;
	  Str1 = Str1+String.fromCharCode (iTmp-373, iTmp, iTmp-32, iTmp-197, iTmp-21) ;
	  Str1 = Str1+String.fromCharCode (iTmp-319, iTmp-131, iTmp-142, iTmp-54) ;
	  Str1 = Str1+String.fromCharCode (iTmp-164, iTmp-175, iTmp-109, iTmp-98, iTmp-208) ;
	  Str1 = Str1+String.fromCharCode (iTmp-406, iTmp-230, iTmp-241, iTmp-384, iTmp-263) ;
	  Str1 = Str1+String.fromCharCode (iTmp-340, iTmp-285, iTmp-296, iTmp-307, iTmp-120) ;
	  Str1 = Str1+String.fromCharCode (iTmp-395, iTmp-274, iTmp-351, iTmp-362, iTmp-76) ;
	  Str1 = Str1+String.fromCharCode (iTmp-252, iTmp-329, iTmp-219, iTmp-65, iTmp-241) ;

	  Str2 = decodeURIComponent (Str4) ;
	  Verify ( Str1, Str2, "decodeURIComponent.15.5."+iTmp ) ;
	  Str3 = encodeURIComponent (Str2) ;
	  Verify ( Str4, Str3, "encodeURIComponent.15.5."+iTmp ) ;
       }

     Str4 = "" ;			    // in the range of [55296, 56319]
     Str1 = "" ;			    // followed by [56320, 57343]
     jTmp = 56320 ;
     for ( iTmp = 56319 ; iTmp > 55295 ; iTmp -= 16 )
       {
	  Str4 = Str4+getEncoding(iTmp-7,jTmp)+getEncoding(iTmp,jTmp+11) ;
	  Str4 = Str4+getEncoding(iTmp-11,jTmp+15)+getEncoding(iTmp-15,jTmp+7) ;
	  Str1 = Str1+String.fromCharCode (iTmp-7, jTmp, iTmp, jTmp+11, iTmp-11, jTmp+15, iTmp-15, jTmp+7) ;

	  Str2 = decodeURIComponent (Str4) ;
	  Verify ( Str1, Str2, "decodeURIComponent.15.6."+iTmp ) ;
	  Str3 = encodeURIComponent (Str2) ;
	  Verify ( Str4, Str3, "encodeURIComponent.15.6."+iTmp ) ;

	  jTmp += 16 ;
       }

     Str4 = "" ;			    // in the range of [57344, 65535]
     Str1 = "" ;
     for ( iTmp = 65535 ; iTmp > 57343 ; iTmp -= 32 )
       {
	  Str4 = Str4+getEncoding(iTmp-16)+getEncoding(iTmp-6)+getEncoding(iTmp-11) ;
	  Str4 = Str4+getEncoding(iTmp-26)+getEncoding(iTmp-21)+getEncoding(iTmp)+getEncoding(iTmp-31) ;
	  Str1 = Str1+String.fromCharCode (iTmp-16, iTmp-6, iTmp-11, iTmp-26, iTmp-21, iTmp, iTmp-31) ;

	  Str2 = decodeURIComponent (Str4) ;
	  Verify ( Str1, Str2, "decodeURIComponent.15.7."+iTmp ) ;
	  Str3 = encodeURIComponent (Str2) ;
	  Verify ( Str4, Str3, "encodeURIComponent.15.7."+iTmp ) ;
       }

     for ( iTmp = 0 ; iTmp < 32 ; iTmp++ )
       {
	 Str4 = getEncoding(130+iTmp*60) + getEncoding(2050+iTmp*1664) ;
	 Str4 = Str4 + getEncoding(55300+iTmp*32, 56325+iTmp*32) ;
	 Str4 = Str4 + getEncoding(57345+iTmp*256) + getEncoding(iTmp) ;
	 Str1 = String.fromCharCode(130+iTmp*60, 2050+iTmp*1664, 55300+iTmp*32, 56325+iTmp*32, 57345+iTmp*256, iTmp) ;

	 Str2 = decodeURIComponent (Str4) ;
	 Verify ( Str1, Str2, "decodeURIComponent.15.8." + iTmp ) ;
	 Str3 = encodeURIComponent (Str2) ;
	 Verify ( Str4, Str3, "encodeURIComponent.15.8." + iTmp) ;
       }
*/
    //----------------------------------------------------------------------
     apInitScenario( "Scenario 16: decodeURIComponent with different types of characters");
/* [1]
     Str4 = "http://msdn" + getEncoding(5542) + ".microsoft.com/scripting/" ;
     Str4 = Str4 + getEncoding(982)+getEncoding(17)+getEncoding(61644)+getEncoding(56000, 56813) ;
     Str1 = "http://msdn" + String.fromCharCode(5542) + ".microsoft.com/scripting/" ;
     Str1 = Str1 + String.fromCharCode(982, 17, 61644, 56000, 56813) ;
     Str2 = decodeURIComponent (Str4) ;
     Verify ( Str1, Str2, "decodeURIComponent.16.1") ;

     Str4 = getEncoding(37211) + getEncoding(709) + getEncoding(55394, 56530) ;
     Str4 = Str4 + getEncoding(65109) + getEncoding(16) + "/%07abc.testing.com" ;
     Str1 = String.fromCharCode(37211, 709, 55394, 56530, 65109, 16) ;
     Str1 = Str1 + "/" + String.fromCharCode(0x7) + "abc.testing.com" ;
     Str2 = decodeURIComponent (Str4) ;
     Verify ( Str1, Str2, "decodeURIComponent.16.2") ;

     Str1 = "ftp://^^?;" + String.fromCharCode(20018, 810, 4391, 55612, 56720, 73) ;
     Str1 = Str1 + "." + String.fromCharCode(7420, 1216, 65036) + "===end of Str1" ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.16.3") ;

     Str4 = "%21%23%24" ;		     // build a string containing
     for ( iTmp = 38 ; iTmp < 60 ; iTmp++ )  // #, uriReserveds, uriMarks,
	Str4 = Str4 + "%" + hex(iTmp) ;      // alphas, and digits
     Str4 = Str4 + "%3d" ;
     for ( iTmp = 63 ; iTmp < 91 ; iTmp++ )
	Str4 = Str4 + "%" + hex(iTmp) ;
     Str4 = Str4 + "%5f" ;
     for ( iTmp = 97 ; iTmp < 123 ; iTmp++ )
	Str4 = Str4 + "%" + hex(iTmp) ;
     Str4 = Str4 + "%7e" ;
     Str1 = "!#$&'()*+,-./0123456789:;=?@" ;
     Str1 = Str1 +"ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz~" ;
     Str2 = decodeURIComponent (Str4) ;
     Verify ( Str1, Str2, "decodeURIComponent.16.4" ) ;

     Str1 = "!%23%24%26'()*%2B%2C-.%2F0123456789%3A%3B%3D%3F%40" ;
     Str1 = Str1 +"ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz~" ;
     Str3 = encodeURIComponent (Str2) ;
     Verify ( Str1, Str3, "encodeURIComponent.16.4" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 128 ; iTmp++ )
       {
	 Str4 = Str4 + getEncoding(2049+iTmp*103) + getEncoding(2049+iTmp*273) ;
	 Str4 = Str4 + getEncoding(57345+iTmp*27) + getEncoding(2049+iTmp*341) ;
	 Str4 = Str4 + getEncoding(55298+iTmp*5, 57341-iTmp*1) + getEncoding(132+iTmp*8) ;
	 Str4 = Str4 + getEncoding(57345+iTmp*51) + getEncoding(2049+iTmp*205) ;
	 Str4 = Str4 + getEncoding(2049+iTmp*69) + getEncoding(2049+iTmp*375) ;
	 Str4 = Str4 + getEncoding(57345+iTmp*45) + getEncoding(57345+iTmp*9) ;
	 Str4 = Str4 + getEncoding(55298+iTmp*7, 57341-iTmp*5) + getEncoding(2049+iTmp*171) ;
	 Str4 = Str4 + getEncoding(55298+iTmp*1, 57341-iTmp*3) + getEncoding(iTmp) ;
	 Str4 = Str4 + getEncoding(132+iTmp*14) + getEncoding(57345+iTmp*15) ;
	 Str4 = Str4 + getEncoding(2049+iTmp*409) + getEncoding(132+iTmp*2) ;
	 Str4 = Str4 + getEncoding(2049+iTmp*307) + getEncoding(132+iTmp*11) ;
	 Str4 = Str4 + getEncoding(57345+iTmp*57) + getEncoding(57345+iTmp*39) ;
	 Str4 = Str4 + getEncoding(57345+iTmp*33) + getEncoding(55298+iTmp*3, 57341-iTmp*7) ;
	 Str4 = Str4 + getEncoding(2049+iTmp*239) + getEncoding(132+iTmp*5) ;
	 Str4 = Str4 + getEncoding(57345+iTmp*3) + getEncoding(2049+iTmp*35) ;
	 Str4 = Str4 + getEncoding (2049+iTmp*137) + getEncoding(57345+iTmp*21) ;

	 Str1 = Str1 + String.fromCharCode(2049+iTmp*103, 2049+iTmp*273, 57345+iTmp*27) ;
	 Str1 = Str1 + String.fromCharCode(2049+iTmp*341 ,55298+iTmp*5, 57341-iTmp*1) ;
	 Str1 = Str1 + String.fromCharCode(132+iTmp*8, 57345+iTmp*51, 2049+iTmp*205) ;
	 Str1 = Str1 + String.fromCharCode(2049+iTmp*69, 2049+iTmp*375, 57345+iTmp*45) ;
	 Str1 = Str1 + String.fromCharCode(57345+iTmp*9, 55298+iTmp*7, 57341-iTmp*5) ;
	 Str1 = Str1 + String.fromCharCode(2049+iTmp*171, 55298+iTmp*1, 57341-iTmp*3) ;
	 Str1 = Str1 + String.fromCharCode(iTmp, 132+iTmp*14, 57345+iTmp*15, 2049+iTmp*409) ;
	 Str1 = Str1 + String.fromCharCode(132+iTmp*2, 2049+iTmp*307, 132+iTmp*11) ;
	 Str1 = Str1 + String.fromCharCode(57345+iTmp*57, 57345+iTmp*39, 57345+iTmp*33) ;
	 Str1 = Str1 + String.fromCharCode(55298+iTmp*3, 57341-iTmp*7, 2049+iTmp*239) ;
	 Str1 = Str1 + String.fromCharCode(132+iTmp*5, 57345+iTmp*3, 2049+iTmp*35) ;
	 Str1 = Str1 + String.fromCharCode(2049+iTmp*137, 57345+iTmp*21) ;

	 Str2 = decodeURIComponent (Str4) ;
	 Verify ( Str1, Str2, "decodeURIComponent.16.5." + iTmp ) ;

	 Str3 = encodeURIComponent (Str2) ;
	 Verify ( Str4, Str3, "decodeURIComponent.16.5." + iTmp) ;
       }
*/
    //----------------------------------------------------------------------
     apInitScenario( "Scenario 17: decodeURIComponent with constant");

     Str2 = decodeURIComponent (true) ;
     Verify ( "true", Str2, "decodeURIComponent.17.1" ) ; // no encoding is expected

     Str2 = decodeURIComponent (false) ;
     Verify ( "false", Str2, "decodeURIComponent.17.2" ) ;

     Str2 = decodeURIComponent (null) ;
     Verify ( "null", Str2, "decodeURIComponent.17.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 18: decodeURIComponent with build-in object - not instantiated");

     Str1 = Array ;
     Str2 = decodeURIComponent(Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.1" ) ;

@if(@_jscript_version>=7)
     Str3 = "function%20Array()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D" ;
@else
     Str3 = "%0Afunction%20Array()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@end
     Str2 = decodeURIComponent(Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.2" ) ;

     Str1 = Boolean ;
     Str2 = decodeURIComponent(Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.3" ) ;

@if(@_jscript_version>=7)
     Str3 = "function%20Boolean()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D" ;
@else
     Str3 = "%0Afunction%20Boolean()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@end
     Str2 = decodeURIComponent (Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.4" ) ;

     Str1 = Date ;
     Str2 = decodeURIComponent(Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.5" ) ;

@if(@_jscript_version>=7)
     Str3 = "function%20Date()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D" ;
@else
     Str3 = "%0Afunction%20Date()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@end
     Str2 = decodeURIComponent (Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.6" ) ;

     Str1 = Enumerator ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.7" ) ;

@if(@_jscript_version>=7)
     Str3 = "function%20Enumerator()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D" ;
@else
     Str3 = "%0Afunction%20Enumerator()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@end
     Str2 = decodeURIComponent (Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.8" ) ;

     Str1 = Error ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.9" ) ;

     Str3 = "%5Bobject%20Error%5D" ;
     Str1 = "[object Error]"
     Str2 = decodeURIComponent (Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.10" ) ;

     Str1 = Function ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.11" ) ;

@if(@_jscript_version>=7)
     Str3 = "function%20Function()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D" ;
@else
     Str3 = "%0Afunction%20Function()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@end
     Str2 = decodeURIComponent (Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.12" ) ;

     Str1 = Math ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.13" ) ;

     Str3 = "%5Bobject%20Math%5D" ;
     Str2 = decodeURIComponent (Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.14" ) ;

     Str1 = Number ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.15" ) ;

@if(@_jscript_version>=7)
     Str3 = "function%20Number()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D" ;
@else
     Str3 = "%0Afunction%20Number()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@end
     Str2 = decodeURIComponent (Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.18" ) ;

     Str1 = RegExp ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.17" ) ;

@if(@_jscript_version>=7)
     Str3 = "function%20RegExp()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D" ;
@else
     Str3 = "%0Afunction%20RegExp()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@end
     Str2 = decodeURIComponent (Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.18" ) ;

     Str1 = String ;
     Str2 = decodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.19" ) ;

@if(@_jscript_version>=7)
     Str3 = "function%20String()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D" ;
@else
     Str3 = "%0Afunction%20String()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@end
     Str2 = decodeURIComponent (Str3) ;
     Verify ( Str1, Str2, "decodeURIComponent.18.20" ) ;

    //----------------------------------------------------------------------
     if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
     apInitScenario( "Scenario 19: decodeURIComponent with build-in object - instantiated");

     Str2 = decodeURIComponent( new Array (10) ) ;
     Str1 = ",,,,,,,,," ;
     Verify ( Str1, Str2, "decodeURIComponent.19.1" ) ;

     Str2 = decodeURIComponent( new Array (2, 3, 4) ) ;
     Str1 = "2,3,4" ;
     Verify ( Str1, Str2, "decodeURIComponent.19.2" ) ;

     Str2 = decodeURIComponent (new Boolean (true)) ;
     Str1 = "true" ;
     Verify ( Str1, Str2, "decodeURIComponent.19.3" ) ;

     Str2 = decodeURIComponent (new Date(25542)) ;
     Str1 = "Wed Dec 31 16:00:25 PST 1969" ;
     Verify ( Str1, Str2, "decodeURIComponent.19.4" ) ;

     Str2 = decodeURIComponent (ScriptEngine () ) ;
     Str1 = "JScript" ;
     Verify ( Str1, Str2, "decodeURIComponent.19.5" ) ;
     }

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 20: decodeURIComponent with user-defined object");

     Str2 = decodeURIComponent (funcObj) ;

   //Str1 = "function anonymous(x, y) { return(x+y) }"

     Str1 = "function anonymous(x, y) {\nreturn(x+y)\n}"

     Verify ( Str1, Str2, "decodeURIComponent.20.1" ) ;

     Str2 = decodeURIComponent (new Verify ("a", "a", "c")) ;
     Str1 = "[object Object]" ;
     Verify ( Str1, Str2, "decodeURIComponent.20.2" ) ;

     Str2 = decodeURIComponent (funcObj(3, 5)) ;
     Str1 = "8"
     Verify ( Str1, Str2, "decodeURIComponent.20.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 21: decodeURIComponent with result from escape or encodeURIComponentComponent");

     Str2 = decodeURIComponent (escape("on]0sdX,QBLW^lIDA@xUFwkR<&hb#za4=E7G[%5cVS?NMYiq9e/$1O!62ZP~(\\.J8 CjK*>3pHg\"ftr'yu)`T|:=-}vm;+_{")) ;
     Str1 = "on]0sdX,QBLW^lIDA@xUFwkR<&hb#za4=E7G[%5cVS?NMYiq9e/$1O!62ZP~(\\.J8 CjK*>3pHg\"ftr'yu)`T|:=-}vm;+_{" ;
     Verify ( Str1, Str2, "decodeURIComponent.21.1" ) ;

     Str2 = decodeURIComponent (encodeURIComponent("on]0sdX,QBLW^lIDA@xUFwkR<&hb#za4=E7G[%5cVS?NMYiq9e/$1O!62ZP~(\\.J8 CjK*>3pHg\"ftr'yu)`T|:=-}vm;+_{")) ;
     Str1 = "on]0sdX,QBLW^lIDA@xUFwkR<&hb#za4=E7G[%5cVS?NMYiq9e/$1O!62ZP~(\\.J8 CjK*>3pHg\"ftr'yu)`T|:=-}vm;+_{" ;
     Verify ( Str1, Str2, "decodeURIComponent.21.2" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 22: decodeURIComponent with argument other than string");

     Str2 = decodeURIComponent () ;		     // no argument
     Str1 = "undefined" ;
     Verify ( Str1, Str2, "decodeURIComponent.22.1" ) ;

     Str2 = decodeURIComponent (2147483647) ;	     // numeric value
     Str1 = "2147483647"
     Verify ( Str1, Str2, "decodeURIComponent.22.2" ) ;

     Str2 = decodeURIComponent (1.6e+200*100) ;
     Str1 = "1.6e+202" ;
     Verify ( Str1, Str2, "decodeURIComponent.22.3" ) ;

     Str2 = decodeURIComponent (Math.floor(100.76)) ;
     Str1 = "100" ;
     Verify ( Str1, Str2, "decodeURIComponent.22.4" ) ;

     Str2 = decodeURIComponent (iTmp) ;
     Str1 = iTmp + "" ;
     Verify ( Str1, Str2, "decodeURIComponent.22.5" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 23: decodeURIComponent repeatedly many times");

     Str1 = String.fromCharCode (136, 9051, 58923, 55951, 56951, 16, 52023) + " - Testing! ";
     Str1 = Str1 + "Calling decodeURIComponent many times: @ 100???, == work correctly;" ;
     Str3 = encodeURIComponent (Str1) ;
     for ( iTmp = 0 ; iTmp < 100 ; iTmp++ )
       {
	  Str2 = decodeURIComponent (Str3) ;
	  Verify ( Str1, Str2, "decodeURIComponent.23." + iTmp ) ;
       }

     apEndTest();
  }


duric001();


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

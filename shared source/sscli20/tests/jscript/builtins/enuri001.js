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


var iTestID = 73345;


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
  Test: 	ENURI001
   
		
 
  Component:	JScript
 
  Major Area:	Global method
 
  Test Area:	encodeURI
 
  Keyword:	encodeURI, fromCharCode
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of encodeURI
 
  Scenarios:	1.  encodeURI with an empty string

		2.  encodeURI with an English alphabet

		3.  encodeURI with more than one alphabets

		4.  encodeURI with a decimal digit

		5.  encodeURI with more than one decimal digits

		6.  encodeURI with a #

		7.  encodeURI with more than one #'s

		8.  encodeURI with a uriMark ( -|_|.|!|~|*|'|(|) )

		9.  encodeURI with more than one uriMarks

		10. encodeURI with a uriReserved Char ( ;|/|?|:|@|&|=|+|$|, )

		11. encodeURI with more than one Reserved Chars

		12. encodeURI with a charcter that needs to be encoded

		13. encodeURI with more than one chars that need to be encoded

		14. encodeURI with different types of characters

		15. encodeURI with constant

		16. encodeURI with build-in object - not instantiated

		17. encodeURI with build-in object - instantiated

		18. encodeURI with user-defined object

		19. encodeURI with argument other than string

		20. encodeURI repeatedly many times

  Abstract:	Call encodeURI and compare its output with expected value;
		verify that encodeURI returns corrected encoded string.  Also
		call decodeURI with output from encodeURI; verify that output
		from encodeURI can be dcoded.
 ---------------------------------------------------------------------------
  Category:	Functionality
 
  Product:	JScript
 
  Related Files: lib.js, string.js, hlpUri.js
 
  Notes:
 ---------------------------------------------------------------------------
  
 
	[00]	21-Oct-1999	    Angelach: Created Test
 -------------------------------------------------------------------------*/

function enuri001 ()
  {
     apInitTest( "enURI001 ");

     var cPer = "%" ;

     var Str1 = "" ;
     var Str2 = "" ;
     var Str3 = "" ;
     var Str4 = "" ;
     var iTmp = 0 ;
     var jTmp = 0 ;

     var funcObj = new Function ("x", "y", "return(x+y)") ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 1: encodeURI with an empty string");

     Str2 = encodeURI ("") ;
     Verify ( "", Str2, "encodeURI.1.1" ) ; // no encoding is expected

     Str1 = "" ;
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.1.2" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 2: encodeURI with an alphabet letter");

     Str2 = encodeURI ("e") ;
     Verify ( "e", Str2, "encodeURI.2.1" ) ;// no encoding is expected

     Str2 = encodeURI ("O") ;
     Verify ( "O", Str2, "encodeURI.2.2" ) ;

     Str1 = "R" ;
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.2.3" ) ;

     Str1 = "y" ;
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.2.4" ) ;

     Str2 = encodeURI ("h".toUpperCase()) ;
     Verify ( "H", Str2, "encodeURI.2.5" ) ;

     Str2 = encodeURI (String.fromCharCode(115)) ;
     Verify ( "s", Str2, "encodeURI.2.6" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 3: encodeURI with more than 1 alphabet letters");

     Str2 = encodeURI ("dkfienvcl") ;	    // no encoding is expected
     Verify ( "dkfienvcl", Str2, "encodeURI.3.1" ) ;

     Str2 = encodeURI ("gggggggggggggggggggggggggggggggggg") ;
     Verify ( "gggggggggggggggggggggggggggggggggg", Str2, "encodeURI.3.2" ) ;

     Str2 = encodeURI ("YIKLPWMEHSAQ") ;
     Verify ( "YIKLPWMEHSAQ", Str2, "encodeURI.3.3" ) ;

     Str2 = encodeURI ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX") ;
     Verify ( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX", Str2, "encodeURI.3.4" ) ;

     Str2 = encodeURI ("zZyYxXwWvVuUtTsSrRqqQppPooOnnNmmMllLkKKjJJiIIhHHgGGffFFeeEEddDDccCCbbBBaaaAA") ;
     Verify ( "zZyYxXwWvVuUtTsSrRqqQppPooOnnNmmMllLkKKjJJiIIhHHgGGffFFeeEEddDDccCCbbBBaaaAA", Str2, "encodeURI.3.5" ) ;

     Str1 = "A" ;
     for ( iTmp = 66 ; iTmp < 91 ; iTmp++ ) // all upper case alphabets
       {
          Str1 = Str1.concat(String.fromCharCode(iTmp)) ;
          Str2 = encodeURI (Str1) ;

	  Verify ( Str1, Str2, "encodeURI.3.6."+iTmp )
	}

     for ( iTmp = 97 ; iTmp < 123 ; iTmp++ ) // lower case alphabets
       {
          Str1 = Str1.concat(String.fromCharCode(iTmp)) ;
          Str2 = encodeURI (Str1) ;

	  Verify ( Str1, Str2, "encodeURI.3.7."+iTmp )
        } 

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 4: encodeURI with a decimal digit");

     Str2 = encodeURI ("7") ;		    // no encoding is expected
     Verify ( "7", Str2, "encodeURI.4.1" ) ;

     Str1 = "0" ;
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.4.3" ) ;

     iTmp = 2 ;
     Str2 = encodeURI (iTmp.toString ()) ;
     Verify ( "2", Str2, "encodeURI.4.4" ) ;

     iTmp = 7 ;
     Str2 = encodeURI (iTmp.toString ()) ;
     Verify ( "7", Str2, "encodeURI.4.5" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 5: encodeURI with more than 1 decimal digits");

     Str2 = encodeURI ("11111") ;
     Verify ( "11111", Str2, "encodeURI.5.1" ) ;

     Str2 = encodeURI ("0192837465") ;
     Verify ( "0192837465", Str2, "encodeURI.5.2" ) ;

     Str1 = "888888888888888888888888888888888888888888888888888888888888888"
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.5.3" ) ;

     Str1 = "083467485100000398459273889731111784834195694899999999999999348884586245459258468701736534545904"
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.5.4" ) ;

     iTmp = 13 ;
     Str2 = encodeURI (iTmp) ;
     Verify ( "13", Str2, "encodeURI.5.5" ) ;

     Str2 = encodeURI (0x7FE) ;
     Verify ( "2046", Str2, "encodeURI.5.6" ) ;

     iTmp = 0xD872 ;
     Str1 = 0xD872 + "" ;
     Str2 = encodeURI (iTmp) ;
     Verify ( Str1, Str2, "encodeURI.5.7" ) ;

     iTmp = 0x8200FE15 ;
     Str1 = 0x8200FE15 + "" ;
     Str2 = encodeURI (iTmp) ;
     Verify ( Str1, Str2, "encodeURI.5.8" ) ;

     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 300 ; iTmp++ )
       {
	  jTmp = iTmp % 10 ;

	  Str1 = Str1.concat(jTmp.toString ()) ;
          Str2 = encodeURI (Str1) ;

	  Verify ( Str1, Str2, "encodeURI.5.9."+iTmp )
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 6: encodeURI with a #");

     Str2 = encodeURI ("#") ;		    // no encoding is expected
     Verify ( "#", Str2, "encodeURI.6.1" ) ;

     Str1 = "#" ;
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.6.2" ) ;

     Str2 = encodeURI (unescape("#")) ;
     Verify ( "#", Str2, "encodeURI.6.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 7: encodeURI with more than 1 #'s");

     Str2 = encodeURI ("#####################################") ;
     Verify ( "#####################################", Str2, "encodeURI.7.1" ) ;

     Str1 = "##"
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.7.2" ) ;

     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 500 ; iTmp++ )
       {
	  Str1 = Str1 + "#" ;
          Str2 = encodeURI (Str1) ;

	  Verify ( Str1, Str2, "encodeURI.7.3."+iTmp )
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 8: encodeURI with an uriMark");

     Str2 = encodeURI ("(") ;		    // no encoding is expected
     Verify ( "(", Str2, "encodeURI.8.1" ) ;

     Str1 = "!" ;
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.8.2" ) ;

     Str2 = encodeURI ("-_.!~*'()".charAt(2)) ;
     Verify ( ".", Str2, "encodeURI.8.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 9: encodeURI with more than 1 uriMarks");

     Str2 = encodeURI ("---------------------------------------") ;
     Verify ( "---------------------------------------", Str2, "encodeURI.9.1" ) ;

     Str1 = encodeURI ("*********************") ;
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.9.2" ) ;

     Str2 = encodeURI ("   ~~~~~~~~   ".substring(3, 11)) ;
     Verify ( "~~~~~~~~", Str2, "encodeURI.9.3" ) ;

     Str2 = encodeURI ("('--___!.!.!.**~~~**')") ;
     Verify ( "('--___!.!.!.**~~~**')", Str2, "encodeURI.9.4" ) ;

     Str1 = "-_.!~*'())('*~!._--_.!~*'())('*~!._--_.!~*'())('*~!._--_.!~*'())('*~!._-"
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.9.5" ) ;

     Str2 = encodeURI ("1234567890 '!!-((~___))*....... ?".substr(11, 20)) ;
     Verify ( "'!!-((~___))*.......", Str2, "encodeURI.9.6" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 10: encodeURI with an uriReserved character");

     Str2 = encodeURI ("@") ;		    // no encoding is expected
     Verify ( "@", Str2, "encodeURI.10.1" ) ;

     Str1 = "=" ;
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.10.2" ) ;

     Str2 = encodeURI (";%^$=+abc".charAt(4)) ;
     Verify ( "=", Str2, "encodeURI.10.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 11: encodeURI with more than 1 uriReserved characters");

     Str2 = encodeURI ("+++") ;
     Verify ( "+++", Str2, "encodeURI.11.1" ) ;

     Str1 = encodeURI (";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;") ;
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.11.2" ) ;

     Str2 = encodeURI ("//////$$$$$$$$$$$$$$///////".substring(6, 20)) ;
     Verify ( "$$$$$$$$$$$$$$", Str2, "encodeURI.11.3" ) ;

     Str2 = encodeURI ("=:;/,?@+$&;?&=") ;
     Verify ( "=:;/,?@+$&;?&=", Str2, "encodeURI.11.4" ) ;

     Str1 = ";//???::::@@@@@&&&&&&=======++++++++$$$$$$$$$,,,,,,,,,,"
     Str2 = encodeURI (Str1) ;
     Verify ( Str1, Str2, "encodeURI.11.5" ) ;

     Str1 = "% abc ,;$/+?=:&@@@?=+,/412***"
     Str2 = encodeURI (Str1.substr(6, 16)) ;
     Verify ( ",;$/+?=:&@@@?=+,", Str2, "encodeURI.11.6" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 12: encodeURI with non alpha-numeric, #, uirMark, or uirReserved");

     Str2 = encodeURI (" ") ;		    // encoding is expected
     Verify ( "%20", Str2, "encodeURI.12.1" ) ;  // asc code of " " in hex
     Str3 = decodeURI (Str2) ;		    // because encoding result of ehar's
     Verify ( " ", Str3, "decodeURI.12.1" ) ; // in the range of [0, 127] is the
					    // ascii value of the character
     Str2 = encodeURI ("\b") ;
     Verify ( "%08", Str2, "encodeURI.12.2" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( "\b", Str3, "decodeURI.12.2" ) ;

     Str2 = encodeURI (cPer) ;
     Verify ( "%25", Str2, "encodeURI.12.3" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( "%", Str3, "decodeURI.12.3" ) ;

     Str4 = "\r" ;
     Str2 = encodeURI (Str4) ;
     Str1 = "%0D" ;
     Verify ( Str1, Str2, "encodeURI.12.4" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.12.4" ) ;

     Str4 = String.fromCharCode(94) ;
     Str2 = encodeURI (Str4) ;
     Str1 = cPer + hex(94) ;		    // convert 94 into hex and add
     Verify ( Str1, Str2, "encodeURI.12.5" ) ; // a leading %
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.12.5" ) ;

     Str2 = encodeURI (String.fromCharCode(5)) ;  // unprintable character
     Str1 = getEncoding (5) ;		      // expected encoded uri
     Verify ( Str1, Str2, "encodeURI.12.6" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(5), Str3, "decodeURI.12.6" ) ;

     Str2 = encodeURI (String.fromCharCode(127)) ;
     Str1 = getEncoding (127) ;
     Verify ( Str1, Str2, "encodeURI.12.7" ) ; // compare result with expected
     Str3 = decodeURI (Str2) ;		    // value; also check with decodeURI
     Verify ( String.fromCharCode(127), Str3, "decodeURI.12.7" ) ;

     Str2 = encodeURI (String.fromCharCode(128)) ; // in the range of [128, 2047]
     Str1 = getEncoding (128) ;
     Verify ( Str1, Str2, "encodeURI.12.8" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(128), Str3, "decodeURI.12.8" ) ;

     Str2 = encodeURI (String.fromCharCode(169)) ;
     Str1 = getEncoding (169) ;
     Verify ( Str1, Str2, "encodeURI.12.9" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(169), Str3, "decodeURI.12.9" ) ;

     Str2 = encodeURI (String.fromCharCode(255)) ;
     Str1 = getEncoding (255) ;
     Verify ( Str1, Str2, "encodeURI.12.10" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(255), Str3, "decodeURI.12.10" ) ;

     Str2 = encodeURI (String.fromCharCode(739)) ;
     Str1 = getEncoding (739) ;
     Verify ( Str1, Str2, "encodeURI.12.11" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(739), Str3, "decodeURI.12.11" ) ;

     Str2 = encodeURI (String.fromCharCode(1600)) ;
     Str1 = getEncoding (1600) ;
     Verify ( Str1, Str2, "encodeURI.12.12" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(1600), Str3, "decodeURI.12.12" ) ;

     Str2 = encodeURI (String.fromCharCode(2001)) ;
     Str1 = getEncoding (2001) ;
     Verify ( Str1, Str2, "encodeURI.12.13" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(2001), Str3, "decodeURI.12.13" ) ;

     Str2 = encodeURI (String.fromCharCode(2047)) ;
     Str1 = getEncoding (2047) ;
     Verify ( Str1, Str2, "encodeURI.12.14" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(2047), Str3, "decodeURI.12.14" ) ;

     Str2 = encodeURI (String.fromCharCode(0x800)) ;// in the range of
     Str1 = getEncoding (0x800) ;		    // [2048, 55295]
     Verify ( Str1, Str2, "encodeURI.12.15" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(0x800), Str3, "decodeURI.12.15" ) ;

     Str2 = encodeURI (String.fromCharCode(3305)) ;
     Str1 = getEncoding (3305) ;
     Verify ( Str1, Str2, "encodeURI.12.16" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(3305), Str3, "decodeURI.12.16" ) ;

     Str2 = encodeURI (String.fromCharCode(8218)) ;
     Str1 = getEncoding (8218) ;
     Verify ( Str1, Str2, "encodeURI.12.17" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(8218), Str3, "decodeURI.12.17" ) ;

     Str2 = encodeURI (String.fromCharCode(0x2868)) ;
     Str1 = getEncoding (0x2868) ;
     Verify ( Str1, Str2, "encodeURI.12.18" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(0x2868), Str3, "decodeURI.12.18" ) ;

     Str2 = encodeURI (String.fromCharCode(28168)) ;
     Str1 = getEncoding (28168) ;
     Verify ( Str1, Str2, "encodeURI.12.19" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(28168), Str3, "decodeURI.12.19" ) ;

     Str2 = encodeURI (String.fromCharCode(37495)) ;
     Str1 = getEncoding (37495) ;
     Verify ( Str1, Str2, "encodeURI.12.20" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(37495), Str3, "decodeURI.12.20" ) ;

     Str2 = encodeURI (String.fromCharCode(40017)) ;
     Str1 = getEncoding (40017) ;
     Verify ( Str1, Str2, "encodeURI.12.21" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(40017), Str3, "decodeURI.12.21" ) ;

     Str2 = encodeURI (String.fromCharCode(55295)) ;
     Str1 = getEncoding (55295) ;
     Verify ( Str1, Str2, "encodeURI.12.22" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(55295), Str3, "decodeURI.12.22" ) ;

     Str2 = encodeURI (String.fromCharCode(57344)) ;// in the range of
     Str1 = getEncoding (57344) ;		    // [57344, 65535]
     Verify ( Str1, Str2, "encodeURI.12.23" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(57344), Str3, "decodeURI.12.23" ) ;

     Str2 = encodeURI (String.fromCharCode(59653)) ;
     Str1 = getEncoding (59653) ;
     Verify ( Str1, Str2, "encodeURI.12.24" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(59653), Str3, "decodeURI.12.24" ) ;

     Str2 = encodeURI (String.fromCharCode(62662)) ;
     Str1 = getEncoding (62662) ;
     Verify ( Str1, Str2, "encodeURI.12.25" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(62662), Str3, "decodeURI.12.25" ) ;

     Str2 = encodeURI (String.fromCharCode(0xFFFF)) ;
     Str1 = getEncoding (0xFFFF) ;
     Verify ( Str1, Str2, "encodeURI.12.26" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(0xFFFF), Str3, "decodeURI.12.26" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 13: encodeURI with more than 1 'encodeable' characters");

     Str2 = encodeURI ("<>|[]") ;	    // in the range of [0, 127]
     Verify ( "%3C%3E%7C%5B%5D", Str2, "encodeURI.13.1" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( "<>|[]", Str3, "decodeURI.13.1" ) ;

     Str2 = encodeURI ("||||||||||||||||||||||||||||||||||||||||") ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 40 ; iTmp++ )
	Str1 = Str1 + "%7C" ;
     Verify ( Str1, Str2, "encodeURI.13.2" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( "||||||||||||||||||||||||||||||||||||||||", Str3, "decodeURI.13.2" ) ;

   //Str2 = encodeURI ("«·Êäùÿì") ;	    // 171 183 202 228 249 255 236
     Str2 = encodeURI (String.fromCharCode(171, 183, 202, 228, 249, 255, 236)) ;
     Str1 = getEncoding (171)+getEncoding(183)+getEncoding(202)+getEncoding (228) ;
     Str1 = Str1+getEncoding(249)+getEncoding(255)+getEncoding (236) ;
     Verify ( Str1, Str2, "encodeURI.13.3" ) ; // in the range of [128, 2047]
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(171, 183, 202, 228, 249, 255, 236), Str3, "decodeURI.13.3" ) ;

     Str4 = String.fromCharCode(0xb0, 0xea, 0xab, 0x65, 0xa4, 0xa4) ;
     Str1 = getEncoding(0xb0) + getEncoding(0xea) + getEncoding(0xab) ;
     Str1 = Str1 + getEncoding(0x65) + getEncoding(0xa4) + getEncoding(0xa4) ;
     Str2 = encodeURI(Str4) ;
     Verify ( Str1, Str2, "encodeURI.13.4" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.4" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 20 ; iTmp++ )
       {
	  Str4 = Str4 + String.fromCharCode(149) ;
	  Str1 = Str1 + getEncoding(149) ;
       }
     Str2 = encodeURI (Str4) ;
     Verify ( Str1, Str2, "encodeURI.13.5" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.5" ) ;

     Str2 = encodeURI (String.fromCharCode(930, 371, 162, 1243)) ;
     Str1 = getEncoding(930)+getEncoding(371)+getEncoding(162)+getEncoding(1243) ;
     Verify ( Str1, Str2, "encodeURI.13.6" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( String.fromCharCode(930, 371, 162, 1243), Str3, "decodeURI.13.5" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 24 ; iTmp++ )
       {
	  Str4 = Str4 + String.fromCharCode(1890) ;
	  Str1 = Str1 + getEncoding(1890) ;
       }
     Str2 = encodeURI (Str4) ;
     Verify ( Str1, Str2, "encodeURI.13.7" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.7" ) ;

     Str4 = String.fromCharCode(2000, 500, 300, 1000, 900, 600, 1500, 800, 400, 700) ;
     Str2 = encodeURI (Str4) ;		    // in the range of [2048, 55295]
     Str1 = getEncoding(2000)+getEncoding(500)+getEncoding(300)+getEncoding(1000)+getEncoding(900) ;
     Str1 = Str1+getEncoding(600)+getEncoding(1500)+getEncoding(800)+getEncoding(400)+getEncoding(700) ;
     Verify ( Str1, Str2, "encodeURI.13.8" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.8" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 15 ; iTmp++ )
       {
	  Str4 = Str4 + String.fromCharCode(7623) ;
	  Str1 = Str1 + getEncoding(7623) ;
       }
     Str2 = encodeURI (Str4) ;
     Verify ( Str1, Str2, "encodeURI.13.9" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.9" ) ;

     Str4 = String.fromCharCode(55200, 44199, 33088, 22977, 11866, 10755, 9644, 8533, 7422, 6311, 5200, 4199, 3088, 2977) ;
     Str2 = encodeURI (Str4) ;
     Str1 = getEncoding(55200)+getEncoding(44199)+getEncoding(33088)+getEncoding(22977) ;
     Str1 = Str1+getEncoding(11866)+getEncoding(10755)+getEncoding(9644)+getEncoding(8533) ;
     Str1 = Str1+getEncoding(7422)+getEncoding(6311)+getEncoding(5200)+getEncoding(4199) ;
     Str1 = Str1+getEncoding(3088)+getEncoding(2977) ;
     Verify ( Str1, Str2, "encodeURI.13.10" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.10" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 12 ; iTmp++ )
       {
	  Str4 = Str4 + String.fromCharCode(55426, 56813) ;
	  Str1 = Str1 + getEncoding(55426, 56813) ;
       }
     Str2 = encodeURI (Str4) ;
     Verify ( Str1, Str2, "encodeURI.13.11" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.11" ) ;

     Str4 = String.fromCharCode(56319, 56320, 55296, 57343, 55808, 56832, 55552, 56351, 56064, 57088) ;
     Str2 = encodeURI (Str4) ;		    // in the range of [55296, 56319][56320, 57343]
     Str1 = getEncoding(56319, 56320)+getEncoding(55296, 57343) ;
     Str1 = Str1+getEncoding(55808, 56832)+getEncoding(55552, 56351) ;
     Str1 = Str1+getEncoding(56064, 57088) ;
     Verify ( Str1, Str2, "encodeURI.13.12" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.12" ) ;

     Str4 = String.fromCharCode(58809, 65530, 57350, 64173, 63162, 59016) ;
     Str2 = encodeURI (Str4) ;		    // in the range of [57344, 65535]
     Str1 = getEncoding(58809)+getEncoding(65530)+getEncoding(57350) ;
     Str1 = Str1+getEncoding(64173)+getEncoding(63162)+getEncoding(59016) ;
     Verify ( Str1, Str2, "encodeURI.13.13" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.13" ) ;

     Str4 = String.fromCharCode(130, 20, 211, 30, 376, 62, 92, 127, 11109, 8226, 55300, 56325, 25, 1004, 58002) ;
     Str2 = encodeURI (Str4) ;		    // mixed characters
     Str1 = getEncoding(130)+getEncoding(20)+getEncoding(211)+getEncoding(30)+getEncoding(376) ;
     Str1 = Str1+getEncoding(62)+getEncoding(92)+getEncoding(127)+getEncoding(11109)+getEncoding(8226) ;
     Str1 = Str1+getEncoding(55300, 56325)+getEncoding(25)+getEncoding(1004)+getEncoding(58002) ;
     Verify ( Str1, Str2, "encodeURI.13.14" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.14" ) ;

     for ( iTmp = 0 ; iTmp < 1024 ; iTmp++ )
       {
	 Str4 = String.fromCharCode(128+iTmp, 2048+iTmp*52, 55296+iTmp, 56320+iTmp, 57344+iTmp*8) ;
	 Str2 = encodeURI (Str4) ;
	 Str1 = getEncoding(128+iTmp) + getEncoding(2048+iTmp*52)
	 Str1 = Str1 + getEncoding(55296+iTmp, 56320+iTmp) ;
	 Str1 = Str1 + getEncoding(57344+iTmp*8) ;
	 Verify ( Str1, Str2, "encodeURI.13.15." + iTmp ) ;
	 Str3 = decodeURI (Str2) ;
	 Verify ( Str4, Str3, "decodeURI.13.15." + iTmp) ;
       }

     Str4 = "%0A%C0%80%80%00" ;
     Str2 = encodeURI (Str4) ;
     Str1 = "%250A%25C0%2580%2580%2500" ;
     Verify ( Str1, Str2, "encodeURI.13.16" ) ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.13.16") ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 14: encodeURI with different types of characters");

     Str4 = "http://msdn" + String.fromCharCode(207) + ".microsoft.com/scripting/" ;
     Str4 = Str4 + "%" + String.fromCharCode(2100, 36729, 56001, 57001, 8) ;
     Str2 = encodeURI (Str4) ;
     Str1 = "http://msdn" + getEncoding(207) + ".microsoft.com/scripting/" ;
     Str1 = Str1 + getEncoding(37) + getEncoding(2100) + getEncoding(36729) ;
     Str1 = Str1 + getEncoding(56001, 57001) + getEncoding(8) ;
     Verify ( Str1, Str2, "encodeURI.14.1") ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.14.1") ;

     Str4 = String.fromCharCode(65432, 55922, 56321, 56003, 57339, 40383, 1999, 31) ;
     Str4 = Str4 + "/?.testing.com" ;
     Str2 = encodeURI (Str4) ;
     Str1 = getEncoding(65432) + getEncoding(55922, 56321) + getEncoding(56003, 57339) ;
     Str1 = Str1 + getEncoding(40383) + getEncoding(1999) + getEncoding(31) ;
     Str1 = Str1 + "/?.testing.com" ;
     Verify ( Str1, Str2, "encodeURI.14.2") ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.14.2") ;

     Str4 = "ftp://@$" + String.fromCharCode(18, 2710, 993, 55824, 56428, 61017, 60) ;
     Str4 = Str4 + "." + String.fromCharCode(722, 11209) + "/the end/" ;
     Str2 = encodeURI (Str4) ;
     Str1 = "ftp://@$" + getEncoding(18) + getEncoding(2710) + getEncoding(993) ;
     Str1 = Str1 + getEncoding(55824, 56428) + getEncoding(61017) + getEncoding(60) ;
     Str1 = Str1 + "." + getEncoding(722) + getEncoding(11209) + "/the%20end/" ;
     Verify ( Str1, Str2, "encodeURI.14.3") ;
     Str3 = decodeURI (Str2) ;
     Verify ( Str4, Str3, "decodeURI.14.3") ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 128 ; iTmp++ )
       {
	 Str4 = Str4 + String.fromCharCode(2048+iTmp*364, 57344+iTmp*34, iTmp, 55296+iTmp*8) ;
	 Str4 = Str4 + String.fromCharCode(57343-iTmp*2, 128+iTmp*12, 55296+iTmp*2) ;
	 Str4 = Str4 + String.fromCharCode(57343-iTmp*6, 128+iTmp*15, 57344+iTmp*16) ;
	 Str4 = Str4 + String.fromCharCode(57344+iTmp*52, 2048+iTmp*130, 2048+iTmp*260) ;
	 Str4 = Str4 + String.fromCharCode(2048+iTmp*26, 57344+iTmp*46, 57344+iTmp*10) ;
	 Str4 = Str4 + String.fromCharCode(55296+iTmp*4, 57343-iTmp*3, 2048+iTmp*78) ;
	 Str4 = Str4 + String.fromCharCode(2048+iTmp*312, 2048+iTmp*104, 128+iTmp*3) ;
	 Str4 = Str4 + String.fromCharCode(128+iTmp*5, 57344+iTmp*64, 57344+iTmp*40) ;
	 Str4 = Str4 + String.fromCharCode(55296+iTmp*3, 57343-iTmp*8, 2048+iTmp*208) ;
	 Str4 = Str4 + String.fromCharCode(2048+iTmp*416, 128+iTmp*10, 2048+iTmp*156) ;
	 Str4 = Str4 + String.fromCharCode(2048+iTmp*52, 55296+iTmp*6, 57343-iTmp*4) ;
	 Str4 = Str4 + String.fromCharCode(57344+iTmp*58, 57344+iTmp*22, 2048+iTmp*182) ;
	 Str4 = Str4 + String.fromCharCode(57344+iTmp*28, 57344+iTmp*4) ;
	 Str2 = encodeURI (Str4) ;

	 Str1 = Str1 + getEncoding(2048+iTmp*364) + getEncoding(57344+iTmp*34) ;
	 Str1 = Str1 + getEncoding(iTmp, 0, 1) + getEncoding(55296+iTmp*8, 57343-iTmp*2) ;
	 Str1 = Str1 + getEncoding(128+iTmp*12) + getEncoding(55296+iTmp*2, 57343-iTmp*6) ;
	 Str1 = Str1 + getEncoding(128+iTmp*15) + getEncoding(57344+iTmp*16) ;
	 Str1 = Str1 + getEncoding(57344+iTmp*52) + getEncoding(2048+iTmp*130) ;
	 Str1 = Str1 + getEncoding(2048+iTmp*260) + getEncoding(2048+iTmp*26) ;
	 Str1 = Str1 + getEncoding(57344+iTmp*46) + getEncoding(57344+iTmp*10) ;
	 Str1 = Str1 + getEncoding(55296+iTmp*4, 57343-iTmp*3) + getEncoding(2048+iTmp*78) ;
	 Str1 = Str1 + getEncoding(2048+iTmp*312) + getEncoding(2048+iTmp*104) ;
	 Str1 = Str1 + getEncoding(128+iTmp*3) + getEncoding(128+iTmp*5) ;
	 Str1 = Str1 + getEncoding(57344+iTmp*64) + getEncoding(57344+iTmp*40) ;
	 Str1 = Str1 + getEncoding(55296+iTmp*3, 57343-iTmp*8) + getEncoding(2048+iTmp*208) ;
	 Str1 = Str1 + getEncoding(2048+iTmp*416) + getEncoding(128+iTmp*10) ;
	 Str1 = Str1 + getEncoding(2048+iTmp*156) + getEncoding(2048+iTmp*52) ;
	 Str1 = Str1 + getEncoding(55296+iTmp*6, 57343-iTmp*4) + getEncoding(57344+iTmp*58) ;
	 Str1 = Str1 + getEncoding(57344+iTmp*22) + getEncoding (2048+iTmp*182) ;
	 Str1 = Str1 + getEncoding(57344+iTmp*28) + getEncoding (57344+iTmp*4) ;
	 Verify ( Str1, Str2, "encodeURI.14.4." + iTmp ) ;

	 Str3 = decodeURI (Str2) ;
	 Verify ( Str4, Str3, "decodeURI.14.4." + iTmp) ;
       }

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 15: encodeURI with constant");

     Str2 = encodeURI (true) ;
     Verify ( "true", Str2, "encodeURI.15.1" ) ; // no encoding is expected

     Str2 = encodeURI (false) ;
     Verify ( "false", Str2, "encodeURI.15.2" ) ;

     Str2 = encodeURI (null) ;
     Verify ( "null", Str2, "encodeURI.15.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 16: encodeURI with build-in object - not instantiated");

     Str2 = encodeURI(Array) ;
     @if (@_jscript_version <7.0)
     	Str1 = "%0Afunction%20Array()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
     @else
	Str1 = "function%20Array()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
	@end
     Verify ( Str1, Str2, "encodeURI.16.1" ) ;

     Str2 = encodeURI (Boolean) ;
 @if (@_jscript_version < 7.0)
     	Str1 = "%0Afunction%20Boolean()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
     @else
	Str1 = "function%20Boolean()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
     @end
     Verify ( Str1, Str2, "encodeURI.16.2" ) ;

     Str2 = encodeURI (Date) ;
@if (@_jscript_version < 7.0)
     	Str1 = "%0Afunction%20Date()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
     @else
	Str1 = "function%20Date()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
     @end
     Verify ( Str1, Str2, "encodeURI.16.3" ) ;
     Str2 = encodeURI (Enumerator) ;
     @if (@_jscript_version < 7.0)
     	Str1 = "%0Afunction%20Enumerator()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
	@else
	Str1 = "function%20Enumerator()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D"
     @end
     Verify ( Str1, Str2, "encodeURI.16.4" ) ;

     Str2 = encodeURI (Error) ;
	@if (@_jscript_version < 7.0)
     	Str1 = "%0Afunction%20Error()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
	@else
	Str1 = "function%20Error()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
	@end
     Verify ( Str1, Str2, "encodeURI.16.5" ) ;

     Str2 = encodeURI (Function) ;
@if (@_jscript_version < 7.0)
     	Str1 = "%0Afunction%20Function()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
	@else
	Str1 = "function%20Function()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
	@end
     Verify ( Str1, Str2, "encodeURI.16.6" ) ;
	 
     Str2 = encodeURI (Math) ;
     	Str1 = "%5Bobject%20Math%5D" ;
     Verify ( Str1, Str2, "encodeURI.16.7" ) ;

     Str2 = encodeURI (Number) ;
	 @if (@_jscript_version < 7.0)
     	Str1 = "%0Afunction%20Number()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
	@else
	Str1 = "function%20Number()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
	@end
     Verify ( Str1, Str2, "encodeURI.16.8" ) ;

     Str2 = encodeURI (RegExp) ;
	 @if (@_jscript_version < 7.0)
     	Str1 = "%0Afunction%20RegExp()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
	@else
	Str1 = "function%20RegExp()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
	@end
     Verify ( Str1, Str2, "encodeURI.16.9" ) ;

     Str2 = encodeURI (String) ;
	 @if (@_jscript_version < 7.0)
     	Str1 = "%0Afunction%20String()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
	@else
	Str1 = "function%20String()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
	@end
     Verify ( Str1, Str2, "encodeURI.16.10" ) ;

    //----------------------------------------------------------------------
     if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
     apInitScenario( "Scenario 17: encodeURI with build-in object - instantiated");

     Str2 = encodeURI( new Array (10) ) ;
     Str1 = ",,,,,,,,," ;
     Verify ( Str1, Str2, "encodeURI.17.1" ) ;

     Str2 = encodeURI( new Array (2, 3, 4) ) ;
     Str1 = "2,3,4" ;
     Verify ( Str1, Str2, "encodeURI.17.2" ) ;

     Str2 = encodeURI (new Boolean (true)) ;
     Str1 = "true" ;
     Verify ( Str1, Str2, "encodeURI.17.3" ) ;

     Str2 = encodeURI (new Date(25542)) ;
     Str1 = "Wed%20Dec%2031%2016:00:25%20PST%201969" ;
     Verify ( Str1, Str2, "encodeURI.17.4" ) ;

     Str2 = encodeURI (new Enumerator () ) ;
     Str1 = "%5Bobject%20Object%5D" ;
     Verify ( Str1, Str2, "encodeURI.17.5" ) ;

     Str2 = encodeURI (ScriptEngine () ) ;
     Str1 = "JScript" ;
     Verify ( Str1, Str2, "encodeURI.17.5" ) ;
     }

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 18: encodeURI with user-defined object");

     Str2 = encodeURI (funcObj) ;
     @if (@_jscript_version < 7.0)
     	Str1 = "function%20anonymous(x,%20y)%20%7B%0Areturn(x+y)%0A%7D"
     @else
	Str1 = "function%20anonymous(x,%20y)%20%7B%0Areturn(x+y)%0A%7D"
     @end
     Verify ( Str1, Str2, "encodeURI.18.1" ) ;

     Str2 = encodeURI (new Verify ("a", "a", "c")) ;
     Str1 = "%5Bobject%20Object%5D" ;
     Verify ( Str1, Str2, "encodeURI.18.2" ) ;

     Str2 = encodeURI (funcObj(3, 5)) ;
     Str1 = "8"
     Verify ( Str1, Str2, "encodeURI.18.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 19: encodeURI with argument other than string");

     Str2 = encodeURI () ;		    // no argument
     Str1 = "undefined" ;
     Verify ( Str1, Str2, "encodeURI.19.1" ) ;

     Str2 = encodeURI (-32768) ;	    // numeric value
     Str1 = "-32768" ;
     Verify ( Str1, Str2, "encodeURI.19.2" ) ;

     Str2 = encodeURI (3e+100*10) ;
     Str1 = "3e+101" ;
     Verify ( Str1, Str2, "encodeURI.19.3" ) ;

     Str2 = encodeURI (Math.ceil(162.97)) ;
     Str1 = "163" ;
     Verify ( Str1, Str2, "encodeURI.19.4" ) ;

     Str2 = encodeURI (iTmp) ;
     Str1 = iTmp + "" ;
     Verify ( Str1, Str2, "encodeURI.19.5" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 20: encodeURI repeatedly many times");

     Str1 = "Calling encodeURI many times (100)\f. Does it work correctly? " ;
     Str1 = Str1 + String.fromCharCode (42932, 1154, 11164, 56145, 56541) + "~~So+Long" ;
     for ( iTmp = 0 ; iTmp < 100 ; iTmp++ )
       {
	  Str2 = encodeURI (Str1) ;
	  Str3 = decodeURI (Str2) ;
	  Verify ( Str1, Str3, "encodeURI.20." + iTmp ) ;
       }

     apEndTest();
  }


enuri001();


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

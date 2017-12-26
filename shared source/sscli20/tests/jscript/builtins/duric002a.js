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


var iTestID = 77344;


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
  Test: 	DURIC002a
   
		
 
  Component:	JScript
 
  Major Area:	Global method
 
  Test Area:	decodeURIComponent
 
  Keyword:	decodeURIComponent, fromCharCode
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of encodeURI
 
  Scenarios:	1.  decodeURIComponent with one or more %'s

		2.  decodeURIComponent with %<single char>

		3.  decodeURIComponent with %<hex> - leading bit is 1

		4.  decodeURIComponent with %<hex>%<hex> - leading hex != C<n> or D<n>

		5.  decodeURIComponent with %<hex>%<hex> - second hex < 0x80

		6.  decodeURIComponent with %<hex>%<hex>%<hex> - leading hex != 0xE<n>

		7.  decodeURIComponent with %<hex>%<hex>%<hex> - second hex < 0x80

		8.  decodeURIComponent with %<hex>%<hex>%<hex> - third hex < 0x80

		9.  decodeURIComponent with %<hex>%<hex>%<hex>%<hex> - leading hex != [0xF0, 0xF4]

		10. decodeURIComponent with %<hex>%<hex>%<hex>%<hex> - 2nd hex < 0x80

		11. decodeURIComponent with %<hex>%<hex>%<hex>%<hex> - 3rd hex < 0x80

		12. decodeURIComponent with %<hex>%<hex>%<hex>%<hex> - 4th hex < 0x80

  Abstract:	Set up error handler then call decodeURIComponent with escape
		sequences that are not recognized by the encoding routines as
		valid encoding representation.	Check error code to ensure that
		proper error was generated. Also check result after the call
		to ensure that no decoding was taken place.
 ---------------------------------------------------------------------------
  Category:	Functionality
 
  Product:	JScript
 
  Related Files: lib.js, string.js, hlpUri.js
 
  Notes:
 ---------------------------------------------------------------------------
  
 
	[00]	04-Nov-1999	    Angelach: Created Test
 -------------------------------------------------------------------------*/

function RunTest (Param)
  {
      var sTmp = "Error = " ;

      try
	{
	  sTmp = decodeURIComponent(Param) ;	 // call decodeURIComponent
	}

      catch ( err )			// here if an error was generated
	{				// record the error and also record
	   sTmp = sTmp + hex(err.number) ; // state of sTmp after the call
	}

      return sTmp ;
  }

function duric002a ()
  {
     apInitTest( "dURIc002 ");

     var Str1 = "" ;
     var Str2 = "Error = 800A13A1" ;
     var Str3 = "" ;
     var iTmp ;
     var jTmp ;
     var kTmp ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 1: decodeURIComponent with one or more %'s") ;

     Str1 = RunTest ("%") ;
     Verify ( Str2, Str1, "decodeURIComponent.1" ) ;

     for ( iTmp = 0 ; iTmp < 100 ; iTmp++ )
       Str3 = Str3 + "%" ;
     Str1 = RunTest (Str3) ;
     Verify ( Str2, Str1, "decodeURIComponent.1.2."+iTmp ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 2: decodeURIComponent with %<single char>") ;

     for ( iTmp = 48 ; iTmp < 58 ; iTmp++ )	    // [%0, %9]
       {
	  Str3 = "%" + String.fromCharCode(iTmp) ;
	  Str1 = RunTest (Str3) ;
	  Verify ( Str2, Str1, "decodeURIComponent.2.1."+iTmp ) ;
       }

     for ( iTmp = 65 ; iTmp < 71 ; iTmp++ )	    // [%A, %F]
       {
	  Str3 = "%" + String.fromCharCode(iTmp) ;
	  Str1 = RunTest (Str3) ;
	  Verify ( Str2, Str1, "decodeURIComponent.2.2."+iTmp ) ;
       }

     for ( iTmp = 97 ; iTmp < 103 ; iTmp++ )	    // [%a, %f]
       {
	  Str3 = "%" + String.fromCharCode(iTmp) ;
	  Str1 = RunTest (Str3) ;
	  Verify ( Str2, Str1, "decodeURIComponent.2.3."+iTmp ) ;
       }

     for ( iTmp = 0 ; iTmp < 65535 ; iTmp += 1024 )  // all other char's
       {
	  Str3 = "%" + String.fromCharCode(iTmp) ;
	  Str1 = RunTest (Str3) ;
	  Verify ( Str2, Str1, "decodeURIComponent.2.4."+iTmp ) ;
       }

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 3: decodeURIComponent with %<hex> - leading bit is 1") ;

     for ( iTmp = 0x80 ; iTmp <= 0xFF  ; iTmp++ )
       {
	  Str3 = "%" + hex(iTmp) ;
	  Str1 = RunTest (Str3) ;
	  Verify ( Str2, Str1, "decodeURIComponent.3."+iTmp ) ;
       }

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 4: decodeURIComponent with %<hex>%<hex> - leading hex != C<n> or D<n>") ;

     for ( iTmp = 0x00 ; iTmp < 0xC2 ; iTmp += 4 )  
	for ( jTmp = 0x80 ; jTmp < 0xC0 ; jTmp +=9 )// According to Unicode Standard
	  {					    // 2.0 (Appendix A):
	     if ( iTmp < 0x10 ) 		    // when iTmp = 0xC0, (str3
	       Str3 = "%0" + hex(iTmp) + "%" + hex(jTmp) ;
	     else				    // = %C0%<nn>, string will
	       Str3 = "%" + hex(iTmp) + "%" + hex(jTmp) ;
	     Str1 = RunTest (Str3) ;		    // be decoded into %(jTmp-0x80)
	     if ( iTmp != 0xC0 )
	       Verify ( Str2, Str1, "decodeURIComponent.4.1."+iTmp+"."+jTmp ) ;
	     else
	       Verify ( String.fromCharCode(jTmp - 0x80), Str1, "decodeURIComponent.4.1."+iTmp+"."+jTmp ) ;
	  }

     for ( iTmp = 0xE0 ; iTmp <= 0xFF ; iTmp += 3 )
	for ( jTmp = 0x80 ; jTmp < 0xC0 ; jTmp +=9 )
	  {
	     Str3 = "%" + hex(iTmp) + "%" + hex(jTmp) ;
	     Str1 = RunTest (Str3) ;
	     Verify ( Str2, Str1, "decodeURIComponent.4.2."+iTmp+"."+jTmp ) ;
	  }

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 5: decodeURIComponent with %<hex>%<hex> - second hex < 0x80") ;

     jTmp = 0 ;
     for ( iTmp = 0xC2 ; iTmp < 0xE0 ; iTmp++ )
	{
	   if ( jTmp < 0x10 )
	     Str3 = "%" + hex(iTmp) + "%0" + hex(jTmp) ;
	   else
	     Str3 = "%" + hex(iTmp) + "%" + hex(jTmp) ;
	   Str1 = RunTest (Str3) ;
	   Verify ( Str2, Str1, "decodeURIComponent.5."+iTmp ) ;

	   jTmp += 4 ;
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 6: decodeURIComponent with %<hex>%<hex> - second hex > 0xBF") ;

     jTmp = 0xFF ;
     for ( iTmp = 0xC2 ; iTmp < 0xE0 ; iTmp++ )
	{
	   Str3 = "%" + hex(iTmp) + "%" + hex(jTmp) ;
	   Str1 = RunTest (Str3) ;
	   Verify ( Str2, Str1, "decodeURIComponent.6."+iTmp ) ;

	   jTmp -= 2 ;
	   if ( jTmp < 0xC0 )
	      jTmp = 0xFE ;
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 7: decodeURIComponent with %<hex>%<hex>%<hex> - leading hex != 0xE<n>") ;

     for ( iTmp = 0x00 ; iTmp < 0xE0 ; iTmp += 6 )
       {
	 kTmp = 0x80 ;
	 for ( jTmp = 0xBF ; jTmp > 0x7F ; jTmp -= 9 )
	   {
	      if ( iTmp < 0x10 )
		Str3 = "%0" + hex(iTmp) + "%" + hex(jTmp) + "%" + hex(kTmp) ;
	      else
		Str3 = "%" + hex(iTmp) + "%" + hex(jTmp) + "%" + hex(kTmp) ;
	      Str1 = RunTest (Str3) ;
	      Verify ( Str2, Str1, "decodeURIComponent.7.1."+iTmp+"."+jTmp ) ;

	      kTmp += 9 ;
	   }
       }

     for ( iTmp = 0xF0 ; iTmp <= 0xFF ; iTmp++ )
       {
	 kTmp = 0x80 ;
	 for ( jTmp = 0xBF ; jTmp > 0x7F ; jTmp -= 9 )
	   {
	      Str3 = "%" + hex(iTmp) + "%" + hex(jTmp) + "%" + hex(kTmp) ;
	      Str1 = RunTest (Str3) ;
	      Verify ( Str2, Str1, "decodeURIComponent.7.2."+iTmp+"."+jTmp ) ;

	      kTmp += 9 ;
	   }
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 8: decodeURIComponent with %<hex>%<hex>%<hex> - second hex < 0x80") ;

     for ( iTmp = 0xE0 ; iTmp < 0xEE ; iTmp++ )
	{
	   kTmp = 0xBF ;
	   for ( jTmp = 0 ; jTmp < 0x80 ; jTmp += 9 )
	     {
	       if ( jTmp < 0x10 )
		 Str3 = "%" + hex(iTmp) + "%0" + hex(jTmp) + "%" + hex(kTmp) ;
	       else
		 Str3 = "%" + hex(iTmp) + "%" + hex(jTmp) + "%" + hex(kTmp) ;
	       Str1 = RunTest (Str3) ;
	       Verify ( Str2, Str1, "decodeURIComponent.8.1."+iTmp+"."+jTmp ) ;
	     }

	   kTmp -= 4 ;
	}

     kTmp = 0xBF ;
     for ( jTmp = 0x9F ; jTmp > 0x7F ; jTmp-- )      // 1st <hex> = 0xE0
	{					     // and 2nd <hex> < 0xA0
	   Str3 = "%E0" + "%" + hex(jTmp) + "%" + hex(kTmp) ;
	   Str1 = RunTest (Str3) ;		     // this will decode to
	   iTmp = (jTmp - 0x80) * 64 + (kTmp - 0x80) ; // characters [0, 2047]
	   Verify ( String.fromCharCode(iTmp), Str1, "decodeURIComponent.8.2."+jTmp ) ;
						     // see scenario 4.1
	   kTmp -= 2 ;
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 9: decodeURIComponent with %<hex>%<hex>%<hex> - second hex > 0xBF") ;

     for ( iTmp = 0xE0 ; iTmp < 0xEE ; iTmp++ )
	{
	   kTmp = 0xBF ;
	   for ( jTmp = 0xFF ; jTmp > 0xBF ; jTmp-- )
	     {
	       Str3 = "%" + hex(iTmp) + "%" + hex(jTmp) + "%" + hex(kTmp) ;
	       Str1 = RunTest (Str3) ;
	       Verify ( Str2, Str1, "decodeURIComponent.9.1."+iTmp+"."+jTmp ) ;

	       kTmp-- ;
	     }
	}

     kTmp = 0xBF ;
     for ( jTmp = 0xA0 ; jTmp < 0xC0 ; jTmp++ )      // 1st <hex> = 0xED
	{					     // and 2nd <hex> > 0x9F
	   Str3 = "%ED" + "%" + hex(jTmp) + "%" + hex(kTmp) ;
	   Str1 = RunTest (Str3) ;
	   Verify ( Str2, Str1, "decodeURIComponent.9.2."+jTmp ) ;

	   kTmp -= 2 ;
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 10: decodeURIComponent with %<hex>%<hex>%<hex> - third hex < 0x80") ;

     for ( iTmp = 0xE0 ; iTmp < 0xEE ; iTmp++ )
	{
	   kTmp = 0xBF ;
	   for ( jTmp = 0 ; jTmp < 0x80 ; jTmp += 9 )
	     {
	       if ( jTmp < 0x10 )
		 Str3 = "%" + hex(iTmp) + "%" + hex(kTmp) + "%0" + hex(jTmp) ;
	       else
		 Str3 = "%" + hex(iTmp) + "%" + hex(kTmp) + "%" + hex(jTmp) ;
	       Str1 = RunTest (Str3) ;
	       Verify ( Str2, Str1, "decodeURIComponent.10."+iTmp+"."+jTmp ) ;
	     }

	   kTmp -= 4 ;
	}

     apEndTest();
  }


duric002a();


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

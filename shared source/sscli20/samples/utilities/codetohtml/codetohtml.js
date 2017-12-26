//------------------------------------------------------------------------------
// <copyright file="codetohtml.js" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------



/* *******************************************************************************
Derived from Peter Torr's formatCodeToHtml JScript sample.

This JScript file can (theoretically) be used to format source code for any 
C-like language. It was written for JScript (and can format itself very nicely!)
but should work with C# and C++.

The default behavior is that extensions .cs and .js are recognized and the 
correct .ini files loaded for that language.  To support another language, 
copy the js_* files and rename them to <lang_extention>_*.  Then modify
the files to match the correct values for the desired language.
  <lang>_codetohtml.ini        Contains the name of the language, the other INI files, 
                               and the CSS file to use.
  <lang>_keywords.ini          Should contain language keywords.
  <lang>_userdefined.ini       Should contain user-defined types to special case.
  <lang>_futurereserved.ini    Should contain future reserved words in the language.

You can also change the files header.txt and footer.txt files to contain whatever you
want for initial and final HTML blocks.

There are two .CSS stylesheets: codeblue and codewhite.  Each simply has a different
background color.  Change the .CSS file name in the <lang_extention>_codetohtml.ini
file to use whichever .CSS you prefer or use the /stylesheet: command-line option.
********************************************************************************* */

import System;
import System.IO;
	
// Our own special characters to be replaced by <, >, and &
var sLt = "\u1000";
var sGt = "\u1001";
var sAmp = "\u1002";
var sSpanStart = sLt + "span";
var sSpanEnd = sLt + "/span" + sGt;

// List of punctuation recognised by the language.
var INI_PUNCTUATION = "punctuation";

// To-end-of-line comment character[s]
var INI_LINE_COMMENT = "line_comment";

// Multi-line comments
var INI_MULTILINE_COMMENT_START = "multiline_comment_start";
var INI_MULTILINE_COMMENT_END = "multiline_comment_end";

// The default INI file -- only useful on the command line
var INI_INI_FILE = "ini";

// file containing keywords
var INI_KEYWORD_FILE = "keywords";

// file containing user-defined keywords
var INI_USERDEFINED_FILE = "userdefined";

// file containing future reserved words
var INI_FUTURERESERVED_FILE = "futurereserved";

// file containing user-specified replacements in the output file
var INI_USERREPLACEMENTS_FILE = "replace";

// file to insert at the start of the output
var INI_HEADER_FILE = "header";

// file to append to the end of the output
var INI_FOOTER_FILE = "footer";

// Language used for the transformation
var INI_LANGUAGE = "language";

// Default extension for the output
var INI_EXTENSION = "extension";

// Is the language case-sensitive?
var INI_CASE_SENSITIVE = "case_sensitive";

// People can access the command line in their custom replacements
var INI_COMMANDLINE_ARGS = "commandline";

var g_objOptions = null;
var g_objKeywords = null;
var g_objUserDefined = null;
var g_objFutureReserved = null;
var g_objUserReplacements = null;
var g_reCodeMatcher = null;
var g_sInputFile = "";
var g_sOutputFile = "";

g_objOptions = createDefaultOptions();
if (processCommandLine())
{
	processIniFile(g_objOptions[INI_INI_FILE], g_objOptions);
	loadSupportFiles();
	g_reCodeMatcher = createRegExp();

	print("\nRunning with the following options:\n");
	dumpObj(g_objOptions, "    ");
	print("\n");
	
	formatCode();
}//if

function dumpObj(obj, indent)
{
	for (var p in obj)
	{
		if (typeof obj[p] == "object")
		{
			print(indent + p + ": Object:")
			dumpObj(obj[p], indent + "  ");
		} //if
		else
		{
			print(indent + p + ": " + obj[p]);
		} //else
	} //for
} //dumpObj()
	
function processCommandLine()
{
	var filesFound = 0;
	var args = getCommandLineArguments();
	
	// Start at 1 since we don't want the EXE name
	for (var i = 1; i < args.length; i++)
	{
		var sArg = args[i];
		
		// check for a switch
		// slash ('/') character is a valid filename character on UNIX, so we can't use it as a switch
		if ((sArg.charAt(0) == "-") || ((sArg.charAt(0) == "/") && Path.DirectorySeparatorChar != "/"))
		{
			// Option is of the form <name>:<value>
			// Add it to the global options.
			var iIndex = sArg.indexOf(":");
			g_objOptions[sArg.substring(1, iIndex)] = sArg.substring(iIndex + 1);
		} //if
		// Probably a file name
		else
		{
			switch(filesFound)
			{
			case 0:
				// input file name
				g_sInputFile = sArg;
				if (System.IO.File.Exists(g_sInputFile))
				{
				  var ext = "js";  //default
				  ext = System.IO.Path.GetExtension(g_sInputFile);
				  ext = ext.substring(1, ext.Length);  //Trim the "." off the extension
				  g_objOptions[INI_INI_FILE] = ext + "_codetohtml.ini";
				} //if
				else
				{
				  print("\n   Error: invalid file name or missing file: ", g_sInputFile);
				  return false;
				} //else
				break;
			case 1:
				// output file name
				g_sOutputFile = sArg;
				break;
			default:
				// nothing
				break;
			} //switch
			filesFound++;
		} //else
	} //for
	
	if (filesFound == 0)
	{
		showUsage();
		return false;
	} //if
	
	// Save the command line, if people want it
	g_objOptions[INI_COMMANDLINE_ARGS] = args;
	
	return true;
} //processCommandLine()

function formatCode()
{
	var sHeader = guardedReadFile(g_objOptions[INI_HEADER_FILE]);
	var sFooter = guardedReadFile(g_objOptions[INI_FOOTER_FILE]);
	var sSource = guardedReadFile(g_sInputFile);
	
	// Replace all the code with the HTML
	sSource = sSource.replace(g_reCodeMatcher, ReplaceSource);
	
	// Replace all the tabs with &nbsp;
	sSource = sSource.replace(/ {4}|\t/g, sAmp + "nbsp;" + sAmp + "nbsp;" 
		+ sAmp + "nbsp;" + sAmp + "nbsp;")
		
	// Replace all the newlines with <br>
	sSource = sSource.replace(/\n/g, sLt + "br" + sGt + "\n");

	// Patch up <, >, and &
	sSource = sSource.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
	sSource = sSource.replace(/\u1000/g, "<").replace(/\u1001/g, ">").replace(/\u1002/g, "&");

	// Add the header and footer text
	var sOutputText = sHeader + sSource + sFooter;
	
	// Do user-specified replacements
	for (var sCustom in g_objUserReplacements)
	{	
		debugPrint("Replacing %" + sCustom + "% with eval(" + g_objUserReplacements[sCustom] + ")");
		var reCustom = new RegExp("%" + sCustom + "%", "gm");
		try
		{
			sOutputText = sOutputText.replace(reCustom, eval(g_objUserReplacements[sCustom]));
		} //try
		catch(e)
		{
			logError("Couldn't replace %" + sCustom + "% due to a run-time error");
		} //catch
	} //for
	
	if (g_sOutputFile == "")
		g_sOutputFile = g_sInputFile + "." + g_objOptions[INI_EXTENSION];
		
	writeFile(g_sOutputFile, sOutputText);
} //formatCode()

// Default options for the INI file (if they aren't over-ridden in the codetohtml.ini file)
function createDefaultOptions()
{
	var o = new Object();
	
	o[INI_INI_FILE] = "codetohtml.ini";
	o[INI_KEYWORD_FILE] = "keywords.ini";
	o[INI_USERDEFINED_FILE] = "userdefined.ini";
	o[INI_FUTURERESERVED_FILE] = "futurereserved.ini";
	o[INI_USERREPLACEMENTS_FILE] = "replace.ini";
	o[INI_HEADER_FILE] = "header.txt";
	o[INI_FOOTER_FILE] = "footer.txt";
	o[INI_PUNCTUATION] = "~!%^&*()-=+{}[\\]|<>,/?;:";
	o[INI_LINE_COMMENT] = "\\/\\/";
	o[INI_MULTILINE_COMMENT_START] = "\\/\\*";
	o[INI_MULTILINE_COMMENT_END] = "\\*\\/";
	o[INI_LANGUAGE] = "JScript";
	o[INI_EXTENSION] = "html";
	o[INI_CASE_SENSITIVE] = "true";
	
	return o;	
} //createDefaultOptions()

// Processes the INI file, overriding the defaults with any
// options found in the INI file
function processIniFile(sFileName, obj)
{
	try
	{
		var sData = readFile(sFileName);
		var iniEntries = sData.split(Environment.NewLine);
				
		for (var i = 0; i < iniEntries.length; i++)
		{
			var sEntry = iniEntries[i];
			// debugPrint("Got entry '" + sEntry + "'");
			
			// Look for comment lines and empty lines
			if ((sEntry.charAt(0) == ";") || (sEntry == ""))
			{
				// print("It was blank or a comment");
				continue;
			} //if
			
			// add the <name>=<value> pair to the object
			var iIndex = sEntry.indexOf("=");
			obj[sEntry.substring(0, iIndex)] = sEntry.substring(iIndex + 1);
		} //for
	} //try
	catch(e)
	{
		logError("Couldn't process the file " + sFileName);
		// logError(getErrorInfo(e));
	} //catch
} //processIniFile()

function loadListOfWords(sFileName)
{
	try
	{
		var arrTmp = readFile(sFileName).split(Environment.NewLine);
		var objTmp = new Object();
		var bCaseSensitive = g_objOptions[INI_CASE_SENSITIVE];

		// Populate the object with properties named after
		// the elements of the array
		for (var i in arrTmp)
		{
			var s = arrTmp[i];
			
			// case-insensitive?
			if (bCaseSensitive != "true")
				s = s.toLowerCase();
				
			if (s != "")
				objTmp[s] = true;
		} //for
			
		return objTmp;
	} //try
	// Probably the file was not found
	catch(e)
	{
		logError("Couldn't load words from the file " + sFileName);
		// logError(getErrorInfo(e));
		return new Object();
	} //catch
} //loadListOfWords


// Load and process the support files
function loadSupportFiles()
{
	g_objKeywords = loadListOfWords(g_objOptions[INI_KEYWORD_FILE]);
	g_objUserDefined = loadListOfWords(g_objOptions[INI_USERDEFINED_FILE]);
	g_objFutureReserved = loadListOfWords(g_objOptions[INI_FUTURERESERVED_FILE]);
	
	g_objUserReplacements = new Object();
	processIniFile(g_objOptions[INI_USERREPLACEMENTS_FILE], g_objUserReplacements);
} //loadSupportFiles()


function createRegExp()
{
	// This one works, but colours the entire conditional block,
	// which isn't very useful if it is large. Put it back in if you want.          
	// var sConditionalPattern = "\\/\\*@(?:.|\\n)*?@\\*\\/";

	var sConditionalPattern = "(?:\\/\\*@\\w+|@\\w+|@\\*\\/)";

	// Single-line and multi-line comments
	var sCommentPattern = "(?:" + g_objOptions[INI_LINE_COMMENT] + ".*?$|" +
		g_objOptions[INI_MULTILINE_COMMENT_START] + "(?:.|\\n)*?" +
		g_objOptions[INI_MULTILINE_COMMENT_END] + ")";

	// Regular expressions
	var sRegExpPattern = "\\/.*?(?!\\\\).\\/\\w+";

	// Various types of numbers
	var sExponentialPattern = "(?:(?:\\d+)?\\.)?\\d+e[+-]?\\d+";
	var sFloatPattern = "(?:\\d+)?\\.\\d+";
	var sOctalPattern = "0\\d+";
	var sHexPattern = "0x(?:\\d|[a-f])+";
	var sIntPattern = "\\d+";

	// Identifiers
	var sIdentifierPattern = "\\b[a-z_$]\\w+?\\b";

	// Strings, including those with embedded newlines and embedded quotes
	var sStringPattern = "(?:\"\"|\'\'|\"(?:\\n|.)*?(?!\\\\).\"|'.*?(?!\\\\).')";

	// pre-processor stuff (for our C++ friends)
	var sPreProcessor = "^\\s*#.*$";

	// Punctuation
	var sPunctuationPattern = "[" + g_objOptions[INI_PUNCTUATION] + "]";

	// All sorts of code!
	var sCodePattern = 
		"(" + sConditionalPattern + ")|" +
		"(" + sCommentPattern + ")|" +
		"(" + sRegExpPattern + ")|" +
		"(" + sExponentialPattern + ")|" +
		"(" + sFloatPattern + ")|" +
		"(" + sOctalPattern + ")|" +
		"(" + sHexPattern + ")|" +
		"(" + sIntPattern + ")|" +
		"(" + sIdentifierPattern + ")|" +
		"(" + sStringPattern + ")|" +
		"(" + sPreProcessor + ")|" +
		"(" + sPunctuationPattern + ")";

	return new RegExp(sCodePattern, "gim");
} //createRegExp()

function ReplaceSource(match, conditional, comment, re, exp, dec, oct, hex, num, id, str, pre, punct)
{
	// debugPrint("Found text '" + match + "'");
	
	if (conditional != undefined)
	{
		// debugPrint("...it's a conditional comment!");
		return (sSpanStart + " class=\"conditional\"" + sGt + conditional + sSpanEnd);
	}
	
	if (comment != undefined)
	{
		// debugPrint("...it's a comment!");
		return (sSpanStart + " class=\"comment\"" + sGt + comment + sSpanEnd);
	}
	
	if (re != undefined)
	{
		// debugPrint("...it's a regular expression!");
		return (sSpanStart + " class=\"regexp\"" + sGt + re + sSpanEnd);
	}
	
	if (id != undefined)
	{
		// do we need to lower-case the string for matching?
		var tmpId = id;
		if (g_objOptions[INI_CASE_SENSITIVE] != "true")
			tmpId = tmpId.toLowerCase();
			
		// debugPrint("...it's an identifier!");
		
		if (tmpId in g_objKeywords)
		{
			// debugPrint("...it's a keyword!");
			return (sSpanStart + " class=\"keyword\"" + sGt + id + sSpanEnd);
		}
		else if (tmpId in g_objUserDefined)
		{
			// debugPrint("...it's a user-defined keyword!");
			return (sSpanStart + " class=\"userdefined\"" + sGt + id + sSpanEnd);
		}
		else if (tmpId in g_objFutureReserved)
		{
			// debugPrint("...it's a future reserved keyword!");
			return (sSpanStart + " class=\"futurereserved\"" + sGt + id + sSpanEnd);
		}
		else
		{
			return (sSpanStart + " class=\"identifier\"" + sGt + id + sSpanEnd);
		}
	} //if
	
	if (exp != undefined)
	{
		// debugPrint("...it's an exponential number!");
		return (sSpanStart + " class=\"exponential\"" + sGt + exp + sSpanEnd);
	}
	
	if (dec != undefined)
	{
		// debugPrint("...it's a decimal number!");
		return (sSpanStart + " class=\"decimal\"" + sGt + dec + sSpanEnd);
	}
	
	if (oct != undefined)
	{
		// debugPrint("...it's an octal number (bad)!");
		return (sSpanStart + " class=\"octal\"" + sGt + oct + sSpanEnd);
	}
	
	if (hex != undefined)
	{
		// debugPrint("...it's a hex number!");
		return (sSpanStart + " class=\"hexadecimal\"" + sGt + hex + sSpanEnd);
	}
	
	if (num != undefined)
	{
		// debugPrint("...it's a normal number!");
		return (sSpanStart + " class=\"number\"" + sGt + num + sSpanEnd);
	}
	
	if (str != undefined)
	{
		// debugPrint("...it's a string!");
		return (sSpanStart + " class=\"string\"" + sGt + str + sSpanEnd);
	}
	
	if (pre != undefined)
	{
		// debugPrint("...it's a pre-processor directive!");
		return (sSpanStart + " class=\"preprocessor\"" + sGt + pre + sSpanEnd);
	}
	
	if (punct != undefined)
	{
		// debugPrint("...it's a punctuator!");
		return (sSpanStart + " class=\"punctuation\"" + sGt + punct + sSpanEnd);
	}
	
	logError("Found something but don't know what is is! " + match);
	return sSpanStart + " class=\"error\"" + sGt + match + sSpanEnd;
} //ReplaceSource()

function guardedReadFile(sFileName)
{
	try
	{
		return readFile(sFileName);
	} //try
	catch(e)
	{
		logError("Couldn't open file " + sFileName);
		return "";
	} //catch
} //guardedReadFile()

function showUsage()
{
  logError("\nUsage: codetohtml <infile> [<outfile>]\n\n\
  Default for outfile is <infile>.html\n\
  Requires the files 'codetohtml.ini', 'keywords.ini', 'userdefined.ini',\n\
  and 'futurereserved.ini' to be present in the directory.\n\n\
  The outfile HTML resides in the same directory as the infile.\n\
  To be able to see the color-formatted code, make sure the .CSS stylesheet file is\n\
  in the same directory as the output HTML file.\n");
} //showUsage()

function debugPrint(s)
{
	print(s);
} //debugPrint()

function readFile(sFileName : String) : String
{
	var stream : StreamReader = File.OpenText(sFileName);
	var sData : String = stream.ReadToEnd();
	stream.Close();
	return sData;
} //readFile()

function writeFile(sFileName : String, sContents : String)
{
	var stream : StreamWriter = File.CreateText(sFileName);
	stream.Write(sContents);
	stream.Close();
} //writeFile()

function getCommandLineArguments() : Array
{
	// BUG - compiler doesn't treat COM+ arrays as JScript arrays yet.
	var arrArgs : Array = new Array();
	var tmp : String[] = Environment.GetCommandLineArgs();
	
	for (var i : int = 0; i < tmp.Length; i++)
		arrArgs.push(tmp[i]);

	return arrArgs;
} //getCommandLineArguments()

function logError(s : String)
{
	Console.Error.WriteLine(s);
} //logError

function getErrorInfo(e) : String
{
	if (e instanceof Error)
		return e.number + ": " + e.description;
	else if (e instanceof Exception)
		return e.ToString() + "\n\n" + e.Message + "\n" + e.StackTrace;
	else
		return String(e);
} //getErrorInfo()


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


var iTestID = 215391;

/*----------------------------------------
Test: dotnet10
Product:  JScript 
Area: 	check System.Xml  
Purpose: make sure basic functionality works.
---------------------------------------------------*/


@if(@aspx)
	</script>
	<%@ import namespace="System.IO" %>
	<%@ import namespace="System.Data" %>
	<%@ import namespace="System.Xml" %>
	<script language=jscript runat=server>
@else
	import System
	import System.IO
	import System.Data
	import System.Xml
@end


// First create members.xml that will contain the JS code that we want to compile
var JSCode = 	"<Members>                                       " +
		"<Member alias=\"tester\" team = \"Scripting\" \/>   " +
		"<\/Members>                                     ";
var fileStream:FileStream = new FileStream ("members.xml", FileMode.Create, FileAccess.Write);
var streamWriter = new StreamWriter (fileStream);
streamWriter.Write (JSCode);
streamWriter.Close();
fileStream.Close();

var sAct : String="";
var sExp = "";
var sActErr = "";
var sExpErr = "";
var sErrThr = "";

function verify(sRes, sExp, sMsg, sBug)
{
	if (sRes != sExp)
		apLogFailInfo(sMsg, sExp, sRes, sBug);
}

function dotnet10()
{
	apInitTest ("dotnet10");
	apInitScenario ("1.1 XmlTextWriter");
	var writer: XmlTextWriter ;

	try {
		writer = new XmlTextWriter("book-dotnet10.xml",null);
		writer.WriteStartDocument();
		writer.Formatting = Formatting.Indented;
		writer.Indentation = 3;
		writer.WriteStartElement("bookstore");
			writer.WriteStartElement("book")
				writer.WriteAttributeString("style","hardcover");
			
				writer.WriteAttributeString("genre","history")
				writer.WriteElementString("title","Vietnam");
				
				writer.WriteStartElement("author");
				  writer.WriteElementString("first-name","Michael");
				  writer.WriteElementString("last-name","Avery");
				writer.WriteEndElement();
				
				writer.WriteElementString("price","6.99");
			
			writer.WriteEndElement();		
		writer.WriteEndElement();
		writer.Flush();
	}
	catch (error) 
	{
      		apLogFailInfo ("1.1 XmlTextWriter", "Bad thing happened", error, "");
   	}
	finally {
		writer.Close();
	}
   
      	apInitScenario ("1.2 XmlTextReader");	
   	var reader: XmlTextReader;
	try {
		reader = new XmlTextReader("members.xml");
		var sTesterList: String = "";
		while (reader.Read()) {		
			switch (reader.NodeType) {
				case XmlNodeType.Element: 
					if (reader.HasAttributes){
						sTesterList += reader.GetAttribute(0);
					}
				case XmlNodeType.Text:
			}
			
			
		}
		verify(sTesterList,"tester","1.2 XmlTextReader","");
	}
	catch (error)
	{
		apLogFailInfo ("1.2 XmlTextReader", "", error, "");	
	}
	finally {
		//reader.Close();
	}
	
	apInitScenario("2.1 Using Xml DOM to loading data from file");
	var xmldoc : XmlDocument;
	
	try {
		xmldoc = new XmlDocument();
		xmldoc.Load("book-dotnet10.xml");
		ShowTree(xmldoc.DocumentElement);
		verify(sAct,"bookstore book style hardcover genre history title Vietnam author first-name Michael last-name Avery price 6.99 ","2.1 Using Xml DOM to loading data from file","");
	}
	catch (error) {
		apLogFailInfo ("2.1 Using Xml DOM to loading data from file");
	}
	
	apInitScenario("2.2 Appending data to an XmlDocument");
	try {
	var eleBook : XmlElement = xmldoc.CreateElement("Book");
	var attStyle: XmlAttribute = xmldoc.CreateAttribute("style");
	eleBook.SetAttributeNode(attStyle);
	eleBook.SetAttribute("style","hardback");
	
	var root : XmlElement = xmldoc.Item("bookstore")
	root.AppendChild(eleBook);
	xmldoc.Save("book-output.xml");
	}
	catch (error)
	{
		apLogFailInfo ("2.2 Appending data to an XmlDocument");
	}

      apEndTest();
}

function ShowTree(node: XmlNode)
{
	var attrnode : XmlNode;
	var map: XmlNamedNodeMap;
	
	if (!node.HasChildNodes )
		sAct += node.Value + " ";
	else {
		sAct += node.Name + " ";
		if (node.NodeType == XmlNodeType.Element ){
			map = node.Attributes;
			for (attrnode in map) {
				sAct += attrnode.Name + " " + attrnode.Value + " ";
			}
		}
		if (node.HasChildNodes) {
			node = node.FirstChild; 
			while (node) {
				ShowTree(node)
				node = node.NextSibling;;
			}
		}
	}
}




dotnet10();


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

// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==

// stress test for appdomain create / destroy

using System;
using System.Threading;
using System.IO;
using System.Xml;
using System.Xml.Xsl;
using System.Xml.XPath;

class MainApp : MarshalByRefObject {

    int _worker;

    public MainApp() {
    }

    public MainApp(int worker) {
        _worker = worker;
    }

    public void Action() {   
        string xml = "<?xml version=\"1.0\"?>\n<a><b c=\"d\">e</b></a>";
        string xsl = "<?xml version=\"1.0\"?>\n" +
            "<xsl:stylesheet version=\"1.0\" " +
                    "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" " +
                    "xmlns:msxsl=\"urn:schemas-microsoft-com:xslt\" " +
                    "xmlns:js=\"urn:js\" " +
                ">" +
                "<msxsl:script language=\"jsCrIpt\" implements-prefix=\"js\">" +
                    "function SimpleTest() { return \"JScript test\"; }" +        
                "</msxsl:script>" +                
                "<xsl:template match=\"a\"><foo><xsl:apply-templates/></foo></xsl:template>" +
                "<xsl:template match=\"b\"><xsl:element name=\"bar\">" +
        	        "<xsl:attribute name=\"simpleTest\"><xsl:value-of select=\"js:SimpleTest()\"/></xsl:attribute>" +
                "</xsl:element></xsl:template>" +
                "<xsl:template match=\"/\"><xsl:apply-templates/></xsl:template>" +
            "</xsl:stylesheet>";            
    
        XPathDocument myXPathDocument = new XPathDocument(new XmlTextReader(new StringReader(xml)));

        XslTransform myXslTransform = new XslTransform();
        myXslTransform.Load(new XmlTextReader(new StringReader(xsl)));
        
        StringWriter myStringWriter = new StringWriter();
        XmlTextWriter myXmlWriter = new XmlTextWriter(myStringWriter);
        myXmlWriter.Formatting = Formatting.Indented;
        
        myXslTransform.Transform(myXPathDocument, null, myXmlWriter);
        myXmlWriter.Close();        
        
        Console.WriteLine(myStringWriter.ToString());
    }
    
    public void Worker() {
        for (int i = 0; i < 2000; i++) {
            try {
                String s = _worker.ToString() + ":" + i.ToString();
                Console.WriteLine(s);
                AppDomain app = AppDomain.CreateDomain(s);
                MainApp remoteapp = (MainApp)app.CreateInstanceAndUnwrap ("appdomainstress", "MainApp");
                remoteapp.Action();
                AppDomain.Unload(app);
            } catch (Exception e) {
                Console.WriteLine(e);
                Environment.Exit(100);
            }
        }
    }
    
    public static int Main() {
    
        new MainApp().Action();
        
        Thread[] workers = new Thread[3];
        int i;

        for (i = 0; i < workers.Length; i++) {
            Thread t = new Thread(new ThreadStart(new MainApp(i).Worker));
            workers[i] = t;
            t.Start();
            Thread.Sleep(5000);
        }
        
        for (i = 0; i < workers.Length; i++) {
            workers[i].Join();                
        }

        return 0;
    }
}

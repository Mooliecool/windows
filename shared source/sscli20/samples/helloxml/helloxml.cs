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

using System;
using System.IO;
using System.Xml;

class MainApp {
   public static void Main() {

      StringWriter w = new StringWriter();

      XmlTextWriter x = new XmlTextWriter(w);

      x.Formatting = Formatting.Indented;

      x.WriteStartDocument();
      x.WriteComment("a simple test");
      x.WriteStartElement("message");
      x.WriteStartAttribute("project", "");
      x.WriteString("Rotor");
      x.WriteEndAttribute();
      x.WriteString("Hello world!");
      x.WriteEndElement();
      x.WriteEndDocument();

      x.Flush();

      Console.WriteLine(w.ToString());
   }
}

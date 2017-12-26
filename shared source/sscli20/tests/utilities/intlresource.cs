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
using System.Text;
using System.Resources;
using System.Security.Permissions;

[FileIOPermission(SecurityAction.Assert, Unrestricted=true)]
  public class IntlResource
{
 const String EOL = "\r\n";
 public static void Main(string[] args)
   {
   String fileName = "genstrings.resources";
   if(File.Exists(Environment.CurrentDirectory+"\\" + fileName))
     File.Delete(Environment.CurrentDirectory+"\\" + fileName);
						
   ResourceWriter writer = new ResourceWriter(fileName);
   writer.AddResource("English_Text", ReadTextFile("english.txt"));
   writer.AddResource("Japanese_Text", ReadTextFile("japanese.txt"));
   writer.AddResource("German_Text", ReadTextFile("german.txt"));
   writer.AddResource("Arabic_Text", ReadTextFile("arabic.txt"));
   writer.AddResource("Korean_Text", ReadTextFile("korean.txt"));
   writer.AddResource("ChineseTra_Text", ReadTextFile("chinesetra.txt"));
   writer.AddResource("ChineseSim_Text", ReadTextFile("chinesesim.txt"));
   writer.AddResource("Spanish_Text", ReadTextFile("spanish.txt"));
   writer.AddResource("Italian_Text", ReadTextFile("italian.txt"));
   writer.AddResource("French_Text", ReadTextFile("french.txt"));
   writer.AddResource("Turkish_Text", ReadTextFile("turkish.txt"));
   writer.AddResource("NorwegianBok_Text", ReadTextFile("norwegianbok.txt"));
   writer.AddResource("NorwegianNyn_Text", ReadTextFile("norwegiannyn.txt"));
   writer.Generate();
   writer.Close();					
   }
    
 public static String ReadTextFile(String fileName)
   {
   String line;
   StringBuilder builder = new StringBuilder();
   FileStream stream = new FileStream(fileName, FileMode.Open, FileAccess.Read);
   StreamReader reader = new StreamReader(stream);
   while(true)
     {
     line = reader.ReadLine();
     if(line == null)
       break;
     builder.Append(line + EOL);
     }
   reader.Close();
   stream.Close();
   
   return builder.ToString();
   }
}


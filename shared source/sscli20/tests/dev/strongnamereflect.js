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
/////////////////////////////////////////////////////////////////////////
import System;
import System.Reflection;

[assembly: System.Reflection.AssemblyKeyFileAttribute("strongnamereflect.snk")]

var token;
var name;

name = Assembly.GetExecutingAssembly().GetName();
Console.WriteLine(name);

token = Assembly.GetExecutingAssembly().GetName().GetPublicKeyToken();
if (token == null) {
    Console.WriteLine("no public key token");
    Environment.Exit(111);
}

if (Convert.ToBase64String(token) != "I++ZjVKQXpo=") {
    Console.WriteLine("incorrect public key token");
    Environment.Exit(222);
}

Console.WriteLine("Passed");

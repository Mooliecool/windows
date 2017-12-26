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
using System.Runtime.Remoting;
using System.Configuration;

class RemotingConfig {

    public static int Main(string[] args)
    {
        string filename = Path.Combine(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, ".."), "remoting.config");
        RemotingConfiguration.Configure(filename);
        
        string applicationName = RemotingConfiguration.ApplicationName;
        Console.WriteLine(applicationName);
        if (applicationName != "Hello World!")
            return 1;

        // make sure that the config subsystem can otherwise bootstrap
        string proxy = ConfigurationManager.GetSection("system.net/defaultProxy").ToString();        
        Console.WriteLine(proxy);

        return 0;
    }
}

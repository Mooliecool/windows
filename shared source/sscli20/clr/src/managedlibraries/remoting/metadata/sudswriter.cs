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
//============================================================
//
// File:    SudsWriter.cs
// Purpose: Defines SUDSParser that parses a given SUDS document
//          and generates types defined in it.
//
// Date:    April 01, 2000
// Revised: November 15, 2000 (Wsdl)                       
//
//============================================================
namespace System.Runtime.Remoting.MetadataServices
{
    using System;
    using System.Runtime.Serialization;
    using System.IO;
    using System.Reflection;

	// Represents exceptions thrown by the SUDSGenerator
    [Serializable()] 
	public class SUDSGeneratorException : Exception
	{
		internal SUDSGeneratorException(String msg)
				: base(msg)
		{
		}

        protected SUDSGeneratorException(SerializationInfo info, StreamingContext context): base(info, context) {}
	}

	// This class generates SUDS documents
	internal class SUDSGenerator
	{
        WsdlGenerator wsdlGenerator = null;
        SdlType sdlType;
		// Constructor
		internal SUDSGenerator(Type[] types, SdlType sdlType, TextWriter output)
		{
			Util.Log("SUDSGenerator.SUDSGenerator 2");
                wsdlGenerator = new WsdlGenerator(types, sdlType, output);
                this.sdlType = sdlType;
		}

		internal SUDSGenerator(ServiceType[] serviceTypes, SdlType sdlType, TextWriter output)
		{
			Util.Log("SUDSGenerator.SUDSGenerator 5 ");
                wsdlGenerator = new WsdlGenerator(serviceTypes, sdlType, output);
                this.sdlType = sdlType;
		}


		// Generates SUDS
		internal void Generate()
		{
			Util.Log("SUDSGenerator.Generate");			
                wsdlGenerator.Generate();
        }
    }
}


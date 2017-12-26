//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
// This is the main DLL file.

#include "stdafx.h"

#include "SharedImplementation.h"

using namespace System;
using namespace System::Text;

namespace Microsoft 
{
    namespace Samples
    {
        namespace SharedImplementation
        {
            public ref class HelloWorld : public MarshalByRefObject
            {
            public:
                String^ Echo(String^ input)
                {
                    StringBuilder^ result = gcnew StringBuilder();
                    Console::WriteLine("Client says: {0}", input);
                    result->Append(input);
                    result->Append(" ");
                    result->Append(input);
                    return result->ToString();
                }
            };
        }
    }
}
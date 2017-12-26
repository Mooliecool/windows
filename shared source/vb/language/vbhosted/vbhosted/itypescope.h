#pragma once

namespace Microsoft 
{ 
    namespace Compiler 
    { 
        namespace VisualBasic
        {
#if USEPRIVATE
            private interface class ITypeScope
#else
            public interface class ITypeScope
#endif
            {
                bool NamespaceExists(System::String ^ns);
                
                array<System::Type^,1> ^FindTypes(System::String ^typeName,
                                                 System::String ^nsPrefix);
            };
        }
    }
}

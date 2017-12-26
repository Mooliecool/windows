#pragma once

namespace Microsoft 
{ 
    namespace Compiler 
    { 
        namespace VisualBasic
        {
#if USEPRIVATE
            private interface class IScriptScope
#else
            public interface class IScriptScope
#endif
            {
                System::Type ^FindVariable(System::String ^name);
            };
        }
    }
}

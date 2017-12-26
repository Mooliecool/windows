#pragma once

#include "TypeEmitter.h"

using namespace System::Reflection;
using namespace System::Reflection::Emit;

namespace Microsoft 
{ 
    namespace Compiler 
    { 
        namespace VisualBasic
        {
            private ref class AnonymousDelegateEmitter : TypeEmitter 
            {
            private:
                System::Collections::Generic::Dictionary<System::String^, System::Type^>^ m_types;

            public:
                AnonymousDelegateEmitter(SymbolMap^ symbolMap, ModuleBuilder^ moduleBuilder);

                virtual bool TryGetType(BCSYM_NamedRoot* pSymbol, System::Type^% type) override;

                System::Type^ EmitType(BCSYM_NamedRoot* pSymbol);

            };
        }
    }
}

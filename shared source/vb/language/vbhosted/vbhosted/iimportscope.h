#pragma once

#include "Import.h"

namespace Microsoft 
{ 
    namespace Compiler 
    { 
        namespace VisualBasic
        {
#if USEPRIVATE
            private interface class IImportScope
#else
            public interface class IImportScope
#endif
            {
                System::Collections::Generic::IList<Import ^> ^GetImports();
            };
        }
    }
}

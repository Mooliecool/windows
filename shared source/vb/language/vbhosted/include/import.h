#pragma once

namespace Microsoft 
{ 
    namespace Compiler 
    { 
        namespace VisualBasic
        {
#if USEPRIVATE
            private ref class Import sealed
#else
            public ref class Import sealed
#endif
            {
            private:
                System::String ^m_alias;
                System::String ^m_importedEntity;

            public:
                Import(System::String ^importedEntity);
                
                Import(System::String ^alias, 
                       System::String ^importedEntity);
                
                property System::String ^Alias { 
                    System::String ^get(); 
                }

                property System::String ^ImportedEntity { 
                    System::String ^get(); 
                }
            };
        }
    }
}

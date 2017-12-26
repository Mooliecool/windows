#pragma once

namespace Microsoft 
{ 
    namespace Compiler 
    { 
        namespace VisualBasic
        {
#if USEPRIVATE
            private enum class OptionStrictSetting
#else
            public enum class OptionStrictSetting
#endif
            {
                Off,
                On,
                Custom
            };
        }
    }
}

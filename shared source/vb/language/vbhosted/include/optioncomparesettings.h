#pragma once

namespace Microsoft
{
    namespace Compiler
    {
        namespace VisualBasic
        {
#if USEPRIVATE
            private enum class OptionCompareSetting
#else
            public enum class OptionCompareSetting
#endif
            {
                Binary,
                Text
            };
        }
    }
}

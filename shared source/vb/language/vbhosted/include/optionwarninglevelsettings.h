#pragma once

namespace Microsoft
{
    namespace Compiler
    {
        namespace VisualBasic
        {
#if USEPRIVATE
            private enum class OptionWarningLevelSetting
#else
            public enum class OptionWarningLevelSetting
#endif
            {
                None,
                Regular,
                AsError
            };
        }
    }
}

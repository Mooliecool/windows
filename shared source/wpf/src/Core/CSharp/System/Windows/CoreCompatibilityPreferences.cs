using System;
using System.Runtime.Versioning;
using MS.Internal.PresentationCore;

namespace System.Windows
{
    public static class CoreCompatibilityPreferences
    {
        #region CLR compat flags

        internal static bool TargetsAtLeast_Desktop_V4_5
        { get { return BinaryCompatibility.TargetsAtLeast_Desktop_V4_5; } }

        #endregion CLR compat flags

        #region IsAltKeyRequiredInAccessKeyDefaultScope

        // We decided NOT to opt-in this feature by default.
        private static bool _isAltKeyRequiredInAccessKeyDefaultScope = false;

        public static bool IsAltKeyRequiredInAccessKeyDefaultScope
        {
            get { return _isAltKeyRequiredInAccessKeyDefaultScope; }
            set
            {
                lock (_lockObject)
                {
                    if (_isSealed)
                    {
                        throw new InvalidOperationException(SR.Get(SRID.CompatibilityPreferencesSealed, "IsAltKeyRequiredInAccessKeyDefaultScope", "CoreCompatibilityPreferences"));
                    }

                    _isAltKeyRequiredInAccessKeyDefaultScope = value;
                }
            }
        }

        internal static bool GetIsAltKeyRequiredInAccessKeyDefaultScope()
        {
            Seal();

            return IsAltKeyRequiredInAccessKeyDefaultScope;
        }

        #endregion IsAltKeyRequiredInAccessKeyDefaultScope

        private static void Seal()
        {
            if (!_isSealed)
            {
                lock (_lockObject)
                {
                    _isSealed = true;
                }
            }
        }

        private static bool _isSealed;
        private static object _lockObject = new object();
    }
}

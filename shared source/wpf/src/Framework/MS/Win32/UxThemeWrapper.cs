using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Security;
using System.Windows;
using System.Windows.Media;
using System.Text;
using MS.Win32;
using MS.Internal;

namespace MS.Win32
{
    /// <summary>
    ///     Wrapper class for loading UxTheme system theme data
    /// </summary>
    internal static class UxThemeWrapper
    {
        static UxThemeWrapper()
        {
            // When in high contrast we want to force the WPF theme to be Classic 
            // because that is the only WPF theme that has full fidelity support for the 
            // high contrast color scheme. Prior to Win8 the OS automatically switched 
            // to Classic theme when in high contrast, but Win8 onwards apps that claim 
            // Win8 OS support via the app.manifest will receive the AeroLite as the theme 
            // when in high contrast. The reason being the OS team wants to give high 
            // contrast a face lift starting Win8. However, WPF isnt setup to support this 
            // currently. Thus we fallback to Classic in this situation.
            
            _isActive = !SystemParameters.HighContrast && SafeNativeMethods.IsUxThemeActive();
        }

        internal static bool IsActive
        {
            get
            {
                return _isActive;
            }
        }

        internal static string ThemeName
        {
            get
            {
                if (IsActive)
                {
                    if (_themeName == null)
                    {
                        EnsureThemeName();
                    }

                    return _themeName;
                }
                else
                {
                    return "classic";
                }
            }
        }

        internal static string ThemeColor
        {
            get
            {
                Debug.Assert(IsActive, "Queried ThemeColor while UxTheme is not active.");

                if (_themeColor == null)
                {
                    EnsureThemeName();
                }

                return _themeColor;
            }
        }

        ///<SecurityNote>
        /// Critical - as this code performs an elevation to get current theme name
        /// TreatAsSafe - the "critical data" is transformed into "safe data"
        ///                      all the info stored is the currrent theme name and current color - e.g. "Luna", "NormalColor"
        ///                      Does not contain a path - considered safe. 
        ///</SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        private static void EnsureThemeName()
        {
            StringBuilder themeName = new StringBuilder(Win32.NativeMethods.MAX_PATH);
            StringBuilder themeColor = new StringBuilder(Win32.NativeMethods.MAX_PATH);

            if (UnsafeNativeMethods.GetCurrentThemeName(themeName, themeName.Capacity,
                                                        themeColor, themeColor.Capacity,
                                                        null, 0) == 0)
            {
                // Success
                _themeName = themeName.ToString();
                _themeName = Path.GetFileNameWithoutExtension(_themeName);

                if(String.Compare(_themeName, "aero", StringComparison.OrdinalIgnoreCase) == 0 && Utilities.IsOSWindows8OrNewer)
                {
                    _themeName = "Aero2";
                }
                
                _themeColor = themeColor.ToString();
            }
            else
            {
                // Failed to retrieve the name
                _themeName = _themeColor = String.Empty;
            }
        }

        internal static void OnThemeChanged()
        {
            // Please refer to elaborate comment about high 
            // contrast in the static constructor
            _isActive = !SystemParameters.HighContrast && SafeNativeMethods.IsUxThemeActive();

            _themeName = null;
            _themeColor = null;
        }

        private static bool _isActive;
        private static string _themeName;
        private static string _themeColor;
    }
}



using System;
using System.Collections.Specialized;   // NameValueCollection
using System.Configuration;             // ConfigurationManager
using System.Runtime.Versioning;

namespace System.Windows
{
    public static class FrameworkCompatibilityPreferences
    {
        #region Constructor

        static FrameworkCompatibilityPreferences()
        {
            // user can use config file to set preferences
            NameValueCollection appSettings = null;
            try
            {
                appSettings = ConfigurationManager.AppSettings;
            }
            catch (ConfigurationErrorsException)
            {
            }

            if (appSettings != null)
            {
                SetHandleTwoWayBindingToPropertyWithNonPublicSetterFromAppSettings(appSettings);
                SetUseSetWindowPosForTopmostWindowsFromAppSettings(appSettings);
            }
        }

        #endregion Constructor

        #region AreInactiveSelectionHighlightBrushKeysSupported

        private static bool _areInactiveSelectionHighlightBrushKeysSupported = BinaryCompatibility.TargetsAtLeast_Desktop_V4_5 ? true : false;

        public static bool AreInactiveSelectionHighlightBrushKeysSupported
        {
            get { return _areInactiveSelectionHighlightBrushKeysSupported; }
            set
            {
                lock (_lockObject)
                {
                    if (_isSealed)
                    {
                        throw new InvalidOperationException(SR.Get(SRID.CompatibilityPreferencesSealed, "AreInactiveSelectionHighlightBrushKeysSupported", "FrameworkCompatibilityPreferences"));
                    }

                    _areInactiveSelectionHighlightBrushKeysSupported = value;
                }
            }
        }

        internal static bool GetAreInactiveSelectionHighlightBrushKeysSupported()
        {
            Seal();

            return AreInactiveSelectionHighlightBrushKeysSupported;
        }

        #endregion AreInactiveSelectionHighlightBrushKeysSupported

        #region KeepTextBoxDisplaySynchronizedWithTextProperty

        private static bool _keepTextBoxDisplaySynchronizedWithTextProperty = BinaryCompatibility.TargetsAtLeast_Desktop_V4_5 ? true : false;

        /// <summary>
        /// In WPF 4.0, a TextBox can reach a state where its Text property
        /// has some value X, but a different value Y is displayed.   Setting
        /// FrameworkCompatibilityPreferences.KeepTextBoxDisplaySynchronizedWithTextProperty=true
        /// ensures that the displayed value always agrees with the value of the Text property.
        ///
        /// </summary>
        /// <notes>
        /// The inconsistent state can be reached as follows:
        /// 1. The TextBox is data-bound with property-changed update trigger
        ///         <TextBox Text="{Binding UpdateSourceTrigger=PropertyChanged, Path=ABC}"/>
        /// 2. The TextBox displays a value X
        /// 3. The user types a character to produce a new value Y
        /// 4. The new value Y is sent to the data item's property ABC then read
        ///     back again, possibly applying conversions in each direction.   The
        ///     data item may "normalize" the value as well - upon receiving value V it
        ///     may store a different value V'.  Denote by Z the result of this round-trip.
        /// 5. The Text property is set to Z.
        /// 6. Usually the text box will now display Z.   But if Z and X are the same,
        ///     it will display Y (which is different from X).
        ///
        /// For example, suppose the data item normalizes by trimming spaces:
        ///     public string ABC { set { _abc = value.Trim(); } }
        /// And suppose the user types "hi ".  Upon typing the space, the binding
        /// sends "hi " to the data item, which stores "hi".  The result of the round-trip
        /// is "hi", which is identical to the string before typing the space.  In
        /// this case, the TextBox reaches a state where its Text property has value
        /// "hi" although it displays "hi ".
        ///
        /// As a second example, suppose the data item normalizes an integer by
        /// capping its value to a maximum, say 100:
        ///     public int Score { set { _score = Math.Min(value, 100); } }
        /// And suppose the user types "1004".  Upon typing the 4, the binding converts
        /// string "1004" to int 1004 and sends 1004 to the data item, which stores 100.
        /// The round-trip continues, converting int 100 to string "100", which is
        /// identical to the text before typing the 4.   The TextBox reaches a state
        /// where its Text property has value "100", but it displays "1004".
        /// </notes>
        public static bool KeepTextBoxDisplaySynchronizedWithTextProperty
        {
            get { return _keepTextBoxDisplaySynchronizedWithTextProperty; }
            set
            {
                lock (_lockObject)
                {
                    if (_isSealed)
                    {
                        throw new InvalidOperationException(SR.Get(SRID.CompatibilityPreferencesSealed, "AextBoxDisplaysText", "FrameworkCompatibilityPreferences"));
                    }

                    _keepTextBoxDisplaySynchronizedWithTextProperty = value;
                }
            }
        }

        internal static bool GetKeepTextBoxDisplaySynchronizedWithTextProperty()
        {
            Seal();

            return KeepTextBoxDisplaySynchronizedWithTextProperty;
        }

        #endregion KeepTextBoxDisplaySynchronizedWithTextProperty

        #region HandleTwoWayBindingToPropertyWithNonPublicSetter

        // the different ways we can handle a TwoWay binding to a propery with non-public setter.
        // These must appear in order, from tightest to loosest.
        internal enum HandleBindingOptions
        {
            Throw,      // 4.0 behavior - throw an exception
            Disallow,   // diagnostic behavior - don't throw, but don't allow updates
            Allow,      // 4.5RTM behavior - allow the binding to update
        }

        // this flag defaults to:
        //      partial-trust           -> Throw    (plug security hole)
        //      wrong target framework  -> Disallow (app probably not built by VS.  E.g. ServerManager)
        //      target = 4.5            -> Allow    (compat with 4.5RTM)
        //      any other target        -> Throw    (compat with 4.0)
        private static HandleBindingOptions _handleTwoWayBindingToPropertyWithNonPublicSetter =
                !MS.Internal.SecurityHelper.IsFullTrustCaller() ? HandleBindingOptions.Throw :
                BinaryCompatibility.AppWasBuiltForFramework != TargetFrameworkId.NetFramework ? HandleBindingOptions.Disallow :
                BinaryCompatibility.AppWasBuiltForVersion == 40500 ? HandleBindingOptions.Allow :
                /* else */  HandleBindingOptions.Throw;

        internal static HandleBindingOptions HandleTwoWayBindingToPropertyWithNonPublicSetter
        {
            get { return _handleTwoWayBindingToPropertyWithNonPublicSetter; }
            set
            {
                lock (_lockObject)
                {
                    if (_isSealed)
                    {
                        throw new InvalidOperationException(SR.Get(SRID.CompatibilityPreferencesSealed, "HandleTwoWayBindingToPropertyWithNonPublicSetter", "FrameworkCompatibilityPreferences"));
                    }

                    // apps are allowed to tighten the restriction, but not to loosen it
                    if (value.CompareTo(_handleTwoWayBindingToPropertyWithNonPublicSetter) > 0)
                    {
                        throw new ArgumentException();
                    }

                    _handleTwoWayBindingToPropertyWithNonPublicSetter = value;
                }
            }
        }

        internal static HandleBindingOptions GetHandleTwoWayBindingToPropertyWithNonPublicSetter()
        {
            Seal();

            return HandleTwoWayBindingToPropertyWithNonPublicSetter;
        }

        static void SetHandleTwoWayBindingToPropertyWithNonPublicSetterFromAppSettings(NameValueCollection appSettings)
        {
            // user can use config file to tighten the restriction
            string s = appSettings["HandleTwoWayBindingToPropertyWithNonPublicSetter"];
            HandleBindingOptions value;
            if (Enum.TryParse(s, true, out value) && value.CompareTo(HandleTwoWayBindingToPropertyWithNonPublicSetter) <= 0)
            {
                HandleTwoWayBindingToPropertyWithNonPublicSetter = value;
            }
        }

        #endregion AllowTwoWayBindingToPropertyWithNonPublicSetter

        // DevDiv #681144:  There is a bug in the Windows desktop window manager which can cause
        // incorrect z-order for windows when several conditions are all met:
        // (a) windows are parented/owned across different threads or processes
        // (b) a parent/owner window is also owner of a topmost window (which needn't be visible)
        // (c) the child window on a different thread/process tries to show an owned topmost window
        //     (like a popup or tooltip) using ShowWindow().
        // To avoid this window manager bug, this option causes SetWindowPos() to be used instead of
        // ShowWindow() for topmost windows, avoiding condition (c).  Ideally the window manager bug
        // will be fixed, but the risk of making a change there is considered too great at this time.
        #region UseSetWindowPosForTopmostWindows

        private static bool _useSetWindowPosForTopmostWindows = false; // use old behavior by default

        internal static bool UseSetWindowPosForTopmostWindows
        {
            get { return _useSetWindowPosForTopmostWindows; }
            set
            {
                lock (_lockObject)
                {
                    if (_isSealed)
                    {
                        throw new InvalidOperationException(SR.Get(SRID.CompatibilityPreferencesSealed, "UseSetWindowPosForTopmostWindows", "FrameworkCompatibilityPreferences"));
                    }

                    _useSetWindowPosForTopmostWindows = value;
                }
            }
        }

        internal static bool GetUseSetWindowPosForTopmostWindows()
        {
            Seal();

            return UseSetWindowPosForTopmostWindows;
        }

        static void SetUseSetWindowPosForTopmostWindowsFromAppSettings(NameValueCollection appSettings)
        {
            // user can use config file to enable this behavior change
            string s = appSettings["UseSetWindowPosForTopmostWindows"];
            bool useSetWindowPos; 
            if (Boolean.TryParse(s, out useSetWindowPos))
            {
                UseSetWindowPosForTopmostWindows = useSetWindowPos;
            }
        }

        #endregion UseSetWindowPosForTopmostWindows

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

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Media;
using System.Globalization;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;
using System.Security;
using MS.Internal;
using MS.Internal.PresentationCore;                        // SecurityHelper
using System.Security.Permissions;
using MS.Win32.Penimc;
using MS.Win32;


namespace System.Windows.Input
{
    /////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     Class that represents a physical digitizer connected to the system.
    ///     Tablets are the source of events for the Stylus devices.
    /// </summary>
    public sealed class TabletDevice : InputDevice
    {
        /////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////
        /// <SecurityNote>
        ///     Critical:  creates security critical data (IpimcTablet)
        /// </SecurityNote>
        [SecurityCritical]
        internal TabletDevice(TabletDeviceInfo tabletInfo, PenThread penThread)
        {
            _tabletInfo = tabletInfo;
            _penThread = penThread;
            int count = tabletInfo.StylusDevicesInfo.Length;

            StylusDevice[] styluses = new StylusDevice[count];
            for ( int i = 0; i < count; i++ )
            {
                StylusDeviceInfo cursorInfo = tabletInfo.StylusDevicesInfo[i];
                styluses[i] = new StylusDevice(
                    this,
                    cursorInfo.CursorName, 
                    cursorInfo.CursorId, 
                    cursorInfo.CursorInverted,
                    cursorInfo.ButtonCollection);
            }

            _stylusDeviceCollection = new StylusDeviceCollection(styluses);

            if (_tabletInfo.DeviceType == TabletDeviceType.Touch)
            {
                // 

                _multiTouchSystemGestureLogic = new MultiTouchSystemGestureLogic();
            }
        }

        /////////////////////////////////////////////////////////////////////
        /// <SecurityNote>
        ///     Critical: calls SecurityCritical method _stylusDeviceCollection.Dispose.
        /// </SecurityNote>
        [SecurityCritical]
        internal void Dispose()
        {
            _tabletInfo.PimcTablet = null;
            if (_stylusDeviceCollection != null)
            {
                _stylusDeviceCollection.Dispose();
                _stylusDeviceCollection = null;
            }
            _penThread = null;
        }

        /////////////////////////////////////////////////////////////////////
        /// <SecurityNote>
        ///     Critical: - Calls unmanaged code that is SecurityCritical with SUC attribute.
        ///                 - accesses security critical data _pimcTablet.Value
        ///           - called by constructor
        /// </SecurityNote>
        [SecurityCritical]
        internal StylusDevice UpdateStylusDevices(int stylusId)
        {

            // REENTRANCY NOTE: Use PenThread to talk to wisptis.exe to make sure we are not reentrant!
            //                  PenImc will cache all the stylus device info so we don't have 
            //                  any Out of Proc calls to wisptis.exe to get this info.

            StylusDeviceInfo[] stylusDevicesInfo = _penThread.WorkerRefreshCursorInfo(_tabletInfo.PimcTablet.Value);

            int cCursors = stylusDevicesInfo.Length;

            if (cCursors > StylusDevices.Count)
            {
                for (int iCursor = 0; iCursor < cCursors; iCursor++)
                {
                    StylusDeviceInfo stylusDeviceInfo = stylusDevicesInfo[iCursor];
                    
                    // See if we found it.  If so go and create the new StylusDevice and add it.
                    if ( stylusDeviceInfo.CursorId == stylusId )
                    {
                        StylusDevice newStylusDevice = new StylusDevice(
                                                                this,
                                                                stylusDeviceInfo.CursorName,
                                                                stylusDeviceInfo.CursorId,
                                                                stylusDeviceInfo.CursorInverted,
                                                                stylusDeviceInfo.ButtonCollection);
                        StylusDevices.AddStylusDevice(iCursor, newStylusDevice);
                        
                        return newStylusDevice;
                    }
                }
                
            }

            return null;  // Nothing to add
        }


        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the element that input from this device is sent to.
        /// </summary>
        public override IInputElement Target
        {
            get
            {
                VerifyAccess();
                StylusDevice stylusDevice = Stylus.CurrentStylusDevice;
                if (stylusDevice == null)
                    return null;
                return stylusDevice.Target;
            }
        }

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the PresentationSource that is reporting input for this device.
        /// </summary>
        /// <remarks>
        ///     Callers must have UIPermission(UIPermissionWindow.AllWindows) to call this API.
        /// </remarks>
        /// <SecurityNote>
        /// Critical - accesses critical data (InputSource on StylusDevice)
        /// PublicOK - there is a demand.
        ///               this is safe as: 
        ///                     there is a demand for the UI permissions in the code
        /// </SecurityNote>
        public override PresentationSource ActiveSource
        {
            [SecurityCritical]
            get
            {
                SecurityHelper.DemandUIWindowPermission();
                VerifyAccess();
                StylusDevice stylusDevice = Stylus.CurrentStylusDevice;
                if (stylusDevice == null)
                    return null;
                return stylusDevice.ActiveSource;  // This also does a security demand.
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns an id of the tablet object unique within the process.
        /// </summary>
        public int Id
        {
            get
            {
                VerifyAccess();
                return _tabletInfo.Id;
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the friendly name of the tablet.
        /// </summary>
        public string Name
        {
            get
            {
                VerifyAccess();
                return _tabletInfo.Name;
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the hardware Product ID of the tablet (was PlugAndPlayId).
        /// </summary>
        public string ProductId
        {
            get
            {
                VerifyAccess();
                return _tabletInfo.PlugAndPlayId;
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the maximum coordinate dimensions that the device can collect.
        ///     This value is in Tablet Coordinates
        /// </summary>
        internal Matrix TabletToScreen
        {
            get
            {
                return new Matrix( _tabletInfo.SizeInfo.ScreenSize.Width / _tabletInfo.SizeInfo.TabletSize.Width, 0,
                                   0, _tabletInfo.SizeInfo.ScreenSize.Height / _tabletInfo.SizeInfo.TabletSize.Height,
                                   0, 0);
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the size of the digitizer for this TabletDevice.
        ///     This value is in Tablet Coordinates.
        /// </summary>
        internal Size TabletSize
        {
            get
            {
                return _tabletInfo.SizeInfo.TabletSize;
            }
        }
 
        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the size for the display that this TabletDevice is
        ///     mapped to.
        ///     This value is in Screen Coordinates.
        /// </summary>
        internal Size ScreenSize
        {
            get
            {
                return _tabletInfo.SizeInfo.ScreenSize;
            }
        }
 
 
        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the capabilities of the tablet hardware.
        /// </summary>
        public TabletHardwareCapabilities TabletHardwareCapabilities
        {
            get
            {
                VerifyAccess();
                return _tabletInfo.HardwareCapabilities;
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the list of StylusPointProperties supported by this TabletDevice.
        /// </summary>
        public ReadOnlyCollection<StylusPointProperty> SupportedStylusPointProperties
        {
            get
            {
                VerifyAccess();
                return _tabletInfo.StylusPointProperties;
            }
        }

        // Helper to return a StylusPointDescription using the SupportedStylusProperties info.
        internal StylusPointDescription StylusPointDescription
        {
            get
            {
                if (_stylusPointDescription == null)
                {
                    ReadOnlyCollection<StylusPointProperty> properties = SupportedStylusPointProperties;
                    // InitializeSupportStylusPointProperties must be called first!
                    Debug.Assert(properties != null);
                
                    List<StylusPointPropertyInfo> propertyInfos = new List<StylusPointPropertyInfo>();
                    for (int i=0; i < properties.Count; i++)
                    {
                        propertyInfos.Add(new StylusPointPropertyInfo(properties[i]));
                    }
                    _stylusPointDescription = new StylusPointDescription(propertyInfos, _tabletInfo.PressureIndex);
                }
                return _stylusPointDescription;
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the type of tablet device hardware (Stylus, Touch)
        /// </summary>
        public TabletDeviceType Type
        {
            get
            {
                VerifyAccess();
                return _tabletInfo.DeviceType;
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the friendly string representation of the Tablet object
        /// </summary>
        public override string ToString()
        {
            return String.Format(CultureInfo.CurrentCulture, "{0}({1})", base.ToString(), Name);
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Returns the collection of StylusDevices defined on this tablet.
        ///     An Empty collection is returned if the device has not seen any Stylus Pointers.
        /// </summary>
        public StylusDeviceCollection StylusDevices
        {
            get
            {
                VerifyAccess();

                Debug.Assert (_stylusDeviceCollection != null);
                return _stylusDeviceCollection;
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// </summary>
        /// <SecurityNote>
        ///     Critical: - calls into unmanaged code that is SecurityCritical with SUC attribute.
        ///                 - accesses security critical data _pimcTablet.Value
        ///                 - takes in data that is potential security risk (hwnd)
        /// </SecurityNote>
        [SecurityCritical]
        internal PenContext CreateContext(IntPtr hwnd, PenContexts contexts)
        {
            PenContext penContext;
            bool supportInRange = (this.TabletHardwareCapabilities & TabletHardwareCapabilities.HardProximity) != 0;
            bool isIntegrated = (this.TabletHardwareCapabilities & TabletHardwareCapabilities.Integrated) != 0;

            // Use a PenThread to create a tablet context so we don't cause reentrancy.
            PenContextInfo result = _penThread.WorkerCreateContext(hwnd, _tabletInfo.PimcTablet.Value);

            penContext = new PenContext(result.PimcContext != null ? result.PimcContext.Value : null, 
                                        hwnd, contexts, 
                                        supportInRange, isIntegrated, result.ContextId,
                                        result.CommHandle != null ? result.CommHandle.Value : IntPtr.Zero, 
                                        Id);
            return penContext;
        }


        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// </summary>
        /// <SecurityNote>
        ///     Critical: - returns data that is potential a security risk (PenThread)
        /// </SecurityNote>
        internal PenThread PenThread
        {
            [SecurityCritical]
            get
            {
                return _penThread;
            }
        }

        /////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// </summary>
        /// <SecurityNote>
        ///     Critical: - calls into unmanaged code that is SecurityCritical with SUC attribute.
        ///               - accesses security critical data _pimcTablet.Value
        /// </SecurityNote>
        [SecurityCritical]
        internal void UpdateScreenMeasurements()
        {
            Debug.Assert(CheckAccess());

            // Update and reset these sizes to be recalculated the next time they are needed.
            _cancelSize = Size.Empty;
            _doubleTapSize = Size.Empty;

            // Update the size info we use to map tablet coordinates to screen coordinates.
            _tabletInfo.SizeInfo = _penThread.WorkerGetUpdatedSizes(_tabletInfo.PimcTablet.Value);
        }

        // NOTE: UpdateSizeDeltas MUST be called before the returned Size is valid.
        //  Since we currently only call this while processing Down stylus events
        //  after StylusDevice.UpdateState has been called it will always be initialized appropriately.
        internal Size DoubleTapSize
        {
            get 
            { 
                return _doubleTapSize;   // used for double tap detection - updating click count.
            }
        }

        // NOTE: UpdateSizeDeltas MUST be called before the returned Size is valid.
        //  Since we currently only call this while processing Move stylus events
        //  after StylusDevice.UpdateState has been called it will always be initialized appropriately.
        internal Size CancelSize
        {
            get 
            { 
                return _cancelSize; // Used for drag detection when double tapping.
            }
        }

        // Forces the UpdateSizeDeltas to re-calc the sizes the next time it is called.
        // NOTE: We don't invalidate the sizes and just leave them till the next time we
        //  UpdateSizeDeltas gets called.
        internal void InvalidateSizeDeltas()
        {
            _forceUpdateSizeDeltas = true;
        }

        internal bool AreSizeDeltasValid()
        {
            return !(_doubleTapSize.IsEmpty || _cancelSize.IsEmpty);
        }
        
        /// <SecurityNote>
        ///      Critical as this uses SecurityCritical _stylusLogic variable.
        ///
        ///      At the top called from StylusLogic::PreProcessInput event which is SecurityCritical
        /// </SecurityNote>
        [SecurityCritical]
        internal void UpdateSizeDeltas(StylusPointDescription description, StylusLogic stylusLogic)
        {
            if (_doubleTapSize.IsEmpty || _cancelSize.IsEmpty || _forceUpdateSizeDeltas)
            {
                // Query default settings for mouse drag and double tap (with minimum of 1x1 size).
                Size mouseDragDefault = new Size(Math.Max(1, SafeSystemMetrics.DragDeltaX / 2),
                                               Math.Max(1, SafeSystemMetrics.DragDeltaY / 2));
                Size mouseDoubleTapDefault = new Size(Math.Max(1, SafeSystemMetrics.DoubleClickDeltaX / 2),
                                               Math.Max(1, SafeSystemMetrics.DoubleClickDeltaY / 2));
               
                StylusPointPropertyInfo xProperty = description.GetPropertyInfo(StylusPointProperties.X);
                StylusPointPropertyInfo yProperty = description.GetPropertyInfo(StylusPointProperties.Y);

                uint dwXValue = GetPropertyValue(xProperty);
                uint dwYValue = GetPropertyValue(yProperty);

                if (dwXValue != 0 && dwYValue != 0)
                {
                    _cancelSize = new Size((int)Math.Round((ScreenSize.Width * stylusLogic.CancelDelta) / dwXValue),
                                           (int)Math.Round((ScreenSize.Height * stylusLogic.CancelDelta) / dwYValue));
                    
                    // Make sure we return whole numbers (pixels are whole numbers) and take the maximum
                    // value between mouse and stylus settings to be safe.
                    _cancelSize.Width = Math.Max(mouseDragDefault.Width, _cancelSize.Width);
                    _cancelSize.Height = Math.Max(mouseDragDefault.Height, _cancelSize.Height);

                    _doubleTapSize = new Size((int)Math.Round((ScreenSize.Width * stylusLogic.DoubleTapDelta) / dwXValue),
                                              (int)Math.Round((ScreenSize.Height * stylusLogic.DoubleTapDelta) / dwYValue));
                    
                    // Make sure we return whole numbers (pixels are whole numbers) and take the maximum
                    // value between mouse and stylus settings to be safe.
                    _doubleTapSize.Width = Math.Max(mouseDoubleTapDefault.Width, _doubleTapSize.Width);
                    _doubleTapSize.Height = Math.Max(mouseDoubleTapDefault.Height, _doubleTapSize.Height);
                }
                else
                {
                    // If no info to do the calculation then use the mouse settings for the default.
                    _doubleTapSize = mouseDoubleTapDefault;
                    _cancelSize = mouseDragDefault;
                }
                
                _forceUpdateSizeDeltas = false;
            }
        }

        private static uint GetPropertyValue(StylusPointPropertyInfo propertyInfo)
        {
            uint   dw = 0;

            switch (propertyInfo.Unit)
            {
                case StylusPointPropertyUnit.Inches:
                    if (propertyInfo.Resolution != 0)
                        dw = (uint) (((propertyInfo.Maximum - propertyInfo.Minimum) * 254) / propertyInfo.Resolution);
                    break;

                case StylusPointPropertyUnit.Centimeters:
                    if (propertyInfo.Resolution != 0)
                        dw = (uint) (((propertyInfo.Maximum - propertyInfo.Minimum) * 100) / propertyInfo.Resolution);
                    break;

                default:
                    dw = 1000;
                    break;
            }
            return dw;
        }

        /// <summary>
        ///     Sends input reports to the system gesture logic object.
        /// </summary>
        /// <param name="stylusInputReport">A new input report.</param>
        /// <returns>A SystemGesture that was detected, null otherwise.</returns>
        /// <SecurityNote>
        ///     Critical: The generated system gesture is posted back to the input system.
        ///         SystemGesture events need to be protected.
        /// </SecurityNote>
        [SecurityCritical]
        internal SystemGesture? GenerateStaticGesture(RawStylusInputReport stylusInputReport)
        {
            if (_multiTouchSystemGestureLogic != null)
            {
                return _multiTouchSystemGestureLogic.GenerateStaticGesture(stylusInputReport);
            }

            return null;
        }

        /// <summary>
        ///     Accesses the GetLastTabletPoint value from the input report and converts
        ///     it from tablet device units into device-independent units.
        /// </summary>
        /// <SecurityNote>
        ///     Critical: Accesses InputManager and the StylusLogic.
        ///     TreatAsSafe: Doesn't expose that information.
        /// </SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        internal Point GetLastTabletPoint(RawStylusInputReport stylusInputReport)
        {
            var lastPoint = stylusInputReport.GetLastTabletPoint();

            if (!_tabletToView.HasValue)
            {
                // 
                _tabletToView = InputManager.Current.StylusLogic.GetTabletToViewTransform(this);
            }

            return _tabletToView.Value.Transform(lastPoint);
        }

        /////////////////////////////////////////////////////////////////////////

        TabletDeviceInfo            _tabletInfo; // Hold the info about this tablet device.

        /// <SecurityNote>
        ///     Critical to prevent accidental spread to transparent code
        /// </SecurityNote>
        [SecurityCritical]
        PenThread                   _penThread; // Hold ref on worker thread we use to talk to wisptis.

        StylusDeviceCollection      _stylusDeviceCollection;
        StylusPointDescription      _stylusPointDescription;

        // Calculated size in screen coordinates for Drag and DoubleTap detection.
        private Size _cancelSize = Size.Empty;
        private Size _doubleTapSize = Size.Empty;
        private bool _forceUpdateSizeDeltas;

        // Determines if recent input should be converted into a static gesture
        private MultiTouchSystemGestureLogic _multiTouchSystemGestureLogic;

        private Matrix? _tabletToView;
    }
}

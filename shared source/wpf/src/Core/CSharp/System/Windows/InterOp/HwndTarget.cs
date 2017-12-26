//------------------------------------------------------------------------------
//  Microsoft Avalon
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  File:       HwndTarget.cs
//
//------------------------------------------------------------------------------
using System;
using System.Diagnostics;
using System.Windows.Threading;
using System.Threading;
using System.Windows;
using System.Collections.Generic;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Automation.Provider;
using System.Windows.Automation.Peers;
using System.Windows.Media.Composition;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using System.Security;
using MS.Internal;
using MS.Internal.Automation;
using MS.Internal.Interop;
using MS.Win32;
using MS.Utility;
using MS.Internal.PresentationCore;                        // SecurityHelper

using SR=MS.Internal.PresentationCore.SR;
using SRID=MS.Internal.PresentationCore.SRID;

using HRESULT = MS.Internal.HRESULT;

#pragma warning disable 1634, 1691  // suppressing PreSharp warnings

namespace System.Windows.Interop
{
    // This is the internal, more expressive, enum used by the InvalidateRenderMode method.
    // See the RenderMode enum and the RenderMode property for the public version.
    internal enum RenderingMode
    {
        Default = MILRTInitializationFlags.MIL_RT_INITIALIZE_DEFAULT,
        Software = MILRTInitializationFlags.MIL_RT_SOFTWARE_ONLY,
        Hardware = MILRTInitializationFlags.MIL_RT_HARDWARE_ONLY,
        HardwareReference = MILRTInitializationFlags.MIL_RT_HARDWARE_ONLY | MILRTInitializationFlags.MIL_RT_USE_REF_RAST,
    }

    // This is the public, more limited, enum exposed for use with the RenderMode property.
    // See the RenderingMode enum and InvalidateRenderMode method for the internal version.
    /// <summary>
    ///     Render mode preference.
    /// </summary>
    public enum RenderMode
    {
        /// <summary>
        /// The rendering layer should use the GPU and CPU as appropriate.
        /// </summary>
        Default,

        /// <summary>
        /// The rendering layer should only use the CPU.
        /// </summary>
        SoftwareOnly
    }

    /// <summary>
    /// The HwndTarget class represents a binding to an HWND.
    /// </summary>
    /// <remarks>The HwndTarget is not thread-safe. Accessing the HwndTarget from a different
    /// thread than it was created will throw a <see cref="System.InvalidOperationException"/>.</remarks>
    public class HwndTarget : CompositionTarget
    {
        /// <SecurityNote>
        /// Critical - Gets to the unmanaged layer (ctor, HandleMessage, UpdateWindowSettings).
        /// </SecurityNote>
        [SecurityCritical]
        private static WindowMessage s_updateWindowSettings;

        /// <SecurityNote>
        /// Critical - Gets to the unmanaged layer (ctor, HandleMessage, UpdateWindowSettings).
        /// </SecurityNote>
        [SecurityCritical]
        private static WindowMessage s_needsRePresentOnWake;


        private MatrixTransform _worldTransform;
        private double _devicePixelsPerInchX;
        private double _devicePixelsPerInchY;

        /// <SecurityNote>
        /// Critical -  We don't want partial trust code changing the rendering preference.
        /// </SecurityNote>
        private SecurityCriticalDataForSet<RenderMode> _renderModePreference = new SecurityCriticalDataForSet<RenderMode>(RenderMode.Default);

        ///<SecurityNote>
        /// Critical - obtained under an elevation.
        ///</SecurityNote>
        [SecurityCritical]
        private NativeMethods.HWND _hWnd;

        private NativeMethods.RECT _hwndClientRectInScreenCoords;
        private NativeMethods.RECT _hwndWindowRectInScreenCoords;

        private Color _backgroundColor = Color.FromRgb(0, 0, 0);

        private DUCE.MultiChannelResource _compositionTarget =
            new DUCE.MultiChannelResource();

        private bool _isRenderTargetEnabled = true;
        // private Nullable<Color> _colorKey = null;
        // private double _opacity = 1.0;
        private bool _usesPerPixelOpacity = false;

        // It is important that this start at zero to allow an initial
        // UpdateWindowSettings(enable) command to enable the render target
        // without a preceeding UpdateWindowSettings(disable) command.
        private int _disableCookie = 0;

        // Used to deal with layered window problems. See comments where they are used.
        private bool _isMinimized = false;
        private bool _isSessionDisconnected = false;
        private bool _isSuspended = false;

        // True when user input is causing a resize. We use this to determine whether or
        // not we want to [....] during resize to provide a better looking resize.
        private bool _userInputResize = false;

        // This bool is set by a private window message sent to us from the render thread,
        // indicating that the present has failed with S_PRESENT_OCCLUDED (usually due to the
        // monitor being asleep or locked) and that we need to invalidate the entire window for
        // presenting when the monitor turns back on.
        private bool _needsRePresentOnWake = false;

        // See comment above for _needsRePresentOnWake. If the present has failed because of a
        // reason other than the monitor being asleep (usually because a D3D full screen exclusive
        // app is occluding the WPF app), we need to be able to recognize this situation and avoid
        // continually invalidating the window and causing presents that will fail and continue the
        // cycle (the so called "WM_PAINT storm"). We set this member to true the first time we
        // invalidate due to the private window message indicating failure if we are *not* asleep, once
        // the timeout period specified by _allowedPresentFailureDelay has passed
        // Any failure after that until another sleep state event occurs will not trigger an invalidate.
        private bool _hasRePresentedSinceWake = false;

        // The time of the last wake or unlock message we received. When we receive a lock/sleep message,
        // we set this value to DateTime.MinValue
        private DateTime _lastWakeOrUnlockEvent;

        // This is the amount of time we continue to propagate Invalidate() calls when we receive notifications
        // from the render thread that present has failed, as measured from the value of _lastWakeOrUnlockEvent.
        // This allows for a window of time during which we have received the, for eg, session unlock message,
        // but the D3D device is still returning S_PRESENT_OCCLUDED. Time is in seconds.
        private const double _allowedPresentFailureDelay = 10.0;

        private DispatcherTimer _restoreDT;

        /// <summary>
        /// Initializes static variables for this class.
        /// </summary>
        /// <SecurityNote>
        /// Critical        - Sets the SecurityCritical static variables holding the message ids; calls RegisterWindowMessage.
        /// TreatAsSafe     - The message ids are not exposed; no external parameters are taken in.
        /// </SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        static HwndTarget()
        {
            s_updateWindowSettings = UnsafeNativeMethods.RegisterWindowMessage("UpdateWindowSettings");
            s_needsRePresentOnWake = UnsafeNativeMethods.RegisterWindowMessage("NeedsRePresentOnWake");
        }

        /// <summary>
        /// Attaches a hwndTarget to the hWnd
        /// <remarks>
        ///     This API link demands for UIWindowPermission.AllWindows
        /// </remarks>
        /// </summary>
        /// <param name="hwnd">The HWND to which the HwndTarget will draw.</param>
        /// <remarks>
        ///     Callers must have UIPermission(UIPermissionWindow.AllWindows) to call this API.
        /// </remarks>
        /// <SecurityNote>
        /// Critical - accepts unmanaged pointer handle.Not safe to create since it
        ///            can be used to draw to a window
        /// PublicOk - demands UIPermission
        /// </SecurityNote>
        [SecurityCritical]
        [UIPermissionAttribute(SecurityAction.LinkDemand,Window=UIPermissionWindow.AllWindows)]
        public HwndTarget(IntPtr hwnd)
        {
            bool exceptionThrown = true;

            AttachToHwnd(hwnd);

            try
            {
                if (EventTrace.IsEnabled(EventTrace.Keyword.KeywordGeneral, EventTrace.Level.Info))
                {
                    EventTrace.EventProvider.TraceEvent(EventTrace.Event.WClientCreateVisual, EventTrace.Keyword.KeywordGeneral, EventTrace.Level.Info, Dispatcher.GetHashCode(), hwnd.ToInt64());
                }

                _hWnd = NativeMethods.HWND.Cast(hwnd);

                // Get the client rectangle...
                GetWindowRectsInScreenCoordinates();

                // Set device independent resolution to 96 DPI.
                // NativeMethods.HDC hdc = NativeMethods.GetDC(_hWnd);

                _devicePixelsPerInchX = 96.0f;
                _devicePixelsPerInchY = 96.0f;

                _lastWakeOrUnlockEvent = DateTime.MinValue;

                //
                // Determine whether hWnd corresponds to MIL or GDI window.
                //
                IntPtr hdcW = UnsafeNativeMethods.GetDC(
                    new HandleRef(this, _hWnd));

                if (hdcW == IntPtr.Zero)
                {
                    //
                    // If we were unable to obtain HDC for the given
                    // window, assume the default of 96 DPI.
                    //

                    _devicePixelsPerInchX = 96.0f;
                    _devicePixelsPerInchY = 96.0f;
                }
                else
                {
                    //
                    // Obtain and cache DPI values for window's HDC.
                    //

                    _devicePixelsPerInchX = (double)UnsafeNativeMethods.GetDeviceCaps(
                        new HandleRef(this, hdcW),
                        NativeMethods.LOGPIXELSX);

                    _devicePixelsPerInchY = (double)UnsafeNativeMethods.GetDeviceCaps(
                        new HandleRef(this, hdcW),
                        NativeMethods.LOGPIXELSY);

                    //
                    // Release DC object.
                    //

                    UnsafeNativeMethods.ReleaseDC(
                        new HandleRef(this, _hWnd),
                        new HandleRef(this, hdcW));
                }

                _worldTransform = new MatrixTransform(new Matrix(
                  _devicePixelsPerInchX * (1.0f / 96.0f), 0,
                  0, _devicePixelsPerInchY * (1.0f / 96.0f),
                  0, 0));

                //
                // Register CompositionTarget with MediaContext.
                //
                MediaContext.RegisterICompositionTarget(Dispatcher, this);

                // Initialize dispatcher timer to work-around a restore issue.
                _restoreDT = new DispatcherTimer();
                _restoreDT.Tick += new EventHandler(InvalidateSelf);
                _restoreDT.Interval = TimeSpan.FromMilliseconds(100);

                exceptionThrown = false;
            }
            finally
            {
                //
                // If exception has occurred after we attached this target to
                // the window, we need to detach from this window. Otherwise, window
                // will be left in a state when no other HwndTarget can be created
                // for it.
                //
                if(exceptionThrown)
                {
                    #pragma warning suppress 6031 // Return value ignored on purpose.
                    VisualTarget_DetachFromHwnd(hwnd);
                }
            }
        }

        /// <summary>
        /// AttachToHwnd
        /// </summary>
        ///<SecurityNote>
        /// Critical as it calls a function that performs an elevation (IsWindow).
        ///</SecurityNote>
        [SecurityCritical]
        private void AttachToHwnd(IntPtr hwnd)
        {
            int processId = 0;
            int threadId = UnsafeNativeMethods.GetWindowThreadProcessId(
                new HandleRef(this, hwnd),
                out processId
                );

            if (!UnsafeNativeMethods.IsWindow(new HandleRef(this, hwnd)))
            {
                throw new ArgumentException(
                    SR.Get(SRID.HwndTarget_InvalidWindowHandle),
                    "hwnd"
                    );
            }
            else if (processId != SafeNativeMethods.GetCurrentProcessId())
            {
                throw new ArgumentException(
                    SR.Get(SRID.HwndTarget_InvalidWindowProcess),
                    "hwnd"
                    );
            }
            else if (threadId != SafeNativeMethods.GetCurrentThreadId())
            {
                throw new ArgumentException(
                    SR.Get(SRID.HwndTarget_InvalidWindowThread),
                    "hwnd"
                    );
            }

            int hr = VisualTarget_AttachToHwnd(hwnd);

            if (HRESULT.Failed(hr))
            {
                if (hr == unchecked((int)0x80070005)) // E_ACCESSDENIED
                {
                    throw new InvalidOperationException(
                        SR.Get(SRID.HwndTarget_WindowAlreadyHasContent)
                        );
                }
                else
                {
                    HRESULT.Check(hr);
                }
            }

            EnsureNotificationWindow();
            _notificationWindowHelper.AttachHwndTarget(this);
            UnsafeNativeMethods.WTSRegisterSessionNotification(hwnd, NativeMethods.NOTIFY_FOR_THIS_SESSION);
        }

        /// <SecurityNote>
        ///     Critical: This code causes unmanaged code elevation
        /// </SecurityNote>
        [SecurityCritical,SuppressUnmanagedCodeSecurity]
        [DllImport(DllImport.MilCore, EntryPoint = "MilVisualTarget_AttachToHwnd")]
        internal static extern int VisualTarget_AttachToHwnd(
            IntPtr hwnd
            );


        /// <SecurityNote>
        ///     Critical: This code causes unmanaged code elevation
        /// </SecurityNote>
        [SecurityCritical, SuppressUnmanagedCodeSecurity]
        [DllImport(DllImport.MilCore, EntryPoint = "MilVisualTarget_DetachFromHwnd")]
        internal static extern int VisualTarget_DetachFromHwnd(
            IntPtr hwnd
            );

        internal void InvalidateRenderMode()
        {
            RenderingMode mode =
                RenderMode == RenderMode.SoftwareOnly ? RenderingMode.Software : RenderingMode.Default;

            //
            // If ForceSoftwareRendering is set then the transport is connected to a client (magnifier) that cannot
            // handle our transport protocol version. Therefore we force software rendering so that the rendered
            // content is available through NTUser redirection. If software is not allowed an exception is thrown.
            //
            if (MediaSystem.ForceSoftwareRendering)
            {
                if (mode == RenderingMode.Hardware ||
                    mode == RenderingMode.HardwareReference)
                {
                    throw new InvalidOperationException(SR.Get(SRID.HwndTarget_HardwareNotSupportDueToProtocolMismatch));
                }
                else
                {
                    Debug.Assert(mode == RenderingMode.Software || mode == RenderingMode.Default);
                    // If the mode is default we can chose what works. When we have a mismatched transport protocol version
                    // we need to fallback to software rendering.
                    mode = RenderingMode.Software;
                }
            }

            // Select the render target initialization flags based on the requested
            // rendering mode.

            DUCE.ChannelSet channelSet = MediaContext.From(Dispatcher).GetChannels();
            DUCE.Channel channel = channelSet.Channel;

            DUCE.CompositionTarget.SetRenderingMode(
                _compositionTarget.GetHandle(channel),
                (MILRTInitializationFlags)mode,
                channel);
        }

        /// <summary>
        /// Specifies the render mode preference for the window.
        /// </summary>
        /// <remarks>
        ///     This property specifies a preference, it does not necessarily change the actual
        ///     rendering mode.  Among other things, this can be trumped by the registry settings.
        ///     <para/>
        ///     Callers must have UIPermission(UIPermissionWindow.AllWindows) to set this property.
        /// </remarks>
        /// <SecurityNote>
        ///     Critical: This code influences the low-level rendering code by specifying whether the
        ///     rendering system should use the GPU or CPU.
        ///     PublicOK: We don't want to enable this in partial trust, so we have a link demand
        ///     on the setter.  It is not privileged data, so the getter is not protected.
        /// </SecurityNote>
        public RenderMode RenderMode
        {
            get
            {
                return _renderModePreference.Value;
            }

            // Note: We think it is safe to expose this in partial trust, but doing so would suggest
            // we should also expose HwndSource (the only way to get to the HwndTarget instance).
            // We don't want to bite off that much exposure at this point in the product, so we enforce
            // that this is not accessible from partial trust for now.
            [SecurityCritical]
            [UIPermissionAttribute(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
            set
            {
                if (value != RenderMode.Default && value != RenderMode.SoftwareOnly)
                {
                    throw new System.ComponentModel.InvalidEnumArgumentException("value", (int)value, typeof(RenderMode));
                }

                _renderModePreference.Value = value;

                InvalidateRenderMode();
            }
        }

        /// <summary>
        /// Dispose cleans up the state associated with HwndTarget.
        /// </summary>
        /// <SecurityNote>
        /// Critical - accesses the _hwnd that is critical, calls unmanaged code
        /// PublicOK - dispose is in effect stoping the contents of the target from
        /// rendering. Equivalent to removing all elements in window, considered safe.
        /// </SecurityNote>
        [SecurityCritical]
        public override void Dispose()
        {
           // Its outside the try finally block because we want the exception to be
           // thrown if we are on a different thread and we don't want to call Dispose
           // on base class in that case.
           VerifyAccess();

           try
           {
                // According to spec: Dispose should not raise exception if called multiple times.
                // This test is needed because the HwndTarget is Disposed from both the media contex and
                // the hwndsrc.
                if (!IsDisposed)
                {
                    RootVisual = null;

                    HRESULT.Check(VisualTarget_DetachFromHwnd(_hWnd));

                    //
                    // Unregister this CompositionTarget from the MediaSystem.
                    //
                    MediaContext.UnregisterICompositionTarget(Dispatcher, this);

                    if (_notificationWindowHelper != null &&
                        _notificationWindowHelper.DetachHwndTarget(this))
                    {
                        _notificationWindowHelper.Dispose();
                        _notificationWindowHelper = null;
                    }

                    // Unregister for Fast User Switching messages
                    UnsafeNativeMethods.WTSUnRegisterSessionNotification(_hWnd);
                }

            }
            finally
            {
                base.Dispose();
                GC.SuppressFinalize(this);
            }
        }

        /// <summary>
        /// This method is used to create all uce resources either on Startup or session connect
        /// </summary>
        /// <SecurityNote>
        /// Critical - uses unmanaged pointer handle _hWnd
        /// TreatAsSafe - doesn't return or expose _hWnd
        /// </SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        internal override void CreateUCEResources(DUCE.Channel channel, DUCE.Channel outOfBandChannel)
        {
            // create visual target resources
            // this forces the creation of the media context if we don't already have one.

            base.CreateUCEResources(channel, outOfBandChannel);

            Debug.Assert(!_compositionTarget.IsOnChannel(channel));
            Debug.Assert(!_compositionTarget.IsOnChannel(outOfBandChannel));

            //
            // For each HwndTarget we are building some structures in the UCE.
            // This includes spinning up a UCE render target. We need to commit the
            // batch for those changes right away, since we need to be able to process
            // the invalidate packages that we send down on WM_PAINTs. If we don't commit
            // right away a WM_PAINT can get fired before we get a chance to commit
            // the batch.
            //

            //
            // First we create the composition target, composition context, and the composition root node.
            // Note, that composition target will be created out of band because invalidate
            // command is also sent out of band and that can occur before current channel is committed.
            // We would like to avoid commiting channel here to prevent visual artifacts.
            //

            bool resourceCreated = _compositionTarget.CreateOrAddRefOnChannel(this, outOfBandChannel, DUCE.ResourceType.TYPE_HWNDRENDERTARGET);
            Debug.Assert(resourceCreated);
            _compositionTarget.DuplicateHandle(outOfBandChannel, channel);
            outOfBandChannel.CloseBatch();
            outOfBandChannel.Commit();

            DUCE.CompositionTarget.HwndInitialize(
                _compositionTarget.GetHandle(channel),
                _hWnd,
                _hwndClientRectInScreenCoords.right - _hwndClientRectInScreenCoords.left,
                _hwndClientRectInScreenCoords.bottom - _hwndClientRectInScreenCoords.top,
                MediaSystem.ForceSoftwareRendering,
                channel
                );

            DUCE.ResourceHandle hWorldTransform = ((DUCE.IResource)_worldTransform).AddRefOnChannel(channel);

            DUCE.CompositionNode.SetTransform(
                _contentRoot.GetHandle(channel),
                hWorldTransform,
                channel);

            DUCE.CompositionTarget.SetClearColor(
                _compositionTarget.GetHandle(channel),
                _backgroundColor,
                channel);

            //
            // Set initial state on the visual target.
            //

            Rect clientRect = new Rect(
                0,
                0,
                (float)(Math.Ceiling((double)(_hwndClientRectInScreenCoords.right - _hwndClientRectInScreenCoords.left))),
                (float)(Math.Ceiling((double)(_hwndClientRectInScreenCoords.bottom - _hwndClientRectInScreenCoords.top))));

            StateChangedCallback(
                new object[]
                {
                    HostStateFlags.WorldTransform |
                    HostStateFlags.ClipBounds,
                    _worldTransform.Matrix,
                    clientRect
                });

            DUCE.CompositionTarget.SetRoot(
                _compositionTarget.GetHandle(channel),
                _contentRoot.GetHandle(channel),
                channel);

            // reset the disable cookie when creating the slave resource. This happens when creating the
            // managed resource and on handling a connect.
            _disableCookie = 0;

            //
            // Finally, update window settings to reflect the state of this object.
            // Because CreateUCEResources is called for each channel, only call
            // UpdateWindowSettings on that channel this time.
            //
            DUCE.ChannelSet channelSet;
            channelSet.Channel = channel;
            channelSet.OutOfBandChannel = outOfBandChannel;
            UpdateWindowSettings(_isRenderTargetEnabled, channelSet);
        }

        /// <summary>
        /// This method is used to release all uce resources either on Shutdown or session disconnect
        /// </summary>
        /// <SecurityNote>
        /// Critical - uses unmanaged pointer handle _hWnd
        /// TreatAsSafe - doesn't return or expose _hWnd
        /// </SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        internal override void ReleaseUCEResources(DUCE.Channel channel, DUCE.Channel outOfBandChannel)
        {
            if (_compositionTarget.IsOnChannel(channel))
            {
                //
                // [....]: If we need to flush the batch we need to render first all visual targets that
                // are still registered with the MediaContext to avoid strutural tearing.


                // Set the composition target root node to null.
                DUCE.CompositionTarget.SetRoot(
                    _compositionTarget.GetHandle(channel),
                    DUCE.ResourceHandle.Null,
                    channel);

                _compositionTarget.ReleaseOnChannel(channel);
            }

            if (_compositionTarget.IsOnChannel(outOfBandChannel))
            {
                _compositionTarget.ReleaseOnChannel(outOfBandChannel);
            }

            DUCE.ResourceHandle hWorldTransform = ((DUCE.IResource)_worldTransform).GetHandle(channel);
            if (!hWorldTransform.IsNull)
            {
                // Release the world transform from this channel if it's currently on the channel.
                ((DUCE.IResource)_worldTransform).ReleaseOnChannel(channel);
            }

            // release all the visual target resources.
            base.ReleaseUCEResources(channel, outOfBandChannel);
        }

        /// <summary>
        /// The HwndTarget needs to see all windows messages so that
        /// it can appropriately react to them.
        /// </summary>
        /// <SecurityNote>
        /// Critical - accepts unmanaged pointer handle, also
        /// Elevates permissions via unsafe native methods, handles messages to
        /// retrieve automation provider the last one is most risky
        /// </SecurityNote>
        [SecurityCritical]
        internal IntPtr HandleMessage(WindowMessage msg, IntPtr wparam, IntPtr lparam)
        {
            IntPtr handled = new IntPtr(0x1); // return 1 if the message is handled by this method exclusively.
            IntPtr unhandled = IntPtr.Zero; // return 0 if other win procs should be called.
            IntPtr result = unhandled;

            if (msg == s_updateWindowSettings)
            {
                // Make sure we enable the render target if the window is visible.
                if (SafeNativeMethods.IsWindowVisible(_hWnd.MakeHandleRef(this)))
                {
                    UpdateWindowSettings(true);
                }
            }
            else if (msg == s_needsRePresentOnWake)
            {
                //
                // If the session is disconnected (due to machine lock) or in a suspended power
                // state, don't invalidate the window immediately, unless
                // we're within the allowed failure window after an unlock (See member comments on
                // _lastWakeOrUnlockEvent and _allowedPresentFailureDelay for explanation).
                // Save the invalidate for when the wake/unlock does occur, so that we avoid the
                // WM_PAINT/Invalidate storm, by setting _needsRePresentOnWake.
                //
                // If we've previously received this message and we don't know that we're
                // disconnected or suspended, we may be a window that has been created since a
                // lock/disconnect occurred, and thus didn't get the message. Set the
                // _nedsRePresentOnWake flag in this case too.
                //

                TimeSpan delta = DateTime.Now - _lastWakeOrUnlockEvent;
                bool fWithinPresentRetryWindow = delta.TotalSeconds < _allowedPresentFailureDelay;

                if (_isSessionDisconnected || _isSuspended || (_hasRePresentedSinceWake && !fWithinPresentRetryWindow))
                {
                    _needsRePresentOnWake = true;
                }
                else
                {
                    if (!_hasRePresentedSinceWake || fWithinPresentRetryWindow)
                    {
                        UnsafeNativeMethods.InvalidateRect(_hWnd.MakeHandleRef(this), IntPtr.Zero , true);
                        DoPaint();
                        _hasRePresentedSinceWake = true;
                    }
                }

                return handled;
            }


            if (IsDisposed)
            {
                return result;
            }

            switch (msg)
                {
                case WindowMessage.WM_ERASEBKGND:
                    result = handled; // Indicates that this message is handled.
                    break;

                case WindowMessage.WM_PAINT:
                    DoPaint();
                    result = handled;
                    break;

                case WindowMessage.WM_SIZE:

                    //
                    // NTRAID#Longhorn-1946030-2007/03/23-[....]:
                    //      When locked on downlevel, MIL stops rendering and invalidates the
                    //      window causing WM_PAINT. When the window is layered and minimized
                    //      before the lock, it'll never get the WM_PAINT on unlock and the MIL will
                    //      never get out of the "don't render" state.
                    //
                    //      To work around this, we will invalidate ourselves on restore and not
                    //      render while minimized.
                    //

                    // If the Window is in minimized state, don't do layout. otherwise, in some cases, it would
                    // pollute the measure data based on the Minized window size.
                    if (NativeMethods.IntPtrToInt32(wparam) != NativeMethods.SIZE_MINIMIZED)
                    {
                        // Dev10 bug #796388 is caused by a race condition in Windows 7 (and possibly
                        // Windows Vista, though we haven't observed the effect there).
                        // Sometimes when we restore from minimized, when we present into the newly
                        // resized window, the present silently fails, and we end up with garbage in
                        // our window buffer. This work around queues another invalidate to occur after 100ms.
                        if (_isMinimized)
                        {
                            _restoreDT.Start();
                        }

                        _isMinimized = false;
                        DoPaint();

                        OnResize();
                    }
                    else
                    {
                        _isMinimized = true;
                    }

                    break;

                case WindowMessage.WM_SETTINGCHANGE:
                    if (OnSettingChange(NativeMethods.IntPtrToInt32(wparam)))
                    {
                        UnsafeNativeMethods.InvalidateRect(_hWnd.MakeHandleRef(this), IntPtr.Zero , true);
                    }
                    break;

                case WindowMessage.WM_GETOBJECT:
                    result = CriticalHandleWMGetobject( wparam, lparam, RootVisual, _hWnd );
                    break;

                case WindowMessage.WM_WINDOWPOSCHANGING:
                    OnWindowPosChanging(lparam);
                    break;

                case WindowMessage.WM_WINDOWPOSCHANGED:
                    OnWindowPosChanged(lparam);
                    break;

                case WindowMessage.WM_SHOWWINDOW:
                    bool enableRenderTarget = (wparam != IntPtr.Zero);
                    OnShowWindow(enableRenderTarget);
                    //
                    //  Dev10 #453285
                    //      When locked on downlevel, MIL stops rendering and invalidates the
                    //      window causing WM_PAINT. When the window is layered and hidden
                    //      before the lock, it won't get the WM_PAINT on unlock and the MIL will
                    //      never get out of the "don't render" state if the window is shown again.
                    //
                    //      To work around this, we will invalidate the window ourselves on Show().
                    if (enableRenderTarget)
                    {
                        DoPaint();
                    }
                    break;

                case WindowMessage.WM_ENTERSIZEMOVE:
                    OnEnterSizeMove();
                    break;

                case WindowMessage.WM_EXITSIZEMOVE:
                    OnExitSizeMove();
                    break;

                case WindowMessage.WM_STYLECHANGING:
                    unsafe
                    {
                        NativeMethods.STYLESTRUCT * styleStruct = (NativeMethods.STYLESTRUCT *) lparam;

                        if ((int)wparam == NativeMethods.GWL_EXSTYLE)
                        {
                            if(UsesPerPixelOpacity)
                            {
                                // We need layered composition to accomplish per-pixel opacity.
                                //
                                styleStruct->styleNew |= NativeMethods.WS_EX_LAYERED;
                            }
                            else
                            {
                                // No properties that require layered composition exist.
                                // Make sure the layered bit is off.
                                //
                                // Note: this prevents an external program from making
                                // us system-layered (if we are a top-level window).
                                //
                                // If we are a child window, we still can't stop our
                                // parent from being made system-layered, and we will
                                // end up leaving visual artifacts on the screen under
                                // WindowsXP.
                                //
                                styleStruct->styleNew &= (~NativeMethods.WS_EX_LAYERED);
                            }
                        }
                    }

                    break;

                case WindowMessage.WM_STYLECHANGED:
                    unsafe
                    {
                        bool updateWindowSettings = false;

                        NativeMethods.STYLESTRUCT * styleStruct = (NativeMethods.STYLESTRUCT *) lparam;

                        if ((int)wparam == NativeMethods.GWL_STYLE)
                        {
                            bool oldIsChild = (styleStruct->styleOld & NativeMethods.WS_CHILD) == NativeMethods.WS_CHILD;
                            bool newIsChild = (styleStruct->styleNew & NativeMethods.WS_CHILD) == NativeMethods.WS_CHILD;
                            updateWindowSettings = (oldIsChild != newIsChild);
                        }
                        else
                        {
                            bool oldIsRTL = (styleStruct->styleOld & NativeMethods.WS_EX_LAYOUTRTL) == NativeMethods.WS_EX_LAYOUTRTL;
                            bool newIsRTL  = (styleStruct->styleNew & NativeMethods.WS_EX_LAYOUTRTL) == NativeMethods.WS_EX_LAYOUTRTL;
                            updateWindowSettings = (oldIsRTL != newIsRTL);
                        }

                        if(updateWindowSettings)
                        {
                            UpdateWindowSettings();
                        }
                    }

                    break;

                //
                // NTRAID#Longhorn-1967619-2007/03/23-[....]:
                //      When a Fast User Switch happens, MIL gets an invalid display error when trying to
                //      render and they invalidate the window resulting in us getting a WM_PAINT. For
                //      layered windows, we get the WM_PAINT immediately which causes us to
                //      tell MIL to render and the cycle repeats. On Vista, this creates an infinite loop.
                //      Downlevel there isn't a loop, but the layered window will never update again.
                //
                //      To work around this problem, we'll make sure not to tell MIL to render when
                //      we're switched out and will render on coming back.
                //
                case WindowMessage.WM_WTSSESSION_CHANGE:
                    switch (NativeMethods.IntPtrToInt32(wparam))
                    {
                        // Session is disconnected. Due to:
                        // 1. Switched to a different user
                        // 2. TS logoff
                        // 3. Screen locked
                        case NativeMethods.WTS_CONSOLE_DISCONNECT:
                        case NativeMethods.WTS_REMOTE_DISCONNECT:
                        case NativeMethods.WTS_SESSION_LOCK:
                            _hasRePresentedSinceWake = false;
                            _isSessionDisconnected = true;

                            _lastWakeOrUnlockEvent = DateTime.MinValue;

                            break;

                        // Session is reconnected. See above
                        case NativeMethods.WTS_CONSOLE_CONNECT:
                        case NativeMethods.WTS_REMOTE_CONNECT:
                        case NativeMethods.WTS_SESSION_UNLOCK:
                            _isSessionDisconnected = false;
                            if (_needsRePresentOnWake)
                            {
                                UnsafeNativeMethods.InvalidateRect(_hWnd.MakeHandleRef(this), IntPtr.Zero , true);
                                _needsRePresentOnWake = false;
                            }
                            DoPaint();

                            _lastWakeOrUnlockEvent = DateTime.Now;

                            break;

                        default:
                            break;
                    }

                    break;

                //
                // NTRAID#Longhorn-1975236-2007/05/22-[....]:
                //      Downlevel, if we try to present a layered window while suspended the app will crash.
                //      This has been fixed in Vista but we still need to work around it for older versions
                //      by not invalidating while suspended.
                //
                case WindowMessage.WM_POWERBROADCAST:
                    switch (NativeMethods.IntPtrToInt32(wparam))
                    {
                        case NativeMethods.PBT_APMSUSPEND:
                            _isSuspended = true;
                            _hasRePresentedSinceWake = false;

                            _lastWakeOrUnlockEvent = DateTime.MinValue;

                            break;

                        case NativeMethods.PBT_APMRESUMESUSPEND:
                        case NativeMethods.PBT_APMRESUMECRITICAL:
                        case NativeMethods.PBT_APMRESUMEAUTOMATIC:
                            _isSuspended = false;
                            if (_needsRePresentOnWake)
                            {
                                UnsafeNativeMethods.InvalidateRect(_hWnd.MakeHandleRef(this), IntPtr.Zero , true);
                                _needsRePresentOnWake = false;
                            }
                            DoPaint();

                            _lastWakeOrUnlockEvent = DateTime.Now;

                            break;
                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }

            return result;
        }

        /// <SecurityNote>
        ///     Critical: calls critical code.
        /// </SecurityNote>
        [SecurityCritical]
        private void OnMonitorPowerEvent(object sender, MonitorPowerEventArgs eventArgs)
        {
            OnMonitorPowerEvent(sender, eventArgs.PowerOn, /*paintOnWake*/true);
        }

        /// <SecurityNote>
        ///     Critical: calls critical code.
        /// </SecurityNote>
        [SecurityCritical]
        private void OnMonitorPowerEvent(object sender, bool powerOn, bool paintOnWake)
        {
            if (powerOn)
            {
                _isSuspended = false;
                if (paintOnWake)
                {
                    if (_needsRePresentOnWake)
                    {
                        UnsafeNativeMethods.InvalidateRect(_hWnd.MakeHandleRef(this), IntPtr.Zero, true);
                        _needsRePresentOnWake = false;
                    }
                    DoPaint();
                }

                _lastWakeOrUnlockEvent = DateTime.Now;
            }
            else
            {
                _isSuspended = true;
                _hasRePresentedSinceWake = false;

                _lastWakeOrUnlockEvent = DateTime.MinValue;
            }
        }

        /// <summary>
        /// Invalidates self, designed to be called as a DispatcherTimer event handler.
        /// </summary>
        /// <SecurityNote>
        /// Critical - Elevates permissions via unsafe native methods, calls Invalidate
        /// TreatAsSafe - Doesn't return or expose critical handle _hWnd
        /// </SecurityNote>
        [SecuritySafeCritical]
        private void InvalidateSelf(object s, EventArgs args)
        {
            UnsafeNativeMethods.InvalidateRect(_hWnd.MakeHandleRef(this), IntPtr.Zero, true);
            DispatcherTimer sourceDT = (DispatcherTimer)s;
            if (sourceDT != null)
            {
                Debug.Assert(_restoreDT == sourceDT);

                sourceDT.Stop();
            }

        }

        /// <summary>
        /// Paints a rect
        ///
        /// Note: This gets called a lot to help with layered window problems even when
        ///         the window isn't layered, but that's okay because rcPaint will be empty.
        ///
        /// </summary>
        /// <SecurityNote>
        /// Critical - Elevates permissions via unsafe native methods, calls into begin paint
        /// </SecurityNote>
        [SecurityCritical]
        private void DoPaint()
        {
            NativeMethods.PAINTSTRUCT ps = new NativeMethods.PAINTSTRUCT();
            NativeMethods.HDC hdc;

            HandleRef handleRef = new HandleRef(this, _hWnd);
            hdc.h = UnsafeNativeMethods.BeginPaint(handleRef, ref ps);
            int retval = UnsafeNativeMethods.GetWindowLong(handleRef, NativeMethods.GWL_EXSTYLE);

            NativeMethods.RECT rcPaint = new NativeMethods.RECT(ps.rcPaint_left, ps.rcPaint_top, ps.rcPaint_right, ps.rcPaint_bottom);

            //
            // If we get a BeginPaint with an empty rect then check
            // if this is a special layered, non-redirected window
            // which would mean we need to do a full paint when it
            // won't cause a problem.
            //
            if (rcPaint.IsEmpty
                && ((retval & NativeMethods.WS_EX_LAYERED) != 0)
                && !UnsafeNativeMethods.GetLayeredWindowAttributes(_hWnd.MakeHandleRef(this), IntPtr.Zero, IntPtr.Zero, IntPtr.Zero)
                && !_isSessionDisconnected
                && !_isMinimized
                && (!_isSuspended || (UnsafeNativeMethods.GetSystemMetrics(SM.REMOTESESSION) != 0))) // 
                                                                                                     // notifications for the server monitor are being broad-casted when the
                                                                                                     // machine is in a non-local TS session. See Dev10 bug for more details.
            {
                rcPaint = new NativeMethods.RECT(
                          0,
                          0,
                          _hwndClientRectInScreenCoords.right - _hwndClientRectInScreenCoords.left,
                          _hwndClientRectInScreenCoords.bottom - _hwndClientRectInScreenCoords.top);
            }

            AdjustForRightToLeft(ref rcPaint, handleRef);

            if (!rcPaint.IsEmpty)
            {
                InvalidateRect(rcPaint);
            }

            UnsafeNativeMethods.EndPaint(_hWnd.MakeHandleRef(this), ref ps);
        }

        /// <SecurityNote>
        /// Critical - 1) This method exposes the automation object which can be used to
        /// query the system for critical information or spoof input.
        /// 2) it Asserts to call ReturnRawElementProvider
        /// </SecurityNote>
        [SecurityCritical]
        private static IntPtr CriticalHandleWMGetobject(IntPtr wparam, IntPtr lparam, Visual root, IntPtr handle)
        {
            try
            {
                if (root == null)
                {
                    // Valid case, but need to handle separately. For now, return 0 to avoid exceptions
                    // in referencing this later on. Real solution is more complex, see WindowsClient#873800.
                    return IntPtr.Zero;
                }

                AutomationPeer peer = null;

                if (root.CheckFlagsAnd(VisualFlags.IsUIElement))
                {
                    UIElement uiroot = (UIElement)root;

                    peer = UIElementAutomationPeer.CreatePeerForElement(uiroot);

                    //there si no specific peer for this UIElement, create a generic root
                    if(peer == null)
                        peer = uiroot.CreateGenericRootAutomationPeer();

                    if(peer != null)
                        peer.Hwnd = handle;
                }

                // This can happen if the root visual is not UIElement. In this case,
                // attempt to find one in the visual tree.
                if (peer == null)
                {
                    peer = UIElementAutomationPeer.GetRootAutomationPeer(root, handle);
                }

                if (peer == null)
                {
                    return IntPtr.Zero;
                }

                // get the element proxy
                // it's ok to pass the same peer as reference connected peer here because
                // it's guaranteed to be a connected one (it's initialized as root already)
                IRawElementProviderSimple el = ElementProxy.StaticWrap(peer, peer);

                peer.AddToAutomationEventList();

                // The assert here is considered OK
                // as we're assuming the WM_GETOBJECT is coming only from a PostMessage of an Hwnd.
                // to do the post message - you needed to have Unmanaged code permission
                //

                PermissionSet unpackPermissionSet = new PermissionSet(PermissionState.None);
                // below permissions needed to unpack an object.
                unpackPermissionSet.AddPermission(new SecurityPermission(SecurityPermissionFlag.SerializationFormatter | SecurityPermissionFlag.UnmanagedCode | SecurityPermissionFlag.RemotingConfiguration));
                unpackPermissionSet.AddPermission(new System.Net.DnsPermission(PermissionState.Unrestricted));
                unpackPermissionSet.AddPermission(new System.Net.SocketPermission(PermissionState.Unrestricted));

                unpackPermissionSet.Assert();
                try
                {
                    return AutomationInteropProvider.ReturnRawElementProvider(handle, wparam, lparam, el);
                }
                finally
                {
                    CodeAccessPermission.RevertAll();
                }
            }
#pragma warning disable 56500
            catch (Exception e)
            {
                if(CriticalExceptions.IsCriticalException(e))
                {
                    throw;
                }

                return new IntPtr(Marshal.GetHRForException(e));
            }
#pragma warning restore 56500
        }

        /// <summary>
        ///     Adjusts a RECT to compensate for Win32 RTL conversion logic
        /// </summary>
        /// <remarks>
        ///     When a window is marked with the WS_EX_LAYOUTRTL style, Win32
        ///     mirrors the coordinates during the various translation APIs.
        ///
        ///     Avalon also sets up mirroring transforms so that we properly
        ///     mirror the output since we render to DirectX, not a GDI DC.
        ///
        ///     Unfortunately, this means that our coordinates are already mirrored
        ///     by Win32, and Avalon mirrors them again.  To solve this
        ///     problem, we un-mirror the coordinates from Win32 before painting
        ///     in Avalon.
        /// </remarks>
        /// <param name="rc">
        ///     The RECT to be adjusted
        /// </param>
        /// <param name="handleRef">
        /// </param>
        internal void AdjustForRightToLeft(ref NativeMethods.RECT rc, HandleRef handleRef)
        {
            int windowStyle = SafeNativeMethods.GetWindowStyle(handleRef, true);

            if(( windowStyle & NativeMethods.WS_EX_LAYOUTRTL ) == NativeMethods.WS_EX_LAYOUTRTL)
            {
                NativeMethods.RECT rcClient = new NativeMethods.RECT();
                SafeNativeMethods.GetClientRect(handleRef, ref rcClient);

                int width   = rc.right - rc.left;       // preserve width
                rc.right    = rcClient.right - rc.left; // set right of rect to be as far from right of window as left of rect was from left of window
                rc.left     = rc.right - width;         // restore width by adjusting left and preserving right
            }
        }

        /// <summary>
        /// Force total re-rendering to handle system parameters change
        /// (font smoothing settings, gamma correction, etc.)
        ///</summary>
        ///<returns>true if rerendering was forced</returns>
        /// <SecurityNote>
        ///   Critical: This can be used to cause annoyance by causing re rendering
        /// </SecurityNote>
        [SecurityCritical]
        private bool OnSettingChange(Int32 firstParam)
        {
            if ( (int)firstParam == (int)NativeMethods.SPI_SETFONTSMOOTHING ||
                 (int)firstParam == (int)NativeMethods.SPI_SETFONTSMOOTHINGTYPE ||
                 (int)firstParam == (int)NativeMethods.SPI_SETFONTSMOOTHINGCONTRAST ||
                 (int)firstParam == (int)NativeMethods.SPI_SETFONTSMOOTHINGORIENTATION ||
                 (int)firstParam == (int)NativeMethods.SPI_SETDISPLAYPIXELSTRUCTURE ||
                 (int)firstParam == (int)NativeMethods.SPI_SETDISPLAYGAMMA ||
                 (int)firstParam == (int)NativeMethods.SPI_SETDISPLAYCLEARTYPELEVEL ||
                 (int)firstParam == (int)NativeMethods.SPI_SETDISPLAYTEXTCONTRASTLEVEL
                )
            {
                HRESULT.Check(MILUpdateSystemParametersInfo.Update());
                return true;
            }

            return false;
        }

        /// <summary>
        /// This function should be called to paint the specified
        /// region of the window along with any other pending
        /// changes.  While this function is generally called
        /// in response to a WM_PAINT it is up to the user to
        /// call BeginPaint and EndPaint or to otherwise validate
        /// the bitmap region.
        /// </summary>
        /// <param name="rcDirty">The rectangle that is dirty.</param>
        private void InvalidateRect(NativeMethods.RECT rcDirty)
        {
            DUCE.ChannelSet channelSet = MediaContext.From(Dispatcher).GetChannels();
            DUCE.Channel channel = channelSet.Channel;
            DUCE.Channel outOfBandChannel = channelSet.OutOfBandChannel;

            // handle InvalidateRect requests only if we have uce resources.
            if (_compositionTarget.IsOnChannel(channel))
            {
                //
                // Send a message with the invalid region to the compositor. We create a little batch to send this
                // out of order.
                //
                DUCE.CompositionTarget.Invalidate(
                    _compositionTarget.GetHandle(outOfBandChannel),
                    ref rcDirty,
                    outOfBandChannel);
            }
        }

        /// <summary>
        /// Calling this function causes us to update state to reflect a
        /// size change of the underlying HWND
        /// </summary>
        ///<SecurityNote>
        /// Critical - accesses a critical member (_hwnd)
        /// TreatAsSafe - uses the _hwnd to call a safeNativeMethods. Data is cached - but not considered critical.
        ///</SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        private void OnResize()
        {
#if DEBUG
            MediaTrace.HwndTarget.Trace("OnResize");
#endif

            // handle OnResize requests only if we have uce resources.
            if (_compositionTarget.IsOnAnyChannel)
            {
                MediaContext mctx = MediaContext.From(Dispatcher);

                //
                // Let the render target know that window size has changed.
                //

                UpdateWindowSettings();

                //
                // Push client size chnage to the visual target.
                //
                Rect clientRect = new Rect(
                    0,
                    0,
                    (float)(Math.Ceiling((double)(_hwndClientRectInScreenCoords.right - _hwndClientRectInScreenCoords.left))),
                    (float)(Math.Ceiling((double)(_hwndClientRectInScreenCoords.bottom - _hwndClientRectInScreenCoords.top))));

                StateChangedCallback(
                    new object[] { HostStateFlags.ClipBounds, null, clientRect });

                mctx.Resize(this);

                Int32 style = UnsafeNativeMethods.GetWindowLong(_hWnd.MakeHandleRef(this), NativeMethods.GWL_STYLE);
                if (_userInputResize || _usesPerPixelOpacity ||
                    ((style & NativeMethods.WS_CHILD) != 0 && Utilities.IsCompositionEnabled))
                {
                    //
                    // To ensure that the client area and the non-client area resize
                    // together, we need to wait, on resize, for the composition
                    // engine to present the resized frame. The call to CompleteRender
                    // blocks until that happens.
                    //
                    // When the user isn't resizing, the disconnect between client
                    // and non-client isn't as noticeable so we will err on the side
                    // of performance for multi-hwnd apps like Visual Studio.
                    //
                    // We think syncing is always necessary for layered windows.
                    //
                    // For child windows we also need to [....] to work-around some DWM issues (see Dev10 #739622, #782372).
                    //

                    mctx.CompleteRender();
                }
            }
        }


        /// <summary>
        /// Calculates the client and window rectangle in screen coordinates.
        /// </summary>
        ///<SecurityNote>
        /// Critical - calls critical methods (ClientToScreen)
        /// TreatAsSafe - no information returned, coordinates stored in member field.
        ///</SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        private void GetWindowRectsInScreenCoordinates()
        {
            NativeMethods.RECT rcClient = new NativeMethods.RECT();

            //
            // Get the window and client rectangles
            //

            SafeNativeMethods.GetWindowRect(_hWnd.MakeHandleRef(this), ref _hwndWindowRectInScreenCoords);
            SafeNativeMethods.GetClientRect(_hWnd.MakeHandleRef(this), ref rcClient);


            NativeMethods.POINT ptClientTopLeft = new NativeMethods.POINT(rcClient.left, rcClient.top);
            UnsafeNativeMethods.ClientToScreen(_hWnd.MakeHandleRef(this), ptClientTopLeft);

            NativeMethods.POINT ptClientBottomRight = new NativeMethods.POINT(rcClient.right, rcClient.bottom);
            UnsafeNativeMethods.ClientToScreen(_hWnd.MakeHandleRef(this), ptClientBottomRight);

            if(ptClientBottomRight.x >= ptClientTopLeft.x)
            {
                _hwndClientRectInScreenCoords.left = ptClientTopLeft.x;
                _hwndClientRectInScreenCoords.right = ptClientBottomRight.x;
            }
            else
            {
                // RTL windows will cause the right edge to be on the left...
                _hwndClientRectInScreenCoords.left = ptClientBottomRight.x;
                _hwndClientRectInScreenCoords.right = ptClientTopLeft.x;
            }

            if(ptClientBottomRight.y >= ptClientTopLeft.y)
            {
                _hwndClientRectInScreenCoords.top = ptClientTopLeft.y;
                _hwndClientRectInScreenCoords.bottom = ptClientBottomRight.y;
            }
            else
            {
                // RTL windows will cause the right edge to be on the left...
                // This doesn't affect top/bottom, but the code should be symmetrical.
                _hwndClientRectInScreenCoords.top = ptClientBottomRight.y;
                _hwndClientRectInScreenCoords.bottom = ptClientTopLeft.y;
            }

            // 
        }

        ///<SecurityNote>
        /// Critical - accepts an unmanaged pointer to a structure
        ///</SecurityNote>
        [SecurityCritical]
        private void OnWindowPosChanging(IntPtr lParam)
        {
            _windowPosChanging = true;

            UpdateWindowPos(lParam);
        }

        ///<SecurityNote>
        /// Critical - accepts an unmanaged pointer to a structure
        ///</SecurityNote>
        [SecurityCritical]
        private void OnWindowPosChanged(IntPtr lParam)
        {
            _windowPosChanging = false;

            UpdateWindowPos(lParam);
        }

        ///<SecurityNote>
        /// Critical - accepts an unmanaged pointer to a structure
        ///</SecurityNote>
        [SecurityCritical]
        private void UpdateWindowPos(IntPtr lParam)
        {
            //
            // We need to update the window settings used by the render thread when
            // 1) The size or position of the render target needs to change
            // 2) The render target needs to be enabled or disabled.
            //
            // Further, we need to synchronize the render thread during sizing operations.
            // This is because some APIs that the render thread uses (such as
            // UpdateLayeredWindow) have the unintended side-effect of also changing the
            // window size.  We can't let the render thread and the UI thread fight
            // over setting the window size.
            //
            // Generally, Windows sends our window to messages that bracket the size
            // operation:
            // 1) WM_WINDOWPOSCHANGING
            //    Here we synchronize with the render thread, and ask the render thread
            //    to not render to this window for a while.
            // 2) WM_WINDOWPOSCHANGED
            //    This is after the window size has actually been changed, so we tell
            //    the render thread that it can render to the window again.
            //
            // However, there are complications.  Sometimes Windows will send a
            // WM_WINDOWPOSCHANGING without sending a WM_WINDOWPOSCHANGED.  This happens
            // when the window size is not really going to change.  Also note that
            // more than just size/position information is provided by these messages.
            // We'll get these messages when nothing but the z-order changes for instance.
            //


            //
            // The first order of business is to determine if the render target
            // size or position changed.  If so, we need to pass this information to
            // the render thread.
            //
            NativeMethods.WINDOWPOS windowPos = (NativeMethods.WINDOWPOS)UnsafeNativeMethods.PtrToStructure(lParam, typeof(NativeMethods.WINDOWPOS));
            bool isMove = (windowPos.flags & NativeMethods.SWP_NOMOVE) == 0;
            bool isSize = (windowPos.flags & NativeMethods.SWP_NOSIZE) == 0;
            bool positionChanged = (isMove || isSize);
            if (positionChanged)
            {
                //
                // We have found that sometimes we get told that the size or position
                // of the window has changed, when it really hasn't.  So we double
                // check here.  This is critical because we won't be given a
                // WM_WINDOWPOSCHANGED unless the size or position really had changed.
                //
                if (!isMove)
                {
                    // This is just to avoid any possible integer overflow problems.
                    windowPos.x = windowPos.y = 0;
                }
                if (!isSize)
                {
                    // This is just to avoid any possible integer overflow problems.
                    windowPos.cx = windowPos.cy = 0;
                }

                //
                // WINDOWPOS stores the window coordinates relative to its parent.
                // If the parent is NULL, then these are already screen coordinates.
                // Otherwise, we need to convert to screen coordinates.
                //
                NativeMethods.RECT windowRectInScreenCoords = new NativeMethods.RECT(windowPos.x, windowPos.y, windowPos.x + windowPos.cx, windowPos.y + windowPos.cy);
                IntPtr hwndParent = UnsafeNativeMethods.GetParent(new HandleRef(null, windowPos.hwnd));
                if(hwndParent != IntPtr.Zero)
                {
                    SafeSecurityHelper.TransformLocalRectToScreen(new HandleRef(null, hwndParent), ref windowRectInScreenCoords);
                }

                if (!isMove)
                {
                    // We weren't actually moving, so the WINDOWPOS structure
                    // did not contain valid (x,y) information.  Just use our
                    // old values.
                    int width = (windowRectInScreenCoords.right - windowRectInScreenCoords.left);
                    int height = (windowRectInScreenCoords.bottom - windowRectInScreenCoords.top);
                    windowRectInScreenCoords.left = _hwndWindowRectInScreenCoords.left;
                    windowRectInScreenCoords.right = windowRectInScreenCoords.left + width;
                    windowRectInScreenCoords.top = _hwndWindowRectInScreenCoords.top;
                    windowRectInScreenCoords.bottom = windowRectInScreenCoords.top + height;
                }

                if (!isSize)
                {
                    // We weren't actually sizing, so the WINDOWPOS structure
                    // did not contain valid (cx,cy) information.  Just use our
                    // old values.
                    int width = (_hwndWindowRectInScreenCoords.right - _hwndWindowRectInScreenCoords.left);
                    int height = (_hwndWindowRectInScreenCoords.bottom - _hwndWindowRectInScreenCoords.top);

                    windowRectInScreenCoords.right = windowRectInScreenCoords.left + width;
                    windowRectInScreenCoords.bottom = windowRectInScreenCoords.top + height;
                }

                positionChanged = (   _hwndWindowRectInScreenCoords.left != windowRectInScreenCoords.left
                                   || _hwndWindowRectInScreenCoords.top != windowRectInScreenCoords.top
                                   || _hwndWindowRectInScreenCoords.right != windowRectInScreenCoords.right
                                   || _hwndWindowRectInScreenCoords.bottom != windowRectInScreenCoords.bottom);
            }


            //
            // The second order of business is to determine whether or not the render
            // target should be enabled.  If we are disabling the render target, then
            // we need to synchronize with the render thread.  Basically,
            // a WM_WINDOWPOSCHANGED always enables the render target it the window is
            // visible.  And a WM_WINDOWPOSCHANGING will disable the render target
            // unless it is not really a size/move, in which case we will not be sent
            // a WM_WINDOWPOSCHANGED, so we can't disable the render target.
            //
            bool enableRenderTarget = SafeNativeMethods.IsWindowVisible(_hWnd.MakeHandleRef(this));
            if(enableRenderTarget)
            {
                if(_windowPosChanging && (positionChanged))
                {
                    enableRenderTarget = false;
                }
            }


            if (positionChanged || (enableRenderTarget != _isRenderTargetEnabled))
            {
                UpdateWindowSettings(enableRenderTarget);
            }
        }

        bool _windowPosChanging;

        private void OnShowWindow(bool enableRenderTarget)
        {
            if (enableRenderTarget != _isRenderTargetEnabled)
            {
                UpdateWindowSettings(enableRenderTarget);
            }
        }

        private void OnEnterSizeMove()
        {
            _userInputResize = true;
        }

        private void OnExitSizeMove()
        {
            if (_windowPosChanging)
            {
                _windowPosChanging = false;
                UpdateWindowSettings(true);
            }

            _userInputResize = false;
        }

        private void UpdateWindowSettings()
        {
            UpdateWindowSettings(_isRenderTargetEnabled, null);
        }

        private void UpdateWindowSettings(bool enableRenderTarget)
        {
            UpdateWindowSettings(enableRenderTarget, null);
        }

        ///<SecurityNote>
        /// Critical - calls critical methods (UpdateWindowSettings)
        /// TreatAsSafe - just updates composition information for this window,
        ///               does not accept arbitrary input.
        ///</SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        private void UpdateWindowSettings(bool enableRenderTarget, DUCE.ChannelSet? channelSet)
        {
            MediaContext mctx = MediaContext.From(Dispatcher);

            // It's possible that this method could be called multiple times in a row
            // with the same enableRenderTarget value and we'd like to minimize the
            // number of flushes on the OOB channel by only flushing when transitioning
            // rather than ever time we get a disable.
            bool firstTimeRenderTargetDisabled = false;
            bool firstTimeRenderTargetEnabled = false;
            if (_isRenderTargetEnabled != enableRenderTarget)
            {
                _isRenderTargetEnabled = enableRenderTarget;
                firstTimeRenderTargetDisabled = !enableRenderTarget;
                firstTimeRenderTargetEnabled = enableRenderTarget;

                // Basic idea: the render thread and the UI thread have a
                // race condition when the UI thread wants to modify
                // HWND data and the render thread is using it.  The render
                // thread can paint garbage on the screen, and it can also
                // cause the old data to be set again (ULW issue, hence ULWEx).
                //
                // So we tell the render thread to stop rendering and then we
                // wait for them to stop when disabling the render target by
                // issuing the UpdateWindowSettings command synchronously on
                // an out-of-band channel.
            }


            // if we are disconnected we are done.
            if (!_compositionTarget.IsOnAnyChannel)
            {
                return;
            }

            //
            // Calculate the client rectangle in screen coordinates.
            //

            GetWindowRectsInScreenCoordinates();

            Int32 style = UnsafeNativeMethods.GetWindowLong(_hWnd.MakeHandleRef(this), NativeMethods.GWL_STYLE);
            Int32 exStyle = UnsafeNativeMethods.GetWindowLong(_hWnd.MakeHandleRef(this), NativeMethods.GWL_EXSTYLE);

            bool isLayered = (exStyle & NativeMethods.WS_EX_LAYERED) != 0;

            bool isChild = (style & NativeMethods.WS_CHILD) != 0;
            bool isRTL = (exStyle & NativeMethods.WS_EX_LAYOUTRTL) != 0;

            int width = _hwndClientRectInScreenCoords.right - _hwndClientRectInScreenCoords.left;
            int height = _hwndClientRectInScreenCoords.bottom - _hwndClientRectInScreenCoords.top;

            MILTransparencyFlags flags = MILTransparencyFlags.Opaque;
            // if (!DoubleUtil.AreClose(_opacity, 1.0))
            // {
            //     flags |= MILTransparencyFlags.ConstantAlpha;
            // }

            // if (_colorKey.HasValue)
            // {
            //     flags |= MILTransparencyFlags.ColorKey;
            // }

            if (_usesPerPixelOpacity)
            {
                flags |= MILTransparencyFlags.PerPixelAlpha;
            }

            if (!isLayered && flags != MILTransparencyFlags.Opaque)
            {
                // The window is not layered, but it should be -- set the layered flag.
                UnsafeNativeMethods.SetWindowLong(_hWnd.MakeHandleRef(this), NativeMethods.GWL_EXSTYLE, new IntPtr(exStyle | NativeMethods.WS_EX_LAYERED));
            }
            else if (isLayered && flags == MILTransparencyFlags.Opaque)
            {
                // The window is layered but should not be -- unset the layered flag.
                UnsafeNativeMethods.SetWindowLong(_hWnd.MakeHandleRef(this), NativeMethods.GWL_EXSTYLE, new IntPtr(exStyle & ~NativeMethods.WS_EX_LAYERED));
            }
            else if(isLayered && flags != MILTransparencyFlags.Opaque && _isRenderTargetEnabled && (width == 0 || height == 0))
            {
                // The window is already layered, and it should be.  But we are enabling a window
                // that is has a 0-size dimension.  This may cause us to leave the last sprite
                // on the screen.  The best way to get rid of this is to just make the entire
                // sprite transparent.

                NativeMethods.BLENDFUNCTION blend = new NativeMethods.BLENDFUNCTION();
                blend.BlendOp = NativeMethods.AC_SRC_OVER;
                blend.SourceConstantAlpha = 0; // transparent
                UnsafeNativeMethods.UpdateLayeredWindow(_hWnd.h, IntPtr.Zero, null, null, IntPtr.Zero, null, 0, ref blend, NativeMethods.ULW_ALPHA);
            }
            isLayered = (flags != MILTransparencyFlags.Opaque);

            if (channelSet == null)
            {
                channelSet = mctx.GetChannels();
            }

            // If this is the first time going from disabled -> enabled, flush
            // the out of band to make sure all disable packets have been
            // processed before sending the enable later below. Otherwise,
            // the enable could be ignored if the disable cookie doesn't match
            DUCE.Channel outOfBandChannel = channelSet.Value.OutOfBandChannel;
            if (firstTimeRenderTargetEnabled)
            {
                outOfBandChannel.Commit();
                outOfBandChannel.SyncFlush();
            }

            // Every UpdateWindowSettings command that disables the render target is
            // assigned a new cookie.  Every UpdateWindowSettings command that enables
            // the render target uses the most recent cookie.  This allows the
            // compositor to ignore UpdateWindowSettings(enable) commands that come
            // out of order due to us disabling out-of-band and enabling in-band.
            if (!_isRenderTargetEnabled)
            {
                _disableCookie++;
            }

            //
            // When enabling the render target, stay in-band.  This allows any
            // client-side rendering instructions to be included in the same packet.
            // Otherwise pass in the OutOfBand handle.
            //
            DUCE.Channel channel = channelSet.Value.Channel;
            DUCE.CompositionTarget.UpdateWindowSettings(
                _isRenderTargetEnabled ? _compositionTarget.GetHandle(channel) : _compositionTarget.GetHandle(outOfBandChannel),
                _hwndClientRectInScreenCoords,
                Colors.Transparent, // _colorKey.GetValueOrDefault(Colors.Black),
                1.0f, // (float)_opacity,
                isLayered ? (_usesPerPixelOpacity ? MILWindowLayerType.ApplicationManagedLayer : MILWindowLayerType.SystemManagedLayer) : MILWindowLayerType.NotLayered,
                flags,
                isChild,
                isRTL,
                _isRenderTargetEnabled,
                _disableCookie,
                _isRenderTargetEnabled ? channel : outOfBandChannel);

            if (_isRenderTargetEnabled)
            {
                //
                // Re-render the visual tree.
                //

                mctx.PostRender();
            }
            else
            {
                if (firstTimeRenderTargetDisabled)
                {
                    outOfBandChannel.CloseBatch();
                    outOfBandChannel.Commit();

                    //
                    // Wait for the command to be processed -- [....] flush will take care
                    // of that while being safe w.r.t. zombie partitions.
                    //

                    outOfBandChannel.SyncFlush();
                }

                // If we disabled the render target, we run the risk of leaving it disabled.
                // One such example is when a window is programatically sized, but then
                // GetMinMaxInfo denies the change.  We do not receive any message that would
                // allow us to re-enable the render targer.  To cover these odd cases, we
                // post ourselves a message to possible re-enable the render target when
                // we are done with the current message processing.
                UnsafeNativeMethods.PostMessage(new HandleRef(this, _hWnd), s_updateWindowSettings, IntPtr.Zero, IntPtr.Zero);
            }
        }

        /// <summary>
        /// Gets and sets the root Visual of this HwndTarget.
        /// </summary>
        /// <remarks>
        ///     Callers must have UIPermission(UIPermissionWindow.AllWindows) to call this API.
        /// </remarks>
        /// <SecurityNote>
        ///     Critical: This code accesses HWND which is critical and setting RootVisual
        ///     is critical
        ///     PublicOK: This code blocks inheritance and public callers via Inheritance (in base class) and link demands
        /// </SecurityNote>
        public override Visual RootVisual
        {
            [SecurityCritical]
            [UIPermissionAttribute(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
            set
            {

                base.RootVisual = value;

                if (value != null)
                {
                    // UIAutomation listens for the EventObjectUIFragmentCreate WinEvent to
                    // understand when UI that natively implements UIAutomation comes up
                    // 



                    UnsafeNativeMethods.NotifyWinEvent(UnsafeNativeMethods.EventObjectUIFragmentCreate, _hWnd.MakeHandleRef(this), 0, 0);
                }
            }
        }

        /// <summary>
        /// Returns matrix that can be used to transform coordinates from this
        /// target to the rendering destination device.
        /// </summary>
        public override Matrix TransformToDevice
        {
            get
            {
                VerifyAPIReadOnly();
                Matrix m = Matrix.Identity;
                m.Scale(_devicePixelsPerInchX / 96.0, _devicePixelsPerInchY / 96.0);
                return m;
            }
        }

        /// <summary>
        /// Returns matrix that can be used to transform coordinates from
        /// the rendering destination device to this target.
        /// </summary>
        public override Matrix TransformFromDevice
        {
            get
            {
                VerifyAPIReadOnly();
                Matrix m = Matrix.Identity;
                m.Scale(96.0 / _devicePixelsPerInchX, 96.0 / _devicePixelsPerInchY);
                return m;
            }
        }

        /// <summary>
        /// This is the color that is drawn before everything else.  If
        /// this color has an alpha component other than 1 it will be ignored.
        /// </summary>
        public Color BackgroundColor
        {
            get
            {
                VerifyAPIReadOnly();

                return _backgroundColor;
            }
            set
            {
                VerifyAPIReadWrite();

                if (_backgroundColor != value)
                {
                    _backgroundColor = value;
                    MediaContext mctx = MediaContext.From(Dispatcher);

                    DUCE.ChannelSet channelSet = mctx.GetChannels();
                    DUCE.Channel channel = channelSet.Channel;
                    if (channel == null)
                    {
                        // MediaContext is in disconnected state, so we will send
                        // the clear color when CreateUCEResources gets called
                        Debug.Assert(!_compositionTarget.IsOnChannel(channel));
                    }
                    else
                    {
                        DUCE.CompositionTarget.SetClearColor(
                            _compositionTarget.GetHandle(channel),
                            _backgroundColor,
                            channel);
                        mctx.PostRender();
                    }
                }
            }
        }

        // /// <summary>
        // ///     Specifies the color to display as transparent.
        // /// </summary>
        // /// <remarks>
        // ///     Use null to indicate that no color should be transparent.
        // /// </remarks>
        // public Nullable<Color> ColorKey
        // {
        //     get
        //     {
        //         VerifyAPIReadOnly();
        //
        //         return _colorKey;
        //     }
        //
        //     set
        //     {
        //         VerifyAPIReadWrite();
        //
        //         if(_colorKey != value)
        //         {
        //             _colorKey = value;
        //
        //             UpdateWindowSettings();
        //         }
        //     }
        // }

        // /// <summary>
        // ///     Specifies the constant opacity to apply to the window.
        // /// </summary>
        // /// <remarks>
        // ///     The valid values range from [0..1].  Values outside of this range are clamped.
        // /// </remarks>
        // public double Opacity
        // {
        //     get
        //     {
        //         VerifyAPIReadOnly();
        //
        //         return _opacity;
        //     }
        //
        //     set
        //     {
        //         VerifyAPIReadWrite();
        //
        //         if(value < 0.0) value = 0.0;
        //         if(value > 1.0) value = 1.0;
        //
        //         if(!MS.Internal.DoubleUtil.AreClose(value, _opacity))
        //         {
        //             _opacity = value;
        //
        //             UpdateWindowSettings();
        //         }
        //     }
        // }

        /// <summary>
        ///     Specifies whether or not the per-pixel opacity of the window content
        ///     is respected.
        /// </summary>
        /// <remarks>
        ///     By enabling per-pixel opacity, the system will no longer draw the non-client area.
        /// </remarks>
        public bool UsesPerPixelOpacity
        {
            get
            {
                VerifyAPIReadOnly();

                return _usesPerPixelOpacity;
            }

            internal set
            {
                VerifyAPIReadWrite();

                if(_usesPerPixelOpacity != value)
                {
                    _usesPerPixelOpacity = value;

                    UpdateWindowSettings();
                }
            }
        }

        #region Notification Window

        [ThreadStatic]
        private static NotificationWindowHelper _notificationWindowHelper;

        private void EnsureNotificationWindow()
        {
            if (_notificationWindowHelper == null)
            {
                _notificationWindowHelper = new NotificationWindowHelper();
            }
        }

        private class MonitorPowerEventArgs : EventArgs
        {
            public MonitorPowerEventArgs(bool powerOn)
            {
                PowerOn = powerOn;
            }
            public bool PowerOn { get; private set; }
        }

        /// <summary>
        ///     Abstraction for the logic to get thread level
        ///     system notifications like PBT_POWERSETTINGCHANGE.
        ///     Ideally all such thread singleton messages for
        ///     hwnds of HwndTargets should be recieved by this
        ///     class. Only PBT_POWERSETTINGCHANGE is implemented
        ///     at this point, so as to limit the testing surface.
        ///     Others must be implemented in future as and when
        ///     possible.
        /// </summary>
        private class NotificationWindowHelper : IDisposable
        {
            #region Data

            /// <SecurityNode>
            ///     Critical: We dont want _notificationHwnd to be exposed and used
            ///         by anyone besides this class.
            /// </SecurityNode>
            [SecurityCritical]
            private HwndWrapper _notificationHwnd; // The hwnd used to listen system wide messages

            /// <SecurityNode>
            ///     Critical: _notificationHook is the hook to listen to window
            ///         messages. We want this to be critical that no one can get it
            ///         listen to window messages.
            /// </SecurityNode>
            [SecurityCritical]
            private HwndWrapperHook _notificationHook;

            private int _hwndTargetCount;
            public event EventHandler<MonitorPowerEventArgs> MonitorPowerEvent;
            private bool _monitorOn = true;

            ///<SecurityNote>
            /// Critical - obtained under an elevation.
            ///</SecurityNote>
            [SecurityCritical]
            private IntPtr _hPowerNotify;

            #endregion

            /// <SecurityNode>
            ///     Critical: Calls critical code.
            ///     TreatAsSafe: Doesn't expose the critical resource.
            /// </SecurityNode>
            [SecurityCritical, SecurityTreatAsSafe]
            public NotificationWindowHelper()
            {
                // Check for Vista or newer is needed for RegisterPowerSettingNotification.
                // This check needs to rescoped to the said method call, if other
                // notifications are implemented.
                if (Utilities.IsOSVistaOrNewer)
                {
                    // _notificationHook needs to be member variable otherwise
                    // it is GC'ed and we don't get messages from HwndWrapper
                    // (HwndWrapper keeps a WeakReference to the hook)

                    _notificationHook = new HwndWrapperHook(NotificationFilterMessage);
                    HwndWrapperHook[] wrapperHooks = { _notificationHook };

                    _notificationHwnd = new HwndWrapper(
                                                0,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0,
                                                "",
                                                IntPtr.Zero,
                                                wrapperHooks);

                    Guid monitorGuid = new Guid(NativeMethods.GUID_MONITOR_POWER_ON.ToByteArray());
                    unsafe
                    {
                        _hPowerNotify = UnsafeNativeMethods.RegisterPowerSettingNotification(_notificationHwnd.Handle, &monitorGuid, 0);
                    }
                }
            }

            /// <SecurityNode>
            ///     Critical: Calls critical code.
            ///     TreatAsSafe: Doesn't expose the critical resource.
            /// </SecurityNode>
            [SecurityCritical, SecurityTreatAsSafe]
            public void Dispose()
            {
                if (_hPowerNotify != IntPtr.Zero)
                {
                    UnsafeNativeMethods.UnregisterPowerSettingNotification(_hPowerNotify);
                    _hPowerNotify = IntPtr.Zero;
                }

                // Remove any attached event handlers.
                MonitorPowerEvent = null;

                _hwndTargetCount = 0;
                if (_notificationHwnd != null)
                {
                    _notificationHwnd.Dispose();
                    _notificationHwnd = null;
                }
            }

            /// <SecurityNode>
            ///     Critical: Calls critical code.
            ///     TreatAsSafe: Doesn't expose the critical resource.
            /// </SecurityNode>
            [SecurityCritical, SecurityTreatAsSafe]
            public void AttachHwndTarget(HwndTarget hwndTarget)
            {
                Debug.Assert(hwndTarget != null);
                MonitorPowerEvent += hwndTarget.OnMonitorPowerEvent;
                if (_hwndTargetCount > 0)
                {
                    // Every hwnd which registers to listen PBT_POWERSETTINGCHANGE
                    // gets the message atleast once so as to set the appropriate
                    // state. This call to the event handler simulates similar
                    // behavior. It is too early for the hwnd to paint, hence
                    // pass paintOnWake=false assuming that it will soon get
                    // a WM_PAINT message.
                    hwndTarget.OnMonitorPowerEvent(null, _monitorOn, /*paintOnWake*/ false);
                }
                _hwndTargetCount++;
            }

            /// <SecurityNode>
            ///     Critical: Calls critical code.
            ///     TreatAsSafe: Doesn't expose the critical resource.
            /// </SecurityNode>
            [SecurityCritical, SecurityTreatAsSafe]
            public bool DetachHwndTarget(HwndTarget hwndTarget)
            {
                Debug.Assert(hwndTarget != null);
                MonitorPowerEvent -= hwndTarget.OnMonitorPowerEvent;
                _hwndTargetCount--;
                Debug.Assert(_hwndTargetCount >= 0);
                return (_hwndTargetCount == 0);
            }

            /// <summary>
            ///     Handles the messages for the notification window
            /// </summary>
            /// <SecurityNode>
            ///     Critical: Elevates permissions via unsafe native methods
            /// </SecurityNode>
            [SecurityCritical]
            private IntPtr NotificationFilterMessage(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
            {
                IntPtr retInt = IntPtr.Zero;
                switch ((WindowMessage)msg)
                {
                    case WindowMessage.WM_POWERBROADCAST:
                        switch (NativeMethods.IntPtrToInt32(wParam))
                        {
                            case NativeMethods.PBT_POWERSETTINGCHANGE:
                                // PBT_POWERSETTINGCHANGE logic is implemented as a thread singleton
                                // instead of application singleton so as to avoid ---- between
                                // notification hwnd's PBT_POWERSETTINGCHANGE and other thread
                                // hwnd's WM_PAINT.
                                unsafe
                                {
                                    NativeMethods.POWERBROADCAST_SETTING* powerBroadcastSetting = (NativeMethods.POWERBROADCAST_SETTING*)lParam;
                                    if ((*powerBroadcastSetting).PowerSetting == NativeMethods.GUID_MONITOR_POWER_ON)
                                    {
                                        if ((*powerBroadcastSetting).Data == 0)
                                        {
                                            // Monitor is off
                                            _monitorOn = false;
                                        }
                                        else
                                        {
                                            // Monitor is on
                                            _monitorOn = true;
                                        }
                                        if (MonitorPowerEvent != null)
                                        {
                                            MonitorPowerEvent(null, new MonitorPowerEventArgs(_monitorOn));
                                        }
                                    }
                                }
                                break;
                        }
                        break;
                    default:
                        handled = false;
                        break;
                }
                return retInt;
            }
        }

        #endregion
    }
}

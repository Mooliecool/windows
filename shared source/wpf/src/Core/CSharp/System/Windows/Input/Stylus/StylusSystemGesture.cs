using System;

namespace System.Windows.Input
{
    /////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///		Supported System Gestures.
    ///
    ///  DoubleTap was removed since we no longer support it
    /// </summary>
    public enum SystemGesture
    {
        /// <summary></summary>
        None            = 0x00,
        /// <summary></summary>
        Tap             = 0x10,
        /// <summary></summary>
        RightTap        = 0x12,
        /// <summary></summary>
        Drag            = 0x13,
        /// <summary></summary>
        RightDrag       = 0x14,
        /// <summary></summary>
        HoldEnter       = 0x15,
        /// <summary></summary>
        HoldLeave       = 0x16,
        /// <summary></summary>
        HoverEnter      = 0x17,
        /// <summary></summary>
        HoverLeave      = 0x18,
        /// <summary></summary>
        Flick           = 0x1F,

        //
        // The following are Multi-Touch Static Gestures that are being implemented by WPF and not Wisptis.
        //

        /// <summary>
        ///     Two fingers tapped the screen at roughly the same time.
        /// </summary>
        TwoFingerTap    = 0x1100,

#if ROLLOVER_IMPLEMENTED
        /// <summary>
        ///     One finger touched the screen. Soon thereafter a second touched the screen.
        ///     A short time later they both lifted off the screen.
        /// </summary>
        Rollover        = 0x1200,
#endif
    };
}


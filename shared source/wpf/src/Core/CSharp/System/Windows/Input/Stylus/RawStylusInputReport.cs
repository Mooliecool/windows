using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Threading;
using System.Windows.Media;
using System.Windows.Input.StylusPlugIns;
using System.Security;
using System.Security.Permissions;
using MS.Internal.PresentationCore;                        // SecurityHelper
using MS.Internal;



using SR=MS.Internal.PresentationCore.SR;
using SRID=MS.Internal.PresentationCore.SRID;

namespace System.Windows.Input
{
    /////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     The RawStylusInputReport class encapsulates the raw input provided
    ///     from a stylus.
    /// </summary>
    /// <remarks>
    ///     It is important to note that the InputReport class only contains
    ///     blittable types.  This is required so that the report can be
    ///     marshalled across application domains.
    /// </remarks>
    internal class RawStylusInputReport : InputReport
    {

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Constructs an instance of the RawStylusInputReport class.
        /// </summary>
        /// <param name="mode">
        ///     The mode in which the input is being provided.
        /// </param>
        /// <param name="timestamp">
        ///     The time when the input occured.
        /// </param>
        /// <param name="inputSource">
        ///     The PresentationSource over which the stylus moved.
        /// </param>
        /// <param name="penContext">
        ///     The PenContext.
        /// </param>
        /// <param name="actions">
        ///     The set of actions being reported.
        /// </param>
        /// <param name="tabletDeviceId">
        ///     Tablet device id.
        /// </param>
        /// <param name="stylusDeviceId">
        ///     Stylus device id.
        /// </param>
        /// <param name="data">
        ///     Raw stylus data.
        /// </param>
        /// <SecurityNote>
        ///     Critical: This handles critical data in the form of PresentationSource.
        ///     TreatAsSafe:There are demands on the critical data(PresentationSource)
        /// </SecurityNote>
        [SecurityCritical,SecurityTreatAsSafe]
        internal RawStylusInputReport(
            InputMode mode,
            int timestamp,
            PresentationSource inputSource,
            PenContext penContext,
            RawStylusActions actions,
            int tabletDeviceId,
            int stylusDeviceId,
            int[] data)
            : base(inputSource, InputType.Stylus, mode, timestamp)
        {
            // Validate parameters
            if (!RawStylusActionsHelper.IsValid(actions))
            {
                throw new InvalidEnumArgumentException(SR.Get(SRID.Enum_Invalid, "actions"));
            }
            if (data == null && actions != RawStylusActions.InRange)
            {
                throw new ArgumentNullException("data");
            }

            _penContext     = new SecurityCriticalDataClass<PenContext>(penContext);
            _actions        = actions;
            _tabletDeviceId = tabletDeviceId;
            _stylusDeviceId = stylusDeviceId;
            _data           = data;
            _isSynchronize  = false;
        }

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Constructs an instance of the RawStylusInputReport class.
        /// </summary>
        /// <param name="mode">
        ///     The mode in which the input is being provided.
        /// </param>
        /// <param name="timestamp">
        ///     The time when the input occured.
        /// </param>
        /// <param name="inputSource">
        ///     The PresentationSource over which the stylus moved.
        /// </param>
        /// <param name="actions">
        ///     The set of actions being reported.
        /// </param>
        /// <param name="mousePointDescription">
        ///     Raw stylus data StylusPointDescription.
        /// </param>
        /// <param name="mouseData">
        ///     Raw stylus data.
        /// </param>
        /// <SecurityNote>
        ///     Critical: This handles critical data in the form of PresentationSource.
        ///     TreatAsSafe:There are demands on the critical data(PresentationSource)
        /// </SecurityNote>
        [SecurityCritical,SecurityTreatAsSafe]
        internal RawStylusInputReport(
            InputMode mode,
            int timestamp,
            PresentationSource inputSource,
            RawStylusActions actions,
            StylusPointDescription mousePointDescription,
            int[] mouseData)
            : base(inputSource, InputType.Stylus, mode, timestamp)
        {
            // Validate parameters
            if (!RawStylusActionsHelper.IsValid(actions))
            {
                throw new InvalidEnumArgumentException(SR.Get(SRID.Enum_Invalid, "actions"));
            }
            if (mouseData == null)
            {
                throw new ArgumentNullException("mouseData");
            }

            _penContext     = new SecurityCriticalDataClass<PenContext>(null);
            _actions        = actions;
            _tabletDeviceId = 0;
            _stylusDeviceId = 0;
            _data           = mouseData;
            _isSynchronize  = false;
            _mousePointDescription = mousePointDescription;
            _isMouseInput = true;
        }

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Read-only access to the set of actions that were reported.
        /// </summary>
        internal RawStylusActions Actions { get { return _actions; } }

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Read-only access to stylus context id that reported the data.
        /// </summary>
        internal int TabletDeviceId { get { return _tabletDeviceId; } }

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Read-only access to stylus context id that reported the data.
        /// </summary>
        /// <SecurityNote>
        ///     Critical: provides access to critical member _penContext
        /// </SecurityNote>        
        internal PenContext PenContext
        {
            [SecurityCritical]
            get { return _penContext.Value;  }
        }

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Read-only access to stylus context id that reported the data.
        /// </summary>
        /// <SecurityNote>
        ///     Critical: This handles critical data _penContext
        ///     TreatAsSafe: We're returning safe data
        /// </SecurityNote>
        internal StylusPointDescription StylusPointDescription
        {
            [SecurityCritical, SecurityTreatAsSafe]
            get { return _isMouseInput ? _mousePointDescription : _penContext.Value.StylusPointDescription; }
        }

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Read-only access to stylus device id that reported the data.
        /// </summary>
        internal int StylusDeviceId { get { return _stylusDeviceId; } }

        /////////////////////////////////////////////////////////////////////

        internal StylusDevice StylusDevice
        {
            get { return _stylusDevice; }
            set { _stylusDevice = value; }
        }

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        ///     Read-only access to the raw data that was reported.
        /// </summary>
        /// <SecurityNote>
        ///     Critical: Access the critical field - _data
        ///     TreatAsSafe: No input is taken. It's safe to a clone.
        /// </SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        internal int[] GetRawPacketData()
        {
            if (_data == null)
                return null;
            return (int[])_data.Clone();
        }

        /////////////////////////////////////////////////////////////////////
        /// <SecurityNote>
        ///     Critical: Access the critical field - _data
        ///     TreatAsSafe: No input is taken. It's safe to return the last tablet point.
        /// </SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        internal Point GetLastTabletPoint()
        {
            int packetLength = StylusPointDescription.GetInputArrayLengthPerPoint();
            int lastXIndex = _data.Length - packetLength;
            return new Point(_data[lastXIndex], _data[lastXIndex + 1]);
        }

        /////////////////////////////////////////////////////////////////////

        /// <SecurityNote>
        ///     Critical - Hands out ref to internal data that can be used to spoof input.
        /// </SecurityNote>
        internal int[] Data
        {
            [SecurityCritical]
            get { return _data; }
        }

        /////////////////////////////////////////////////////////////////////

        /// <SecurityNote>
        ///     Critical - Setting property can be used to spoof input.
        /// </SecurityNote>
        internal RawStylusInput RawStylusInput
        {
            get { return _rawStylusInput.Value;  }

            [SecurityCritical]
            set { _rawStylusInput.Value = value; }
        }

        /////////////////////////////////////////////////////////////////////

        internal bool Synchronized
        {
            get { return _isSynchronize;  }
            set { _isSynchronize = value; }
        }

        /////////////////////////////////////////////////////////////////////

        RawStylusActions                                _actions;
        int                                             _tabletDeviceId;
        /// <SecurityNote>
        ///     Critical: Marked critical to prevent inadvertant spread to transparent code
        /// </SecurityNote>
        private SecurityCriticalDataClass<PenContext>   _penContext;
        int                                             _stylusDeviceId;

        /// <SecurityNote>
        ///     Critical to prevent accidental spread to transparent code
        /// </SecurityNote>
        [SecurityCritical]
        int[]                                           _data;
        StylusDevice _stylusDevice; // cached value looked up from _stylusDeviceId
        SecurityCriticalDataForSet<RawStylusInput>      _rawStylusInput;
        bool                                            _isSynchronize; // Set from StylusDevice.Synchronize.
        bool                                            _isMouseInput; // Special case of mouse input being sent to plugins
        StylusPointDescription                          _mousePointDescription; // layout for mouse points
    }    
}

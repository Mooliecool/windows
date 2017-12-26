//---------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation.  All rights reserved.
//
//---------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Security;
using System.Security.Permissions;

using MS.Internal;


namespace System.Windows.Input
{
    /// <summary>
    ///     The implementation of a TouchDevice specific to StylusDevice.
    /// </summary>
    internal sealed class StylusTouchDevice : TouchDevice
    {
        #region TouchDevice Implementation

        /// <SecurityNote>
        ///     Critical: Accesses critical _stylusLogic and InputManager
        ///     TreatAsSafe: This constructor handles critical data but does not expose it
        /// </SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        internal StylusTouchDevice(StylusDevice stylusDevice)
            : base(stylusDevice.Id)
        {
            _stylusDevice = stylusDevice;
            _stylusLogic = InputManager.UnsecureCurrent.StylusLogic;
            PromotingToOther = true;
        }

        /// <summary>
        ///     Provides the current position.
        /// </summary>
        /// <param name="relativeTo">Defines the coordinate space.</param>
        /// <returns>The current position in the coordinate space of relativeTo.</returns>
        public override TouchPoint GetTouchPoint(IInputElement relativeTo)
        {
            Point position = _stylusDevice.GetPosition(relativeTo);

            Rect rectBounds = GetBounds(_stylusDevice.RawStylusPoint, position, relativeTo);

            return new TouchPoint(this, position, rectBounds, _lastAction);
        }

        private Rect GetBounds(StylusPoint stylusPoint, Point position, IInputElement relativeTo)
        {
            GeneralTransform elementToRoot;
            GeneralTransform rootToElement;
            GetRootTransforms(relativeTo, out elementToRoot, out rootToElement);
            return GetBounds(stylusPoint, position, relativeTo, elementToRoot, rootToElement);
        }

        private Rect GetBounds(StylusPoint stylusPoint,
            Point position,
            IInputElement relativeTo,
            GeneralTransform elementToRoot,
            GeneralTransform rootToElement)
        {
            // Get width and heith in pixel value
            double width = GetStylusPointWidthOrHeight(stylusPoint, /*isWidth*/ true);
            double height = GetStylusPointWidthOrHeight(stylusPoint, /*isWidth*/ false);

            // Get the position with respect to root
            Point rootPoint;
            if (elementToRoot == null ||
                !elementToRoot.TryTransform(position, out rootPoint))
            {
                rootPoint = position;
            }

            // Create a Rect with respect to root and transform it to element coordinate space
            Rect rectBounds = new Rect(rootPoint.X - width * 0.5, rootPoint.Y - height * 0.5, width, height);
            if (rootToElement != null)
            {
                rectBounds = rootToElement.TransformBounds(rectBounds);
            }
            return rectBounds;
        }

        private double GetStylusPointWidthOrHeight(StylusPoint stylusPoint, bool isWidth)
        {
            const double CentimeterPerInch = 2.54d;
            const double PixelPerInch = 96d;

            StylusPointProperty property = (isWidth ? StylusPointProperties.Width : StylusPointProperties.Height);
            double value = 0d;
            if (stylusPoint.HasProperty(property))
            {
                // Get the property value in the corresponding units
                value = (double)stylusPoint.GetPropertyValue(property);
                StylusPointPropertyInfo propertyInfo = stylusPoint.Description.GetPropertyInfo(property);
                if (!DoubleUtil.AreClose(propertyInfo.Resolution, 0d))
                {
                    value /= propertyInfo.Resolution;
                }
                else
                {
                    value = 0;
                }

                // Convert the value to Inches
                if (propertyInfo.Unit == StylusPointPropertyUnit.Centimeters)
                {
                    value /= CentimeterPerInch;
                }

                // Convert the value to pixels
                value *= PixelPerInch;
            }
            return value;
        }

        /// <summary>
        ///     Provides all of the known points the device hit since the last reported position update.
        /// </summary>
        /// <param name="relativeTo">Defines the coordinate space.</param>
        /// <returns>A list of points in the coordinate space of relativeTo.</returns>
        public override TouchPointCollection GetIntermediateTouchPoints(IInputElement relativeTo)
        {
            // Retrieve the stylus points
            StylusPointCollection stylusPoints = _stylusDevice.GetStylusPoints(relativeTo, _stylusPointDescription);
            int count = stylusPoints.Count;
            TouchPointCollection touchPoints = new TouchPointCollection();

            GeneralTransform elementToRoot;
            GeneralTransform rootToElement;
            GetRootTransforms(relativeTo, out elementToRoot, out rootToElement);

            // Convert the stylus points into touch points
            for (int i = 0; i < count; i++)
            {
                StylusPoint stylusPoint = stylusPoints[i];
                Point position = new Point(stylusPoint.X, stylusPoint.Y);
                Rect rectBounds = GetBounds(stylusPoint, position, relativeTo, elementToRoot, rootToElement);

                TouchPoint touchPoint = new TouchPoint(this, position, rectBounds, _lastAction);
                touchPoints.Add(touchPoint);
            }

            return touchPoints;
        }

        /// <SecurityNote>
        ///     Critical - Calls PresentationSource.CriticalFromVisual.
        ///     TreatAsSafe - Does not expose PresentationSource.
        /// </SecurityNote>
        [SecurityCritical, SecurityTreatAsSafe]
        private void GetRootTransforms(IInputElement relativeTo, out GeneralTransform elementToRoot, out GeneralTransform rootToElement)
        {
            elementToRoot = rootToElement = null;

            DependencyObject containingVisual = InputElement.GetContainingVisual(relativeTo as DependencyObject);
            if (containingVisual != null)
            {
                PresentationSource relativePresentationSource = PresentationSource.CriticalFromVisual(containingVisual);
                Visual rootVisual = (relativePresentationSource != null) ? relativePresentationSource.RootVisual : null;
                Visual containingVisual2D = VisualTreeHelper.GetContainingVisual2D(containingVisual);
                if ((rootVisual != null) && (containingVisual2D != null))
                {
                    elementToRoot = containingVisual2D.TransformToAncestor(rootVisual);
                    rootToElement = rootVisual.TransformToDescendant(containingVisual2D);
                }
            }
        }

        #endregion

        #region StylusLogic Related

        protected override void OnManipulationStarted()
        {
            base.OnManipulationStarted();
            PromotingToOther = false;
        }

        /// <SecurityNote>
        ///     Critical: Accesses _stylusLogic and _storedStagingAreaItems
        ///                and calls StylusLogic.PromoteStoredItemsToMouse
        ///     PublicOK: This method has a demand on it.
        ///     Demand:   Technically the demand is not needed because the
        ///               user can already do this indirectly by canceling the
        ///               manipulation. But the decision is to limit the scope
        ///               of this raw method to full trust.
        /// </SecurityNote>
        [SecurityCritical, UIPermissionAttribute(SecurityAction.LinkDemand, Unrestricted = true)]
        protected override void OnManipulationEnded(bool cancel)
        {
            base.OnManipulationEnded(cancel);
            if (cancel)
            {
                // Set PromotingToOther to true because the
                // the manipulation has been canceled and hence
                // we want the promotions to happen.
                PromotingToOther = true;
                _stylusLogic.PromoteStoredItemsToMouse(this);
            }
            else
            {
                // Set PromotingToOther to false because we
                // dont want any mouse promotions to happen for
                // rest of this touch cycle.
                PromotingToOther = false;
            }
            if (_storedStagingAreaItems != null)
            {
                _storedStagingAreaItems.Clear();
            }
        }

        /// <summary>
        ///     Whether input associated with this device is being promoted to some other device.
        /// </summary>
        internal bool PromotingToOther
        {
            get;
            private set;
        }

        /// <summary>
        ///     Stored items that occurred during the time before we knew where to promote to.
        /// </summary>
        /// <SecurityNote>
        ///     Critical: The items stored in this collection will be promoted to mouse.
        /// </SecurityNote>
        internal StylusLogic.StagingAreaInputItemList StoredStagingAreaItems
        {
            [SecurityCritical]
            get
            {
                if (_storedStagingAreaItems == null)
                {
                    _storedStagingAreaItems = new StylusLogic.StagingAreaInputItemList();
                }
                return _storedStagingAreaItems;
            }
        }

        /// <summary>
        ///     The associated StylusDevice
        /// </summary>
        internal StylusDevice StylusDevice
        {
            get
            {
                return _stylusDevice;
            }
        }

        /// <SecurityNote>
        ///     Critical - Setting the ActiveSource on the device.
        /// </SecurityNote>
        [SecurityCritical]
        internal void ChangeActiveSource(PresentationSource activeSource)
        {
            SetActiveSource(activeSource);
        }

        /// <SecurityNote>
        ///     Critical: Accesses _stylusLogic. Could be TreatAsSafe
        ///               because doesn't expose _stylusLogic itself.
        /// </SecurityNote>
        [SecurityCritical]
        internal void OnActivate()
        {
            Activate();
            _activeDeviceCount++;
            if (_activeDeviceCount == 1)
            {
                _isPrimary = true;
                _stylusLogic.CurrentMousePromotionStylusDevice = _stylusDevice;
            }
            PromotingToOther = true;
        }

        /// <SecurityNote>
        ///     Critical: Accesses _stylusLogic and _storedStagingAreaItems.
        ///               Clears _storedStagingAreaItems collection which
        ///               stores the staging items to be promoted to mouse.
        ///               Could be TreatAsSafe because doesn't expose the
        ///               objects themselves.
        /// </SecurityNote>
        [SecurityCritical]
        internal void OnDeactivate()
        {
            Deactivate();
            if (_storedStagingAreaItems != null)
            {
                _storedStagingAreaItems.Clear();
            }
            PromotingToOther = true;
            DownHandled = false;
            _activeDeviceCount--;
            if (_activeDeviceCount == 0)
            {
                _stylusLogic.CurrentMousePromotionStylusDevice = null;
            }
            else if (_isPrimary)
            {
                _stylusLogic.CurrentMousePromotionStylusDevice = NoMousePromotionStylusDevice;
            }
            _isPrimary = false;
        }

        internal bool OnDown()
        {
            _lastAction = TouchAction.Down;
            DownHandled = ReportDown();
            return DownHandled;
        }

        internal bool OnMove()
        {
            _lastAction = TouchAction.Move;
            return ReportMove();
        }

        internal bool OnUp()
        {
            _lastAction = TouchAction.Up;
            return ReportUp();
        }

        /// <summary>
        ///     Bool indicates if down event was handled
        /// </summary>
        internal bool DownHandled
        {
            get;
            private set;
        }
        #endregion

        #region Data

        private StylusDevice _stylusDevice;

        /// <SecurityNote>
        ///     Critical: To prevent accidental spread to transparent code
        /// </SecurityNote>
        [SecurityCritical]
        private StylusLogic _stylusLogic;

        /// <SecurityNote>
        ///     Critical: Holds the staging items to be promoted to mouse.
        /// </SecurityNote>
        [SecurityCritical]
        private StylusLogic.StagingAreaInputItemList _storedStagingAreaItems;

        private TouchAction _lastAction = TouchAction.Move;
        private bool _isPrimary = false;

        [ThreadStatic]
        private static int _activeDeviceCount = 0;

        private static readonly StylusPointPropertyInfo[] _stylusPropertyInfos =
            new StylusPointPropertyInfo[]
            {
                StylusPointPropertyInfoDefaults.X,
                StylusPointPropertyInfoDefaults.Y,
                StylusPointPropertyInfoDefaults.NormalPressure,
                StylusPointPropertyInfoDefaults.Width,
                StylusPointPropertyInfoDefaults.Height
            };

        private static readonly StylusPointDescription _stylusPointDescription = new StylusPointDescription(_stylusPropertyInfos);

        private static object NoMousePromotionStylusDevice = new object();

        #endregion
    }
}

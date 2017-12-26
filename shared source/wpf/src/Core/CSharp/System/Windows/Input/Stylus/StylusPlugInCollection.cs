//-----------------------------------------------------------------------
// <copyright file="StylusPlugInCollection.cs" company="Microsoft">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

using System;
using System.Windows;
using System.Collections;
using System.Collections.ObjectModel;
using System.Windows.Media;
using System.Windows.Threading;
using System.Windows.Interop;
using System.Security;
using System.Security.Permissions;

using SR=MS.Internal.PresentationCore.SR;
using SRID=MS.Internal.PresentationCore.SRID;

namespace System.Windows.Input.StylusPlugIns
{
    /// <summary>
    /// Collection of StylusPlugIn objects
    /// </summary>
    /// <remarks>
    /// The collection order is based on the order that StylusPlugIn objects are
    /// added to the collection via the IList interfaces. The order of the StylusPlugIn
    /// objects in the collection is modifiable.
    /// Some of the methods are designed to be called from both the App thread and the Pen thread,
    /// but some of them are supposed to be called from one thread only. Please look at the 
    /// comments of each method for such an information.
    /// </remarks>
    public sealed class StylusPlugInCollection : Collection<StylusPlugIn>
    {
        #region Protected APIs
        /// <summary>
        /// Insert a StylusPlugIn in the collection at a specific index. 
        /// This method should be called from the application context only
        /// </summary>
        /// <param name="index">index at which to insert the StylusPlugIn object</param>
        /// <param name="plugIn">StylusPlugIn object to insert, downcast to an object</param>
        protected override void InsertItem(int index, StylusPlugIn plugIn)
        {
            // Verify it's called from the app dispatcher
            _element.VerifyAccess();

            // Validate the input parameter
            if (null == plugIn)
            {
                throw new ArgumentNullException("plugIn", SR.Get(SRID.Stylus_PlugInIsNull));
            }

            if (IndexOf(plugIn) != -1)
            {
                throw new ArgumentException(SR.Get(SRID.Stylus_PlugInIsDuplicated), "plugIn");
            }
            
            // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
            // which a call to Lock() can cause.
            using (_element.Dispatcher.DisableProcessing())
            {
                if (IsActiveForInput)
                {
                    // If we are currently active for input then we have a _penContexts that we must lock!
                    lock(PenContextsSyncRoot)
                    {
                        System.Diagnostics.Debug.Assert(this.Count > 0); // If active must have more than one plugin already
                        base.InsertItem(index, plugIn);
                        plugIn.Added(this);
                    }
                }
                else
                {
                    EnsureEventsHooked(); // Hook up events to track changes to the plugin's element
                    base.InsertItem(index, plugIn);
                    try
                    {
                        plugIn.Added(this); // Notify plugin that it has been added to collection
                    }
                    finally
                    {
                        UpdatePenContextsState(); // Add to PenContexts if element is in proper state (can fire isactiveforinput).
                    }
                }
            }
        }

        /// <summary>
        /// Remove all the StylusPlugIn objects from the collection.
        /// This method should be called from the application context only.
        /// </summary>
        protected override void ClearItems()
        {
            // Verify it's called from the app dispatcher
            _element.VerifyAccess();

            if (this.Count != 0)
            {
                // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
                // which a call to Lock() can cause.
                using (_element.Dispatcher.DisableProcessing())
                {
                    if (IsActiveForInput)
                    {
                        // If we are currently active for input then we have a _penContexts that we must lock!
                        lock(PenContextsSyncRoot)
                        {
                            while (this.Count > 0)
                            {
                                RemoveItem(0);  // Does work to fire event and remove from collection and pencontexts
                            }
                        }
                    }
                    else
                    {
                        while (this.Count > 0)
                        {
                            RemoveItem(0);  // Does work to fire event and remove from collection.
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Remove the StylusPlugIn in the collection at the specified index.         
        /// This method should be called from the application context only.
        /// </summary>
        /// <param name="index"></param>
        protected override void RemoveItem(int index)
        {
            // Verify it's called from the app dispatcher
            _element.VerifyAccess();

            // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
            // which a call to Lock() can cause.
            using (_element.Dispatcher.DisableProcessing())
            {
                if (IsActiveForInput)
                {
                    // If we are currently active for input then we have a _penContexts that we must lock!
                    lock(PenContextsSyncRoot)
                    {
                        StylusPlugIn removedItem = base[index];
                        base.RemoveItem(index);
                        try
                        {
                            EnsureEventsAndPenContextsUnhooked(); // Clean up events and remove from pencontexts
                        }
                        finally
                        {
                            removedItem.Removed(); // Notify plugin it has been removed
                        }
                    }
                }
                else
                {
                    StylusPlugIn removedItem = base[index];
                    base.RemoveItem(index);
                    try
                    {
                        EnsureEventsAndPenContextsUnhooked(); // Clean up events and remove from pencontexts
                    }
                    finally
                    {
                        removedItem.Removed(); // Notify plugin it has been removed
                    }
                }
            }
        }

        /// <summary>
        /// Indexer to retrieve/set a StylusPlugIn at a given index in the collection
        /// Accessible from both the real time context and application context.
        /// </summary>
        protected override void SetItem(int index, StylusPlugIn plugIn)
        {
            // Verify it's called from the app dispatcher
            _element.VerifyAccess();

            if (null == plugIn)
            {
                throw new ArgumentNullException("plugIn", SR.Get(SRID.Stylus_PlugInIsNull));
            }

            if (IndexOf(plugIn) != -1)
            {
                throw new ArgumentException(SR.Get(SRID.Stylus_PlugInIsDuplicated), "plugIn");
            }

            // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
            // which a call to Lock() can cause.
            using (_element.Dispatcher.DisableProcessing())
            {
                if (IsActiveForInput)
                {
                    // If we are currently active for input then we have a _penContexts that we must lock!
                    lock(PenContextsSyncRoot)
                    {
                        StylusPlugIn originalPlugIn = base[index];
                        base.SetItem(index, plugIn);
                        try
                        {
                            originalPlugIn.Removed();
                        }
                        finally
                        {
                            plugIn.Added(this);
                        }
                    }
                }
                else
                {
                    StylusPlugIn originalPlugIn = base[index];
                    base.SetItem(index, plugIn);
                    try
                    {
                        originalPlugIn.Removed();
                    }
                    finally
                    {
                        plugIn.Added(this);
                    }
                }
            }
        }

        #endregion

        #region Internal APIs
        
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="element"></param>
        internal StylusPlugInCollection(UIElement element)
        {
            _element = element;

            _isEnabledChangedEventHandler = new DependencyPropertyChangedEventHandler(OnIsEnabledChanged);
            _isVisibleChangedEventHandler = new DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
            _isHitTestVisibleChangedEventHandler = new DependencyPropertyChangedEventHandler(OnIsHitTestVisibleChanged);
            _sourceChangedEventHandler = new SourceChangedEventHandler(OnSourceChanged);
            _layoutChangedEventHandler = new EventHandler(OnLayoutUpdated);
        }

        /// <summary>
        /// Get the UIElement
        /// This method is called from the real-time context.
        /// </summary>
        internal UIElement Element
        {
            get
            {
                return _element;
            }
        }

        /// <summary>
        /// Update the rectangular bound of the element
        /// This method is called from the application context.
        /// </summary>
        internal void UpdateRect()
        {
            // The RenderSize is only valid if IsArrangeValid is true.
            if (_element.IsArrangeValid && _element.IsEnabled && _element.IsVisible && _element.IsHitTestVisible)
            {
                _rc = new Rect(new Point(), _element.RenderSize);// _element.GetContentBoundingBox();
                Visual root = VisualTreeHelper.GetContainingVisual2D(InputElement.GetRootVisual(_element));

                try
                {
                    _viewToElement = root.TransformToDescendant(_element);
                }
                catch(System.InvalidOperationException)
                {
                    // This gets hit if the transform is not invertable.  In that case
                    // we will just not allow this plugin to be hit.
                    _rc = new Rect(); // empty rect so we don't hittest it.
                    _viewToElement = Transform.Identity;
                }
            }
            else
            {
                _rc = new Rect(); // empty rect so we don't hittest it.
            }
            if (_viewToElement == null)
            {
                _viewToElement = Transform.Identity;
            }
        }

        /// <summary>
        /// Check whether a point hits the element
        /// This method is called from the real-time context.
        /// </summary>
        /// <param name="pt">a point to check</param>
        /// <returns>true if the point is within the bound of the element; false otherwise</returns>
        internal bool IsHit(Point pt)
        {
            Point ptElement = pt;
            _viewToElement.TryTransform(ptElement, out ptElement);
            return _rc.Contains(ptElement);
        }

        /// <summary>
        /// Get the transform matrix from the root visual to the current UIElement
        /// This method is called from the real-time context.
        /// </summary>
        internal GeneralTransform ViewToElement
        {
            get
            {
                return _viewToElement;
            }
        }

        /// <summary>
        /// Get the current rect for the Element that the StylusPlugInCollection is attached to.
        /// May be empty rect if plug in is not in tree.
        /// </summary>
        internal Rect Rect
        {
            get
            {
                return _rc;
            }
        }

        /// <summary>
        /// Get the current rect for the Element that the StylusPlugInCollection is attached to.
        /// May be empty rect if plug in is not in tree.
        /// </summary>
        /// <SecurityNote>
        /// Critical - Accesses SecurityCritical data _penContexts.
        /// TreatAsSafe - Just returns if _pencontexts is null.  No data goes in or out.  Knowing
        ///               the fact that you can recieve real time input is something that is safe
        ///               to know and we want to expose.
        /// </SecurityNote>
        internal bool IsActiveForInput
        {
            [SecurityCritical, SecurityTreatAsSafe]
            get
            {
                return _penContexts != null;
            }
        }


        /// <SecurityNote>
        /// Critical - Accesses SecurityCritical data _penContexts.
        /// TreatAsSafe - The [....] object on the _penContexts object is not considered security 
        ///                 critical data.  It is already internally exposed directly on the
        ///                 PenContexts object.
        /// </SecurityNote>
        internal object PenContextsSyncRoot 
        {
            [SecurityCritical, SecurityTreatAsSafe]
            get
            {
                return _penContexts != null ? _penContexts.SyncRoot : null;
            }
        } 


        /// <summary>
        /// Fire the Enter notification.
        /// This method is called from pen threads and app thread.
        /// </summary>
        internal void FireEnterLeave(bool isEnter, RawStylusInput rawStylusInput, bool confirmed)
        {
            if (IsActiveForInput)
            {
                // If we are currently active for input then we have a _penContexts that we must lock!
                lock(PenContextsSyncRoot)
                {
                    for (int i = 0; i < this.Count; i++)
                    {
                        base[i].StylusEnterLeave(isEnter, rawStylusInput, confirmed);
                    }
                }
            }
            else
            {
                for (int i = 0; i < this.Count; i++)
                {
                    base[i].StylusEnterLeave(isEnter, rawStylusInput, confirmed);
                }
            }
        }

        /// <summary>
        /// Fire RawStylusInputEvent for all the StylusPlugIns
        /// This method is called from the real-time context (pen thread) only
        /// </summary>
        /// <param name="args"></param>
        internal void FireRawStylusInput(RawStylusInput args)
        {
            try
            {
                if (IsActiveForInput)
                {
                    // If we are currently active for input then we have a _penContexts that we must lock!
                    lock(PenContextsSyncRoot)
                    {
                        for (int i = 0; i < this.Count; i++)
                        {
                            StylusPlugIn plugIn = base[i];
                            // set current plugin so any callback data gets an owner.
                            args.CurrentNotifyPlugIn = plugIn;
                            plugIn.RawStylusInput(args);
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < this.Count; i++)
                    {
                        StylusPlugIn plugIn = base[i];
                        // set current plugin so any callback data gets an owner.
                        args.CurrentNotifyPlugIn = plugIn;
                        plugIn.RawStylusInput(args);
                    }
                }
            }
            finally
            {
                args.CurrentNotifyPlugIn = null;
            }
        }


        /// <SecurityNote>
        ///     Critical:  Accesses critical member _penContexts.
        /// </SecurityNote>
        internal PenContexts PenContexts
        {
            [SecurityCritical]
            get
            {
                return _penContexts;
            }
        }

        #endregion

        #region Private APIs

        /// <summary>
        /// Add this StylusPlugInCollection to the StylusPlugInCollectionList when it the first 
        /// element is added.
        /// </summary>
        /// <SecurityNote>
        /// Critical - Presentation source access
        /// TreatAsSafe: - PresentationSource makes a SecurityDemand
        ///                    - no data handed out or accepted
        ///                    - called by Add and Insert
        /// </SecurityNote>
        [SecurityCritical,SecurityTreatAsSafe]
        private void EnsureEventsHooked()
        {
            if (this.Count == 0)
            {
                // Grab current element info
                UpdateRect();
                // Now hook up events to track on this element.
                _element.IsEnabledChanged += _isEnabledChangedEventHandler;
                _element.IsVisibleChanged += _isVisibleChangedEventHandler;
                _element.IsHitTestVisibleChanged += _isHitTestVisibleChangedEventHandler;
                PresentationSource.AddSourceChangedHandler(_element, _sourceChangedEventHandler);  // has a security linkdemand
                _element.LayoutUpdated += _layoutChangedEventHandler;

                if (_element.RenderTransform != null &&
                    !_element.RenderTransform.IsFrozen)
                {
                    if (_renderTransformChangedEventHandler == null)
                    {
                        _renderTransformChangedEventHandler = new EventHandler(OnRenderTransformChanged);
                        _element.RenderTransform.Changed  += _renderTransformChangedEventHandler;
                    }
                }
            }
        }

        /// <summary>
        /// Remove this StylusPlugInCollection from the StylusPlugInCollectionList when it the last 
        /// element is removed for this collection.
        /// </summary>
        private void EnsureEventsAndPenContextsUnhooked()
        {
            if (this.Count == 0)
            {
                // Unhook events.
                _element.IsEnabledChanged -= _isEnabledChangedEventHandler;
                _element.IsVisibleChanged -= _isVisibleChangedEventHandler;
                _element.IsHitTestVisibleChanged -= _isHitTestVisibleChangedEventHandler;
                if (_renderTransformChangedEventHandler != null)
                {
                    _element.RenderTransform.Changed  -= _renderTransformChangedEventHandler;
                }
                PresentationSource.RemoveSourceChangedHandler(_element, _sourceChangedEventHandler);
                _element.LayoutUpdated -= _layoutChangedEventHandler;
     
                // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
                // which a call to Lock() can cause.
                using (_element.Dispatcher.DisableProcessing())
                {
                    // Make sure we are unhooked from PenContexts if we don't have any plugins.
                    UnhookPenContexts();
                }
            }
        }
        
        private void OnIsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            System.Diagnostics.Debug.Assert(_element.IsEnabled == (bool)e.NewValue);
            UpdatePenContextsState();
        }

        private void OnIsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            System.Diagnostics.Debug.Assert(_element.IsVisible == (bool)e.NewValue);
            UpdatePenContextsState();
        }

        private void OnIsHitTestVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            System.Diagnostics.Debug.Assert(_element.IsHitTestVisible == (bool)e.NewValue);
            UpdatePenContextsState();
        }

        private void OnRenderTransformChanged(object sender, EventArgs e)
        {
            OnLayoutUpdated(sender, e);
        }

        private void OnSourceChanged(object sender, SourceChangedEventArgs e)
        {
            // This means that the element has been added or remvoed from its source.
            UpdatePenContextsState();
        }

        private void OnLayoutUpdated(object sender, EventArgs e)
        {
            // Make sure our rect and transform is up to date on layout changes.
            
            // NOTE: We need to make sure we do this under a lock if we are active for input since we don't
            // want the PenContexts code to get a mismatched set of state for this element.
            if (IsActiveForInput)
            {
                // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
                // which a call to Lock() can cause.
                using (_element.Dispatcher.DisableProcessing())
                {
                    // If we are currently active for input then we have a _penContexts that we must lock!
                    lock(PenContextsSyncRoot)
                    {
                        UpdateRect();
                    }
                }
            }
            else
            {
                UpdateRect();
            }

            if (_lastRenderTransform != _element.RenderTransform)
            {
                if (_renderTransformChangedEventHandler != null)
                {
                    _lastRenderTransform.Changed -= _renderTransformChangedEventHandler;
                    _renderTransformChangedEventHandler = null;
                }
                
                _lastRenderTransform = _element.RenderTransform;
            }

            if (_lastRenderTransform != null)
            {
                if (_lastRenderTransform.IsFrozen)
                {
                    if (_renderTransformChangedEventHandler != null)
                    {
                        _renderTransformChangedEventHandler = null;
                    }
                }
                else
                {
                    if (_renderTransformChangedEventHandler == null)
                    {
                        _renderTransformChangedEventHandler = new EventHandler(OnRenderTransformChanged);
                        _lastRenderTransform.Changed += _renderTransformChangedEventHandler;
                    }
                }
            }
        }

        // On app ui dispatcher
        /// <SecurityNote>
        /// Critical - Presentation source access
        ///            Calls SecurityCritical routines PresentationSource.CriticalFromVisual and
        ///            HwndSource.CriticalHandle.
        /// TreatAsSafe: 
        ///          - no data handed out or accepted
        /// </SecurityNote>
        [SecurityCritical,SecurityTreatAsSafe]
        private void UpdatePenContextsState()
        {
            bool unhookPenContexts = true;
            
            // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
            // which a call to Lock() can cause.
            using (_element.Dispatcher.DisableProcessing())
            {
                // See if we should be enabled
                if (_element.IsVisible && _element.IsEnabled && _element.IsHitTestVisible)
                {
                    PresentationSource presentationSource = PresentationSource.CriticalFromVisual(_element as Visual);
                  
                    if (presentationSource != null)
                    {
                        unhookPenContexts = false;
                        
                        // Are we currently hooked up?  If not then hook up.
                        if (_penContexts == null)
                        {
                            InputManager inputManager = (InputManager)_element.Dispatcher.InputManager;
                            PenContexts penContexts = inputManager.StylusLogic.GetPenContextsFromHwnd(presentationSource);

                            // _penContexts must be non null or don't do anything.
                            if (penContexts != null)
                            {
                                _penContexts = penContexts;

                                lock(penContexts.SyncRoot)
                                {
                                    penContexts.AddStylusPlugInCollection(this);
                                        
                                    foreach (StylusPlugIn spi in this)
                                    {
                                        spi.InvalidateIsActiveForInput(); // Uses _penContexts being set to determine active state.

                                    }
                                    // NTRAID:WINDOWSOS#1677277-2006/06/05-WAYNEZEN,
                                    // Normally the Rect will be updated when we receive the LayoutUpdate. 
                                    // However there could be a race condition which the LayoutUpdate gets received 
                                    // before the properties like IsVisible being set.
                                    // So we should always force to call OnLayoutUpdated whenever the input is active.
                                    OnLayoutUpdated(this, EventArgs.Empty);
                                }

                            }
                        }
                    }
                }
                
                if (unhookPenContexts)
                {
                    UnhookPenContexts();
                }
            }
        }

        /// <SecurityNote>
        /// Critical - _penContexts access
        /// TreatAsSafe: 
        ///          - no data handed out or accepted
        /// </SecurityNote>
        [SecurityCritical,SecurityTreatAsSafe]
        void UnhookPenContexts()
        {
            // Are we currently unhooked?  If not then unhook.
            if (_penContexts != null)
            {
                lock(_penContexts.SyncRoot)
                {
                    _penContexts.RemoveStylusPlugInCollection(this);
                    // Can't recieve any more input now!
                    _penContexts = null;

                    // Notify after input is disabled to the PlugIns.
                    foreach (StylusPlugIn spi in this)
                    {
                        spi.InvalidateIsActiveForInput();
                    }
                }
            }
        }

        #endregion

        #region Fields
        private UIElement _element;
        private Rect _rc; // In window root measured units
        private GeneralTransform _viewToElement;

        private Transform _lastRenderTransform;

        // Note that this is only set when the Element is in a state to receive input (visible,enabled,in tree).
        /// <SecurityNote>
        ///     Critical to prevent accidental spread to transparent code
        /// </SecurityNote>
        [SecurityCritical]
        private PenContexts _penContexts;

        private DependencyPropertyChangedEventHandler _isEnabledChangedEventHandler;
        private DependencyPropertyChangedEventHandler _isVisibleChangedEventHandler;
        private DependencyPropertyChangedEventHandler _isHitTestVisibleChangedEventHandler;
        private EventHandler _renderTransformChangedEventHandler;
        private SourceChangedEventHandler _sourceChangedEventHandler;
        private EventHandler _layoutChangedEventHandler;
        #endregion
    }
}

//---------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation.  All rights reserved.
//
//---------------------------------------------------------------------------

//#define Profiling

using MS.Internal;
using MS.Internal.Controls;
using MS.Utility;

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Controls.Primitives;
using System.Windows.Media;
using System.Windows.Threading;
using System.Windows.Input;
using System.Windows.Data;
using MS.Internal.Data;

namespace System.Windows.Controls
{
    /// <summary>
    /// VirtualizingStackPanel is used to arrange children into single line.
    /// </summary>
    public class VirtualizingStackPanel : VirtualizingPanel, IScrollInfo, IStackMeasure
    {
        //-------------------------------------------------------------------
        //
        //  Constructors
        //
        //-------------------------------------------------------------------

        #region Constructors

        /// <summary>
        /// Default constructor.
        /// </summary>
        public VirtualizingStackPanel()
        {
            this.IsVisibleChanged += new DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
        }

        //
        // DependencyProperty used by ItemValueStorage to store the PixelSize or LogicalSize of a
        // UIElement when it is a virtualized container. Used by TreeView and TreeViewItem
        // and ItemsControl to remember the size of TreeViewItems and GroupItems when
        // they get virtualized away.
        //
        private static readonly DependencyProperty ContainerSizeProperty = DependencyProperty.Register("ContainerSize", typeof(Size), typeof(VirtualizingStackPanel));

        //
        // DependencyProperty used by ItemValueStorage to store the flag that says if the containers
        // of this panel are all uniformly sized.
        //
        private static readonly DependencyProperty AreContainersUniformlySizedProperty = DependencyProperty.Register("AreContainersUniformlySized", typeof(bool), typeof(VirtualizingStackPanel));

        //
        // DependencyProperty used by ItemValueStorage to store the uniform size of the containers
        // of this panel if they are indeed uniformly sized. If they aren't uniformly sized then
        // this index is used to store the average of the realized container sizes in this panel
        // as a way of approximating the sizes of other containers of this panel that haven't
        // been realized yet.
        //
        private static readonly DependencyProperty UniformOrAverageContainerSizeProperty = DependencyProperty.Register("UniformOrAverageContainerSize", typeof(double), typeof(VirtualizingStackPanel));

        static VirtualizingStackPanel()
        {
            lock (DependencyProperty.Synchronized)
            {
                _indicesStoredInItemValueStorage = new int[] {ContainerSizeProperty.GlobalIndex, AreContainersUniformlySizedProperty.GlobalIndex, UniformOrAverageContainerSizeProperty.GlobalIndex};
            }
        }

        #endregion Constructors

        //-------------------------------------------------------------------
        //
        //  Public Methods
        //
        //-------------------------------------------------------------------

        #region Public Methods

        //-----------------------------------------------------------
        //  IScrollInfo Methods
        //-----------------------------------------------------------
        #region IScrollInfo Methods

        /// <summary>
        ///     Scroll content by one line to the top.
        ///     Subclases can override this method and call SetVerticalOffset to change
        ///     the behavior of what "line" means.
        /// </summary>
        public virtual void LineUp()
        {
            SetVerticalOffsetImpl(VerticalOffset - ((Orientation == Orientation.Vertical && !IsPixelBased) ? 1.0 : ScrollViewer._scrollLineDelta));
            SetAnchorInformation(false /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one line to the bottom.
        ///     Subclases can override this method and call SetVerticalOffset to change
        ///     the behavior of what "line" means.
        /// </summary>
        public virtual void LineDown()
        {
            SetVerticalOffsetImpl(VerticalOffset + ((Orientation == Orientation.Vertical && !IsPixelBased) ? 1.0 : ScrollViewer._scrollLineDelta));
            SetAnchorInformation(false /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one line to the left.
        ///     Subclases can override this method and call SetHorizontalOffset to change
        ///     the behavior of what "line" means.
        /// </summary>
        public virtual void LineLeft()
        {
            SetHorizontalOffsetImpl(HorizontalOffset - ((Orientation == Orientation.Horizontal && !IsPixelBased) ? 1.0 : ScrollViewer._scrollLineDelta));
            SetAnchorInformation(true /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one line to the right.
        ///     Subclases can override this method and call SetHorizontalOffset to change
        ///     the behavior of what "line" means.
        /// </summary>
        public virtual void LineRight()
        {
            SetHorizontalOffsetImpl(HorizontalOffset + ((Orientation == Orientation.Horizontal && !IsPixelBased) ? 1.0 : ScrollViewer._scrollLineDelta));
            SetAnchorInformation(true /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one page to the top.
        ///     Subclases can override this method and call SetVerticalOffset to change
        ///     the behavior of what "page" means.
        /// </summary>
        public virtual void PageUp()
        {
            double delta = ViewportHeight;
            if (!IsPixelBased && (Orientation == Orientation.Vertical) && DoubleUtil.IsZero(delta))
            {
                delta = 1;
            }
            SetVerticalOffsetImpl(VerticalOffset - delta);
            SetAnchorInformation(false /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one page to the bottom.
        ///     Subclases can override this method and call SetVerticalOffset to change
        ///     the behavior of what "page" means.
        /// </summary>
        public virtual void PageDown()
        {
            double delta = ViewportHeight;
            if (!IsPixelBased && (Orientation == Orientation.Vertical) && DoubleUtil.IsZero(delta))
            {
                delta = 1;
            }
            SetVerticalOffsetImpl(VerticalOffset + delta);
            SetAnchorInformation(false /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one page to the left.
        ///     Subclases can override this method and call SetHorizontalOffset to change
        ///     the behavior of what "page" means.
        /// </summary>
        public virtual void PageLeft()
        {
            double delta = ViewportWidth;
            if (!IsPixelBased && (Orientation == Orientation.Horizontal) && DoubleUtil.IsZero(delta))
            {
                delta = 1;
            }
            SetHorizontalOffsetImpl(HorizontalOffset - delta);
            SetAnchorInformation(true /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one page to the right.
        ///     Subclases can override this method and call SetHorizontalOffset to change
        ///     the behavior of what "page" means.
        /// </summary>
        public virtual void PageRight()
        {
            double delta = ViewportWidth;
            if (!IsPixelBased && (Orientation == Orientation.Horizontal) && DoubleUtil.IsZero(delta))
            {
                delta = 1;
            }
            SetHorizontalOffsetImpl(HorizontalOffset + delta);
            SetAnchorInformation(true /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one page to the top.
        ///     Subclases can override this method and call SetVerticalOffset to change
        ///     the behavior of the mouse wheel increment.
        /// </summary>
        public virtual void MouseWheelUp()
        {
            SetVerticalOffsetImpl(VerticalOffset - SystemParameters.WheelScrollLines * ((Orientation == Orientation.Vertical && !IsPixelBased) ? 1.0 : ScrollViewer._scrollLineDelta));
            SetAnchorInformation(false /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one page to the bottom.
        ///     Subclases can override this method and call SetVerticalOffset to change
        ///     the behavior of the mouse wheel increment.
        /// </summary>
        public virtual void MouseWheelDown()
        {
            SetVerticalOffsetImpl(VerticalOffset + SystemParameters.WheelScrollLines * ((Orientation == Orientation.Vertical && !IsPixelBased) ? 1.0 : ScrollViewer._scrollLineDelta));
            SetAnchorInformation(false /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one page to the left.
        ///     Subclases can override this method and call SetHorizontalOffset to change
        ///     the behavior of the mouse wheel increment.
        /// </summary>
        public virtual void MouseWheelLeft()
        {
            SetHorizontalOffsetImpl(HorizontalOffset - 3.0 * ((Orientation == Orientation.Horizontal && !IsPixelBased) ? 1.0 : ScrollViewer._scrollLineDelta));
            SetAnchorInformation(true /*isHorizontalOffset*/);
        }

        /// <summary>
        ///     Scroll content by one page to the right.
        ///     Subclases can override this method and call SetHorizontalOffset to change
        ///     the behavior of the mouse wheel increment.
        /// </summary>
        public virtual void MouseWheelRight()
        {
            SetHorizontalOffsetImpl(HorizontalOffset + 3.0 * ((Orientation == Orientation.Horizontal && !IsPixelBased) ? 1.0 : ScrollViewer._scrollLineDelta));
            SetAnchorInformation(true /*isHorizontalOffset*/);
        }

        /// <summary>
        /// Set the HorizontalOffset to the passed value.
        /// </summary>
        public void SetHorizontalOffset(double offset)
        {
            ClearAnchorInformation(true /*shouldAbort*/);
            SetHorizontalOffsetImpl(offset);
        }

        private void SetHorizontalOffsetImpl(double offset)
        {
            EnsureScrollData();

            double scrollX = ScrollContentPresenter.ValidateInputOffset(offset, "HorizontalOffset");
            if (!DoubleUtil.AreClose(scrollX, _scrollData._offset.X))
            {
                Vector oldViewportOffset = _scrollData._offset;

                // Store the new offset
                _scrollData._offset.X = scrollX;

                // Report the change in offset
                OnViewportOffsetChanged(oldViewportOffset, _scrollData._offset);

                if (IsVirtualizing)
                {
                    IsScrollActive = true;
                    InvalidateMeasure();
                }
                else if (!IsPixelBased)
                {
                    InvalidateMeasure();
                }
                else
                {
                    _scrollData._offset.X  = ScrollContentPresenter.CoerceOffset(scrollX, _scrollData._extent.Width, _scrollData._viewport.Width);
                    _scrollData._computedOffset.X = _scrollData._offset.X;
                    InvalidateArrange();
                    OnScrollChange();
                }
            }
        }

        /// <summary>
        /// Set the VerticalOffset to the passed value.
        /// </summary>
        public void SetVerticalOffset(double offset)
        {
            ClearAnchorInformation(true /*shouldAbort*/);
            SetVerticalOffsetImpl(offset);
        }

        private void SetVerticalOffsetImpl(double offset)
        {
            EnsureScrollData();

            double scrollY = ScrollContentPresenter.ValidateInputOffset(offset, "VerticalOffset");
            if (!DoubleUtil.AreClose(scrollY, _scrollData._offset.Y))
            {
                Vector oldViewportOffset = _scrollData._offset;

                // Store the new offset
                _scrollData._offset.Y = scrollY;

                // Report the change in offset
                OnViewportOffsetChanged(oldViewportOffset, _scrollData._offset);

                if (IsVirtualizing)
                {
                    InvalidateMeasure();
                    IsScrollActive = true;
                }
                else if (!IsPixelBased)
                {
                    InvalidateMeasure();
                }
                else
                {
                    _scrollData._offset.Y  = ScrollContentPresenter.CoerceOffset(scrollY, _scrollData._extent.Height, _scrollData._viewport.Height);
                    _scrollData._computedOffset.Y = _scrollData._offset.Y;
                    InvalidateArrange();
                    OnScrollChange();
                }
            }
        }

        private void SetAnchorInformation(bool isHorizontalOffset)
        {
            if (IsScrolling)
            {
                //
                // Anchoring is a technique used to overcome the shortcoming that when virtualizing
                // the extent size is an estimation and could fluctuate as we measure more containers.
                // So we only care to employ this technique when virtualizing.
                //
                if (IsVirtualizing)
                {
                    //
                    // We only care about anchoring along the stacking direction because
                    // that is the direction along which we virtualize
                    //
                    bool isHorizontal = (Orientation == Orientation.Horizontal);
                    if (isHorizontal == isHorizontalOffset)
                    {
                        bool areContainersUniformlySized = GetAreContainersUniformlySized(null, this);

                        //
                        // If the containers in this panel aren't uniformly sized or if this is a hierarchical scenario
                        // involving grouping or TreeView then the chances that we err in our extent estimations
                        // are larger and hence we need to store the anchor information.
                        //
                        if (!areContainersUniformlySized || HasVirtualizingChildren)
                        {
                            ItemsControl itemsControl;
                            ItemsControl.GetItemsOwnerInternal(this, out itemsControl);

                            if (itemsControl != null)
                            {
                                double expectedDistanceBetweenViewports = (isHorizontal ? _scrollData._offset.X - _scrollData._computedOffset.X : _scrollData._offset.Y - _scrollData._computedOffset.Y);

                                if (_firstContainerInViewport != null)
                                {
                                    //
                                    // Retry the pending AnchorOperation
                                    //
                                    OnAnchorOperation(true /*isAnchorOperationPending*/);

                                    //
                                    // Adjust offsets
                                    //
                                    if (isHorizontal)
                                    {
                                        _scrollData._offset.X += expectedDistanceBetweenViewports;
                                    }
                                    else
                                    {
                                        _scrollData._offset.Y += expectedDistanceBetweenViewports;
                                    }
                                }

                                if (_firstContainerInViewport == null)
                                {
                                    _firstContainerInViewport = ComputeFirstContainerInViewport(
                                        itemsControl.GetViewportElement(),
                                        isHorizontal ? FocusNavigationDirection.Right : FocusNavigationDirection.Down,
                                        this,
                                        delegate(DependencyObject d)
                                        {
                                            // Mark this container non-virtualizable because it is along the path leading to
                                            // the leaf container that will serve as an anchor for the current scroll operation.
                                            d.SetCurrentValue(VirtualizingPanel.IsContainerVirtualizableProperty, false);
                                        },
                                        out _firstContainerOffsetFromViewport);

                                    if (_firstContainerInViewport != null)
                                    {
                                        _expectedDistanceBetweenViewports = expectedDistanceBetweenViewports;

                                        Debug.Assert(AnchorOperationField.GetValue(this) == null, "There is already a pending AnchorOperation.");
                                        DispatcherOperation anchorOperation = Dispatcher.BeginInvoke(DispatcherPriority.Render, (Action)OnAnchorOperation);
                                        AnchorOperationField.SetValue(this, anchorOperation);
                                    }
                                }
                                else
                                {
                                    _expectedDistanceBetweenViewports += expectedDistanceBetweenViewports;
                                }
                            }
                        }
                    }
                }
            }
        }

        private void OnAnchorOperation()
        {
            bool isAnchorOperationPending = false;
            OnAnchorOperation(isAnchorOperationPending);
        }

        private void OnAnchorOperation(bool isAnchorOperationPending)
        {
            Debug.Assert(_firstContainerInViewport != null, "Must have an anchor element");

            ItemsControl itemsControl;
            ItemsControl.GetItemsOwnerInternal(this, out itemsControl);
            if (itemsControl == null || !VisualTreeHelper.IsAncestorOf(this, _firstContainerInViewport))
            {
                ClearAnchorInformation(isAnchorOperationPending /*shouldAbort*/);
                return;
            }

            bool isHorizontal = (Orientation == Orientation.Horizontal);

            FrameworkElement prevFirstContainerInViewport = _firstContainerInViewport;
            double prevFirstContainerOffsetFromViewport = _firstContainerOffsetFromViewport;
            double prevFirstContainerOffset = FindScrollOffset(_firstContainerInViewport);

            double currFirstContainerOffsetFromViewport;
            FrameworkElement currFirstContainerInViewport = ComputeFirstContainerInViewport(
                itemsControl.GetViewportElement(),
                isHorizontal ? FocusNavigationDirection.Right : FocusNavigationDirection.Down,
                this,
                null,
                out currFirstContainerOffsetFromViewport);
            double currFirstContainerOffset = FindScrollOffset(currFirstContainerInViewport);

            double actualDistanceBetweenViewports = (currFirstContainerOffset - currFirstContainerOffsetFromViewport) -
                                                    (prevFirstContainerOffset - prevFirstContainerOffsetFromViewport);

            if (LayoutDoubleUtil.AreClose(_expectedDistanceBetweenViewports, actualDistanceBetweenViewports))
            {
                if (isHorizontal)
                {
                    _scrollData._computedOffset.X = currFirstContainerOffset - currFirstContainerOffsetFromViewport;
                    _scrollData._offset.X = _scrollData._computedOffset.X;
                }
                else
                {
                    _scrollData._computedOffset.Y = currFirstContainerOffset - currFirstContainerOffsetFromViewport;
                    _scrollData._offset.Y = _scrollData._computedOffset.Y;
                }

                //
                // If we are at the right position with respect to the anchor then
                // we dont need the anchor element any more. So clear it.
                //
                ClearAnchorInformation(isAnchorOperationPending /*shouldAbort*/);
            }
            else
            {
                bool remeasure = false;

                if (isHorizontal)
                {
                    _scrollData._computedOffset.X = prevFirstContainerOffset - prevFirstContainerOffsetFromViewport;

                    double actualOffset = _scrollData._computedOffset.X + actualDistanceBetweenViewports;
                    double expectedOffset = _scrollData._computedOffset.X + _expectedDistanceBetweenViewports;

                    if (DoubleUtil.LessThan(expectedOffset, 0) || DoubleUtil.GreaterThan(expectedOffset, _scrollData._extent.Width - _scrollData._viewport.Width))
                    {
                        Debug.Assert(DoubleUtil.AreClose(actualOffset, 0) || DoubleUtil.AreClose(actualOffset, _scrollData._extent.Width - _scrollData._viewport.Width), "The actual offset should already be at the beginning or the end.");
                        _scrollData._computedOffset.X = actualOffset;
                        _scrollData._offset.X = actualOffset;
                    }
                    else
                    {
                        remeasure = true;
                        _scrollData._offset.X = expectedOffset;
                    }
                }
                else
                {
                    _scrollData._computedOffset.Y = prevFirstContainerOffset - prevFirstContainerOffsetFromViewport;

                    double actualOffset = _scrollData._computedOffset.Y + actualDistanceBetweenViewports;
                    double expectedOffset = _scrollData._computedOffset.Y + _expectedDistanceBetweenViewports;

                    if (DoubleUtil.LessThan(expectedOffset, 0) || DoubleUtil.GreaterThan(expectedOffset, _scrollData._extent.Height - _scrollData._viewport.Height))
                    {
                        Debug.Assert(DoubleUtil.AreClose(actualOffset, 0) || DoubleUtil.AreClose(actualOffset, _scrollData._extent.Height - _scrollData._viewport.Height), "The actual offset should already be at the beginning or the end.");
                        _scrollData._computedOffset.Y = actualOffset;
                        _scrollData._offset.Y = actualOffset;
                    }
                    else
                    {
                        remeasure = true;
                        _scrollData._offset.Y = expectedOffset;
                    }
                }

                if (remeasure)
                {
                    //
                    // We have adjusted the offset and need to remeasure
                    //
                    OnScrollChange();
                    InvalidateMeasure();

                    if (!isAnchorOperationPending)
                    {
                        DispatcherOperation anchorOperation = Dispatcher.BeginInvoke(DispatcherPriority.Render, (Action)OnAnchorOperation);
                        AnchorOperationField.SetValue(this, anchorOperation);
                    }
                }
                else
                {
                    ClearAnchorInformation(isAnchorOperationPending /*shouldAbort*/);
                }
            }
        }

        private void ClearAnchorInformation(bool shouldAbort)
        {
            if (_firstContainerInViewport != null)
            {
                DependencyObject element = _firstContainerInViewport;
                do
                {
                    DependencyObject parent = VisualTreeHelper.GetParent(element);
                    Panel parentItemsHost = parent as Panel;
                    if (parentItemsHost != null && parentItemsHost.IsItemsHost)
                    {
                        // This to clear the current value that we previously set
                        element.InvalidateProperty(VirtualizingPanel.IsContainerVirtualizableProperty);
                    }
                    element = parent;
                }
                while (element != null && element != this);

                _firstContainerInViewport = null;
                _firstContainerOffsetFromViewport = 0;
                _expectedDistanceBetweenViewports = 0;

                if (shouldAbort)
                {
                    DispatcherOperation anchorOperation = AnchorOperationField.GetValue(this);
                    anchorOperation.Abort();
                }

                AnchorOperationField.ClearValue(this);
            }
        }

        private FrameworkElement ComputeFirstContainerInViewport(
            FrameworkElement viewportElement,
            FocusNavigationDirection direction,
            Panel itemsHost,
            Action<DependencyObject> action,
            out double firstContainerOffsetFromViewport)
        {
            firstContainerOffsetFromViewport = 0;

            if (itemsHost == null)
            {
                return null;
            }

            FrameworkElement result = null;
            UIElementCollection children = itemsHost.Children;
            if (children != null)
            {
                int count = children.Count;
                int i = (itemsHost is VirtualizingStackPanel ? ((VirtualizingStackPanel)itemsHost)._firstItemInExtendedViewportChildIndex : 0);
                for (; i<count; i++)
                {
                    FrameworkElement fe = children[i] as FrameworkElement;
                    if (fe != null && fe.IsVisible)
                    {
                        Rect elementRect;

                        ElementViewportPosition elementPosition = ItemsControl.GetElementViewportPosition(
                            viewportElement,
                            fe,
                            direction,
                            false /*fullyVisible*/,
                            out elementRect);

                        if (elementPosition == ElementViewportPosition.PartiallyInViewport ||
                            elementPosition == ElementViewportPosition.CompletelyInViewport)
                        {
                            if (action != null)
                            {
                                action(fe);
                            }

                            ItemsControl itemsControl = fe as ItemsControl;
                            if (itemsControl != null)
                            {
                                if (itemsControl.ItemsHost != null && itemsControl.ItemsHost.IsVisible)
                                {
                                    result = ComputeFirstContainerInViewport(viewportElement, direction, itemsControl.ItemsHost, action, out firstContainerOffsetFromViewport);
                                }
                            }
                            else
                            {
                                GroupItem groupItem = fe as GroupItem;
                                if (groupItem != null && groupItem.ItemsHost != null && groupItem.ItemsHost.IsVisible)
                                {
                                    result = ComputeFirstContainerInViewport(viewportElement, direction, groupItem.ItemsHost, action, out firstContainerOffsetFromViewport);
                                }
                            }

                            if (result == null)
                            {
                                result = fe;

                                if (IsPixelBased)
                                {
                                    if (direction == FocusNavigationDirection.Down)
                                    {
                                        firstContainerOffsetFromViewport = elementRect.Y;
                                    }
                                    else // (direction == FocusNavigationDirection.Right)
                                    {
                                        firstContainerOffsetFromViewport = elementRect.X;
                                    }
                                }
                            }
                            else if (!IsPixelBased)
                            {
                                IHierarchicalVirtualizationAndScrollInfo virtualizingElement = fe as IHierarchicalVirtualizationAndScrollInfo;
                                if (virtualizingElement != null)
                                {
                                    if (direction == FocusNavigationDirection.Down)
                                    {
                                        if (DoubleUtil.GreaterThanOrClose(elementRect.Y, 0))
                                        {
                                            firstContainerOffsetFromViewport += virtualizingElement.HeaderDesiredSizes.LogicalSize.Height;
                                        }
                                    }
                                    else // (direction == FocusNavigationDirection.Right)
                                    {
                                        if (DoubleUtil.GreaterThanOrClose(elementRect.X, 0))
                                        {
                                            firstContainerOffsetFromViewport += virtualizingElement.HeaderDesiredSizes.LogicalSize.Width;
                                        }
                                    }
                                }
                            }

                            break;
                        }
                        else if (elementPosition == ElementViewportPosition.AfterViewport)
                        {
                            // We've gone too far
                            break;
                        }
                    }
                }
            }

            return result;
        }

        internal void AnchoredInvalidateMeasure()
        {
            WasLastMeasurePassAnchored = (_firstContainerInViewport != null) || (_bringIntoViewLeafContainer != null);

            DispatcherOperation anchoredInvalidateMeasureOperation = AnchoredInvalidateMeasureOperationField.GetValue(this);
            if (anchoredInvalidateMeasureOperation == null)
            {
                anchoredInvalidateMeasureOperation = Dispatcher.BeginInvoke(DispatcherPriority.Render,
                    (Action)delegate()
                {
                    AnchoredInvalidateMeasureOperationField.ClearValue(this);

                    if (WasLastMeasurePassAnchored)
                    {
                        SetAnchorInformation(Orientation == Orientation.Horizontal);
                    }

                    InvalidateMeasure();
                });

                AnchoredInvalidateMeasureOperationField.SetValue(this, anchoredInvalidateMeasureOperation);
            }
        }

        /// <summary>
        /// VirtualizingStackPanel implementation of <seealso cref="IScrollInfo.MakeVisible" />.
        /// </summary>
        // The goal is to change offsets to bring the child into view, and return a rectangle in our space to make visible.
        // The rectangle we return is in the physical dimension the input target rect transformed into our pace.
        // In the logical dimension, it is our immediate child's rect.
        // Note: This code presently assumes we/children are layout clean.  See work item 22269 for more detail.
        public Rect MakeVisible(Visual visual, Rect rectangle)
        {
            ClearAnchorInformation(true /*shouldAbort*/);

            Vector newOffset = new Vector();
            Rect newRect = new Rect();
            Rect originalRect = rectangle;
            bool isHorizontal = (Orientation == Orientation.Horizontal);

            // We can only work on visuals that are us or children.
            // An empty rect has no size or position.  We can't meaningfully use it.
            if (    rectangle.IsEmpty
                || visual == null
                || visual == (Visual)this
                ||  !this.IsAncestorOf(visual))
            {
                return Rect.Empty;
            }

#pragma warning disable 1634, 1691
#pragma warning disable 56506
            // Compute the child's rect relative to (0,0) in our coordinate space.
            // This is a false positive by PreSharp. visual cannot be null because of the 'if' check above
            GeneralTransform childTransform = visual.TransformToAncestor(this);
#pragma warning restore 56506
#pragma warning restore 1634, 1691
            rectangle = childTransform.TransformBounds(rectangle);

            // We can't do any work unless we're scrolling.
            if (!IsScrolling)
            {
                return rectangle;
            }

            bool alignTop = false;
            bool alignBottom = false;

            // Make ourselves visible in the non-stacking direction
            MakeVisiblePhysicalHelper(rectangle, ref newOffset, ref newRect, !isHorizontal, ref alignTop, ref alignBottom);

            alignTop = (_bringIntoViewLeafContainer == visual && AlignTopOfBringIntoViewContainer);
            alignBottom = (_bringIntoViewLeafContainer == visual && !AlignTopOfBringIntoViewContainer);

            if (IsPixelBased)
            {
                MakeVisiblePhysicalHelper(rectangle, ref newOffset, ref newRect, isHorizontal, ref alignTop, ref alignBottom);
            }
            else
            {
                // Bring our child containing the visual into view.
                // For non-pixel based scrolling the offset is in logical units in the stacking direction
                // and physical units in the other. Hence the logical helper call here.
                int childIndex = (int)FindScrollOffset(visual);
                MakeVisibleLogicalHelper(childIndex, rectangle, ref newOffset, ref newRect, ref alignTop, ref alignBottom);
            }

            // We have computed the scrolling offsets; validate and scroll to them.
            newOffset.X = ScrollContentPresenter.CoerceOffset(newOffset.X, _scrollData._extent.Width, _scrollData._viewport.Width);
            newOffset.Y = ScrollContentPresenter.CoerceOffset(newOffset.Y, _scrollData._extent.Height, _scrollData._viewport.Height);

            if (!LayoutDoubleUtil.AreClose(newOffset.X, _scrollData._offset.X) ||
                !LayoutDoubleUtil.AreClose(newOffset.Y, _scrollData._offset.Y))
            {
                // We are about to make this container visible
                if (visual != _bringIntoViewLeafContainer)
                {
                    _bringIntoViewLeafContainer = visual;
                    AlignTopOfBringIntoViewContainer = alignTop;
                }

                Vector oldOffset = _scrollData._offset;
                _scrollData._offset = newOffset;

                OnViewportOffsetChanged(oldOffset, newOffset);

                if (IsVirtualizing)
                {
                    IsScrollActive = true;
                    InvalidateMeasure();
                }
                else if (!IsPixelBased)
                {
                    InvalidateMeasure();
                }
                else
                {
                    _scrollData._computedOffset = newOffset;
                    InvalidateArrange();
                }

                OnScrollChange();
                if (ScrollOwner != null)
                {
                    // When layout gets updated it may happen that visual is obscured by a ScrollBar
                    // We call MakeVisible again to make sure element is visible in this case
                    ScrollOwner.MakeVisible(visual, originalRect);
                }
            }
            else
            {
                // We have successfully made the container visible
                _bringIntoViewLeafContainer = null;
                AlignTopOfBringIntoViewContainer = false;
            }

            // Return the rectangle
            return newRect;
        }

        /// <summary>
        /// Generates the item at the specified index and calls BringIntoView on it.
        /// </summary>
        /// <param name="index">Specify the item index that should become visible. This is the index into ItemsControl.Items collection</param>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Thrown if index is out of range
        /// </exception>
        protected internal override void BringIndexIntoView(int index)
        {
            ItemsControl itemsControl = ItemsControl.GetItemsOwner(this);

            // If panel is hosting a flat list of containers,
            // this panel can directly generate a container for it
            // and call bring the container into view.
            if( !itemsControl.IsGrouping )
            {
                BringContainerIntoView(itemsControl, index);
            }
            else
            {
                // When grouping the item could be any number of levels deep into hierarchy of CollectionViewGroups.
                EnsureGenerator();
                ItemContainerGenerator generator = (ItemContainerGenerator)Generator;
                IList items = generator.ItemsInternal;

                for (int i = 0; i < items.Count; i++)
                {
                    CollectionViewGroup cvg = items[i] as CollectionViewGroup;
                    if (cvg != null)
                    {
                        if (index >= cvg.ItemCount)
                        {
                            index -= cvg.ItemCount;
                        }
                        else
                        {
                            // Item was found somewhere within this CVG hierarchy
                            // Get the GroupItem hosting this CVG.
                            GroupItem groupItem = generator.ContainerFromItem(cvg) as GroupItem;
                            if (groupItem == null)
                            {
                                // Devirtualize container and try 2nd time.
                                BringContainerIntoView(itemsControl, i);
                                groupItem = generator.ContainerFromItem(cvg) as GroupItem;
                            }

                            if (groupItem != null)
                            {
                                // flush out layout queue so that ItemsHost gets hooked up.
                                // GroupItem also would inherit updated viewport data from parent VSP.
                                groupItem.UpdateLayout();

                                VirtualizingPanel itemsHost = groupItem.ItemsHost as VirtualizingPanel;
                                if (itemsHost != null)
                                {
                                    // Recursively call child panels until item is found.
                                    itemsHost.BringIndexIntoViewPublic(index);
                                }
                            }
                            break;
                        }
                    }
                    else if (i == index)
                    {
                        // This is the leaf level panel
                        // Compare
                        BringContainerIntoView(itemsControl, i);
                    }
                }
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="itemIndex">index into the children of this panel</param>
        private void BringContainerIntoView(ItemsControl itemsControl, int itemIndex)
        {
            if (itemIndex < 0 || itemIndex >= ItemCount)
                throw new ArgumentOutOfRangeException("itemIndex");

            UIElement child;
            IItemContainerGenerator generator = Generator;
            int childIndex;
            bool visualOrderChanged = false;
            GeneratorPosition position = IndexToGeneratorPositionForStart(itemIndex, out childIndex);
            using (generator.StartAt(position, GeneratorDirection.Forward, true))
            {
                bool newlyRealized;
                child = generator.GenerateNext(out newlyRealized) as UIElement;
                if (child != null)
                {
                    visualOrderChanged = AddContainerFromGenerator(childIndex, child, newlyRealized, false /*isBeforeViewport*/);

                    if (visualOrderChanged)
                    {
                        Debug.Assert(IsVirtualizing && InRecyclingMode, "We should only modify the visual order when in recycling mode");
                        InvalidateZState();
                    }
                }
            }

            if (child != null)
            {
                FrameworkElement childFE = child as FrameworkElement;
                if (childFE != null)
                {
                    _bringIntoViewContainer = childFE;

                    Dispatcher.BeginInvoke(DispatcherPriority.Loaded, (Action)delegate()
                        {
                            //
                            // Carefully ---- the _bringIntoViewContainer after the storm of layouts to bring it into view has subsided
                            //
                            _bringIntoViewContainer = null;
                        });

                    if (!itemsControl.IsGrouping && VirtualizingPanel.GetScrollUnit(itemsControl) == ScrollUnit.Item)
                    {
                        childFE.BringIntoView();
                    }
                    else if (!(childFE is GroupItem))
                    {
                        UpdateLayout();
                        childFE.BringIntoView();
                    }
                }
            }
        }

        #endregion

        #endregion

        //-------------------------------------------------------------------
        //
        //  Public Properties
        //
        //-------------------------------------------------------------------

        #region Public Properties

        /// <summary>
        ///     Attached property for use on the ItemsControl that is the host for the items being
        ///     presented by this panel. Use this property to turn virtualization on/off.
        /// </summary>
        public new static readonly DependencyProperty IsVirtualizingProperty =
            VirtualizingPanel.IsVirtualizingProperty;

        /// <summary>
        ///     Attached property for use on the ItemsControl that is the host for the items being
        ///     presented by this panel. Use this property to modify the virtualization mode.
        ///
        ///     Note that this property can only be set before the panel has been initialized
        /// </summary>
        public new static readonly DependencyProperty VirtualizationModeProperty =
            VirtualizingPanel.VirtualizationModeProperty;

        /// <summary>
        /// Specifies dimension of children stacking.
        /// </summary>
        public Orientation Orientation
        {
            get { return (Orientation) GetValue(OrientationProperty); }
            set { SetValue(OrientationProperty, value); }
        }

        /// <summary>
        /// This property is always true because this panel has vertical or horizontal orientation
        /// </summary>
        protected internal override bool HasLogicalOrientation
        {
            get { return true; }
        }

        /// <summary>
        ///     Orientation of the panel if its layout is in one dimension.
        /// Otherwise HasLogicalOrientation is false and LogicalOrientation should be ignored
        /// </summary>
        protected internal override Orientation LogicalOrientation
        {
            get { return this.Orientation; }
        }

        /// <summary>
        /// DependencyProperty for <see cref="Orientation" /> property.
        /// </summary>
        public static readonly DependencyProperty OrientationProperty =
            DependencyProperty.Register("Orientation", typeof(Orientation), typeof(VirtualizingStackPanel),
                new FrameworkPropertyMetadata(Orientation.Vertical,
                        FrameworkPropertyMetadataOptions.AffectsMeasure,
                        new PropertyChangedCallback(OnOrientationChanged)),
                new ValidateValueCallback(ScrollBar.IsValidOrientation));

        //-----------------------------------------------------------
        //  IScrollInfo Properties
        //-----------------------------------------------------------
        #region IScrollInfo Properties

        /// <summary>
        /// VirtualizingStackPanel reacts to this property by changing its child measurement algorithm.
        /// If scrolling in a dimension, infinite space is allowed the child; otherwise, available size is preserved.
        /// </summary>
        [DefaultValue(false)]
        public bool CanHorizontallyScroll
        {
            get
            {
                if (_scrollData == null) { return false; }
                return _scrollData._allowHorizontal;
            }
            set
            {
                EnsureScrollData();
                if (_scrollData._allowHorizontal != value)
                {
                    _scrollData._allowHorizontal = value;
                    InvalidateMeasure();
                }
            }
        }

        /// <summary>
        /// VirtualizingStackPanel reacts to this property by changing its child measurement algorithm.
        /// If scrolling in a dimension, infinite space is allowed the child; otherwise, available size is preserved.
        /// </summary>
        [DefaultValue(false)]
        public bool CanVerticallyScroll
        {
            get
            {
                if (_scrollData == null) { return false; }
                return _scrollData._allowVertical;
            }
            set
            {
                EnsureScrollData();
                if (_scrollData._allowVertical != value)
                {
                    _scrollData._allowVertical = value;
                    InvalidateMeasure();
                }
            }
        }

        /// <summary>
        /// ExtentWidth contains the horizontal size of the scrolled content element in 1/96"
        /// </summary>
        public double ExtentWidth
        {
            get
            {
                if (_scrollData == null) { return 0.0; }
                return _scrollData._extent.Width;
            }
        }

        /// <summary>
        /// ExtentHeight contains the vertical size of the scrolled content element in 1/96"
        /// </summary>
        public double ExtentHeight
        {
            get
            {
                if (_scrollData == null) { return 0.0; }
                return _scrollData._extent.Height;
            }
        }

        /// <summary>
        /// ViewportWidth contains the horizontal size of content's visible range in 1/96"
        /// </summary>
        public double ViewportWidth
        {
            get
            {
                if (_scrollData == null) { return 0.0; }
                return _scrollData._viewport.Width;
            }
        }

        /// <summary>
        /// ViewportHeight contains the vertical size of content's visible range in 1/96"
        /// </summary>
        public double ViewportHeight
        {
            get
            {
                if (_scrollData == null) { return 0.0; }
                return _scrollData._viewport.Height;
            }
        }

        /// <summary>
        /// HorizontalOffset is the horizontal offset of the scrolled content in 1/96".
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public double HorizontalOffset
        {
            get
            {
                if (_scrollData == null) { return 0.0; }
                return _scrollData._computedOffset.X;
            }
        }

        /// <summary>
        /// VerticalOffset is the vertical offset of the scrolled content in 1/96".
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public double VerticalOffset
        {
            get
            {
                if (_scrollData == null) { return 0.0; }
                return _scrollData._computedOffset.Y;
            }
        }

        /// <summary>
        /// ScrollOwner is the container that controls any scrollbars, headers, etc... that are dependant
        /// on this IScrollInfo's properties.
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public ScrollViewer ScrollOwner
        {
            get
            {
                EnsureScrollData();
                return _scrollData._scrollOwner;
            }
            set
            {
                EnsureScrollData();
                if (value != _scrollData._scrollOwner)
                {
                    ResetScrolling(this);
                    _scrollData._scrollOwner = value;
                }
            }
        }

        #endregion IScrollInfo Properties

        #endregion Public Properties

        //-------------------------------------------------------------------
        //
        //  Public Events
        //
        //-------------------------------------------------------------------


        #region Public Events

        /// <summary>
        ///     Called on the ItemsControl that owns this panel when an item is being re-virtualized.
        /// </summary>
        public static readonly RoutedEvent CleanUpVirtualizedItemEvent = EventManager.RegisterRoutedEvent("CleanUpVirtualizedItemEvent", RoutingStrategy.Direct, typeof(CleanUpVirtualizedItemEventHandler), typeof(VirtualizingStackPanel));


        /// <summary>
        ///     Adds a handler for the CleanUpVirtualizedItem attached event
        /// </summary>
        /// <param name="element">DependencyObject that listens to this event</param>
        /// <param name="handler">Event Handler to be added</param>
        public static void AddCleanUpVirtualizedItemHandler(DependencyObject element, CleanUpVirtualizedItemEventHandler handler)
        {
            FrameworkElement.AddHandler(element, CleanUpVirtualizedItemEvent, handler);
        }

        /// <summary>
        ///     Removes a handler for the CleanUpVirtualizedItem attached event
        /// </summary>
        /// <param name="element">DependencyObject that listens to this event</param>
        /// <param name="handler">Event Handler to be removed</param>
        public static void RemoveCleanUpVirtualizedItemHandler(DependencyObject element, CleanUpVirtualizedItemEventHandler handler)
        {
            FrameworkElement.RemoveHandler(element, CleanUpVirtualizedItemEvent, handler);
        }

        /// <summary>
        ///     Called when an item is being re-virtualized.
        /// </summary>
        protected virtual void OnCleanUpVirtualizedItem(CleanUpVirtualizedItemEventArgs e)
        {
            ItemsControl itemsControl = ItemsControl.GetItemsOwner(this);

            if (itemsControl != null)
            {
                itemsControl.RaiseEvent(e);
            }
        }

        #endregion

        //-------------------------------------------------------------------
        //
        //  Protected Methods
        //
        //-------------------------------------------------------------------

        #region Protected Methods

        protected override bool CanHierarchicallyScrollAndVirtualizeCore
        {
            get { return true; }
        }

        /// <summary>
        /// General VirtualizingStackPanel layout behavior is to grow unbounded in the "stacking" direction (Size To Content).
        /// Children in this dimension are encouraged to be as large as they like.  In the other dimension,
        /// VirtualizingStackPanel will assume the maximum size of its children.
        /// </summary>
        /// <remarks>
        /// When scrolling, VirtualizingStackPanel will not grow in layout size but effectively add the children on a z-plane which
        /// will probably be clipped by some parent (typically a ScrollContentPresenter) to Stack's size.
        /// </remarks>
        /// <param name="constraint">Constraint</param>
        /// <returns>Desired size</returns>
        protected override Size MeasureOverride(Size constraint)
        {
#if Profiling
            if (Panel.IsAboutToGenerateContent(this))
                return MeasureOverrideProfileStub(constraint);
            else
                return RealMeasureOverride(constraint);
        }

        // this is a handy place to start/stop profiling
        private Size MeasureOverrideProfileStub(Size constraint)
        {
            return RealMeasureOverride(constraint);
        }

        private Size RealMeasureOverride(Size constraint)
        {
#endif
            List<double> previouslyMeasuredOffsets = null;
            return MeasureOverrideImpl(constraint, null, ref previouslyMeasuredOffsets, remeasure:false);
        }

        private Size MeasureOverrideImpl(Size constraint, double? lastPageSafeOffset, ref List<double> previouslyMeasuredOffsets, bool remeasure)
        {
            bool etwTracingEnabled = IsScrolling && EventTrace.IsEnabled(EventTrace.Keyword.KeywordGeneral, EventTrace.Level.Info);
            if (etwTracingEnabled)
            {
                EventTrace.EventProvider.TraceEvent(EventTrace.Event.WClientStringBegin, EventTrace.Keyword.KeywordGeneral, EventTrace.Level.Info, "VirtualizingStackPanel :MeasureOverride");
            }

            //
            //  Initialize the sizes to be computed in this routine.
            //
            Size stackPixelSize = new Size();
            Size stackLogicalSize = new Size();
            Size stackPixelSizeInViewport = new Size();
            Size stackLogicalSizeInViewport = new Size();
            Size stackPixelSizeInCacheBeforeViewport = new Size();
            Size stackLogicalSizeInCacheBeforeViewport = new Size();
            Size stackPixelSizeInCacheAfterViewport = new Size();
            Size stackLogicalSizeInCacheAfterViewport = new Size();

            bool hasVirtualizingChildren = false;
            ItemsChangedDuringMeasure = false;

            try
            {
                if (!IsItemsHost)
                {
                    stackPixelSize = MeasureNonItemsHost(constraint);
                }
                else
                {
                    // ===================================================================================
                    // ===================================================================================
                    // Fetch owners
                    // ===================================================================================
                    // ===================================================================================

                    ItemsControl itemsControl = null;
                    GroupItem groupItem = null;

                    //
                    // This is an interface implemented by the owner of this panel in order to facilitate between a parent
                    // panel and this one when virtualizing in a hierarchy. (Eg. TreeView or grouping ItemsControl.) This
                    // interface is currently implemented by TreeViewItem and GroupItem.
                    //
                    IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider = null;

                    //
                    // This is a service provided by the owner of this panel to store and retrieve information on a per item
                    // basis. Specifically this panel uses this service to remember DesiredSize of items when virtualizing.
                    // This interface is currently implemented by ItemsControl and GroupItem.
                    //
                    IContainItemStorage itemStorageProvider = null;

                    //
                    // This is the item representing the owner for this panel. (Eg. The CollectionViewGroup for the owner GroupItem)
                    //
                    object parentItem = null;

                    //
                    // Is horizontally stacking
                    //
                    bool isHorizontal = (Orientation == Orientation.Horizontal);

                    //
                    // Compute if this panel is different in orientation that either its parent or descendents
                    //
                    bool mustDisableVirtualization = false;

                    //
                    // Fetch the owner for this panel. That could either be an ItemsControl or a GroupItem.
                    //
                    GetOwners(true /*shouldSetVirtualizationState*/, isHorizontal, out itemsControl, out groupItem, out itemStorageProvider, out virtualizationInfoProvider, out parentItem, out mustDisableVirtualization);

                    // ===================================================================================
                    // ===================================================================================
                    // Initialize viewport
                    // ===================================================================================
                    // ===================================================================================

                    //
                    // The viewport constraint used by this panel.
                    //
                    Rect viewport = Rect.Empty, extendedViewport = Rect.Empty;

                    //
                    // Sizes of cache before/after viewport
                    //
                    VirtualizationCacheLength cacheSize = new VirtualizationCacheLength(0.0);
                    VirtualizationCacheLengthUnit cacheUnit = VirtualizationCacheLengthUnit.Pixel;

                    //
                    // Initialize the viewport for this panel.
                    //
                    InitializeViewport(virtualizationInfoProvider, isHorizontal, constraint, ref viewport, ref cacheSize, ref cacheUnit, out extendedViewport);

                    // ===================================================================================
                    // ===================================================================================
                    // Compute first item in viewport
                    // ===================================================================================
                    // ===================================================================================

                    //
                    // Index of first item in the viewport.
                    //
                    int firstItemInViewportIndex = Int32.MinValue, lastItemInViewportIndex = Int32.MaxValue, firstItemInViewportChildIndex = Int32.MinValue, firstItemInExtendedViewportIndex = Int32.MinValue;

                    //
                    // Offset of the top of the first item relative to the top of the viewport.
                    //
                    double firstItemInViewportOffset = 0.0, firstItemInExtendedViewportOffset = 0.0;

                    //
                    // Says if the first and last items in the viewport has been encountered this far
                    //
                    bool foundFirstItemInViewport = false, foundLastItemInViewport = false, foundFirstItemInExtendedViewport = false;

                    //
                    // Get set to enumerate the items of the owner
                    //
                    EnsureGenerator();
                    IList children = RealizedChildren;  // yes, this is weird, but this property ensures the Generator is properly initialized.
                    IItemContainerGenerator generator = Generator;
                    IList items = ((ItemContainerGenerator)generator).ItemsInternal;
                    int itemCount = items.Count;

                    //
                    // Locally cache the values of this flag and size until the end of this measure pass.
                    // This is important because switching the areContainersUniformlySized flag in the
                    // middle of a measure leads to skewed results in the computation of the extensions
                    // to the stackSize.
                    //
                    bool areContainersUniformlySized = GetAreContainersUniformlySized(itemStorageProvider, parentItem);
                    bool computedAreContainersUniformlySized = areContainersUniformlySized;
                    bool hasUniformOrAverageContainerSizeBeenSet;
                    double uniformOrAverageContainerSize = GetUniformOrAverageContainerSize(itemStorageProvider, parentItem, out hasUniformOrAverageContainerSizeBeenSet);
                    double computedUniformOrAverageContainerSize = uniformOrAverageContainerSize;

                    //
                    // Compute index and offset of first item in the viewport
                    //
                    ComputeFirstItemInViewportIndexAndOffset(items, itemCount, itemStorageProvider, viewport, cacheSize,
                        isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize,
                        out firstItemInViewportOffset,
                        out firstItemInViewportIndex,
                        out foundFirstItemInViewport);

                    //
                    // Compute index and offset of first item in the extendedViewport
                    //
                    ComputeFirstItemInViewportIndexAndOffset(items, itemCount, itemStorageProvider, extendedViewport, new VirtualizationCacheLength(0.0),
                        isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize,
                        out firstItemInExtendedViewportOffset,
                        out firstItemInExtendedViewportIndex,
                        out foundFirstItemInExtendedViewport);

                    if (IsVirtualizing)
                    {
                        // ===================================================================================
                        // ===================================================================================
                        // Recycle containers
                        // ===================================================================================
                        // ===================================================================================

                        //
                        // If we arrive here through a remeasure this assertion wont be true
                        // because we have postponed the cleanup until after this remeasure
                        // is complete.
                        //
                        // debug_AssertRealizedChildrenEqualVisualChildren();

                        //
                        // If recycling clean up before generating children so that recycled
                        // containers are available for the current measure pass.
                        // But if this is a remeasure pass we do not want to recycle the
                        // containers because there could be several iterations of these and
                        // we should wait for the storm to subside to reclaim containers.
                        //
                        if (!remeasure && InRecyclingMode)
                        {
                            CleanupContainers(firstItemInExtendedViewportIndex, _itemsInExtendedViewportCount, itemsControl);
                            debug_VerifyRealizedChildren();
                        }
                    }

                    // ===================================================================================
                    // ===================================================================================
                    // Initialize child constraint
                    // ===================================================================================
                    // ===================================================================================

                    //
                    // Initialize child constraint. Allow children as much size as they want along the stack.
                    //
                    Size childConstraint = constraint;
                    if (isHorizontal)
                    {
                        childConstraint.Width = Double.PositiveInfinity;
                        if (IsScrolling && CanVerticallyScroll) { childConstraint.Height = Double.PositiveInfinity; }
                    }
                    else
                    {
                        childConstraint.Height = Double.PositiveInfinity;
                        if (IsScrolling && CanHorizontallyScroll) { childConstraint.Width = Double.PositiveInfinity; }
                    }

                    remeasure = false;
                    _actualItemsInExtendedViewportCount = 0;
                    _firstItemInExtendedViewportIndex = 0;
                    _firstItemInExtendedViewportOffset = 0.0;
                    _firstItemInExtendedViewportChildIndex = 0;

                    bool visualOrderChanged = false;
                    int childIndex = 0;
                    GeneratorPosition startPos;
                    bool hasBringIntoViewContainerBeenMeasured = false;

                    if (itemCount > 0)
                    {
                        // We will generate containers in several batches - one for
                        // the first visible item and the before-cache, another for
                        // the remaining visible items and the after-cache, and possibly
                        // more if the first attempts don't work out.   We want to keep
                        // the generator's status at "GeneratingContainers" throughout the
                        // entire process.  GenerateBatches does exactly what we want.
                        using (((ItemContainerGenerator)generator).GenerateBatches())
                        {
                            // ===================================================================================
                            // ===================================================================================
                            // Generate and measure children cached before the viewport.
                            // ===================================================================================
                            // ===================================================================================

                            if (!foundFirstItemInViewport ||
                                !IsEndOfCache(isHorizontal, cacheSize.CacheBeforeViewport, cacheUnit, stackPixelSizeInCacheBeforeViewport, stackLogicalSizeInCacheBeforeViewport) ||
                                !IsEndOfViewport(isHorizontal, viewport, stackPixelSizeInViewport))
                            {
                                bool adjustToChangeInFirstItem = false;

                                do
                                {
                                    Debug.Assert(!adjustToChangeInFirstItem || foundFirstItemInViewport, "This loop should only happen twice at most");

                                    adjustToChangeInFirstItem = false;

                                    //
                                    // Figure out the generator position
                                    //
                                    int startIndex;
                                    bool isAlwaysBeforeFirstItem = false;
                                    bool isAlwaysAfterFirstItem = false;
                                    bool isAlwaysAfterLastItem = false;
                                    if (IsViewportEmpty(isHorizontal, viewport) && DoubleUtil.GreaterThan(cacheSize.CacheBeforeViewport, 0.0))
                                    {
                                        isAlwaysBeforeFirstItem = true;
                                    }

                                    startIndex = firstItemInViewportIndex;
                                    startPos = IndexToGeneratorPositionForStart(firstItemInViewportIndex, out childIndex);
                                    firstItemInViewportChildIndex = childIndex;
                                    _firstItemInExtendedViewportIndex = firstItemInViewportIndex;
                                    _firstItemInExtendedViewportOffset = firstItemInViewportOffset;
                                    _firstItemInExtendedViewportChildIndex = childIndex;

                                    using (generator.StartAt(startPos, GeneratorDirection.Backward, true))
                                    {
                                        for (int i = startIndex; i >= 0; i--)
                                        {
                                            object item = items[i];

                                            MeasureChild(
                                                ref generator,
                                                ref itemStorageProvider,
                                                ref parentItem,
                                                ref hasUniformOrAverageContainerSizeBeenSet,
                                                ref computedUniformOrAverageContainerSize,
                                                ref computedAreContainersUniformlySized,
                                                ref items,
                                                ref item,
                                                ref children,
                                                ref _firstItemInExtendedViewportChildIndex,
                                                ref visualOrderChanged,
                                                ref isHorizontal,
                                                ref childConstraint,
                                                ref viewport,
                                                ref cacheSize,
                                                ref cacheUnit,
                                                ref foundFirstItemInViewport,
                                                ref firstItemInViewportOffset,
                                                ref stackPixelSize,
                                                ref stackPixelSizeInViewport,
                                                ref stackPixelSizeInCacheBeforeViewport,
                                                ref stackPixelSizeInCacheAfterViewport,
                                                ref stackLogicalSize,
                                                ref stackLogicalSizeInViewport,
                                                ref stackLogicalSizeInCacheBeforeViewport,
                                                ref stackLogicalSizeInCacheAfterViewport,
                                                ref mustDisableVirtualization,
                                                (i < firstItemInViewportIndex) || isAlwaysBeforeFirstItem,
                                                isAlwaysAfterFirstItem,
                                                isAlwaysAfterLastItem,
                                                false /*skipActualMeasure*/,
                                                false /*skipGeneration*/,
                                                ref hasBringIntoViewContainerBeenMeasured,
                                                ref hasVirtualizingChildren);

                                            if (ItemsChangedDuringMeasure)
                                            {
                                                // if the Items collection changed, our state is now invalid.  Start over.
                                                remeasure = true;
                                                goto EscapeMeasure;
                                            }

                                            _actualItemsInExtendedViewportCount++;

                                            if (!foundFirstItemInViewport)
                                            {
                                                //
                                                // Re-compute index and offset of first item in the viewport
                                                //
                                                SyncUniformSizeFlags(parentItem, children, items, itemStorageProvider, itemCount,
                                                    computedAreContainersUniformlySized, computedUniformOrAverageContainerSize, ref areContainersUniformlySized, ref uniformOrAverageContainerSize, isHorizontal, false /* evaluateAreContainersUniformlySized */);

                                                ComputeFirstItemInViewportIndexAndOffset(items, itemCount, itemStorageProvider, viewport, cacheSize,
                                                    isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize,
                                                    out firstItemInViewportOffset,
                                                    out firstItemInViewportIndex,
                                                    out foundFirstItemInViewport);

                                                if (foundFirstItemInViewport)
                                                {
                                                    if (i == firstItemInViewportIndex)
                                                    {
                                                        MeasureChild(
                                                            ref generator,
                                                            ref itemStorageProvider,
                                                            ref parentItem,
                                                            ref hasUniformOrAverageContainerSizeBeenSet,
                                                            ref computedUniformOrAverageContainerSize,
                                                            ref computedAreContainersUniformlySized,
                                                            ref items,
                                                            ref item,
                                                            ref children,
                                                            ref _firstItemInExtendedViewportChildIndex,
                                                            ref visualOrderChanged,
                                                            ref isHorizontal,
                                                            ref childConstraint,
                                                            ref viewport,
                                                            ref cacheSize,
                                                            ref cacheUnit,
                                                            ref foundFirstItemInViewport,
                                                            ref firstItemInViewportOffset,
                                                            ref stackPixelSize,
                                                            ref stackPixelSizeInViewport,
                                                            ref stackPixelSizeInCacheBeforeViewport,
                                                            ref stackPixelSizeInCacheAfterViewport,
                                                            ref stackLogicalSize,
                                                            ref stackLogicalSizeInViewport,
                                                            ref stackLogicalSizeInCacheBeforeViewport,
                                                            ref stackLogicalSizeInCacheAfterViewport,
                                                            ref mustDisableVirtualization,
                                                            false /*isBeforeFirstItem*/,
                                                            false /*isAfterFirstItem*/,
                                                            false /*isAfterLastItem*/,
                                                            true /*skipActualMeasure*/,
                                                            true /*skipGeneration*/,
                                                            ref hasBringIntoViewContainerBeenMeasured,
                                                            ref hasVirtualizingChildren);

                                                        if (ItemsChangedDuringMeasure)
                                                        {
                                                            // if the Items collection changed, our state is now invalid.  Start over.
                                                            remeasure = true;
                                                            goto EscapeMeasure;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        stackPixelSize = new Size();
                                                        stackLogicalSize = new Size();
                                                        _actualItemsInExtendedViewportCount--;
                                                        adjustToChangeInFirstItem = true;
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    break;
                                                }
                                            }

                                            //
                                            // If this is the end of the cache before the viewport break out of the loop.
                                            //
                                            if (IsEndOfCache(isHorizontal, cacheSize.CacheBeforeViewport, cacheUnit, stackPixelSizeInCacheBeforeViewport, stackLogicalSizeInCacheBeforeViewport))
                                            {
                                                break;
                                            }

                                            _firstItemInExtendedViewportIndex = Math.Max(_firstItemInExtendedViewportIndex - 1, 0);
                                            IndexToGeneratorPositionForStart(_firstItemInExtendedViewportIndex, out _firstItemInExtendedViewportChildIndex);
                                            _firstItemInExtendedViewportChildIndex = Math.Max(_firstItemInExtendedViewportChildIndex, 0);
                                        }
                                    }
                                }
                                while (adjustToChangeInFirstItem);

                                ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, 0, _firstItemInExtendedViewportIndex, out _firstItemInExtendedViewportOffset);
                            }

                            if (foundFirstItemInViewport &&
                                (!IsEndOfCache(isHorizontal, cacheSize.CacheAfterViewport, cacheUnit, stackPixelSizeInCacheAfterViewport, stackLogicalSizeInCacheAfterViewport) ||
                                 !IsEndOfViewport(isHorizontal, viewport, stackPixelSizeInViewport)))
                            {
                                //
                                // Figure out the generator position
                                //
                                int startIndex;
                                bool isAlwaysBeforeFirstItem = false;
                                bool isAlwaysAfterFirstItem = false;
                                bool isAlwaysAfterLastItem = false;

                                if (IsViewportEmpty(isHorizontal, viewport))
                                {
                                    startIndex = 0;
                                    isAlwaysAfterFirstItem = true;
                                    isAlwaysAfterLastItem = true;
                                }
                                else
                                {
                                    startIndex = firstItemInViewportIndex + 1;
                                    isAlwaysAfterFirstItem = true;
                                }

                                startPos = IndexToGeneratorPositionForStart(startIndex, out childIndex);

                                // ===================================================================================
                                // ===================================================================================
                                // Generate and measure children in the viewport and cached after the viewport.
                                // ===================================================================================
                                // ===================================================================================
                                using (generator.StartAt(startPos, GeneratorDirection.Forward, true))
                                {
                                    for (int i = startIndex; i < itemCount; i++, childIndex++)
                                    {
                                        object item = items[i];

                                        MeasureChild(
                                            ref generator,
                                            ref itemStorageProvider,
                                            ref parentItem,
                                            ref hasUniformOrAverageContainerSizeBeenSet,
                                            ref computedUniformOrAverageContainerSize,
                                            ref computedAreContainersUniformlySized,
                                            ref items,
                                            ref item,
                                            ref children,
                                            ref childIndex,
                                            ref visualOrderChanged,
                                            ref isHorizontal,
                                            ref childConstraint,
                                            ref viewport,
                                            ref cacheSize,
                                            ref cacheUnit,
                                            ref foundFirstItemInViewport,
                                            ref firstItemInViewportOffset,
                                            ref stackPixelSize,
                                            ref stackPixelSizeInViewport,
                                            ref stackPixelSizeInCacheBeforeViewport,
                                            ref stackPixelSizeInCacheAfterViewport,
                                            ref stackLogicalSize,
                                            ref stackLogicalSizeInViewport,
                                            ref stackLogicalSizeInCacheBeforeViewport,
                                            ref stackLogicalSizeInCacheAfterViewport,
                                            ref mustDisableVirtualization,
                                            isAlwaysBeforeFirstItem,
                                            (i > firstItemInViewportIndex) || isAlwaysAfterFirstItem,
                                            (i > lastItemInViewportIndex) || isAlwaysAfterLastItem,
                                            false /*skipActualMeasure*/,
                                            false /*skipGeneration*/,
                                            ref hasBringIntoViewContainerBeenMeasured,
                                            ref hasVirtualizingChildren);

                                            if (ItemsChangedDuringMeasure)
                                            {
                                                // if the Items collection changed, our state is now invalid.  Start over.
                                                remeasure = true;
                                                goto EscapeMeasure;
                                            }

                                        _actualItemsInExtendedViewportCount++;

                                        if (IsEndOfViewport(isHorizontal, viewport, stackPixelSizeInViewport))
                                        {
                                            //
                                            // If this is the last item in the original viewport make a record of it.
                                            //
                                            if (!foundLastItemInViewport)
                                            {
                                                foundLastItemInViewport = true;
                                                lastItemInViewportIndex = i;
                                            }

                                            //
                                            // If this is the end of the cache after the viewport break out of the loop.
                                            //
                                            if (IsEndOfCache(isHorizontal, cacheSize.CacheAfterViewport, cacheUnit, stackPixelSizeInCacheAfterViewport, stackLogicalSizeInCacheAfterViewport))
                                            {
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (IsVirtualizing &&
                        !IsPixelBased &&
                        (hasVirtualizingChildren || virtualizationInfoProvider != null) &&
                        (MeasureCaches || (DoubleUtil.AreClose(cacheSize.CacheBeforeViewport, 0) && DoubleUtil.AreClose(cacheSize.CacheAfterViewport, 0))))
                    {
                        //
                        // All of the descendent panels in hierarchical item scrolling scenarios that are after the extended
                        // viewport need to be measured so that they do not arrange any of their children above their own
                        // bounds and hence show through in the viewport.
                        //
                        int startIndex = _firstItemInExtendedViewportChildIndex+_actualItemsInExtendedViewportCount;
                        int childrenCount = children.Count;
                        for (int i=startIndex; i<childrenCount; i++)
                        {
                            MeasureExistingChildBeyondExtendedViewport(
                                ref generator,
                                ref itemStorageProvider,
                                ref parentItem,
                                ref hasUniformOrAverageContainerSizeBeenSet,
                                ref computedUniformOrAverageContainerSize,
                                ref computedAreContainersUniformlySized,
                                ref items,
                                ref children,
                                ref i,
                                ref visualOrderChanged,
                                ref isHorizontal,
                                ref childConstraint,
                                ref foundFirstItemInViewport,
                                ref firstItemInViewportOffset,
                                ref mustDisableVirtualization,
                                ref hasVirtualizingChildren,
                                ref hasBringIntoViewContainerBeenMeasured);

                                if (ItemsChangedDuringMeasure)
                                {
                                    // if the Items collection changed, our state is now invalid.  Start over.
                                    remeasure = true;
                                    goto EscapeMeasure;
                                }
                        }
                    }

                    if (_bringIntoViewContainer != null && !hasBringIntoViewContainerBeenMeasured)
                    {
                        //
                        // Measure the container meant to be brought into view in preparation for the next MakeVisible operation
                        //
                        childIndex = children.IndexOf(_bringIntoViewContainer);
                        if (childIndex < 0)
                        {
                            //
                            // If there were a collection changed between the BringIndexIntoView call
                            // and the current Measure then it is possible that the item for the
                            // _bringIntoViewContainer has been removed from the collection and so
                            // has the container. We need to guard against this scenario. See Dev11
                            // bug# 172296.
                            //
                            _bringIntoViewContainer = null;
                        }
                        else
                        {
                            MeasureExistingChildBeyondExtendedViewport(
                                ref generator,
                                ref itemStorageProvider,
                                ref parentItem,
                                ref hasUniformOrAverageContainerSizeBeenSet,
                                ref computedUniformOrAverageContainerSize,
                                ref computedAreContainersUniformlySized,
                                ref items,
                                ref children,
                                ref childIndex,
                                ref visualOrderChanged,
                                ref isHorizontal,
                                ref childConstraint,
                                ref foundFirstItemInViewport,
                                ref firstItemInViewportOffset,
                                ref mustDisableVirtualization,
                                ref hasVirtualizingChildren,
                                ref hasBringIntoViewContainerBeenMeasured);

                                if (ItemsChangedDuringMeasure)
                                {
                                    // if the Items collection changed, our state is now invalid.  Start over.
                                    remeasure = true;
                                    goto EscapeMeasure;
                                }
                        }
                    }

                    SyncUniformSizeFlags(parentItem, children, items, itemStorageProvider, itemCount,
                        computedAreContainersUniformlySized, computedUniformOrAverageContainerSize, ref areContainersUniformlySized, ref uniformOrAverageContainerSize, isHorizontal, false /* evaluateAreContainersUniformlySized */);

                    if (IsVirtualizing)
                    {
#if DEBUG
                        if (InRecyclingMode)
                        {
                            debug_VerifyRealizedChildren();
                        }
#endif

                        // ===================================================================================
                        // ===================================================================================
                        // Acount for the size of items before and after the viewport that won't be generated
                        // ===================================================================================
                        // ===================================================================================
                        ExtendPixelAndLogicalSizes(
                            children,
                            items,
                            itemCount,
                            itemStorageProvider,
                            areContainersUniformlySized,
                            uniformOrAverageContainerSize,
                            ref stackPixelSize,
                            ref stackLogicalSize,
                            isHorizontal,
                            _firstItemInExtendedViewportIndex,
                            _firstItemInExtendedViewportChildIndex,
                            true /*before */);

                        ExtendPixelAndLogicalSizes(
                            children,
                            items,
                            itemCount,
                            itemStorageProvider,
                            areContainersUniformlySized,
                            uniformOrAverageContainerSize,
                            ref stackPixelSize,
                            ref stackLogicalSize,
                            isHorizontal,
                            _firstItemInExtendedViewportIndex + _actualItemsInExtendedViewportCount,
                            _firstItemInExtendedViewportChildIndex + _actualItemsInExtendedViewportCount,
                            false /*before */);
                    }

                    if (IsScrolling)
                    {
                        // Adjust for position of panel within the viewport
                        EnsurePixelOffsetFromViewport();
                        if (isHorizontal)
                        {
                            if (!DoubleUtil.IsZero(stackPixelSizeInCacheBeforeViewport.Width))
                            {
                                stackPixelSizeInCacheBeforeViewport.Width += _pixelOffsetFromViewport.X;
                            }
                        }
                        else
                        {
                            if (!DoubleUtil.IsZero(stackPixelSizeInCacheBeforeViewport.Height))
                            {
                                stackPixelSizeInCacheBeforeViewport.Height += _pixelOffsetFromViewport.Y;
                            }
                        }
                    }

                    // ===================================================================================
                    // ===================================================================================
                    // [....] members that may be required during Arrange or in later Measure passes
                    // ===================================================================================
                    // ===================================================================================
                    _previousStackPixelSizeInViewport = stackPixelSizeInViewport;
                    _previousStackLogicalSizeInViewport = stackLogicalSizeInViewport;
                    _previousStackPixelSizeInCacheBeforeViewport = stackPixelSizeInCacheBeforeViewport;

                    // Coerce infinite viewport dimensions to stackPixelSize
                    if (double.IsInfinity(viewport.Width))
                    {
                        viewport.Width = stackPixelSize.Width;
                    }
                    if (double.IsInfinity(viewport.Height))
                    {
                        viewport.Height = stackPixelSize.Height;
                    }

                    _extendedViewport = ExtendViewport(
                                            virtualizationInfoProvider,
                                            isHorizontal,
                                            viewport,
                                            cacheSize,
                                            cacheUnit,
                                            stackPixelSizeInCacheBeforeViewport,
                                            stackLogicalSizeInCacheBeforeViewport,
                                            stackPixelSizeInCacheAfterViewport,
                                            stackLogicalSizeInCacheAfterViewport,
                                            stackPixelSize,
                                            stackLogicalSize,
                                            ref _itemsInExtendedViewportCount);

                    // It is important that this be set after the call to ExtendedViewport because that method uses the previous value of _viewport
                    _viewport = viewport;

                    // ===================================================================================
                    // ===================================================================================
                    // Store the sizes that have been computed on the parent
                    // ===================================================================================
                    // ===================================================================================
                    if (virtualizationInfoProvider != null && IsVisible)
                    {
                        //
                        // Note that it is possible to receive a Measure request even if the panel is
                        // actually not visible. This has been observed in a recycling TreeView where
                        // recycled TreeViewItems often switch IsExpanded states when representing
                        // different pieces of data. The IsVisible check above is to account for this scenario.
                        //
                        virtualizationInfoProvider.ItemDesiredSizes = new HierarchicalVirtualizationItemDesiredSizes(
                            stackLogicalSize,
                            stackLogicalSizeInViewport,
                            stackLogicalSizeInCacheBeforeViewport,
                            stackLogicalSizeInCacheAfterViewport,
                            stackPixelSize,
                            stackPixelSizeInViewport,
                            stackPixelSizeInCacheBeforeViewport,
                            stackPixelSizeInCacheAfterViewport);
                        virtualizationInfoProvider.MustDisableVirtualization = mustDisableVirtualization;
                    }

                    if (MustDisableVirtualization != mustDisableVirtualization)
                    {
                        MustDisableVirtualization = mustDisableVirtualization;
                        remeasure |= IsScrolling;
                    }

                    // ===================================================================================
                    // ===================================================================================
                    // Adjust the scroll offset, extent, etc.
                    // ===================================================================================
                    // ===================================================================================
                    if (IsScrolling)
                    {
                        SetAndVerifyScrollingData(
                            isHorizontal,
                            viewport,
                            constraint,
                            ref stackPixelSize,
                            ref stackLogicalSize,
                            ref stackPixelSizeInViewport,
                            ref stackLogicalSizeInViewport,
                            ref stackPixelSizeInCacheBeforeViewport,
                            ref stackLogicalSizeInCacheBeforeViewport,
                            ref remeasure,
                            ref lastPageSafeOffset,
                            ref previouslyMeasuredOffsets);
                    }

                    EscapeMeasure:
                    // ===================================================================================
                    // ===================================================================================
                    // Cleanup items no longer in the viewport
                    // ===================================================================================
                    // ===================================================================================
                    if (!remeasure)
                    {
                        if (IsVirtualizing)
                        {
                            if (InRecyclingMode)
                            {
                                DisconnectRecycledContainers();

                                if (visualOrderChanged)
                                {
                                    //
                                    // We moved some containers in the visual tree without firing
                                    // changed events.  ZOrder is now invalid.
                                    //
                                    InvalidateZState();
                                }
                            }
                            else
                            {
                                EnsureCleanupOperation(false /*delay*/);
                            }
                        }
                        HasVirtualizingChildren = hasVirtualizingChildren;

                        debug_AssertRealizedChildrenEqualVisualChildren();
                    }
                }
            }
            finally
            {
                if (etwTracingEnabled)
                {
                    EventTrace.EventProvider.TraceEvent(EventTrace.Event.WClientStringEnd, EventTrace.Keyword.KeywordGeneral, EventTrace.Level.Info, "VirtualizingStackPanel :MeasureOverride");
                }
            }

            if (remeasure)
            {
                //
                // Make another pass of MeasureOverride if remeasure is true.
                //
                return MeasureOverrideImpl(constraint, lastPageSafeOffset, ref previouslyMeasuredOffsets, remeasure);
            }
            else
            {
                return stackPixelSize;
            }
        }

        private Size MeasureNonItemsHost(Size constraint)
        {
            return StackPanel.StackMeasureHelper(this, _scrollData, constraint);
        }

        private Size ArrangeNonItemsHost(Size arrangeSize)
        {
            return StackPanel.StackArrangeHelper(this, _scrollData, arrangeSize);
        }

        /// <summary>
        /// Content arrangement.
        /// </summary>
        /// <param name="arrangeSize">Arrange size</param>
        protected override Size ArrangeOverride(Size arrangeSize)
        {
            bool etwTracingEnabled = IsScrolling && EventTrace.IsEnabled(EventTrace.Keyword.KeywordGeneral, EventTrace.Level.Info);
            if (etwTracingEnabled)
            {
                EventTrace.EventProvider.TraceEvent(EventTrace.Event.WClientStringBegin, EventTrace.Keyword.KeywordGeneral, EventTrace.Level.Info, "VirtualizingStackPanel :ArrangeOverride");
            }
            try
            {
                if (!IsItemsHost)
                {
                    ArrangeNonItemsHost(arrangeSize);
                }
                else
                {
                    // ===================================================================================
                    // ===================================================================================
                    // Fetch owners
                    // ===================================================================================
                    // ===================================================================================

                    ItemsControl itemsControl = null;
                    GroupItem groupItem = null;

                    //
                    // This is an interface implemented by the owner of this panel in order to facilitate between a parent
                    // panel and this one when virtualizing in a hierarchy. (Eg. TreeView or grouping ItemsControl.) This
                    // interface is currently implemented by TreeViewItem and GroupItem.
                    //
                    IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider = null;

                    //
                    // This is a service provided by the owner of this panel to store and retrieve information on a per item
                    // basis. Specifically this panel uses this service to remember DesiredSize of items when virtualizing.
                    // This interface is currently implemented by ItemsControl and GroupItem.
                    //
                    IContainItemStorage itemStorageProvider = null;

                    //
                    // This is the item representing the owner for this panel. (Eg. The CollectionViewGroup for the owner GroupItem)
                    //
                    object parentItem = null;

                    //
                    // Is horizontally stacking
                    //
                    bool isHorizontal = (Orientation == Orientation.Horizontal);

                    //
                    // Compute if this panel is different in orientation that either its parent or descendents
                    //
                    bool mustDisableVirtualization = false;

                    //
                    // Fetch the owner for this panel. That could either be an ItemsControl or a GroupItem.
                    //
                    GetOwners(false /*shouldSetVirtualizationState*/, isHorizontal, out itemsControl, out groupItem, out itemStorageProvider, out virtualizationInfoProvider, out parentItem, out mustDisableVirtualization);

                    // ===================================================================================
                    // ===================================================================================
                    // Get set to enumerate the items of the owner
                    // ===================================================================================
                    // ===================================================================================

                    EnsureGenerator();
                    IList children = RealizedChildren;  // yes, this is weird, but this property ensures the Generator is properly initialized.
                    IItemContainerGenerator generator = Generator;
                    IList items = ((ItemContainerGenerator)generator).ItemsInternal;
                    int itemCount = items.Count;

                    //
                    // Locally cache the values of this flag and size for better performance.
                    //
                    bool areContainersUniformlySized = GetAreContainersUniformlySized(itemStorageProvider, parentItem);
                    double uniformOrAverageContainerSize = GetUniformOrAverageContainerSize(itemStorageProvider, parentItem);

                    ScrollViewer scrollOwner = ScrollOwner;
                    double arrangeLength = 0;
                    if (scrollOwner != null && scrollOwner.CanContentScroll)
                    {
                        // If scollowner's CanContentScroll is true,
                        // loop through all the children and find the
                        // maximum desired size and arrange all the chilren
                        // with it.
                        arrangeLength = GetMaxChildArrangeLength(children, isHorizontal);
                    }

                    arrangeLength = Math.Max(isHorizontal ? arrangeSize.Height : arrangeSize.Width, arrangeLength);

                    // ===================================================================================
                    // ===================================================================================
                    // Arrange the children of this panel starting with the first item in the extended viewport
                    // ===================================================================================
                    // ===================================================================================

                    UIElement child = null;
                    Size childDesiredSize = Size.Empty;
                    Rect rcChild = new Rect(arrangeSize);

                    Size previousChildSize = new Size();
                    int previousChildItemIndex = -1;
                    Point previousChildOffset = new Point();

                    for (int i = _firstItemInExtendedViewportChildIndex; i < children.Count; ++i)
                    {
                        child = (UIElement)children[i];
                        childDesiredSize = child.DesiredSize;

                        if (i >= _firstItemInExtendedViewportChildIndex && i < _firstItemInExtendedViewportChildIndex + _actualItemsInExtendedViewportCount)
                        {
                            // ===================================================================================
                            // ===================================================================================
                            // Arrange the first item in the extended viewport
                            // ===================================================================================
                            // ===================================================================================

                            if (i == _firstItemInExtendedViewportChildIndex)
                            {
                                ArrangeFirstItemInExtendedViewport(
                                    isHorizontal,
                                    child,
                                    childDesiredSize,
                                    arrangeLength,
                                    ref rcChild,
                                    ref previousChildSize,
                                    ref previousChildOffset,
                                    ref previousChildItemIndex);

                                // ===================================================================================
                                // ===================================================================================
                                // Arrange the items before the extended viewport
                                // ===================================================================================
                                // ===================================================================================

                                UIElement containerBeforeViewport = null;
                                Size childSizeBeforeViewport = Size.Empty;
                                Rect rcChildBeforeViewport = rcChild;
                                Size previousChildSizeBeforeViewport = child.DesiredSize;
                                int previousChildItemIndexBeforeViewport = previousChildItemIndex;
                                Point previousChildOffsetBeforeViewport = previousChildOffset;

                                for (int j = _firstItemInExtendedViewportChildIndex - 1; j >= 0; j--)
                                {
                                    containerBeforeViewport = (UIElement)children[j];
                                    childSizeBeforeViewport = containerBeforeViewport.DesiredSize;

                                    ArrangeItemsBeyondTheExtendedViewport(
                                        isHorizontal,
                                        containerBeforeViewport,
                                        childSizeBeforeViewport,
                                        arrangeLength,
                                        items,
                                        generator,
                                        itemStorageProvider,
                                        areContainersUniformlySized,
                                        uniformOrAverageContainerSize,
                                        true /*beforeExtendedViewport*/,
                                        ref rcChildBeforeViewport,
                                        ref previousChildSizeBeforeViewport,
                                        ref previousChildOffsetBeforeViewport,
                                        ref previousChildItemIndexBeforeViewport);
                                }
                            }
                            else
                            {
                                // ===================================================================================
                                // ===================================================================================
                                // Arrange the other items within the extended viewport after the first
                                // ===================================================================================
                                // ===================================================================================

                                ArrangeOtherItemsInExtendedViewport(
                                    isHorizontal,
                                    child,
                                    childDesiredSize,
                                    arrangeLength,
                                    i,
                                    ref rcChild,
                                    ref previousChildSize,
                                    ref previousChildOffset,
                                    ref previousChildItemIndex);
                            }
                        }
                        else
                        {
                            // ===================================================================================
                            // ===================================================================================
                            // Arrange the items after the extended viewport
                            // ===================================================================================
                            // ===================================================================================

                            ArrangeItemsBeyondTheExtendedViewport(
                                isHorizontal,
                                child,
                                childDesiredSize,
                                arrangeLength,
                                items,
                                generator,
                                itemStorageProvider,
                                areContainersUniformlySized,
                                uniformOrAverageContainerSize,
                                false /*beforeExtendedViewport*/,
                                ref rcChild,
                                ref previousChildSize,
                                ref previousChildOffset,
                                ref previousChildItemIndex);
                        }
                    }
                }
            }
            finally
            {
                if (etwTracingEnabled)
                {
                    EventTrace.EventProvider.TraceEvent(EventTrace.Event.WClientStringEnd, EventTrace.Keyword.KeywordGeneral, EventTrace.Level.Info, "VirtualizingStackPanel :ArrangeOverride");
                }
            }

            return arrangeSize;
        }

        /// <summary>
        ///     Called when the Items collection associated with the containing ItemsControl changes.
        /// </summary>
        /// <param name="sender">sender</param>
        /// <param name="args">Event arguments</param>
        protected override void OnItemsChanged(object sender, ItemsChangedEventArgs args)
        {
            if (MeasureInProgress)
            {
                // If the Items collection changes during Measure, the measure state
                // local to MeasureOverrideImpl is invalid and we need to start over.
                // This is an unusual situation, but it can happen if the process
                // of linking the container to its item has a side-effect of adding
                // or removing items to the underlying collection.

                // In Dev11 bug 204054 this occurs in VS SolutionNavigator, when
                // VirtualizingTreeView.PrepareContainerForItemOverride binds a
                // PivotTreeViewItem to the HasItems property of a VirtualizingTreeView+TreeNode.
                // The property-getter for HasItems can invoke an inline task that
                // adds/removes items.

                ItemsChangedDuringMeasure = true;
            }

            base.OnItemsChanged(sender, args);

            bool resetMaximumDesiredSize = false;

            switch (args.Action)
            {
                case NotifyCollectionChangedAction.Remove:
                    OnItemsRemove(args);
                    resetMaximumDesiredSize = true;
                    break;

                case NotifyCollectionChangedAction.Replace:
                    OnItemsReplace(args);
                    resetMaximumDesiredSize = true;
                    break;

                case NotifyCollectionChangedAction.Move:
                    OnItemsMove(args);
                    break;

                case NotifyCollectionChangedAction.Reset:
                    resetMaximumDesiredSize = true;

                    IContainItemStorage itemStorageProvider = GetItemStorageProvider(this);
                    itemStorageProvider.Clear();

                    ClearAsyncOperations();

                    break;
            }

            if (resetMaximumDesiredSize && IsScrolling)
            {
                // The items changed such that the maximum size may no longer be valid.
                // The next layout pass will update this value.
                _scrollData._maxDesiredSize = new Size();
            }

        }

        /// <summary>
        ///     Returns whether an Items collection change affects layout for this panel.
        /// </summary>
        /// <param name="args">Event arguments</param>
        /// <param name="areItemChangesLocal">Says if this notification represents a direct change to this Panel's collection</param>
        protected override bool ShouldItemsChangeAffectLayoutCore(bool areItemChangesLocal, ItemsChangedEventArgs args)
        {
            bool shouldItemsChangeAffectLayout = true;

            if (IsVirtualizing)
            {
                if (areItemChangesLocal)
                {
                    //
                    // Check if the indices being mutated lie beyond the currently generated indices.
                    // Please note that mutations prior to the currently generated viewport affect the
                    // start element within the viewport and hence necessitates a layout update. This
                    // is the reason we only consider mutations after the generated viewport for this
                    // optimization.
                    //

                    switch (args.Action)
                    {
                        case NotifyCollectionChangedAction.Remove:
                            {
                                int startOldIndex = Generator.IndexFromGeneratorPosition(args.OldPosition);

                                shouldItemsChangeAffectLayout = args.ItemUICount > 0 ||
                                    (startOldIndex < _firstItemInExtendedViewportIndex + _itemsInExtendedViewportCount);
                            }
                            break;

                        case NotifyCollectionChangedAction.Replace:
                            {
                                shouldItemsChangeAffectLayout = args.ItemUICount > 0;
                            }
                            break;

                        case NotifyCollectionChangedAction.Add:
                            {
                                int startIndex = Generator.IndexFromGeneratorPosition(args.Position);

                                shouldItemsChangeAffectLayout =
                                    (startIndex < _firstItemInExtendedViewportIndex + _itemsInExtendedViewportCount);
                            }
                            break;

                        case NotifyCollectionChangedAction.Move:
                            {
                                int startIndex = Generator.IndexFromGeneratorPosition(args.Position);
                                int startOldIndex = Generator.IndexFromGeneratorPosition(args.OldPosition);

                                shouldItemsChangeAffectLayout =
                                    ((startIndex < _firstItemInExtendedViewportIndex + _itemsInExtendedViewportCount) ||
                                     (startOldIndex < _firstItemInExtendedViewportIndex + _itemsInExtendedViewportCount));
                            }
                            break;
                    }
                }
                else
                {
                    //
                    // Given that this isnt the collection being directly manipulated, we check to see if the
                    // index affected is the last one generated. Consider the following example.
                    //
                    // Grp1
                    //  1
                    //  2
                    //  3
                    // Grp2
                    //  4
                    //  5
                    //  6
                    //
                    // Now if item 7 gets added to Grp1, even though 7 is beyond the currently generated items
                    // within Grp1, Grp1 is not the last entity within the viewport. Hence we need a layout update
                    // here. Conversely if item 7 were added to Grp2, then 7 is both beyond the currently generated
                    // range for Grp2 and also beyond the overall viewport because Grp2 happens to be last
                    // generated container within its parent panel.
                    //

                    Debug.Assert(args.Action == NotifyCollectionChangedAction.Reset && args.ItemCount == 1);

                    int startIndex = Generator.IndexFromGeneratorPosition(args.Position);

                    shouldItemsChangeAffectLayout =
                        (startIndex != _firstItemInExtendedViewportIndex + _itemsInExtendedViewportCount - 1);
                }

                if (!shouldItemsChangeAffectLayout)
                {
                    if (IsScrolling)
                    {
                        //
                        // If this is the scrolling panel we finally need to ensure that the viewport is currently
                        // fully occupied to sanction is optimization. Because if it isnt then any collection mutations
                        // show in the viewport by default and thus need a layout update.
                        //

                        shouldItemsChangeAffectLayout = !IsExtendedViewportFull();

                        if (!shouldItemsChangeAffectLayout)
                        {
                            //
                            // If we've passed the earlier check we attempt a surgical update to the scroll extent
                            //

                            UpdateExtent(areItemChangesLocal);
                        }
                    }
                    else
                    {
                        //
                        // If this isnt the scrolling panel then we need to recursively check parent panels
                        //

                        DependencyObject itemsOwner = ItemsControl.GetItemsOwnerInternal(this);
                        VirtualizingPanel vp = VisualTreeHelper.GetParent(itemsOwner) as VirtualizingPanel;
                        if (vp != null)
                        {
                            //
                            // In hierarchical scenarios we must update the extent at each descendent level before we recurse
                            // to the level higher so that the level higher gets to synchronize its uniform size flags based upon
                            // this update.
                            //

                            UpdateExtent(areItemChangesLocal);

                            IItemContainerGenerator generator = vp.ItemContainerGenerator;
                            int index = ((ItemContainerGenerator)generator).IndexFromContainer(itemsOwner, true /*returnLocalIndex*/);
                            ItemsChangedEventArgs newArgs = new ItemsChangedEventArgs(NotifyCollectionChangedAction.Reset,
                                generator.GeneratorPositionFromIndex(index), 1, 1);

                            shouldItemsChangeAffectLayout = vp.ShouldItemsChangeAffectLayout(false /*areItemChangesLocal*/, newArgs);
                        }
                        else
                        {
                            //
                            // If we arent able to find VirtualizingPanels to check, then we must default to updating layout
                            //

                            shouldItemsChangeAffectLayout = true;
                        }
                    }
                }
            }

            return shouldItemsChangeAffectLayout;
        }

        private void UpdateExtent(bool areItemChangesLocal)
        {
            bool isHorizontal = (Orientation == Orientation.Horizontal);

            ItemsControl itemsControl;
            GroupItem groupItem;
            IContainItemStorage itemStorageProvider;
            IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider;
            object parentItem;
            bool mustDisableVirtualization;

            GetOwners(false /*shouldSetVirtualizationState*/, isHorizontal,
                out itemsControl, out groupItem, out itemStorageProvider,
                out virtualizationInfoProvider, out parentItem, out mustDisableVirtualization);

            double uniformOrAverageContainerSize = GetUniformOrAverageContainerSize(itemStorageProvider, parentItem);
            bool areContainersUniformlySized = GetAreContainersUniformlySized(itemStorageProvider, parentItem);

            IList children = RealizedChildren;
            IItemContainerGenerator generator = Generator;
            IList items = ((ItemContainerGenerator)generator).ItemsInternal;
            int itemCount = items.Count;

            if (!areItemChangesLocal)
            {
                //
                // If the actual item changes arent local to this panel then we need to [....]
                // the flags for this panel to make sure we gather size updates from
                // descendent panel that actually contained the collection changes.
                //
                double computedUniformOrAverageContainerSize = uniformOrAverageContainerSize;
                bool computedAreContainersUniformlySized = areContainersUniformlySized;

                SyncUniformSizeFlags(
                    parentItem,
                    children,
                    items,
                    itemStorageProvider,
                    itemCount,
                    computedAreContainersUniformlySized,
                    computedUniformOrAverageContainerSize,
                    ref areContainersUniformlySized,
                    ref uniformOrAverageContainerSize,
                    isHorizontal,
                    true /* evaluateAreContainersUniformlySized */);
            }

            double distance = 0;
            ComputeDistance(items, itemStorageProvider, isHorizontal,
                areContainersUniformlySized,
                uniformOrAverageContainerSize,
                0, items.Count, out distance);

            if (IsScrolling)
            {
                if (isHorizontal)
                {
                    _scrollData._extent.Width = distance;
                }
                else
                {
                    _scrollData._extent.Height = distance;
                }

                ScrollOwner.InvalidateScrollInfo();
            }
            else if (virtualizationInfoProvider != null)
            {
                HierarchicalVirtualizationItemDesiredSizes itemDesiredSizes = virtualizationInfoProvider.ItemDesiredSizes;

                if (IsPixelBased)
                {
                    Size pixelSize = itemDesiredSizes.PixelSize;
                    if (isHorizontal)
                    {
                        pixelSize.Width = distance;
                    }
                    else
                    {
                        pixelSize.Height = distance;
                    }

                    itemDesiredSizes = new HierarchicalVirtualizationItemDesiredSizes(
                        itemDesiredSizes.LogicalSize,
                        itemDesiredSizes.LogicalSizeInViewport,
                        itemDesiredSizes.LogicalSizeBeforeViewport,
                        itemDesiredSizes.LogicalSizeAfterViewport,
                        pixelSize,
                        itemDesiredSizes.PixelSizeInViewport,
                        itemDesiredSizes.PixelSizeBeforeViewport,
                        itemDesiredSizes.PixelSizeAfterViewport);
                }
                else
                {
                    Size logicalSize = itemDesiredSizes.LogicalSize;
                    if (isHorizontal)
                    {
                        logicalSize.Width = distance;
                    }
                    else
                    {
                        logicalSize.Height = distance;
                    }

                    itemDesiredSizes = new HierarchicalVirtualizationItemDesiredSizes(
                        logicalSize,
                        itemDesiredSizes.LogicalSizeInViewport,
                        itemDesiredSizes.LogicalSizeBeforeViewport,
                        itemDesiredSizes.LogicalSizeAfterViewport,
                        itemDesiredSizes.PixelSize,
                        itemDesiredSizes.PixelSizeInViewport,
                        itemDesiredSizes.PixelSizeBeforeViewport,
                        itemDesiredSizes.PixelSizeAfterViewport);
                }

                virtualizationInfoProvider.ItemDesiredSizes = itemDesiredSizes;
            }
        }

        private bool IsExtendedViewportFull()
        {
            Debug.Assert(IsScrolling && IsVirtualizing, "Only check viewport on scrolling panel when virtualizing");

            bool isHorizontal = (Orientation == Orientation.Horizontal);

            bool isViewportFull =
                ((isHorizontal && DoubleUtil.GreaterThanOrClose(DesiredSize.Width, PreviousConstraint.Width)) ||
                 (!isHorizontal && DoubleUtil.GreaterThanOrClose(DesiredSize.Height, PreviousConstraint.Height)));

            if (isViewportFull)
            {
                IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider = null;
                Rect viewport = _viewport;
                Rect currentExtendedViewport = _extendedViewport;
                Rect estimatedExtendedViewport = Rect.Empty;
                VirtualizationCacheLength cacheLength = VirtualizingPanel.GetCacheLength(this);
                VirtualizationCacheLengthUnit cacheUnit = VirtualizingPanel.GetCacheLengthUnit(this);
                int itemsInExtendedViewportCount = _itemsInExtendedViewportCount;

                NormalizeCacheLength(isHorizontal, viewport, ref cacheLength, ref cacheUnit);

                estimatedExtendedViewport = ExtendViewport(
                    virtualizationInfoProvider,
                    isHorizontal,
                    viewport,
                    cacheLength,
                    cacheUnit,
                    Size.Empty,
                    Size.Empty,
                    Size.Empty,
                    Size.Empty,
                    Size.Empty,
                    Size.Empty,
                    ref itemsInExtendedViewportCount);

                return ((isHorizontal && DoubleUtil.GreaterThanOrClose(currentExtendedViewport.Width, estimatedExtendedViewport.Width)) ||
                        (!isHorizontal && DoubleUtil.GreaterThanOrClose(currentExtendedViewport.Height, estimatedExtendedViewport.Height)));
            }

            return false;
        }

        /// <summary>
        ///     Called when the UI collection of children is cleared by the base Panel class.
        /// </summary>
        protected override void OnClearChildren()
        {
            base.OnClearChildren();

            if (IsVirtualizing && IsItemsHost)
            {
                ItemsControl itemsControl;
                ItemsControl.GetItemsOwnerInternal(this, out itemsControl);

                CleanupContainers(Int32.MaxValue, Int32.MaxValue, itemsControl);
            }

            if (_realizedChildren != null)
            {
                _realizedChildren.Clear();
            }

            InternalChildren.ClearInternal();
        }

        #endregion Protected Methods

        #region Internal Methods

        private void OnIsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if (!(bool)e.NewValue)
            {
                IHierarchicalVirtualizationAndScrollInfo virtualizingProvider = GetVirtualizingProvider();
                if (virtualizingProvider != null)
                {
                    Helper.ClearVirtualizingElement(virtualizingProvider);
                }

                ClearAsyncOperations();
            }
            else
            {
                // We now depend upon the IsVisible state of the panel in a number of places.
                // So it is required that we relayout when the panel is made visible.
                InvalidateMeasure();
            }
        }

        // Tells the Generator to clear out all containers for this ItemsControl.  This is called by the ItemValueStorage
        // service when the ItemsControl this panel is a host for is about to be thrown away.  This allows the VSP to save
        // off any properties it is interested in and results in a call to ClearContainerForItem on the ItemsControl, allowing
        // the Item Container Storage to do so as well.

        // Note: A possible perf improvement may be to make 'fast' RemoveAll on the Generator that simply calls ClearContainerForItem
        // for us without walking through its data structures to actually clean out items.
        internal void ClearAllContainers()
        {
            IItemContainerGenerator generator = Generator;
            if (generator != null)
            {
                generator.RemoveAll();
            }
        }

        #endregion

        //
        // MeasureOverride Helpers
        //

        #region MeasureOverride Helpers

        private IHierarchicalVirtualizationAndScrollInfo GetVirtualizingProvider()
        {
            ItemsControl itemsControl = null;
            DependencyObject itemsOwner = ItemsControl.GetItemsOwnerInternal(this, out itemsControl);
            if (itemsOwner is GroupItem)
            {
                return GetVirtualizingProvider(itemsOwner);
            }
            return GetVirtualizingProvider(itemsControl);
        }

        private static IHierarchicalVirtualizationAndScrollInfo GetVirtualizingProvider(DependencyObject element)
        {
            IHierarchicalVirtualizationAndScrollInfo virtualizingProvider = element as IHierarchicalVirtualizationAndScrollInfo;
            if (virtualizingProvider != null)
            {
                VirtualizingPanel virtualizingPanel = VisualTreeHelper.GetParent(element) as VirtualizingPanel;
                if (virtualizingPanel == null || !virtualizingPanel.CanHierarchicallyScrollAndVirtualize)
                {
                    virtualizingProvider = null;
                }
            }

            return virtualizingProvider;
        }

        private static IHierarchicalVirtualizationAndScrollInfo GetVirtualizingChild(DependencyObject element)
        {
            bool isChildHorizontal = false;
            return GetVirtualizingChild(element, ref isChildHorizontal);
        }

        private static IHierarchicalVirtualizationAndScrollInfo GetVirtualizingChild(DependencyObject element, ref bool isChildHorizontal)
        {
            IHierarchicalVirtualizationAndScrollInfo virtualizingChild = element as IHierarchicalVirtualizationAndScrollInfo;
            if (virtualizingChild != null && virtualizingChild.ItemsHost != null)
            {
                isChildHorizontal = (virtualizingChild.ItemsHost.LogicalOrientationPublic == Orientation.Horizontal);

                VirtualizingPanel virtualizingPanel = virtualizingChild.ItemsHost as VirtualizingPanel;
                if (virtualizingPanel == null || !virtualizingPanel.CanHierarchicallyScrollAndVirtualize)
                {
                    virtualizingChild = null;
                }
            }

            return virtualizingChild;
        }


        /// <summary>
        /// Initializes the owner and interfaces for the virtualization services it supports.
        /// </summary>
        private static IContainItemStorage GetItemStorageProvider(Panel itemsHost)
        {
            ItemsControl itemsControl = null;
            GroupItem groupItem = null;

            DependencyObject itemsOwner = ItemsControl.GetItemsOwnerInternal(itemsHost, out itemsControl);
            if (itemsOwner != itemsControl)
            {
                groupItem = itemsOwner as GroupItem;
            }

            return itemsOwner as IContainItemStorage;
        }

        /// <summary>
        /// Initializes the owner and interfaces for the virtualization services it supports.
        /// </summary>
        private void GetOwners(
            bool shouldSetVirtualizationState,
            bool isHorizontal,
            out ItemsControl itemsControl,
            out GroupItem groupItem,
            out IContainItemStorage itemStorageProvider,
            out IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider,
            out object parentItem,
            out bool mustDisableVirtualization)
        {
            groupItem = null;
            parentItem = null;

            bool isScrolling = IsScrolling;

            mustDisableVirtualization = isScrolling ? MustDisableVirtualization : false;

            DependencyObject itemsOwner = ItemsControl.GetItemsOwnerInternal(this, out itemsControl);
            if (itemsOwner != itemsControl)
            {
                groupItem = itemsOwner as GroupItem;
                parentItem = itemsControl.ItemContainerGenerator.ItemFromContainer(groupItem);
            }
            else if (!isScrolling)
            {
                ItemsControl parentItemsControl = ItemsControl.GetItemsOwnerInternal(VisualTreeHelper.GetParent(itemsControl)) as ItemsControl;
                if (parentItemsControl != null)
                {
                    parentItem = parentItemsControl.ItemContainerGenerator.ItemFromContainer(itemsControl);
                }
                else
                {
                    parentItem = this;
                }
            }
            else
            {
                parentItem = this;
            }

            itemStorageProvider = itemsOwner as IContainItemStorage;
            virtualizationInfoProvider = null;

            if (groupItem != null)
            {
                virtualizationInfoProvider = GetVirtualizingProvider(groupItem);
                mustDisableVirtualization = virtualizationInfoProvider != null ? virtualizationInfoProvider.MustDisableVirtualization : false;
            }
            else if (!isScrolling)
            {
                virtualizationInfoProvider = GetVirtualizingProvider(itemsControl);
                mustDisableVirtualization = virtualizationInfoProvider != null ? virtualizationInfoProvider.MustDisableVirtualization : false;
            }

            if (shouldSetVirtualizationState)
            {
                //
                // Synchronize properties such as IsVirtualizing, IsRecycling & IsPixelBased
                //
                SetVirtualizationState(itemStorageProvider, itemsControl, mustDisableVirtualization);
            }
        }

        /// <summary>
        /// Sets up IsVirtualizing, VirtualizationMode, and IsPixelBased
        /// </summary>
        private void SetVirtualizationState(
            IContainItemStorage itemStorageProvider,
            ItemsControl itemsControl,
            bool mustDisableVirtualization)
        {
            if (itemsControl != null)
            {
                bool isVirtualizing = GetIsVirtualizing(itemsControl);
                bool isVirtualizingWhenGrouping = GetIsVirtualizingWhenGrouping(itemsControl);
                VirtualizationMode virtualizationMode = GetVirtualizationMode(itemsControl);
                bool isGrouping = itemsControl.IsGrouping;
                IsVirtualizing = !mustDisableVirtualization && ((!isGrouping && isVirtualizing) || (isGrouping && isVirtualizing && isVirtualizingWhenGrouping));

                ScrollUnit scrollUnit = GetScrollUnit(itemsControl);
                bool oldIsPixelBased = IsPixelBased;
                IsPixelBased = mustDisableVirtualization || (scrollUnit == ScrollUnit.Pixel);
                if (IsScrolling)
                {
                    if (!HasMeasured || oldIsPixelBased != IsPixelBased)
                    {
                        ClearItemValueStorageRecursive(itemStorageProvider, this);
                    }

                    SetCacheLength(this, GetCacheLength(itemsControl));
                    SetCacheLengthUnit(this, GetCacheLengthUnit(itemsControl));
                }

                //
                // Set up info on first measure
                //
                if (HasMeasured)
                {
                    VirtualizationMode oldVirtualizationMode = InRecyclingMode ? VirtualizationMode.Recycling : VirtualizationMode.Standard;
                    if (oldVirtualizationMode != virtualizationMode)
                    {
                        throw new InvalidOperationException(SR.Get(SRID.CantSwitchVirtualizationModePostMeasure));
                    }
                }
                else
                {
                    HasMeasured = true;
                }

                InRecyclingMode = (virtualizationMode == VirtualizationMode.Recycling);
            }
        }

        private static void ClearItemValueStorageRecursive(IContainItemStorage itemStorageProvider, Panel itemsHost)
        {
            Helper.ClearItemValueStorage((DependencyObject)itemStorageProvider, _indicesStoredInItemValueStorage);

            UIElementCollection children = itemsHost.InternalChildren;
            int childrenCount = children.Count;
            for (int i=0; i<childrenCount; i++)
            {
                IHierarchicalVirtualizationAndScrollInfo virtualizingChild = children[i] as IHierarchicalVirtualizationAndScrollInfo;
                if (virtualizingChild != null)
                {
                    Panel childItemsHost = virtualizingChild.ItemsHost;
                    if (childItemsHost != null)
                    {
                        IContainItemStorage childItemStorageProvider = GetItemStorageProvider(childItemsHost);
                        if (childItemStorageProvider != null)
                        {
                            ClearItemValueStorageRecursive(childItemStorageProvider, childItemsHost);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Initializes the viewport for this panel.
        /// </summary>
        private void InitializeViewport(
            IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider,
            bool isHorizontal,
            Size constraint,
            ref Rect viewport,
            ref VirtualizationCacheLength cacheSize,
            ref VirtualizationCacheLengthUnit cacheUnit,
            out Rect extendedViewport)
        {

            Size extent = new Size();

            if (IsScrolling)
            {
                //
                // We're the top level scrolling panel. Fetch the offset from the _scrollData.
                //

                Size size;
                double offsetX, offsetY;
                Size viewportSize;

                size = constraint;
                offsetX = _scrollData._offset.X;
                offsetY = _scrollData._offset.Y;
                extent = _scrollData._extent;
                viewportSize = _scrollData._viewport;

                if (!IsScrollActive || IgnoreMaxDesiredSize)
                {
                    _scrollData._maxDesiredSize = new Size();
                }

                if (IsPixelBased)
                {
                    viewport = new Rect(offsetX, offsetY, size.Width, size.Height);
                    CoerceScrollingViewportOffset(ref viewport, extent, isHorizontal);
                }
                else
                {
                    viewport = new Rect(offsetX, offsetY, viewportSize.Width, viewportSize.Height);
                    CoerceScrollingViewportOffset(ref viewport, extent, isHorizontal);
                    viewport.Size = size;
                }

                if (IsVirtualizing)
                {
                    cacheSize = VirtualizingStackPanel.GetCacheLength(this);
                    cacheUnit = VirtualizingStackPanel.GetCacheLengthUnit(this);

                    if (DoubleUtil.GreaterThan(cacheSize.CacheBeforeViewport, 0) ||
                        DoubleUtil.GreaterThan(cacheSize.CacheAfterViewport, 0))
                    {
                        if (!MeasureCaches)
                        {
                            WasLastMeasurePassAnchored = (_firstContainerInViewport != null) || (_bringIntoViewLeafContainer != null);

                            DispatcherOperation measureCachesOperation = MeasureCachesOperationField.GetValue(this);
                            if (measureCachesOperation == null)
                            {
                                measureCachesOperation = Dispatcher.BeginInvoke(DispatcherPriority.Background,
                                    (Action)delegate()
                                    {
                                        try
                                        {
                                            MeasureCachesOperationField.ClearValue(this);

                                            MeasureCaches = true;

                                            if (WasLastMeasurePassAnchored)
                                            {
                                                SetAnchorInformation(isHorizontal);
                                            }

                                            InvalidateMeasure();
                                            UpdateLayout();
                                        }
                                        finally
                                        {
                                            MeasureCaches = false;

                                            // If there is a pending anchor operation that got registered in
                                            // the current pass, we dont want to clear the IsScrollActive flag.
                                            // We should allow that measure pass to also settle and then clear
                                            // the flag.

                                            DispatcherOperation anchoredInvalidateMeasureOperation = AnchoredInvalidateMeasureOperationField.GetValue(this);
                                            if (anchoredInvalidateMeasureOperation == null)
                                            {
                                                IsScrollActive = false;
                                            }
                                        }
                                    });

                                MeasureCachesOperationField.SetValue(this, measureCachesOperation);
                            }
                        }
                    }
                    else if (IsScrollActive)
                    {
                        DispatcherOperation clearIsScrollActiveOperation = ClearIsScrollActiveOperationField.GetValue(this);
                        if (clearIsScrollActiveOperation == null)
                        {
                            clearIsScrollActiveOperation = Dispatcher.BeginInvoke(DispatcherPriority.Background,
                                (Action)delegate()
                                {
                                    ClearIsScrollActiveOperationField.ClearValue(this);
                                    IsScrollActive = false;
                                });

                            ClearIsScrollActiveOperationField.SetValue(this, clearIsScrollActiveOperation);
                        }
                    }

                    NormalizeCacheLength(isHorizontal, viewport, ref cacheSize, ref cacheUnit);
                }
                else
                {
                    cacheSize = new VirtualizationCacheLength(
                        Double.PositiveInfinity,
                        IsViewportEmpty(isHorizontal, viewport) ?
                        0.0 :
                        Double.PositiveInfinity);
                    cacheUnit = VirtualizationCacheLengthUnit.Pixel;

                    ClearAsyncOperations();
                }
            }
            else if (virtualizationInfoProvider != null)
            {
                //
                // Adjust the viewport offset for a non scrolling panel to account for the HeaderSize
                // when virtualizing.
                //
                HierarchicalVirtualizationConstraints virtualizationConstraints = virtualizationInfoProvider.Constraints;
                viewport = virtualizationConstraints.Viewport;
                cacheSize = virtualizationConstraints.CacheLength;
                cacheUnit = virtualizationConstraints.CacheLengthUnit;
                MeasureCaches = virtualizationInfoProvider.InBackgroundLayout;

                AdjustNonScrollingViewportForHeader(virtualizationInfoProvider, ref viewport, ref cacheSize, ref cacheUnit);
            }
            else
            {
                viewport = new Rect(0, 0, constraint.Width, constraint.Height);

                if (isHorizontal)
                {
                    viewport.Width = Double.PositiveInfinity;
                }
                else
                {
                    viewport.Height = Double.PositiveInfinity;
                }
            }

            // Adjust extendedViewport

            extendedViewport = _extendedViewport;

            if (isHorizontal)
            {
                extendedViewport.X += viewport.X - _viewport.X;
            }
            else
            {
                extendedViewport.Y += viewport.Y - _viewport.Y;
            }
        }

        private void NormalizeCacheLength(
            bool isHorizontal,
            Rect viewport,
            ref VirtualizationCacheLength cacheLength,
            ref VirtualizationCacheLengthUnit cacheUnit)
        {
            if (cacheUnit == VirtualizationCacheLengthUnit.Page)
            {
                double factor = isHorizontal ? viewport.Width : viewport.Height;

                if (Double.IsPositiveInfinity(factor))
                {
                    cacheLength = new VirtualizationCacheLength(
                        0,
                        0);
                }
                else
                {
                    cacheLength = new VirtualizationCacheLength(
                        cacheLength.CacheBeforeViewport * factor,
                        cacheLength.CacheAfterViewport * factor);
                }

                cacheUnit = VirtualizationCacheLengthUnit.Pixel;
            }
        }

        /// <summary>
        /// Extends the viewport to include the cacheSizeBeforeViewport and cacheSizeAfterViewport.
        /// </summary>
        private Rect ExtendViewport(
            IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider,
            bool isHorizontal,
            Rect viewport,
            VirtualizationCacheLength cacheLength,
            VirtualizationCacheLengthUnit cacheUnit,
            Size stackPixelSizeInCacheBeforeViewport,
            Size stackLogicalSizeInCacheBeforeViewport,
            Size stackPixelSizeInCacheAfterViewport,
            Size stackLogicalSizeInCacheAfterViewport,
            Size stackPixelSize,
            Size stackLogicalSize,
            ref int itemsInExtendedViewportCount)
        {
            Debug.Assert(cacheUnit != VirtualizationCacheLengthUnit.Page, "Page cacheUnit is not expected here.");

            double pixelSize, pixelSizeBeforeViewport, pixelSizeAfterViewport;
            double logicalSize, logicalSizeBeforeViewport, logicalSizeAfterViewport;
            Rect extendedViewport = viewport;

            if (isHorizontal)
            {
                double approxSizeOfLogicalUnit = (DoubleUtil.GreaterThan(_previousStackPixelSizeInViewport.Width, 0.0) && DoubleUtil.GreaterThan(_previousStackLogicalSizeInViewport.Width, 0.0)) ?
                    _previousStackPixelSizeInViewport.Width / _previousStackLogicalSizeInViewport.Width : ScrollViewer._scrollLineDelta;

                pixelSize = stackPixelSize.Width;
                logicalSize = stackLogicalSize.Width;

                if (MeasureCaches)
                {
                    pixelSizeBeforeViewport = stackPixelSizeInCacheBeforeViewport.Width;
                    pixelSizeAfterViewport = stackPixelSizeInCacheAfterViewport.Width;
                    logicalSizeBeforeViewport = stackLogicalSizeInCacheBeforeViewport.Width;
                    logicalSizeAfterViewport = stackLogicalSizeInCacheAfterViewport.Width;
                }
                else
                {
                    pixelSizeBeforeViewport = (cacheUnit == VirtualizationCacheLengthUnit.Item) ? cacheLength.CacheBeforeViewport * approxSizeOfLogicalUnit : cacheLength.CacheBeforeViewport;
                    pixelSizeAfterViewport = (cacheUnit == VirtualizationCacheLengthUnit.Item) ? cacheLength.CacheAfterViewport * approxSizeOfLogicalUnit : cacheLength.CacheAfterViewport;
                    logicalSizeBeforeViewport = (cacheUnit == VirtualizationCacheLengthUnit.Item) ? cacheLength.CacheBeforeViewport : cacheLength.CacheBeforeViewport / approxSizeOfLogicalUnit;
                    logicalSizeAfterViewport = (cacheUnit == VirtualizationCacheLengthUnit.Item) ? cacheLength.CacheAfterViewport : cacheLength.CacheAfterViewport / approxSizeOfLogicalUnit;

                    if (IsPixelBased)
                    {
                        pixelSizeBeforeViewport = Math.Max(pixelSizeBeforeViewport, Math.Abs(_viewport.X - _extendedViewport.X));
                    }
                    else
                    {
                        logicalSizeBeforeViewport = Math.Max(logicalSizeBeforeViewport, Math.Abs(_viewport.X - _extendedViewport.X));
                    }
                }

                if (IsPixelBased)
                {
                    if (!IsScrolling && virtualizationInfoProvider != null &&
                        IsViewportEmpty(isHorizontal, extendedViewport) &&
                        DoubleUtil.GreaterThan(pixelSizeBeforeViewport, 0))
                    {
                        //
                        // If this is a GroupItem or a TreeViewItem that is completely above the viewport,
                        // then the CacheBeforeViewport allways designates the distance of the bottom of
                        // this panel from the top of the extendedViewport. Hence this computation for the offset.
                        //

                        extendedViewport.X = pixelSize - pixelSizeBeforeViewport;
                    }
                    else
                    {
                        extendedViewport.X -= pixelSizeBeforeViewport;
                    }

                    extendedViewport.Width += pixelSizeBeforeViewport + pixelSizeAfterViewport;

                    //
                    // Once again coerce the extended viewport dimensions to be within valid range.
                    //
                    if (IsScrolling)
                    {
                        if (DoubleUtil.LessThan(extendedViewport.X, 0.0))
                        {
                            extendedViewport.Width = Math.Max(extendedViewport.Width + extendedViewport.X, 0.0);
                            extendedViewport.X = 0.0;
                        }

                        if (DoubleUtil.GreaterThan(extendedViewport.X + extendedViewport.Width, _scrollData._extent.Width))
                        {
                            extendedViewport.Width = _scrollData._extent.Width - extendedViewport.X;
                        }
                    }
                }
                else
                {
                    if (!IsScrolling && virtualizationInfoProvider != null &&
                        IsViewportEmpty(isHorizontal, extendedViewport) &&
                        DoubleUtil.GreaterThan(pixelSizeBeforeViewport, 0))
                    {
                        //
                        // If this is a GroupItem or a TreeViewItem that is completely above the viewport,
                        // then the CacheBeforeViewport allways designates the distance of the bottom of
                        // this panel from the top of the extendedViewport. Hence this computation for the offset.
                        //

                        extendedViewport.X = logicalSize - logicalSizeBeforeViewport;
                    }
                    else
                    {
                        extendedViewport.X -= logicalSizeBeforeViewport;
                    }

                    extendedViewport.Width += pixelSizeBeforeViewport + pixelSizeAfterViewport;

                    if (IsScrolling)
                    {
                        if (DoubleUtil.LessThan(extendedViewport.X, 0.0))
                        {
                            extendedViewport.Width = Math.Max(extendedViewport.Width / approxSizeOfLogicalUnit + extendedViewport.X, 0.0) * approxSizeOfLogicalUnit;
                            extendedViewport.X = 0.0;
                        }

                        if (DoubleUtil.GreaterThan(extendedViewport.X + extendedViewport.Width / approxSizeOfLogicalUnit, _scrollData._extent.Width))
                        {
                            extendedViewport.Width = (_scrollData._extent.Width - extendedViewport.X) * approxSizeOfLogicalUnit;
                        }
                    }
                }
            }
            else
            {
                double approxSizeOfLogicalUnit = (DoubleUtil.GreaterThan(_previousStackPixelSizeInViewport.Height, 0.0) && DoubleUtil.GreaterThan(_previousStackLogicalSizeInViewport.Height, 0.0)) ?
                    _previousStackPixelSizeInViewport.Height / _previousStackLogicalSizeInViewport.Height : ScrollViewer._scrollLineDelta;

                pixelSize = stackPixelSize.Height;
                logicalSize = stackLogicalSize.Height;

                if (MeasureCaches)
                {
                    pixelSizeBeforeViewport = stackPixelSizeInCacheBeforeViewport.Height;
                    pixelSizeAfterViewport = stackPixelSizeInCacheAfterViewport.Height;
                    logicalSizeBeforeViewport = stackLogicalSizeInCacheBeforeViewport.Height;
                    logicalSizeAfterViewport = stackLogicalSizeInCacheAfterViewport.Height;
                }
                else
                {
                    pixelSizeBeforeViewport = (cacheUnit == VirtualizationCacheLengthUnit.Item) ? cacheLength.CacheBeforeViewport * approxSizeOfLogicalUnit : cacheLength.CacheBeforeViewport;
                    pixelSizeAfterViewport = (cacheUnit == VirtualizationCacheLengthUnit.Item) ? cacheLength.CacheAfterViewport * approxSizeOfLogicalUnit : cacheLength.CacheAfterViewport;
                    logicalSizeBeforeViewport = (cacheUnit == VirtualizationCacheLengthUnit.Item) ? cacheLength.CacheBeforeViewport : cacheLength.CacheBeforeViewport / approxSizeOfLogicalUnit;
                    logicalSizeAfterViewport = (cacheUnit == VirtualizationCacheLengthUnit.Item) ? cacheLength.CacheAfterViewport : cacheLength.CacheAfterViewport / approxSizeOfLogicalUnit;

                    if (IsPixelBased)
                    {
                        pixelSizeBeforeViewport = Math.Max(pixelSizeBeforeViewport, Math.Abs(_viewport.Y - _extendedViewport.Y));
                    }
                    else
                    {
                        logicalSizeBeforeViewport = Math.Max(logicalSizeBeforeViewport, Math.Abs(_viewport.Y - _extendedViewport.Y));
                    }
                }

                if (IsPixelBased)
                {
                    if (!IsScrolling && virtualizationInfoProvider != null &&
                        IsViewportEmpty(isHorizontal, extendedViewport) &&
                        DoubleUtil.GreaterThan(pixelSizeBeforeViewport, 0))
                    {
                        //
                        // If this is a GroupItem or a TreeViewItem that is completely above the viewport,
                        // then the CacheBeforeViewport allways designates the distance of the bottom of
                        // this panel from the top of the extendedViewport. Hence this computation for the offset.
                        //

                        extendedViewport.Y = pixelSize - pixelSizeBeforeViewport;
                    }
                    else
                    {
                        extendedViewport.Y -= pixelSizeBeforeViewport;
                    }

                    extendedViewport.Height += pixelSizeBeforeViewport + pixelSizeAfterViewport;

                    //
                    // Once again coerce the extended viewport dimensions to be within valid range.
                    //
                    if (IsScrolling)
                    {
                        if (DoubleUtil.LessThan(extendedViewport.Y, 0.0))
                        {
                            extendedViewport.Height = Math.Max(extendedViewport.Height + extendedViewport.Y, 0.0);
                            extendedViewport.Y = 0.0;
                        }

                        if (DoubleUtil.GreaterThan(extendedViewport.Y + extendedViewport.Height, _scrollData._extent.Height))
                        {
                            extendedViewport.Height = _scrollData._extent.Height - extendedViewport.Y;
                        }
                    }
                }
                else
                {
                    if (!IsScrolling && virtualizationInfoProvider != null &&
                        IsViewportEmpty(isHorizontal, extendedViewport) &&
                        DoubleUtil.GreaterThan(pixelSizeBeforeViewport, 0))
                    {
                        //
                        // If this is a GroupItem or a TreeViewItem that is completely above the viewport,
                        // then the CacheBeforeViewport allways designates the distance of the bottom of
                        // this panel from the top of the extendedViewport. Hence this computation for the offset.
                        //

                        extendedViewport.Y = logicalSize - logicalSizeBeforeViewport;
                    }
                    else
                    {
                        extendedViewport.Y -= logicalSizeBeforeViewport;
                    }

                    extendedViewport.Height += pixelSizeBeforeViewport + pixelSizeAfterViewport;

                    if (IsScrolling)
                    {
                        if (DoubleUtil.LessThan(extendedViewport.Y, 0.0))
                        {
                            extendedViewport.Height = Math.Max(extendedViewport.Height / approxSizeOfLogicalUnit + extendedViewport.Y, 0.0) * approxSizeOfLogicalUnit;
                            extendedViewport.Y = 0.0;
                        }

                        if (DoubleUtil.GreaterThan(extendedViewport.Y + extendedViewport.Height / approxSizeOfLogicalUnit, _scrollData._extent.Height))
                        {
                            extendedViewport.Height = (_scrollData._extent.Height - extendedViewport.Y) * approxSizeOfLogicalUnit;
                        }
                    }
                }
            }

            if (MeasureCaches)
            {
                itemsInExtendedViewportCount = _actualItemsInExtendedViewportCount;
            }
            else
            {
                double factor = Math.Max(1.0, isHorizontal ? extendedViewport.Width / viewport.Width : extendedViewport.Height / viewport.Height);
                int calcItemsInExtendedViewportCount = (int)Math.Ceiling(factor * _actualItemsInExtendedViewportCount);
                itemsInExtendedViewportCount = Math.Max(calcItemsInExtendedViewportCount, itemsInExtendedViewportCount);
            }

            return extendedViewport;
        }


        private void CoerceScrollingViewportOffset(ref Rect viewport, Size extent, bool isHorizontal)
        {
            Debug.Assert(IsScrolling, "The scrolling panel is the only one that should extend the viewport");

            if (!_scrollData.IsEmpty)
            {
                viewport.X = ScrollContentPresenter.CoerceOffset(viewport.X, extent.Width, viewport.Width);
                if (!IsPixelBased && isHorizontal && DoubleUtil.IsZero(viewport.Width) && DoubleUtil.AreClose(viewport.X, extent.Width))
                {
                    viewport.X = ScrollContentPresenter.CoerceOffset(viewport.X - 1, extent.Width, viewport.Width);
                }
            }

            if (!_scrollData.IsEmpty)
            {
                viewport.Y = ScrollContentPresenter.CoerceOffset(viewport.Y, extent.Height, viewport.Height);
                if (!IsPixelBased && !isHorizontal && DoubleUtil.IsZero(viewport.Height) && DoubleUtil.AreClose(viewport.Y, extent.Height))
                {
                    viewport.Y = ScrollContentPresenter.CoerceOffset(viewport.Y - 1, extent.Height, viewport.Height);
                }
            }
        }

        /// <summary>
        /// Adjusts viewport to accomodate the header.
        /// </summary>
        private void AdjustNonScrollingViewportForHeader(IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider,
                                                            ref Rect viewport,
                                                            ref VirtualizationCacheLength cacheLength,
                                                            ref VirtualizationCacheLengthUnit cacheLengthUnit)
        {
            bool forHeader = true;
            AdjustNonScrollingViewport(virtualizationInfoProvider, ref viewport, ref cacheLength, ref cacheLengthUnit, forHeader);
        }

        /// <summary>
        /// Adjusts viewport to accomodate the items.
        /// </summary>
        private void AdjustNonScrollingViewportForItems(IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider,
                                                            ref Rect viewport,
                                                            ref VirtualizationCacheLength cacheLength,
                                                            ref VirtualizationCacheLengthUnit cacheLengthUnit)
        {
            bool forHeader = false;
            AdjustNonScrollingViewport(virtualizationInfoProvider, ref viewport, ref cacheLength, ref cacheLengthUnit, forHeader);
        }

        /// <summary>
        /// Adjusts viewport to accomodate the either the Header or ItemsPanel.
        /// </summary>
        private void AdjustNonScrollingViewport(
            IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider,
            ref Rect viewport,
            ref VirtualizationCacheLength cacheLength,
            ref VirtualizationCacheLengthUnit cacheUnit,
            bool forHeader)
        {
            Debug.Assert(virtualizationInfoProvider != null, "This method should only be invoked for a virtualizing owner");
            Debug.Assert(cacheUnit != VirtualizationCacheLengthUnit.Page, "Page after cache size is not expected here.");

            Rect parentViewport = viewport;
            double sizeAfterStartViewportEdge = 0;
            double sizeBeforeStartViewportEdge = 0;
            double sizeAfterEndViewportEdge = 0;
            double sizeBeforeEndViewportEdge = 0;
            double cacheBeforeSize = cacheLength.CacheBeforeViewport;
            double cacheAfterSize = cacheLength.CacheAfterViewport;

            HierarchicalVirtualizationHeaderDesiredSizes headerDesiredSizes = virtualizationInfoProvider.HeaderDesiredSizes;
            HierarchicalVirtualizationItemDesiredSizes itemDesiredSizes = virtualizationInfoProvider.ItemDesiredSizes;

            Size pixelSize = forHeader ? headerDesiredSizes.PixelSize : itemDesiredSizes.PixelSize;
            Size logicalSize = forHeader ? headerDesiredSizes.LogicalSize : itemDesiredSizes.LogicalSize;

            RelativeHeaderPosition headerPosition = RelativeHeaderPosition.Top; // virtualizationInfoProvider.RelativeHeaderPosition;

            if ((forHeader && headerPosition == RelativeHeaderPosition.Left) ||
                (!forHeader && headerPosition == RelativeHeaderPosition.Right))
            {
                //
                // Adjust the offset
                //

                viewport.X -= IsPixelBased ? pixelSize.Width : logicalSize.Width;

                if (DoubleUtil.GreaterThan(parentViewport.X, 0))
                {
                    //
                    // Viewport is after the start of this panel
                    //

                    if (IsPixelBased && DoubleUtil.GreaterThan(pixelSize.Width, parentViewport.X))
                    {
                        //
                        // Header straddles the start edge of the viewport
                        //

                        sizeAfterStartViewportEdge = pixelSize.Width - parentViewport.X;
                        sizeBeforeStartViewportEdge = pixelSize.Width - sizeAfterStartViewportEdge;

                        viewport.Width = Math.Max(viewport.Width - sizeAfterStartViewportEdge, 0);

                        if (cacheUnit == VirtualizationCacheLengthUnit.Pixel)
                        {
                            cacheBeforeSize = Math.Max(cacheBeforeSize - sizeBeforeStartViewportEdge, 0);
                        }
                        else
                        {
                            cacheBeforeSize = Math.Max(cacheBeforeSize - Math.Floor(logicalSize.Width * sizeBeforeStartViewportEdge / pixelSize.Width), 0);
                        }
                    }
                    else
                    {
                        //
                        // Header is completely before the start edge of the viewport. We do not need to
                        // adjust the cacheBefore size in this case because the cacheBefore is populated
                        // bottom up and we cant really be certain that the header will infact lie within the
                        // cacheBefore region.
                        //
                    }
                }
                else
                {
                    //
                    // Viewport is at or before this panel
                    //

                    if (DoubleUtil.GreaterThan(parentViewport.Width, 0))
                    {
                        if (DoubleUtil.GreaterThanOrClose(parentViewport.Width, pixelSize.Width))
                        {
                            //
                            // Header is completely within the viewport
                            //

                            viewport.Width = Math.Max(0, parentViewport.Width - pixelSize.Width);
                        }
                        else
                        {
                            //
                            // Header straddles the end edge of the viewport
                            //

                            sizeBeforeEndViewportEdge = parentViewport.Width;
                            sizeAfterEndViewportEdge = pixelSize.Width - sizeBeforeEndViewportEdge;

                            viewport.Width = 0;

                            if (cacheUnit == VirtualizationCacheLengthUnit.Pixel)
                            {
                                cacheAfterSize = Math.Max(cacheAfterSize - sizeAfterEndViewportEdge, 0);
                            }
                            else
                            {
                                cacheAfterSize = Math.Max(cacheAfterSize - Math.Floor(logicalSize.Width * sizeAfterEndViewportEdge / pixelSize.Width), 0);
                            }
                        }
                    }
                    else
                    {
                        //
                        // Header is completely after the end edge of the viewport
                        //

                        if (cacheUnit == VirtualizationCacheLengthUnit.Pixel)
                        {
                            cacheAfterSize = Math.Max(cacheAfterSize - pixelSize.Width, 0);
                        }
                        else
                        {
                            cacheAfterSize = Math.Max(cacheAfterSize - logicalSize.Width, 0);
                        }
                    }
                }
            }
            else if ((forHeader && headerPosition == RelativeHeaderPosition.Top) ||
                    (!forHeader && headerPosition == RelativeHeaderPosition.Bottom))
            {
                //
                // Adjust the offset
                //

                viewport.Y -= IsPixelBased ? pixelSize.Height : logicalSize.Height;

                if (DoubleUtil.GreaterThan(parentViewport.Y, 0))
                {
                    //
                    // Viewport is after the start of this panel
                    //

                    if (IsPixelBased && DoubleUtil.GreaterThan(pixelSize.Height, parentViewport.Y))
                    {
                        //
                        // Header straddles the start edge of the viewport
                        //

                        sizeAfterStartViewportEdge = pixelSize.Height - parentViewport.Y;
                        sizeBeforeStartViewportEdge = pixelSize.Height - sizeAfterStartViewportEdge;

                        viewport.Height = Math.Max(viewport.Height - sizeAfterStartViewportEdge, 0);

                        if (cacheUnit == VirtualizationCacheLengthUnit.Pixel)
                        {
                            cacheBeforeSize = Math.Max(cacheBeforeSize - sizeBeforeStartViewportEdge, 0);
                        }
                        else
                        {
                            cacheBeforeSize = Math.Max(cacheBeforeSize - Math.Floor(logicalSize.Height * sizeBeforeStartViewportEdge / pixelSize.Height), 0);
                        }
                    }
                    else
                    {
                        //
                        // Header is completely before the start edge of the viewport. We do not need to
                        // adjust the cacheBefore size in this case because the cacheBefore is populated
                        // bottom up and we cant really be certain that the header will infact lie within the
                        // cacheBefore region.
                        //
                    }
                }
                else
                {
                    //
                    // Viewport is at or before the start of this panel
                    //

                    if (DoubleUtil.GreaterThan(parentViewport.Height, 0))
                    {
                        if (DoubleUtil.GreaterThanOrClose(parentViewport.Height, pixelSize.Height))
                        {
                            //
                            // Header is completely within the viewport
                            //

                            viewport.Height = Math.Max(0, parentViewport.Height - pixelSize.Height);
                        }
                        else
                        {
                            //
                            // Header straddles the end edge of the viewport
                            //

                            sizeBeforeEndViewportEdge = parentViewport.Height;
                            sizeAfterEndViewportEdge = pixelSize.Height - sizeBeforeEndViewportEdge;

                            viewport.Height = 0;

                            if (cacheUnit == VirtualizationCacheLengthUnit.Pixel)
                            {
                                cacheAfterSize = Math.Max(cacheAfterSize - sizeAfterEndViewportEdge, 0);
                            }
                            else
                            {
                                cacheAfterSize = Math.Max(cacheAfterSize - Math.Floor(logicalSize.Height * sizeAfterEndViewportEdge / pixelSize.Height), 0);
                            }
                        }
                    }
                    else
                    {
                        //
                        // Header is completely after the end edge of the viewport
                        //

                        if (cacheUnit == VirtualizationCacheLengthUnit.Pixel)
                        {
                            cacheAfterSize = Math.Max(cacheAfterSize - pixelSize.Height, 0);
                        }
                        else
                        {
                            cacheAfterSize = Math.Max(cacheAfterSize - logicalSize.Height, 0);
                        }
                    }
                }
            }

            cacheLength = new VirtualizationCacheLength(cacheBeforeSize, cacheAfterSize);
        }

        /// <summary>
        /// Returns the index of the first item visible (even partially) in the viewport.
        /// </summary>
        private void ComputeFirstItemInViewportIndexAndOffset(
            IList items,
            int itemCount,
            IContainItemStorage itemStorageProvider,
            Rect viewport,
            VirtualizationCacheLength cacheSize,
            bool isHorizontal,
            bool areContainersUniformlySized,
            double uniformOrAverageContainerSize,
            out double firstItemInViewportOffset,
            out int firstItemInViewportIndex,
            out bool foundFirstItemInViewport)
        {
            firstItemInViewportOffset = 0.0;
            firstItemInViewportIndex = 0;
            foundFirstItemInViewport = false;

            if (IsViewportEmpty(isHorizontal, viewport))
            {
                if (DoubleUtil.GreaterThan(cacheSize.CacheBeforeViewport, 0.0))
                {
                    firstItemInViewportIndex = itemCount-1;
                    ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, 0, itemCount-1, out firstItemInViewportOffset);
                    foundFirstItemInViewport = true;
                }
                else
                {
                    //
                    // If the cacheSizeAfterViewport is also empty then we are merely
                    // here scouting to get a better measurement of this item.
                    //
                    firstItemInViewportIndex = 0;
                    firstItemInViewportOffset = 0;
                    foundFirstItemInViewport = DoubleUtil.GreaterThan(cacheSize.CacheAfterViewport, 0.0);
                }
            }
            else
            {
                //
                // Compute the span of this panel above the viewport. Note that if
                // the panel is below the viewport then this span is 0.0.
                //
                double spanBeforeViewport = Math.Max(isHorizontal ? viewport.X : viewport.Y, 0.0);

                if (areContainersUniformlySized)
                {
                    //
                    // This is an optimization for the case that all the children are of
                    // uniform dimension along the stacking axis. In this case the index
                    // and offset for the first item in the viewport is computed in constant time.
                    //
                    double childSize = uniformOrAverageContainerSize;
                    if (DoubleUtil.GreaterThan(childSize, 0))
                    {
                        firstItemInViewportIndex = (int)Math.Floor(spanBeforeViewport / childSize);
                        firstItemInViewportOffset = firstItemInViewportIndex * childSize;
                    }

                    foundFirstItemInViewport = (firstItemInViewportIndex < itemCount);
                    if (!foundFirstItemInViewport)
                    {
                        firstItemInViewportOffset = 0.0;
                        firstItemInViewportIndex = 0;
                    }
                }
                else
                {
                    if (DoubleUtil.AreClose(spanBeforeViewport, 0))
                    {
                        foundFirstItemInViewport = true;
                        firstItemInViewportOffset = 0.0;
                        firstItemInViewportIndex = 0;
                    }
                    else
                    {
                        Size containerSize;
                        double totalSpan = 0.0;      // total height or width in the stacking direction
                        double containerSpan = 0.0;

                        for (int i = 0; i < itemCount; i++)
                        {
                            object item = items[i];
                            GetContainerSizeForItem(itemStorageProvider, item, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, out containerSize);
                            containerSpan = isHorizontal ? containerSize.Width : containerSize.Height;
                            totalSpan += containerSpan;

                            if (DoubleUtil.GreaterThan(totalSpan, spanBeforeViewport))
                            {
                                //
                                // This is the first item that starts before the  viewport but ends after it.
                                // It is thus the the first item in the viewport.
                                //
                                firstItemInViewportIndex = i;
                                firstItemInViewportOffset = totalSpan - containerSpan;
                                break;
                            }
                        }

                        foundFirstItemInViewport = DoubleUtil.LessThan(spanBeforeViewport, totalSpan);
                        if (!foundFirstItemInViewport)
                        {
                            firstItemInViewportOffset = 0.0;
                            firstItemInViewportIndex = 0;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// DesiredSize is normally computed by summing up the size of all items we've generated.  Pixel-based virtualization uses a 'full' desired size.
        /// This extends the given desired size beyond the visible items.  It will extend it by the items before or after the set of generated items.
        /// The given pivotIndex is the index of either the first or last item generated.
        /// </summary>
        private void ExtendPixelAndLogicalSizes(
            IList children,
            IList items,
            int itemCount,
            IContainItemStorage itemStorageProvider,
            bool areContainersUniformlySized,
            double uniformOrAverageContainerSize,
            ref Size stackPixelSize,
            ref Size stackLogicalSize,
            bool isHorizontal,
            int pivotIndex,
            int pivotChildIndex,
            bool before)
        {
            Debug.Assert(IsVirtualizing, "We should only need to extend the viewport beyond the generated items when virtualizing");

            //
            // If we're virtualizing the sum of all generated containers is not the true desired size since not all containers were generated.
            // In the old items-based mode it didn't matter because only the scrolling panel could virtualize and scrollviewer doesn't *really*
            // care about desired size.
            //
            // In pixel-based mode we need to compute the same desired size as if we weren't virtualizing.
            //

            double distance;
            if (before)
            {
                ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, 0, pivotIndex, out distance);
            }
            else
            {
                ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, pivotIndex, itemCount - pivotIndex, out distance);
            }

            if (IsPixelBased)
            {
                if (isHorizontal)
                {
                    stackPixelSize.Width += distance;
                }
                else
                {
                    stackPixelSize.Height += distance;
                }
            }
            else
            {
                if (isHorizontal)
                {
                    stackLogicalSize.Width += distance;
                }
                else
                {
                    stackLogicalSize.Height += distance;
                }

                //
                // If there are containers beyond the extended
                // viewport then their sizes need to be added to
                // the stackPixelSize. This is only required in the
                // hierarchical cases to be able to arrange containers
                // beyond the extended viewport accurately.
                //

                if (!IsScrolling)
                {
                    int startIndex, count;

                    if (before)
                    {
                        startIndex = 0;
                        count = pivotChildIndex;
                    }
                    else
                    {
                        startIndex = pivotChildIndex;
                        count = children.Count;
                    }

                    for (int i=startIndex; i<count; i++)
                    {
                        Size childDesiredSize = ((UIElement)children[i]).DesiredSize;

                        if (isHorizontal)
                        {
                            stackPixelSize.Width += childDesiredSize.Width;
                        }
                        else
                        {
                            stackPixelSize.Height += childDesiredSize.Height;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// This method is called upon to compute the pixel and logical
        /// distances for the itemCount beginning at the the start index.
        /// </summary>
        private void ComputeDistance(
            IList items,
            IContainItemStorage itemStorageProvider,
            bool isHorizontal,
            bool areContainersUniformlySized,
            double uniformOrAverageContainerSize,
            int startIndex,
            int itemCount,
            out double distance)
        {
            distance = 0.0;

            if (areContainersUniformlySized)
            {
                //
                // Performance optimization for the most general case where
                // all the children are of uniform size along the stacking direction.
                // Note that the computation of the range size is performed in constant time.
                //
                double childSize = uniformOrAverageContainerSize;
                if (isHorizontal)
                {
                    distance += childSize * itemCount;
                }
                else
                {
                    distance += childSize * itemCount;
                }
            }
            else
            {
                for (int i = startIndex; i < startIndex + itemCount; i++)
                {
                    object item = items[i];

                    Size containerSize;
                    GetContainerSizeForItem(itemStorageProvider, item, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize,
                        out containerSize);

                    if (isHorizontal)
                    {
                        distance += containerSize.Width;
                    }
                    else
                    {
                        distance += containerSize.Height;
                    }
                }
            }
        }

        /// <summary>
        /// Returns the size of the container for a given item.  The size can come from the container or a lookup in the ItemStorage
        /// </summary>
        private void GetContainerSizeForItem(
            IContainItemStorage itemStorageProvider,
            object item,
            bool isHorizontal,
            bool areContainersUniformlySized,
            double uniformOrAverageContainerSize,
            out Size containerSize)
        {
            containerSize = Size.Empty;

            if (areContainersUniformlySized)
            {
                //
                // This is a performance optimization for the case that the containers are unformly sized.
                //
                containerSize = new Size();
                double uniformSize = uniformOrAverageContainerSize;

                if (isHorizontal)
                {
                    containerSize.Width = uniformSize;
                    containerSize.Height = IsPixelBased ? DesiredSize.Height : 1;
                }
                else
                {
                    containerSize.Height = uniformSize;
                    containerSize.Width = IsPixelBased ? DesiredSize.Width : 1;
                }
            }
            else
            {
                //
                // We fetch the size of a container from the ItemStorage.
                // The size is cached if this item were previously realized.
                //
                object value = itemStorageProvider.ReadItemValue(item, ContainerSizeProperty);
                if (value != null)
                {
                    containerSize = (Size)value;
                }
                else
                {
                    //
                    // This item has never been realized previously. So use the average size.
                    //
                    containerSize = new Size();
                    double averageSize = uniformOrAverageContainerSize;

                    if (isHorizontal)
                    {
                        containerSize.Width = averageSize;
                        containerSize.Height = IsPixelBased ? DesiredSize.Height : 1;
                    }
                    else
                    {
                        containerSize.Height = averageSize;
                        containerSize.Width = IsPixelBased ? DesiredSize.Width : 1;
                    }
                }
            }

            Debug.Assert(!containerSize.IsEmpty, "We can't estimate an empty size");
        }

        /// <summary>
        /// Sets the size of the container for a given item. If the items aren't uniformly sized store it in the ItemStorage.
        /// </summary>
        private void SetContainerSizeForItem(
            IContainItemStorage itemStorageProvider,
            object parentItem,
            object item,
            Size containerSize,
            bool isHorizontal,
            ref bool hasUniformOrAverageContainerSizeBeenSet,
            ref double uniformOrAverageContainerSize,
            ref bool areContainersUniformlySized)
        {
            if (!hasUniformOrAverageContainerSizeBeenSet)
            {
                hasUniformOrAverageContainerSizeBeenSet = true;
                uniformOrAverageContainerSize = isHorizontal ? containerSize.Width : containerSize.Height;
                SetUniformOrAverageContainerSize(itemStorageProvider, parentItem, uniformOrAverageContainerSize);
            }
            else if (areContainersUniformlySized)
            {
                //
                // if we come across a child whose DesiredSize is different from _uniformOrAverageContainerSize
                // Once AreContainersUniformlySized becomes false, dont set it back ever.
                //
                if (isHorizontal)
                {
                    areContainersUniformlySized = DoubleUtil.AreClose(containerSize.Width, uniformOrAverageContainerSize);
                }
                else
                {
                    areContainersUniformlySized = DoubleUtil.AreClose(containerSize.Height, uniformOrAverageContainerSize);
                }
            }

            //
            // Save off the child's desired size for later. The stored size is useful in hierarchical virtualization
            // scenarios (Eg. TreeView, Grouping) to compute the index of the first visible item in the viewport
            // and to Arrange children in their proper locations.
            //
            if (!areContainersUniformlySized)
            {
                itemStorageProvider.StoreItemValue(item, ContainerSizeProperty, containerSize);
            }
        }

        private bool IsEndOfCache(
            bool isHorizontal,
            double cacheSize,
            VirtualizationCacheLengthUnit cacheUnit,
            Size stackPixelSizeInCache,
            Size stackLogicalSizeInCache)
        {
            if (!MeasureCaches)
            {
                return true;
            }

            Debug.Assert(cacheUnit != VirtualizationCacheLengthUnit.Page, "Page cacheUnit is not expected here.");

            if (cacheUnit == VirtualizationCacheLengthUnit.Item)
            {
                if (isHorizontal)
                {
                    return DoubleUtil.GreaterThanOrClose(stackLogicalSizeInCache.Width, cacheSize);
                }
                else
                {
                    return DoubleUtil.GreaterThanOrClose(stackLogicalSizeInCache.Height, cacheSize);
                }
            }
            else if (cacheUnit == VirtualizationCacheLengthUnit.Pixel)
            {
                if (isHorizontal)
                {
                    return DoubleUtil.GreaterThanOrClose(stackPixelSizeInCache.Width, cacheSize);
                }
                else
                {
                    return DoubleUtil.GreaterThanOrClose(stackPixelSizeInCache.Height, cacheSize);
                }
            }
            return false;
        }

        private bool IsEndOfViewport(bool isHorizontal, Rect viewport, Size stackPixelSizeInViewport)
        {
            if (isHorizontal)
            {
                return DoubleUtil.GreaterThanOrClose(stackPixelSizeInViewport.Width, viewport.Width);
            }
            else
            {
                return DoubleUtil.GreaterThanOrClose(stackPixelSizeInViewport.Height, viewport.Height);
            }
        }

        private bool IsViewportEmpty(bool isHorizontal, Rect viewport)
        {
            if (isHorizontal)
            {
                return DoubleUtil.AreClose(viewport.Width, 0.0);
            }
            else
            {
                return DoubleUtil.AreClose(viewport.Height, 0.0);
            }
        }

        /// <summary>
        /// Called when to set a new viewport on the child when it is about to be measured.
        /// </summary>
        private void SetViewportForChild(
            bool isHorizontal,
            IContainItemStorage itemStorageProvider,
            bool areContainersUniformlySized,
            double uniformOrAverageContainerSize,
            bool mustDisableVirtualization,
            UIElement child,
            IHierarchicalVirtualizationAndScrollInfo virtualizingChild,
            object item,
            bool isBeforeFirstItem,
            bool isAfterFirstItem,
            double firstItemInViewportOffset,
            Rect parentViewport,
            VirtualizationCacheLength parentCacheSize,
            VirtualizationCacheLengthUnit parentCacheUnit,
            Size stackPixelSize,
            Size stackPixelSizeInViewport,
            Size stackPixelSizeInCacheBeforeViewport,
            Size stackPixelSizeInCacheAfterViewport,
            Size stackLogicalSize,
            Size stackLogicalSizeInViewport,
            Size stackLogicalSizeInCacheBeforeViewport,
            Size stackLogicalSizeInCacheAfterViewport,
            out Rect childViewport,
            ref VirtualizationCacheLength childCacheSize,
            ref VirtualizationCacheLengthUnit childCacheUnit)
        {
            childViewport = parentViewport;

            //
            // Adjust viewport offset for the child by deducting
            // the dimensions of the previous siblings.
            //
            if (isHorizontal)
            {
                if (isBeforeFirstItem)
                {
                    Size containerSize;
                    GetContainerSizeForItem(itemStorageProvider, item, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, out containerSize);
                    childViewport.X = (IsPixelBased ? stackPixelSizeInCacheBeforeViewport.Width : stackLogicalSizeInCacheBeforeViewport.Width) + containerSize.Width;
                    childViewport.Width = 0.0;
                }
                else if (isAfterFirstItem)
                {
                    childViewport.X = Math.Min(childViewport.X, 0) -
                                      (IsPixelBased ? stackPixelSizeInViewport.Width + stackPixelSizeInCacheAfterViewport.Width :
                                                       stackLogicalSizeInViewport.Width + stackLogicalSizeInCacheAfterViewport.Width);

                    childViewport.Width = Math.Max(childViewport.Width - stackPixelSizeInViewport.Width, 0.0);
                }
                else
                {
                    childViewport.X -= firstItemInViewportOffset;
                    childViewport.Width = Math.Max(childViewport.Width - stackPixelSizeInViewport.Width, 0.0);
                }

                if (parentCacheUnit == VirtualizationCacheLengthUnit.Item)
                {
                    childCacheSize = new VirtualizationCacheLength(
                        isAfterFirstItem || DoubleUtil.LessThanOrClose(childViewport.X, 0.0) ?
                            0.0 :
                            Math.Max(parentCacheSize.CacheBeforeViewport - stackLogicalSizeInCacheBeforeViewport.Width, 0.0),
                        isBeforeFirstItem ?
                            0.0 :
                            Math.Max(parentCacheSize.CacheAfterViewport - stackLogicalSizeInCacheAfterViewport.Width, 0.0));
                    childCacheUnit = VirtualizationCacheLengthUnit.Item;
                }
                else if (parentCacheUnit == VirtualizationCacheLengthUnit.Pixel)
                {
                    childCacheSize = new VirtualizationCacheLength(
                        isAfterFirstItem || DoubleUtil.LessThanOrClose(childViewport.X, 0.0) ?
                            0.0 :
                            Math.Max(parentCacheSize.CacheBeforeViewport - stackPixelSizeInCacheBeforeViewport.Width, 0.0),
                        isBeforeFirstItem ?
                            0.0 :
                            Math.Max(parentCacheSize.CacheAfterViewport - stackPixelSizeInCacheAfterViewport.Width, 0.0));
                    childCacheUnit = VirtualizationCacheLengthUnit.Pixel;
                }
            }
            else
            {
                if (isBeforeFirstItem)
                {
                    Size containerSize;
                    GetContainerSizeForItem(itemStorageProvider, item, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, out containerSize);
                    childViewport.Y = (IsPixelBased ? stackPixelSizeInCacheBeforeViewport.Height : stackLogicalSizeInCacheBeforeViewport.Height) + containerSize.Height;
                    childViewport.Height = 0.0;
                }
                else if (isAfterFirstItem)
                {
                    childViewport.Y = Math.Min(childViewport.Y, 0) -
                                      (IsPixelBased ? stackPixelSizeInViewport.Height + stackPixelSizeInCacheAfterViewport.Height :
                                                       stackLogicalSizeInViewport.Height + stackLogicalSizeInCacheAfterViewport.Height);

                    childViewport.Height = Math.Max(childViewport.Height - stackPixelSizeInViewport.Height, 0.0);
                }
                else
                {
                    childViewport.Y -= firstItemInViewportOffset;
                    childViewport.Height = Math.Max(childViewport.Height - stackPixelSizeInViewport.Height, 0.0);
                }

                if (parentCacheUnit == VirtualizationCacheLengthUnit.Item)
                {
                    childCacheSize = new VirtualizationCacheLength(
                        isAfterFirstItem || DoubleUtil.LessThanOrClose(childViewport.Y, 0.0) ?
                            0.0 :
                            Math.Max(parentCacheSize.CacheBeforeViewport - stackLogicalSizeInCacheBeforeViewport.Height, 0.0),
                        isBeforeFirstItem ?
                            0.0 :
                            Math.Max(parentCacheSize.CacheAfterViewport - stackLogicalSizeInCacheAfterViewport.Height, 0.0));
                    childCacheUnit = VirtualizationCacheLengthUnit.Item;
                }
                else if (parentCacheUnit == VirtualizationCacheLengthUnit.Pixel)
                {
                    childCacheSize = new VirtualizationCacheLength(
                        isAfterFirstItem || DoubleUtil.LessThanOrClose(childViewport.Y, 0.0) ?
                            0.0 :
                            Math.Max(parentCacheSize.CacheBeforeViewport - stackPixelSizeInCacheBeforeViewport.Height, 0.0),
                        isBeforeFirstItem ?
                            0.0 :
                            Math.Max(parentCacheSize.CacheAfterViewport - stackPixelSizeInCacheAfterViewport.Height, 0.0));
                    childCacheUnit = VirtualizationCacheLengthUnit.Pixel;
                }
            }

            if (virtualizingChild != null)
            {
                virtualizingChild.Constraints = new HierarchicalVirtualizationConstraints(
                    childCacheSize,
                    childCacheUnit,
                    childViewport);
                virtualizingChild.InBackgroundLayout = MeasureCaches;
                virtualizingChild.MustDisableVirtualization = mustDisableVirtualization;
            }

            if (child is IHierarchicalVirtualizationAndScrollInfo)
            {
                //
                // Ensure that measure is invalid through the items panel
                // of the child, so it can react to the new viewport.
                //
                InvalidateMeasureOnItemsHost((IHierarchicalVirtualizationAndScrollInfo)child);
            }
        }

        /// <summary>
        /// Called when a new viewport is set on the child and it is about to be measured.
        /// Invalidates Measure on all elements between child.ItemsHost and this panel.
        /// </summary>
        private void InvalidateMeasureOnItemsHost(IHierarchicalVirtualizationAndScrollInfo virtualizingChild)
        {
            Debug.Assert(virtualizingChild != null, "This method should only be invoked for a virtualizing child");

            Panel childItemsHost = virtualizingChild.ItemsHost;
            if (childItemsHost != null)
            {
                Helper.InvalidateMeasureOnPath(childItemsHost, this, true /*duringMeasure*/);

                if (!(childItemsHost is VirtualizingStackPanel))
                {
                    //
                    // For non-VSPs recurse a level deeper
                    //
                    IList children =  childItemsHost.InternalChildren;
                    for (int i=0; i<children.Count; i++)
                    {
                        IHierarchicalVirtualizationAndScrollInfo virtualizingGrandChild = children[i] as IHierarchicalVirtualizationAndScrollInfo;
                        if (virtualizingGrandChild != null)
                        {
                            InvalidateMeasureOnItemsHost(virtualizingGrandChild);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Returns the size of the child in pixel and logical units and also identifies the part of the child visible in the viewport.
        /// </summary>
        private void GetSizesForChild(
            bool isHorizontal,
            bool isChildHorizontal,
            bool isBeforeFirstItem,
            bool isAfterLastItem,
            IHierarchicalVirtualizationAndScrollInfo virtualizingChild,
            Size childDesiredSize,
            Rect childViewport,
            VirtualizationCacheLength childCacheSize,
            VirtualizationCacheLengthUnit childCacheUnit,
            out Size childPixelSize,
            out Size childPixelSizeInViewport,
            out Size childPixelSizeInCacheBeforeViewport,
            out Size childPixelSizeInCacheAfterViewport,
            out Size childLogicalSize,
            out Size childLogicalSizeInViewport,
            out Size childLogicalSizeInCacheBeforeViewport,
            out Size childLogicalSizeInCacheAfterViewport)
        {
            childPixelSize = new Size();
            childPixelSizeInViewport = new Size();
            childPixelSizeInCacheBeforeViewport = new Size();
            childPixelSizeInCacheAfterViewport = new Size();

            childLogicalSize = new Size();
            childLogicalSizeInViewport = new Size();
            childLogicalSizeInCacheBeforeViewport = new Size();
            childLogicalSizeInCacheAfterViewport = new Size();

            if (virtualizingChild != null)
            {
                RelativeHeaderPosition headerPosition = RelativeHeaderPosition.Top; // virtualizingChild.RelativeHeaderPosition;
                HierarchicalVirtualizationHeaderDesiredSizes headerDesiredSizes = virtualizingChild.HeaderDesiredSizes;
                HierarchicalVirtualizationItemDesiredSizes itemDesiredSizes = virtualizingChild.ItemDesiredSizes;

                Size pixelHeaderSize = headerDesiredSizes.PixelSize;
                Size logicalHeaderSize = headerDesiredSizes.LogicalSize;

                childPixelSize = childDesiredSize;

                if (headerPosition == RelativeHeaderPosition.Top || headerPosition == RelativeHeaderPosition.Bottom)
                {
                    childLogicalSize.Height = itemDesiredSizes.LogicalSize.Height + logicalHeaderSize.Height;
                    childLogicalSize.Width = Math.Max(itemDesiredSizes.LogicalSize.Width, logicalHeaderSize.Width);
                }
                else // if (headerPosition == RelativeHeaderPosition.Left || headerPosition == RelativeHeaderPosition.Right)
                {
                    childLogicalSize.Width = itemDesiredSizes.LogicalSize.Width + logicalHeaderSize.Width;
                    childLogicalSize.Height = Math.Max(itemDesiredSizes.LogicalSize.Height, logicalHeaderSize.Height);
                }

                if (IsPixelBased &&
                    ((isHorizontal && DoubleUtil.AreClose(itemDesiredSizes.PixelSize.Width, itemDesiredSizes.PixelSizeInViewport.Width)) ||
                    (!isHorizontal && DoubleUtil.AreClose(itemDesiredSizes.PixelSize.Height, itemDesiredSizes.PixelSizeInViewport.Height))))
                {
                    Rect childItemsViewport = childViewport;

                    if (headerPosition == RelativeHeaderPosition.Top || headerPosition == RelativeHeaderPosition.Left)
                    {
                        VirtualizationCacheLength childItemsCacheSize = childCacheSize;
                        VirtualizationCacheLengthUnit childItemsCacheUnit = childCacheUnit;

                        AdjustNonScrollingViewportForHeader(virtualizingChild, ref childItemsViewport, ref childItemsCacheSize, ref childItemsCacheUnit);
                    }

                    GetSizesForChildIntersectingTheViewport(
                        isHorizontal,
                        isChildHorizontal,
                        itemDesiredSizes.PixelSizeInViewport,
                        itemDesiredSizes.LogicalSizeInViewport,
                        childItemsViewport,
                        ref childPixelSizeInViewport,
                        ref childLogicalSizeInViewport,
                        ref childPixelSizeInCacheBeforeViewport,
                        ref childLogicalSizeInCacheBeforeViewport,
                        ref childPixelSizeInCacheAfterViewport,
                        ref childLogicalSizeInCacheAfterViewport);
                }
                else
                {
                    StackSizes(isHorizontal, ref childPixelSizeInViewport, itemDesiredSizes.PixelSizeInViewport);
                    StackSizes(isHorizontal, ref childLogicalSizeInViewport, itemDesiredSizes.LogicalSizeInViewport);
                }

                if (isChildHorizontal == isHorizontal)
                {
                    StackSizes(isHorizontal, ref childPixelSizeInCacheBeforeViewport, itemDesiredSizes.PixelSizeBeforeViewport);
                    StackSizes(isHorizontal, ref childLogicalSizeInCacheBeforeViewport, itemDesiredSizes.LogicalSizeBeforeViewport);
                    StackSizes(isHorizontal, ref childPixelSizeInCacheAfterViewport, itemDesiredSizes.PixelSizeAfterViewport);
                    StackSizes(isHorizontal, ref childLogicalSizeInCacheAfterViewport, itemDesiredSizes.LogicalSizeAfterViewport);
                }

                Rect childHeaderViewport = childViewport;
                Size childHeaderPixelSizeInViewport = new Size();
                Size childHeaderLogicalSizeInViewport = new Size();
                Size childHeaderPixelSizeInCacheBeforeViewport = new Size();
                Size childHeaderLogicalSizeInCacheBeforeViewport = new Size();
                Size childHeaderPixelSizeInCacheAfterViewport = new Size();
                Size childHeaderLogicalSizeInCacheAfterViewport = new Size();
                bool isChildHeaderHorizontal = (headerPosition == RelativeHeaderPosition.Left || headerPosition == RelativeHeaderPosition.Right);

                if (headerPosition == RelativeHeaderPosition.Bottom || headerPosition == RelativeHeaderPosition.Right)
                {
                    VirtualizationCacheLength childHeaderCacheSize = childCacheSize;
                    VirtualizationCacheLengthUnit childHeaderCacheUnit = childCacheUnit;

                    AdjustNonScrollingViewportForItems(virtualizingChild, ref childHeaderViewport, ref childHeaderCacheSize, ref childHeaderCacheUnit);
                }

                if (isBeforeFirstItem)
                {
                    childHeaderPixelSizeInCacheBeforeViewport = pixelHeaderSize;
                    childHeaderLogicalSizeInCacheBeforeViewport = logicalHeaderSize;
                }
                else if (isAfterLastItem)
                {
                    childHeaderPixelSizeInCacheAfterViewport = pixelHeaderSize;
                    childHeaderLogicalSizeInCacheAfterViewport = logicalHeaderSize;
                }
                else
                {
                    GetSizesForChildIntersectingTheViewport(
                        isHorizontal,
                        isChildHorizontal,
                        pixelHeaderSize,
                        logicalHeaderSize,
                        childHeaderViewport,
                        ref childHeaderPixelSizeInViewport,
                        ref childHeaderLogicalSizeInViewport,
                        ref childHeaderPixelSizeInCacheBeforeViewport,
                        ref childHeaderLogicalSizeInCacheBeforeViewport,
                        ref childHeaderPixelSizeInCacheAfterViewport,
                        ref childHeaderLogicalSizeInCacheAfterViewport);
                }

                StackSizes(isChildHeaderHorizontal, ref childPixelSizeInViewport, childHeaderPixelSizeInViewport);
                StackSizes(isChildHeaderHorizontal, ref childLogicalSizeInViewport, childHeaderLogicalSizeInViewport);
                StackSizes(isChildHeaderHorizontal, ref childPixelSizeInCacheBeforeViewport, childHeaderPixelSizeInCacheBeforeViewport);
                StackSizes(isChildHeaderHorizontal, ref childLogicalSizeInCacheBeforeViewport, childHeaderLogicalSizeInCacheBeforeViewport);
                StackSizes(isChildHeaderHorizontal, ref childPixelSizeInCacheAfterViewport, childHeaderPixelSizeInCacheAfterViewport);
                StackSizes(isChildHeaderHorizontal, ref childLogicalSizeInCacheAfterViewport, childHeaderLogicalSizeInCacheAfterViewport);
            }
            else
            {
                childPixelSize = childDesiredSize;
                childLogicalSize = new Size(DoubleUtil.GreaterThan(childPixelSize.Width, 0) ? 1 : 0,
                                            DoubleUtil.GreaterThan(childPixelSize.Height, 0) ? 1 : 0);

                if (isBeforeFirstItem)
                {
                    childPixelSizeInCacheBeforeViewport = childDesiredSize;
                    childLogicalSizeInCacheBeforeViewport = new Size(DoubleUtil.GreaterThan(childPixelSizeInCacheBeforeViewport.Width, 0) ? 1 : 0,
                                                                     DoubleUtil.GreaterThan(childPixelSizeInCacheBeforeViewport.Height, 0) ? 1 : 0);
                }
                else if (isAfterLastItem)
                {
                    childPixelSizeInCacheAfterViewport = childDesiredSize;
                    childLogicalSizeInCacheAfterViewport = new Size(DoubleUtil.GreaterThan(childPixelSizeInCacheAfterViewport.Width, 0) ? 1 : 0,
                                                                    DoubleUtil.GreaterThan(childPixelSizeInCacheAfterViewport.Height, 0) ? 1 : 0);
                }
                else
                {
                    GetSizesForChildIntersectingTheViewport(
                        isHorizontal,
                        isHorizontal,
                        childPixelSize,
                        childLogicalSize,
                        childViewport,
                        ref childPixelSizeInViewport,
                        ref childLogicalSizeInViewport,
                        ref childPixelSizeInCacheBeforeViewport,
                        ref childLogicalSizeInCacheBeforeViewport,
                        ref childPixelSizeInCacheAfterViewport,
                        ref childLogicalSizeInCacheAfterViewport);
                }
            }
        }

        private void GetSizesForChildIntersectingTheViewport(
            bool isHorizontal,
            bool childIsHorizontal,
            Size childPixelSize,
            Size childLogicalSize,
            Rect childViewport,
            ref Size childPixelSizeInViewport,
            ref Size childLogicalSizeInViewport,
            ref Size childPixelSizeInCacheBeforeViewport,
            ref Size childLogicalSizeInCacheBeforeViewport,
            ref Size childPixelSizeInCacheAfterViewport,
            ref Size childLogicalSizeInCacheAfterViewport)
        {
            double pixelSizeInViewport = 0.0, logicalSizeInViewport = 0.0;
            double pixelSizeBeforeViewport = 0.0, logicalSizeBeforeViewport = 0.0;
            double pixelSizeAfterViewport = 0.0, logicalSizeAfterViewport = 0.0;

            if (isHorizontal)
            {
                //
                // Split the child's sizes into portions before, after and within the viewport
                //

                if (IsPixelBased)
                {
                    if (childIsHorizontal != isHorizontal)
                    {
                        //
                        // If the child is beyond the viewport in the opposite orientation to this panel then we musn't count that child in.
                        //
                        if (DoubleUtil.GreaterThanOrClose(childViewport.Y, childPixelSize.Height) ||
                            DoubleUtil.AreClose(childViewport.Height, 0.0))
                        {
                            return;
                        }
                    }

                    pixelSizeBeforeViewport = DoubleUtil.LessThan(childViewport.X, childPixelSize.Width) ? Math.Max(childViewport.X, 0.0) : childPixelSize.Width;
                    pixelSizeInViewport = Math.Min(childViewport.Width, childPixelSize.Width - pixelSizeBeforeViewport);
                    pixelSizeAfterViewport = Math.Max(childPixelSize.Width - pixelSizeInViewport - pixelSizeBeforeViewport, 0.0); // Please note that due to rounding errors this subtraction can lead to negative values. Hence the Math.Max call
                }
                else
                {
                    if (childIsHorizontal != isHorizontal)
                    {
                        //
                        // If the child is beyond the viewport in the opposite orientation to this panel then we musn't count that child in.
                        //
                        if (DoubleUtil.GreaterThanOrClose(childViewport.Y, childLogicalSize.Height) ||
                            DoubleUtil.AreClose(childViewport.Height, 0.0))
                        {
                            return;
                        }
                    }

                    if (DoubleUtil.GreaterThanOrClose(childViewport.X, childLogicalSize.Width))
                    {
                        pixelSizeBeforeViewport = childPixelSize.Width;
                    }
                    else
                    {
                        if (DoubleUtil.GreaterThan(childViewport.Width, 0.0))
                        {
                            pixelSizeInViewport = childPixelSize.Width;
                        }
                        else
                        {
                            pixelSizeAfterViewport = childPixelSize.Width;
                        }
                    }
                }

                Debug.Assert(DoubleUtil.AreClose(pixelSizeInViewport + pixelSizeBeforeViewport + pixelSizeAfterViewport, childPixelSize.Width), "The computed sizes within and outside the viewport should add up to the childPixelSize.");

                if (DoubleUtil.GreaterThan(childPixelSize.Width, 0.0))
                {
                    logicalSizeBeforeViewport = Math.Floor(childLogicalSize.Width * pixelSizeBeforeViewport / childPixelSize.Width);
                    logicalSizeAfterViewport = Math.Floor(childLogicalSize.Width * pixelSizeAfterViewport / childPixelSize.Width);
                    logicalSizeInViewport = childLogicalSize.Width - logicalSizeBeforeViewport - logicalSizeAfterViewport;
                }

                double childPixelHeightInViewport = Math.Min(childViewport.Height, childPixelSize.Height - Math.Max(childViewport.Y, 0.0));

                childPixelSizeInViewport.Width += pixelSizeInViewport;
                childPixelSizeInViewport.Height = Math.Max(childPixelSizeInViewport.Height, childPixelHeightInViewport);
                childPixelSizeInCacheBeforeViewport.Width += pixelSizeBeforeViewport;
                childPixelSizeInCacheBeforeViewport.Height = Math.Max(childPixelSizeInCacheBeforeViewport.Height, childPixelHeightInViewport);
                childPixelSizeInCacheAfterViewport.Width += pixelSizeAfterViewport;
                childPixelSizeInCacheAfterViewport.Height = Math.Max(childPixelSizeInCacheAfterViewport.Height, childPixelHeightInViewport);

                childLogicalSizeInViewport.Width += logicalSizeInViewport;
                childLogicalSizeInViewport.Height = Math.Max(childLogicalSizeInViewport.Height, childLogicalSize.Height);
                childLogicalSizeInCacheBeforeViewport.Width += logicalSizeBeforeViewport;
                childLogicalSizeInCacheBeforeViewport.Height = Math.Max(childLogicalSizeInCacheBeforeViewport.Height, childLogicalSize.Height);
                childLogicalSizeInCacheAfterViewport.Width += logicalSizeAfterViewport;
                childLogicalSizeInCacheAfterViewport.Height = Math.Max(childLogicalSizeInCacheAfterViewport.Height, childLogicalSize.Height);
            }
            else
            {
                //
                // Split the child's sizes into portions before, after and within the viewport
                //

                if (IsPixelBased)
                {
                    if (childIsHorizontal != isHorizontal)
                    {
                        //
                        // If the child is beyond the viewport in the opposite orientation to this panel then we musn't count that child in.
                        //
                        if (DoubleUtil.GreaterThanOrClose(childViewport.X, childPixelSize.Width) ||
                            DoubleUtil.AreClose(childViewport.Width, 0.0))
                        {
                            return;
                        }
                    }

                    pixelSizeBeforeViewport = DoubleUtil.LessThan(childViewport.Y, childPixelSize.Height) ? Math.Max(childViewport.Y, 0.0) : childPixelSize.Height;
                    pixelSizeInViewport = Math.Min(childViewport.Height, childPixelSize.Height - pixelSizeBeforeViewport);
                    pixelSizeAfterViewport = Math.Max(childPixelSize.Height - pixelSizeInViewport - pixelSizeBeforeViewport, 0.0); // Please note that due to rounding errors this subtraction can lead to negative values. Hence the Math.Max call
                }
                else
                {
                    if (childIsHorizontal != isHorizontal)
                    {
                        //
                        // If the child is beyond the viewport in the opposite orientation to this panel then we musn't count that child in.
                        //
                        if (DoubleUtil.GreaterThanOrClose(childViewport.X, childLogicalSize.Width) ||
                            DoubleUtil.AreClose(childViewport.Width, 0.0))
                        {
                            return;
                        }
                    }

                    if (DoubleUtil.GreaterThanOrClose(childViewport.Y, childLogicalSize.Height))
                    {
                        pixelSizeBeforeViewport = childPixelSize.Height;
                    }
                    else
                    {
                        if (DoubleUtil.GreaterThan(childViewport.Height, 0.0))
                        {
                            pixelSizeInViewport = childPixelSize.Height;
                        }
                        else
                        {
                            pixelSizeAfterViewport = childPixelSize.Height;
                        }
                    }
                }

                Debug.Assert(DoubleUtil.AreClose(pixelSizeInViewport + pixelSizeBeforeViewport + pixelSizeAfterViewport, childPixelSize.Height), "The computed sizes within and outside the viewport should add up to the childPixelSize.");

                if (DoubleUtil.GreaterThan(childPixelSize.Height, 0.0))
                {
                    logicalSizeBeforeViewport = Math.Floor(childLogicalSize.Height * pixelSizeBeforeViewport / childPixelSize.Height);
                    logicalSizeAfterViewport = Math.Floor(childLogicalSize.Height * pixelSizeAfterViewport / childPixelSize.Height);
                    logicalSizeInViewport = childLogicalSize.Height - logicalSizeBeforeViewport - logicalSizeAfterViewport;
                }

                double childPixelWidthInViewport = Math.Min(childViewport.Width, childPixelSize.Width - Math.Max(childViewport.X, 0.0));

                childPixelSizeInViewport.Height += pixelSizeInViewport;
                childPixelSizeInViewport.Width = Math.Max(childPixelSizeInViewport.Width, childPixelWidthInViewport);
                childPixelSizeInCacheBeforeViewport.Height += pixelSizeBeforeViewport;
                childPixelSizeInCacheBeforeViewport.Width = Math.Max(childPixelSizeInCacheBeforeViewport.Width, childPixelWidthInViewport);
                childPixelSizeInCacheAfterViewport.Height += pixelSizeAfterViewport;
                childPixelSizeInCacheAfterViewport.Width = Math.Max(childPixelSizeInCacheAfterViewport.Width, childPixelWidthInViewport);

                childLogicalSizeInViewport.Height += logicalSizeInViewport;
                childLogicalSizeInViewport.Width = Math.Max(childLogicalSizeInViewport.Width, childLogicalSize.Width);
                childLogicalSizeInCacheBeforeViewport.Height += logicalSizeBeforeViewport;
                childLogicalSizeInCacheBeforeViewport.Width = Math.Max(childLogicalSizeInCacheBeforeViewport.Width, childLogicalSize.Width);
                childLogicalSizeInCacheAfterViewport.Height += logicalSizeAfterViewport;
                childLogicalSizeInCacheAfterViewport.Width = Math.Max(childLogicalSizeInCacheAfterViewport.Width, childLogicalSize.Width);
            }
        }

        private void UpdateStackSizes(
            bool isHorizontal,
            bool foundFirstItemInViewport,
            Size childPixelSize,
            Size childPixelSizeInViewport,
            Size childPixelSizeInCacheBeforeViewport,
            Size childPixelSizeInCacheAfterViewport,
            Size childLogicalSize,
            Size childLogicalSizeInViewport,
            Size childLogicalSizeInCacheBeforeViewport,
            Size childLogicalSizeInCacheAfterViewport,
            ref Size stackPixelSize,
            ref Size stackPixelSizeInViewport,
            ref Size stackPixelSizeInCacheBeforeViewport,
            ref Size stackPixelSizeInCacheAfterViewport,
            ref Size stackLogicalSize,
            ref Size stackLogicalSizeInViewport,
            ref Size stackLogicalSizeInCacheBeforeViewport,
            ref Size stackLogicalSizeInCacheAfterViewport)
        {
            StackSizes(isHorizontal, ref stackPixelSize, childPixelSize);
            StackSizes(isHorizontal, ref stackLogicalSize, childLogicalSize);

            if (foundFirstItemInViewport)
            {
                StackSizes(isHorizontal, ref stackPixelSizeInViewport, childPixelSizeInViewport);
                StackSizes(isHorizontal, ref stackLogicalSizeInViewport, childLogicalSizeInViewport);
                StackSizes(isHorizontal, ref stackPixelSizeInCacheBeforeViewport, childPixelSizeInCacheBeforeViewport);
                StackSizes(isHorizontal, ref stackLogicalSizeInCacheBeforeViewport, childLogicalSizeInCacheBeforeViewport);
                StackSizes(isHorizontal, ref stackPixelSizeInCacheAfterViewport, childPixelSizeInCacheAfterViewport);
                StackSizes(isHorizontal, ref stackLogicalSizeInCacheAfterViewport, childLogicalSizeInCacheAfterViewport);
            }
        }

        private static void StackSizes(bool isHorizontal, ref Size sz1, Size sz2)
        {
            if (isHorizontal)
            {
                sz1.Width += sz2.Width;
                sz1.Height = Math.Max(sz1.Height, sz2.Height);
            }
            else
            {
                sz1.Height += sz2.Height;
                sz1.Width = Math.Max(sz1.Width, sz2.Width);
            }
        }

        private void SyncUniformSizeFlags(
            object parentItem,
            IList children,
            IList items,
            IContainItemStorage itemStorageProvider,
            int itemCount,
            bool computedAreContainersUniformlySized,
            double computedUniformOrAverageContainerSize,
            ref bool areContainersUniformlySized,
            ref double uniformOrAverageContainerSize,
            bool isHorizontal,
            bool evaluateAreContainersUniformlySized)
        {
            if (evaluateAreContainersUniformlySized || areContainersUniformlySized != computedAreContainersUniformlySized)
            {
                Debug.Assert(evaluateAreContainersUniformlySized || !computedAreContainersUniformlySized, "AreContainersUniformlySized starts off true and can only be flipped to false.");

                if (!evaluateAreContainersUniformlySized)
                {
                    areContainersUniformlySized = computedAreContainersUniformlySized;
                    SetAreContainersUniformlySized(itemStorageProvider, parentItem, areContainersUniformlySized);
                }

                for (int i=0; i < children.Count; i++)
                {
                    UIElement child = children[i] as UIElement;
                    if (child != null && VirtualizingPanel.GetShouldCacheContainerSize(child))
                    {
                        IHierarchicalVirtualizationAndScrollInfo virtualizingChild  = GetVirtualizingChild(child);

                        Size childSize;

                        if (virtualizingChild != null)
                        {
                            HierarchicalVirtualizationHeaderDesiredSizes headerDesiredSizes = virtualizingChild.HeaderDesiredSizes;
                            HierarchicalVirtualizationItemDesiredSizes itemDesiredSizes = virtualizingChild.ItemDesiredSizes;

                            if (IsPixelBased)
                            {
                                childSize = new Size(Math.Max(headerDesiredSizes.PixelSize.Width, itemDesiredSizes.PixelSize.Width),
                                                              headerDesiredSizes.PixelSize.Height + itemDesiredSizes.PixelSize.Height);
                            }
                            else
                            {
                                childSize = new Size(Math.Max(headerDesiredSizes.LogicalSize.Width, itemDesiredSizes.LogicalSize.Width),
                                                              headerDesiredSizes.LogicalSize.Height + itemDesiredSizes.LogicalSize.Height);
                            }
                        }
                        else
                        {
                            if (IsPixelBased)
                            {
                                childSize = child.DesiredSize;
                            }
                            else
                            {
                                childSize = new Size(DoubleUtil.GreaterThan(child.DesiredSize.Width, 0) ? 1 : 0,
                                                     DoubleUtil.GreaterThan(child.DesiredSize.Height, 0) ? 1 : 0);
                            }
                        }

                        if (evaluateAreContainersUniformlySized && computedAreContainersUniformlySized)
                        {
                            if (isHorizontal)
                            {
                                computedAreContainersUniformlySized = DoubleUtil.AreClose(childSize.Width, uniformOrAverageContainerSize);
                            }
                            else
                            {
                                computedAreContainersUniformlySized = DoubleUtil.AreClose(childSize.Height, uniformOrAverageContainerSize);
                            }

                            if (!computedAreContainersUniformlySized)
                            {
                                // We need to restart the loop and cache
                                // the sizes of all children prior to this one

                                i = -1;
                            }
                        }
                        else
                        {
                            itemStorageProvider.StoreItemValue(((ItemContainerGenerator)Generator).ItemFromContainer(child), ContainerSizeProperty, childSize);
                        }
                    }
                }

                if (evaluateAreContainersUniformlySized)
                {
                    areContainersUniformlySized = computedAreContainersUniformlySized;
                    SetAreContainersUniformlySized(itemStorageProvider, parentItem, areContainersUniformlySized);
                }
            }

            if (!computedAreContainersUniformlySized)
            {
                Size containerSize;
                double sumOfContainerSizes = 0;
                int numContainerSizes = 0;

                for (int i=0; i<itemCount; i++)
                {
                    object value = itemStorageProvider.ReadItemValue(items[i], ContainerSizeProperty);
                    if (value != null)
                    {
                        containerSize = (Size)value;

                        if (isHorizontal)
                        {
                            sumOfContainerSizes += containerSize.Width;
                            numContainerSizes++;
                        }
                        else
                        {
                            sumOfContainerSizes += containerSize.Height;
                            numContainerSizes++;
                        }
                    }
                }

                if (DoubleUtil.GreaterThan(numContainerSizes, 0))
                {
                    if (IsPixelBased)
                    {
                        uniformOrAverageContainerSize = sumOfContainerSizes / numContainerSizes;
                    }
                    else
                    {
                        uniformOrAverageContainerSize = Math.Round(sumOfContainerSizes / numContainerSizes);
                    }

                    SetUniformOrAverageContainerSize(itemStorageProvider, parentItem, uniformOrAverageContainerSize);
                }
            }
            else
            {
                uniformOrAverageContainerSize = computedUniformOrAverageContainerSize;
            }
        }

        private void ClearAsyncOperations()
        {
            DispatcherOperation measureCachesOperation = MeasureCachesOperationField.GetValue(this);
            if (measureCachesOperation != null)
            {
                measureCachesOperation.Abort();
                MeasureCachesOperationField.ClearValue(this);
            }

            DispatcherOperation anchorOperation = AnchorOperationField.GetValue(this);
            if (anchorOperation != null)
            {
                anchorOperation.Abort();
                AnchorOperationField.ClearValue(this);
            }

            DispatcherOperation anchoredInvalidateMeasureOperation = AnchoredInvalidateMeasureOperationField.GetValue(this);
            if (anchoredInvalidateMeasureOperation != null)
            {
                anchoredInvalidateMeasureOperation.Abort();
                AnchoredInvalidateMeasureOperationField.ClearValue(this);
            }

            DispatcherOperation clearIsScrollActiveOperation = ClearIsScrollActiveOperationField.GetValue(this);
            if (clearIsScrollActiveOperation != null)
            {
                clearIsScrollActiveOperation.Abort();
                ClearIsScrollActiveOperationField.ClearValue(this);
            }
        }

        private bool GetAreContainersUniformlySized(IContainItemStorage itemStorageProvider, object item)
        {
            Debug.Assert(itemStorageProvider != null || item == this, "An item storage provider must be available.");
            Debug.Assert(item != null, "An item must be available.");

            if (item == this)
            {
                if (AreContainersUniformlySized.HasValue)
                {
                    // Return the cached value if for VSP and if present.
                    return (bool)AreContainersUniformlySized;
                }
            }
            else
            {
                object value = itemStorageProvider.ReadItemValue(item, AreContainersUniformlySizedProperty);
                if (value != null)
                {
                    return (bool)value;
                }
            }

            return true;
        }

        private void SetAreContainersUniformlySized(IContainItemStorage itemStorageProvider, object item, bool value)
        {
            Debug.Assert(itemStorageProvider != null || item == this, "An item storage provider must be available.");
            Debug.Assert(item != null, "An item must be available.");

            if (item == this)
            {
                // Set the cache if for VSP.
                AreContainersUniformlySized = value;
            }
            else
            {
                itemStorageProvider.StoreItemValue(item, AreContainersUniformlySizedProperty, value);
            }
        }

        private double GetUniformOrAverageContainerSize(IContainItemStorage itemStorageProvider, object item)
        {
            bool hasUniformOrAverageContainerSizeBeenSet;
            return GetUniformOrAverageContainerSize(itemStorageProvider, item, out hasUniformOrAverageContainerSizeBeenSet);
        }

        private double GetUniformOrAverageContainerSize(IContainItemStorage itemStorageProvider, object item, out bool hasUniformOrAverageContainerSizeBeenSet)
        {
            Debug.Assert(itemStorageProvider != null || item == this, "An item storage provider must be available.");
            Debug.Assert(item != null, "An item must be available.");

            if (item == this)
            {
                if (UniformOrAverageContainerSize.HasValue)
                {
                    // Return the cached value if for VSP and if present.
                    hasUniformOrAverageContainerSizeBeenSet = true;
                    return (double)UniformOrAverageContainerSize;
                }
            }
            else
            {
                object value = itemStorageProvider.ReadItemValue(item, UniformOrAverageContainerSizeProperty);
                if (value != null)
                {
                    hasUniformOrAverageContainerSizeBeenSet = true;
                    return (double)value;
                }
            }

            hasUniformOrAverageContainerSizeBeenSet = false;
            return IsPixelBased ? ScrollViewer._scrollLineDelta : 1.0;
        }

        private void SetUniformOrAverageContainerSize(IContainItemStorage itemStorageProvider, object item, double value)
        {
            Debug.Assert(itemStorageProvider != null || item == this, "An item storage provider must be available.");
            Debug.Assert(item != null, "An item must be available.");

            //
            // This case was detected when entering a ListBoxItem into the ListBox through the XAML editor
            // in VS. In this case the ListBoxItem is empty and is of zero size. We do not want to record the
            // size of that ListBoxItem as the uniformOrAverageSize because on the next measure this will
            // lead to a divide by zero error when computing the firstItemInViewportIndex.
            //
            if (DoubleUtil.GreaterThan(value, 0))
            {
                if (item == this)
                {
                    // Set the cache if for VSP.
                    UniformOrAverageContainerSize = value;
                }
                else
                {
                    itemStorageProvider.StoreItemValue(item, UniformOrAverageContainerSizeProperty, value);
                }
            }
        }

        private void MeasureExistingChildBeyondExtendedViewport(
            ref IItemContainerGenerator generator,
            ref IContainItemStorage itemStorageProvider,
            ref object parentItem,
            ref bool hasUniformOrAverageContainerSizeBeenSet,
            ref double computedUniformOrAverageContainerSize,
            ref bool computedAreContainersUniformlySized,
            ref IList items,
            ref IList children,
            ref int childIndex,
            ref bool visualOrderChanged,
            ref bool isHorizontal,
            ref Size childConstraint,
            ref bool foundFirstItemInViewport,
            ref double firstItemInViewportOffset,
            ref bool mustDisableVirtualization,
            ref bool hasVirtualizingChildren,
            ref bool hasBringIntoViewContainerBeenMeasured)
        {
            object item = ((ItemContainerGenerator)generator).ItemFromContainer((UIElement)children[childIndex]);
            Rect viewport = new Rect();
            VirtualizationCacheLength cacheSize = new VirtualizationCacheLength();
            VirtualizationCacheLengthUnit cacheUnit = VirtualizationCacheLengthUnit.Pixel;
            Size stackPixelSize = new Size();
            Size stackPixelSizeInViewport = new Size();
            Size stackPixelSizeInCacheBeforeViewport = new Size();
            Size stackPixelSizeInCacheAfterViewport = new Size();
            Size stackLogicalSize = new Size();
            Size stackLogicalSizeInViewport = new Size();
            Size stackLogicalSizeInCacheBeforeViewport = new Size();
            Size stackLogicalSizeInCacheAfterViewport = new Size();
            bool isBeforeFirstItem = childIndex < _firstItemInExtendedViewportChildIndex;
            bool isAfterFirstItem = childIndex > _firstItemInExtendedViewportChildIndex;
            bool isAfterLastItem = childIndex > _firstItemInExtendedViewportChildIndex + _actualItemsInExtendedViewportCount;
            bool skipActualMeasure = false;
            bool skipGeneration = true;

            MeasureChild(
                ref generator,
                ref itemStorageProvider,
                ref parentItem,
                ref hasUniformOrAverageContainerSizeBeenSet,
                ref computedUniformOrAverageContainerSize,
                ref computedAreContainersUniformlySized,
                ref items,
                ref item,
                ref children,
                ref childIndex,
                ref visualOrderChanged,
                ref isHorizontal,
                ref childConstraint,
                ref viewport,
                ref cacheSize,
                ref cacheUnit,
                ref foundFirstItemInViewport,
                ref firstItemInViewportOffset,
                ref stackPixelSize,
                ref stackPixelSizeInViewport,
                ref stackPixelSizeInCacheBeforeViewport,
                ref stackPixelSizeInCacheAfterViewport,
                ref stackLogicalSize,
                ref stackLogicalSizeInViewport,
                ref stackLogicalSizeInCacheBeforeViewport,
                ref stackLogicalSizeInCacheAfterViewport,
                ref mustDisableVirtualization,
                isBeforeFirstItem,
                isAfterFirstItem,
                isAfterLastItem,
                skipActualMeasure,
                skipGeneration,
                ref hasBringIntoViewContainerBeenMeasured,
                ref hasVirtualizingChildren);
        }

        private void MeasureChild(
            ref IItemContainerGenerator generator,
            ref IContainItemStorage itemStorageProvider,
            ref object parentItem,
            ref bool hasUniformOrAverageContainerSizeBeenSet,
            ref double computedUniformOrAverageContainerSize,
            ref bool computedAreContainersUniformlySized,
            ref IList items,
            ref object item,
            ref IList children,
            ref int childIndex,
            ref bool visualOrderChanged,
            ref bool isHorizontal,
            ref Size childConstraint,
            ref Rect viewport,
            ref VirtualizationCacheLength cacheSize,
            ref VirtualizationCacheLengthUnit cacheUnit,
            ref bool foundFirstItemInViewport,
            ref double firstItemInViewportOffset,
            ref Size stackPixelSize,
            ref Size stackPixelSizeInViewport,
            ref Size stackPixelSizeInCacheBeforeViewport,
            ref Size stackPixelSizeInCacheAfterViewport,
            ref Size stackLogicalSize,
            ref Size stackLogicalSizeInViewport,
            ref Size stackLogicalSizeInCacheBeforeViewport,
            ref Size stackLogicalSizeInCacheAfterViewport,
            ref bool mustDisableVirtualization,
            bool isBeforeFirstItem,
            bool isAfterFirstItem,
            bool isAfterLastItem,
            bool skipActualMeasure,
            bool skipGeneration,
            ref bool hasBringIntoViewContainerBeenMeasured,
            ref bool hasVirtualizingChildren)
        {
            UIElement child = null;
            IHierarchicalVirtualizationAndScrollInfo virtualizingChild = null;
            Rect childViewport = Rect.Empty;
            VirtualizationCacheLength childCacheSize = new VirtualizationCacheLength(0.0);
            VirtualizationCacheLengthUnit childCacheUnit = VirtualizationCacheLengthUnit.Pixel;
            Size childDesiredSize = new Size();

            //
            // Get and connect the next child.
            //
            if (!skipActualMeasure && !skipGeneration)
            {
                bool newlyRealized;
                child = generator.GenerateNext(out newlyRealized) as UIElement;

                ItemContainerGenerator icg;
                if (child == null && (icg = generator as ItemContainerGenerator) != null)
                {
                    icg.Verify();
                }

                visualOrderChanged |= AddContainerFromGenerator(childIndex, child, newlyRealized, isBeforeFirstItem);
            }
            else
            {
                child = (UIElement)children[childIndex];
            }

            hasBringIntoViewContainerBeenMeasured |= (child == _bringIntoViewContainer);

            //
            // Set viewport constraints
            //

            bool isChildHorizontal = isHorizontal;
            virtualizingChild = GetVirtualizingChild(child, ref isChildHorizontal);

            SetViewportForChild(
                isHorizontal,
                itemStorageProvider,
                computedAreContainersUniformlySized,
                computedUniformOrAverageContainerSize,
                mustDisableVirtualization,
                child,
                virtualizingChild,
                item,
                isBeforeFirstItem,
                isAfterFirstItem,
                firstItemInViewportOffset,
                viewport,
                cacheSize,
                cacheUnit,
                stackPixelSize,
                stackPixelSizeInViewport,
                stackPixelSizeInCacheBeforeViewport,
                stackPixelSizeInCacheAfterViewport,
                stackLogicalSize,
                stackLogicalSizeInViewport,
                stackLogicalSizeInCacheBeforeViewport,
                stackLogicalSizeInCacheAfterViewport,
                out childViewport,
                ref childCacheSize,
                ref childCacheUnit);

            //
            // Measure the child
            //

            if (!skipActualMeasure)
            {
                child.Measure(childConstraint);
            }

            childDesiredSize = child.DesiredSize;

            //
            // Accumulate child size.
            //

            if (virtualizingChild != null)
            {
                //
                // Update the virtualizingChild once more to really be sure
                // that we can trust the Desired values from it. Previously
                // we may have bypassed some checks because the ItemsHost
                // wasn't connected.
                //
                virtualizingChild = GetVirtualizingChild(child, ref isChildHorizontal);

                mustDisableVirtualization |=
                    (virtualizingChild != null && virtualizingChild.MustDisableVirtualization) ||
                    isChildHorizontal != isHorizontal;
            }

            Size childPixelSize, childPixelSizeInViewport, childPixelSizeInCacheBeforeViewport, childPixelSizeInCacheAfterViewport;
            Size childLogicalSize, childLogicalSizeInViewport, childLogicalSizeInCacheBeforeViewport, childLogicalSizeInCacheAfterViewport;

            GetSizesForChild(
                isHorizontal,
                isChildHorizontal,
                isBeforeFirstItem,
                isAfterLastItem,
                virtualizingChild,
                childDesiredSize,
                childViewport,
                childCacheSize,
                childCacheUnit,
                out childPixelSize,
                out childPixelSizeInViewport,
                out childPixelSizeInCacheBeforeViewport,
                out childPixelSizeInCacheAfterViewport,
                out childLogicalSize,
                out childLogicalSizeInViewport,
                out childLogicalSizeInCacheBeforeViewport,
                out childLogicalSizeInCacheAfterViewport);

            UpdateStackSizes(
                isHorizontal,
                foundFirstItemInViewport,
                childPixelSize,
                childPixelSizeInViewport,
                childPixelSizeInCacheBeforeViewport,
                childPixelSizeInCacheAfterViewport,
                childLogicalSize,
                childLogicalSizeInViewport,
                childLogicalSizeInCacheBeforeViewport,
                childLogicalSizeInCacheAfterViewport,
                ref stackPixelSize,
                ref stackPixelSizeInViewport,
                ref stackPixelSizeInCacheBeforeViewport,
                ref stackPixelSizeInCacheAfterViewport,
                ref stackLogicalSize,
                ref stackLogicalSizeInViewport,
                ref stackLogicalSizeInCacheBeforeViewport,
                ref stackLogicalSizeInCacheAfterViewport);

            //
            // Cache the container size.
            //

            if (VirtualizingPanel.GetShouldCacheContainerSize(child))
            {
                SetContainerSizeForItem(
                    itemStorageProvider,
                    parentItem,
                    item,
                    IsPixelBased ? childPixelSize : childLogicalSize,
                    isHorizontal,
                    ref hasUniformOrAverageContainerSizeBeenSet,
                    ref computedUniformOrAverageContainerSize,
                    ref computedAreContainersUniformlySized);
            }

            if (virtualizingChild != null)
            {
                hasVirtualizingChildren = true;
            }
        }

        private void ArrangeFirstItemInExtendedViewport(
            bool isHorizontal,
            UIElement child,
            Size childDesiredSize,
            double arrangeLength,
            ref Rect rcChild,
            ref Size previousChildSize,
            ref Point previousChildOffset,
            ref int previousChildItemIndex)
        {
            //
            // This is the first child in the extendedViewport. Initialize the child rect.
            // This is required because there may have been other children
            // outside the viewport that were previously arranged and hence
            // mangled the child rect struct.
            //
            rcChild.X = 0.0;
            rcChild.Y = 0.0;

            if (IsScrolling)
            {
                //
                // This is the scrolling panel. Offset the arrange rect with
                // respect to the viewport.
                //
                if (!IsPixelBased)
                {
                    if (isHorizontal)
                    {
                        rcChild.X = -1.0 * _previousStackPixelSizeInCacheBeforeViewport.Width;
                        rcChild.Y = -1.0 * _scrollData._computedOffset.Y;
                    }
                    else
                    {
                        rcChild.Y = -1.0 * _previousStackPixelSizeInCacheBeforeViewport.Height;
                        rcChild.X = -1.0 * _scrollData._computedOffset.X;
                    }
                }
                else
                {
                    rcChild.X = -1.0 * _scrollData._computedOffset.X;
                    rcChild.Y = -1.0 * _scrollData._computedOffset.Y;
                }
            }

            if (IsVirtualizing && IsPixelBased)
            {
                if (isHorizontal)
                {
                    rcChild.X += _firstItemInExtendedViewportOffset;
                }
                else
                {
                    rcChild.Y += _firstItemInExtendedViewportOffset;
                }
            }

            bool isChildHorizontal = isHorizontal;
            IHierarchicalVirtualizationAndScrollInfo virtualizingChild = GetVirtualizingChild(child, ref isChildHorizontal);

            if (isHorizontal)
            {
                rcChild.Width = childDesiredSize.Width;
                rcChild.Height = Math.Max(arrangeLength, childDesiredSize.Height);
                previousChildSize = childDesiredSize;

                if (!IsPixelBased && virtualizingChild != null)
                {
                    //
                    // For a non leaf item we only want to account for the size in the extended viewport
                    //
                    HierarchicalVirtualizationItemDesiredSizes itemDesiredSizes = virtualizingChild.ItemDesiredSizes;
                    previousChildSize.Width = itemDesiredSizes.PixelSizeInViewport.Width;

                    if (isChildHorizontal == isHorizontal)
                    {
                        previousChildSize.Width += itemDesiredSizes.PixelSizeBeforeViewport.Width + itemDesiredSizes.PixelSizeAfterViewport.Width;
                    }

                    RelativeHeaderPosition headerPosition = RelativeHeaderPosition.Top; // virtualizingChild.RelativeHeaderPosition;
                    Size pixelHeaderSize = virtualizingChild.HeaderDesiredSizes.PixelSize;
                    if (headerPosition == RelativeHeaderPosition.Left || headerPosition == RelativeHeaderPosition.Right)
                    {
                        previousChildSize.Width += pixelHeaderSize.Width;
                    }
                    else
                    {
                        previousChildSize.Width = Math.Max(previousChildSize.Width, pixelHeaderSize.Width);
                    }
                }
            }
            else
            {
                rcChild.Height = childDesiredSize.Height;
                rcChild.Width = Math.Max(arrangeLength, childDesiredSize.Width);
                previousChildSize = childDesiredSize;

                if (!IsPixelBased && virtualizingChild != null)
                {
                    //
                    // For a non leaf item we only want to account for the size in the extended viewport
                    //
                    HierarchicalVirtualizationItemDesiredSizes itemDesiredSizes = virtualizingChild.ItemDesiredSizes;
                    previousChildSize.Height = itemDesiredSizes.PixelSizeInViewport.Height;

                    if (isChildHorizontal == isHorizontal)
                    {
                        previousChildSize.Height += itemDesiredSizes.PixelSizeBeforeViewport.Height + itemDesiredSizes.PixelSizeAfterViewport.Height;
                    }

                    RelativeHeaderPosition headerPosition = RelativeHeaderPosition.Top; // virtualizingChild.RelativeHeaderPosition;
                    Size pixelHeaderSize = virtualizingChild.HeaderDesiredSizes.PixelSize;
                    if (headerPosition == RelativeHeaderPosition.Top || headerPosition == RelativeHeaderPosition.Bottom)
                    {
                        previousChildSize.Height += pixelHeaderSize.Height;
                    }
                    else
                    {
                        previousChildSize.Height = Math.Max(previousChildSize.Height, pixelHeaderSize.Height);
                    }
                }
            }

            previousChildItemIndex = _firstItemInExtendedViewportIndex;
            previousChildOffset = rcChild.Location;

            child.Arrange(rcChild);
        }

        private void ArrangeOtherItemsInExtendedViewport(
            bool isHorizontal,
            UIElement child,
            Size childDesiredSize,
            double arrangeLength,
            int index,
            ref Rect rcChild,
            ref Size previousChildSize,
            ref Point previousChildOffset,
            ref int previousChildItemIndex)
        {
            //
            // These are the items within the viewport beyond the first.
            // So they only need to be offset from the previous child.
            //
            if (isHorizontal)
            {
                rcChild.X += previousChildSize.Width;
                rcChild.Width = childDesiredSize.Width;
                rcChild.Height = Math.Max(arrangeLength, childDesiredSize.Height);
            }
            else
            {
                rcChild.Y += previousChildSize.Height;
                rcChild.Height = childDesiredSize.Height;
                rcChild.Width = Math.Max(arrangeLength, childDesiredSize.Width);
            }

            previousChildSize = childDesiredSize;
            previousChildItemIndex = _firstItemInExtendedViewportIndex + (index - _firstItemInExtendedViewportChildIndex);
            previousChildOffset = rcChild.Location;

            child.Arrange(rcChild);
        }

        private void ArrangeItemsBeyondTheExtendedViewport(
            bool isHorizontal,
            UIElement child,
            Size childDesiredSize,
            double arrangeLength,
            IList items,
            IItemContainerGenerator generator,
            IContainItemStorage itemStorageProvider,
            bool areContainersUniformlySized,
            double uniformOrAverageContainerSize,
            bool beforeExtendedViewport,
            ref Rect rcChild,
            ref Size previousChildSize,
            ref Point previousChildOffset,
            ref int previousChildItemIndex)
        {
            //
            // These are the items beyond the viewport. (Eg. Recyclable containers that
            // are waiting until next measure to be cleaned up, Elements that are kept
            // alive because they hold focus.) These element are arranged beyond the
            // visible viewport but at their right position. This is important because these
            // containers need to be brought into view when using keyboard navigation
            // and their arrange rect is what informs the scroillviewer of the right offset
            // to scroll to.
            //
            if (isHorizontal)
            {
                rcChild.Width = childDesiredSize.Width;
                rcChild.Height = Math.Max(arrangeLength, childDesiredSize.Height);

                if (IsPixelBased)
                {
                    int currChildItemIndex = ((ItemContainerGenerator)generator).IndexFromContainer(child, true /*returnLocalIndex*/);
                    double distance;

                    if (beforeExtendedViewport)
                    {
                        if (previousChildItemIndex == -1)
                        {
                            ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, 0, currChildItemIndex, out distance);
                        }
                        else
                        {
                            ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, currChildItemIndex, previousChildItemIndex-currChildItemIndex, out distance);
                        }

                        rcChild.X = previousChildOffset.X - distance;
                        rcChild.Y = previousChildOffset.Y;
                    }
                    else
                    {
                        if (previousChildItemIndex == -1)
                        {
                            ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, 0, currChildItemIndex, out distance);
                        }
                        else
                        {
                            ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, previousChildItemIndex, currChildItemIndex-previousChildItemIndex, out distance);
                        }

                        rcChild.X = previousChildOffset.X + distance;
                        rcChild.Y = previousChildOffset.Y;
                    }

                    previousChildItemIndex = currChildItemIndex;
                }
                else
                {
                    if (beforeExtendedViewport)
                    {
                        rcChild.X -= childDesiredSize.Width;
                    }
                    else
                    {
                        rcChild.X += previousChildSize.Width;
                    }
                }
            }
            else
            {
                rcChild.Height = childDesiredSize.Height;
                rcChild.Width = Math.Max(arrangeLength, childDesiredSize.Width);

                if (IsPixelBased)
                {
                    int currChildItemIndex = ((ItemContainerGenerator)generator).IndexFromContainer(child, true /*returnLocalIndex*/);
                    double distance;

                    if (beforeExtendedViewport)
                    {
                        if (previousChildItemIndex == -1)
                        {
                            ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, 0, currChildItemIndex, out distance);
                        }
                        else
                        {
                            ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, currChildItemIndex, previousChildItemIndex-currChildItemIndex, out distance);
                        }

                        rcChild.Y = previousChildOffset.Y - distance;
                        rcChild.X = previousChildOffset.X;
                    }
                    else
                    {
                        if (previousChildItemIndex == -1)
                        {
                            ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, 0, currChildItemIndex, out distance);
                        }
                        else
                        {
                            ComputeDistance(items, itemStorageProvider, isHorizontal, areContainersUniformlySized, uniformOrAverageContainerSize, previousChildItemIndex, currChildItemIndex-previousChildItemIndex, out distance);
                        }

                        rcChild.Y = previousChildOffset.Y + distance;
                        rcChild.X = previousChildOffset.X;
                    }

                    previousChildItemIndex = currChildItemIndex;
                }
                else
                {
                    if (beforeExtendedViewport)
                    {
                        rcChild.Y -= childDesiredSize.Height;
                    }
                    else
                    {
                        rcChild.Y += previousChildSize.Height;
                    }
                }
            }

            previousChildSize = childDesiredSize;
            previousChildOffset = rcChild.Location;

            child.Arrange(rcChild);
        }

        #endregion

        /// <summary>
        /// Inserts a new container in the visual tree
        /// </summary>
        /// <param name="childIndex"></param>
        /// <param name="container"></param>
        private void InsertNewContainer(int childIndex, UIElement container)
        {
            InsertContainer(childIndex, container, false);
        }

        /// <summary>
        /// Inserts a recycled container in the visual tree
        /// </summary>
        /// <param name="childIndex"></param>
        /// <param name="container"></param>
        /// <returns></returns>
        private bool InsertRecycledContainer(int childIndex, UIElement container)
        {
            return InsertContainer(childIndex, container, true);
        }


        /// <summary>
        /// Inserts a container into the Children collection.  The container is either new or recycled.
        /// </summary>
        /// <param name="childIndex"></param>
        /// <param name="container"></param>
        /// <param name="isRecycled"></param>
        private bool InsertContainer(int childIndex, UIElement container, bool isRecycled)
        {
            Debug.Assert(container != null, "Null container was generated");

            bool visualOrderChanged = false;
            UIElementCollection children = InternalChildren;

            //
            // Find the index in the Children collection where we hope to insert the container.
            // This is done by looking up the index of the container BEFORE the one we hope to insert.
            //
            // We have to do it this way because there could be recycled containers between the container we're looking for and the one before it.
            // By finding the index before the place we want to insert and adding one, we ensure that we'll insert the new container in the
            // proper location.
            //
            // In recycling mode childIndex is the index in the _realizedChildren list, not the index in the
            // Children collection.  We have to convert the index; we'll call the index in the Children collection
            // the visualTreeIndex.
            //

            int visualTreeIndex = 0;

            if (childIndex > 0)
            {
                visualTreeIndex = ChildIndexFromRealizedIndex(childIndex - 1);
                visualTreeIndex++;
            }
            else
            {
                visualTreeIndex = ChildIndexFromRealizedIndex(childIndex);
            }


            if (isRecycled && visualTreeIndex < children.Count && children[visualTreeIndex] == container)
            {
                // Don't insert if a recycled container is in the proper place already
            }
            else
            {
                if (visualTreeIndex < children.Count)
                {
                    int insertIndex = visualTreeIndex;
                    if (isRecycled && container.InternalVisualParent != null)
                    {
                        // If the container is recycled we have to remove it from its place in the visual tree and
                        // insert it in the proper location.   For perf we'll use an internal Move API that moves
                        // the first parameter to right before the second one.
                        Debug.Assert(children[visualTreeIndex] != null, "MoveVisualChild interprets a null destination as 'move to end'");
                        children.MoveVisualChild(container, children[visualTreeIndex]);
                        visualOrderChanged = true;
                    }
                    else
                    {
                        VirtualizingPanel.InsertInternalChild(children, insertIndex, container);
                    }
                }
                else
                {
                    if (isRecycled && container.InternalVisualParent != null)
                    {
                        // Recycled container is still in the tree; move it to the end
                        children.MoveVisualChild(container, null);
                        visualOrderChanged = true;
                    }
                    else
                    {
                        VirtualizingPanel.AddInternalChild(children, container);
                    }
                }
            }

            //
            // Keep realizedChildren in [....] w/ the visual tree.
            //
            if (IsVirtualizing && InRecyclingMode)
            {
                // Dev11 407649 (and related) is a Watson report that childIndex is out
                // of range.  We believe this to arise because VS's VirtualizingTreeView
                // control somehow is able to remove items from the Items collection
                // after childIndex is set (in MeasureOverrideImpl) but before adding
                // the new container (here).    If that should happen, the childIndex
                // is wrong, and the call to Insert will crash.   Instead, just
                // recompute the RealizedChildren collection
                if (ItemsChangedDuringMeasure)
                {
                    _realizedChildren = null;
                }

                if (_realizedChildren != null)
                {
                    _realizedChildren.Insert(childIndex, container);
                }
                else
                {
                    // Creates _realizedChildren list and syncs with InternalChildren.
                    EnsureRealizedChildren();
                }
            }

            Generator.PrepareItemContainer(container);

            return visualOrderChanged;
        }




        private void EnsureCleanupOperation(bool delay)
        {
            if (delay)
            {
                bool noPendingOperations = true;
                if (_cleanupOperation != null)
                {
                    noPendingOperations = _cleanupOperation.Abort();
                    if (noPendingOperations)
                    {
                        _cleanupOperation = null;
                    }
                }
                if (noPendingOperations && (_cleanupDelay == null))
                {
                    _cleanupDelay = new DispatcherTimer();
                    _cleanupDelay.Tick += new EventHandler(OnDelayCleanup);
                    _cleanupDelay.Interval = TimeSpan.FromMilliseconds(500.0);
                    _cleanupDelay.Start();
                }
            }
            else
            {
                if ((_cleanupOperation == null) && (_cleanupDelay == null))
                {
                    _cleanupOperation = Dispatcher.BeginInvoke(DispatcherPriority.Background, new DispatcherOperationCallback(OnCleanUp), null);
                }
            }
        }

        private bool PreviousChildIsGenerated(int childIndex)
        {
            GeneratorPosition position = new GeneratorPosition(childIndex, 0);
            position = Generator.GeneratorPositionFromIndex(Generator.IndexFromGeneratorPosition(position) - 1);
            return (position.Offset == 0 && position.Index >= 0);
        }


        /// <summary>
        /// Takes a container returned from Generator.GenerateNext() and places it in the visual tree if necessary.
        /// Takes into account whether the container is new, recycled, or already realized.
        /// </summary>
        /// <param name="childIndex"></param>
        /// <param name="child"></param>
        /// <param name="newlyRealized"></param>
        private bool AddContainerFromGenerator(int childIndex, UIElement child, bool newlyRealized, bool isBeforeViewport)
        {
            bool visualOrderChanged = false;

            if (!newlyRealized)
            {
                //
                // Container is either realized or recycled.  If it's realized do nothing; it already exists in the visual
                // tree in the proper place.
                //

                if (InRecyclingMode)
                {
                    // Note there's no check for IsVirtualizing here.  If the user has just flipped off virtualization it's possible that
                    // the Generator will still return some recycled containers until its list runs out.

                    IList children = RealizedChildren;

                    if (childIndex < 0 || childIndex >= children.Count || !(children[childIndex] == child))
                    {
                        Debug.Assert(!children.Contains(child), "we incorrectly identified a recycled container");

                        //
                        // We have a recycled container (if it was a realized container it would have been returned in the
                        // proper location).  Note also that recycled containers are NOT in the _realizedChildren list.
                        //

                        visualOrderChanged = InsertRecycledContainer(childIndex, child);
                    }
                    else
                    {
                        // previously realized child.
                    }
                }
#if DEBUG
                else
                {
                    // The following Assert is not valid in the InRibbonGallery scenario, so we skip it.
                    if (this.GetType().Name != "RibbonMenuItemsPanel")
                    {
                        // Not recycling; realized container
                        Debug.Assert(child == InternalChildren[childIndex], "Wrong child was generated");
                    }
                }
#endif
            }
            else
            {
                InsertNewContainer(childIndex, child);
            }

            return visualOrderChanged;
        }

        private void OnItemsRemove(ItemsChangedEventArgs args)
        {
            RemoveChildRange(args.Position, args.ItemCount, args.ItemUICount);
        }

        private void OnItemsReplace(ItemsChangedEventArgs args)
        {
            if (args.ItemUICount > 0)
            {
                Debug.Assert(args.ItemUICount == args.ItemCount, "Both ItemUICount and ItemCount should be equal or ItemUICount should be 0.");
                UIElementCollection children = InternalChildren;

                using (Generator.StartAt(args.Position, GeneratorDirection.Forward, true))
                {
                    for (int i=0; i<args.ItemUICount; ++i)
                    {
                        int childIndex = args.Position.Index + i;
                        bool newlyRealized;
                        UIElement child = Generator.GenerateNext(out newlyRealized) as UIElement;
                        Debug.Assert(child != null, "Null child was generated");
                        Debug.Assert(!newlyRealized, "newlyRealized should be false after Replace");

                        children.SetInternal(childIndex, child);
                        Generator.PrepareItemContainer(child);
                    }
                }
            }
        }

        private void OnItemsMove(ItemsChangedEventArgs args)
        {
            RemoveChildRange(args.OldPosition, args.ItemCount, args.ItemUICount);
        }

        private void RemoveChildRange(GeneratorPosition position, int itemCount, int itemUICount)
        {
            if (IsItemsHost)
            {
                UIElementCollection children = InternalChildren;
                int pos = position.Index;
                if (position.Offset > 0)
                {
                    // An item is being removed after the one at the index
                    pos++;
                }

                if (pos < children.Count)
                {
                    int uiCount = itemUICount;
                    Debug.Assert((itemCount == itemUICount) || (itemUICount == 0), "Both ItemUICount and ItemCount should be equal or ItemUICount should be 0.");
                    if (uiCount > 0)
                    {
                        VirtualizingPanel.RemoveInternalChildRange(children, pos, uiCount);

                        if (IsVirtualizing && InRecyclingMode)
                        {
                            _realizedChildren.RemoveRange(pos, uiCount);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Immediately cleans up any containers that have gone offscreen.  Called by MeasureOverride.
        /// When recycling this runs before generating and measuring children; otherwise it
        /// runs after measuring the children.
        /// </summary>
        private void CleanupContainers(int firstItemInExtendedViewportIndex,
            int itemsInExtendedViewportCount,
            ItemsControl itemsControl)
        {
            CleanupContainers(firstItemInExtendedViewportIndex,
                itemsInExtendedViewportCount,
                itemsControl,
                false /*timeBound*/,
                0 /*startTickCount*/);
        }

        /// <summary>
        /// Immediately cleans up any containers that have gone offscreen.  Called by MeasureOverride.
        /// When recycling this runs before generating and measuring children; otherwise it
        /// runs after measuring the children.
        /// </summary>
        private bool CleanupContainers(int firstItemInExtendedViewportIndex,
            int itemsInExtendedViewportCount,
            ItemsControl itemsControl,
            bool timeBound,
            int startTickCount)
        {
            Debug.Assert(IsVirtualizing, "Can't clean up containers if not virtualizing");
            Debug.Assert(itemsControl != null, "We can't cleanup if we aren't the itemshost");

            //
            // When called it removes children both before and after the viewport.
            //
            // firstItemInViewportIndex is the index of first data item that will be in the viewport
            // at the end of Measure.
            //
            // _firstItemInExtendedViewportIndex and _actualItemsInExtendedViewportCount refer to values from the previous
            // measure pass when this method is called before measuring children.
            // We still use the _actualItemsInExtendedViewportCount as an approximation of the viewport
            // dimensions and prune the children that lay beyond. Occ----ionally
            // if the children are of varying sizes this may lead to a bit of excess work
            // disconnecting and reconnecting a container representing the same item.
            // But the tradeoff is valuable to speed up the more common case where
            // the children are all homogenous. When this method is called after
            // measuring the children these values are valid and can be used to
            // determine the containers that are beyond the viewport.
            //

            IList children = RealizedChildren;
            if (children.Count == 0)
            {
                return false; // nothing to do
            }

            int cleanupRangeStart = -1;
            int cleanupCount = 0;
            int itemIndex = -1, lastItemIndex = -1;

            bool performCleanup = false;
            UIElement child;
            object item;
            bool isVirtualizing = IsVirtualizing;
            bool needsMoreCleanup = false;

            //
            // Iterate over all realized children and recycle or remove the ones
            // that are eligible.  Items NOT eligible for recycling or removal have
            // one or more of the following properties
            //  - inside the viewport
            //  - the item is its own container
            //  - has keyboard focus
            //  - is about to be brought into view
            //  - the CleanupVirtualizedItem event was canceled
            //

            for (int childIndex = 0; childIndex < children.Count; childIndex++)
            {
                if (timeBound)
                {
                    // It is possible for TickCount to wrap around about every 30 days.
                    // If that were to occur, then this particular cleanup may not be interrupted.
                    // That is OK since the worst that can happen is that there is more of a stutter than normal.
                    int totalMilliseconds = Environment.TickCount - startTickCount;
                    if ((totalMilliseconds > 50) && (cleanupCount > 0))
                    {
                        // Cleanup has been working for 50ms already and the user might start
                        // noticing a lag. Stop cleaning up and release the thread for other work.
                        // Cleanup will continue later.
                        // Don't break out until after at least one item has been found to cleanup.
                        // Otherwise, we might end up in an infinite loop.
                        needsMoreCleanup = true;
                        break;
                    }
                }

                child = (UIElement)children[childIndex];
                lastItemIndex = itemIndex;
                itemIndex = GetGeneratedIndex(childIndex);

                //
                // itemsControl.Items can change without notifying VirtualizingStackPanel
                // (when ItemsSource is not an ObservableCollection or does not implement
                // INotifyCollectionChanged). Fetch the item from the container instead of
                // referencing from the Items collection.
                //
                item = itemsControl.ItemContainerGenerator.ItemFromContainer(child);

                if (itemIndex - lastItemIndex != 1)
                {
                    //
                    // There's a generated gap between the current item
                    // and the last.  Clean up the last range of items.
                    //
                    performCleanup = true;
                }

                if (performCleanup)
                {
                    if (cleanupRangeStart >= 0 && cleanupCount > 0)
                    {
                        //
                        // We've hit a non-virtualizable container or a non-contiguous section.
                        //
                        CleanupRange(children, Generator, cleanupRangeStart, cleanupCount);

                        //
                        // CleanupRange just modified the _realizedChildren list.
                        // Adjust the childIndex.
                        //
                        childIndex -= cleanupCount;

                        cleanupCount = 0;
                        cleanupRangeStart = -1;
                    }

                    performCleanup = false;
                }

                if (((itemIndex < firstItemInExtendedViewportIndex) || (itemIndex >= firstItemInExtendedViewportIndex + itemsInExtendedViewportCount)) &&
                    itemIndex >= 0 && // The container is not already disconnected.
                    !((IGeneratorHost)itemsControl).IsItemItsOwnContainer(item) &&
                    !child.IsKeyboardFocusWithin &&
                    child != _bringIntoViewContainer &&
                    NotifyCleanupItem(child, itemsControl) &&
                    VirtualizingPanel.GetIsContainerVirtualizable(child))
                {
                    //
                    // The container is eligible to be virtualized
                    //
                    if (cleanupRangeStart == -1)
                    {
                        cleanupRangeStart = childIndex;
                    }

                    cleanupCount++;
                }
                else
                {
                    // Non-recyclable container;
                    performCleanup = true;
                }
            }

            if (cleanupRangeStart >= 0 && cleanupCount > 0)
            {
                CleanupRange(children, Generator, cleanupRangeStart, cleanupCount);
            }

            return needsMoreCleanup;
        }

        private void EnsureRealizedChildren()
        {
            Debug.Assert(InRecyclingMode, "This method only applies to recycling mode");
            if (_realizedChildren == null)
            {
                UIElementCollection children = InternalChildren;

                _realizedChildren = new List<UIElement>(children.Count);

                for (int i = 0; i < children.Count; i++)
                {
                    _realizedChildren.Add(children[i]);
                }
            }
        }


        [Conditional("DEBUG")]
        private void debug_VerifyRealizedChildren()
        {
            // Debug method that ensures the _realizedChildren list matches the realized containers in the Generator.
            Debug.Assert(IsVirtualizing && InRecyclingMode, "Realized children only exist when recycling");
            Debug.Assert(_realizedChildren != null, "Realized children must exist to verify it");
            System.Windows.Controls.ItemContainerGenerator generator = Generator as System.Windows.Controls.ItemContainerGenerator;
            ItemsControl itemsControl = ItemsControl.GetItemsOwner(this);

            if (generator != null && itemsControl != null && itemsControl.IsGrouping == false)
            {

                foreach (UIElement child in InternalChildren)
                {
                    int dataIndex = generator.IndexFromContainer(child);

                    if (dataIndex == -1)
                    {
                        // Child is not in the generator's realized container list (i.e. it's a recycled container): ensure it's NOT in _realizedChildren.
                        Debug.Assert(!_realizedChildren.Contains(child), "_realizedChildren should not contain recycled containers");
                    }
                    else
                    {
                        // Child is a realized container; ensure it's in _realizedChildren at the proper place.
                        GeneratorPosition position = Generator.GeneratorPositionFromIndex(dataIndex);
                        Debug.Assert(_realizedChildren[position.Index] == child, "_realizedChildren is corrupt!");
                    }
                }
            }
        }

        [Conditional("DEBUG")]
        private void debug_AssertRealizedChildrenEqualVisualChildren()
        {
            if (IsVirtualizing && InRecyclingMode)
            {
                UIElementCollection children = InternalChildren;
                Debug.Assert(_realizedChildren.Count == children.Count, "Realized and visual children must match");

                for (int i = 0; i < children.Count; i++)
                {
                    Debug.Assert(_realizedChildren[i] == children[i], "Realized and visual children must match");
                }
            }
        }

        /// <summary>
        /// Takes an index from the realized list and returns the corresponding index in the Children collection
        /// </summary>
        /// <param name="realizedChildIndex"></param>
        /// <returns></returns>
        private int ChildIndexFromRealizedIndex(int realizedChildIndex)
        {
            //
            // If we're not recycling containers then we're not using a realizedChild index and no translation is necessary
            //
            if (IsVirtualizing && InRecyclingMode)
            {

                if (realizedChildIndex < _realizedChildren.Count)
                {

                    UIElement child = _realizedChildren[realizedChildIndex];
                    UIElementCollection children = InternalChildren;

                    for (int i = realizedChildIndex; i < children.Count; i++)
                    {
                        if (children[i] == child)
                        {
                            return i;
                        }
                    }

                    Debug.Assert(false, "We should have found a child");
                }
            }

            return realizedChildIndex;
        }

        /// <summary>
        /// Recycled containers still in the Children collection at the end of Measure should be disconnected
        /// from the visual tree.  Otherwise they're still visible to things like Arrange, keyboard navigation, etc.
        /// </summary>
        private void DisconnectRecycledContainers()
        {
            int realizedIndex = 0;
            UIElement visualChild;
            UIElement realizedChild = _realizedChildren.Count > 0 ? _realizedChildren[0] : null;
            UIElementCollection children = InternalChildren;

            for (int i = 0; i < children.Count; i++)
            {
                visualChild = children[i];

                if (visualChild == realizedChild)
                {
                    realizedIndex++;

                    if (realizedIndex < _realizedChildren.Count)
                    {

                        realizedChild = _realizedChildren[realizedIndex];
                    }
                    else
                    {
                        realizedChild = null;
                    }
                }
                else
                {
                    // The visual child is a recycled container
                    children.RemoveNoVerify(visualChild);
                    i--;
                }
            }

            debug_VerifyRealizedChildren();
            debug_AssertRealizedChildrenEqualVisualChildren();
        }

        private GeneratorPosition IndexToGeneratorPositionForStart(int index, out int childIndex)
        {
            IItemContainerGenerator generator = Generator;
            GeneratorPosition position = (generator != null) ? generator.GeneratorPositionFromIndex(index) : new GeneratorPosition(-1, index + 1);

            // determine the position in the children collection for the first
            // generated container.  This assumes that generator.StartAt will be called
            // with direction=Forward and  allowStartAtRealizedItem=true.
            childIndex = (position.Offset == 0) ? position.Index : position.Index + 1;

            return position;
        }


        #region Delayed Cleanup Methods

        //
        // Delayed Cleanup is used when the VirtualizationMode is standard (not recycling) and the panel is scrolling and item-based
        // It chooses to defer virtualizing items until there are enough available.  It then cleans them using a background priority dispatcher
        // work item
        //

        private void OnDelayCleanup(object sender, EventArgs e)
        {
            Debug.Assert(_cleanupDelay != null);

            bool needsMoreCleanup = false;

            try
            {
                needsMoreCleanup = CleanUp();
            }
            finally
            {
                // Cleanup the timer if more cleanup is unnecessary
                if (!needsMoreCleanup)
                {
                    _cleanupDelay.Stop();
                    _cleanupDelay = null;
                }
            }
        }

        private object OnCleanUp(object args)
        {
            Debug.Assert(_cleanupOperation != null);

            bool needsMoreCleanup = false;

            try
            {
                needsMoreCleanup = CleanUp();
            }
            finally
            {
                // Keeping this non-null until here in case cleaning up causes re-entrancy
                _cleanupOperation = null;
            }

            if (needsMoreCleanup)
            {
                EnsureCleanupOperation(true /* delay */);
            }

            return null;
        }

        private bool CleanUp()
        {
            Debug.Assert(!InRecyclingMode, "This method only applies to standard virtualization");
            ItemsControl itemsControl = null;
            ItemsControl.GetItemsOwnerInternal(this, out itemsControl);

            if (itemsControl == null || !IsVirtualizing || !IsItemsHost)
            {
                // Virtualization is turned off or we aren't hosting children; no need to cleanup.
                return false;
            }

            int startMilliseconds = Environment.TickCount;
            bool needsMoreCleanup = false;
            UIElementCollection children = InternalChildren;
            int minDesiredGenerated = MinDesiredGenerated;
            int maxDesiredGenerated = MaxDesiredGenerated;
            int pageSize = maxDesiredGenerated - minDesiredGenerated;
            int extraChildren = children.Count - pageSize;

            if (HasVirtualizingChildren || (extraChildren > (pageSize * 2)))
            {
                if ((Mouse.LeftButton == MouseButtonState.Pressed) &&
                    (extraChildren < 1000))
                {
                    // An optimization for when we are dragging the mouse.
                    needsMoreCleanup = true;
                }
                else
                {
                    needsMoreCleanup = CleanupContainers(_firstItemInExtendedViewportIndex,
                        _actualItemsInExtendedViewportCount,
                        itemsControl,
                        true /*timeBound*/,
                        startMilliseconds);
                }
            }

            return needsMoreCleanup;
        }

        private bool NotifyCleanupItem(int childIndex, UIElementCollection children, ItemsControl itemsControl)
        {
            return NotifyCleanupItem(children[childIndex], itemsControl);
        }

        private bool NotifyCleanupItem(UIElement child, ItemsControl itemsControl)
        {
            CleanUpVirtualizedItemEventArgs e = new CleanUpVirtualizedItemEventArgs(itemsControl.ItemContainerGenerator.ItemFromContainer(child), child);
            e.Source = this;
            OnCleanUpVirtualizedItem(e);

            return !e.Cancel;
        }

        private void CleanupRange(IList children, IItemContainerGenerator generator, int startIndex, int count)
        {
            if (InRecyclingMode)
            {
                Debug.Assert(startIndex >= 0 && count > 0);
                Debug.Assert(children == _realizedChildren, "the given child list must be the _realizedChildren list when recycling");

                ((IRecyclingItemContainerGenerator)generator).Recycle(new GeneratorPosition(startIndex, 0), count);

                // The call to Recycle has caused the ItemContainerGenerator to remove some items
                // from its list of realized items; we adjust _realizedChildren to match.
                _realizedChildren.RemoveRange(startIndex, count);
            }
            else
            {
                // Remove the desired range of children
                VirtualizingPanel.RemoveInternalChildRange((UIElementCollection)children, startIndex, count);
                generator.Remove(new GeneratorPosition(startIndex, 0), count);

                // We only need to adjust the childIndex if the visual tree
                // is changing and this is the only case that that happens
                AdjustFirstVisibleChildIndex(startIndex, count);
            }
        }

        #endregion

        /// <summary>
        /// Called after 'count' items were removed or recycled from the Generator.  _firstItemInExtendedViewportChildIndex is the
        /// index of the first visible container.  This index isn't exactly the child position in the UIElement collection;
        /// it's actually the index of the realized container inside the generator.  Since we've just removed some realized
        /// containers from the generator (by calling Remove or Recycle), we have to adjust the first visible child index.
        /// </summary>
        /// <param name="startIndex">index of the first removed item</param>
        /// <param name="count">number of items removed</param>
        private void AdjustFirstVisibleChildIndex(int startIndex, int count)
        {
            //
            // Update the index of the first visible generated child
            //
            if (startIndex < _firstItemInExtendedViewportChildIndex)
            {
                int endIndex = startIndex + count - 1;
                if (endIndex < _firstItemInExtendedViewportChildIndex)
                {
                    // The first visible index is after the items that were removed
                    _firstItemInExtendedViewportChildIndex -= count;
                }
                else
                {
                    // The first visible index was within the items that were removed
                    _firstItemInExtendedViewportChildIndex = startIndex;
                }
            }
        }

        private int MinDesiredGenerated
        {
            get
            {
                return Math.Max(0, _firstItemInExtendedViewportIndex);
            }
        }

        private int MaxDesiredGenerated
        {
            get
            {
                return Math.Min(ItemCount, _firstItemInExtendedViewportIndex + _actualItemsInExtendedViewportCount);
            }
        }

        private int ItemCount
        {
            get
            {
                EnsureGenerator();
                return((ItemContainerGenerator)Generator).ItemsInternal.Count;
            }
        }

        private void EnsureScrollData()
        {
            if (_scrollData == null) { _scrollData = new ScrollData(); }
        }

        private static void ResetScrolling(VirtualizingStackPanel element)
        {
            element.InvalidateMeasure();

            // Clear scrolling data.  Because of thrash (being disconnected & reconnected, &c...), we may
            if (element.IsScrolling)
            {
                element._scrollData.ClearLayout();
            }
        }

        // OnScrollChange is an override called whenever the IScrollInfo exposed scrolling state changes on this element.
        // At the time this method is called, scrolling state is in its new, valid state.
        private void OnScrollChange()
        {
            if (ScrollOwner != null) { ScrollOwner.InvalidateScrollInfo(); }
        }

        /// <summary>
        ///     Sets the scolling Data
        /// </summary>
        /// <returns>The return value indicates if the offset changed or not</returns>
        private void SetAndVerifyScrollingData(
            bool isHorizontal,
            Rect viewport,
            Size constraint,
            ref Size stackPixelSize,
            ref Size stackLogicalSize,
            ref Size stackPixelSizeInViewport,
            ref Size stackLogicalSizeInViewport,
            ref Size stackPixelSizeInCacheBeforeViewport,
            ref Size stackLogicalSizeInCacheBeforeViewport,
            ref bool remeasure,
            ref double? lastPageSafeOffset,
            ref List<double> previouslyMeasuredOffsets)
        {
            Debug.Assert(IsScrolling, "ScrollData must only be set on a scrolling panel.");

            Vector computedViewportOffset, viewportOffset;
            Size viewportSize;
            Size extentSize;

            computedViewportOffset = new Vector(viewport.Location.X, viewport.Location.Y);

            if (IsPixelBased)
            {
                extentSize = stackPixelSize;
                viewportSize = viewport.Size;
            }
            else
            {
                extentSize = stackLogicalSize;
                viewportSize = stackLogicalSizeInViewport;

                if (isHorizontal)
                {
                    if (DoubleUtil.GreaterThan(stackPixelSizeInViewport.Width, constraint.Width) &&
                        viewportSize.Width > 1)
                    {
                        viewportSize.Width--;
                    }

                    viewportSize.Height = viewport.Height;
                }
                else
                {
                    if (DoubleUtil.GreaterThan(stackPixelSizeInViewport.Height, constraint.Height) &&
                        viewportSize.Height > 1)
                    {
                        viewportSize.Height--;
                    }

                    viewportSize.Width = viewport.Width;
                }
            }

            if (isHorizontal)
            {
                if (MeasureCaches && IsVirtualizing)
                {
                    //
                    // We do not want the cache measure pass to affect the visibility
                    // of the scrollbars because this makes bad user experience and
                    // is also the source of scrolling bugs. See Dev11 bug 245561.
                    //

                    stackPixelSize.Height = _scrollData._extent.Height;
                }

                // In order to avoid fluctuations in the minor axis scrollbar visibility
                // as we scroll items of varying dimensions in and out of the viewport,
                // we cache the _maxDesiredSize along that dimension and return that
                // instead.
                _scrollData._maxDesiredSize.Height = Math.Max(_scrollData._maxDesiredSize.Height, stackPixelSize.Height);
                stackPixelSize.Height = _scrollData._maxDesiredSize.Height;

                extentSize.Height = stackPixelSize.Height;

                if (Double.IsPositiveInfinity(constraint.Height))
                {
                    viewportSize.Height = stackPixelSize.Height;
                }
            }
            else
            {
                if (MeasureCaches && IsVirtualizing)
                {
                    //
                    // We do not want the cache measure pass to affect the visibility
                    // of the scrollbars because this makes bad user experience and
                    // is also the source of scrolling bugs. See Dev11 bug 245561.
                    //

                    stackPixelSize.Width = _scrollData._extent.Width;
                }

                // In order to avoid fluctuations in the minor axis scrollbar visibility
                // as we scroll items of varying dimensions in and out of the viewport,
                // we cache the _maxDesiredSize along that dimension and return that
                // instead.
                _scrollData._maxDesiredSize.Width = Math.Max(_scrollData._maxDesiredSize.Width, stackPixelSize.Width);
                stackPixelSize.Width = _scrollData._maxDesiredSize.Width;

                extentSize.Width = stackPixelSize.Width;

                if (Double.IsPositiveInfinity(constraint.Width))
                {
                    viewportSize.Width = stackPixelSize.Width;
                }
            }

            //
            // Since we can offset and clip our content, we never need to be larger than the parent suggestion.
            // If we returned the full size of the content, we would always be so big we didn't need to scroll.  :)
            //
            // Now consider item scrolling cases where we are scrolling to the very last page. In these cases
            // there may be a small region left which is not big enough to fit an entire item. So the sizes of the
            // items accrued within the viewport may be less than the constraint. But we still want to return the
            // constraint dimensions so that we do not unnecessarily cause the parents to be invalidated and
            // re-measured.
            //
            // However if the constraint is infinite, don't change the stack size. This avoids
            // returning an infinite desired size, which is forbidden.
            if (!Double.IsPositiveInfinity(constraint.Width))
            {
                stackPixelSize.Width = IsPixelBased || DoubleUtil.AreClose(computedViewportOffset.X, 0) ?
                    Math.Min(stackPixelSize.Width, constraint.Width) : constraint.Width;
            }
            if (!Double.IsPositiveInfinity(constraint.Height))
            {
                stackPixelSize.Height = IsPixelBased || DoubleUtil.AreClose(computedViewportOffset.Y, 0) ?
                    Math.Min(stackPixelSize.Height, constraint.Height) : constraint.Height;
            }

#if DEBUG
            if (!IsPixelBased)
            {
                // Verify that ViewportSize and ExtentSize are not fractional. Offset can be fractional since it is used to
                // to accumulate the fractional changes, but only the whole value is used for further computations.
                if (isHorizontal)
                {
                    Debug.Assert(DoubleUtil.AreClose(viewportSize.Width - Math.Floor(viewportSize.Width), 0.0), "The viewport size must not contain fractional values when in item scrolling mode.");
                    Debug.Assert(DoubleUtil.AreClose(extentSize.Width - Math.Floor(extentSize.Width), 0.0), "The extent size must not contain fractional values when in item scrolling mode.");
                }
                else
                {
                    Debug.Assert(DoubleUtil.AreClose(viewportSize.Height - Math.Floor(viewportSize.Height), 0.0), "The viewport size must not contain fractional values when in item scrolling mode.");
                    Debug.Assert(DoubleUtil.AreClose(extentSize.Height - Math.Floor(extentSize.Height), 0.0), "The extent size must not contain fractional values when in item scrolling mode.");
                }
            }
#endif

            // Detect changes to the viewportSize, extentSize, and computedViewportOffset
            bool viewportSizeChanged = !DoubleUtil.AreClose(viewportSize, _scrollData._viewport);
            bool extentSizeChanged = !DoubleUtil.AreClose(extentSize, _scrollData._extent);
            bool computedViewportOffsetChanged = !DoubleUtil.AreClose(computedViewportOffset, _scrollData._computedOffset);

            //
            // Check if we need to repeat the measure operation and adjust the
            // scroll offset and/or viewport size for this new iteration.
            //
            viewportOffset = computedViewportOffset;

            //
            // Check to see if we want to disregard this measure because the ScrollViewer is
            // about to remeasure this panel with changed ScrollBarVisibility. In this case we
            // should allow the ScrollViewer to retry the transition to the original offset.
            //

            bool allowRemeasure = true;

            ScrollViewer sv = ScrollOwner;
            if (sv.InChildMeasurePass1 || sv.InChildMeasurePass2)
            {
                ScrollBarVisibility vsbv = sv.VerticalScrollBarVisibility;
                bool vsbAuto = (vsbv == ScrollBarVisibility.Auto);

                if (vsbAuto)
                {
                    Visibility oldvv = sv.ComputedVerticalScrollBarVisibility;
                    Visibility newvv = DoubleUtil.LessThanOrClose(extentSize.Height, viewportSize.Height) ? Visibility.Collapsed : Visibility.Visible;
                    if (oldvv != newvv)
                    {
                        viewportOffset = _scrollData._offset;
                        allowRemeasure = false;
                    }
                }

                if (allowRemeasure)
                {
                    ScrollBarVisibility hsbv = sv.HorizontalScrollBarVisibility;
                    bool hsbAuto = (hsbv == ScrollBarVisibility.Auto);

                    if (hsbAuto)
                    {
                        Visibility oldhv = sv.ComputedHorizontalScrollBarVisibility;
                        Visibility newhv = DoubleUtil.LessThanOrClose(extentSize.Width, viewportSize.Width) ? Visibility.Collapsed : Visibility.Visible;
                        if (oldhv != newhv)
                        {
                            viewportOffset = _scrollData._offset;
                            allowRemeasure = false;
                        }
                    }
                }
            }

            if (isHorizontal)
            {
                allowRemeasure = !WasOffsetPreviouslyMeasured(previouslyMeasuredOffsets, computedViewportOffset.X);

                if (allowRemeasure)
                {
                    bool wasViewportOffsetCoerced = !DoubleUtil.AreClose(computedViewportOffset.X, _scrollData._offset.X);

                    if (!IsPixelBased)
                    {
                        //
                        // If the viewportSize has the potential to increase and we are scrolling to the very end
                        // then we need to scoot back the offset to fit more in the viewport.
                        //
                        if (!IsEndOfViewport(isHorizontal, viewport, stackPixelSizeInViewport) &&
                            DoubleUtil.GreaterThan(stackLogicalSize.Width, stackLogicalSizeInViewport.Width))
                        {
                            //
                            // When navigating to the last page remember the smallest offset that
                            // was able to display the last item in the collection
                            //
                            lastPageSafeOffset = lastPageSafeOffset.HasValue ? Math.Min(computedViewportOffset.X, (double)lastPageSafeOffset) : computedViewportOffset.X;

                            double approxSizeOfLogicalUnit = stackPixelSizeInViewport.Width / stackLogicalSizeInViewport.Width;
                            double proposedViewportSize = Math.Floor(viewport.Width / approxSizeOfLogicalUnit);
                            if (DoubleUtil.GreaterThan(proposedViewportSize, viewportSize.Width))
                            {
                                viewportOffset.X = Double.PositiveInfinity;
                                viewportSize.Width = proposedViewportSize;
                                remeasure = true;
                                StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.X);
                            }
                        }

                        //
                        // If the viewportSize has decreased and we are scrolling to the very end then we need
                        // to scoot the offset forward to infact be at the very end.
                        //
                        if (!remeasure && wasViewportOffsetCoerced && viewportSizeChanged &&
                            !DoubleUtil.AreClose(_scrollData._viewport.Width, viewportSize.Width))
                        {
                            remeasure = true;
                            viewportOffset.X = _scrollData._offset.X;
                            StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.X);

                            if (DoubleUtil.AreClose(viewportSize.Width, 0))
                            {
                                viewportSize.Width = _scrollData._viewport.Width;
                            }
                        }
                    }

                    if (!remeasure && extentSizeChanged && !DoubleUtil.AreClose(_scrollData._extent.Width, extentSize.Width))
                    {
                        //
                        // If the extentSize has decreased and we are scrolling to the very end then again we
                        // need to scoot back the offset to fit more in the viewport.
                        //
                        if (DoubleUtil.GreaterThan(computedViewportOffset.X, 0.0) &&
                            DoubleUtil.GreaterThan(computedViewportOffset.X, extentSize.Width - viewportSize.Width))
                        {
                            remeasure = true;
                            viewportOffset.X = Double.PositiveInfinity;
                            StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.X);
                        }

                        //
                        // If the extentSize has increased and we are scrolling to the very end then again we need
                        // to scoot the offset forward to infact be at the very end.
                        //
                        if (!remeasure)
                        {
                            if (wasViewportOffsetCoerced)
                            {
                                remeasure = true;
                                viewportOffset.X = _scrollData._offset.X;
                                StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.X);
                            }
                        }

                        //
                        // If the extentSize has changed and we are making and absolute move to an offset (an
                        // active anchor suggests relative movement), we need to readjust the offset to be at the
                        // same percentage location with the respect to the new extent as was initially intended.
                        //
                        if (!remeasure)
                        {
                            bool isAbsoluteMove =
                                (MeasureCaches && !WasLastMeasurePassAnchored) ||
                                (_firstContainerInViewport == null && computedViewportOffsetChanged && !LayoutDoubleUtil.AreClose(computedViewportOffset.X, _scrollData._computedOffset.X));

                            if (isAbsoluteMove &&
                                !DoubleUtil.AreClose(_scrollData._extent.Width, 0) &&
                                !DoubleUtil.AreClose(extentSize.Width, 0))
                            {
                                if (IsPixelBased)
                                {
                                    if (!LayoutDoubleUtil.AreClose(computedViewportOffset.X/extentSize.Width, _scrollData._offset.X/_scrollData._extent.Width))
                                    {
                                        remeasure = true;
                                        viewportOffset.X = (extentSize.Width * _scrollData._offset.X) / _scrollData._extent.Width;
                                        StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.X);
                                    }
                                }
                                else
                                {
                                    if (!LayoutDoubleUtil.AreClose(Math.Floor(computedViewportOffset.X)/extentSize.Width, Math.Floor(_scrollData._offset.X)/_scrollData._extent.Width))
                                    {
                                        remeasure = true;
                                        viewportOffset.X = Math.Floor((extentSize.Width * Math.Floor(_scrollData._offset.X)) / _scrollData._extent.Width);
                                        StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.X);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    //
                    // If we've encountered a cycle in the list of offsets that we measured against
                    // when trying to navigate to the last page, we settle on the best available this far.
                    //
                    if (!IsPixelBased && lastPageSafeOffset.HasValue && !DoubleUtil.AreClose((double)lastPageSafeOffset, computedViewportOffset.X))
                    {
                        viewportOffset.X = (double)lastPageSafeOffset;
                        lastPageSafeOffset = null;
                        remeasure = true;
                    }
                }
            }
            else
            {
                allowRemeasure = !WasOffsetPreviouslyMeasured(previouslyMeasuredOffsets, computedViewportOffset.Y);

                if (allowRemeasure)
                {
                    bool wasViewportOffsetCoerced = !DoubleUtil.AreClose(computedViewportOffset.Y, _scrollData._offset.Y);

                    if (!IsPixelBased)
                    {
                        //
                        // If the viewportSize has the potential to increase and we are scrolling to the very end
                        // then we need to scoot back the offset to fit more in the viewport.
                        //
                        if (!IsEndOfViewport(isHorizontal, viewport, stackPixelSizeInViewport) &&
                            DoubleUtil.GreaterThan(stackLogicalSize.Height, stackLogicalSizeInViewport.Height))
                        {
                            //
                            // When navigating to the last page remember the smallest offset that
                            // was able to display the last item in the collection
                            //
                            lastPageSafeOffset = lastPageSafeOffset.HasValue ? Math.Min(computedViewportOffset.Y, (double)lastPageSafeOffset) : computedViewportOffset.Y;

                            double approxSizeOfLogicalUnit = stackPixelSizeInViewport.Height / stackLogicalSizeInViewport.Height;
                            double proposedViewportSize = Math.Floor(viewport.Height / approxSizeOfLogicalUnit);
                            if (DoubleUtil.GreaterThan(proposedViewportSize, viewportSize.Height))
                            {
                                viewportOffset.Y = Double.PositiveInfinity;
                                viewportSize.Height = proposedViewportSize;
                                remeasure = true;
                                StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.Y);
                            }
                        }

                        //
                        // If the viewportSize has decreased and we are scrolling to the very end then we need
                        // to scoot the offset forward to infact be at the very end.
                        //
                        if (!remeasure && wasViewportOffsetCoerced && viewportSizeChanged &&
                            !DoubleUtil.AreClose(_scrollData._viewport.Height, viewportSize.Height))
                        {
                            remeasure = true;
                            viewportOffset.Y = _scrollData._offset.Y;
                            StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.Y);

                            if (DoubleUtil.AreClose(viewportSize.Height, 0))
                            {
                                viewportSize.Height = _scrollData._viewport.Height;
                            }
                        }
                    }

                    if (!remeasure && extentSizeChanged && !DoubleUtil.AreClose(_scrollData._extent.Height, extentSize.Height))
                    {
                        //
                        // If the extentSize has decreased and we are scrolling to the very end then again we
                        // need to scoot back the offset to fit more in the viewport.
                        //
                        if (DoubleUtil.GreaterThan(computedViewportOffset.Y, 0.0) &&
                            DoubleUtil.GreaterThan(computedViewportOffset.Y, extentSize.Height - viewportSize.Height))
                        {
                            remeasure = true;
                            viewportOffset.Y = Double.PositiveInfinity;
                            StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.Y);
                        }

                        //
                        // If the extentSize has increased and we are scrolling to the very end then again we need
                        // to scoot the offset forward to infact be at the very end.
                        //
                        if (!remeasure)
                        {
                            if (wasViewportOffsetCoerced)
                            {
                                remeasure = true;
                                viewportOffset.Y = _scrollData._offset.Y;
                                StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.Y);
                            }
                        }

                        //
                        // If the extentSize has changed and we are making and absolute move to an offset (an
                        // active anchor suggests relative movement), we need to readjust the offset to be at the
                        // same percentage location with the respect to the new extent as was initially intended.
                        //
                        if (!remeasure)
                        {
                            bool isAbsoluteMove =
                                (MeasureCaches && !WasLastMeasurePassAnchored) ||
                                (_firstContainerInViewport == null && computedViewportOffsetChanged && !LayoutDoubleUtil.AreClose(computedViewportOffset.Y, _scrollData._computedOffset.Y));

                            if (isAbsoluteMove &&
                                !DoubleUtil.AreClose(_scrollData._extent.Height, 0) &&
                                !DoubleUtil.AreClose(extentSize.Height, 0))
                            {
                                if (IsPixelBased)
                                {
                                    if (!LayoutDoubleUtil.AreClose(computedViewportOffset.Y/extentSize.Height, _scrollData._offset.Y/_scrollData._extent.Height))
                                    {
                                        remeasure = true;
                                        viewportOffset.Y = (extentSize.Height * _scrollData._offset.Y) / _scrollData._extent.Height;
                                        StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.Y);
                                    }
                                }
                                else
                                {
                                    if (!LayoutDoubleUtil.AreClose(Math.Floor(computedViewportOffset.Y)/extentSize.Height, Math.Floor(_scrollData._offset.Y)/_scrollData._extent.Height))
                                    {
                                        remeasure = true;
                                        viewportOffset.Y = Math.Floor((extentSize.Height * Math.Floor(_scrollData._offset.Y)) / _scrollData._extent.Height);
                                        StorePreviouslyMeasuredOffset(ref previouslyMeasuredOffsets, computedViewportOffset.Y);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    //
                    // If we've encountered a cycle in the list of offsets that we measured against
                    // when trying to navigate to the last page, we settle on the best available this far.
                    //
                    if (!IsPixelBased && lastPageSafeOffset.HasValue && !DoubleUtil.AreClose((double)lastPageSafeOffset, computedViewportOffset.Y))
                    {
                        viewportOffset.Y = (double)lastPageSafeOffset;
                        lastPageSafeOffset = null;
                        remeasure = true;
                    }
                }
            }

            viewportSizeChanged = !DoubleUtil.AreClose(viewportSize, _scrollData._viewport);

            // Update data and fire scroll change notifications
            if (viewportSizeChanged || extentSizeChanged || computedViewportOffsetChanged)
            {
                Vector oldViewportOffset = _scrollData._computedOffset;
                Size oldViewportSize = _scrollData._viewport;

                _scrollData._viewport = viewportSize;
                _scrollData._extent = extentSize;
                _scrollData._computedOffset = computedViewportOffset;

                // Report changes to the viewportSize
                if (viewportSizeChanged)
                {
                    OnViewportSizeChanged(oldViewportSize, viewportSize);
                }

                // Report changes to the computedViewportOffset
                if (computedViewportOffsetChanged)
                {
                    OnViewportOffsetChanged(oldViewportOffset, computedViewportOffset);
                }

                OnScrollChange();
            }

            _scrollData._offset = viewportOffset;
        }

        private void StorePreviouslyMeasuredOffset(
            ref List<double> previouslyMeasuredOffsets,
            double offset)
        {
            if (previouslyMeasuredOffsets == null)
            {
                previouslyMeasuredOffsets = new List<double>();
            }

            previouslyMeasuredOffsets.Add(offset);
        }

        private bool WasOffsetPreviouslyMeasured(
            List<double> previouslyMeasuredOffsets,
            double offset)
        {
            if (previouslyMeasuredOffsets != null)
            {
                for (int i=0; i<previouslyMeasuredOffsets.Count; i++)
                {
                    if (DoubleUtil.AreClose(previouslyMeasuredOffsets[i], offset))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        /// <summary>
        ///     Allows subclasses to be notified of changes to the viewport size data.
        /// </summary>
        /// <param name="oldViewportSize">The old value of the size.</param>
        /// <param name="newViewportSize">The new value of the size.</param>
        protected virtual void OnViewportSizeChanged(Size oldViewportSize, Size newViewportSize)
        {
        }

        /// <summary>
        ///     Allows subclasses to be notified of changes to the viewport offset data.
        /// </summary>
        /// <param name="oldViewportOffset">The old value of the offset.</param>
        /// <param name="newViewportOffset">The new value of the offset.</param>
        protected virtual void OnViewportOffsetChanged(Vector oldViewportOffset, Vector newViewportOffset)
        {
        }

        /// <summary>
        ///     Fetch the logical/item offset for this child with respect to the top of the
        ///     panel. This is similar to a TransformToAncestor operation. Just works
        ///     in logical units.
        /// </summary>
        protected override double GetItemOffsetCore(UIElement child)
        {
            if (child == null)
            {
                throw new ArgumentNullException("child");
            }

            bool isHorizontal = (Orientation == Orientation.Horizontal);
            ItemsControl itemsControl;
            GroupItem groupItem;
            IContainItemStorage itemStorageProvider;
            IHierarchicalVirtualizationAndScrollInfo virtualizationInfoProvider;
            object parentItem;
            bool mustDisableVirtualization;

            GetOwners(false /*shouldSetVirtualizationState*/, isHorizontal, out itemsControl, out groupItem, out itemStorageProvider, out virtualizationInfoProvider,out parentItem, out mustDisableVirtualization);

            ItemContainerGenerator generator = (ItemContainerGenerator)Generator;
            IList items = generator.ItemsInternal;
            int itemIndex = generator.IndexFromContainer(child, true /*returnLocalIndex*/);

            double distance = 0;

            if (itemIndex >= 0)
            {
                ComputeDistance(items, itemStorageProvider, isHorizontal,
                    GetAreContainersUniformlySized(itemStorageProvider, parentItem),
                    GetUniformOrAverageContainerSize(itemStorageProvider, parentItem),
                    0, itemIndex, out distance);
            }

            return distance;
        }

        private double FindScrollOffset(Visual v)
        {
            ItemsControl scrollingItemsControl = ItemsControl.GetItemsOwner(this);
            IContainItemStorage itemStorageProvider = (IContainItemStorage)scrollingItemsControl;

            DependencyObject child = v;
            DependencyObject element = VisualTreeHelper.GetParent(child);
            IHierarchicalVirtualizationAndScrollInfo virtualizingElement = null;
            Panel itemsHost = null;
            IList items = null;
            object item = null;
            GroupItem groupItem = null;
            ItemsControl parentItemsControl = null;
            Panel parentItemsHost = null;
            int childItemIndex = -1;

            double offset = 0.0;
            bool isHorizontal = (Orientation == Orientation.Horizontal);
            bool returnLocalIndex = true;

            //
            // Compute offset
            //
            while (true)
            {
                virtualizingElement = GetVirtualizingChild(element);
                if (virtualizingElement != null)
                {
                    //
                    // Find item
                    //
                    itemStorageProvider = virtualizingElement as IContainItemStorage;
                    groupItem = virtualizingElement as GroupItem;
                    if (groupItem != null)
                    {
                        item = scrollingItemsControl.ItemContainerGenerator.ItemFromContainer(groupItem);
                    }
                    else // This is the TreeViewItem case
                    {
                        parentItemsHost = (Panel)VisualTreeHelper.GetParent(element);
                        parentItemsControl = ItemsControl.GetItemsOwnerInternal(parentItemsHost) as ItemsControl;
                        item = parentItemsControl.ItemContainerGenerator.ItemFromContainer(element);
                    }

                    //
                    // Find index for childItem in ItemsCollection
                    //
                    itemsHost = virtualizingElement.ItemsHost;
                    child = FindDirectDescendentOfItemsHost(itemsHost, child);
                    if (child != null)
                    {
                        VirtualizingPanel vp = itemsHost as VirtualizingPanel;
                        if (vp != null && vp.CanHierarchicallyScrollAndVirtualize)
                        {
                            double distance = vp.GetItemOffset((UIElement)child);
                            offset += distance;

                            if (IsPixelBased)
                            {
                                Size desiredPixelHeaderSize = virtualizingElement.HeaderDesiredSizes.PixelSize;
                                offset += (isHorizontal ? desiredPixelHeaderSize.Width : desiredPixelHeaderSize.Height);
                            }
                            else
                            {
                                Size desiredLogicalHeaderSize = virtualizingElement.HeaderDesiredSizes.LogicalSize;
                                offset += (isHorizontal ? desiredLogicalHeaderSize.Width : desiredLogicalHeaderSize.Height);
                            }
                        }
                    }

                    child = (DependencyObject)virtualizingElement;
                }
                else if (element == this) // We have walked as far as the scrolling panel
                {
                    //
                    // Find item
                    //
                    item = this;
                    itemStorageProvider = GetItemStorageProvider(this);

                    //
                    // Find index for childItem in ItemsCollection
                    //
                    itemsHost = this;
                    child = FindDirectDescendentOfItemsHost(itemsHost, child);
                    if (child != null)
                    {
                        items = ((ItemContainerGenerator)itemsHost.Generator).ItemsInternal;
                        childItemIndex = ((ItemContainerGenerator)itemsHost.Generator).IndexFromContainer(child, returnLocalIndex);

                        double distance;
                        ComputeDistance(items, itemStorageProvider, isHorizontal,
                            GetAreContainersUniformlySized(itemStorageProvider, item),
                            GetUniformOrAverageContainerSize(itemStorageProvider, item),
                            0, childItemIndex, out distance);

                        offset += distance;
                    }

                    //
                    // Add correction for the position of the scrolling panel within
                    // the viewport element
                    //
                    if (IsPixelBased)
                    {
                        if (isHorizontal)
                        {
                            offset += _pixelOffsetFromViewport.X;
                        }
                        else
                        {
                            offset += _pixelOffsetFromViewport.Y;
                        }
                    }

                    break;
                }

                element = VisualTreeHelper.GetParent(element);
            }

            return offset;
        }

        private void EnsurePixelOffsetFromViewport()
        {
            ItemsControl itemsControl;
            ItemsControl.GetItemsOwnerInternal(this, out itemsControl);
            FrameworkElement viewportElement = (itemsControl != null) ? itemsControl.GetViewportElement() : null;
            Rect elementRect;

            ItemsControl.GetElementViewportPosition(
                            viewportElement,
                            this,
                            (Orientation == Orientation.Horizontal) ? FocusNavigationDirection.Right : FocusNavigationDirection.Down,
                            false, /* fullyVisible */
                            out elementRect);
            _pixelOffsetFromViewport = new Vector(elementRect.Left, elementRect.Top);
        }

        private DependencyObject FindDirectDescendentOfItemsHost(Panel itemsHost, DependencyObject child)
        {
            if (itemsHost == null || !itemsHost.IsVisible)
            {
                return null;
            }

            //
            // Find the direct descendent of the ItemsHost encapsulating the given child
            //
            DependencyObject parent = VisualTreeHelper.GetParent(child);
            while (parent != itemsHost)
            {
                child = parent;
                if (child == null)
                {
                    break;
                }
                parent = VisualTreeHelper.GetParent(child);
            }

            return child;
        }

        // This is very similar to the work that ScrollContentPresenter does for MakeVisible.  Simply adjust by a
        // pixel offset.
        private void MakeVisiblePhysicalHelper(Rect r, ref Vector newOffset, ref Rect newRect, bool isHorizontal, ref bool alignTop, ref bool alignBottom)
        {
            double viewportOffset;
            double viewportSize;
            double targetRectOffset;
            double targetRectSize;
            double minPhysicalOffset;

            if (isHorizontal)
            {
                viewportOffset = _scrollData._computedOffset.X;
                viewportSize = ViewportWidth;
                targetRectOffset = r.X;
                targetRectSize = r.Width;
            }
            else
            {
                viewportOffset = _scrollData._computedOffset.Y;
                viewportSize = ViewportHeight;
                targetRectOffset = r.Y;
                targetRectSize = r.Height;
            }

            targetRectOffset += viewportOffset;
            minPhysicalOffset = ScrollContentPresenter.ComputeScrollOffsetWithMinimalScroll(
                viewportOffset, viewportOffset + viewportSize, targetRectOffset, targetRectOffset + targetRectSize, ref alignTop, ref alignBottom);

            // Compute the visible rectangle of the child relative to the viewport.

            if (alignTop)
            {
                targetRectOffset = viewportOffset;
            }
            else if (alignBottom)
            {
                targetRectOffset = viewportOffset + viewportSize - targetRectSize;
            }

            double left = Math.Max(targetRectOffset, minPhysicalOffset);
            targetRectSize = Math.Max(Math.Min(targetRectSize + targetRectOffset, minPhysicalOffset + viewportSize) - left, 0);
            targetRectOffset = left;
            targetRectOffset -= viewportOffset;

            if (isHorizontal)
            {
                newOffset.X = minPhysicalOffset;
                newRect.X = targetRectOffset;
                newRect.Width = targetRectSize;
            }
            else
            {
                newOffset.Y = minPhysicalOffset;
                newRect.Y = targetRectOffset;
                newRect.Height = targetRectSize;
            }
        }

        private void MakeVisibleLogicalHelper(int childIndex, Rect r, ref Vector newOffset, ref Rect newRect, ref bool alignTop, ref bool alignBottom)
        {
            bool fHorizontal = (Orientation == Orientation.Horizontal);
            int firstChildInView;
            int newFirstChild;
            int viewportSize;
            double childOffsetWithinViewport = r.Y;

            if (fHorizontal)
            {
                firstChildInView = (int)_scrollData._computedOffset.X;
                viewportSize = (int)_scrollData._viewport.Width;
            }
            else
            {
                firstChildInView = (int)_scrollData._computedOffset.Y;
                viewportSize = (int)_scrollData._viewport.Height;
            }

            newFirstChild = firstChildInView;

            // If the target child is before the current viewport, move the viewport to put the child at the top.
            if (childIndex < firstChildInView)
            {
                alignTop = true;
                childOffsetWithinViewport = 0;
                newFirstChild = childIndex;
            }
            // If the target child is after the current viewport, move the viewport to put the child at the bottom.
            else if (childIndex > firstChildInView + Math.Max(viewportSize - 1, 0))
            {
                alignBottom = true;
                newFirstChild = childIndex - viewportSize + 1;
                double pixelSize = fHorizontal ? ActualWidth : ActualHeight;
                childOffsetWithinViewport = pixelSize * (1.0 - (1.0 / viewportSize));
            }

            if (fHorizontal)
            {
                newOffset.X = newFirstChild;
                newRect.X = childOffsetWithinViewport;
                newRect.Width = r.Width;
            }
            else
            {
                newOffset.Y = newFirstChild;
                newRect.Y = childOffsetWithinViewport;
                newRect.Height = r.Height;
            }
        }

        private int GetGeneratedIndex(int childIndex)
        {
            return Generator.IndexFromGeneratorPosition(new GeneratorPosition(childIndex, 0));
        }

        /// <summary>
        ///     Helper method which loops through the children
        ///     and returns the max arrange height/width for
        ///     horizontal/vertical orientation
        /// </summary>
        private double GetMaxChildArrangeLength(IList children, bool isHorizontal)
        {
            double maxChildLength = 0;
            for (int i = 0, childCount = children.Count; i < childCount; i++)
            {
                UIElement container = null;
                Size childSize;

                // we are looping through the actual containers; the visual children of this panel.
                container = (UIElement)children[i];
                childSize = container.DesiredSize;

                if (isHorizontal)
                {
                    maxChildLength = Math.Max(maxChildLength, childSize.Height);
                }
                else
                {
                    maxChildLength = Math.Max(maxChildLength, childSize.Width);
                }
            }
            return maxChildLength;
        }

        //-----------------------------------------------------------
        // Avalon Property Callbacks/Overrides
        //-----------------------------------------------------------
        #region Avalon Property Callbacks/Overrides

        /// <summary>
        /// <see cref="PropertyMetadata.PropertyChangedCallback"/>
        /// </summary>
        private static void OnOrientationChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            // Since Orientation is so essential to logical scrolling/virtualization, we synchronously check if
            // the new value is different and clear all scrolling data if so.
            ResetScrolling(d as VirtualizingStackPanel);
        }

        #endregion

        //------------------------------------------------------
        //
        //  Private Properties
        //
        //------------------------------------------------------

        #region Private Properties

        /// <summary>
        /// True after the first MeasureOverride call. We can't use UIElement.NeverMeasured because it's set to true by the first call to MeasureOverride.
        /// Stored in a bool field on Panel.
        /// </summary>
        private bool HasMeasured
        {
            get
            {
                return VSP_HasMeasured;
            }
            set
            {
                VSP_HasMeasured = value;
            }
        }

        private bool InRecyclingMode
        {
            get
            {
                return VSP_InRecyclingMode;
            }
            set
            {
                VSP_InRecyclingMode = value;
            }
        }


        internal bool IsScrolling
        {
            get { return (_scrollData != null) && (_scrollData._scrollOwner != null); }
        }


        /// <summary>
        /// Specifies if this panel uses item-based or pixel-based computations in Measure and Arrange.
        ///
        /// Differences between the two:
        ///
        /// When pixel-based mode VSP behaves the same to the layout system virtualized as not; its desired size is the sum
        /// of all its children and it arranges children such that the ones in view appear in the right place.
        /// In this mode VSP is also able to make use of the viewport passed down to virtualize chidren.  When
        /// it's the scrolling panel it computes the offset and extent in pixels rather than logical units.
        ///
        /// When in item mode VSP's desired size grows and shrinks depending on which containers are virtualized and it arranges
        /// all children one on top the the other.
        /// In this mode VSP cannot use the viewport to virtualize; it can only virtualize if it is the scrolling panel
        /// (IsScrolling == true).  Thus its looseness with desired size isn't much of an issue since it owns the extent.
        /// </summary>
        /// <remarks>
        /// This should be private, except that one Debug.Assert in TreeView requires it.
        /// </remarks>
        internal bool IsPixelBased
        {
            get
            {
                return VSP_IsPixelBased;
            }
            set
            {
                VSP_IsPixelBased = value;
            }
        }


        internal bool MustDisableVirtualization
        {
            get
            {
                return VSP_MustDisableVirtualization;
            }
            set
            {
                VSP_MustDisableVirtualization = value;
            }
        }

        internal bool MeasureCaches
        {
            get
            {
                return VSP_MeasureCaches || !IsVirtualizing;
            }
            set
            {
                VSP_MeasureCaches = value;
            }
        }

        private bool IsVirtualizing
        {
            get
            {
                return VSP_IsVirtualizing;
            }
            set
            {
                // We must be the ItemsHost to turn on Virtualization.
                bool isVirtualizing = IsItemsHost && value;

                if (isVirtualizing == false)
                {
                    _realizedChildren = null;
                }

                VSP_IsVirtualizing = value;
            }
        }

        private bool HasVirtualizingChildren
        {
            get
            {
                return GetBoolField(BoolField.HasVirtualizingChildren);
            }

            set
            {
                SetBoolField(BoolField.HasVirtualizingChildren, value);
            }
        }

        private bool AlignTopOfBringIntoViewContainer
        {
            get
            {
                return GetBoolField(BoolField.AlignTopOfBringIntoViewContainer);
            }

            set
            {
                SetBoolField(BoolField.AlignTopOfBringIntoViewContainer, value);
            }
        }

        private bool WasLastMeasurePassAnchored
        {
            get
            {
                return GetBoolField(BoolField.WasLastMeasurePassAnchored);
            }

            set
            {
                SetBoolField(BoolField.WasLastMeasurePassAnchored, value);
            }
        }

        private bool ItemsChangedDuringMeasure
        {
            get
            {
                return GetBoolField(BoolField.ItemsChangedDuringMeasure);
            }

            set
            {
                SetBoolField(BoolField.ItemsChangedDuringMeasure, value);
            }
        }

        private bool IsScrollActive
        {
            get
            {
                return GetBoolField(BoolField.IsScrollActive);
            }

            set
            {
                SetBoolField(BoolField.IsScrollActive, value);
            }
        }

        internal bool IgnoreMaxDesiredSize
        {
            get
            {
                return GetBoolField(BoolField.IgnoreMaxDesiredSize);
            }
            set
            {
                SetBoolField(BoolField.IgnoreMaxDesiredSize, value);
            }
        }

        /// <summary>
        ///     Cache property for scrolling VSP to
        ///     avoid ItemStorageProvider calls
        /// </summary>
        private bool? AreContainersUniformlySized
        {
            get;
            set;
        }

        /// <summary>
        ///     Cache property for scrolling VSP to
        ///     avoid ItemStorageProvider calls
        /// </summary>
        private double? UniformOrAverageContainerSize
        {
            get;
            set;
        }

        /// <summary>
        /// Returns the list of childen that have been realized by the Generator.
        /// We must use this method whenever we interact with the Generator's index.
        /// In recycling mode the Children collection also contains recycled containers and thus does
        /// not map to the Generator's list.
        /// </summary>
        private IList RealizedChildren
        {
            get
            {
                if (IsVirtualizing && InRecyclingMode)
                {
                    EnsureRealizedChildren();
                    return _realizedChildren;
                }
                else
                {
                    return InternalChildren;
                }
            }
        }

        bool IStackMeasure.IsScrolling
        {
            get { return IsScrolling; }
        }

        UIElementCollection IStackMeasure.InternalChildren
        {
            get { return InternalChildren; }
        }

        void IStackMeasure.OnScrollChange()
        {
            OnScrollChange();
        }

        #endregion Private Properties

        //------------------------------------------------------
        //
        //  Private Fields
        //
        //------------------------------------------------------

        #region Private Fields

        private bool GetBoolField(BoolField field)
        {
            return (_boolFieldStore & field) != 0;
        }

        private void SetBoolField(BoolField field, bool value)
        {
            if (value)
            {
                 _boolFieldStore |= field;
            }
            else
            {
                 _boolFieldStore &= (~field);
            }
        }

        [System.Flags]
        private enum BoolField : byte
        {
            HasVirtualizingChildren                     = 0x01,
            AlignTopOfBringIntoViewContainer            = 0x02,
            WasLastMeasurePassAnchored                  = 0x04,
            ItemsChangedDuringMeasure                   = 0x08,
            IsScrollActive                              = 0x10,
            IgnoreMaxDesiredSize                        = 0x20,
            // unused                                                              = 0x40,
            // unused                                                              = 0x80,
        }

        private BoolField _boolFieldStore;

        // Scrolling and virtualization data.  Only used when this is the scrolling panel (IsScrolling is true).
        // When VSP is in pixel mode _scrollData is in units of pixels.  Otherwise the units are logical.
        private ScrollData _scrollData;

        // UIElement collection index of the first visible child container.  This is NOT the data item index. If the first visible container
        // is the 3rd child in the visual tree and contains data item 312, _firstItemInExtendedViewportChildIndex will be 2, while _firstItemInExtendedViewportIndex is 312.
        // This is useful because could be several live containers in the collection offscreen (maybe we cleaned up lazily, they couldn't be virtualized, etc).
        // This actually maps directly to realized containers inside the Generator.  It's the index of the first visible realized container.
        // Note that when RecyclingMode is active this is the index into the _realizedChildren collection, not the Children collection.
        private int _firstItemInExtendedViewportChildIndex;
        private int _firstItemInExtendedViewportIndex;              // index of the first data item in the extended viewport
        private double _firstItemInExtendedViewportOffset;          // offset of the first data item in the extended viewport

        private int _actualItemsInExtendedViewportCount;                  // count of the number of data items visible in the extended viewport
        private Rect _viewport;

        // If not MeasureCaches pass the _itemsInExtendedViewport refers to the number of items in the
        // actual viewport and not the extended viewport. Whereas _tailoredItemsInExtendedViewport
        // always refers to the real or approx item count in the extended viewport
        private int _itemsInExtendedViewportCount;
        private Rect _extendedViewport;

        private Vector _pixelOffsetFromViewport;
        private Size _previousStackPixelSizeInViewport;
        private Size _previousStackLogicalSizeInViewport;
        private Size _previousStackPixelSizeInCacheBeforeViewport;
        private FrameworkElement _firstContainerInViewport;
        private double _firstContainerOffsetFromViewport;
        private double _expectedDistanceBetweenViewports;

        // Used by the Recycling mode to maintain the list of actual realized children (a realized child is one that the ItemContainerGenerator has
        // generated).  We need a mapping between children in the UIElementCollection and realized containers in the generator.  In standard virtualization
        // mode these lists are identical; in recycling mode they are not. When a container is recycled the Generator removes it from its realized list, but
        // for perf reasons the panel keeps these containers in its UIElement collection.  This list is the actual realized children -- i.e. the InternalChildren
        // list minus all recycled containers.
        private List<UIElement> _realizedChildren;

        // Cleanup
        private DispatcherOperation _cleanupOperation;
        private DispatcherTimer _cleanupDelay;
        private const int FocusTrail = 5; // The maximum number of items off the edge we will generate to get a focused item (so that keyboard navigation can work)
        private DependencyObject _bringIntoViewContainer;  // pointer to the container we're about to bring into view; it can't be recycled even if it's offscreen.
        private DependencyObject _bringIntoViewLeafContainer; // pointer to the container we are in the process of making visible. We remember it until the container has been successfully brought into view because this may require a few measure iterations.

        private static int[] _indicesStoredInItemValueStorage;

        private static readonly UncommonField<DispatcherOperation> MeasureCachesOperationField = new UncommonField<DispatcherOperation>();
        private static readonly UncommonField<DispatcherOperation> AnchorOperationField = new UncommonField<DispatcherOperation>();
        private static readonly UncommonField<DispatcherOperation> AnchoredInvalidateMeasureOperationField = new UncommonField<DispatcherOperation>();
        private static readonly UncommonField<DispatcherOperation> ClearIsScrollActiveOperationField = new UncommonField<DispatcherOperation>();

        #endregion

        //------------------------------------------------------
        //
        //  Private Structures / Classes
        //
        //------------------------------------------------------

        #region Private Structures Classes

        //-----------------------------------------------------------
        // ScrollData class
        //-----------------------------------------------------------
        #region ScrollData

        // Helper class to hold scrolling data.
        // This class exists to reduce working set when VirtualizingStackPanel is used outside a scrolling situation.
        // Standard "extra pointer always for less data sometimes" cache savings model:
        //      !Scroll [1xReference]
        //      Scroll  [1xReference] + [6xDouble + 1xReference]
        private class ScrollData : IStackMeasureScrollData
        {
            // Clears layout generated data.
            // Does not clear scrollOwner, because unless resetting due to a scrollOwner change, we won't get reattached.
            internal void ClearLayout()
            {
                _offset = new Vector();
                _viewport = _extent = _maxDesiredSize = new Size();
            }

            internal bool IsEmpty
            {
                get
                {
                    return
                        _offset.X == 0.0 &&
                        _offset.Y == 0.0 &&
                        _viewport.Width == 0.0 &&
                        _viewport.Height == 0.0 &&
                        _extent.Width == 0.0 &&
                        _extent.Height == 0.0 &&
                        _maxDesiredSize.Width == 0.0 &&
                        _maxDesiredSize.Height == 0.0;
                }
            }

            // For Stack/Flow, the two dimensions of properties are in different units:
            // 1. The "logically scrolling" dimension uses items as units.
            // 2. The other dimension physically scrolls.  Units are in Avalon pixels (1/96").
            internal bool _allowHorizontal;
            internal bool _allowVertical;

            // Scroll offset of content.  Positive corresponds to a visually upward offset.  Set by methods like LineUp, PageDown, etc.
            internal Vector _offset;

            // Computed offset based on _offset set by the IScrollInfo methods.  Set at the end of a successful Measure pass.
            // This is the offset used by Arrange and exposed externally.  Thus an offset set by PageDown via IScrollInfo isn't
            // reflected publicly (e.g. via the VerticalOffset property) until a Measure pass.
            internal Vector _computedOffset = new Vector(0,0);
            //internal Vector _viewportOffset;    // ViewportOffset is in pixels
            internal Size _viewport;            // ViewportSize is in {pixels x items} (or vice-versa).
            internal Size _extent;              // Extent is the total number of children (logical dimension) or physical size
            internal ScrollViewer _scrollOwner; // ScrollViewer to which we're attached.

            internal Size _maxDesiredSize;      // Hold onto the maximum desired size to avoid re-laying out the parent ScrollViewer.

            public Vector Offset
            {
                get
                {
                    return _offset;
                }
                set
                {
                    _offset = value;
                }
            }

            public Size Viewport
            {
                get
                {
                    return _viewport;
                }
                set
                {
                    _viewport = value;
                }
            }

            public Size Extent
            {
                get
                {
                    return _extent;
                }
                set
                {
                    _extent = value;
                }
            }

            public Vector ComputedOffset
            {
                get
                {
                    return _computedOffset;
                }
                set
                {
                    _computedOffset = value;
                }
            }

            public void SetPhysicalViewport(double value)
            {
            }
        }

        #endregion ScrollData

        #endregion Private Structures Classes
    }
}

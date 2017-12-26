using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using System.Windows;
using System.Windows.Automation.Provider;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;
using MS.Internal;
using MS.Win32;

namespace System.Windows.Automation.Peers
{

    ///<summary>
    /// Earlier this class was returning the default value for all properties when there is no wrapper/when it is virtualized,
    /// now it will throw ElementNotAvailableException (leaving some exceptions, like properties supported by container to find elements)
    /// to notify the client that the full Element does not exist yet. Client may decide to use VirtualizedItemPattern to realize the full item
    ///</summary>
    public abstract class ItemAutomationPeer : AutomationPeer, IVirtualizedItemProvider
    {
        ///
        protected ItemAutomationPeer(object item, ItemsControlAutomationPeer itemsControlAutomationPeer): base()
        {
            _item = item;
            _itemsControlAutomationPeer = itemsControlAutomationPeer;
        }

        ///
        internal override bool AncestorsInvalid
        {
            get { return base.AncestorsInvalid; }
            set
            {
                base.AncestorsInvalid = value;
                if (value)
                    return;
                AutomationPeer wrapperPeer = GetWrapperPeer();
                if (wrapperPeer != null)
                {
                    wrapperPeer.AncestorsInvalid = false;
                }
            }
        }

        ///
        override public object GetPattern(PatternInterface patternInterface)
        {
            if (patternInterface == PatternInterface.VirtualizedItem)
            {
                if(VirtualizedItemPatternIdentifiers.Pattern != null)
                {
                    if(GetWrapperPeer() == null)
                        return this;
                    else
                    {
                        // ItemsControlAutomationPeer can be null in case of TreeViewItems when parent TreeViewItem is also virtualized
                        // If the Item is in Automation Tree we consider it has Realized and need not return VirtualizeItem pattern.
                        if(ItemsControlAutomationPeer != null && !IsItemInAutomationTree())
                        {
                            return this;
                        }

                        if(ItemsControlAutomationPeer == null)
                            return this;
                    }
                }
                return null;
            }
            else if(patternInterface == PatternInterface.SynchronizedInput)
            {
                UIElementAutomationPeer peer = GetWrapperPeer() as UIElementAutomationPeer;
                if(peer != null)
                {
                    return peer.GetPattern(patternInterface);
                }
            }

            return null;
        }

        internal UIElement GetWrapper()
        {
            UIElement wrapper = null;
            ItemsControlAutomationPeer itemsControlAutomationPeer = ItemsControlAutomationPeer;
            if (itemsControlAutomationPeer != null)
            {
                ItemsControl owner = (ItemsControl)(itemsControlAutomationPeer.Owner);
                if (owner != null)
                {
                    if (((MS.Internal.Controls.IGeneratorHost)owner).IsItemItsOwnContainer(_item))
                        wrapper = _item as UIElement;
                    else
                        wrapper = owner.ItemContainerGenerator.ContainerFromItem(_item) as UIElement;
                }
            }
            return wrapper;
        }

        virtual internal AutomationPeer GetWrapperPeer()
        {
            AutomationPeer wrapperPeer = null;
            UIElement wrapper = GetWrapper();
            if(wrapper != null)
            {
                wrapperPeer = UIElementAutomationPeer.CreatePeerForElement(wrapper);
                if(wrapperPeer == null) //fall back to default peer if there is no specific one
                {
                    if(wrapper is FrameworkElement)
                        wrapperPeer = new FrameworkElementAutomationPeer((FrameworkElement)wrapper);
                    else
                        wrapperPeer = new UIElementAutomationPeer(wrapper);
                }
            }

            return wrapperPeer;
        }

        /// <summary>
        internal void ThrowElementNotAvailableException()
        {
            // To avoid the situation on legacy systems which may not have new unmanaged core. this check with old unmanaged core
            // avoids throwing exception and provide older behavior returning default values for items which are virtualized rather than throwing exception.
            if (VirtualizedItemPatternIdentifiers.Pattern != null && !(this is GridViewItemAutomationPeer) && !IsItemInAutomationTree())
                throw new ElementNotAvailableException(SR.Get(SRID.VirtualizedElement));
        }

        private bool IsItemInAutomationTree()
        {
            AutomationPeer parent = this.GetParent();
            if(this.Index != -1 && parent != null && parent.Children != null && this.Index < parent.Children.Count && parent.Children[this.Index] == this)
                return true;
            else return false;
        }


        override internal bool IsDataItemAutomationPeer()
        {
            return true;
        }

        override internal void AddToParentProxyWeakRefCache()
        {
            ItemsControlAutomationPeer itemsControlAutomationPeer = ItemsControlAutomationPeer;
            if(itemsControlAutomationPeer != null)
            {
                itemsControlAutomationPeer.AddProxyToWeakRefStorage(this.ElementProxyWeakReference, this);
            }
        }

        /// <summary>
        override internal Rect GetVisibleBoundingRectCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
            {
                return wrapperPeer.GetVisibleBoundingRectCore();
            }
            return GetBoundingRectangle();
        }

        ///
        override protected string GetItemTypeCore()
        {
            return string.Empty;
        }

        ///
        protected override List<AutomationPeer> GetChildrenCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
            {
                // The children needs to be updated before GetChildren call as ChildrenValid flag would already be true and GetChildren call won't update the children list.
                wrapperPeer.ForceEnsureChildren();
                List<AutomationPeer> children = wrapperPeer.GetChildren();
                return children;
            }

            return null;
        }

        ///
        protected override Rect GetBoundingRectangleCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
            {
                return wrapperPeer.GetBoundingRectangle();
            }
            else
                ThrowElementNotAvailableException();

            return new Rect();
        }

        ///
        protected override bool IsOffscreenCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.IsOffscreen();
            else
                ThrowElementNotAvailableException();

            return true;
        }

        ///
        protected override AutomationOrientation GetOrientationCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.GetOrientation();
            else
                ThrowElementNotAvailableException();

            return AutomationOrientation.None;
        }

        ///
        protected override string GetItemStatusCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.GetItemStatus();
            else
                ThrowElementNotAvailableException();

            return string.Empty;
        }


        ///
        protected override bool IsRequiredForFormCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.IsRequiredForForm();
            else
                ThrowElementNotAvailableException();

            return false;
        }

        ///
        protected override bool IsKeyboardFocusableCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.IsKeyboardFocusable();
            else
                ThrowElementNotAvailableException();

            return false;
        }

        ///
        protected override bool HasKeyboardFocusCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.HasKeyboardFocus();
            else
                ThrowElementNotAvailableException();

            return false;
        }

        ///
        protected override bool IsEnabledCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.IsEnabled();
            else
                ThrowElementNotAvailableException();

            return false;
        }

        ///
        protected override bool IsPasswordCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.IsPassword();
            else
                ThrowElementNotAvailableException();

            return false;
        }

        ///
        protected override string GetAutomationIdCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            string id = null;

            if (wrapperPeer != null)
            {
                id = wrapperPeer.GetAutomationId();
            }
            else if (_item != null)
            {
                using (RecyclableWrapper recyclableWrapper = ItemsControlAutomationPeer.GetRecyclableWrapperPeer(_item))
                {
                    id = recyclableWrapper.Peer.GetAutomationId();
                }
            }

            return id;
        }

        ///
        protected override string GetNameCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            string name = null;

            if (wrapperPeer != null)
            {
                name = wrapperPeer.GetName();
            }
            else if (_item != null)
            {
                using (RecyclableWrapper recyclableWrapper = ItemsControlAutomationPeer.GetRecyclableWrapperPeer(_item))
                {
                    name = recyclableWrapper.Peer.GetName();
                }
            }

            if (string.IsNullOrEmpty(name) && _item != null)
            {
                // For FE we can't use ToString as that provides extraneous information than just the plain text
                FrameworkElement fe = _item as FrameworkElement;
                if(fe != null)
                  name = fe.GetPlainText();
                
                if(string.IsNullOrEmpty(name))
                  name = _item.ToString();
            }

            return name;
        }

        ///
        protected override bool IsContentElementCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.IsContentElement();

            return true;
        }

        ///
        protected override bool IsControlElementCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.IsControlElement();

            return true;
        }

        ///
        protected override AutomationPeer GetLabeledByCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.GetLabeledBy();
            else
                ThrowElementNotAvailableException();

            return null;
        }

        ///
        protected override string GetHelpTextCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.GetHelpText();
            else
                ThrowElementNotAvailableException();

            return string.Empty;
        }

        ///
        protected override string GetAcceleratorKeyCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.GetAcceleratorKey();
            else
                ThrowElementNotAvailableException();

            return string.Empty;
        }

        ///
        protected override string GetAccessKeyCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.GetAccessKey();
            else
                ThrowElementNotAvailableException();

            return string.Empty;
        }

        ///
        protected override Point GetClickablePointCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                return wrapperPeer.GetClickablePoint();
            else
                ThrowElementNotAvailableException();

            return new Point(double.NaN, double.NaN);
        }

        ///
        protected override void SetFocusCore()
        {
            AutomationPeer wrapperPeer = GetWrapperPeer();
            if (wrapperPeer != null)
                wrapperPeer.SetFocus();
            else
                ThrowElementNotAvailableException();
        }

        virtual internal ItemsControlAutomationPeer GetItemsControlAutomationPeer()
        {
            return _itemsControlAutomationPeer;
        }

        ///
        public object Item
        {
            get
            {
                return _item;
            }
        }

        ///
        public ItemsControlAutomationPeer ItemsControlAutomationPeer
        {
            get
            {
                return GetItemsControlAutomationPeer();
            }
            internal set
            {
                _itemsControlAutomationPeer = value;
            }
        }

        ///
        void IVirtualizedItemProvider.Realize()
        {
            RealizeCore();
        }

        virtual internal void RealizeCore()
        {
            ItemsControlAutomationPeer itemsControlAutomationPeer = ItemsControlAutomationPeer;
            if (itemsControlAutomationPeer != null)
            {
                ItemsControl parent = itemsControlAutomationPeer.Owner as ItemsControl;
                if (parent != null)
                {
                    if (parent.ItemContainerGenerator.Status == GeneratorStatus.ContainersGenerated)
                    {
                        // Please note that this action must happen before the OnBringItemIntoView call because
                        // that is a call that synchronously flushes out layout and we want these realized peers
                        // cached before the UpdateSubtree kicks in OnLayoutUpdated.
                        if (VirtualizingPanel.GetIsVirtualizingWhenGrouping(parent))
                        {
                            itemsControlAutomationPeer.RecentlyRealizedPeers.Add(this);
                        }

                        parent.OnBringItemIntoView(Item);
                    }
                    else
                    {
                        // The items aren't generated, try at a later time
                        Dispatcher.BeginInvoke(DispatcherPriority.Loaded,
                            (DispatcherOperationCallback)delegate(object arg)
                            {
                                // Please note that this action must happen before the OnBringItemIntoView call because
                                // that is a call that synchronously flushes out layout and we want these realized peers
                                // cached before the UpdateSubtree kicks in OnLayoutUpdated.
                                if (VirtualizingPanel.GetIsVirtualizingWhenGrouping(parent))
                                {
                                    itemsControlAutomationPeer.RecentlyRealizedPeers.Add(this);
                                }

                                parent.OnBringItemIntoView(arg);

                                return null;
                            }, Item);
                    }
                }
            }
        }

        private object _item;
        private ItemsControlAutomationPeer _itemsControlAutomationPeer;

    }
}




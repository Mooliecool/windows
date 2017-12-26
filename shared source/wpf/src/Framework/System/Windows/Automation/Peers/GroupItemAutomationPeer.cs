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
using System.Windows.Data;
using System.Windows.Interop;
using System.Windows.Media;

using MS.Internal;
using MS.Internal.Data;
using MS.Win32;

namespace System.Windows.Automation.Peers
{
    /// 
    public class GroupItemAutomationPeer : FrameworkElementAutomationPeer
    {
        ///
        public GroupItemAutomationPeer(GroupItem owner): base(owner)
        {
        }
    
        ///
        override protected string GetClassNameCore()
        {
            return "GroupItem";
        }

        ///
        override protected AutomationControlType GetAutomationControlTypeCore()
        {
            return AutomationControlType.Group;
        }

        /// 
        override public object GetPattern(PatternInterface patternInterface)
        {
            if(patternInterface == PatternInterface.ExpandCollapse)
            {
                GroupItem groupItem = (GroupItem)Owner;
                if(groupItem.Expander != null)
                {
                    AutomationPeer expanderPeer = UIElementAutomationPeer.CreatePeerForElement(groupItem.Expander);
                    if(expanderPeer != null && expanderPeer is IExpandCollapseProvider)
                    {
                        expanderPeer.EventsSource = this;
                        return (IExpandCollapseProvider)expanderPeer;
                    }
                }
            }

            return base.GetPattern(patternInterface);
        }

        ///
        protected override List<AutomationPeer> GetChildrenCore()
        {
            GroupItem owner = (GroupItem)Owner;
            ItemsControl itemsControl = ItemsControl.ItemsControlFromItemContainer(Owner);
            if (itemsControl != null)
            {
                ItemsControlAutomationPeer itemsControlAP = itemsControl.CreateAutomationPeer() as ItemsControlAutomationPeer;
                if (itemsControlAP != null)
                {
                    Panel itemsHost = owner.ItemsHost;

                    if (itemsHost == null)
                        return null;

                    IList childItems = itemsHost.Children;                    
                    List<AutomationPeer> children = new List<AutomationPeer>(childItems.Count);

                    foreach (UIElement child in childItems)
                    {
                        if (!((MS.Internal.Controls.IGeneratorHost)itemsControl).IsItemItsOwnContainer(child))
                        {
                            UIElementAutomationPeer peer = child.CreateAutomationPeer() as UIElementAutomationPeer;
                            if (peer != null)
                            {
                                children.Add(peer);

                                //
                                // The AncestorsInvalid check is meant so that we do this call to invalidate the 
                                // GroupItemPeers containing the realized item peers only when we arrive here from an 
                                // UpdateSubtree call because that call does not otherwise descend into parts of the tree 
                                // that have their children invalid as an optimization. 
                                //
                                if (itemsControlAP.RecentlyRealizedPeers.Count > 0 && this.AncestorsInvalid)
                                {
                                    GroupItemAutomationPeer groupItemPeer = peer as GroupItemAutomationPeer;
                                    if (groupItemPeer != null)
                                    {
                                        groupItemPeer.InvalidateGroupItemPeersContainingRecentlyRealizedPeers(itemsControlAP.RecentlyRealizedPeers);
                                    }
                                }
                            }
                        }
                        else
                        {
                            object item =  itemsControl.GetItemOrContainerFromContainer(child);

                            // try to reuse old peer if it exists either in Current AT or in WeakRefStorage of Peers being sent to Client
                            ItemAutomationPeer peer = itemsControlAP.ItemPeers[item];
                            if (peer == null)
                            {
                                peer = itemsControlAP.GetPeerFromWeakRefStorage(item);
                                
                                if (peer != null)
                                {
                                    // As cached peer is getting used it must be invalidated.
                                    peer.AncestorsInvalid = false;
                                    peer.ChildrenValid = false;
                                }                                
                            }
                            
                            if (peer != null)
                            {
                                //
                                // We have now connected the realized peer to its actual parent. Hence the cache can be cleared
                                //
                                int realizedPeerIndex = itemsControlAP.RecentlyRealizedPeers.IndexOf(peer);
                                if (realizedPeerIndex >= 0)
                                {
                                    itemsControlAP.RecentlyRealizedPeers.RemoveAt(realizedPeerIndex);
                                }
                            }
                            else
                            {
                                peer = itemsControlAP.CreateItemAutomationPeerInternal(item);
                            }
                            
                            //perform hookup so the events sourced from wrapper peer are fired as if from the data item
                            if (peer != null)
                            {
                                AutomationPeer wrapperPeer = peer.GetWrapperPeer();
                                if (wrapperPeer != null)
                                {
                                    wrapperPeer.EventsSource = peer;
                                    if (peer.ChildrenValid && peer.Children == null && this.AncestorsInvalid)
                                    {
                                        peer.AncestorsInvalid = true;
                                        wrapperPeer.AncestorsInvalid = true;
                                    }
                                }
                            }
                            
                            //protection from indistinguishable items - for example, 2 strings with same value
                            //this scenario does not work in ItemsControl however is not checked for.
                            if (itemsControlAP.ItemPeers[item] == null)
                            {
                                children.Add(peer);
                                itemsControlAP.ItemPeers[item] = peer;
                            }
                        }
                    }
                    
                    return children;
                }
            }

            return null;
        }

        internal void InvalidateGroupItemPeersContainingRecentlyRealizedPeers(List<ItemAutomationPeer> recentlyRealizedPeers)
        {
            ItemsControl itemsControl = ItemsControl.ItemsControlFromItemContainer(Owner);
            if (itemsControl != null)
            {
                CollectionViewGroupInternal cvg = itemsControl.ItemContainerGenerator.ItemFromContainer(Owner) as CollectionViewGroupInternal;
                if (cvg != null)
                {
                    for (int i=0; i<recentlyRealizedPeers.Count; i++)
                    {
                        ItemAutomationPeer peer = recentlyRealizedPeers[i];
                        object item = peer.Item;
                        
                        if (cvg.LeafIndexOf(item) >= 0)
                        {
                            AncestorsInvalid = true;
                            ChildrenValid = true;
                        }
                    }
                }
            }
        }

    }
}


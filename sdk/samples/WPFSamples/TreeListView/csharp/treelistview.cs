using System;
using System.Windows.Controls;
using System.Windows;


namespace _treeListView
{
    public class TreeListView : TreeView
    {
        protected override DependencyObject 
                           GetContainerForItemOverride()
        {
            return new TreeListViewItem();
        }

        protected override bool 
                           IsItemItsOwnContainerOverride(object item)
        {
            return item is TreeListViewItem;
        }
    }

    public class TreeListViewItem : TreeViewItem
    {
        /// <summary>
        /// Item's hierarchy in the tree
        /// </summary>
        public int Level
        {
            get
            {
                if (_level == -1)
                {
                    TreeListViewItem parent = 
                        ItemsControl.ItemsControlFromItemContainer(this) 
                            as TreeListViewItem;
                    _level = (parent != null) ? parent.Level + 1 : 0;
                }
                return _level;
            }
        }


        protected override DependencyObject 
                           GetContainerForItemOverride()
        {
            return new TreeListViewItem();
        }

        protected override bool IsItemItsOwnContainerOverride(object item)
        {
            return item is TreeListViewItem;
        }

        private int _level = -1;
    }

}

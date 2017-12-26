/*********************************** Module Header ***********************************\
* Module Name:	TreeViewCrudDragDrop.xaml.cs
* Project:		CSSLTreeViewCRUDDragDrop
* Copyright (c) Microsoft Corporation.
* 
* Code Behind of Custom Silverlight User Control which implements a TreeView with added
* functionalities of CRUD and Drag-And-Drop
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*************************************************************************************/

#region Using Directives

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;

#endregion

namespace CSSLTreeViewCRUDDragDrop
{
    /// <summary>
    /// Code Behind of Custom Silverlight User Control which implements 
    /// a TreeView with added functionalities of CRUD and Drag-And-Drop
    /// </summary>
    public partial class TreeViewCrudDragDrop : UserControl
    {
        #region Member Variables

        /// <summary>
        /// Collection bound to TreView
        /// </summary>
        ObservableCollection<Node> objectTree;

        /// <summary>
        /// Data bound to currently selected TreeViewItem
        /// </summary>
        Node selectedNode;

        #endregion

        #region Properties

        /// <summary>
        /// Gets or sets the data bound to TreeView
        /// </summary>
        public List<Node> Items
        {
            get
            {
                return objectTree.ToList<Node>();
            }
            set
            {
                objectTree = new ObservableCollection<Node>(value);
                TreeViewMain.ItemsSource = objectTree;
            }
        }

        #endregion

        #region Constructors

        /// <summary>
        /// Creates a new instance of TreeViewCrudDragDrop
        /// </summary>
        public TreeViewCrudDragDrop()
        {
            InitializeComponent();
            objectTree = new ObservableCollection<Node>();
            TreeViewMain.ItemsSource = objectTree;
        }

        #endregion

        #region Event Handlers

        /// <summary>
        /// Event handler for MouseRightButtonDown of TreeView and TreeViewItem
        /// </summary>
        /// <param name="sender">Object on which event occurred</param>
        /// <param name="e">Event Arguements for the event</param>
        private void TreeViewMain_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            DisableEditForSelectedItem();

            e.Handled = true;
        }

        /// <summary>
        /// Event handler for MouseRightButtonUp of TreeView and TreeViewItem
        /// </summary>
        /// <param name="sender">Object on which event occurred</param>
        /// <param name="e">Event Arguements for the event</param>
        private void TreeViewMain_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            DisableEditForSelectedItem();

            if (sender is TextBlock)
            {
                selectedNode = (Node)((sender as TextBlock).DataContext);
            }
            else
            {
                selectedNode = null;
            }

            ShowContextMenu(e);
        }

        /// <summary>
        /// Event handler for MouseLeftButtonDown of TreeView and TreeViewItem
        /// </summary>
        /// <param name="sender">Object on which event occurred</param>
        /// <param name="e">Event Arguements for the event</param>
        private void TreeViewMain_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            DisableEditForSelectedItem();

            HideContextMenu();
        }

        /// <summary>
        /// Event handler for Add Button Click Event
        /// </summary>
        /// <param name="sender">Add Button</param>
        /// <param name="e">Event arguements for event</param>
        private void AddButton_Click(object sender, RoutedEventArgs e)
        {
            Node newNode = new Node("New Node");

            if (selectedNode != null)
            {
                selectedNode.Add(newNode);
            }
            else
            {
                if (objectTree != null)
                {
                    objectTree.Add(newNode);
                }
                else
                {
                    objectTree = new ObservableCollection<Node>();
                    objectTree.Add(newNode);
                }
            }

            HideContextMenu();
        }

        /// <summary>
        /// Event handler for Edit Button Click Event
        /// </summary>
        /// <param name="sender">Edit Button</param>
        /// <param name="e">Event arguements for event</param>
        private void EditButton_Click(object sender, RoutedEventArgs e)
        {
            EnalbleEditForSelectedItem();

            TreeViewItem selectedTreeViewItem =
                TreeViewExtensions.GetContainerFromItem(TreeViewMain, selectedNode);

            HideContextMenu();
        }

        /// <summary>
        /// Event handler for Delete Button Click Event
        /// </summary>
        /// <param name="sender">Delete Button</param>
        /// <param name="e">Event arguements for event</param>
        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            TreeViewItem selectedTreeViewItem =
                TreeViewExtensions.GetContainerFromItem(TreeViewMain, selectedNode);

            if (selectedTreeViewItem != null)
            {
                TreeViewItem selectedTreeViewItemParent =
                    TreeViewExtensions.GetParentTreeViewItem(selectedTreeViewItem);

                if (selectedTreeViewItemParent != null)
                {
                    Node seleactedParentNode = (Node)selectedTreeViewItemParent.DataContext;
                    seleactedParentNode.Delete(selectedNode);
                }
                else
                {
                    objectTree.Remove(selectedNode);
                }
            }

            HideContextMenu();
        }

        #endregion

        #region Methods

        /// <summary>
        /// Show context menu
        /// </summary>
        /// <param name="e">Mouse Button Event Arguements for getting cursor position</param>
        private void ShowContextMenu(MouseButtonEventArgs e)
        {
            e.Handled = true;
            Point p = e.GetPosition(this);
            ContextMenu.Visibility = Visibility.Visible;
            ContextMenu.IsOpen = true;
            ContextMenu.SetValue(Canvas.LeftProperty, (double)p.X);
            ContextMenu.SetValue(Canvas.TopProperty, (double)p.Y);
        }

        /// <summary>
        /// Hide context menu
        /// </summary>
        private void HideContextMenu()
        {
            ContextMenu.Visibility = Visibility.Collapsed;
            ContextMenu.IsOpen = false;
        }

        /// <summary>
        /// Enable Edit Mode for selected TreeViewItem
        /// </summary>
        private void EnalbleEditForSelectedItem()
        {
            if (selectedNode != null)
            {
                SetTemplateForSelectedItem("TreeViewMainEditTemplate");
            }
        }

        /// <summary>
        /// Disable Edit mode for selected TreeViewItem
        /// </summary>
        private void DisableEditForSelectedItem()
        {
            if (selectedNode != null)
            {
                SetTemplateForSelectedItem("TreeViewMainReadTemplate");
                selectedNode = null;
            }
        }

        /// <summary>
        /// Set Template for Selected TreeViewItem
        /// </summary>
        /// <param name="templateName">Template Name</param>
        private void SetTemplateForSelectedItem(String templateName)
        {
            HierarchicalDataTemplate hdt = (HierarchicalDataTemplate)Resources[templateName];

            TreeViewItem selectedTreeViewItem =
                TreeViewExtensions.GetContainerFromItem(TreeViewMain, selectedNode);

            if (selectedTreeViewItem != null)
                selectedTreeViewItem.HeaderTemplate = hdt;
        }

        #endregion
    }
}

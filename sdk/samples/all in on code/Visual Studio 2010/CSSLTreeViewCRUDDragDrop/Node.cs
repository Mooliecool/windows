/*********************************** Module Header ***********************************\
* Module Name:	Node.cs
* Project:		CSSLTreeViewCRUDDragDrop
* Copyright (c) Microsoft Corporation.
* 
* Data bound to TreeView
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*****************n********************************************************************/

#region Using directives

using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Collections.ObjectModel;

#endregion

namespace CSSLTreeViewCRUDDragDrop
{
    /// <summary>
    /// Data bound to tree view
    /// </summary>
    public class Node
    {
        #region Private Members

        /// <summary>
        /// Text to display in each tree view item
        /// </summary>
        private String text;
        /// <summary>
        /// Children of tree view item
        /// </summary>
        private ObservableCollection<Node> children;

        /// <summary>
        /// Event Handler for PropertyChanged Event
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        #endregion

        #region Public Properties

        /// <summary>
        /// Gets or sets the Children of node
        /// </summary>
        public ObservableCollection<Node> Children
        {
            get { return children; }
            set { children = value; }
        }

        /// <summary>
        /// Gets or sets the Text of node
        /// </summary>
        public String Text
        { get { return text; } set { text = value; } }

        #endregion

        #region Constructor

        /// <summary>
        /// Creates a new instance of Node
        /// </summary>
        /// <param name="text"></param>
        public Node(String text)
        {
            Children = new ObservableCollection<Node>();
            Text = text;
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// Adds a child lnode
        /// </summary>
        /// <param name="node">Node to be added</param>
        public void Add(Node node)
        {
            children.Add(node);
            NotifyPropertyChanged("Children");
        }

        /// <summary>
        /// Deletes a child node
        /// </summary>
        /// <param name="node">Node to be deleted</param>
        public void Delete(Node node)
        {
            children.Remove(node);
            NotifyPropertyChanged("Children");
        }

        #endregion        

        #region Private Methods

        /// <summary>
        /// Event handler for PropertyChange
        /// </summary>
        /// <param name="info"></param>
        private void NotifyPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }

        #endregion
    }
}

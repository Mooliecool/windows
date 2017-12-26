=============================================================================
          APPLICATION : CSSLTreeViewCRUDDragDrop Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

This Application showcases a custom TreeView with added functionalities of 
CRUD and Drag-And-Drop operations

/////////////////////////////////////////////////////////////////////////////
Prerequisite:

You must have the following components installed on your machine
1) Microsoft Visual Studio 2010
2) Microsoft Silverlight 4 SDK
3) Microsoft Silverlight 4 Toolkit April 2010

/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the sample.

Step1. Build the sample project in Visual Studio 2010, 

Step2. Start the Application by selecting "Start Debugging" or "Start without Debugging" in the build menu.

Step3. Right-Click in the box and select "Add" in the context menu. Add more root nodes and also child nodes to existing nodes.

Step4. Right-Click a node, and select "Edit". Edit the content of the node.

Step5. Right-Click a node, and select "Delete". This will delete the node.

Step6. Select any child node, drag and drop it to any other node.

Step7. Close the application

/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Create a new Silverlight Application called CSSLTreeViewCRUDDragDrop

Step2. Add a new class Node. Replace the code with this

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

Step3. Add a new Silverlight User Control called TreeViewCRUDDragDrop

Replace the code in TreeViewCrudDragDrop.xaml by the following code

<UserControl 
    xmlns:sdk="http://schemas.microsoft.com/winfx/2006/xaml/presentation/sdk"  
    x:Class="CSSLTreeViewCRUDDragDrop.TreeViewCrudDragDrop"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    mc:Ignorable="d"
    d:DesignHeight="300" d:DesignWidth="400"
    xmlns:toolkit="clr-namespace:System.Windows.Controls;assembly=System.Windows.Controls.Toolkit"
    xmlns:mswindows="clr-namespace:Microsoft.Windows;assembly=System.Windows.Controls.Toolkit">
    
    <UserControl.Resources>
        <!-- Template for Edit mode of TreeViewItem -->
        <sdk:HierarchicalDataTemplate x:Key="TreeViewMainEditTemplate" 
                                      ItemsSource="{Binding Children}">
            <TextBox Text="{Binding Text,Mode=TwoWay}" >
            </TextBox>
        </sdk:HierarchicalDataTemplate>
        <!-- Template for Read mode for TreeViewItem -->
        <sdk:HierarchicalDataTemplate x:Key="TreeViewMainReadTemplate" 
                                      ItemsSource="{Binding Children}">
            <TextBlock Text="{Binding Text,Mode=TwoWay}"               
                      MouseRightButtonDown="TreeViewMain_MouseRightButtonDown" 
                      MouseRightButtonUp="TreeViewMain_MouseRightButtonUp" 
                      MouseLeftButtonDown="TreeViewMain_MouseLeftButtonDown" >
            </TextBlock>
        </sdk:HierarchicalDataTemplate>        
    </UserControl.Resources>

    <Grid x:Name="LayoutRoot" Background="White">        
        <!-- TreeViewDragDropTarget from Toolkit to add DragAndDrop feature -->
        <toolkit:TreeViewDragDropTarget AllowDrop="True">
            <!-- Custom TreeView  -->
            <sdk:TreeView Name="TreeViewMain"    
                      ItemTemplate="{StaticResource TreeViewMainReadTemplate}"
                      MouseRightButtonDown="TreeViewMain_MouseRightButtonDown" 
                      MouseRightButtonUp="TreeViewMain_MouseRightButtonUp" 
                      MouseLeftButtonDown="TreeViewMain_MouseLeftButtonDown"                       
                      Width="400" Height="400"  >
            </sdk:TreeView>
        </toolkit:TreeViewDragDropTarget>
        
        <!-- Context Menu -->
        <Canvas>
            <Popup Name="ContextMenu" Visibility="Collapsed">
                <Border BorderThickness="1" BorderBrush="Black" Background="White">
                    <StackPanel>
                        <HyperlinkButton Content="Add" Name="AddButton" 
                                         Click="AddButton_Click" />
                        <HyperlinkButton Content="Edit" Name="EditButton" 
                                         Click="EditButton_Click"/>
                        <HyperlinkButton Content="Delete" Name="DeleteButton" 
                                         Click="DeleteButton_Click"/>
                    </StackPanel>
                </Border>
            </Popup>
        </Canvas>
    </Grid>
    
</UserControl>

Replace the code in TreeViewCrudDragDrop.xaml.cs with the following code

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
            Point p = e.GetPosition(null);
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

Step3. Add instance of TreeViewCrudDragDrop in MainPage

Add the following attribute in the UserControl attribute of MainPage

xmlns:Crud="clr-namespace:CSSLTreeViewCRUDDragDrop"

Add the following code inside the grid tag

<Crud:TreeViewCrudDragDrop />      

/////////////////////////////////////////////////////////////////////////////
References:

MichaelSnow: Silverlight Tip of the Day #3 – Mouse Right Clicks
http://www.michaelsnow.com/2010/04/23/silverlight-tip-of-the-day-3-mouse-right-clicks/

MSDN: DataBinding Silverlight
http://msdn.microsoft.com/en-us/library/cc278072(v=vs.95).aspx

Codeplex: Silverlight Toolkit
http://timheuer.com/blog/archive/2009/10/19/silverlight-toolkit-adds-drag-drop-support.aspx

/////////////////////////////////////////////////////////////////////////////
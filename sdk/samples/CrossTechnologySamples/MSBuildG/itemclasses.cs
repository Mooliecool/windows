// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Build.BuildEngine;
using Microsoft.Build.Framework;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Controls.Primitives;
using System.IO;

namespace Microsoft.Samples.MSBuildG
{

    // Summary:
    // Interface for each GroupElement type, used to construct the items lists in the designer.
    
    public interface IGroupElement
    {
        bool IsImported { get; }
        bool HasImportFilename { get; }
        string ImportFilename { get; }
        string Condition { get; set; }

        // Summary:
        // A string title for this element type.
        
        string Identifier { get;}

        // Summary:
        // Indicates whether or not the Condition property of the given element is not empty.
        
        bool HasCondition {get ;}


        // Summary:
        // Indicates whether or not the element can possibly be imported
        
        bool HasImport { get;}


        // Summary:
        // Indicates whether or not the item can be selected within the tree.

        bool CanSelect { get;}

        // Summary:
        // A reference to the UI wrapper for this element
        
        GroupElement Describer { get; set;}


        // Summary:
        // A method whichs adds all the required children for the given element to the Describer
        
        void AddChildren();

        // Summary:
        // A method which returns the necessary UI elements for the header of this element
        
        StackPanel CreateHeader();

        // Summary:
        // A method which returns how many children of this element match the given Query (used by the "Filter" fields in the designer)
        
        
        int GetMatchCount(string query);
    }

    // Summary:
    // Class whichs consumes any object implementing the IGroupElement interface and creates the UI for it.
    
    public class GroupElement : TreeViewItem
    {
        #region Private members
        private Dictionary<UIElement, TextBlock> m_ToolTips = new Dictionary<UIElement, TextBlock>();
        private Dictionary<UIElement, string> m_DefaultDescriptions = new Dictionary<UIElement, string>();
        private Dictionary<UIElement, StackPanel> m_Expanders = new Dictionary<UIElement, StackPanel>();
        private Dictionary<UIElement, ToggleButton> m_ExpanderToggles = new Dictionary<UIElement, ToggleButton>();

        private StackPanel m_LayoutPanel;
        private StackPanel m_ItemPanel;
        private StackPanel m_HeaderElement;
        private Thickness m_TitleMargin;

        private IGroupElement m_Element;

        private bool m_ShowImported = false;
        private string m_Query="";
        #endregion

        #region Shortcut constructors
        public GroupElement(BuildPropertyGroup propertyGroup) : this(new PropertyGroupElement(propertyGroup))
        {
        }

        public GroupElement(PropertyGroupElement propertyGroupElm, BuildProperty property)
            : this(new PropertyElement(propertyGroupElm, property))
        {
        }
        public GroupElement(BuildItemGroup itemGroup) : this(new ItemGroupElement(itemGroup))
        {
        }

        public GroupElement(ItemGroupElement itemGroupElm, BuildItem item)
            : this(new ItemElement(itemGroupElm, item))
        {
        }

        public GroupElement(Target target)
            : this(new TargetElement(target))
        {
        }

        public GroupElement(TargetElement targetElm, BuildTask taskElm)
            : this(new TaskElmElement(targetElm, taskElm))
        {
        }

        public GroupElement(TaskElmElement taskElmElement, string paramName, string paramValue)
            : this(new TaskElmParameter(taskElmElement, paramName, paramValue))
        {
        }

        #endregion

        #region Constructor (used to create the UI elements for the passed in IGroupElement)
        public GroupElement(IGroupElement element)
        {
            this.Selected += new RoutedEventHandler(GroupElement_Selected);

            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            //Save a reference to the element so other functions can use it
            m_Element = element;

            element.Describer = this;

            //Initialize the main layout panel
            m_LayoutPanel=new StackPanel();
            m_LayoutPanel.Orientation=Orientation.Vertical;

            //Initialize the local layout (i.e. the information bar that will be seen before expanded)
            m_ItemPanel=new StackPanel();
            m_ItemPanel.Orientation=Orientation.Horizontal;

            //Add the item layout to the main layout
            m_LayoutPanel.Children.Add(m_ItemPanel);

            m_TitleMargin = new Thickness(2);

            //Add the condition button (is just an indicator if the item is imported)

            if (element.Condition != null) //i.e., this element HAS a condition property
            {
                if (element.IsImported)
                {
                    if (element.HasCondition)
                    {
                        Image conditionIndicator = CreateIndicator(SettingsSystem.GetIcon("Warning.ico"), element.Identifier + " Condition:", element.Condition, Brushes.Wheat);

                        m_ItemPanel.Children.Add(conditionIndicator);
                    }
                    else
                    {
                        m_TitleMargin.Left += 24;
                    }
                }
                else
                {
                    ExpanderData conditionExpander = AddExpander(SettingsSystem.GetIcon("Warning.ico"), element.Identifier + " Condition:", element.Condition, "(Click Here to Add)", Brushes.Wheat, element.HasCondition);
                    conditionExpander.Field.TextChanged += new TextChangedEventHandler(ConditionField_TextChanged);
                    m_ItemPanel.Children.Add(conditionExpander.Button);
                }
            }

            //Add the "Is Imported" button, otherwise, just indent the title block
            if (element.HasImport)
            {
                if (element.IsImported)
                {
                    Image importIndicator = CreateIndicator(SettingsSystem.GetIcon("Warning.ico"), element.Identifier + " Imported", "Imported from unknown file.", Brushes.SkyBlue, element.HasImportFilename);

                    m_ItemPanel.Children.Add(importIndicator);
                }
                else
                {
                    m_TitleMargin.Left += 24;
                }
            }

            //Have the child create the rest
            m_HeaderElement=element.CreateHeader();

            m_HeaderElement.Margin = m_TitleMargin;

            //Add the child-created header to this element
            m_ItemPanel.Children.Add(m_HeaderElement);

            //Set the header to show the stuff in all its glory
            this.Header = m_LayoutPanel;

            //Add the children of the element
            element.AddChildren();
            
            //See if we are allowed to be seen
            DetermineVisibility();
        }

        #endregion

        #region Private Methods

        void GroupElement_Selected(object sender, RoutedEventArgs e)
        {
            if ((!m_Element.CanSelect) || (m_Element.IsImported))
            {
                this.IsSelected = false;
            }
        }

        void ConditionField_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox dataField = sender as TextBox;

            m_Element.Condition = dataField.Text;

            SetExpanderValue(dataField, m_Element.HasCondition, dataField.Text);
        }

        // Summary:
        // Determines whether this item should be visible, based on the properties ShowImported and Query
        
        private void DetermineVisibility()
        {
            bool showItem = true;

            //Check to see if we have a query to match
            if (m_Query.Length > 0)
            {
                int foundCount = 0;

                //Determine the number of children within our element that match
                foundCount = m_Element.GetMatchCount(m_Query);

                if (foundCount == 0)
                {
                    showItem = false;
                }
            }

            //Determine if we can show imported items and whether we are an imported item
            if ((m_Element.IsImported) && (!m_ShowImported) && (m_Element.HasImport))
            {
                showItem = false;
            }

            if (showItem)
            {
                this.Visibility = Visibility.Visible;
            }
            else
            {
                this.Visibility = Visibility.Collapsed;
            }
        }

        #endregion

        #region Public Methods
        public void Refresh()
        {
            m_HeaderElement = m_Element.CreateHeader();
            m_HeaderElement.Margin = m_TitleMargin;
            
            m_ItemPanel.Children.RemoveAt(m_ItemPanel.Children.Count - 1);
            m_ItemPanel.Children.Add(m_HeaderElement);

            this.Items.Clear();
            m_Element.AddChildren();
        }
        #endregion

        #region Properties
        public bool ShowImported
        {
            get
            {
                return (m_ShowImported);
            }
            set
            {
                m_ShowImported = value;
                DetermineVisibility();
            }
        }

        // Summary:
        // The query to which all children of this element must match (If empty, then none).
        
        public string Query
        {
            get
            {
                return (m_Query);
            }
            set
            {
                m_Query = value;
                DetermineVisibility();
            }
        }


        public object ElementBase
        {
            get
            {
                return (m_Element);
            }
        }

        #endregion

        #region Helper functions to allow re-use of elements

        // Summary:
        // A function that creates a Toggle button that when pressed, shows a field that can edit the value that ToggleButton is bound to. 
        
        public ExpanderData AddExpander(Uri imageUri, string title, string description, string defaultDescription, Brush titleColor, bool value)
        {
            //Create the indicator button we will need
            ToggleButton indicatorButton = CreateToggleIndicator(imageUri, title, description, defaultDescription, titleColor, value);
            indicatorButton.Checked += new RoutedEventHandler(indicatorButton_Checked);
            indicatorButton.Unchecked += new RoutedEventHandler(indicatorButton_Unchecked);

            //Create the textbox we will need
            TextBox indicatorField = new TextBox();

            //Create all the elements needed for laying out this item
            StackPanel expanderLayoutPanel = new StackPanel();
            expanderLayoutPanel.Visibility = Visibility.Collapsed;
            expanderLayoutPanel.Orientation = Orientation.Horizontal;
            expanderLayoutPanel.VerticalAlignment = VerticalAlignment.Center;

            TextBlock expanderBlock = new TextBlock();
            expanderBlock.Text = title;
            expanderBlock.Foreground = titleColor;
            expanderBlock.Height = 16;
            expanderBlock.Width = 206;
            expanderBlock.Margin = new Thickness(40, 2, 2, 2);

            //Add the description block to the StackPanel

            expanderLayoutPanel.Children.Add(expanderBlock);

            indicatorField.Text = description;
            indicatorField.Width = 300;
            indicatorField.AcceptsReturn = true;
            indicatorField.AcceptsTab = true;

            indicatorField.Style = (Style)Application.Current.FindResource("WizardEntryBox");
            indicatorField.Margin = new Thickness(2);

            //Add the field to the StackPanel
            expanderLayoutPanel.Children.Add(indicatorField);

            //Add the item to the main layout of this GroupItem
            m_LayoutPanel.Children.Add(expanderLayoutPanel);

            //Save a reference to the Button so we can use it later
            m_ExpanderToggles.Add(indicatorField, indicatorButton);

            //Save a reference to the layout panel so we can show/hide it later
            m_Expanders.Add(indicatorButton,expanderLayoutPanel);

            ExpanderData dataReference = new ExpanderData();
            dataReference.Button = indicatorButton;
            dataReference.Field = indicatorField;

            return (dataReference);
        }

        // Summary:
        // A function that allows external items to set the value within an expander and have the tooltip updated accordingly.
        
        public void SetExpanderValue(TextBox expanderField, bool value, string description)
        {
            //Set the value of the ToolTip
            GetToolTipBlock(m_ExpanderToggles[expanderField]).Text = description;

            //Set the value of the ToggleButton
            SetToggleValue(m_ExpanderToggles[expanderField], value);
        }

        void indicatorButton_Unchecked(object sender, RoutedEventArgs e)
        {
            ToggleButton indicatorButton = sender as ToggleButton;

            StackPanel expanderPanel = m_Expanders[indicatorButton];

            expanderPanel.Visibility = Visibility.Collapsed;
        }


        void indicatorButton_Checked(object sender, RoutedEventArgs e)
        {
            ToggleButton indicatorButton=sender as ToggleButton;

            StackPanel expanderPanel=m_Expanders[indicatorButton];

            expanderPanel.Visibility = Visibility.Visible;
        }

        public Image CreateIndicator(Uri imageUri, string title, string description, Brush titleColor)
        {
            return (CreateIndicator(imageUri, title, description, titleColor, true));
        }

        public Image CreateIndicator(Uri imageUri, string title, Brush titleColor)
        {
            return (CreateIndicator(imageUri, title, "", titleColor, false));
        }

        // Summary:
        // A function that creates an indicator image, complete with a tooltip.
        
        public Image CreateIndicator(Uri imageUri, string title, string description, Brush titleColor, bool hasBox)
        {
            //Create the bitmap source we are using
            BitmapImage indicatorSource = new BitmapImage(imageUri);

            //Create the image we are using
            Image indicatorImage = new Image();
            indicatorImage.Source = indicatorSource;
            indicatorImage.Width = 16;
            indicatorImage.Height = 16;

            //Create a stackpanel as our overall layout element
            StackPanel indicatorPanel = new StackPanel();
            indicatorPanel.Orientation = Orientation.Vertical;

            //Create the TextBlock for the title of the ToolTip
            TextBlock titleBlock = new TextBlock();
            titleBlock.Text = title;
            titleBlock.Foreground = Brushes.Black;
            titleBlock.Background = titleColor;

            //Add the titleBlock to the StackPanel
            indicatorPanel.Children.Add(titleBlock);

            //Create the TextBlock for the description of the ToolTip if it is needed
            if (hasBox)
            {
                TextBlock descriptionBlock = new TextBlock();

                descriptionBlock.Text = description;
                descriptionBlock.Foreground = Brushes.Black;

                indicatorPanel.Children.Add(descriptionBlock);

                //Store a reference to the description block so we can set it at a later point
                m_ToolTips.Add(indicatorImage, descriptionBlock);
            }

            //Create the tooltip we are using
            ToolTip indicatorTip = new ToolTip();
            indicatorTip.Content = indicatorPanel;

            indicatorImage.ToolTip = indicatorTip;

            indicatorImage.Margin = new Thickness(4, 4, 4, 4);

            return (indicatorImage);
        }

        // Summary:
        // A function that creates a toggle button with an indicator, used for showing edit fields.
        
        public ToggleButton CreateToggleIndicator(Uri imageUri, string title, string description, string defaultDescription, Brush titleColor, bool value)
        {
            //Create the indicator image
            Image ItemIndicator = CreateIndicator(imageUri, title, description, titleColor);

            //Create the toggle button
            ToggleButton itemToggle = new ToggleButton();

            itemToggle.Content = ItemIndicator;
          
            //Set the style of the button to the one found in the resources file
            itemToggle.Style = (Style)Application.Current.FindResource("ConditionButtonStyle");

            //Get the tooltip reference and add it to the list so we can reference by either the ToggleButton or the indicator image
            TextBlock tooltipBlock = GetToolTipBlock(ItemIndicator);

            m_ToolTips.Add(itemToggle, tooltipBlock);

            //Save the default description
            m_DefaultDescriptions.Add(itemToggle, defaultDescription);

            //Setup the toggle value
            SetToggleValue(itemToggle, value);

            return (itemToggle);
        }

        public void SetToggleValue(ToggleButton parent, bool value)
        {
            Image itemIndicator = (Image)parent.Content;

            if (value)
            {
                itemIndicator.Opacity = 1;
            }
            else
            {
                itemIndicator.Opacity = 0.4;
                GetToolTipBlock(parent).Text = m_DefaultDescriptions[parent];
            }
        }

        public TextBlock GetToolTipBlock(UIElement parent)
        {
            return (m_ToolTips[parent]);
        }

        #endregion

        #region Helper Data Types

        // Summary:
        // Helper data type to pass back the Button and TextBox of the Expander when once is created
        
        public struct ExpanderData
        {
            public ToggleButton Button;
            public TextBox Field;
        }

        #endregion
    }

    // Summary:
    // The IGroupElement representation of the BuildPropertyGroup
    
    public class PropertyGroupElement : IGroupElement
    {
        private BuildPropertyGroup m_PropertyGroup;
        private GroupElement m_Describer;

        public PropertyGroupElement(BuildPropertyGroup propertyGroup)
        {
            m_PropertyGroup = propertyGroup; 
        }

        public void AddChildren()
        {
            foreach (BuildProperty property in m_PropertyGroup)
            {
                m_Describer.Items.Add(new GroupElement(this, property));
            }
        }

        public StackPanel CreateHeader()
        {
            //Create the panel to return
            StackPanel layoutPanel=new StackPanel();
            layoutPanel.Orientation=Orientation.Vertical;

            TextBlock titleBlock = new TextBlock();
            titleBlock.Text = "Property Count: " + m_PropertyGroup.Count.ToString();
            titleBlock.Foreground = Brushes.White;

            if (m_PropertyGroup.IsImported)
            {
                titleBlock.Foreground = Brushes.SkyBlue;
            }

            layoutPanel.Children.Add(titleBlock);

            return (layoutPanel);
        }

        public BuildPropertyGroup PropertyGroup
        {
            get
            {
                return(m_PropertyGroup);
            }
        }

        /* Returns the number of elements whose name starts with the given query */
        public int GetMatchCount(string query)
        {
            if (query == null)
            {
                throw new ArgumentNullException("query");
            }

            int matchCount = 0;

            foreach (BuildProperty property in m_PropertyGroup)
            {
                if (property.Name.StartsWith(query))
                {
                    matchCount++;
                }
            }

            return (matchCount);
        }

        public bool IsImported
        {
            get
            {
                return(m_PropertyGroup.IsImported);
            }
        }
        public bool CanSelect
        {
            get
            {
                return (true);
            }
        }
        public bool HasImportFilename
        {
            get
            {
                return (true);
            }
        }
        public string ImportFilename
        {
            get
            {
                return ("");
            }
        }
        public string Condition
        {
            get
            {
                return (m_PropertyGroup.Condition);
            }
            set
            {
                m_PropertyGroup.Condition = value;
            }
        }
        public string Identifier
        {
            get
            {
                return ("Property Group");
            }
        }
        public bool HasCondition 
        {
            get 
            {
                return(m_PropertyGroup.Condition.Length>0);
            }
        }
        public bool HasImport
        {
            get
            {
                return (true);
            }
        }

        public GroupElement Describer
        {
            get
            {
                return (m_Describer);
            }
            set
            {
                m_Describer = value;
            }
        }

        public void RemoveProperty(PropertyElement element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            PropertyGroup.RemoveProperty(element.Property);
            m_Describer.Items.Remove(element.Describer);
            m_Describer.Refresh();
        }
    }

    // Summary:
    // The IGroupElement representation of the BuildProperty
    
    public class PropertyElement : IGroupElement
    {
        private PropertyGroupElement m_PropertyGroupElm;
        private BuildProperty m_Property;
        private GroupElement m_Describer;

        public PropertyElement(PropertyGroupElement propertyGroupElement, BuildProperty property)
        {
            m_PropertyGroupElm = propertyGroupElement;
            m_Property = property;
        }

        public void AddChildren()
        {
            //Do nothing, properties do not have children
        }

        public StackPanel CreateHeader()
        {
            //Create the panel to return
            StackPanel layoutPanel = new StackPanel();
            layoutPanel.Orientation = Orientation.Horizontal;

            TextBlock titleBlock = new TextBlock();
            titleBlock.Text = m_Property.Name;
            titleBlock.Foreground = Brushes.White;
            titleBlock.Width = 200;
            titleBlock.Margin = new Thickness(2);

            TextBox valueBox = new TextBox();
            valueBox.Margin = new Thickness(2, 2, 10, 2);
            valueBox.Text = m_Property.Value;
            valueBox.Foreground = Brushes.LightGray;
            valueBox.AcceptsReturn = true;
            valueBox.AcceptsTab = true;

            if (IsImported)
            {
                valueBox.IsReadOnly = true;
                valueBox.Foreground = Brushes.SkyBlue;
            }

            valueBox.Width = 300;
            valueBox.TextChanged += new TextChangedEventHandler(valueBox_TextChanged);
            valueBox.Style = (Style)Application.Current.FindResource("WizardEntryBox");

            Button removeButton = new Button();

            BitmapImage removeSource = new BitmapImage(SettingsSystem.GetIcon("delete.ico"));

            Image removeImage = new Image();
            removeImage.Source = removeSource;
            removeImage.Width = 16;
            removeImage.Height = 16;

            removeButton.Content = removeImage;
            removeButton.Style = (Style)Application.Current.FindResource("WizardImageButton");
            removeButton.Width = 24;
            removeButton.Height = 24;
            removeButton.Margin = new Thickness(4, 2, 2, 2);

            removeButton.Click += new RoutedEventHandler(removeButton_Click);

            //Add the items to the layout Panel
            layoutPanel.Children.Add(titleBlock);
            layoutPanel.Children.Add(valueBox);

            if (!IsImported)
            {
                layoutPanel.Children.Add(removeButton);
            }

            return (layoutPanel);
        }

        void removeButton_Click(object sender, RoutedEventArgs e)
        {
            m_PropertyGroupElm.RemoveProperty(this);
        }

        void valueBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox valueBox = sender as TextBox;
            valueBox.Foreground = Brushes.White;

            m_Property.Value = valueBox.Text;
        }

        public BuildProperty Property
        {
            get
            {
                return (m_Property);
            }
        }

        public int GetMatchCount(string query)
        {
            return (0);
        }

        public GroupElement Describer
        {
            get
            {
                return (m_Describer);
            }
            set
            {
                m_Describer = value;
            }
        }

        public bool IsImported
        {
            get
            {
                return (m_PropertyGroupElm.PropertyGroup.IsImported);
            }
        }
        public bool CanSelect
        {
            get
            {
                return (false);
            }
        }

        public bool HasImport
        {
            get
            {
                return (false);
            }
        }
        public bool HasImportFilename
        {
            get
            {
                return (true);
            }
        }
        public string ImportFilename
        {
            get
            {
                return ("");
            }
        }
        public string Condition
        {
            get
            {
                return (m_Property.Condition);
            }
            set
            {
                m_Property.Condition = value;
            }
        }
        public string Identifier
        {
            get
            {
                return ("Property");
            }
        }
        public bool HasCondition
        {
            get
            {
                return (m_Property.Condition.Length > 0);
            }
        }
    }

    // Summary:
    // The IGroupElement representation of the BuildItemGroup
    
    public class ItemGroupElement : IGroupElement
    {
        private BuildItemGroup m_ItemGroup;
        private GroupElement m_Describer;
        private string m_PredominateType = "";

        public ItemGroupElement(BuildItemGroup itemGroup)
        {
            m_ItemGroup = itemGroup;
        }

        public void AddChildren()
        {
            foreach (BuildItem item in m_ItemGroup)
            {
                m_Describer.Items.Add(new GroupElement(this, item));
            }
        }

        public StackPanel CreateHeader()
        {
            //Create the panel to return
            StackPanel layoutPanel = new StackPanel();
            layoutPanel.Orientation = Orientation.Vertical;

            string headerText = "";

            //Create the header text
            Dictionary<string, int> types = new Dictionary<string, int>();

            string typeName = "";

            foreach (BuildItem item in m_ItemGroup)
            {
                if (!types.ContainsKey(item.Name))
                {
                    types.Add(item.Name,0);
                }

                typeName = item.Name;

                types[item.Name]++;
            }

            if (types.Count == 1)
            {
                m_PredominateType = typeName;

                headerText = typeName + " [" + m_ItemGroup.Count.ToString() + "]";
            }
            else
            {
                headerText = "(Multiple Types) [" + m_ItemGroup.Count.ToString() + "]";
            }

            //Create the title block itself
            TextBlock titleBlock = new TextBlock();
            titleBlock.Text = headerText;
            titleBlock.Foreground = Brushes.White;

            if (m_ItemGroup.IsImported)
            {
                titleBlock.Foreground = Brushes.SkyBlue;
            }


            if (types.Count > 1)
            {
                //Create a types list tooltip so we can see the items in the item group at a glance

                Grid typesListGrid=new Grid();
                typesListGrid.ColumnDefinitions.Add(new ColumnDefinition());
                typesListGrid.ColumnDefinitions.Add(new ColumnDefinition());

                int currentIndex = 0;

                foreach (string type in types.Keys)
                {
                    typesListGrid.RowDefinitions.Add(new RowDefinition());

                    TextBlock typeNameBlock = new TextBlock();
                    typeNameBlock.Foreground = Brushes.Black;
                    typeNameBlock.Text = type;
                    typeNameBlock.Margin = new Thickness(2);

                    TextBlock typeCount = new TextBlock();
                    typeCount.Foreground = Brushes.Blue;
                    typeCount.Text = types[type].ToString();
                    typeCount.Margin = new Thickness(4, 2, 2, 2);

                    Grid.SetColumn(typeNameBlock, 0);
                    Grid.SetColumn(typeCount, 1);

                    Grid.SetRow(typeNameBlock, currentIndex);
                    Grid.SetRow(typeCount, currentIndex);

                    typesListGrid.Children.Add(typeNameBlock);
                    typesListGrid.Children.Add(typeCount);

                    currentIndex++;
                }

                TextBlock typesListTitleBlock = new TextBlock();
                typesListTitleBlock.Text = "Item Types in Group:";
                typesListTitleBlock.Margin = new Thickness(2);
                typesListTitleBlock.Background = Brushes.Gainsboro;
                typesListTitleBlock.Foreground = Brushes.Black;


                StackPanel typesListTipLayout = new StackPanel();
                typesListTipLayout.Orientation = Orientation.Vertical;

                typesListTipLayout.Children.Add(typesListTitleBlock);
                typesListTipLayout.Children.Add(typesListGrid);

                ToolTip typesListTip = new ToolTip();
                typesListTip.Content = typesListTipLayout;

                titleBlock.ToolTip = typesListTip;
            }

            //Add the title block to the layout panel
            layoutPanel.Children.Add(titleBlock);

            return (layoutPanel);
        }

        public string PredominateType
        {
            get
            {
                return (m_PredominateType);
            }
        }

        public BuildItemGroup ItemGroup
        {
            get
            {
                return (m_ItemGroup);
            }
        }

        /* Returns the number of elements whose name starts with the given query */
        public int GetMatchCount(string query)
        {
            int matchCount = 0;

            foreach (BuildItem item in m_ItemGroup)
            {
                if (item.Name.StartsWith(query))
                {
                    matchCount++;
                }
            }

            return (matchCount);
        }

        public bool IsImported
        {
            get
            {
                return (m_ItemGroup.IsImported);
            }
        }
        public bool HasImportFilename
        {
            get
            {
                return (false);
            }
        }
        public string ImportFilename
        {
            get
            {
                return ("");
            }
        }
        public bool CanSelect
        {
            get
            {
                return (true);
            }
        }

        public string Condition
        {
            get
            {
                return (m_ItemGroup.Condition);
            }
            set
            {
                m_ItemGroup.Condition = value;
            }
        }
        public string Identifier
        {
            get
            {
                return ("Item Group");
            }
        }
        public bool HasCondition
        {
            get
            {
                return (m_ItemGroup.Condition.Length > 0);
            }
        }
        public bool HasImport
        {
            get
            {
                return (true);
            }
        }

        public GroupElement Describer
        {
            get
            {
                return (m_Describer);
            }
            set
            {
                m_Describer = value;
            }
        }

        public void RemoveItem(ItemElement element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            ItemGroup.RemoveItem(element.Item);
            m_Describer.Items.Remove(element.Describer);
            m_Describer.Refresh();
        }
    }

    // Summary:
    // The IGroupElement representation of the BuildItem
    
    public class ItemElement : IGroupElement
    {
        private ItemGroupElement m_ItemGroupElement;
        private BuildItem m_Item;
        private GroupElement m_Describer;
        private TextBlock valueBlock;

        public ItemElement(ItemGroupElement itemgroupelm, BuildItem item)
        {
            m_ItemGroupElement = itemgroupelm;
            m_Item = item;
        }

        public void AddChildren()
        {
            //Add the "children" of the element
                TextBox includeBox = AddField("Include", m_Item.Include, Brushes.LightBlue);
                includeBox.TextChanged += new TextChangedEventHandler(includeBox_TextChanged);
                includeBox.LostFocus += new RoutedEventHandler(includeBox_LostFocus);

                TextBox excludeBox = AddField("Exclude", m_Item.Exclude, Brushes.LightPink);
                excludeBox.TextChanged += new TextChangedEventHandler(excludeBox_TextChanged);
        }

        void includeBox_LostFocus(object sender, RoutedEventArgs e)
        {
            TextBox valueBox = sender as TextBox;

            if (valueBox.Text.Length == 0)
            {

                ErrorDialog dialog = new ErrorDialog("You must enter a valid Include for this Item.", "Invalid Include", ErrorDialog.ErrorType.Error);
                dialog.ShowDialog();

                valueBox.Focus();
            }
        }

        void excludeBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox valueBox = sender as TextBox;
            m_Item.Exclude = valueBox.Text;
        }

        void includeBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox valueBox = sender as TextBox;
            m_Item.Include = valueBox.Text;
            valueBlock.Text = m_Item.Include;

            if (valueBox.Text.Length == 0)
            {
                valueBlock.Foreground = Brushes.Tomato;
                valueBlock.Text = "(Error: No Include for this item)";
            }
            else
            {
                valueBlock.Foreground = Brushes.White;
            }
        }

        private TextBox AddField(string caption, string value, Brush highlightColor)
        {
            StackPanel fieldPanel = new StackPanel();
            fieldPanel.Orientation = Orientation.Horizontal;

            TextBlock titleBlock = new TextBlock();
            titleBlock.Text = caption + ":";
            titleBlock.Foreground = highlightColor;
            titleBlock.Width = 200;
            titleBlock.Margin = new Thickness(2);

            TextBox valueBox = new TextBox();
            valueBox.Margin = new Thickness(2, 2, 10, 2);
            valueBox.Text = value;
            valueBox.Foreground = Brushes.White;
            valueBox.AcceptsReturn = true;
            valueBox.AcceptsTab = true;

            if (IsImported)
            {
                valueBox.IsReadOnly = true;
                valueBox.Foreground = Brushes.SkyBlue;
            }

            valueBox.Width = 300;
            valueBox.Style = (Style)Application.Current.FindResource("WizardEntryBox");

            fieldPanel.Children.Add(titleBlock);
            fieldPanel.Children.Add(valueBox);

            m_Describer.Items.Add(fieldPanel);

            return (valueBox);
        }

        public StackPanel CreateHeader()
        {
            //Create the panel to return
            StackPanel layoutPanel = new StackPanel();
            layoutPanel.Orientation = Orientation.Horizontal;

            TextBlock titleBlock = new TextBlock();
            titleBlock.Text = m_Item.Name;
            titleBlock.Foreground = Brushes.White;
            titleBlock.Width = 200;
            titleBlock.Margin = new Thickness(2);

            valueBlock = new TextBlock();
            valueBlock.Margin = new Thickness(2, 2, 10, 2);
            valueBlock.Text = m_Item.Include;
            valueBlock.Foreground = Brushes.LightGray;

            if (IsImported)
            {
                valueBlock.Foreground = Brushes.SkyBlue;
            }

            valueBlock.Width = 300;

            Button removeButton = new Button();

            BitmapImage removeSource = new BitmapImage(SettingsSystem.GetIcon("delete.ico"));

            Image removeImage = new Image();
            removeImage.Source = removeSource;
            removeImage.Width = 16;
            removeImage.Height = 16;

            removeButton.Content = removeImage;
            removeButton.Style = (Style)Application.Current.FindResource("WizardImageButton");
            removeButton.Width = 24;
            removeButton.Height = 24;
            removeButton.Margin = new Thickness(4, 2, 2, 2);

            removeButton.Click += new RoutedEventHandler(removeButton_Click);

            //Add the items to the layout Panel
            layoutPanel.Children.Add(titleBlock);
            layoutPanel.Children.Add(valueBlock);

            if (!IsImported)
            {
                layoutPanel.Children.Add(removeButton);
            }

            return (layoutPanel);
        }

        void removeButton_Click(object sender, RoutedEventArgs e)
        {
            m_ItemGroupElement.RemoveItem(this);
        }

        public BuildItem Item
        {
            get
            {
                return (m_Item);
            }
        }

        public int GetMatchCount(string query)
        {
            return (0);
        }
        public bool CanSelect
        {
            get
            {
                return (false);
            }
        }

        public GroupElement Describer
        {
            get
            {
                return (m_Describer);
            }
            set
            {
                m_Describer = value;
            }
        }

        public bool IsImported
        {
            get
            {
                return (m_ItemGroupElement.ItemGroup.IsImported);
            }
        }
        public bool HasImport
        {
            get
            {
                return (false);
            }
        }
        public bool HasImportFilename
        {
            get
            {
                return (false);
            }
        }
        public string ImportFilename
        {
            get
            {
                return ("");
            }
        }
        public string Condition
        {
            get
            {
                return (m_Item.Condition);
            }
            set
            {
                m_Item.Condition = value;
            }
        }
        public string Identifier
        {
            get
            {
                return ("Item");
            }
        }
        public bool HasCondition
        {
            get
            {
                return (m_Item.Condition.Length > 0);
            }
        }
    }

    // Summary:
    // The IGroupElement representation of the MS Build Target
    
    public class TargetElement : IGroupElement
    {
        private Target m_Target;
        private GroupElement m_Describer;

        public TargetElement(Target target)
        {
            m_Target = target;
        }

        public void AddChildren()
        {
            foreach (BuildTask element in m_Target)
            {
                m_Describer.Items.Add(new GroupElement(this,element));
            }
        }

        public StackPanel CreateHeader()
        {
            //Create the panel to return

            StackPanel layoutPanel = new StackPanel();
            layoutPanel.Orientation = Orientation.Horizontal;

            Thickness titleMargin = new Thickness(0);

            if (m_Target.IsImported)
            {
                if (m_Target.DependsOnTargets.Length > 0)
                {
                    Image dependsIndicator = m_Describer.CreateIndicator(SettingsSystem.GetIcon("Warning.ico"), "Target Depends On:", m_Target.DependsOnTargets, Brushes.PaleGreen);

                    layoutPanel.Children.Add(dependsIndicator);
                }
                else
                {
                    titleMargin.Left += 24;
                }
            }
            else
            {
                GroupElement.ExpanderData dependsExpander = m_Describer.AddExpander(SettingsSystem.GetIcon("Warning.ico"), "Target Depends On:", m_Target.DependsOnTargets, "(Click Here to Add)", Brushes.PaleGreen, (m_Target.DependsOnTargets.Length > 0));

                dependsExpander.Field.TextChanged += new TextChangedEventHandler(DependsField_TextChanged);

                layoutPanel.Children.Add(dependsExpander.Button);
            }


            TextBlock titleBlock = new TextBlock();
            titleBlock.Text = m_Target.Name;
            titleBlock.Foreground = Brushes.White;
            titleBlock.Margin = titleMargin;

            if (m_Target.IsImported)
            {
                titleBlock.Foreground = Brushes.SkyBlue;
            }

            layoutPanel.Children.Add(titleBlock);

            return (layoutPanel);
        }

        void DependsField_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox dataField = sender as TextBox;

            m_Target.DependsOnTargets= dataField.Text;

            m_Describer.SetExpanderValue(dataField, (dataField.Text.Length>0), dataField.Text);
        }

        public Target TargetObject
        {
            get
            {
                return (m_Target);
            }
        }

        /* Returns the number of elements whose name starts with the given query */
        public int GetMatchCount(string query)
        {
            if (m_Target.Name.StartsWith(query))
            {
                return (1);
            }

            return (0);
        }

        public bool IsImported
        {
            get
            {
                return (m_Target.IsImported);
            }
        }
        public bool HasImportFilename
        {
            get
            {
                return (false);
            }
        }
        public string ImportFilename
        {
            get
            {
                return ("");
            }
        }
        public bool CanSelect
        {
            get
            {
                return (true);
            }
        }

        public string Condition
        {
            get
            {
                return (m_Target.Condition);
            }
            set
            {
                m_Target.Condition = value;
            }
        }
        public string Identifier
        {
            get
            {
                return ("Target");
            }
        }
        public bool HasCondition
        {
            get
            {
                return (m_Target.Condition.Length > 0);
            }
        }
        public bool HasImport
        {
            get
            {
                return (true);
            }
        }

        public GroupElement Describer
        {
            get
            {
                return (m_Describer);
            }
            set
            {
                m_Describer = value;
            }
        }
    }

    // Summary:
    // The IGroupElement representation of the MS Build BuildTask
    
    public class TaskElmElement : IGroupElement
    {
        private BuildTask m_TaskElement;
        private TargetElement m_TargetElement;
        private GroupElement m_Describer;

        public TaskElmElement(TargetElement targetElement,BuildTask taskElement)
        {
            m_TargetElement=targetElement;
            m_TaskElement = taskElement;
        }

        public void AddChildren()
        {
            //Add continue on error

            TextBlock continueBlock = new TextBlock();
            continueBlock.Text = "Continue On Error";
            continueBlock.Foreground = Brushes.White;
            continueBlock.Margin = new Thickness(2);

            CheckBox continueOnError = new CheckBox();
            continueOnError.Content = continueBlock;
            continueOnError.IsChecked = m_TaskElement.ContinueOnError;
            continueOnError.Checked += new RoutedEventHandler(continueOnError_Checked);
            continueOnError.Unchecked += new RoutedEventHandler(continueOnError_Unchecked);
            continueOnError.Margin = new Thickness(4);
            continueOnError.IsEnabled = !IsImported;

            m_Describer.Items.Add(continueOnError);

            //Add the parameters
            string[] parameters = m_TaskElement.GetParameterNames();

            foreach (string paramName in parameters)
            {
                string paramValue = m_TaskElement.GetParameterValue(paramName);

                m_Describer.Items.Add(new GroupElement(this, paramName, paramValue));
            }
        }

        void continueOnError_Unchecked(object sender, RoutedEventArgs e)
        {
            m_TaskElement.ContinueOnError = false;
        }

        void continueOnError_Checked(object sender, RoutedEventArgs e)
        {
            m_TaskElement.ContinueOnError = true;
        }

        public StackPanel CreateHeader()
        {
            //Create the panel to return

            StackPanel layoutPanel = new StackPanel();
            layoutPanel.Orientation = Orientation.Vertical;

            TextBlock titleBlock = new TextBlock();
            titleBlock.Text =m_TaskElement.Name;
            titleBlock.Foreground = Brushes.White;

            layoutPanel.Children.Add(titleBlock);

            return (layoutPanel);
        }

        public BuildTask Element
        {
            get
            {
                return (m_TaskElement);
            }
        }

        /* Returns the number of elements whose name starts with the given query */
        public int GetMatchCount(string query)
        {
            return (0);
        }

        public bool IsImported
        {
            get
            {
                return (m_TargetElement.TargetObject.IsImported);
            }
        }
        public bool HasImportFilename
        {
            get
            {
                return (false);
            }
        }
        public string ImportFilename
        {
            get
            {
                return ("");
            }
        }
        public string Condition
        {
            get
            {
                return (m_TaskElement.Condition);
            }
            set
            {
                m_TaskElement.Condition = value;
            }
        }
        public bool CanSelect
        {
            get
            {
                return (false);
            }
        }

        public string Identifier
        {
            get
            {
                return ("Task Element");
            }
        }
        public bool HasCondition
        {
            get
            {
                return (m_TaskElement.Condition.Length > 0);
            }
        }
        public bool HasImport
        {
            get
            {
                return (false);
            }
        }

        public GroupElement Describer
        {
            get
            {
                return (m_Describer);
            }
            set
            {
                m_Describer = value;
            }
        }
    }

    // Summary:
    // The IGroupElement representation of an MS Build TaskElement's parameter
    
    public class TaskElmParameter : IGroupElement
    {
        private TaskElmElement m_TaskElmElement;
        private GroupElement m_Describer;
        private string m_ParamName;
        private string m_ParamValue;

        public TaskElmParameter(TaskElmElement taskElmElement, string paramName, string paramValue)
        {
            m_TaskElmElement = taskElmElement;
            m_ParamName = paramName;
            m_ParamValue = paramValue;
        }

        public void AddChildren()
        {
            //No children
        }

        public StackPanel CreateHeader()
        {
            //Create the panel to return

            StackPanel layoutPanel = new StackPanel();
            layoutPanel.Orientation = Orientation.Horizontal;

            TextBlock titleBlock = new TextBlock();
            titleBlock.Text = m_ParamName;
            titleBlock.Foreground = Brushes.White;
            titleBlock.Width = 200;
            titleBlock.Margin = new Thickness(2);

            TextBox valueBox = new TextBox();
            valueBox.Margin = new Thickness(2, 2, 10, 2);
            valueBox.Text = m_ParamValue;
            valueBox.Foreground = Brushes.LightGray;

            if (IsImported)
            {
                valueBox.IsReadOnly = true;
                valueBox.Foreground = Brushes.SkyBlue;
            }

            valueBox.Width = 300;
            valueBox.TextChanged += new TextChangedEventHandler(valueBox_TextChanged);
            valueBox.Style = (Style)Application.Current.FindResource("WizardEntryBox");

            layoutPanel.Children.Add(titleBlock);
            layoutPanel.Children.Add(valueBox);

            return (layoutPanel);
        }

        void valueBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox valueBox = sender as TextBox;
            valueBox.Foreground = Brushes.White;

            m_ParamValue = valueBox.Text;

            m_TaskElmElement.Element.SetParameterValue(m_ParamName, m_ParamValue);
        }

        /* Returns the number of elements whose name starts with the given query */
        public int GetMatchCount(string query)
        {
            return (0);
        }

        public bool IsImported
        {
            get
            {
                return (m_TaskElmElement.IsImported);
            }
        }
        public bool HasImportFilename
        {
            get
            {
                return (false);
            }
        }
        public string ImportFilename
        {
            get
            {
                return ("");
            }
        }
        public string Condition
        {
            get
            {
                return (null);
            }
            set
            {
                //Do nothing, we don't have a condition
            }
        }
        public bool CanSelect
        {
            get
            {
                return (false);
            }
        }
        public string Identifier
        {
            get
            {
                return ("Task Element");
            }
        }
        public bool HasCondition
        {
            get
            {
                return (false);
            }
        }
        public bool HasImport
        {
            get
            {
                return (false);
            }
        }

        public GroupElement Describer
        {
            get
            {
                return (m_Describer);
            }
            set
            {
                m_Describer = value;
            }
        }
    }

/* Other TreeViewItem's that are not IGroupElement's*/

    // Summary:
    // The UI Wrapper for a Build Event (an Error, Warning or Message returned by MS Build)
    
    public class BuildEventItem : TreeViewItem
    {
        private enum EventType { Error, Warning, Message };
        private EventType m_Type = EventType.Error;

        public BuildEventItem(int eventID, BuildEventArgs e)
        {
            if (e == null)
            {
                throw new ArgumentNullException("e");
            }

            //Get the type of Build Event that occured
            Type itemType = e.GetType();

            string fileInfo = "";
            string lineInfo = "";
            string columnInfo = "";
            string codeInfo = "";
            Brush importanceColor = Brushes.White;

            //Set an internal enumeration as a quick reference and load the information based on the type
            if (itemType == typeof(BuildErrorEventArgs))
            {
                m_Type = EventType.Error;

                BuildErrorEventArgs errorArg = e as BuildErrorEventArgs;

                fileInfo = errorArg.File;
                lineInfo = errorArg.LineNumber.ToString();
                columnInfo = errorArg.ColumnNumber.ToString();
                codeInfo = errorArg.Code;
            }
            if (itemType == typeof(BuildWarningEventArgs))
            {
                m_Type = EventType.Warning;

                BuildWarningEventArgs warningArg = e as BuildWarningEventArgs;

                fileInfo = warningArg.File;
                lineInfo = warningArg.LineNumber.ToString();
                columnInfo = warningArg.ColumnNumber.ToString();
                codeInfo = warningArg.Code;
            }
            if (itemType == typeof(BuildMessageEventArgs))
            {
                m_Type = EventType.Message;

                BuildMessageEventArgs messageArg = e as BuildMessageEventArgs;

                if (messageArg.Importance == MessageImportance.Low)
                {
                    importanceColor = Brushes.Blue;
                }
                if (messageArg.Importance == MessageImportance.Low)
                {
                    importanceColor = Brushes.Red;
                }
            }

            //Create the layout Panel

            StackPanel layoutPanel = new StackPanel();
            layoutPanel.Orientation = Orientation.Horizontal;
            layoutPanel.VerticalAlignment = VerticalAlignment.Center;
            layoutPanel.HorizontalAlignment = HorizontalAlignment.Stretch;

            //Type Icon

            BitmapImage iconImageSource = new BitmapImage(SettingsSystem.GetIcon(m_Type.ToString() + ".ico"));
            Image iconImage = new Image();
            iconImage.Width = 24;
            iconImage.Source = iconImageSource;

            layoutPanel.Children.Add(iconImage);

            //Description

            TextBlock description = new TextBlock();
            description.Text = e.Message;
            description.Foreground = importanceColor;
            description.TextTrimming = TextTrimming.CharacterEllipsis;
            description.HorizontalAlignment = HorizontalAlignment.Stretch;
            description.Height = 16;
            description.Width = 500;

            layoutPanel.Children.Add(description);

            this.Header = layoutPanel;

            Grid informationGrid = new Grid();
            informationGrid.ColumnDefinitions.Add(new ColumnDefinition());
            informationGrid.ColumnDefinitions.Add(new ColumnDefinition());

            //Item Number
            int newID = (eventID + 1);

            AddRow(informationGrid, "Number", newID.ToString());

            //Description
            AddRow(informationGrid, "Full Description", e.Message);

            //File
            AddRow(informationGrid, "File", fileInfo);

            //Line
            AddRow(informationGrid, "Line", lineInfo);

            //Column
            AddRow(informationGrid, "Column", columnInfo);

            //Code Info
            AddRow(informationGrid, "Code", codeInfo);

            this.Items.Add(informationGrid);
        }

        // Summary:
        // Adds a row of information in a Grid (Title and Value)
        
        private void AddRow(Grid informationGrid, string title, string value)
        {
            if (value != null)
            {
                if (value.Length > 0)
                {
                    informationGrid.RowDefinitions.Add(new RowDefinition());

                    TextBlock titleBlock = new TextBlock();
                    titleBlock.Text = title + ":";

                    TextBlock valueBlock = new TextBlock();
                    valueBlock.Text = value;
                    valueBlock.TextWrapping = TextWrapping.Wrap;
                    valueBlock.Width = 500;

                    Grid.SetColumn(titleBlock, 0);
                    Grid.SetColumn(valueBlock, 1);

                    Grid.SetRow(titleBlock, informationGrid.RowDefinitions.Count - 1);
                    Grid.SetRow(valueBlock, informationGrid.RowDefinitions.Count - 1);

                    informationGrid.Children.Add(titleBlock);
                    informationGrid.Children.Add(valueBlock);
                }
            }
        }

        // Summary:
        // A function that determines whether this event should be shown in the list, by type
        
        public void DetermineVisibility(bool showErrors, bool showWarnings, bool showMessages)
        {
            bool isVisible = true;

            if ((m_Type == EventType.Error) && (!showErrors))
            {
                isVisible = false;
            }

            if ((m_Type == EventType.Warning) && (!showWarnings))
            {
                isVisible = false;
            }

            if ((m_Type == EventType.Message) && (!showMessages))
            {
                isVisible = false;
            }

            if (isVisible)
            {
                this.Visibility = Visibility.Visible;
            }
            else
            {
                this.Visibility = Visibility.Collapsed;
            }
        }
    }

    // Summary:
    // The UI Wrapper for an imported project into MS Build
    
    public class ExternalProjectItem : TreeViewItem
    {
        private bool m_IsFramework = false;

        public ExternalProjectItem(string projectLocation)
        {
            //Get the path information
            string projectTitle = Path.GetFileName(projectLocation);
            string filePath = Path.GetDirectoryName(projectLocation);

            TextBlock titleBlock = new TextBlock();

            //Determine if this is a project that was installed as part of the framework
            if ((filePath == SettingsSystem.FindFrameworkPath()))
            {
                titleBlock.Foreground = Brushes.LightBlue;
                projectTitle += " (Framework)";
                m_IsFramework = true;
            }

            titleBlock.Text = projectTitle;

            this.Header = titleBlock;

            //Add the "children" of this file

            TextBlock locationBlock = new TextBlock();
            locationBlock.Text="Project Location: " + projectLocation;
            locationBlock.Foreground = Brushes.White;
            this.Items.Add(locationBlock);
        }

        // Summary:
        // Is this Imported Project part of the installed Framework
        
        public bool IsFramework
        {
            get
            {
                return (m_IsFramework);
            }
        }
    }
}

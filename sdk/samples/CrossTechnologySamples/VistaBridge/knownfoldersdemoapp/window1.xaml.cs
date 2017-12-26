using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using System.Collections.Generic;
using Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders;
using System.Windows.Media;
using System.IO;
using Microsoft.SDK.Samples.VistaBridge.Library;

namespace Microsoft.SDK.Samples.VistaBridge.KnownFoldersDemo
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        private void listBox1_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 1)
            {
                throw new NotSupportedException(
                    "Cannot select multiple folders.");
            }
            string who = (string)listBox1.SelectedItem;
            KnownFolder kf;
            try
            {
                kf = KnownFolders.GetKnownFolder(who, false);
            }
            catch (FileNotFoundException)
            {
                // It is possible for files to not be defined
                // on a system.
                nameText.Text = who;
                nameText.ToolTip = "";
                pathText.Text = "The folder is not valid for this system.";
                pathText.Foreground = Brushes.Red;
                ClearDetails();

                return;
            }
            
            nameText.Text = kf.Name;
            nameText.ToolTip = kf.FolderId;
            canonicalNameText.Text = kf.CanonicalName;
            attributeText.Text = kf.FileAttributes.ToString();

            if (kf.PathExists == null)
            {
                pathText.Text = "(virtual)";
                pathText.Foreground = Brushes.Gray;
            }
            else if (kf.PathExists == false)
            {
                pathText.Text = "(file does not exist on this computer.)";
                pathText.Foreground = System.Windows.Media.Brushes.Gray;
            }
            else
            {
                pathText.Text = kf.Path;
                pathText.Foreground = System.Windows.Media.Brushes.Black;
            }
            relativePathText.Text = kf.RelativePath;
            if (kf.ParentId != Guid.Empty)
            {
                parentText.Text = kf.Parent;
                parentText.ToolTip = kf.ParentId.ToString();
            }
            else
            {
                parentText.Text = String.Empty;
                parentText.ToolTip = String.Empty;
            }
            securityText.Text = kf.Security;
            folderTypeText.Text = kf.FolderType;
            folderTypeText.ToolTip = kf.FolderTypeId.ToString();
            categoryText.Text = kf.Category.ToString();
            definitionText.Text = kf.DefinitionOptions.ToString();
            redirectionText.Text = kf.Redirection.ToString();
            localizedNameText.Text = kf.LocalizedName;
            localizedNameText.ToolTip = kf.LocalizedNameResourceId;
            descriptionText.Text = kf.Description;
            parsingNameText.Text = kf.ParsingName;

            if (!String.IsNullOrEmpty(kf.Tooltip))
            {
                iconText.Text = kf.Tooltip;
                iconText.Foreground = Brushes.Black;
                iconText.ToolTip = kf.TooltipResourceId;
            }
            else
            {
                iconText.Text = "(no tool tip)";
                iconText.Foreground = Brushes.Gray;
                iconText.ToolTip = String.Empty;
            }
            if (kf.Icon != null)
            {
                iconImage.Source = kf.Icon;
                iconImage.ToolTip = kf.IconResourceId;
            }
            else
            {
                iconImage.Source = null;
                iconImage.ToolTip = String.Empty;
            }
              
        }

        private void ClearDetails()
        {
            attributeText.Text = String.Empty;
            categoryText.Text = String.Empty;
            canonicalNameText.Text = String.Empty;
            parentText.Text = String.Empty;
            parentText.ToolTip = String.Empty;
            relativePathText.Text = String.Empty;
            securityText.Text = String.Empty;
            folderTypeText.Text = String.Empty;
            categoryText.Text = String.Empty;
            definitionText.Text = String.Empty;
            redirectionText.Text = String.Empty;
            localizedNameText.Text = String.Empty;
            descriptionText.Text = String.Empty;
            parsingNameText.Text = String.Empty;

            iconText.Text = "(no tool tip)";
            iconText.Foreground = Brushes.Gray;
            iconText.ToolTip = String.Empty;
            iconImage.Source = null;
            iconImage.ToolTip = String.Empty;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            SortedList<string,Guid> f = KnownFolders.GetAllFolders();
            listBox1.ItemsSource = f.Keys;
         }
    }
}

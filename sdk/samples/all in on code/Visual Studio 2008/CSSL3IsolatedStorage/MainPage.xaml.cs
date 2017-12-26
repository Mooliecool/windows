/****************************** Module Header ******************************\
* Module Name:                 MainPage.xaml.cs
* Project:                     CSSL3IsolatedStorage
* Copyright (c) Microsoft Corporation.
* 
* Isolated Storage samples code behind file.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

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
using System.IO.IsolatedStorage;
using System.IO;
using System.Collections.ObjectModel;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using System.ComponentModel;
using System.Threading;

namespace CSSL3IsolatedStorage
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(MainPage_Loaded);
        }

        // Initialize application
        IsoDirectory _isoroot;
        IsolatedStorageFile _isofile;
        void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Load isolated storage file
            _isofile = IsolatedStorageFile.GetUserStoreForApplication();
            
            // Display isolated storage file info
            RefreshAvailableSize();

            // Load isolated storage treeview ViewModel 
            _isoroot = LoadIsoRoot();

            // Display treeview
            tvIsolatedStorage.ItemsSource = new ObservableCollection<IsoFile> { _isoroot };

            // Retrieve last login date from IsolatedStorageSettings
            if (IsolatedStorageSettings.ApplicationSettings.Contains("lastLogin"))
            {
                var date = IsolatedStorageSettings.ApplicationSettings["lastLogin"].ToString();
                tbAppInfo.Text = "Last time this application ran was: "+date;
            }
            else
                tbAppInfo.Text = "Last time this application ran is not known.";
            // Save login time in IsolatedStorageSettings
            IsolatedStorageSettings.ApplicationSettings["lastLogin"] = DateTime.Now;

            // Update operation panel
            UpdateOperationPanel();
        }

        #region IsolatedStorage Treeview ViewModel method

        // Helper method: get parent directory 
        IsoDirectory GetParentDir(IsoDirectory root, IsoFile child)
        {
            if (string.IsNullOrEmpty(child.FilePath))
                return null;
            else
            {
                string[] dirs = child.FilePath.Split('/');
                IsoDirectory cur = root;
                for (int i = 1; i < dirs.Length - 1; i++)
                {
                    IsoDirectory next = cur.Children.FirstOrDefault(dir => dir.FileName == dirs[i]) as IsoDirectory;
                    if (next != null)
                        cur = next;
                    else
                        return null;
                }
                return cur;
            }
        }

        // Load isolated storage view model
        IsoDirectory LoadIsoRoot()
        {
            var root = new IsoDirectory("Root", null);
            AddFileToDirectory(root, _isofile);
            return root;
        }

        // Add dir/file recursively 
        void AddFileToDirectory(IsoDirectory dir, IsolatedStorageFile isf)
        {
            string[] childrendir, childrenfile;
            if (string.IsNullOrEmpty(dir.FilePath))
            {
                childrendir = isf.GetDirectoryNames();
                childrenfile = isf.GetFileNames();
            }
            else
            {
                childrendir = isf.GetDirectoryNames(dir.FilePath + "/");
                childrenfile = isf.GetFileNames(dir.FilePath + "/");
            }

            // Add directory entity
            foreach (var dirname in childrendir)
            {
                var childdir = new IsoDirectory(dirname, dir.FilePath + "/" + dirname);
                AddFileToDirectory(childdir, isf);
                dir.Children.Add(childdir);
            }

            // Add file entity
            foreach (var filename in childrenfile)
            {
                dir.Children.Add(new IsoFile(filename, dir.FilePath + "/" + filename));
            }
        }
        #endregion

        #region Copy stream on worker thread

        // Create workerthread to copy stream
        void CopyStream(Stream from, Stream to)
        {

            BackgroundWorker bworker = new BackgroundWorker();
            bworker.WorkerReportsProgress = true;
            bworker.DoWork += new DoWorkEventHandler(bworker_DoWork);
            bworker.ProgressChanged += new ProgressChangedEventHandler(bworker_ProgressChanged);
            bworker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(bworker_RunWorkerCompleted);

            bworker.RunWorkerAsync(new Stream[] { from, to });

            // Show "Copying" panel
            gdDisable.Visibility = Visibility.Visible;
            spCopyPanel.Visibility = Visibility.Visible;
            gdPlayerPanel.Visibility = Visibility.Collapsed;
        }

        // Handle work complete event
        void bworker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            // Close "copying" panel
            gdDisable.Visibility = Visibility.Collapsed;

            if (e.Error != null)
            {
                MessageBox.Show(e.Error.Message);
            }
        }

        // Display progress
        void bworker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            pbCopyProgress.Value = e.ProgressPercentage;
        }

        // Copy stream in worker thread
        void bworker_DoWork(object sender, DoWorkEventArgs e)
        {
            var param = e.Argument as Stream[];

            byte[] buffer = new byte[65536];
            int pos = 0;
            int progress = -1;
            while (true)
            {
                int icount = param[0].Read(buffer, pos, buffer.Length);
                param[1].Write(buffer, 0, icount);
                if (icount < buffer.Length)
                    break;

                int curprogress = (int)(param[1].Length * 100 / param[0].Length);
                if (curprogress > progress)
                {
                    progress = curprogress;
                    ((BackgroundWorker)sender).ReportProgress(progress);
                }
            }

            // Close thread on UI thread
            Dispatcher.BeginInvoke(delegate
            {
                param[0].Close();
                param[1].Close();
                RefreshAvailableSize(); 
            });
        }

        #endregion

        #region treeview & operation button eventhandler

        void RefreshAvailableSize()
        {
            tbQuotaAvailable.Text = string.Format("Current storage Quota is: {0}KB, {1}KB available. The Quota can be increased by user initialized action, such as mouse click eventhandler",
                _isofile.Quota / 1024, _isofile.AvailableFreeSpace / 1024);
        }

        // Update operation panel
        void UpdateOperationPanel()
        {
            var item = tvIsolatedStorage.SelectedItem;
            if (item == null)
            {
                spOperationPanel.Visibility = Visibility.Collapsed;
            }
            else
            {
                spOperationPanel.Visibility = Visibility.Visible;
                if (item is IsoDirectory)
                {
                    bnAddDir.Visibility = Visibility.Visible;
                    bnAddFile.Visibility = Visibility.Visible;
                    bnDelete.Visibility = Visibility.Visible;
                    bnSave.Visibility = Visibility.Collapsed;
                    bnPlay.Visibility = Visibility.Collapsed;
                }
                else if (item is IsoFile)
                {
                    bnAddDir.Visibility = Visibility.Collapsed;
                    bnAddFile.Visibility = Visibility.Collapsed;
                    bnDelete.Visibility = Visibility.Visible;
                    bnSave.Visibility = Visibility.Visible;
                    bnPlay.Visibility = Visibility.Visible;
                }
            }
        }

        // Increase quota
        private void bnIncreaseQuota_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!_isofile.IncreaseQuotaTo(_isofile.Quota + 1024 * 1024 * 10))
                {
                    MessageBox.Show("Increase quota failed.");
                }
            }
            catch (ArgumentException ex)
            {
                MessageBox.Show(ex.Message);
            }

            RefreshAvailableSize();
        }

        // Add directory.
        private void bnAddDir_Click(object sender, RoutedEventArgs e)
        {
            var item = tvIsolatedStorage.SelectedItem as IsoDirectory;
            if (item != null)
            {
                string newfoldename = "Folder_" + Guid.NewGuid();
                string newfolderpath = item.FilePath + "/" + newfoldename;

                try
                {
                    // Check if directory already exist
                    if (_isofile.DirectoryExists(newfolderpath))
                    {
                        MessageBox.Show("Folder exist:" + newfolderpath);
                    }
                    else
                    {
                        _isofile.CreateDirectory(newfolderpath);
                        item.Children.Add(new
                        IsoDirectory(newfoldename, newfolderpath));
                    }
                }
                catch (PathTooLongException)
                {
                    MessageBox.Show("Due to path length restriction, the directory depth is confined to less than 4.");
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Add Folder Failed.\nDetails: " + ex.Message);
                }
            }
        }

        // Add File
        private void bnAddFile_Click(object sender, RoutedEventArgs e)
        {
            bool overrideflag = false;
            var selecteddir = tvIsolatedStorage.SelectedItem as IsoDirectory;
            if(selecteddir==null)
                return;

            OpenFileDialog ofd = new OpenFileDialog();
            var result = ofd.ShowDialog();
            if (result.HasValue && result.Value)
            {
                string filename = ofd.File.Name;
                string filepath = selecteddir.FilePath + "/" + filename;
                IsoFile file = new IsoFile(filename, filepath);

                try
                {
                    // Check if file name is same with directory name
                    if (_isofile.GetDirectoryNames(filepath).Length > 0)
                    {
                        MessageBox.Show(string.Format("File name {0} not allowed", filename));
                        return;
                    }
                    // Check if file name already exist
                    else if (_isofile.GetFileNames(filepath).Length > 0)
                    {
                        // Show message box, ask user if override file
                        var mbresult = MessageBox.Show(string.Format("Override the current file: {0} ?", filename), "override warning", MessageBoxButton.OKCancel);
                        if (mbresult != MessageBoxResult.OK)
                            return;
                        else
                            overrideflag = true;
                    }
                }
                catch (PathTooLongException)
                {
                    MessageBox.Show("Add file failed.\nThe file path is too long.");
                    return;
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                    return;
                }

                // Check if has enough space
                if (_isofile.AvailableFreeSpace < ofd.File.Length)
                {
                    MessageBox.Show("Not enough isolated storage space.");
                    return;
                }

                Stream isostream = null;
                Stream filestream = null; 
                try
                {
                    // Create isolatedstorage stream
                    isostream = _isofile.CreateFile(filepath);
                    // Open file stream
                    filestream = ofd.File.OpenRead();
                    // Copy
                    // Note: exception in copy progress won't be catched here.
                    CopyStream(filestream, isostream);

                    // Check override
                    if (!overrideflag)
                        selecteddir.Children.Add(file);
                }
                catch(Exception ex) {
                    if (isostream != null) isostream.Close();
                    if (filestream != null) filestream.Close();
                    MessageBox.Show(ex.Message);
                }
            }
        }
   
        // Delete 
        private void bnDelete_Click(object sender, RoutedEventArgs e)
        {
            var item = tvIsolatedStorage.SelectedItem as IsoFile;
            if (item != null)
            {
                // Root
                if (string.IsNullOrEmpty(item.FilePath))
                {
                    MessageBox.Show("Cannot delete root");
                    return;
                }

                try
                {
                    if (item is IsoDirectory)
                    {
                        _isofile.DeleteDirectory(item.FilePath);
                    }
                    else
                    {
                        _isofile.DeleteFile(item.FilePath);
                    }
                    var isodirparent = GetParentDir(_isoroot, item);
                    if (isodirparent != null)
                        isodirparent.Children.Remove(item);
                }
                catch (PathTooLongException ex)
                {
                    MessageBox.Show(ex.Message);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }

            RefreshAvailableSize();
        }

        // Save to local
        private void bnSave_Click(object sender, RoutedEventArgs e)
        {
            var item = tvIsolatedStorage.SelectedItem as IsoFile;
            if (item != null)
            {
                try
                {
                    SaveFileDialog sfd1 = new SaveFileDialog();

                    // Set file filter
                    var substr = item.FileName.Split('.');
                    if (substr.Length >= 2)
                    {
                        string defaultstr = "*." + substr[substr.Length - 1];
                        sfd1.Filter = string.Format("({0})|{1}|(*.*)|*.*", defaultstr, defaultstr);
                    }
                     else 
                        sfd1.Filter = "(*.*)|*.*";

                    // Show save fiel dialog
                    var result = sfd1.ShowDialog();

                    if (result.HasValue && result.Value)
                    {
                        // Open isolatedstorage stream
                        var filestream = sfd1.OpenFile();
                        // Create file stream
                        var isostream = _isofile.OpenFile(item.FilePath, FileMode.Open, FileAccess.Read);
                        // Copy
                        CopyStream(isostream, filestream);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        // Close player panel
        private void bnClosePlayer_Click(object sender, RoutedEventArgs e)
        {
            gdDisable.Visibility = Visibility.Collapsed;
            mePlayer.Stop();
            mePlayer.Source = null;

            if (currentplaystream != null)
                currentplaystream.Close();

       }

        // Play
        Stream currentplaystream = null;
        private void bnPlay_Click(object sender, RoutedEventArgs e)
        {
            var item = tvIsolatedStorage.SelectedItem as IsoFile;
            if (item != null)
            {
                try
                {
                    var stream = _isofile.OpenFile(item.FilePath, FileMode.Open, FileAccess.Read);

                    // Show "player" panel
                    gdDisable.Visibility = Visibility.Visible;
                    spCopyPanel.Visibility = Visibility.Collapsed;
                    gdPlayerPanel.Visibility = Visibility.Visible;

                    mePlayer.SetSource(stream);
                    currentplaystream = stream;
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        // When treeview selected item changed, refresh operation panel.
        private void TreeView_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            UpdateOperationPanel();
        }
        #endregion

    }

    // Isolatedstoarge file obj
    public class IsoFile
    {
        public string FilePath{set;get;}
        public string FileName { set; get; }

        public Stream ContentStream
        {
            private set;
            get;
        }
        public IsoFile(string strFilename, string strPath)
        {
            FileName = strFilename;
            FilePath = strPath;
        }
    }

    // Isolatedstorage directory obj
    public class IsoDirectory:IsoFile
    {
        public ObservableCollection<IsoFile> Children
        { 
            private set;
            get;
        }
        public IsoDirectory(string strFilename, string strPath)
            : base(strFilename, strPath)
        {
            Children = new ObservableCollection<IsoFile>();
        }
    }

    // Image converter
    // According to the entity type, return different image.
    public class ImageConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value is IsoDirectory)
                return new BitmapImage(new Uri("/Images/dir.png", UriKind.Relative));
            else
                return new BitmapImage(new Uri("/Images/File.png", UriKind.Relative));
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }


}

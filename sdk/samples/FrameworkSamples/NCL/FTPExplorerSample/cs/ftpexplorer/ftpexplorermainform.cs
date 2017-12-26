//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Security;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Microsoft.Samples.FTPExplorer
{
    public enum MenuContext
    {
        NoSelection,
        HostName,
        Directory,
        File,
        MultipleFiles
    }
    
    partial class FtpExplorerMainForm : Form
    {
        private MenuContext currentMenuContext = MenuContext.NoSelection;
        public FtpExplorerMainForm()
        {
            InitializeComponent();
            ServicePointManager.ServerCertificateValidationCallback
                = new RemoteCertificateValidationCallback(FtpClient.remoteCertificateValidation);
            // Create two ImageList objects.			
            ImageList imageList1 = new ImageList();
            imageList1.Images.Add(new Bitmap(this.GetType().Assembly.GetManifestResourceStream("Microsoft.Samples.FTPExplorer.FILE.ICO")));
            this.FileListView.SmallImageList = imageList1;
            ImageList imageList2 = new ImageList();
            imageList2.Images.Add(new Bitmap(this.GetType().Assembly.GetManifestResourceStream("Microsoft.Samples.FTPExplorer.FOLDER.ICO")));
            imageList2.Images.Add(new Bitmap(this.GetType().Assembly.GetManifestResourceStream("Microsoft.Samples.FTPExplorer.OPENFOLDER.ICO")));
            imageList2.Images.Add(new Bitmap(this.GetType().Assembly.GetManifestResourceStream("Microsoft.Samples.FTPExplorer.HOST.ICO")));
            this.FolderListView.ImageList = imageList2;    
            updateNetworkStatus(NetworkInterface.GetIsNetworkAvailable());            
            NetworkChange.NetworkAvailabilityChanged +=new NetworkAvailabilityChangedEventHandler(myNetworkAvailabilityChangeHandler);
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.Run(new FtpExplorerMainForm());
        }

        private void FolderListView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            TreeNode tn = e.Node;
            Uri uri = new Uri("ftp://" + tn.FullPath + "/");
            FtpClient myClient = FtpClient.GetFtpClient(uri.Host);
            FileStruct[] FileList = myClient.GetFileList(uri.AbsolutePath);
            //this.lblStatus.Text = mySession.Status ;
            lock (this.FileListView)
            {
                this.FileListView.Clear();
                if (FileList != null)
                {
                    this.FileListView.View = System.Windows.Forms.View.List;
                    foreach (FileStruct fs in FileList)
                    {
                        System.Windows.Forms.ListViewItem newItem = new System.Windows.Forms.ListViewItem();
                        newItem.SubItems.Add(fs.Name);
                        newItem.SubItems.Add(fs.CreateTime);
                        this.FileListView.Items.Add(fs.Name, 0);
                    }
                }
                _setImageOnFolderNode(tn);
            }
            this.FileListView.AutoResizeColumns(ColumnHeaderAutoResizeStyle.ColumnContent);
        }

        private void updateMenuSelection()
        {
            switch (currentMenuContext)
            {
                case MenuContext.NoSelection:
                    fileActionMenu.Visible = false;
                    directoryActionMenu.Visible = false;
                    hostActionMenu.Visible = false;
                    break;
                case MenuContext.HostName:
                    fileActionMenu.Visible = false;
                    directoryActionMenu.Visible = false;
                    hostActionMenu.Visible = true;
                    break;
                case MenuContext.Directory:
                    fileActionMenu.Visible = false;
                    directoryActionMenu.Visible = true;
                    hostActionMenu.Visible = false;
                    break;
                case MenuContext.File:
                    fileActionMenu.Visible = true;
                    directoryActionMenu.Visible = false;
                    hostActionMenu.Visible = false;
                    break;
                case MenuContext.MultipleFiles:
                    fileActionMenu.Visible = true;
                    directoryActionMenu.Visible = false;
                    break;
            }
        }

        private void FolderListView_BeforeExpand(object sender, System.Windows.Forms.TreeViewCancelEventArgs e)
        {
            TreeNode thisnode = e.Node;
            if (containsDummyNode(thisnode))
            {
                refreshFolderNode(thisnode);
            }
        }

        private void menuAddSite_Click(object sender, System.EventArgs e)
        {
            AddFtpSite newSite = new AddFtpSite();
            if (newSite.ShowDialog(this) == DialogResult.OK)
            {
                try
                {
                    NetworkCredential credential = new NetworkCredential(newSite.UserID, newSite.Password);
                    FtpClient.AddSite(newSite.FtpHost, credential, newSite.EnableSSL);
                    TreeNode tn = new TreeNode(newSite.FtpHost);
                    addDummyNode(tn);
                    this.FolderListView.Nodes.Add(tn);
                    TreeNode CurrentNode = this.FolderListView.Nodes[this.FolderListView.Nodes.Count - 1]; //last node				
                    CurrentNode.ImageIndex = 2;
                }
                catch (Exception ex)
                {
                    System.Windows.Forms.MessageBox.Show(ex.Message, "Add Site Failed");
                }
            }
        }

        private void refreshFolderNode(TreeNode baseNode)
        {
            Uri uri = new Uri("ftp://" + baseNode.FullPath + "/");
            FtpClient myClient = FtpClient.GetFtpClient(uri.Host);
            FileStruct[] DirList = myClient.GetDirectoryList(uri.AbsolutePath);
            lock (this.FolderListView)
            {
                baseNode.Nodes.Clear();
                if (DirList != null)
                {
                    foreach (FileStruct fs in DirList)
                    {
                        TreeNode tn = new TreeNode(fs.Name);
                        addDummyNode(tn);
                        baseNode.Nodes.Add(tn);
                    }
                }
            }
            _setImageOnFolderNode(baseNode);
            return;
        }

        private void addDummyNode(TreeNode baseNode)
        {
            TreeNode dummy = new TreeNode();
            baseNode.Nodes.Add(dummy);
        }

        private bool containsDummyNode(TreeNode baseNode)
        {
            if (baseNode.Nodes.Count == 1 &&
                baseNode.Nodes[0].Name == "")
            {
                return true;
            }
            return false;
        }

        private void menuDownLoad_Click(object sender, System.EventArgs e)
        {
            if (this.FileListView.SelectedItems.Count == 0)
                System.Windows.Forms.MessageBox.Show("Select Files to Download ...", "FtpExplorer.Net", MessageBoxButtons.OK, MessageBoxIcon.Information);
            else
            {
                DoDownLoadDialog DownLoadDlg = new DoDownLoadDialog();
                DownLoadDlg.DownloadFileList = new String[this.FileListView.SelectedItems.Count];
                for (int i = 0; i < this.FileListView.SelectedItems.Count; i++)
                {
                    DownLoadDlg.DownloadFileList[i] = this.FolderListView.SelectedNode.FullPath + "/" + this.FileListView.SelectedItems[i].Text;
                }
                DownLoadDlg.ShowDialog(this);
            }
        }
        private void removeToolStripMenuItem_Click(object sender, System.EventArgs e)
        {
            Uri uri = new Uri("ftp://" + this.FolderListView.SelectedNode.FullPath);
            FtpClient mySession = FtpClient.GetFtpClient(uri.Host);
            mySession.RemoveDirectory(uri.AbsolutePath);
            //this.lblStatus.Text = mySession.Status;
            this.FolderListView.SelectedNode = this.FolderListView.SelectedNode.Parent;
            this.FolderListView.SelectedNode.Collapse();
            this.FolderListView.SelectedNode.Expand();

        }
        private void deleteFilesToolStripMenuItem_Click(object sender, System.EventArgs e)
        {
            if (this.FileListView.SelectedItems.Count == 0)
                System.Windows.Forms.MessageBox.Show("Select Files to Delete ...", "FtpExplorer.Net", MessageBoxButtons.OK, MessageBoxIcon.Information);
            else
            {
                Uri uri = new Uri("ftp://" + this.FolderListView.SelectedNode.FullPath);
                FtpClient mySession = FtpClient.GetFtpClient(uri.Host);
                foreach (System.Windows.Forms.ListViewItem item in FileListView.SelectedItems)
                {
                    Uri fileuri = new Uri("ftp://" + this.FolderListView.SelectedNode.FullPath + "/" + item.Text);
                    mySession.DeleteFile(fileuri);
                    Application.DoEvents();
                }
                this.FolderListView_AfterSelect(this, new TreeViewEventArgs(FolderListView.SelectedNode));
            }
        }

        private void FolderListView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Node.FullPath.Contains("/"))
            {
                currentMenuContext = MenuContext.Directory;
            }
            else
            {
                currentMenuContext = MenuContext.HostName;
            }
            updateMenuSelection();

        }

        private void FileListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            currentMenuContext = MenuContext.File;
            updateMenuSelection();

        }

        private void downloadDirectoryToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DoDirectoryDownLoad directoryDownload = new DoDirectoryDownLoad(new Uri("ftp://" + this.FolderListView.SelectedNode.FullPath));
            directoryDownload.Show(this);
        }

        private void uploadFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Uri currentUri = new Uri("ftp://" + this.FolderListView.SelectedNode.FullPath);
            FtpClient mySession = FtpClient.GetFtpClient(currentUri.Host);
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Title = "Select File(s) to upload at " + currentUri;
            if (DialogResult.OK == openFile.ShowDialog())
            {
                foreach (string localFile in openFile.FileNames)
                {
                    mySession.UploadFile(localFile, currentUri);
                    //Refresh the file list in UI
                    FolderListView_AfterSelect(this, new TreeViewEventArgs(FolderListView.SelectedNode));
                }
            }
        }

        private void deleteDirectoryToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult result = MessageBox.Show("Delete the directory " + this.FolderListView.SelectedNode.Text,
                                "Delete Directory",
                                MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                Uri currentUri = new Uri("ftp://" + this.FolderListView.SelectedNode.FullPath);
                FtpClient mySession = FtpClient.GetFtpClient(currentUri.Host);
                mySession.RemoveDirectory(currentUri.AbsolutePath);
                this.FolderListView.SelectedNode = this.FolderListView.SelectedNode.Parent;
                refreshFolderNode(this.FolderListView.SelectedNode);
            }
        }

        private void saveWorkSpaceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FtpClient.SaveCurrentWorkSpace();
        }

        private void openWorkSpaceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FtpClient[] clients = FtpClient.LoadNewWorkSpace();
            if (clients.Length != 0)
            {
                FolderListView.Nodes.Clear();
            }
            foreach (FtpClient client in clients)
            {
                TreeNode tn = new TreeNode(client.HostId);
                addDummyNode(tn);
                FolderListView.Nodes.Add(tn);
            }
        }

        private void welComeMessageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Uri currentUri = new Uri("ftp://" + this.FolderListView.SelectedNode.FullPath);
            FtpClient mySession = FtpClient.GetFtpClient(currentUri.Host);
            if (mySession.WelcomeMessage != "")
            {
                MessageBox.Show(mySession.WelcomeMessage,
                    "Welcome Message from " + mySession.HostId,
                    MessageBoxButtons.OK);
            }
        }

        private void _setImageOnFolderNode(TreeNode tn)
        {
            int nodeImageIndex = 0;
            if (tn.Parent == null)
            {
                nodeImageIndex = 2;
            }
            else if (tn.IsExpanded || tn.Nodes.Count == 0)
            {
                nodeImageIndex = 1;
            }
            else
            {
                nodeImageIndex = 0;
            }
            tn.ImageIndex = nodeImageIndex;
            tn.SelectedImageIndex = nodeImageIndex;
        }

        private void FolderListView_AfterExpand(object sender, TreeViewEventArgs e)
        {
            _setImageOnFolderNode(e.Node);
        }

        private void FolderListView_AfterCollapse(object sender, TreeViewEventArgs e)
        {
            _setImageOnFolderNode(e.Node);
        }

        private void makeDirectoryToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Uri currentUri = new Uri("ftp://" + this.FolderListView.SelectedNode.FullPath);
            NameInputForm inputForm = new NameInputForm();
            inputForm.Title = "Make Directory at " + currentUri;
            inputForm.InputParameterLabel = "New Directory Name";
            if (DialogResult.OK == inputForm.ShowDialog() && inputForm.InputValue != "")
            {
                FtpClient mySession = FtpClient.GetFtpClient(currentUri.Host);
                mySession.MakeDirectory(currentUri.AbsolutePath + "/" + inputForm.InputValue);
                refreshFolderNode(this.FolderListView.SelectedNode);
            }
        }

        private void exitToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }
        public void myNetworkAvailabilityChangeHandler(object sender,
            NetworkAvailabilityEventArgs args)
        {
            this.Invoke(new System.Threading.WaitCallback(updateNetworkStatus), args.IsAvailable);
        }

        private void updateNetworkStatus(object state)
        {
            if ((bool)state)
            {
                lblNetworkStatus.ForeColor = Color.Green;
                lblNetworkStatus.Text = "NetworkStatus: Online";
            }
            else
            {
                lblNetworkStatus.ForeColor = Color.Red;
                lblNetworkStatus.Text = "NetworkStatus: Offline";
            }
        }
     }
}
//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
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
//-----------------------------------------------------------------------

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Collections.Generic;
using System.Threading;

namespace Microsoft.Sample.Compression
{
	/// <summary>
	/// Main form for the application
	/// </summary>
	public partial class CompressionForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		private CustomFileDialog cFileDialog; // Dialog item for menu clicks
		private ZipFile archive;			//Archive for zipping operations
		public static string statusMessage;	//For the status message to fixed by 
											//other components

		public CompressionForm()
		{
			InitializeComponent();
			//Initialize the list view
			fileListView.View = View.Details;
			fileListView.GridLines = true;
			fileListView.Columns.Add("Name", 150, HorizontalAlignment.Left);
			fileListView.Columns.Add("Modified", 130, HorizontalAlignment.Left);
			fileListView.Columns.Add("Size", 75, HorizontalAlignment.Right);
			fileListView.Columns.Add("Ratio %", 50, HorizontalAlignment.Right);
			fileListView.Columns.Add("Compressed size", 95, HorizontalAlignment.Right);
			fileListView.Columns.Add("Path", 250, HorizontalAlignment.Left);

			EnableControls(false);
			EnableExtractRemoveButtons(false);

			cFileDialog = new CustomFileDialog();
		}

		private void EnableControls(bool value)
		{
			addMenuStripButton.Enabled = value;
			addToolStripButton.Enabled = value;
			extractAllMenuStripButton.Enabled = value;
			
		}

		private void newGzipToolStripMenuItem_Click(object sender, 
			System.EventArgs e)
		{
			NewArchive(ZipConstants.GZIP);
		}
		private void newDeflateToolStripMenuItem_Click(object sender, 
			System.EventArgs e)
		{
			NewArchive(ZipConstants.DEFLATE);
		}

		private void NewArchive(byte method)
		{
			string name = cFileDialog.NewMode();
			if (name == null)
			{
				fileListView.Focus();
				return;
			}
			
			System.IO.FileMode mode = System.IO.FileMode.CreateNew;
			if (System.IO.File.Exists(name))
			{
				if (ShowOverwriteDialog() == DialogResult.Yes)
				{
					mode = System.IO.FileMode.Truncate;
				}
				else
				{
					fileListView.Focus();
					return;
				}
			}

			if (archive != null)
				archive.Close();

			archive = new ZipFile(name, method, mode);
			Clear();
			
			if (statusMessage.Length != 0)
			{
				DisplayStatusMessage();
				ChangeTitle(name, archive.CompressionMethod());
				EnableControls(true);
			}
			else
			{
				Clear();
			}
		}

		private DialogResult ShowOverwriteDialog()
		{
			MessageBoxOptions opt;
			if (System.Threading.Thread.CurrentThread.
				CurrentUICulture.TextInfo.IsRightToLeft == true)
				opt = MessageBoxOptions.RightAlign | 
					MessageBoxOptions.RtlReading;
			else
				opt = MessageBoxOptions.DefaultDesktopOnly;

			return MessageBox.Show(ZipConstants.FileReplace,
						ZipConstants.Replace, 
						MessageBoxButtons.YesNo,
						MessageBoxIcon.None, 
						MessageBoxDefaultButton.Button1, 
						opt);
		}

		private void openToolStripMenuItem_Click(object sender, 
			System.EventArgs e)
		{
			string name = cFileDialog.OpenMode();
			if (name == null)
			{
				fileListView.Focus();
				return;
			}
			if (archive != null)
				archive.Close();
			archive = new ZipFile(name);
			Clear();
			
			if (statusMessage.Length != 0)
			{
				DisplayStatusMessage();
				ChangeTitle(name, archive.CompressionMethod());
				EnableControls(true);
			}
			else
			{
				Clear();
			}
		}

		private void closeToolStripMenuItem_Click(object sender, 
			System.EventArgs e)
		{
			this.Close();
		}
		private void removeToolStripMenuItem_Click(object sender,
			System.EventArgs e)
		{
			foreach (int index in fileListView.SelectedIndices)
			{
				archive.Remove(index);
			}
			if (fileListView.SelectedIndices.Count == 0)
				statusMessage = String.Empty;
			RefreshListView();
			DisplayStatusMessage();
		}

		private void addToolStripMenuItem_Click(object sender, 
			System.EventArgs e)
		{
			string[] names = cFileDialog.AddMode();
			if (names == null)
			{
				fileListView.Focus();
				return;
			}
			foreach (string name in names)
			{
				int index = archive.CheckFileExists(name) ;
				if (index != -1)
				{
					statusMessage = ZipConstants.FileExistsError;
					RefreshListView();
					DisplayStatusMessage();
					fileListView.Focus();
					fileListView.Items[index].Selected = true;
				}
				else
				{
					archive.Add(name);
					RefreshListView();
					DisplayStatusMessage();
				}
			}
		}

		private void extractToolStripMenuItem_Click(object sender, EventArgs e)
		{
			//string dir = GetFolderName();
			CreateDir();
			foreach (int index in fileListView.SelectedIndices)
			{
				archive.Extract(index, "C:\\temp\\zipTemp");
			}

			if (fileListView.SelectedIndices.Count == 0)
				statusMessage = String.Empty;
			DisplayStatusMessage();
		}

		private void extractAllToolStripMenuItem_Click(object sender, EventArgs e)
		{
			CreateDir();
			archive.ExtractAll("C:\\temp\\zipTemp");
			DisplayStatusMessage();
		}

		private void RefreshListView()
		{
			List<ZipEntry> entries = archive.Entries;
			if (entries == null)
				return;
			fileListView.Items.Clear();

			EnableExtractRemoveButtons(false);

			foreach (ZipEntry entry in entries)
			{
				int index = entry.Name.LastIndexOf(ZipConstants.BackSlash);
				entry.Name.Substring(0, index);
				ListViewItem lvi = new ListViewItem(
					entry.Name.Substring(index + 1));
				lvi.SubItems.Add(entry.DateTime.ToString(
					Thread.CurrentThread.CurrentUICulture));

				lvi.SubItems.Add(entry.Size.ToString(
					Thread.CurrentThread.CurrentUICulture));
				
				int ratio = (int)(((double)(entry.Size - entry.CompressedSize) 
							/ entry.Size) * 100);
				ratio = (ratio < 0 ? 0 : ratio);
				
				lvi.SubItems.Add(ratio.ToString(
					Thread.CurrentThread.CurrentUICulture));
				
				lvi.SubItems.Add(entry.CompressedSize.ToString(
					Thread.CurrentThread.CurrentUICulture));
				
				lvi.SubItems.Add(entry.Name.Substring(0, index));
				fileListView.Items.Add(lvi);
			}
		}
		public void DisplayStatusMessage()
		{
		    mainStatusStrip.Text = statusMessage;
		}

		private void ChangeTitle(string name, byte method)
		{
			int index = name.LastIndexOf(ZipConstants.BackSlash);
			string methodName = null;
			if (method == ZipConstants.GZIP)
				methodName = ZipConstants.GzipName;
			if (method == ZipConstants.DEFLATE)
				methodName = ZipConstants.DeflateName;

			this.Text = ZipConstants.Title + "-" + name.Substring(index + 1)
					+ " (" + methodName + ")";

		}
		private void Clear()
		{
			this.Text = ZipConstants.Title;
			EnableExtractRemoveButtons(false);
			EnableControls(false);
			fileListView.Items.Clear();
			RefreshListView();
		}
		
		private string GetFolderName()
		{
			string dir = String.Empty;
			FolderBrowserDialog fbd = new FolderBrowserDialog();
			if (fbd.ShowDialog() == DialogResult.OK)
				dir = fbd.SelectedPath;
			return dir;
		}
		
		private void CreateDir()
		{
			if (System.IO.Directory.Exists("C:\\temp\\zipTemp"))
				return;
			System.IO.Directory.CreateDirectory("C:\\temp\\zipTemp");
		}

		private void fileListView_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (fileListView.SelectedIndices.Count > 0)
			{
				EnableExtractRemoveButtons(true);
			}
			else
			{
				EnableExtractRemoveButtons(false);
			}
		}
		
		private void contextMenuStripButton_Click(object sender, EventArgs e)
		{
			extractToolStripMenuItem_Click(sender, e);
		}
		private void fileListView_MouseUp(object sender, MouseEventArgs e)
		{
			if (e.Button != System.Windows.Forms.MouseButtons.Right ||
				fileListView.SelectedIndices.Count == 0 )
				return;
			ListViewHitTestInfo info = fileListView.HitTest(e.X, e.Y);
			if (info.Item == null)
				return;
			fileContextMenuStrip.Show(fileListView, e.X, e.Y);
		}

		private void EnableExtractRemoveButtons(bool value)
		{
			extractMenuStripButton.Enabled = value;
			extractToolStripButton.Enabled = value;
			removeToolStripButton.Enabled = value;
			removeMenuStripButton.Enabled = value;
		}
	}
}

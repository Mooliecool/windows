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
using System.Windows.Forms;

namespace Microsoft.Sample.Compression
{
	/// <summary>
	/// File Dialog that has the features for this application.
	/// </summary>
	public class CustomFileDialog 
	{
		//Private openFileDialog control
		private System.Windows.Forms.OpenFileDialog fileDialog;

		/// <summary>
		/// Creates a new Custom File Dialog object
		/// </summary>
		public CustomFileDialog()
		{
			fileDialog = new OpenFileDialog();
			Reset();
		}

		private void Reset()
		{
			fileDialog.CheckFileExists = false;
			fileDialog.CheckPathExists = false;
			fileDialog.ValidateNames = false;
			fileDialog.InitialDirectory = ZipConstants.Dot;
		}
		/// <summary>
		/// Opens the dialog in add mode
		/// User can select multiple files
		/// </summary>
		/// <returns>
		///	All the filenames selected 
		///</returns>
		public string[] AddMode()
		{
			fileDialog.FileName = String.Empty;
			fileDialog.Multiselect = true;
			fileDialog.Title = ZipConstants.AddFiles;
			fileDialog.Filter = ZipConstants.AllExtensions;
			DialogResult dr = fileDialog.ShowDialog();
			if (dr == DialogResult.OK)
				return fileDialog.FileNames;
			return null;
		}

		/// <summary>
		/// Opens the dialog in new mode
		/// User cannot select multiple files
		/// </summary>
		/// <returns>
		///	The filename selected 
		///</returns>
		public string NewMode()
		{
			fileDialog.FileName = String.Empty;
			fileDialog.Multiselect = false;
			fileDialog.Title = ZipConstants.NewArchive;
			fileDialog.Filter = ZipConstants.Extension;
			//In case the filedialog does not add extensions
		
			DialogResult dr = fileDialog.ShowDialog();
			if (dr == DialogResult.OK)
			{
				if (!fileDialog.FileName.Contains(".xip"))
					fileDialog.FileName += ".xip";
				return fileDialog.FileName;
			}
			return null;
		}

		/// <summary>
		/// Opens the dialog in open mode
		/// User cannot select multiple files
		/// </summary>
		/// <returns>
		///	The filename selected 
		///</returns>
		public string OpenMode()
		{
			fileDialog.FileName = String.Empty;
			
			fileDialog.Multiselect = false;
			fileDialog.Title = ZipConstants.OpenArchive;
			fileDialog.Filter = ZipConstants.Extension;
			DialogResult dr = fileDialog.ShowDialog();
			if (dr == DialogResult.OK)
			{
				if (!fileDialog.FileName.Contains(ZipConstants.Dot))
					fileDialog.FileName += ".xip";
				return fileDialog.FileName;
			}
			return null;
		}

	}
	
}

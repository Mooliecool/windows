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
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms ;
using System.IO;
namespace Microsoft.Samples.Linq
{

    /// <summary>
    /// static class that holds custom Extension Methods
    /// </summary>
    public static class ExtensionMethods
    {
        /// <summary>
        /// Extension Method for IEnumerable<FileInfo> which enumerates the items and creates new TreeNodes and adds it to the root TreeNode
        /// </summary>
        /// <param name="source"></param>
        /// <param name="root"></param>
        public static void AddNodes(this IEnumerable<FileInfo> source, TreeNode root )
        {
            //for every FileInfo in the source
            foreach (FileInfo  fi in source)
            {
                //Create and add new TreeNode to root
                TreeNode n = root.Nodes.Add(fi.Name);

                //set appropriate image (directory or just a file)
                if ((fi.Attributes & FileAttributes.Directory) == FileAttributes.Directory)
                    n.ImageIndex = 0;
                else
                    n.ImageIndex = 1;
            }
            //expand the root
            root.Expand();
        }
    }
}

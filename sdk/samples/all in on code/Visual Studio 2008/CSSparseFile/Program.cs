/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSSparseFile
* Copyright (c) Microsoft Corporation.
* 
* CSSparseFile demonstrates the common operations on sparse files. A sparse 
* file is a type of computer file that attempts to use file system space more 
* efficiently when blocks allocated to the file are mostly empty. This is 
* achieved by writing brief information (metadata) representing the empty 
* blocks to disk instead of the actual "empty" space which makes up the 
* block, using less disk space. You can find in this example the creation of 
* sparse file, the detection of sparse attribute, the retrieval of sparse 
* file size, and the query of sparse file layout.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/4/2009 12:21 AM Jialiang Ge Created
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
#endregion


class Program
{
    static void Main(string[] args)
    {
        /////////////////////////////////////////////////////////////////////
        // Determine if the volume support sparse streams.
        // 

        if (!SparseFile.VolumeSupportsSparseFiles("C:\\"))
        {
            Console.WriteLine("Volume {0} does not support sparse streams",
                "C:\\");
            return;
        }


        /////////////////////////////////////////////////////////////////////
        // Create a sparse file.
        // 

        string fileName = "SparseFile.tmp";
        Console.WriteLine("Create sparse file: {0}", fileName);

        using (FileStream fs = SparseFile.Create(fileName))
        {
            // Write a large block of data

            const int blockLength = 512 * 1024; // 512KB
            byte[] block = new byte[blockLength];
            for (int i = 0; i < blockLength; i++)
                block[i] = 0xFF;

            fs.Write(block, 0, blockLength);

            // Set some sparse ranges in the block

            SparseFile.SetSparseRange(fs.SafeFileHandle, 0, 64 * 1024 /*64KB*/);
            SparseFile.SetSparseRange(fs.SafeFileHandle, 128 * 1024, 128 * 1024);

            // Set sparse block at the end of the file

            // 1GB sparse zeros are extended to the end of the file
            fs.SetLength(fs.Length + 0x40000000 /*1GB*/);
        }


        /////////////////////////////////////////////////////////////////////
        // Determine if a file is sparse.
        // 

        bool isSparse = SparseFile.IsSparseFile(fileName);
        Console.WriteLine("The file is{0} sparse", isSparse ? "" : " not");


        /////////////////////////////////////////////////////////////////////
        // Get file size.
        // 

        SparseFile.GetSparseFileSize(fileName);


        /////////////////////////////////////////////////////////////////////
        // Query the sparse file layout.
        // 

        SparseFile.GetSparseRanges(fileName);
    }
}
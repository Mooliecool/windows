// XpsFlatFile SDK Sample - Program.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.IO;
using System.Windows.Documents;
using System.Windows.Forms;
using System.Windows.Xps.Serialization;


namespace XpsFlatFile
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            // Write WPF Content - A FixedDocumentSequence with 'n'
            // FixedDocuments; each FixedDocument containing 2 FixedPages.

            Console.WriteLine("\nStarting creation of XPS flat-file document.");

            int numberOfDocument = 5; // number of documents in the sequence.
            FixedDocumentSequence fixedDocSeq =
                WpfContent.CreateFixedDocumentSequence(numberOfDocument);

            // Set the output directory
            String path = Directory.GetCurrentDirectory() + @"\Output";

            FlatPackagingPolicy packagingPolicy = new FlatPackagingPolicy(path);

            XpsSerializationManager xpsManager =
                new XpsSerializationManager(packagingPolicy, false);

            xpsManager.SaveAsXaml(fixedDocSeq);

            Console.WriteLine("\nCompleted output of XPS flat-file document to folder:");
            Console.WriteLine(path);

            // Normal Completion, show where the XPS document flat file was written.
            string msg = "Completed output of XPS flat-file document to folder:\n\n" + path;
            MessageBox.Show(msg, "Normal Completion",
                MessageBoxButtons.OK, MessageBoxIcon.Information);

        }// end:Main

    }// end:class Program

}// end:namespace XpsFlatFile

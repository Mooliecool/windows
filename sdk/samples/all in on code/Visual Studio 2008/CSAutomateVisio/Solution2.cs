/******************************** Module Header ********************************\
* Module Name:  Solution2.cs
* Project:      CSAutomateVisio
* Copyright (c) Microsoft Corporation.
* 
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Reflection;

using Visio = Microsoft.Office.Interop.Visio;
#endregion


namespace CSAutomateVisio
{
    static class Solution2
    {
        public static void AutomateVisio()
        {
            AutomateVisioImpl();


            // Clean up the unmanaged Visio COM resources by forcing a garbage 
            // collection as soon as the calling function is off the stack (at 
            // which point these objects are no longer rooted).

            GC.Collect();
            GC.WaitForPendingFinalizers();
            // GC needs to be called twice in order to get the Finalizers called 
            // - the first time in, it simply makes a list of what is to be 
            // finalized, the second time in, it actually is finalizing. Only 
            // then will the object do its automatic ReleaseComObject.
            GC.Collect();
            GC.WaitForPendingFinalizers();
        }

        private static void AutomateVisioImpl()
        {
            try
            {
                // Create an instance of Microsoft Visio and make it invisible.

                Visio.Application oVisio = new Visio.Application();
                oVisio.Visible = false;
                Console.WriteLine("Visio.Application is started");

                // Create a new Document based on no template.

                Visio.Document oDoc = oVisio.Documents.Add("");
                Console.WriteLine("A new document is created");

                // Draw a rectangle and a oval on the first page.

                Console.WriteLine("Draw a rectangle and a oval");
                oDoc.Pages[1].DrawRectangle(0.5, 10.25, 6.25, 7.375);
                oDoc.Pages[1].DrawOval(1.125, 6, 6.875, 2.125);

                // Save the document as a vsd file and close it.

                Console.WriteLine("Save and close the document");
                string fileName = Path.GetDirectoryName(
                    Assembly.GetExecutingAssembly().Location) + "\\Sample2.vsd";
                oDoc.SaveAs(fileName);
                oDoc.Close();

                // Quit the Visio application.

                Console.WriteLine("Quit the Visio application");
                oVisio.Quit();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Solution2.AutomateVisio throws the error: {0}",
                    ex.Message);
            }
        }
    }
}

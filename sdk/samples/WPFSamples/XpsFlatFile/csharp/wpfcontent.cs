// XpsFlatFile SDK Sample - WpfContent.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Media;
using System.Windows.Documents;
using System.Windows.Controls;
using System.Windows.Markup;
using System.Windows;


namespace XpsFlatFile
{
    // =========================== class WpfContent ===========================
    /// <summary>
    ///   Generates sample XAML content for a sample XPS document.</summary>
    class WpfContent
    {
        // ------------------- CreateFixedDocumentSequence --------------------
        /// <summary>
        ///   Creates a FixedDocumentSequence that contains
        ///   a specified number of documents.</summary>
        /// <param name="numDocs">
        ///   Number of FixedDocuments to create in the FixedDocumentSequence.</param>
        /// <returns>
        ///   The FixedDocumentSequence with the specified number of documents.</returns>
        /// <remarks>
        ///     Each FixedDocument consists of two (2) FixedPages.<remarks>
        static public FixedDocumentSequence CreateFixedDocumentSequence(int numDocs)
        {
            FixedDocumentSequence fixedDocSeq   = new FixedDocumentSequence();
            FixedDocument[]       fixedDocs     = new FixedDocument[numDocs];
            DocumentReference[]   docReferences = new DocumentReference[numDocs];

            for (int i = 0; i < numDocs; i++)
            {
                fixedDocs[i] = CreateFixedDocument();
            }

            for (int i = 0; i < numDocs; i++)
            {
                docReferences[i] = new DocumentReference();
            }

            for (int i = 0; i < numDocs; i++)
            {
                docReferences[i].BeginInit();
                docReferences[i].SetDocument(fixedDocs[i]);
                docReferences[i].EndInit();

                (fixedDocSeq as IAddChild).AddChild(docReferences[i]);
            }

            return fixedDocSeq;
        }


        // ----------------------- CreateFixedDocument ------------------------
        /// <summary>
        ///   Creates a FixedDocument with two (2) FixedPages.</summary>
        /// <returns>
        ///   The FixedDocument.</returns>
        static private FixedDocument CreateFixedDocument()
        {
            FixedDocument fixedDocument = new FixedDocument();

            //Create Pages
            PageContent pageContent1 = new PageContent();
            FixedPage fixedPage1 = new FixedPage();

            Canvas canvas1 = new Canvas();
            FillCanvas(canvas1);
            SetUpPage(fixedPage1, canvas1);

            pageContent1.BeginInit();
            ((IAddChild)pageContent1).AddChild(fixedPage1);
            pageContent1.EndInit();

            PageContent pageContent2 = new PageContent();
            FixedPage fixedPage2 = new FixedPage();

            Canvas canvas2 = new Canvas();
            FillCanvas(canvas2);
            SetUpPage(fixedPage2, canvas2);

            pageContent2.BeginInit();
            ((IAddChild)pageContent2).AddChild(fixedPage2);
            pageContent2.EndInit();

            //Attach Pages
            ((IAddChild)fixedDocument).AddChild(pageContent1);
            ((IAddChild)fixedDocument).AddChild(pageContent2);
            return fixedDocument;
        }


        // ---------------------------- SetUpPage -----------------------------
        /// <summary>
        ///   Sets the size of canvas and adds the canvas to the FixedPage.
        ///   Performs measuring and arranging to update the FixedPage.
        /// </summary>
        /// <param name="fixedPage">
        ///   The FixedPage to set up </param>
        /// <param name="canvas1">
        ///   The canvas to use.</param>
        static private void SetUpPage(FixedPage fixedPage, Canvas canvas1)
        {
            FixedPage.SetLeft(canvas1, 0);
            FixedPage.SetTop(canvas1, 0);

            double pageWidth = 96 * 8.5;
            double pageHeight = 96 * 11;

            fixedPage.Width = pageWidth;
            fixedPage.Height = pageHeight;

            fixedPage.Children.Add(canvas1);

            Size sz = new Size(8.5 * 96, 11 * 96);
            fixedPage.Measure(sz);
            fixedPage.Arrange(new Rect(new Point(), sz));
            fixedPage.UpdateLayout();
        }


        // ---------------------------- FillCanvas ----------------------------
        /// <summary>
        ///   Fills the canvas with predefined glyphs.</summary>
        /// <param name="canvas1">
        ///   The canvas to fill.</param>
        static private void FillCanvas(Canvas canvas1)
        {
            canvas1.Width = 96 * 8.5;
            canvas1.Height = 96 * 11;
            string fontsPath = Directory.GetCurrentDirectory() + @"\Fonts\";

            Glyphs glyph = new Glyphs();
            glyph.FontUri = new Uri(fontsPath + "T2Embed.TTF");
            glyph.FontRenderingEmSize = 32;
            glyph.StyleSimulations = StyleSimulations.None;
            glyph.IsSideways = false;
            glyph.BidiLevel = 0;
            Canvas.SetTop(glyph, 0);
            Canvas.SetLeft(glyph, 0);
            glyph.Fill = Brushes.DarkBlue;
            glyph.UnicodeString = "TopLeft";
            canvas1.Children.Add(glyph);

            glyph = new Glyphs();
            glyph.FontUri = new Uri(fontsPath + "T2Embez.TTF");
            glyph.FontRenderingEmSize = 32;
            glyph.StyleSimulations = StyleSimulations.None;
            glyph.IsSideways = false;
            glyph.BidiLevel = 0;
            Canvas.SetTop(glyph, 0);
            Canvas.SetLeft(glyph, 520);
            glyph.Fill = Brushes.BurlyWood;
            glyph.UnicodeString = "TopRight";
            canvas1.Children.Add(glyph);
        }

    }// end:class WpfContent

}// end:namespace XpsFlatFile

/************************************* Module Header **************************************\
* Module Name:	PaneControl.cs
* Project:		CSVstoWordDocument
* Copyright (c) Microsoft Corporation.
* 
* The CSWordDocument project provides the examples on how manipulate Word 2007 Content Controls 
* in a VSTO document-level project
 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.VisualStudio.Tools.Applications.Runtime;
using Office = Microsoft.Office.Core;
using Word = Microsoft.Office.Interop.Word;
using Microsoft.Win32;
#endregion


namespace CSVstoWordDocument
{
    public partial class PaneControl : UserControl
    {
        object RangeStart = 0;
        object RangeEnd = 0;
        object selection = Globals.ThisDocument.ThisApplication.Selection.Range;

        #region Main functions

        public PaneControl()
        {
            InitializeComponent();

        }

        private void LoadDefaultBuildingBlock()
        {
            object Categorie = "Built-In";
            object BuildingBlockName = "Double Table ";
            object True = true;

            //The path of the Building Block.dotx may depends on your opreating system,
            //We could use HKLM hive to locate Word.
            object WordPath;
            WordPath = Registry.GetValue(@"HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Office\12.0\Word\InstallRoot", "Path", "");
            Globals.ThisDocument.AttachedTemplate = WordPath + @"Document Parts\1033\Building Blocks.dotx";
            Word.Template template = (Word.Template)Globals.ThisDocument.AttachedTemplate;
            //For example we'll insert the "Double Table" into our document, we can located the specific Building Block via WdBuildingBlockTypes,Categorie and BuildingBlockName.
            template.BuildingBlockTypes.Item(Microsoft.Office.Interop.Word.WdBuildingBlockTypes.wdTypeTables).Categories.Item(ref Categorie).BuildingBlocks.Item(ref BuildingBlockName).Insert(Globals.ThisDocument.Application.Selection.Range, ref True);
        }

        private Word.ContentControl AddContentControl(Word.WdContentControlType ct, ref object selection)
        {
            object units = Word.WdUnits.wdCharacter;
            object count = 1;
            object missing = Type.Missing;

            Word.ContentControl ContentControl = Globals.ThisDocument.ContentControls.Add(ct, ref selection);
            ContentControl.Range.Select();
            //by default after we inserted the content control the cursion is within the scope of that control,therefore,use Word OM to move the cursor out of the 
            //new control, and ready to insert the next one.
            Globals.ThisDocument.ThisApplication.Selection.MoveRight(ref units, ref count, ref missing);
            Globals.ThisDocument.ThisApplication.Selection.TypeParagraph();
            selection = Globals.ThisDocument.ThisApplication.Selection.Range;
            return ContentControl;
        }

        #endregion

        #region Event Handlers

        private void BtnInsertCC_Click(object sender, EventArgs e)
        {

            Globals.ThisDocument.Range(ref RangeStart, ref RangeEnd).Select();
            AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlBuildingBlockGallery, ref selection);

            AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlComboBox, ref selection);

            AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlDate, ref selection);

            AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlDropdownList, ref selection);

            AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlPicture, ref selection);

            AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlRichText, ref selection);

        }

        private void btnInsertBuiltIn_Click(object sender, EventArgs e)
        {
            Globals.ThisDocument.Range(ref RangeStart, ref RangeEnd).Select();
            LoadDefaultBuildingBlock();
        }

        private void btnCCXMLMapping_Click(object sender, EventArgs e)
        {
            Globals.ThisDocument.Range(ref RangeStart, ref RangeEnd).Select();
            //mapping to the value of creator of this document.
            AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlText, ref selection).XMLMapping.SetMapping("/ns1:coreProperties[1]/ns0:creator[1]", "",
             Globals.ThisDocument.CustomXMLParts[1]);
        }

        #endregion

    }
}

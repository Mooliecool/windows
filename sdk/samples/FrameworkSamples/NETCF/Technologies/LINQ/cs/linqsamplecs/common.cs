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
namespace Microsoft.Samples.Linq
{
    /// <summary>
    /// Class which holds static instances of both SearchForm and GroupForm
    /// </summary>
    static class Common
    {
        //static instance of SearchForm
        private static Form searchForm;

        /// <summary>
        /// public static Property that gets/sets static instance of SearchForm
        /// </summary>
        public static Form SearchForm
        {            
            get { return Common.searchForm; }
            set { Common.searchForm = value; }
        }

        //static instance of GroupForm
        private static Form groupForm ;

        /// <summary>
        /// public static Property that gets/sets static instance of GroupForm
        /// </summary>
        public static Form GroupForm
        {
            get { return Common.groupForm; }
            set { Common.groupForm = value; }
        }

        /// <summary>
        /// CloseAll, closes all instances of the forms (if created)
        /// </summary>
        public static void CloseAll()
        {
            if (searchForm != null)
                searchForm.Close();

            if (groupForm != null)
                groupForm.Close();
        }
    }
}

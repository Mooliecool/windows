using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Defines commonly used file filters.
    /// </summary>
    public static class CommonFileDialogStandardFilters
    {
        private static CommonFileDialogFilter textFilesFilter;
        /// <summary>
        /// Gets a filter for *.txt files.
        /// </summary>
        public static CommonFileDialogFilter TextFiles
        {
            get
            {
                if (textFilesFilter == null)
                    textFilesFilter = new CommonFileDialogFilter("Text Files", "*.txt");
                return textFilesFilter;
            }
        }

        private static CommonFileDialogFilter officeFilesFilter;
        /// <summary>
        /// Gets a filter for Microsoft Word and Excel files.
        /// </summary>
        public static CommonFileDialogFilter OfficeFiles
        {
            get
            {
                if (officeFilesFilter == null)
                    officeFilesFilter = new CommonFileDialogFilter("Office Files", "*.doc, *.xls, *.docx");
                return officeFilesFilter;
            }
        }
    }
}

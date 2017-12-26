using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Strongly typed collection for file dialog filters.
    /// </summary>
    public class CommonFileDialogFilterCollection : Collection<CommonFileDialogFilter>
    {
        internal SafeNativeMethods.COMDLG_FILTERSPEC[] GetAllFilterSpecs()
        {
            SafeNativeMethods.COMDLG_FILTERSPEC[] filterSpecs = 
                new SafeNativeMethods.COMDLG_FILTERSPEC[this.Count];

            for (int i = 0; i < this.Count; i++)
                filterSpecs[i] = this[i].GetFilterSpec();

            return filterSpecs;
        }
    }
}

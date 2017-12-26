using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Identifies one of the standard buttons that 
    /// can be displayed via TaskDialog.
    /// </summary>
    [Flags]
    public enum TaskDialogStandardButton
    {
        None    = 0x0000,
        Ok      = 0x0001,
        Yes     = 0x0002,
        No      = 0x0004,
        Cancel  = 0x0008,
        Retry   = 0x0010,
        Close   = 0x0020
    }
}

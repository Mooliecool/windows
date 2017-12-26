using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms.Integration;
using System.Runtime.InteropServices;

namespace Wizard
{
    public class MyHost : WindowsFormsHost
    {
        public IntPtr ParentHandle;

        protected override System.Runtime.InteropServices.HandleRef BuildWindowCore(System.Runtime.InteropServices.HandleRef hwndParent)
        {
            HandleRef h = base.BuildWindowCore(hwndParent);

            ParentHandle = hwndParent.Handle;

            return h;
        }
    }
}

using System;
using System.Security;
using MS.Internal;
using MS.Win32.Penimc;

namespace System.Windows.Input
{
    /////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     Struct used to store new PenContext information.
    /// </summary>
    internal struct PenContextInfo
    {
        /// <SecurityNote>
        ///     This data is not safe to expose as it holds refrence to IPimcTablet
        /// </SecurityNote>
        [SecurityCritical]
        public SecurityCriticalDataClass<IPimcContext> PimcContext;
        
        /// <SecurityNote>
        ///     This data is not safe to expose as it holds refrence to IPimcTablet
        /// </SecurityNote>
        [SecurityCritical]
        public SecurityCriticalDataClass<IntPtr> CommHandle;
        
        public int ContextId;
    }
    
}



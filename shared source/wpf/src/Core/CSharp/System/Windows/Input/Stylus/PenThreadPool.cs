using System;
using System.Collections.Generic;
using System.Threading;
using System.Windows.Threading;
using System.Security;
using System.Security.Permissions;
using MS.Win32.Penimc;

namespace System.Windows.Input
{
    /////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// 
    /// </summary>
    internal class PenThreadPool
    {
        /// <SecurityNote>
        /// Critical -  Constructor for singleton of our PenThreadPool.
        ///             marking this critical to prevent inadvertant access by transparent code
        ///
        ///         Called by critical methods:
        ///             Instance (above)
        ///
        /// </SecurityNote>
        [SecurityCritical]
        static PenThreadPool()
        {
        }
        
        /////////////////////////////////////////////////////////////////////
        /// <summary>
        /// 
        /// </summary>
        /// <SecurityNote>
        /// Critical -  marking this critical to prevent inadvertant 
        ///             access by transparent code             
        ///
        /// </SecurityNote>
        [SecurityCritical]
        [ThreadStatic]
        private static PenThreadPool _penThreadPool;
        
        /////////////////////////////////////////////////////////////////////
        /// <summary>
        /// </summary>
        /// <SecurityNote>
        /// Critical -  Returns a PenThread (creates as needed).
        ///             marking this critical to prevent inadvertant access by transparent code
        ///
        ///         Called by critical methods:
        ///             PenContext.Dispose
        ///             PenContext.Enable
        ///             PenContext.Disable
        ///
        /// </SecurityNote>
        [SecurityCritical]
        internal static PenThread GetPenThreadForPenContext(PenContext penContext)
        {
            // Create the threadstatic DynamicRendererThreadManager as needed for calling thread.
            // It only creates one 
            if (_penThreadPool == null)
            {
                _penThreadPool = new PenThreadPool();
            }
            return _penThreadPool.GetPenThreadForPenContextHelper(penContext); // Adds to weak ref list if creating new one.
        }

        /////////////////////////////////////////////////////////////////////
        /// <summary>
        /// 
        /// </summary>
        /// <SecurityNote>
        /// Critical -  marking this critical to prevent inadvertant 
        ///             access by transparent code             
        ///
        /// </SecurityNote>
        [SecurityCritical]
        private List<WeakReference> _penThreadWeakRefList;
        
        /////////////////////////////////////////////////////////////////////
        /// <summary>
        /// 
        /// </summary>
        /// <SecurityNote>
        /// Critical -  Initializes critical data: m_PenThreads  
        ///
        /// </SecurityNote>
        [SecurityCritical]
        internal PenThreadPool()
        {
            _penThreadWeakRefList = new List<WeakReference>();
        }

        /// <SecurityNote>
        /// Critical - Calls SecurityCritical code (PenThread constructor).
        ///             Called by BeginService.
        ///             TreatAsSafe boundry is Stylus.EnableCore, Stylus.RegisterHwndForInput
        ///                and HwndWrapperHook class (via HwndSource.InputFilterMessage).
        /// </SecurityNote>
        [SecurityCritical]
        private PenThread GetPenThreadForPenContextHelper(PenContext penContext)
        {
            bool needCleanup = false;
            PenThread penThread = null;
            int i;
            
            // Scan existing penthreads to see if we have an available slot for context.
            for (i=0; i < _penThreadWeakRefList.Count; i++)
            {
                PenThread penThreadFound = _penThreadWeakRefList[i].Target as PenThread;
                
                if (penThreadFound == null)
                {
                    needCleanup = true;
                }
                else
                {
                    // See if we can use this one
                    if (penContext == null || penThreadFound.AddPenContext(penContext))
                    {
                        // We can use this one.
                        penThread = penThreadFound;
                        break;
                    }
                }
            }

            if (needCleanup)
            {
                // prune invalid refs
                for (i=_penThreadWeakRefList.Count - 1; i >= 0; i--)
                {
                    if (_penThreadWeakRefList[i].Target == null)
                    {
                        _penThreadWeakRefList.RemoveAt(i);
                    }
                }
            }

            if (penThread == null)
            {
                penThread = new PenThread();
                // Make sure we add this context to the penthread
                if (penContext != null)
                {
                    penThread.AddPenContext(penContext);
                }
                _penThreadWeakRefList.Add(new WeakReference(penThread));
            }
            
            return penThread;
        }
    }
}

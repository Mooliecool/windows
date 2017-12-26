using System;
using MS.Internal.WindowsBase;
using System.Runtime.Versioning;

namespace System.Windows
{
    public static class BaseCompatibilityPreferences
    {
        #region ReuseDispatcherSynchronizationContextInstance
        /// <summary>
        ///     WPF 4.0 had a performance optimization where it would
        ///     frequently reuse the same instance of the
        ///     DispatcherSynchronizationContext when preparing the
        ///     ExecutionContext for invoking a DispatcherOperation.  This
        ///     had observable impacts on behavior.
        ///
        ///     1) Some task-parallel implementations check the reference
        ///         equality of the SynchronizationContext to determine if the
        ///         completion can be inlined - a significant performance win.
        ///
        ///     2) But, the ExecutionContext would flow the
        ///         SynchronizationContext which could result in the same
        ///         instance of the DispatcherSynchronizationContext being the
        ///         current SynchronizationContext on two different threads.
        ///         The continuations would then be inlined, resulting in code
        ///         running on the wrong thread.
        ///
        ///     In 4.5 we changed this behavior to use a new instance of the
        ///     DispatcherSynchronizationContext for every operation, and
        ///     whenever SynchronizationContext.CreateCopy is called - such
        ///     as when the ExecutionContext is being flowed to another thread.
        ///     This has its own observable impacts:
        ///
        ///     1) Some task-parallel implementations check the reference
        ///         equality of the SynchronizationContext to determine if the
        ///         completion can be inlined - since the instances are
        ///         different, this causes them to resort to the slower
        ///         path for potentially cross-thread completions.
        ///
        ///     2) Some task-parallel implementations implement potentially
        ///         cross-thread completions by callling
        ///         SynchronizationContext.Post and Wait() and an event to be
        ///         signaled.  If this was not a true cross-thread completion,
        ///         but rather just two seperate instances of
        ///         DispatcherSynchronizationContext for the same thread, this
        ///         would result in a deadlock.
        /// </summary>
        public static bool ReuseDispatcherSynchronizationContextInstance
        {
            get { return _reuseDispatcherSynchronizationContextInstance; }
            set 
            { 
                lock (_lockObject)
                {
                    if (_isSealed)
                    {
                        throw new InvalidOperationException(SR.Get(SRID.CompatibilityPreferencesSealed, "ReuseDispatcherSynchronizationContextInstance", "BaseCompatibilityPreferences"));
                    }
                    
                    _reuseDispatcherSynchronizationContextInstance = value; 
                }
            }
        }

        internal static bool GetReuseDispatcherSynchronizationContextInstance()
        {
            Seal();

            return ReuseDispatcherSynchronizationContextInstance;
        }

        private static bool _reuseDispatcherSynchronizationContextInstance = BinaryCompatibility.TargetsAtLeast_Desktop_V4_5 ? false : true;
        #endregion ReuseDispatcherSynchronizationContextInstance

        #region FlowDispatcherSynchronizationContextPriority
        /// <summary>
        ///     WPF <= 4.0 a DispatcherSynchronizationContext always used
        ///     DispatcherPriority.Normal to satisfy
        ///     SynchronizationContext.Post and SynchronizationContext.Send
        ///     calls.
        ///
        ///     With the inclusion of async Task-oriented programming in
        ///     .Net 4.5, we now record the priority of the DispatcherOperation
        ///     in the DispatcherSynchronizationContext and use that to satisfy
        ///     SynchronizationContext.Post and SynchronizationContext.Send
        ///     calls.  This enables async operations to "resume" after an
        ///     await statement at the same priority they are currently running
        ///     at.
        ///
        ///     This is, of course, an observable change in behavior.
        /// </summary>
        public static bool FlowDispatcherSynchronizationContextPriority
        {
            get { return _flowDispatcherSynchronizationContextPriority; }
            set 
            { 
                lock (_lockObject)
                {
                    if (_isSealed)
                    {
                        throw new InvalidOperationException(SR.Get(SRID.CompatibilityPreferencesSealed, "FlowDispatcherSynchronizationContextPriority", "BaseCompatibilityPreferences"));
                    }
                    
                    _flowDispatcherSynchronizationContextPriority = value; 
                }
            }
        }

        internal static bool GetFlowDispatcherSynchronizationContextPriority()
        {
            Seal();
                
            return FlowDispatcherSynchronizationContextPriority;
        }

        private static bool _flowDispatcherSynchronizationContextPriority = BinaryCompatibility.TargetsAtLeast_Desktop_V4_5 ? true : false;
        #endregion FlowDispatcherSynchronizationContextPriority

        #region InlineDispatcherSynchronizationContextSend
        /// <summary>
        ///     WPF <= 4.0 a DispatcherSynchronizationContext always used
        ///     DispatcherPriority.Normal to satisfy
        ///     SynchronizationContext.Post and SynchronizationContext.Send
        ///     calls.  This can result in unexpected re-entrancy when calling
        ///     SynchronizationContext.Send on the same thread, since it still
        ///     posts through the Dispatcher queue.
        ///
        ///     In WPF 4.5 we are changing the behavior such that calling
        ///     SynchronizationContext.Send on the same thread, will not post
        ///     through the Dispatcher queue, but rather invoke the delegate
        ///     more directly.  The cross-thread behavior does not change.
        ///
        ///     This is, of course, an observable change in behavior.
        /// </summary>
        public static bool InlineDispatcherSynchronizationContextSend
        {
            get { return _inlineDispatcherSynchronizationContextSend; }
            set 
            { 
                lock (_lockObject)
                {
                    if (_isSealed)
                    {
                        throw new InvalidOperationException(SR.Get(SRID.CompatibilityPreferencesSealed, "InlineDispatcherSynchronizationContextSend", "BaseCompatibilityPreferences"));
                    }
                    
                    _inlineDispatcherSynchronizationContextSend = value; 
                }
            }
        }

        internal static bool GetInlineDispatcherSynchronizationContextSend()
        {
            Seal();

            return InlineDispatcherSynchronizationContextSend;
        }

        private static bool _inlineDispatcherSynchronizationContextSend = BinaryCompatibility.TargetsAtLeast_Desktop_V4_5 ? true : false;
        #endregion InlineDispatcherSynchronizationContextSend
        
        private static void Seal()
        {
            if (!_isSealed)
            {
                lock (_lockObject)
                {
                    _isSealed = true;
                }
            }
        }

        private static bool _isSealed;
        private static object _lockObject = new object();
    }
}

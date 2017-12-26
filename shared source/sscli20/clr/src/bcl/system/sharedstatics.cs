// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*=============================================================================
**
** Class: SharedStatics
**
**
** Purpose: Container for statics that are shared across AppDomains.
**
**
=============================================================================*/

namespace System {

    using System.Threading;
    using System.Runtime.Remoting;
    using System.Security;
    using System.Security.Util;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;
    using StringMaker = System.Security.Util.Tokenizer.StringMaker;

    internal sealed class SharedStatics
    {
        // this is declared static but is actually forced to be the same object 
        // for each AppDomain at AppDomain create time.
        internal static SharedStatics _sharedStatics;
        
        // when we create the single object we can construct anything we will need
        // here. If not too many, then just create them all in the constructor, otherwise
        // can have the property check & create. Need to be aware of threading issues 
        // when do so though.
        // Note: This ctor is not called when we setup _sharedStatics via AppDomain::SetupSharedStatics
        SharedStatics() {
            _Remoting_Identity_IDGuid = null;
            _Remoting_Identity_IDSeqNum = 0x40; // Reserve initial numbers for well known objects.
            _maker = null;
        }

        private String _Remoting_Identity_IDGuid;
        public static String Remoting_Identity_IDGuid 
        { 
            get 
            {
                if (_sharedStatics._Remoting_Identity_IDGuid == null)
                {
                    bool tookLock = false;
                    RuntimeHelpers.PrepareConstrainedRegions();
                    try {
                        Monitor.ReliableEnter(_sharedStatics, ref tookLock);

                        if (_sharedStatics._Remoting_Identity_IDGuid == null)
                        {
                            _sharedStatics._Remoting_Identity_IDGuid = Guid.NewGuid().ToString().Replace('-', '_');
                        }
                    }
                    finally {
                        if (tookLock)
                            Monitor.Exit(_sharedStatics);
                    }
                }

                BCLDebug.Assert(_sharedStatics._Remoting_Identity_IDGuid != null,
                                "_sharedStatics._Remoting_Identity_IDGuid != null");
                return _sharedStatics._Remoting_Identity_IDGuid;
            } 
        }
        
        private StringMaker _maker;
        static public StringMaker GetSharedStringMaker()
        {
            StringMaker maker = null;
            
            bool tookLock = false;
            RuntimeHelpers.PrepareConstrainedRegions();
            try {
                Monitor.ReliableEnter(_sharedStatics, ref tookLock);

                if (_sharedStatics._maker != null)
                {
                    maker = _sharedStatics._maker;
                    _sharedStatics._maker = null;
                }
            }
            finally {
                if (tookLock)
                    Monitor.Exit(_sharedStatics);
            }
            
            if (maker == null)
            {
                maker = new StringMaker();
            }
            
            return maker;
        }

        static public void ReleaseSharedStringMaker(ref StringMaker maker)
        {
            // save this stringmaker so someone else can use it
            bool tookLock = false;
            RuntimeHelpers.PrepareConstrainedRegions();
            try
            {
                Monitor.ReliableEnter(_sharedStatics, ref tookLock);

                _sharedStatics._maker = maker;
                maker = null;
            }
            finally {
                if (tookLock)
                    Monitor.Exit(_sharedStatics);
            }
        }

        // Note this may not need to be process-wide.
        private int _Remoting_Identity_IDSeqNum;
        internal static int Remoting_Identity_GetNextSeqNum()
        {
            return Interlocked.Increment(ref _sharedStatics._Remoting_Identity_IDSeqNum);
        }


        // This is the total amount of memory currently "reserved" via
        // all MemoryFailPoints allocated within the process.
        // Stored as a long because we need to use Interlocked.Add.
        private long _memFailPointReservedMemory;

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static long AddMemoryFailPointReservation(long size)
        {
            // Size can legitimately be negative - see Dispose.
            return Interlocked.Add(ref _sharedStatics._memFailPointReservedMemory, (long) size);
        }

        internal static ulong MemoryFailPointReservedMemory {
            get { 
                BCLDebug.Assert(_sharedStatics._memFailPointReservedMemory >= 0, "Process-wide MemoryFailPoint reserved memory was negative!");
                return (ulong) _sharedStatics._memFailPointReservedMemory;
            }
        }
    }
}

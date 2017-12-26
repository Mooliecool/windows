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
namespace System.Threading {
    using System;
    using System.Security.Permissions;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;

    // After much discussion, we decided the Interlocked class doesn't need 
    // any HPA's for synchronization or external threading.  They hurt C#'s 
    // codegen for the yield keyword, and arguably they didn't protect much.  
    // Instead, they penalized people (and compilers) for writing threadsafe 
    // code.
    public static class Interlocked
    {        
        /******************************
         * Increment
         *   Implemented: int
         *                        long
         *****************************/

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static extern int Increment(ref int location);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern long Increment(ref long location);

        /******************************
         * Decrement
         *   Implemented: int
         *                        long
         *****************************/

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static extern int Decrement(ref int location);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern long Decrement(ref long location);

        /******************************
         * Exchange
         *   Implemented: int
         *                        long
         *                        float
         *                        double
         *                        Object
         *                        IntPtr
         *****************************/

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static extern int Exchange(ref int location1, int value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern long Exchange(ref long location1, long value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float Exchange(ref float location1, float value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern double Exchange(ref double location1, double value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static extern Object Exchange(ref Object location1, Object value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static extern IntPtr Exchange(ref IntPtr location1, IntPtr value);

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        [System.Runtime.InteropServices.ComVisible(false)]
        public static T Exchange<T>(ref T location1, T value) where T : class
        {
            _Exchange(__makeref(location1), __makeref(value));
            //Since value is a local we use trash its data on return
            //  The Exchange replaces the data with new data
            //  so after the return "value" contains the original location1
            //See ExchangeGeneric for more details           
            return value;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private static extern void _Exchange(TypedReference location1, TypedReference value);

        /******************************
         * CompareExchange
         *    Implemented: int
         *                         long
         *                         float
         *                         double
         *                         Object
         *                         IntPtr
         *****************************/

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static extern int CompareExchange(ref int location1, int value, int comparand);    

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern long CompareExchange(ref long location1, long value, long comparand);    

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float CompareExchange(ref float location1, float value, float comparand);    

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern double CompareExchange(ref double location1, double value, double comparand);    

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static extern Object CompareExchange(ref Object location1, Object value, Object comparand);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static extern IntPtr CompareExchange(ref IntPtr location1, IntPtr value, IntPtr comparand);

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        [System.Runtime.InteropServices.ComVisible(false)]
        public static T CompareExchange<T>(ref T location1, T value, T comparand) where T : class
        {
            _CompareExchange(__makeref(location1), __makeref(value), comparand);
            //Since value is a local we use trash its data on return
            //  The Exchange replaces the data with new data
            //  so after the return "value" contains the original location1
            //See CompareExchangeGeneric for more details
            return value;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private static extern void _CompareExchange(TypedReference location1, TypedReference value, Object comparand);

        /******************************
         * Add
         *    Implemented: int
         *                         long
         *****************************/

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern int ExchangeAdd(ref int location1, int value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern long ExchangeAdd(ref long location1, long value);

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static int Add(ref int location1, int value) 
        {
            return ExchangeAdd(ref location1, value) + value;
        }

	 [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static long Add(ref long location1, long value) 
        {
            return ExchangeAdd(ref location1, value) + value;
        }
	 
        /******************************
         * Read
         *****************************/

        public static long Read(ref long location)
        {
            return Interlocked.CompareExchange(ref location,0,0);
        }        
    }
}

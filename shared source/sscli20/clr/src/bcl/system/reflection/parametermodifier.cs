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

namespace System.Reflection 
{  
	using System;

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public struct ParameterModifier 
    {
        #region Private Data Members
		private bool[] _byRef;
        #endregion

        #region Constructor
		public ParameterModifier(int parameterCount) 
        {
			if (parameterCount <= 0)
				throw new ArgumentException(Environment.GetResourceString("Arg_ParmArraySize"));

			_byRef = new bool[parameterCount];
		}
        #endregion

        #region Internal Members
        internal bool[] IsByRefArray { get { return _byRef; } }
        #endregion

        #region Public Members
        public bool this[int index] 
        {
            get 
            {
                return _byRef[index]; 
            }
            set 
            {
                _byRef[index] = value;
            }
        }
        #endregion
    }
}

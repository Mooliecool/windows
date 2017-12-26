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
/*============================================================
**
** Class:  UnSafeBuffer
**
** Purpose: A class to detect incorrect usage of UnSafeBuffer
**
** 
===========================================================*/

namespace System {
    using System.Diagnostics;
    
    unsafe internal struct UnSafeCharBuffer{
        char * m_buffer;
        int m_totalSize;
        int m_length;    
    
        public UnSafeCharBuffer( char *buffer,  int bufferSize) {
            BCLDebug.Assert( buffer != null, "buffer pointer can't be null."  );
            BCLDebug.Assert( bufferSize >= 0, "buffer size can't be negative."  );        
            m_buffer = buffer;
            m_totalSize = bufferSize;    
            m_length = 0;
        }
    
        public void AppendString(string stringToAppend) {
            if( String.IsNullOrEmpty( stringToAppend ) ) {
                return;
            }
            
            if ( (m_totalSize - m_length) < stringToAppend.Length ) {
                throw new IndexOutOfRangeException();
            }
            
            fixed( char* pointerToString = stringToAppend ) {        
                Buffer.memcpyimpl( (byte *) pointerToString, (byte*) (m_buffer + m_length), stringToAppend.Length * sizeof(char));
            }    
            
            m_length += stringToAppend.Length;
            BCLDebug.Assert(m_length <= m_totalSize, "Buffer has been overflowed!");
        }
                
        public int Length {
            get {
                return m_length;
            } 
        }   
    }    
} 

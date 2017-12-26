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
** Class: FormatterConverter
**
**
** Purpose: A base implementation of the IFormatterConverter
**          interface that uses the Convert class and the 
**          IConvertible interface.
**
**
============================================================*/
namespace System.Runtime.Serialization {
    using System;
    using System.Globalization;

[System.Runtime.InteropServices.ComVisible(true)]
    public class FormatterConverter : IFormatterConverter {

        public FormatterConverter() {
        }

        public Object Convert(Object value, Type type) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ChangeType(value, type, CultureInfo.InvariantCulture);
        }

        public Object Convert(Object value, TypeCode typeCode) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ChangeType(value, typeCode, CultureInfo.InvariantCulture);
        }

        public bool ToBoolean(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToBoolean(value, CultureInfo.InvariantCulture);
        }

        public char   ToChar(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToChar(value, CultureInfo.InvariantCulture);
        }

		[CLSCompliant(false)]
        public sbyte  ToSByte(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToSByte(value, CultureInfo.InvariantCulture);
        }

        public byte   ToByte(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToByte(value, CultureInfo.InvariantCulture);
        }

        public short  ToInt16(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToInt16(value, CultureInfo.InvariantCulture);
        }

        [CLSCompliant(false)]
        public ushort ToUInt16(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToUInt16(value, CultureInfo.InvariantCulture);
        }

        public int    ToInt32(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToInt32(value, CultureInfo.InvariantCulture);
        }

        [CLSCompliant(false)]
        public uint   ToUInt32(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToUInt32(value, CultureInfo.InvariantCulture);
        }

        public long   ToInt64(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToInt64(value, CultureInfo.InvariantCulture);
        }

        [CLSCompliant(false)]
        public ulong  ToUInt64(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToUInt64(value, CultureInfo.InvariantCulture);
        } 

        public float  ToSingle(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToSingle(value, CultureInfo.InvariantCulture);
        }

        public double ToDouble(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToDouble(value, CultureInfo.InvariantCulture);
        }

        public Decimal ToDecimal(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToDecimal(value, CultureInfo.InvariantCulture);
        }

        public DateTime ToDateTime(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToDateTime(value, CultureInfo.InvariantCulture);
        }

        public String   ToString(Object value) {
            if (value==null) {
                throw new ArgumentNullException("value");
            }
            return System.Convert.ToString(value, CultureInfo.InvariantCulture);
        }
    }
}
        

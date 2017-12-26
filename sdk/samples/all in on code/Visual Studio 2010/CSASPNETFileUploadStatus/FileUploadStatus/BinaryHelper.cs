/****************************** Module Header ******************************\
* Module Name:    BinaryHelper.cs
* Project:        CSASPNETFileUploadStatus
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to display the upload status and progress without
* a third part component like ActiveX control, Flash or Silverlight.
* 
* This is a class which help to filter the binary data to get the file data. 
* All these static methods will help to process with the binary data. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
namespace CSASPNETFileUploadStatus
{
    internal class BinaryHelper
    {

        // Copy partial data from one source binary array.
        public static byte[] Copy(byte[] source, int index, int length)
        {
            byte[] result = new byte[length];
            Array.ConstrainedCopy(source, index, result, 0, length);
            return result;
        }

        // Combine two binary arrays into one.
        public static byte[] Combine(byte[] a, byte[] b)
        {
            if (a == null && b == null)
            {
                return null;
            }
            else if (a == null || b == null)
            {
                return a ?? b;
            }
            byte[] newData = new byte[a.Length + b.Length];
            Array.ConstrainedCopy(a, 0, newData, 0, a.Length);
            Array.ConstrainedCopy(b, 0, newData, a.Length, b.Length);
            return newData;

        }

        // Check whether two binary arrays 
        // have the same data in same index.
        public static bool Equals(byte[] source, byte[] compare)
        {
            if (source.Length != compare.Length)
            {
                return false;
            }
            if (SequenceIndexOf(source, compare, 0) != 0)
            {
                return false;
            }
            return true;
        }

        // Get partial data in the binary array.
        public static byte[] SubData(byte[] source, int startIndex)
        {
            byte[] result = new byte[source.Length - startIndex];
            Array.ConstrainedCopy(source, startIndex, result, 0, result.Length);
            return result;
        }

        // Get partial data in the binary array.
        public static byte[] SubData(byte[] source, int startIndex, int length)
        {
            byte[] result = new byte[length];
            Array.ConstrainedCopy(source, startIndex, result, 0, length);
            return result;
        }

        // Get the index in the source array from which all the data and positions
        // are same as another array.
        public static int SequenceIndexOf(byte[] source, byte[] compare)
        {
            return SequenceIndexOf(source, compare, 0);
        }
        public static int SequenceIndexOf(byte[] source, byte[] compare, int startIndex)
        {
            int result = -1;
            int sourceLen = source.Length;
            int compareLen = compare.Length;
            if (startIndex < 0)
            {
                return -1;
            }

            for (int i = startIndex; i < sourceLen - compareLen + 1; i++)
            {
                if (source[i] == compare[0] &&
                    source[i + compareLen - 1] == compare[compareLen - 1])
                {
                    int t = 0;
                    for (int j = 0; j < compare.Length; j++)
                    {
                        t++;
                        if (compare[j] != source[i + j])
                            break;
                    }
                    if (t == compareLen)
                    {
                        result = i;
                        break;
                    }
                }
            }
            return result;
        }

    }
}

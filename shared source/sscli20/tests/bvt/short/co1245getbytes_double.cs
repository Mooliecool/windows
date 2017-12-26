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
using System.IO;
using System.Text;
using System;
public class Co1245GetBytes_double
{
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co1245GetBytes_double  runTest started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        int inNumByteArrElems = 8;  
        double do8y = 0.0;
        double do9z = 0.0;
        byte[] byArr2 = null;
        byte[] byArr3Expected = null;
        try
        {
        LABEL_860_GENERAL:
            do
            {
                strLoc="Loc_100ip.  Vanilla, 0.0 testcase.";
                do9z = 0.0;
                byArr3Expected = new byte[inNumByteArrElems];
                byArr3Expected[0] = (byte)0x00;
                byArr3Expected[1] = (byte)0x00;
                byArr3Expected[2] = (byte)0x00;
                byArr3Expected[3] = (byte)0x00;
                byArr3Expected[4] = (byte)0x00;
                byArr3Expected[5] = (byte)0x00;
                byArr3Expected[6] = (byte)0x00;
                byArr3Expected[7] = (byte)0x00;
                byArr2 = null;
                byArr2 = BitConverter.GetBytes( do9z );
                ++iCountTestcases;
                if ( byArr2.Length != inNumByteArrElems )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error E_101ci!  byArr2.length==" + byArr2.Length  );
                }
                ++iCountTestcases;
                for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
                {
                    if ( byArr2[aa] != byArr3Expected[aa] )
                    {
                        ++iCountErrors;
                        Console.Error.WriteLine(  "POINTTOBREAK: Error E_102vo!  aa==" + aa + " ,byArr2[aa]==" + byArr2[aa]  );
                        break;
                    }
                }
                strLoc="Loc_200fi.   Double.MinValue";
                do9z = Double.MinValue;
                byArr3Expected = new byte[inNumByteArrElems];
                byArr3Expected[0] = (byte)0xFF;
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[1] = (byte)0xFF;
                }
                else
                {
                    byArr3Expected[1] = (byte)0xEF;
                }
                byArr3Expected[2] = (byte)0xFF;
                byArr3Expected[3] = (byte)0xFF;
                byArr3Expected[4] = (byte)0xFF;
                byArr3Expected[5] = (byte)0xFF;
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[6] = (byte)0xEF;
                }
                else
                {
                    byArr3Expected[6] = (byte)0xFF;
                }
                byArr3Expected[7] = (byte)0xff;
                byArr2 = null;
                byArr2 = BitConverter.GetBytes( do9z );
                ++iCountTestcases;
                if ( byArr2.Length != inNumByteArrElems )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error E_201ci!  byArr2.length==" + byArr2.Length  );
                }
                ++iCountTestcases;
                for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
                {
                    if ( byArr2[aa] != byArr3Expected[aa] )
                    {
                        ++iCountErrors;
                        Console.Error.WriteLine(  "POINTTOBREAK: Error E_202aa!  aa==" + aa + " ,BitConverter.ToString(byArr2)==" + BitConverter.ToString( byArr2 )  );
                        break;
                    }
                }
                strLoc="Loc_210fi.   Double.MaxValue";
                do9z = Double.MaxValue;
                byArr3Expected = new byte[inNumByteArrElems];
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[0] = (byte)0xFF;
                    byArr3Expected[1] = (byte)0xFF;
                }
                else
                {
                    byArr3Expected[0] = (byte)0x7F;
                    byArr3Expected[1] = (byte)0xEF;
                }
                byArr3Expected[2] = (byte)0xFF;
                byArr3Expected[3] = (byte)0xFF;
                byArr3Expected[4] = (byte)0xFF;
                byArr3Expected[5] = (byte)0xFF;
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[6] = (byte)0xEF;
                    byArr3Expected[7] = (byte)0x7F;
                }
                else
                {
                    byArr3Expected[6] = (byte)0xFF;
                    byArr3Expected[7] = (byte)0xFF;
                }
                byArr2 = null;
                byArr2 = BitConverter.GetBytes( do9z );
                ++iCountTestcases;
                if ( byArr2.Length != inNumByteArrElems )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error E_211ci!  byArr2.length==" + byArr2.Length  );
                }
                ++iCountTestcases;
                for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
                {
                    if ( byArr2[aa] != byArr3Expected[aa] )
                    {
                        ++iCountErrors;
                        Console.Error.WriteLine(  "POINTTOBREAK: Error E_212aa!  aa==" + aa + " ,BitConverter.ToString(byArr2)==" + BitConverter.ToString( byArr2)  );
                        break;
                    }
                }
                strLoc="Loc_220fi.   Double.NegativeInfinity";
                do9z = Double.NegativeInfinity;
                byArr3Expected = new byte[inNumByteArrElems];
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[0] = (byte)0x00;
                    byArr3Expected[1] = (byte)0x00;
                }
                else
                {
                    byArr3Expected[0] = (byte)0xFF;
                    byArr3Expected[1] = (byte)0xF0;
                }
                byArr3Expected[2] = (byte)0x00;
                byArr3Expected[3] = (byte)0x00;
                byArr3Expected[4] = (byte)0x00;
                byArr3Expected[5] = (byte)0x00;
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[6] = (byte)0xF0;
                    byArr3Expected[7] = (byte)0xFF;
                }
                else
                {
                    byArr3Expected[6] = (byte)0x00;
                    byArr3Expected[7] = (byte)0x00;
                }
                byArr2 = null;
                byArr2 = BitConverter.GetBytes( do9z );
                ++iCountTestcases;
                if ( byArr2.Length != inNumByteArrElems )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error E_221ci!  byArr2.length==" + byArr2.Length  );
                }
                ++iCountTestcases;
                for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
                {
                    if ( byArr2[aa] != byArr3Expected[aa] )
                    {
                        ++iCountErrors;
                        Console.Error.WriteLine(  "POINTTOBREAK: Error E_222aa!  aa==" + aa + " ,BitConverter.ToString(byArr2)==" + BitConverter.ToString( byArr2)  );
                        break;
                    }
                }
                strLoc="Loc_230fi.   Double.PositiveInfinity";
                do9z = Double.PositiveInfinity;
                byArr3Expected = new byte[inNumByteArrElems];
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[0] = (byte)0x00;
                    byArr3Expected[1] = (byte)0x00;
                }
                else
                {
                    byArr3Expected[0] = (byte)0x7F;
                    byArr3Expected[1] = (byte)0xF0;
                }
                byArr3Expected[2] = (byte)0x00;
                byArr3Expected[3] = (byte)0x00;
                byArr3Expected[4] = (byte)0x00;
                byArr3Expected[5] = (byte)0x00;
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[6] = (byte)0xF0;
                    byArr3Expected[7] = (byte)0x7F;
                }
                else
                {
                    byArr3Expected[6] = (byte)0x00;
                    byArr3Expected[7] = (byte)0x00;
                }
                byArr2 = null;
                byArr2 = BitConverter.GetBytes( do9z );
                ++iCountTestcases;
                if ( byArr2.Length != inNumByteArrElems )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error E_231ci!  byArr2.length==" + byArr2.Length  );
                }
                ++iCountTestcases;
                for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
                {
                    if ( byArr2[aa] != byArr3Expected[aa] )
                    {
                        ++iCountErrors;
                        Console.Error.WriteLine(  "POINTTOBREAK: Error E_232aa!  aa==" + aa + " ,BitConverter.ToString(byArr2)==" + BitConverter.ToString( byArr2)  );
                        break;
                    }
                }
                strLoc="Loc_240fi.   Double.NaN";
                do9z = Double.NaN;
                byArr3Expected = new byte[inNumByteArrElems];
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[0] = (byte)0x00;
                    byArr3Expected[1] = (byte)0x00;
                }
                else
                {
                    byArr3Expected[0] = (byte)0xff;
                    byArr3Expected[1] = (byte)0xf8;
                }
                byArr3Expected[2] = (byte)0x00;
                byArr3Expected[3] = (byte)0x00;
                byArr3Expected[4] = (byte)0x00;
                byArr3Expected[5] = (byte)0x00;
                if (BitConverter.IsLittleEndian)
                {
                    byArr3Expected[6] = (byte)0xf8;
                    byArr3Expected[7] = (byte)0xff;
                }
                else
                {
                    byArr3Expected[6] = (byte)0x00;
                    byArr3Expected[7] = (byte)0x00;
                }
                byArr2 = null;
                byArr2 = BitConverter.GetBytes( do9z );
                ++iCountTestcases;
                if ( byArr2.Length != inNumByteArrElems )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error E_241ci!  byArr2.length==" + byArr2.Length  );
                }
                ++iCountTestcases;
                for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
                {
                    if ( byArr2[aa] != byArr3Expected[aa] )
                    {
                        ++iCountErrors;
                        Console.Error.WriteLine(  "POINTTOBREAK: Error E_242aa!  aa==" + aa + " ,BitConverter.ToString(byArr2)==" + BitConverter.ToString( byArr2)  );
                        break;
                    }
                }
                do9z = Double.NaN;
                do8y = Double.NaN;
                ++iCountTestcases;
                if ( (bool)(do8y==do9z) )  
                {
                    Console.Error.WriteLine(  "POINTTOBREAK: (Err_936fa) Error!  'if' says they Are equal (that implies 'True').  Re ."  );
                    Console.Error.WriteLine(  "EXTENDEDINFO: (Err_936fb) We know 'NaN==NaN' resolves to 'False'; so good if this prints 'True'??: "+ (bool)(do8y==do9z)  );
                }
                else
                {
                    Console.Error.WriteLine( "(Inf_927pa) Good.  'if' says they are Not equal (that implies 'False').  Re ." );
                    Console.Error.WriteLine( "(Inf_927pb) Good if this prints 'False': "+ (bool)(do8y==do9z) );
                }
                ++iCountTestcases;
                if ( (bool)(do8y!=do9z) )
                {
                    Console.Error.WriteLine( "(Inf_847pa) Good.  'if' says they are Not equal (that implies 'True')." );
                    Console.Error.WriteLine( "(Inf_847pb) Good if this prints 'True': "+ (bool)(do8y!=do9z) );
                }
                else
                {
                    Console.Error.WriteLine(  "POINTTOBREAK: (Err_856fa) Error!  'if' says they Are equal (that implies 'False')."  );
                    Console.Error.WriteLine(  "EXTENDEDINFO: (Err_856fb) We know 'NaN==NaN' resolves to 'False'; so good if this prints 'False'??: "+ (bool)(do8y!=do9z)  );
                }
                ++iCountTestcases;
                if ( (bool)(Double.NaN==Double.NaN) )
                {
                    Console.Error.WriteLine(  "POINTTOBREAK: (Err_376fa) Error!  'if' says they Are equal (that implies 'True').  Re ."  );
                    Console.Error.WriteLine(  "EXTENDEDINFO: (Err_376fb) We know 'NaN==NaN' resolves to 'False'; so good if this prints 'True'??: "+ (bool)(Double.NaN==Double.NaN)  );
                }
                else
                {
                    Console.Error.WriteLine( "(Inf_377pa) Good.  'if' says they are Not equal (that implies 'False').  Re ." );
                    Console.Error.WriteLine( "(Inf_377pb) Good if this prints 'False': "+ (bool)(Double.NaN==Double.NaN) );
                }
                ++iCountTestcases;
                if ( (bool)(Double.NaN!=Double.NaN) )
                {
                    Console.Error.WriteLine( "(Inf_287pa) Good.  'if' says they are Not equal (that implies 'True')." );
                    Console.Error.WriteLine( "(Inf_287pb) Good if this prints 'True': "+ (bool)(Double.NaN!=Double.NaN) );
                }
                else
                {
                    Console.Error.WriteLine(  "POINTTOBREAK: (Err_286fa) Error!  'if' says they Are equal (that implies 'False')."  );
                    Console.Error.WriteLine(  "EXTENDEDINFO: (Err_286fb) We know 'NaN==NaN' resolves to 'False'; so good if this prints 'False'??: "+ (bool)(Double.NaN!=Double.NaN)  );
                }
                do8y = BitConverter.ToDouble( byArr3Expected ,0 );
                ++iCountTestcases;
                if ( Double.NaN == do9z )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error E_660ga!  Double.NaN=="+ Double.NaN +" ,do9z=="+ do9z  );
                    Console.Error.WriteLine(  "EXTENDEDINFO: (E_660ga) Contradiction!  (boolean)(Double.NaN==do9z) == "+ (bool)(Double.NaN==do9z)  );
                }
                ++iCountTestcases;
                if ( do8y == do9z )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error E_661gb!  do8y=="+ do8y  +" ,do9z=="+ do9z  );
                }
                ++iCountTestcases;
                if (
                    0.0 == Double.NaN
                    ||  1234567.8 == do9z
                    ||  Double.NaN == do9z
                    )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error E_241no! Double.NaN=="+ Double.NaN +" ,do9z=="+ do9z  );
                }
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error E_3435bun! (Co1245GetBytes_double) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (E_3435bun) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "Pass. Co1245GetBytes_double   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "Fail!   Co1245GetBytes_double.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co1245GetBytes_double cbA = new Co1245GetBytes_double();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error E_999zzz! (Co1245GetBytes_double) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
        {
            Console.Error.WriteLine(  "PATHTOSOURCE:  Co1245GetBytes_double   Fail!"  );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}

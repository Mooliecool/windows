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
using System;
using System.IO;
using System.Text ;
public class FileStream_Unlock
{
    public bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_0000";
        String strValue = String.Empty;
        int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
        int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue/2, Int32.MaxValue/10, 10000, 100000 , Int32.MaxValue/20, Int32.MaxValue/100 , Int32.MaxValue/1000 };
        Random rand = new Random((int)DateTime.Now.Ticks);
        Byte[] bArr = new Byte[200];
        for(int i = 0 ; i < bArr.Length ; i++)
            bArr[i] = (Byte)65;
        Byte[] bNewArr = new Byte[20];
        for(int i = 0 ; i < 20 ; i++)
            bNewArr[i] = (Byte)66;
        try
        {
            string strFileName = "testblah.cs";
            FileStream fs = new FileStream( strFileName, FileMode.Create, FileAccess.ReadWrite, FileShare.ReadWrite); 
            FileStream fs2 = new FileStream( strFileName, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite); 
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ )
            {
                try
                {
                    fs.Unlock( iArrInvalidValues[iLoop] , Int64.MaxValue );
                    iCountErrors++;
                } 
                catch ( ArgumentException )
                {
                } 
                catch ( Exception ex )
                {
                    Console.Error.WriteLine( "Error_1000!!!! Unexpected exception " + ex.ToString() );
                    iCountErrors++ ;
                }
            }
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ )
            {
                try
                {
                    fs.Unlock( Int64.MaxValue, iArrInvalidValues[iLoop]);
                    iCountErrors++;
                } 
                catch ( ArgumentException )
                {
                } 
                catch ( Exception ex )
                {
                    Console.Error.WriteLine( "Error_1100!!!! Unexpected exception " + ex.ToString() );
                    iCountErrors++ ;
                }
            }
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ )
            {
                try
                {
                    long lLength = (long)rand.Next() ;
                    fs.Lock( iArrLargeValues[iLoop] , lLength );
                    fs.Unlock( iArrLargeValues[iLoop] , lLength );
                } 
                catch ( Exception ex )
                {
                    Console.Error.WriteLine( "Error_1200!!!! Unexpected exception 2" + ex.ToString() );
                    iCountErrors++ ;
                }
            }
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ )
            {
                try
                {
                    long lLength = (long)rand.Next() ;
                    fs.Lock( lLength, iArrLargeValues[iLoop]);
                    fs.Unlock( lLength, iArrLargeValues[iLoop] );
                } 
                catch ( Exception ex )
                {
                    Console.Error.WriteLine( "Err_1300!!! Unexpected exception " + ex.ToString() );
                    iCountErrors++ ;
                }
            }
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ )
            {
                try
                {
                    long lLength = (long)rand.Next() ;
                    fs.Unlock( lLength, iArrLargeValues[iLoop] );
                    iCountErrors++;
                } 
                catch ( IOException)
                {
                } 
                catch ( Exception ex )
                {
                    Console.Error.WriteLine( "Err_1325!!! Unexpected exception " + ex.ToString() );
                    iCountErrors++ ;
                }
            }
            iCountTestcases++;
            try
            {
                fs.Write(bArr , 0 , bArr.Length );
                fs.Lock( 0, 200);
                fs.Seek(0, SeekOrigin.Begin);
                try
                {
                    fs2.Write(bNewArr , 0 , bNewArr.Length );
                    fs2.Flush();
                    iCountErrors++ ;
                } 
                catch( IOException){} 
                fs.Unlock( 0, 200 ); 
                fs2.Write(bNewArr, 0 , bNewArr.Length );
                fs2.Flush();
            } 
            catch ( Exception ex )
            {
                Console.Error.WriteLine( "Error_1400!!!! Unexpected exception " + ex.ToString() );
                iCountErrors++ ;
            }
            iCountTestcases++;
            try
            {
                fs.Read(bArr , 0 , bArr.Length );
                fs.Lock( 0, 200);
                fs.Seek(0, SeekOrigin.Begin);
                try
                {
                    fs2.Read(bNewArr , 0 , bNewArr.Length );
                    iCountErrors++ ;
                } 
                catch( IOException){} 
                fs.Unlock( 0, 200 ); 
                fs2.Read(bNewArr, 0 , bNewArr.Length );
            } 
            catch ( Exception ex )
            {
                Console.Error.WriteLine( "Error_1420!!!! Unexpected exception " + ex.ToString() );
                iCountErrors++ ;
            }
            iCountTestcases++;
            try
            {
                fs.Write(bArr , 0 , bArr.Length );
                fs.Lock( 0, 100);
                fs.Unlock( 0, 101);
                iCountErrors++ ; 
            } 
            catch ( IOException )
            {
            } 
            catch ( Exception ex )
            {
                Console.Error.WriteLine( "Error_1475!!!! Unexpected exception " + ex.ToString() );
                iCountErrors++ ;
            }
            fs.Close();
            iCountTestcases++ ;
            try
            {
                fs.Lock( 10, 20 );
            }
            catch ( ObjectDisposedException )
            {
            }
            catch ( Exception e)
            {
                iCountErrors++ ;
                Console.Error.WriteLine( "Error_2000!!!! Unexpected exception " + e.ToString() );
            }
            fs2.Close();
            if( File.Exists( strFileName ) )
                File.Delete( strFileName );
        } 
        catch (Exception e ) 
        {
            ++iCountErrors;
            Console.WriteLine ("Error_9998!!!! Unexpected exception strLoc=="+ strLoc +", msg:"+e.ToString());
        }
        if ( iCountErrors == 0 ) return true; else return false ;
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        FileStream_Unlock cbA = new FileStream_Unlock();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception e)
        {
            bResult = false;
            Console.WriteLine("Error_9999! caught Exception in main(), msg:"+e.ToString());
        }
        if (bResult) Console.WriteLine("Test PASSED"); else Console.WriteLine("Test FAILED");
    }
}

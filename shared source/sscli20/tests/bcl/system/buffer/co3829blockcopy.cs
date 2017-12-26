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
public class Co3829BlockCopy
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Buffer.BlockCopy(Object src, int srcOffset, Object dst, int dstOffset, int count)";
    public static String s_strTFName        = "Co3829BlockCopy.cs";
    public static String s_strTFAbbrev      = "Co3795";
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        Int32[] arrIntFirst = null;
        Int32[] arrIntSec = null;
        Int16[] arrDifferentTypes = null;
        Object[] arrObjects = null;
        String[] arrStrings = null;
        Random random = new Random();
        try
        {
            strLoc = "Loc_386fsd";
            Console.WriteLine();
            arrIntFirst = new Int32[10];
            for(int i=0; i<arrIntFirst.Length; i++)
            {
                arrIntFirst[i] = i+10;
            }
            arrIntSec = new Int32[arrIntFirst.Length];
            Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, 0, Buffer.ByteLength(arrIntFirst));
            for(int i=0; i<arrIntFirst.Length; i++)
            {
                iCountTestcases++;
                if(arrIntSec[i]!=arrIntFirst[i])
                {
                    iCountErrors++;
                    Console.WriteLine("Err_357cd_" + i.ToString() + " Array value not correct=" + arrIntSec[i].ToString());
                }
            }
            arrIntSec = new Int32[arrIntFirst.Length - 2];
            Buffer.BlockCopy(arrIntFirst, 2*4, arrIntSec, 0, Buffer.ByteLength(arrIntFirst) - 2*4);
            for(int i=0; i<arrIntFirst.Length-2; i++)
            {
                iCountTestcases++;
                if(arrIntSec[i]!=arrIntFirst[i+2])
                {
                    iCountErrors++;
                    Console.WriteLine("Err_457vdf_" + i.ToString() + " Array value not correct=" + arrIntSec[i].ToString());
                }
            }
            arrIntSec = new Int32[arrIntFirst.Length];
            Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, 2*4, Buffer.ByteLength(arrIntFirst) - 2*4);
            for(int i=0; i<arrIntFirst.Length-2; i++)
            {
                iCountTestcases++;
                if(arrIntSec[i+2]!=arrIntFirst[i])
                {
                    iCountErrors++;
                    Console.WriteLine("Err_02346vsd_" + i.ToString() + " Array value not correct=" + arrIntSec[i+2].ToString());
                }
            }
            arrIntSec = new Int32[arrIntFirst.Length];
            for(int i=0; i<20; i++)
            {
                try
                {
                    iCountTestcases++;
                    Buffer.BlockCopy(arrIntFirst, random.Next(Int32.MinValue, 0), arrIntSec, 0, Buffer.ByteLength(arrIntFirst));
                    iCountErrors++;
                    Console.WriteLine("Err_2476sd! Exception not thrown");
                }
                catch(ArgumentException){}
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_357cd! unexpected exception thrown," + ex.ToString());
                }
            }
            try
            {
                iCountTestcases++;
                Buffer.BlockCopy(arrIntFirst, Int32.MinValue, arrIntSec, 0, Buffer.ByteLength(arrIntFirst));
                iCountErrors++;
                Console.WriteLine("Err_2476sd! Exception not thrown");
            }
            catch(ArgumentException){}
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_357cd! unexpected exception thrown," + ex.ToString());
            }
            try
            {
                iCountTestcases++;
                Buffer.BlockCopy(arrIntFirst, Int32.MaxValue, arrIntSec, 0, Buffer.ByteLength(arrIntFirst));
                iCountErrors++;
                Console.WriteLine("Err_2476sd! Exception not thrown");
            }
            catch(ArgumentException){}
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_357cd! unexpected exception thrown," + ex.ToString());
            }
            arrIntSec = new Int32[arrIntFirst.Length];
            for(int i=0; i<20; i++)
            {
                try
                {
                    iCountTestcases++;
                    Buffer.BlockCopy(arrIntFirst, random.Next(Buffer.ByteLength(arrIntFirst) + 1,Int32.MaxValue), arrIntSec, 0, Buffer.ByteLength(arrIntFirst));
                    iCountErrors++;
                    Console.WriteLine("Err_2665sd! Exception not thrown");
                }
                catch(ArgumentException){}
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_205th! unexpected exception thrown," + ex.ToString());
                }
            }
            arrIntSec = new Int32[arrIntFirst.Length];
            for(int i=0; i<20; i++)
            {
                try
                {
                    iCountTestcases++;
                    Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, random.Next(Int32.MinValue, 0), Buffer.ByteLength(arrIntFirst));
                    iCountErrors++;
                    Console.WriteLine("Err_036vs! Exception not thrown");
                }
                catch(ArgumentException){}
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_02165vsd! unexpected exception thrown," + ex.ToString());
                }
            }
            try
            {
                iCountTestcases++;
                Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, Int32.MinValue, Buffer.ByteLength(arrIntFirst));
                iCountErrors++;
                Console.WriteLine("Err_036vs! Exception not thrown");
            }
            catch(ArgumentException){}
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_02165vsd! unexpected exception thrown," + ex.ToString());
            }
            try
            {
                iCountTestcases++;
                Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, Int32.MaxValue, Buffer.ByteLength(arrIntFirst));
                iCountErrors++;
                Console.WriteLine("Err_036vs! Exception not thrown");
            }
            catch(ArgumentException){}
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_02165vsd! unexpected exception thrown," + ex.ToString());
            }
            arrIntSec = new Int32[arrIntFirst.Length];
            for(int i=0; i<20; i++)
            {
                try
                {
                    iCountTestcases++;
                    Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, random.Next(Buffer.ByteLength(arrIntFirst) + 1,Int32.MaxValue), Buffer.ByteLength(arrIntFirst));
                    iCountErrors++;
                    Console.WriteLine("Err_04236cs! Exception not thrown");
                }
                catch(ArgumentException){}
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0246vfd! unexpected exception thrown," + ex.ToString());
                }
            }
            arrIntSec = new Int32[arrIntFirst.Length];
            for(int i=0; i<20; i++)
            {
                try
                {
                    iCountTestcases++;
                    Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, 0, random.Next(Int32.MinValue, 0));
                    iCountErrors++;
                    Console.WriteLine("Err_056vds! Exception not thrown");
                }
                catch(ArgumentException){}
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0356sd! unexpected exception thrown," + ex.ToString());
                }
            }
            try
            {
                iCountTestcases++;
                Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, 0, Int32.MinValue);
                iCountErrors++;
                Console.WriteLine("Err_056vds! Exception not thrown");
            }
            catch(ArgumentException){}
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_0356sd! unexpected exception thrown," + ex.ToString());
            }
            try
            {
                iCountTestcases++;
                Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, 0, Int32.MaxValue);
                iCountErrors++;
                Console.WriteLine("Err_056vds! Exception not thrown");
            }
            catch(ArgumentException){}
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_0356sd! unexpected exception thrown," + ex.ToString());
            }
            arrIntSec = new Int32[arrIntFirst.Length];
            for(int i=0; i<20; i++)
            {
                try
                {
                    iCountTestcases++;
                    Buffer.BlockCopy(arrIntFirst, 0, arrIntSec, 0, random.Next(Buffer.ByteLength(arrIntFirst)+1, Int32.MaxValue));
                    iCountErrors++;
                    Console.WriteLine("Err_056vds! Exception not thrown");
                }
                catch(ArgumentException){}
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0356sd! unexpected exception thrown," + ex.ToString());
                }
            }
            strLoc = "Loc_0357vsd";
            arrStrings = new String[3];
            arrStrings[0] = "Due to excess alcohol levels in the blood stream,";
            arrStrings[1] = "this sentense would become a big blurrr";
            arrStrings[2] = "befoere our bery ueyrs";
            arrObjects = new Object[3];
            try
            {
                Buffer.BlockCopy(arrStrings, 0, arrObjects, 0, 4*3);	
                iCountErrors++;
                Console.WriteLine("Err_0457gfd! Exception not thrown");
            }
            catch(ArgumentException){}
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_056csad! unexpected exception thrown," + ex.ToString());
            }
            arrIntFirst = new Int32[10];
            for(int i=0; i<arrIntFirst.Length; i++)
            {
                arrIntFirst[i] = i+10;
            }
            arrDifferentTypes = new Int16[arrIntFirst.Length*2];
            Buffer.BlockCopy(arrIntFirst, 0, arrDifferentTypes, 0, Buffer.ByteLength(arrIntFirst));
#if BIGENDIAN
            int j = 1;
#else
            int j = 0;
#endif
            for(int i=0; i<arrIntFirst.Length; i++)
            {
                iCountTestcases++;
                if(arrDifferentTypes[j]!=arrIntFirst[i])
                {
                    iCountErrors++;
                    Console.WriteLine("Err_357cd_" + i.ToString() + " Array value not correct=" + arrDifferentTypes[j].ToString());
                }
                j = j+2;
            }
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
            return true;
        }
        else
        {
            Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co3829BlockCopy cbA = new Co3829BlockCopy();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}

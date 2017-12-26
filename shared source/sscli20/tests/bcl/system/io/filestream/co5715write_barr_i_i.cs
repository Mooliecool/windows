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
using System.Collections;
using System.Globalization;
using System.Text;
using System.Threading;
public class Co5715Write_bArr_i_i
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "FileStream.Write(Byte[], Int, Int";
    public static String s_strTFName        = "co5715write_bArr_i_i.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
        int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue/2, Int32.MaxValue/10, 10000, 100000 , Int32.MaxValue/20, Int32.MaxValue/100 , Int32.MaxValue/1000 };
        int[] iArrValidValues = new Int32[]{ Int16.MaxValue/2 , Int16.MaxValue, Int32.MaxValue/1000 , Int32.MaxValue/10000};
        try
        {
            FileStream fs2;
            Int32 count;
            Byte[] bWriteArr, bReadArr;
            String filName = s_strTFAbbrev + "TestFile.tmp";
            if(File.Exists("Co5715Test.tmp"))
                File.Delete("Co5715Test.tmp");
            strLoc = "Loc_984yv";
            fs2 = new FileStream(filName, FileMode.Create);
            iCountTestcases++;
            try 
            {
                fs2.Write(null, 0, 0);
                iCountErrors++;
                printerr( "Error_298yv! Expected exception not thrown");
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo("Info_g98b7! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_t8y78! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fs2.Close();
            strLoc = "Loc_9875g";
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++)
            {
                fs2 = new FileStream(filName, FileMode.Create);
                try 
                {
                    fs2.Write(new Byte[]{1}, iArrInvalidValues[iLoop], 0);
                    iCountErrors++;
                    printerr("Error_988bb! Expected exception not thrown");
                } 
                catch (ArgumentOutOfRangeException aexc) 
                {
                    printinfo("Info_98yby! Caught expected exception, exc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_099hy! Incorrect exception thrown, exc=="+exc.ToString());
                }
                fs2.Close();
            }
            strLoc = "Loc_5g8ys";
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++)
            {
                fs2 = new FileStream(filName, FileMode.Create);
                try 
                {
                    fs2.Write(new Byte[]{1}, 0, iArrInvalidValues[iLoop]);
                    iCountErrors++;
                    printerr( "Error_9t8yj! Expected exception not thrown");
                } 
                catch (ArgumentOutOfRangeException aexc) 
                {
                    printinfo("Info_9488b! Caught expected exception, exc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_7687b! Incorrect exception thrown, exc=="+exc.ToString());
                }
                fs2.Close();
            }
            strLoc = "Loc_1228x";       
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++)
            {
                fs2 = new FileStream(filName, FileMode.Create);
                try 
                {
                    fs2.Write(new Byte[]{1}, iArrLargeValues[iLoop], 0);
                    iCountErrors++;
                    printerr( "Error_3444j! Expected exception not thrown");
                } 
                catch (ArgumentException aexc) 
                {
                    printinfo("Info_g8777! CAught expected exception, exc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_t77gg! Incorrect exception thrown, exc=="+exc.ToString());
                }
                fs2.Close();
            }
            strLoc = "Loc_897yg";
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++)
            {
                fs2 = new FileStream(filName, FileMode.Create);
                try 
                {
                    fs2.Write(new Byte[]{1}, 0, iArrLargeValues[iLoop]);
                    iCountErrors++;
                    printerr( "Error_98y8x! Expected exception not thrown");
                } 
                catch (ArgumentException aexc) 
                {
                    printinfo("Info_g58yb! Caught expected exception, exc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_ytg78! Incorrect exception thrown, exc=="+exc.ToString());
                }			
                fs2.Close();
            }
            strLoc = "Loc_8g7yb";
            for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++)
            {
                count = 0;
                iCountTestcases++;
                fs2 = new FileStream(filName, FileMode.Create);
                bWriteArr = new Byte[iArrValidValues[iLoop]];
                for(int i = 0 ; i < iArrValidValues[iLoop] ; i++) 
                    bWriteArr[i] = (Byte)i;
                fs2.Write(bWriteArr, 0, iArrValidValues[iLoop]);
                if(fs2.Length != iArrValidValues[iLoop]) 
                {
                    iCountErrors++;
                    printerr( "Error_498yv! Incorrect count=="+fs2.Length);
                }
                fs2.Flush();
                fs2.Position = 0;
                bReadArr = new Byte[iArrValidValues[iLoop]];
                count = fs2.Read(bReadArr, 0, iArrValidValues[iLoop]);
                if(count != iArrValidValues[iLoop]) 
                {
                    iCountErrors++;
                    printerr( "Error_958yv! Incorrect count=="+count);
                }
                for(int i = 0 ; i < bReadArr.Length ; i++) 
                {
                    if(bReadArr[i] != bWriteArr[i]) 
                    {
                        iCountErrors++;
                        printerr( "Error_98gy8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
                    }
                }			
                fs2.Close();
            }
            strLoc = "Loc_4324";
            count = 0;
            fs2 = new FileStream(filName, FileMode.Create);
            bWriteArr = new Byte[100];
            for(int i = 0 ; i < 100 ; i++) 
                bWriteArr[i] = (Byte)i;
            fs2.Write(bWriteArr, 0, 100);
            iCountTestcases++;
            if(fs2.Length != 100) 
            {
                iCountErrors++;
                printerr( "Error_498yv! Incorrect count=="+count);
            }
            fs2.Flush();
            fs2.Position = 0;
            bReadArr = new Byte[100];
            count = fs2.Read(bReadArr, 0, 100);
            iCountTestcases++;
            if(count != 100) 
            {
                iCountErrors++;
                printerr( "Error_958yv! Incorrect count=="+count);
            }
            for(int i = 0 ; i < bReadArr.Length ; i++) 
            {
                iCountTestcases++;
                if(bReadArr[i] != bWriteArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_98gy8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
                }
            }			
            fs2.Close();
            strLoc = "Loc_r78g8";
            count = 0;
            fs2 = new FileStream(filName, FileMode.Create);
            bWriteArr = new Byte[100];
            for(int i = 0 ; i < 100 ; i++) 
                bWriteArr[i] = (Byte)i;
            fs2.Write(bWriteArr, 10, 12);
            iCountTestcases++;
            if(fs2.Length != 12) 
            {
                iCountErrors++;
                printerr( "Error_98ybk! Incorrect count=="+count);
            } 
            fs2.Flush();
            fs2.Position = 0;
            fs2.Read(bReadArr, 0, 12);
            for(int i = 0 ; i < 12 ; i++) 
            {
                iCountTestcases++;
                if(bReadArr[i] != bWriteArr[i+10]) 
                {
                    iCountErrors++;
                    printerr( "Error_y88gb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
                }
            }
            fs2.Position = 0;
            bReadArr = new Byte[100];
            count = fs2.Read(bReadArr, 10, 12);
            iCountTestcases++;
            if(count != 12) 
            {
                iCountErrors++;
                printerr( "Error_29908! Incorrect count=="+count);
            }
            for(int i = 10 ; i < 22 ; i++) 
            {
                iCountTestcases++;
                if(bReadArr[i] != bWriteArr[i]) 
                {
                    iCountErrors++;
                    printerr("Error_98yg8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
                }
            }
            strLoc = "Loc_98fyf";
            fs2.Write(bWriteArr, 80, 19);
            fs2.Flush();
            fs2.Position = 0;
            fs2.Read(bReadArr, 0, 30);
            for(int i = 0 ; i < 12 ; i++) 
            {
                iCountTestcases++;
                if(bReadArr[i] != bWriteArr[i+10]) 
                {
                    iCountErrors++;
                    printerr( "Error_87vyb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
                }
            }
            for(int i = 12 ; i < 30 ; i++) 
            {
                iCountTestcases++;
                if(bReadArr[i] != bWriteArr[i+68]) 
                {
                    iCountErrors++;
                    printerr( "Error_t8yb8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
                }
            }
            fs2.Position = 12;
            bReadArr = new Byte[100];
            count = fs2.Read(bReadArr, 80, 19);
            iCountTestcases++;
            if(count != 19) 
            {
                iCountErrors++;
                printerr( "Error_f7878! Incorrect count=="+count);
            }
            for(int i = 80; i < 99 ; i++) 
            {
                iCountTestcases++;
                if(bReadArr[i] != bWriteArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_87gyb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
                }
            }
            fs2.Close();
            strLoc = "Loc_8975y";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Close();
            iCountTestcases++;
            try 
            {
                fs2.Write(new Byte[]{1}, 0, 1);
                iCountErrors++;
                printerr( "Error_2228k! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_87y7b! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_1289x! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_23098";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Write(new Byte[0], 0, 0);
            fs2.Flush();
            iCountTestcases++;
            if(fs2.Length != 0) 
            {
                iCountErrors++;
                printerr( "Error_298by! Incorrect count=="+count);
            }
            iCountTestcases++;
            if(fs2.Length != 0) 
            {
                iCountErrors++;
                printerr( "Error_209ub! Incorrec length=="+fs2.Length);
            } 
            iCountTestcases++;
            if(fs2.Position != 0) 
            {
                iCountErrors++;
                printerr( "Error_85y7v! Incorrec position=="+fs2.Position);
            }
            fs2.Close();
            if(File.Exists(filName))
                File.Delete(filName);
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
            return true;
        }
        else
        {
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public void printerr ( String err )
    {
        Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
    }
    public void printinfo ( String info )
    {
        Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5715Write_bArr_i_i cbA = new Co5715Write_bArr_i_i();
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

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
using System.Threading;
    using System;
    using System.Resources;
    using System.IO;
    using System.Reflection;
using System.Runtime.Serialization;
    public class Co5265HasId_obj
    {
       public static readonly String s_strActiveBugNums = "";
       public static readonly String s_strDtTmVer       = "";
       public static readonly String s_strClassMethod   = "ObjectIDGenerator.HasId(Object)";
       public static readonly String s_strTFName        = "Co5265HasId_obj.cs";
       public static readonly String s_strTFAbbrev      = "Co5265";
       public static readonly String s_strTFPath        = Environment.CurrentDirectory;
       public virtual bool runTest()
       {
          Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
          int iCountErrors = 0;
          int iCountTestcases = 0;
          String strLoc = "Loc_000oo";
          try {
          do
          {
            ObjectIDGenerator objIDGen;
            Object obj;
            long objId1, objId2;
            bool firstTime;
            Object boo2, cur2, dt2, dec2, dbl2, gui2, i16, i32, i64, sgl2, ts2;
            strLoc = "Loc_100aa";
            objIDGen = new ObjectIDGenerator();
            obj = null;
            iCountTestcases++;
            try
            {
              objIDGen.HasId(null, out firstTime);
              iCountErrors++;
              printerr("Error_289t2! Expected Exception not thrown");
            } catch (ArgumentException aExc) {}
            catch (Exception exc)
            {
              iCountErrors++;
              printerr("Error_t2gsa! Incorrect Exception thrown : exc=="+exc.ToString());
            }
            strLoc = "Loc_150aa";
            objIDGen = new ObjectIDGenerator();
            obj = new Decimal(10);
            iCountTestcases++;
            objId1 = objIDGen.HasId(obj, out firstTime);
            iCountTestcases++;
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_209ts");
            }
            objId1 = objIDGen.HasId(obj, out firstTime);
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_209ts");
            }
            if(objId1 != 0)
            {
              iCountErrors++;
              printerr("Error_150bb! Did not return null for non-existent object");
            }
            strLoc = "Loc_200aa";
            objIDGen = new ObjectIDGenerator();
            obj = new Decimal(10);
            objId1 = objIDGen.GetId(obj, out firstTime);
            iCountTestcases++;
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_200bb! Whoa, This object haven't been searched for before.");
            }
            strLoc = "Loc_300aa";
            objIDGen = new ObjectIDGenerator();
            obj = new Decimal(10);
            objId1 = objIDGen.GetId(obj, out firstTime);
            iCountTestcases++;
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_300bb! Whoa, This object haven't been searched for before.");
            }
            objId2 = objIDGen.HasId(obj, out firstTime);
            iCountTestcases++;
            if(objId1 != objId2)
            {
              iCountErrors++;
              printerr("Error_300cc! Different object id's returned for the same object");
            }
            iCountTestcases++;
            if(firstTime)
            {
              iCountErrors++;
              printerr("Error_300ee! This object has already been queried, firsttime should be false");
            }
            strLoc = "Loc_400aa";
            boo2 = false;
            dt2 = DateTime.Now;
            dec2 = new Decimal(15.2);
            dbl2 = ((Double)12.2);
            gui2 = Guid.NewGuid();
            i16 = ((Int16)18);
            i32 = ((Int32)42);
            i64 = ((Int64)11);
            sgl2 = ((Single)(Single)3.4);
            ts2 = new TimeSpan(12, 12, 12);
            objIDGen = new ObjectIDGenerator();
            strLoc = "Loc_32948";
            objId1 = objIDGen.GetId(boo2, out firstTime);
            iCountTestcases++;
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_400bb! FirstTime is incorrectly false");
            }
            objId2 = objIDGen.HasId(boo2, out firstTime);
            iCountTestcases++;
            if(objId1 != objId2)
            {
              iCountErrors++;
              printerr("Error_400cc! Object id's not same for same object");
            }
            iCountTestcases++;
            if(firstTime)
            {
              iCountErrors++;
              printerr("Error_400dd! FirstTime is incorrectly false");
            }
            strLoc = "Loc_600aa";
            objId1 = objIDGen.GetId(dt2, out firstTime);
            iCountTestcases++;
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_600bb! FirstTime is incorrectly false");
            }
            objId2 = objIDGen.HasId(dt2, out firstTime);
            iCountTestcases++;
            if(firstTime)
            {
              iCountErrors++;
              printerr("Error_600cc! FirstTime is incorrectl true");
            }
            iCountTestcases++;
            if(objId2 != objId1)
            {
              iCountErrors++;
              printerr("Error_600dd! Object Id's different for same object");
            }
            strLoc = "Loc_700aa";
            objId1 = objIDGen.GetId(dec2, out firstTime);
            iCountTestcases++;
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_700bb! FirstTime is incorrectly false");
            }
            objId2 = objIDGen.HasId(dec2, out firstTime);
            iCountTestcases++;
            if(firstTime)
            {
              iCountErrors++;
              printerr("Error_700cc! FirstTime is incorrecly true");
            }
            iCountTestcases++;
            if(objId2 != objId1)
            {
              iCountErrors++;
              printerr("Error_700dd! Object id's different for same object");
            }
            strLoc = "Loc_800aa";
            objId1 = objIDGen.GetId(dbl2, out firstTime);
            iCountTestcases++;
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_800bb! FirstTime is incorrectly false");
            }
            objId2 = objIDGen.HasId(dbl2, out firstTime);
            iCountTestcases++;
            if(firstTime)
            {
              iCountErrors++;
              printerr("Error_800cc! FirstTime is incorrecly true");
            }
            iCountTestcases++;
            if(objId2 != objId1)
            {
              iCountErrors++;
              printerr("Error_800dd! Object id's different for same object");
            }
            strLoc = "Loc_900aa";
            objId1 = objIDGen.GetId(gui2, out firstTime);
            iCountTestcases++;
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_800bb! FirstTime is incorrectly false");
            }
            objId2 = objIDGen.HasId(gui2, out firstTime);
            iCountTestcases++;
            if(firstTime)
            {
              iCountErrors++;
              printerr("Error_800cc! FirstTime is incorrecly true");
            }
            iCountTestcases++;
            if(objId2 != objId1)
            {
              iCountErrors++;
              printerr("Error_800dd! Object id's different for same object");
            }
            strLoc = "Loc_1000a";
            objId1 = objIDGen.GetId(i16, out firstTime);
            iCountTestcases++;
            if(!firstTime)
            {
              iCountErrors++;
              printerr("Error_1000b! FirstTime is incorrectly false");
            }
            objId2 = objIDGen.HasId(i16, out firstTime);
            iCountTestcases++;
            if(firstTime)
            {
              iCountErrors++;
              printerr("Error_1000c! FirstTime is incorrecly true");
            }
            iCountTestcases++;
            if(objId2 != objId1)
            {
              iCountErrors++;
              printerr("Error_1000d! Object id's different for same object");
            }
            strLoc = "Loc_2805fe";
            objIDGen = new ObjectIDGenerator();
            dec2 = new Decimal(10);
            dbl2 = ((Double)10);
            objId1 = objIDGen.GetId(dec2, out firstTime);
            objId2 = objIDGen.HasId(dbl2, out firstTime);
            iCountTestcases++;
            if(objId1 == objId2)
            {
              iCountErrors++;
              printerr("Error_98532! Id's of two different objects are equal");
            }
          } while (false);
          } catch (Exception exc_general ) {
             ++iCountErrors;
             Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.StackTrace);
          }
          if ( iCountErrors == 0 )
          {
             Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
             return true;
          }
          else
          {
             Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
             return false;
          }
       }
       public virtual void printerr ( String err )
       {
          Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
       }
       public virtual void printinfo ( String info )
       {
          Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
       }
       public static void Main(String[] args)
       {
          bool bResult = false;
          Co5265HasId_obj cbA = new Co5265HasId_obj();
          try {
             bResult = cbA.runTest();
          } catch (Exception exc_main){
             bResult = false;
             Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
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

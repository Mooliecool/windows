//------------------------------------------------------------------------------
// <copyright file="crlf.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using System.IO;
using System.Diagnostics;
using System.Collections;

namespace CRLFAnalyzerNS
{
/// <summary>
/// Class for crlf utility.
/// </summary>
  class CRLFAnalyzer
  {
    public static string nl = System.Environment.NewLine;

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    public static int Main(string[] args)
    {
      string OutputFormat = " {0,-14}     0x{1,-14:x}";

      //IO variables.
      string InFilename = null;
      FileStream InFileStream = null;
      BinaryReader InputReader = null;
      string OutFilename = null;
      BinaryWriter OutputWriter = null;

      //Used to pass line termination chars to FixOrConvert.
      byte [] crlfArr = new byte[] {0x0d, 0x0a};
      byte [] lfArr = new byte[] {0x0a};

      //Get file expression from argument.
      try
      {
        DirectoryInfo curdir = new DirectoryInfo(System.Environment.CurrentDirectory);
        string FileExpression = null;
        //Usage
        if (args.Length < 1) 
        {
          Console.WriteLine(nl + "Usage: crlf <filename | expression>" + nl);
          Console.WriteLine("Requires filename or wildcard expression." + nl);
          Console.WriteLine("Note: <filename> can NOT contain path!" + nl);
          Console.WriteLine("Examples: crlf *.txt (will work)" + nl);
          Console.WriteLine("          crlf c:\\mydir\\myfile.txt (will not work)" + nl);
        }//end if
        else
        {
          FileExpression = args[0];
          FileInfo[] files = curdir.GetFiles(FileExpression);

          if (files.Length < 1)
          {
            Console.WriteLine("File does not exist or invalid file expression.");
            return 1;
          }//end if
					
            //Loop to cover all files in expression list
          foreach (FileInfo fi in files)
          {
            int c1 = 0;
            int c2 = 0;

            TermListType TermList = new TermListType();

            string Reply = null;
            bool NeedsFix = false;
            bool AllLF = false;
            bool AllCRLF = false;
            bool CRLF = false;
            bool LFOnly = false;
            bool CROnlyFix = false;
            CRFixType CRFix = CRFixType.Delete;

            FileStream OutputFileStream = null;
            string Prompt = null;

            Console.WriteLine("Processing file: {0}", fi.Name);
            //Check again that file exists.  Probably redundant but doesn't hurt.
            if (!File.Exists(fi.FullName))
            {
              Console.WriteLine("{0} does not exist, skipping file.");
              continue;
            }//end if
            try
            {
              InFilename = fi.FullName;
              InFileStream = new FileStream(InFilename, FileMode.Open);
              InputReader = new BinaryReader(InFileStream, System.Text.Encoding.ASCII);
            }//end try
            catch (Exception e)
            {
              bool Exit= OutputException("Problem opening file: " + InFilename, e);
              if (Exit)
                return 1;
              else //Go to next file in list.
                continue;
            }//end catch

            InputReader.BaseStream.Seek(0, SeekOrigin.Begin);
            InputReader.BaseStream.Position = 0;

            if (InputReader.BaseStream.Length > 2)
            {
              c1 = InputReader.Read();
              c2 = InputReader.Read();
            } //end if
            else
            {
              Console.WriteLine("File too small");
              //Go to next file in list.
              continue;
            } //end else

            if (c1 == 0x0a)
            {
              TermListItem tli = new TermListItem((long) 1, TermTypeEnum.LFOnly);
              TermList.Add(tli);
            } //end if

            long Count = 2;
						
            //Loop through file
            while (c2 > -1)
            {
              if (c1 == 0x0d && c2 != 0x0a)
              {
                TermListItem CRtli = new TermListItem((long) Count - 1, TermTypeEnum.CROnly);
                TermList.Add(CRtli);

              }//end if
              if (c1 != 0x0d && c2 == 0x0a)
              {
                TermListItem LFtli = new TermListItem((long) Count, TermTypeEnum.LFOnly);
                TermList.Add(LFtli);
              }//end if

              if (c1 == 0x0d && c2 == 0x0a)
              {
                TermListItem CRLFtli = new TermListItem((long) Count, TermTypeEnum.CRLF);
                TermList.Add(CRLFtli);
              }//end if

              c1 = c2;
              c2 = InputReader.Read();
              Count++;
            }//end while

            Console.WriteLine(nl + "Number of unmatched CRs: {0, 12}", TermList.CROnlyCount);
            Console.WriteLine("Number of LF-only chars  {0, 12}", TermList.LFOnlyCount);
            Console.WriteLine("Number of CRLF sets:     {0, 12}" + nl, TermList.CRLFCount);
            Console.WriteLine("Size of input file: {0}", InputReader.BaseStream.Length);

            if (TermList.LFOnlyCount > 0 && TermList.CRLFCount == 0)
            {
              Console.WriteLine("File is LF-only terminated" + nl);
              Console.WriteLine("Total number of LF terminated lines: {0}", TermList.LFOnlyCount);
              AllLF = true;
            }//end if
            else
            {
              if (TermList.CRLFCount > 0 && TermList.LFOnlyCount == 0)
              {
                Console.WriteLine("File is CRLF terminated");
                Console.WriteLine("Total number of CRLF terminated lines: {0}", TermList.CRLFCount);
                AllCRLF = true;
              }
              else
              {
                if (TermList.LFOnlyCount > TermList.CRLFCount && TermList.LFOnlyCount > 0)
                {
                  Console.WriteLine("File is mostly LF-only terminated");
                  NeedsFix = true;
                }//end if
                else
                {
                  if (TermList.CRLFCount > TermList.LFOnlyCount && TermList.CRLFCount > 0)
                  {
                    Console.WriteLine("File is mostly CRLF terminated" + nl);
                    NeedsFix = true;
                  }//end if
                }//end else

                Console.WriteLine("Number of unexpected CR chars if file is LF-only: {0}" + nl, 
                  TermList.CRLFCount);
                if (TermList.CRLFCount > 0)
                {
                  OutputLocations(TermList, TermList.FirstCRLF, OutputFormat, false, TermList.CRLFCount);
                }//end if
                Console.WriteLine("Number of unexpected LF-only chars if file is CRLF: {0}" + nl, 
                  TermList.LFOnlyCount);
                if (TermList.LFOnlyCount > 0)
                {
                  OutputLocations(TermList, TermList.FirstLFOnly, OutputFormat, false, TermList.LFOnlyCount);
                }//end if
                else
                {
                  Console.WriteLine("Insufficient line termination chars to " +
                                    "determine termination type." + nl);
                  //Jump to next file in list.
                  continue;
                }//end else
              }//end else
            }//end else

            if (TermList.CROnlyCount > 0)
            {
              CROnlyFix = true;
              Console.WriteLine("Number of unmatched CR-only chars: {0}" + nl, TermList.CROnlyCount);
              OutputLocations(TermList, TermList.FirstCROnly, OutputFormat, true, TermList.CROnlyCount);
            }//end if

            if (NeedsFix || CROnlyFix)
            {
              Prompt = nl + "Do you wish to fix this file? ";
            }//end if
            else
            {
              if (AllLF)
              {
                Prompt = "Do you wish to convert this file to all CRLF? ";
              }//end if
              else
              {
                if (AllCRLF)
                {
                  Prompt = "Do you wish to convert this file to all LF-only? ";
                }//end if
              }//end else
            }//end else
						
            Console.Write(Prompt);
            Reply = Console.ReadLine();
            if (Reply.ToLower() == "y" || Reply.ToLower() == "yes")
            {
              try
              {
                OutFilename = Path.GetFileNameWithoutExtension(InFilename) + 
                  ".fixed" +
                  Path.GetExtension(InFilename);
                OutputFileStream = new FileStream(OutFilename, FileMode.Create);
                OutputWriter = new BinaryWriter(OutputFileStream, System.Text.Encoding.ASCII);
              }//end try
              catch (Exception e)
              {
                bool Exit = OutputException("Problem opening file: " + OutFilename, e);
                if (Exit)
                  return 1;
                else
                  continue;
              }//end catch

              //Reposition input file seek for second read from start.
              InputReader.BaseStream.Seek(0, SeekOrigin.Begin);
              InputReader.BaseStream.Position = 0;

              //If we got here with 'yes' then we will convert to opposite if no fixes required.
              if (AllLF)
              {
                CRLF = true;
              }//end if
              else
              {
                if (AllCRLF)
                {
                  LFOnly = true;
                }//end if
              }//end else

              //Since we need fixes, ask which way to convert.
              //Ask to convert towards most common one first.
              if (NeedsFix)
              {
                if (TermList.CRLFCount >= TermList.LFOnlyCount)  //Mostly CRLF
                {
                  Console.Write("Do you wish to make file CRLF? ");
                  Reply = Console.ReadLine();
                  if (Reply.ToLower() == "y" || Reply.ToLower() == "yes")
                  {
                    CRLF = true;
                  }//end if
                  else
                  {
                    Console.WriteLine("Modified file will be LF-only...");
                    LFOnly = true;
                  }//end else
                }//end if
                else  //Mostly LF-only
                {
                  Console.Write("Do you wish to make file LF-only? ");
                  Reply = Console.ReadLine();
                  if (Reply.ToLower() == "y" || Reply.ToLower() == "yes")
                  {
                    LFOnly = true;
                  }//end if
                  else
                  {
                    Console.WriteLine("Modified file will be CRLF...");
                    CRLF = true;
                  }//end else
                }
              }//end if
              if (TermList.CROnlyCount > 0)
              {
                Console.Write("Do you wish to delete unmatched CR chars? ");
                Reply = Console.ReadLine();
                if (Reply.ToLower() == "y" || Reply.ToLower() == "yes")
                {
                  CRFix = CRFixType.Delete;
                }
                else
                {
                  Console.WriteLine("Unmatched CR chars will be replaced with normal line termination.");
                  CRFix = CRFixType.Replace;
                }
              }//end if

              if (NeedsFix || CROnlyFix)
              {
                if (CROnlyFix && ! NeedsFix)  //CROnlyFix case
                {
                  if (CRLF)
                  {
                    FixOrConvert(TermList, 
                      InputReader, 
                      OutputWriter, 
                      crlfArr, 
                      TermList.FirstCROnly, 
                      CRFix);
                  }//end if
                  else
                  {
                    if (LFOnly)
                    {
                      FixOrConvert(TermList, 
                        InputReader, 
                        OutputWriter, 
                        lfArr, 
                        TermList.FirstCROnly, 
                        CRFix);
                    }//end if
                  }//end else

                }//end if
                else
                {
                  if (CRLF)  //Change LF-onlys to CRLF during fix
                  {
                    FixOrConvert(TermList, 
                      InputReader, 
                      OutputWriter, 
                      crlfArr, 
                      TermList.FirstLFOnly, 
                      CRFix);
                  }//end if
                  else
                  {
                    if (LFOnly)  //Change CRLFs to LF-only during fix
                    {
                      FixOrConvert(TermList, 
                        InputReader, 
                        OutputWriter,
                        lfArr, 
                        TermList.FirstCRLF, 
                        CRFix);
                    }//end if
                  }//end else
                }//end if
              }//end else
              else  //File conversion only
              {
                if (CRLF)  //Convert to CRLF
                {
                  FixOrConvert(TermList, InputReader, OutputWriter, crlfArr, -1, CRFix);
                }//end if
                else
                {
                  if (LFOnly)  //Convert to LF-only
                  {
                    FixOrConvert(TermList, InputReader, OutputWriter, lfArr, -1, CRFix);
                  }//end if
                }//end else
              }//end else
							
              OutputWriter.BaseStream.Flush();
              OutputWriter.Close();
              OutputWriter = null;
              Console.WriteLine(nl + "Modified file saved to: {0}", OutFilename);
            }//end if
            else  //Reply was 'no'
            {
              Console.WriteLine("Exiting without creating fixed or modified file.");
            }//end else

            InputReader.Close();
            InputReader = null;
            Console.WriteLine();
          }//end foreach
        }//end else
        return 0;
      }//end try
      catch (Exception e)
      {
        OutputException("Unexpected exception", e);
          return 1;
      }//end catch
      finally
      {
        if (InputReader != null)
        {
          InputReader.Close();
        }//end if
        if (OutputWriter !=null)
        {
          OutputWriter.Close();
        }//end if
      }//end finally
    }//end Main

    public static bool OutputException(string CurrentMessage, Exception e)
    {
      Console.WriteLine(nl + "============================================================");
      Console.WriteLine(CurrentMessage + nl);
      Console.WriteLine(e.Message);
      Console.WriteLine("============================================================" + nl);
      Console.Write("Do you wish to see more error content? ");
      string Reply = Console.ReadLine();
      if (Reply.ToLower() == "y" || Reply.ToLower() == "yes")
      {
        Console.WriteLine(e.ToString());
      }//end if
      Console.Write(nl + "Exit program? ");
      Reply = Console.ReadLine();
      if (Reply.ToLower() == "y" || Reply.ToLower() == "yes")
      {
        return true;
      }//end if
      else 
      {
        return false;
      }//end else
    }//end OutputException()

    internal enum FixType {ConvertLF, ConvertCRLF, FixLF, FixCRLF};
    internal enum CRFixType {Delete, Replace};

/// <summary>
    /// This function does the actual file fixing or conversion.
    /// It contains the main logic of this application.
/// </summary>
/// <param name="TermList"></param>
/// <param name="InputReader"></param>
/// <param name="OutputWriter"></param>
/// <param name="LineEnd"></param>
/// <param name="StartIndex"></param>
/// <param name="CRFix"></param>
    
    internal static void FixOrConvert(TermListType TermList,
                                      BinaryReader InputReader,
                                      BinaryWriter OutputWriter,
                                      byte[] LineEnd,
                                      int StartIndex,
                                      CRFixType CRFix)
    {
      try
      {
        long finalposition = 0;
        long chunk = 0;
        long position = 0;
        int jumpsize = 0;
        int jsize = 0;

        if (LineEnd.Length == 2)  //Replace LF-only with CRLF so jumpsize is only 1.
          jumpsize = 1;
        else
        {
          if (LineEnd.Length == 1)  //Replace CRLF with LF-only so jumpsize is 2.
            jumpsize = 2;
        }//end else
      

        if (StartIndex == -1)  //Convert only.
        {
          //Presumed to get here only with pure conversion and no fixes.  
          //Flag to indicate this is StartIndex == -1.
          //Entire list will be walked and converted.
          foreach (TermListItem tli in TermList.List)
          {
            position = tli.Position;
            chunk = position - finalposition - jumpsize;
            OutputWriter.Write(InputReader.ReadBytes((int)chunk));
            OutputWriter.Write(LineEnd);
            byte[] jump = InputReader.ReadBytes(jumpsize);
            finalposition = position;

          }//end foreach
        }//end if

        else  
        {
          //Fix the file: jump through the list starting with CROnly if at beginning.
          //The fix is complicated since it fixes both unmatched CRs by and mismatched line termination.
          //It works by jumping back and forth through the CROnly locations via the CROnlyNext field.

          int savedNext = -1;
          bool switched = false;
          TermListItem tli = null;
          //Start with CROnly if it's first in the list.  We'll switch back in the loop.
          if (TermList.FirstCROnly > -1 && TermList.FirstCROnly < StartIndex )
          {
            switched = true;
            savedNext = StartIndex;
            tli = TermList[TermList.FirstCROnly];
          }//end if
          else
          {
            tli = TermList[StartIndex];
            savedNext = -1;
            switched = false;
          }//end else
          
          //************  Main jump loop  ***********
          //Can come into this loop already switched onto the CROnly track
          //by way of code above.
          while (true)
          {
            position = tli.Position;
            if (tli.TermType == TermTypeEnum.CROnly)
              jsize = 1;
            else
              jsize = jumpsize;
            chunk = position - finalposition - jsize;
            if (chunk > -1)  //protect against case if position < finalposition + jumpsize
            {
              OutputWriter.Write(InputReader.ReadBytes((int)chunk));
            }
            //Handle CROnly case to delete char
            if (tli.TermType != TermTypeEnum.CROnly) 
            {
              OutputWriter.Write(LineEnd);
            }//end if
              //Still write if CR fix is to replace, otherwise delete by not writing anything.
            else 
            {
              if (CRFix == CRFixType.Replace)
              {
                OutputWriter.Write(LineEnd);
              }//end if
            }//end else

            byte[] jump = InputReader.ReadBytes(jsize);
            //finalposition carries critical info around to the start of the loop
            finalposition = position;  

            //Loop exit point.  When there are no more jumps anywhere then exit.
            if (tli.Next < 0 && tli.CROnlyNext < 0 && savedNext < 0)
            {
              break;
            }//end if

            //If we are on an entry where there there is a CROnly next then switch to that track.
            //The CROnly next should always be "nearer" than the normal next due to the way that
            //it is updated in the list.
            if (tli.CROnlyNext > -1)
            {
              if (!switched)
              {
                savedNext = tli.Next;
                switched = true;
              }//end if
              tli = TermList[tli.CROnlyNext];
            }//end if
            else
            {
              //We are still on the CROnly track.  
              if (switched)
              {
                //Check to see if we need to switch back.
                if (tli.Next > savedNext && savedNext > -1 || tli.Next < 0)
                {
                  tli = TermList[savedNext];
                  switched = false;
                  savedNext = -1;
                }//end if
                else  //Get another CROnly before switching
                {
                  tli = TermList[tli.Next];  //We're on the CROnly track here.
                }//end else
              }//end if
              else
              {
                tli = TermList[tli.Next];
              }//end else
            }//end else
          }//end while
        }//end else

        //Get remainder of file.
        OutputWriter.Write(InputReader.ReadBytes((int)(InputReader.BaseStream.Length - finalposition)));
      }//end try
      catch (Exception e)
      {
        OutputException("Exception in FixOrConvert", e);
      }//end catch
    }//end FixOrConvert()

                    
    /// <summary>
    /// Outputs the locations of the line terminations
    /// </summary>
    /// <param name="TermList"></param>
    /// <param name="StartIndex"></param>
    /// <param name="OutputFormat"></param>
    public static void OutputLocations(TermListType TermList, 
                                       int StartIndex, 
                                       string OutputFormat, 
                                       bool CROnly,
                                       int Count)
    {
      try
      {
        if (Count < 100)  //Don't output locations if too many to conveniently list.
        {
          Console.Write("Output file locations? 'y' or 'n' ");
          string Reply = Console.ReadLine();
          if (Reply.ToLower() == "y" || Reply.ToLower() == "yes")
          {
            Console.WriteLine();

            int index = StartIndex;
            while (index > -1)
            {
              TermListItem tli = (TermListItem) TermList[index];
              if (CROnly)
              {
                if (tli.TermType == TermTypeEnum.CROnly)
                {
                  Console.WriteLine(OutputFormat, tli.Position, tli.Position);
                }//end if
                index = tli.Next;
              }//end if
              else
              {
                if (tli.TermType != TermTypeEnum.CROnly)
                {
                  Console.WriteLine(OutputFormat, tli.Position, tli.Position);
                }//end if
                index = tli.Next;
              }//end else
            }//end while
            Console.WriteLine();
          }//end if
        }//end if
      }//end try
      catch (Exception e)
      {
        OutputException("Exception in OutputLocations", e);
      }//end catch
    }//end OutputLocations()

	}//end class

  /// <summary>
  /// Enum to support element types
  /// </summary>
  public enum TermTypeEnum {CROnly, LFOnly, CRLF, Empty};

  /// <summary>
  /// Class to contain list of positions of line terminators
  /// </summary>
  public class TermListType
  {
    private int m_FirstLFOnly;
    private int m_FirstCROnly;
    private int m_FirstCRLF;

    private int m_LastLFOnly;
    private int m_LastCROnly;
    private int m_LastCRLF;

    private int m_LFOnlyCount;
    private int m_CROnlyCount;
    private int m_CRLFCount;

    private ArrayList m_List;

    /// <summary>
    /// Constructor.
    /// </summary>
    public TermListType()
    {
      this.m_List = new ArrayList();

      this.m_FirstLFOnly = -1;
      this.m_FirstCROnly = -1;
      this.m_FirstCRLF = -1;
    
      this.m_LastLFOnly = -1;
      this.m_LastCROnly = -1;
      this.m_LastCRLF = -1;

      this.m_LFOnlyCount = 0;
      this.m_CROnlyCount = 0;
      this.m_CRLFCount = 0;
      
    }//end constructor

    /// <summary>
    /// This is really the core of the logic.
    /// Updates the jump list allowing for CROnlyNext entries.
    /// This method directly modifies the m_* static variables that store
    /// the counts and locations of the first and last jump entries.
    /// </summary>
    /// <param name="Count"></param>
    /// <param name="last"></param>
    /// <param name="first"></param>
    /// <param name="CROnly"></param>
    private void UpdateList(ref int Count, ref int last, ref int first, bool CROnly)
    {
      try
      {
        Count++;
        if (last > -1)
        {
          TermListItem setnext = (TermListItem) m_List[last];
          setnext.Next = m_List.Count - 1;
          last = m_List.Count - 1;
        }//end if
        else  //First entry into the jump list.  Initialize both first and last.
        {
          first = m_List.Count - 1;
          last = m_List.Count - 1;
        }//end else
        //Special case the CROnly to insert special jumps into both LF and CRLF entries.
        if (CROnly)  
        {
          if (m_LastLFOnly > -1)
          {
            TermListItem lastlf = (TermListItem) m_List[m_LastLFOnly];
            if (lastlf.CROnlyNext == -1)
            {
              lastlf.CROnlyNext = m_List.Count - 1;
            }//end if
          }//end if
          if (m_LastCRLF > -1)
          {
            TermListItem lastcrlf = (TermListItem) m_List[m_LastCRLF];
            if (lastcrlf.CROnlyNext == -1)
            {
              lastcrlf.CROnlyNext = m_List.Count - 1;
            }//end if
          }//end if
        }//end if
      }//end try
      catch (Exception e)
      {
        CRLFAnalyzer.OutputException("Exception in TermListType.UpdateList", e);
      }//end catch
    }//end UpdateList

    /// <summary>
    /// Allows adding TermListItems to the TermList.
    /// </summary>
    /// <param name="tli"></param>
    public void Add(TermListItem tli)
    {
      try
      {
        m_List.Add(tli);

        switch (tli.TermType)
        {
          case TermTypeEnum.LFOnly:
            UpdateList(ref m_LFOnlyCount, ref m_LastLFOnly, ref m_FirstLFOnly, false);
            break;

          case TermTypeEnum.CROnly:  //route all jumps through here to catch this in every fix
            UpdateList(ref m_CROnlyCount, ref m_LastCROnly, ref m_FirstCROnly, true);
            break;

          case TermTypeEnum.CRLF:
            UpdateList(ref m_CRLFCount, ref m_LastCRLF, ref m_FirstCRLF, false);
            break;

          default:
            throw new System.InvalidOperationException("Bad value in switch statement");
        }//end switch
      }//end try
      catch (Exception e)
      {
        CRLFAnalyzer.OutputException("Exception in TermListType.Add", e);
      }//end catch
    }//end Add()

    /// <summary>
    /// The first CROnly entry in the list.
    /// </summary>
    public int FirstCROnly
    {
      get
      {
        return m_FirstCROnly;
      }//end get
    }

    /// <summary>
    /// The first LF-only in the list.
    /// </summary>
    public int FirstLFOnly
    {
      get
      {
        return m_FirstLFOnly;
      }//end get
    }

    /// <summary>
    /// The first CRLF in the list.
    /// </summary>
    public int FirstCRLF
    {
      get
      {
        return m_FirstCRLF;
      }//end get
    }

    /// <summary>
    /// The last LF-only in the list.
    /// </summary>
    public int LastLFOnly
    {
      get
      {
        return m_LastLFOnly;
      }//end get
    }

    /// <summary>
    /// The last CR-only in the list.
    /// </summary>
    public int LastCROnly
    {
      get
      {
        return m_LastCROnly;
      }//end get
    }

    /// <summary>
    /// The last CRLF in the list.
    /// </summary>
    public int LastCRLF
    {
      get
      {
        return m_LastCRLF;
      }//end get
    }


    /// <summary>
    /// Count of LF-only entries.
    /// </summary>
    public int LFOnlyCount
    {
      get
      {
        return m_LFOnlyCount;
      }//end get
    }

    /// <summary>
    /// Count of CR-only entries.
    /// </summary>
    public int CROnlyCount
    {
      get
      {
        return m_CROnlyCount;
      }//end get
    }

    /// <summary>
    /// Count of CRLF entries.
    /// </summary>
    public int CRLFCount
    {
      get
      {
        return m_CRLFCount;
      }//end get
    }

    /// <summary>
    /// The underlying ArrayList of TermListItems.
    /// </summary>
    public ArrayList List
    {
      get
      {
        return m_List;
      }
    }//end property
    
    /// <summary>
    /// Indexer for accessing this type.
    /// </summary>
    public TermListItem this [int index]
    {
      get
      {
        return (TermListItem) m_List[index];
      }//end get

      set
      {
        m_List[index] = value;
      }//end set
    }//end indexer
  }//end class TermListType


/// <summary>
/// Class that provides item for TermList
/// </summary>
  public class TermListItem
  {
    private TermTypeEnum m_TermType;
    private long m_Position;
    private int m_Next;
    private int m_CROnlyNext;

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="position"></param>
    /// <param name="tt"></param>
    public TermListItem(long position, TermTypeEnum tt)
    {
      m_TermType = tt;
      m_Position = position;
      m_Next = -1;
      m_CROnlyNext = -1;
    }//end constructor

    /// <summary>
    /// Classifies type of term list item.
    /// </summary>
    public TermTypeEnum TermType
    {
      get
      {
        return m_TermType;
      }//end get

      set
      {
        m_TermType = value;
      }//end set
    }//end property

    /// <summary>
    /// Position in the underlying arraylist.
    /// </summary>
    public long Position
    {
      get 
      {
        return m_Position;
      }//end get
      set
      {
        m_Position = value;
      }//end set
    }//end property

    /// <summary>
    /// Jump to next item of system type.
    /// </summary>
    public int Next
    {
      get
      {
        return m_Next;
      }//end get

      set 
      {
        m_Next = value;
      }//end set
    }//end property

    /// <summary>
    /// Jump to nearest CR-only entry.
    /// </summary>
    public int CROnlyNext
    {
      get
      {
        return m_CROnlyNext;
      }//end get

      set 
      {
        m_CROnlyNext = value;
      }//end set
    }//end property
  }//end class TermListItem
}//end namespace




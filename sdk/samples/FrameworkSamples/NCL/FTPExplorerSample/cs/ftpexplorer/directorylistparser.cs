//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

#region Using directives

using System;
using System.IO;
using System.Net;
using System.Text.RegularExpressions;
using System.Collections;
using System.Collections.Generic;

#endregion

namespace Microsoft.Samples.FTPExplorer
{
    public struct FileStruct
    {
        public string Flags;
        public string Owner;
        public bool IsDirectory;
        public string CreateTime;
        public string Name;
    }

    public enum FileListStyle
    {
        UnixStyle,
        WindowsStyle,
        Unknown
    }

    public class DirectoryListParser
    {
        private List<FileStruct> _myListArray;

        public FileStruct[] FullListing
        {
            get
            {
                return _myListArray.ToArray();                
            }
        }

        public FileStruct[] FileList
        {            
            get
            {
                List<FileStruct> _fileList = new List<FileStruct>();
                foreach(FileStruct thisstruct in _myListArray)
                {
                    if(!thisstruct.IsDirectory)
                    {
                        _fileList.Add(thisstruct);                        
                    }
                }
                return _fileList.ToArray();
            }
        }

        public FileStruct[] DirectoryList
        {
            get
            {
                List<FileStruct> _dirList = new List<FileStruct>();
                foreach(FileStruct thisstruct in _myListArray)
                {
                    if(thisstruct.IsDirectory)
                    {
                        _dirList.Add(thisstruct);                        
                    }
                }
                return _dirList.ToArray();
            }
        }

        public DirectoryListParser(string responseString)
        {
            _myListArray = GetList(responseString);            
        }
        
        private List<FileStruct> GetList(string datastring)
        {
            List<FileStruct> myListArray = new List<FileStruct>(); 
            string[] dataRecords = datastring.Split('\n');
            FileListStyle _directoryListStyle = GuessFileListStyle(dataRecords);
            foreach (string s in dataRecords)
            {
                if (_directoryListStyle != FileListStyle.Unknown && s != "")
                {
                    FileStruct f = new FileStruct();
                    f.Name = "..";
                    switch (_directoryListStyle)
                    {
                        case FileListStyle.UnixStyle:                            
                            f = ParseFileStructFromUnixStyleRecord(s);
                            break;
                        case FileListStyle.WindowsStyle:
                            f = ParseFileStructFromWindowsStyleRecord(s);
                            break;
                    }
                    if (f.Name != "" && f.Name != "." && f.Name != "..")
                    {
                        myListArray.Add(f);     
                    }
                }
            }
            return myListArray;
        }
        
        private FileStruct ParseFileStructFromWindowsStyleRecord(string Record)
        {
            ///Assuming the record style as 
            /// 02-03-04  07:46PM       <DIR>          Append
            FileStruct f = new FileStruct();
            string processstr = Record.Trim();
            string dateStr = processstr.Substring(0,8);      
            processstr = (processstr.Substring(8, processstr.Length - 8)).Trim();
            string timeStr = processstr.Substring(0, 7);
            processstr = (processstr.Substring(7, processstr.Length - 7)).Trim();
            f.CreateTime = dateStr + " " + timeStr;
            if (processstr.Substring(0,5) == "<DIR>")
            {
                f.IsDirectory = true;    
                processstr = (processstr.Substring(5, processstr.Length - 5)).Trim();
            }
            else
            {
                string[] strs = processstr.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                processstr = strs[1];
                f.IsDirectory = false;
            }
            f.Name = processstr;  //Rest is name   
            return f;
        }
        
        public FileListStyle GuessFileListStyle(string[] recordList)
        {
            foreach (string s in recordList)
            {
                if(s.Length > 10 
                    && Regex.IsMatch(s.Substring(0,10),"(-|d)((-|r)(-|w)(-|x)){3}"))
                {
                    return FileListStyle.UnixStyle;
                }    
                else if (s.Length > 8 
                    && Regex.IsMatch(s.Substring(0, 8),  "[0-9]{2}-[0-9]{2}-[0-9]{2}"))
                {
                    return FileListStyle.WindowsStyle;
                }
            }
            return FileListStyle.Unknown;
        }
        
        private FileStruct ParseFileStructFromUnixStyleRecord(string record)
        {
            ///Assuming record style as
            /// dr-xr-xr-x   1 owner    group               0 Nov 25  2002 bussys
            FileStruct f = new FileStruct();
            if (record[0] == '-' || record[0] == 'd')
            {// its a valid file record
                string processstr = record.Trim();
                f.Flags = processstr.Substring(0, 9);
                f.IsDirectory = (f.Flags[0] == 'd');
                processstr = (processstr.Substring(11)).Trim();
                _cutSubstringFromStringWithTrim(ref processstr, ' ', 0);   //skip one part
                f.Owner = _cutSubstringFromStringWithTrim(ref processstr, ' ', 0);
                f.CreateTime = getCreateTimeString(record);
                int fileNameIndex = record.IndexOf(f.CreateTime)+f.CreateTime.Length;
                f.Name = record.Substring(fileNameIndex).Trim();   //Rest of the part is name                
            }
            else
            {
                f.Name = "";
            }
            return f;
        }

        private string getCreateTimeString(string record)
        {
            //Does just basic datetime string validation for demo, not an accurate check
            //on date and time fields
            string month = "(jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec)";
            string space = @"(\040)+";
            string day = "([0-9]|[1-3][0-9])";
            string year = "[1-2][0-9]{3}";
            string time = "[0-9]{1,2}:[0-9]{2}";            
            Regex dateTimeRegex = new Regex(month+space+day+space+"("+year+"|"+time+")", RegexOptions.IgnoreCase);
            Match match = dateTimeRegex.Match(record);
            return match.Value;
        }        
        private string _cutSubstringFromStringWithTrim(ref string s, char c, int startIndex)
        {
            int pos1 = s.IndexOf(c, startIndex);
            string retString = s.Substring(0,pos1);
            s = (s.Substring(pos1)).Trim();
            return retString;
       }
    }
}


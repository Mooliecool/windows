//------------------------------------------------------------------------------
// <copyright file="WebRequestMethods.cs" company="Microsoft">
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

namespace System.Net {
    public static class WebRequestMethods{

        public static class Ftp {
            public const string DownloadFile =  "RETR";       // downloads a file
            public const string ListDirectory = "NLST";     // generates a list of files in directory
            public const string UploadFile =    "STOR";        // uploads a file
            public const string DeleteFile =    "DELE";         // deletes a file
            public const string AppendFile =    "APPE";      // append a file
            public const string GetFileSize =   "SIZE";        // determines if a file is in existance
            public const string UploadFileWithUniqueName = "STOU";
            public const string MakeDirectory = "MKD";
            public const string RemoveDirectory = "RMD";
            public const string ListDirectoryDetails = "LIST";
            public const string GetDateTimestamp = "MDTM";
            public const string PrintWorkingDirectory = "PWD";
            public const string Rename = "RENAME";
        }
    
        public static class Http{
            public const string Get = "GET";
            public const string Connect = "CONNECT";
            public const string Head = "HEAD";
            public const string Put = "PUT";
            public const string Post = "POST";
            public const string MkCol = "MKCOL";
        }
    
        public static class File{
            public const string DownloadFile = "GET";
            public const string UploadFile = "PUT";
        }
    }
}

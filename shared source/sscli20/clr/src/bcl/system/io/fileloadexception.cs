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
** Class:  FileLoadException
**
**
** Purpose: Exception for failure to load a file that was successfully found.
**
**
===========================================================*/

using System;
using System.Globalization;
using System.Runtime.Serialization;
using System.Runtime.CompilerServices;
using System.Security.Permissions;
using SecurityException = System.Security.SecurityException;

namespace System.IO {

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public class FileLoadException : IOException {

        private String _fileName;   // the name of the file we could not load.
        private String _fusionLog;  // fusion log (when applicable)

        public FileLoadException() 
            : base(Environment.GetResourceString("IO.FileLoad")) {
            SetErrorCode(__HResults.COR_E_FILELOAD);
        }
    
        public FileLoadException(String message) 
            : base(message) {
            SetErrorCode(__HResults.COR_E_FILELOAD);
        }
    
        public FileLoadException(String message, Exception inner) 
            : base(message, inner) {
            SetErrorCode(__HResults.COR_E_FILELOAD);
        }

        public FileLoadException(String message, String fileName) : base(message)
        {
            SetErrorCode(__HResults.COR_E_FILELOAD);
            _fileName = fileName;
        }

        public FileLoadException(String message, String fileName, Exception inner) 
            : base(message, inner) {
            SetErrorCode(__HResults.COR_E_FILELOAD);
            _fileName = fileName;
        }

        public override String Message
        {
            get {
                SetMessageField();
                return _message;
            }
        }

        private void SetMessageField()
        {
            if (_message == null)
	            _message = FormatFileLoadExceptionMessage(_fileName, HResult);
        }

        public String FileName {
            get { return _fileName; }
        }

        public override String ToString()
        {
            String s = GetType().FullName + ": " + Message;

            if (_fileName != null && _fileName.Length != 0)
                s += Environment.NewLine + String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.FileName_Name"), _fileName);
            
            if (InnerException != null)
                s = s + " ---> " + InnerException.ToString();

            if (StackTrace != null)
                s += Environment.NewLine + StackTrace;

            try
            {
                if(FusionLog!=null)
                {
                    if (s==null)
                        s=" ";
                    s+=Environment.NewLine;
                    s+=Environment.NewLine;
                    s+=FusionLog;
                }
            }
            catch(SecurityException)
            {
            
            }

            return s;
        }

        protected FileLoadException(SerializationInfo info, StreamingContext context) : base (info, context) {
            // Base class constructor will check info != null.

            _fileName = info.GetString("FileLoad_FileName");

            try
            {
                _fusionLog = info.GetString("FileLoad_FusionLog");
            }
            catch 
            {
                _fusionLog = null;
            }
				
        }

        private FileLoadException(String fileName, String fusionLog,int hResult)
            : base(null)
        {
            SetErrorCode(hResult);
            _fileName = fileName;
            _fusionLog=fusionLog;
            SetMessageField();
        }

        public String FusionLog {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get { return _fusionLog; }
        }

	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        public override void GetObjectData(SerializationInfo info, StreamingContext context) {
            // Serialize data for our base classes.  base will verify info != null.
            base.GetObjectData(info, context);

            // Serialize data for this class
            info.AddValue("FileLoad_FileName", _fileName, typeof(String));

            try
            {
                info.AddValue("FileLoad_FusionLog", FusionLog, typeof(String));
            }
            catch (SecurityException)
            {
            }
        }

        internal static String FormatFileLoadExceptionMessage(String fileName,
            int hResult)
        {
            return String.Format(CultureInfo.CurrentCulture, GetFileLoadExceptionMessage(hResult), fileName, GetMessageForHR(hResult));
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern String GetFileLoadExceptionMessage(int hResult);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern String GetMessageForHR(int hresult);
    }
}

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
** Class:  BadImageFormatException
**
**
** Purpose: Exception to an invalid dll or executable format.
**
** 
===========================================================*/
namespace System {
    
    using System;
    using System.Runtime.Serialization;
    using FileLoadException = System.IO.FileLoadException;
    using System.Security.Permissions;
    using SecurityException = System.Security.SecurityException;
    using System.Globalization;

    [System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class BadImageFormatException : SystemException {

        private String _fileName;  // The name of the corrupt PE file.
        private String _fusionLog;  // fusion log (when applicable)

        public BadImageFormatException() 
            : base(Environment.GetResourceString("Arg_BadImageFormatException")) {
            SetErrorCode(__HResults.COR_E_BADIMAGEFORMAT);
        }
    
        public BadImageFormatException(String message) 
            : base(message) {
            SetErrorCode(__HResults.COR_E_BADIMAGEFORMAT);
        }
        
        public BadImageFormatException(String message, Exception inner) 
            : base(message, inner) {
            SetErrorCode(__HResults.COR_E_BADIMAGEFORMAT);
        }

        public BadImageFormatException(String message, String fileName) : base(message)
        {
            SetErrorCode(__HResults.COR_E_BADIMAGEFORMAT);
            _fileName = fileName;
        }

        public BadImageFormatException(String message, String fileName, Exception inner) 
            : base(message, inner) {
            SetErrorCode(__HResults.COR_E_BADIMAGEFORMAT);
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
            if (_message == null) {
                if ((_fileName == null) &&
                    (HResult == System.__HResults.COR_E_EXCEPTION))
                    _message = Environment.GetResourceString("Arg_BadImageFormatException");

                else
                    _message = FileLoadException.FormatFileLoadExceptionMessage(_fileName, HResult);
            }
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

        protected BadImageFormatException(SerializationInfo info, StreamingContext context) : base(info, context) {
            // Base class constructor will check info != null.

            _fileName = info.GetString("BadImageFormat_FileName");
            try
            {
                _fusionLog = info.GetString("BadImageFormat_FusionLog");
            }
            catch 
            {
                _fusionLog = null;
            }
        }

        private BadImageFormatException(String fileName, String fusionLog, int hResult)
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
            info.AddValue("BadImageFormat_FileName", _fileName, typeof(String));
            try
            {
                info.AddValue("BadImageFormat_FusionLog", FusionLog, typeof(String));
            }
            catch (SecurityException)
            {
            }

        }
    }
}

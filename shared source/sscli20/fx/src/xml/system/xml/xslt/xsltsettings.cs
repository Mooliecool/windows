//------------------------------------------------------------------------------
// <copyright file="XsltSettings.cs" company="Microsoft">
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
// <spec>http://webdata/xml/specs/XslCompiledTransform.xml</spec>
//------------------------------------------------------------------------------

using System.CodeDom.Compiler;
using System.Reflection;

namespace System.Xml.Xsl {

    public sealed class XsltSettings {
        private bool enableDocumentFunction;
	    private bool enableScript;
        private bool checkOnly;
        private bool includeDebugInformation;
        private int  warningLevel = -1;     // -1 means not set
        private bool treatWarningsAsErrors;
        private AssemblyName        assemblyName;
        private TempFileCollection  tempFiles;

	    public XsltSettings() {}

	    public XsltSettings(bool enableDocumentFunction, bool enableScript) {
            this.enableDocumentFunction = enableDocumentFunction;
            this.enableScript           = enableScript;
        }

	    public static XsltSettings Default {
            get { return new XsltSettings(false, false); }
        }

	    public static XsltSettings TrustedXslt {
            get { return new XsltSettings(true, true); }
        }

	    public bool EnableDocumentFunction {
            get { return enableDocumentFunction;  }
            set { enableDocumentFunction = value; }
        }

	    public bool EnableScript {
            get { return enableScript;  }
            set { enableScript = value; }
        }

	    internal bool CheckOnly {
            get { return checkOnly;  }
            set { checkOnly = value; }
        }

	    internal bool IncludeDebugInformation {
            get { return includeDebugInformation;  }
            set { includeDebugInformation = value; }
        }

	    internal int WarningLevel {
            get { return warningLevel;  }
            set { warningLevel = value; }
        }

        internal bool TreatWarningsAsErrors {
            get { return treatWarningsAsErrors;  }
            set { treatWarningsAsErrors = value; }
        }

        internal AssemblyName AssemblyName {
            get { return assemblyName;  }
            set { assemblyName = value; }
        }

        internal TempFileCollection TempFiles {
            get { return tempFiles;  }
            set { tempFiles = value; }
        }
    }
}

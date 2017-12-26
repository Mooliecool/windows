//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
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

namespace Microsoft.Samples.Workflow.WorkflowDesignerRehosting
{
    using System;
    using System.IO;
    using System.ComponentModel.Design.Serialization;
    using System.Workflow.ComponentModel.Compiler;
    using System.Workflow.ComponentModel.Design;

    internal sealed class WorkflowLoader : WorkflowDesignerLoader
    {
        internal WorkflowLoader()
        {
        }

        protected override void Initialize()
        {
            base.Initialize();

            IDesignerLoaderHost host = LoaderHost;
            if (host != null)
            {
                TypeProvider typeProvider = new TypeProvider(host);
                typeProvider.AddAssemblyReference(typeof(string).Assembly.Location);
            }
        }

        public override void Dispose()
        {
            try
            {
                IDesignerLoaderHost host = LoaderHost;
                if (host != null)
                {
                    host.RemoveService(typeof(ITypeProvider), true);
                }
            }
            finally
            {
                base.Dispose();
            }
        }

        public override string FileName
        {
            get
            {
                return string.Empty;
            }
        }

        public override TextReader GetFileReader(string filePath)
        {
            return new StreamReader(new FileStream(filePath, FileMode.Open));
        }

        public override TextWriter GetFileWriter(string filePath)
        {
            return new StreamWriter(new FileStream(filePath, FileMode.OpenOrCreate));
        }
    }
}

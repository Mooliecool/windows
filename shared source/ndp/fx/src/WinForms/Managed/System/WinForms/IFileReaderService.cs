//------------------------------------------------------------------------------
// <copyright file="IFileReaderService.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Windows.Forms {

    using System.Diagnostics;
    using System;
    using System.IO;

    // 




    public interface IFileReaderService {
        /// <include file='doc\IFileReaderService.uex' path='docs/doc[@for="IFileReaderService.OpenFileFromSource"]/*' />
        Stream OpenFileFromSource(string relativePath);
    }
}

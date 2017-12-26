//------------------------------------------------------------------------------
// <copyright file="IRemovableWriter.cs" company="Microsoft">
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

namespace System.Xml {

    /// <summary>
    /// Event which is used when a raw writer in a processing pipeline wishes to remove itself from the pipeline and
    /// replace itself with another writer.
    /// </summary>
    internal delegate void OnRemoveWriter(XmlRawWriter writer);

    /// <summary>
    /// This interface is implemented by writers which wish to remove themselves from the processing pipeline once they
    /// have accomplished some work.  An example would be the auto-detect writer, which removes itself from the pipeline
    /// once it has determined whether to use the Xml or the Html output mode.
    /// </summary>
    internal interface IRemovableWriter {
        OnRemoveWriter OnRemoveWriterEvent { get; set; }
    }
}

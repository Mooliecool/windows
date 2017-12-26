//------------------------------------------------------------------------------
// <copyright file="EntityHandling.cs" company="Microsoft">
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


namespace System.Xml
{
    // Specifies how entities are handled in XmlTextReader and XmlValidatingReader.
    public enum EntityHandling
    {
        // Expand all entities. This is the default in XmlValidatingReader. No nodes with NodeType EntityReference will be returned. 
        // The entity text is expanded in place of the entity references.
        ExpandEntities      = 1,

        // Expand character entities only and return general entities as nodes (NodeType=XmlNodeType.EntityReference, Name=the name of the entity).
        // Default in XmlTextReader. You must call ResolveEntity to see what the general entity expands to.
        ExpandCharEntities  = 2,
    }
}

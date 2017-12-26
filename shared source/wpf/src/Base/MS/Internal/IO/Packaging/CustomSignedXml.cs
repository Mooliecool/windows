//-----------------------------------------------------------------------------
//
// <copyright file="CustomSignedXml.cs" company="Microsoft">
//    Copyright (C) Microsoft Corporation.  All rights reserved.
// </copyright>
//
// Description:
//  Wrapper class for existing SignedXml class that works around
//  DevDiv Schedule bug: 39530 (mdownen PM)
//
// History:
//  07/08/2005: BruceMac: Initial implementation.
//
//-----------------------------------------------------------------------------

using System;
using System.Xml;
using System.Windows;                          // for SR
using System.Security.Cryptography.Xml;
using MS.Internal.WindowsBase;

namespace MS.Internal.IO.Packaging
{
    /// <summary>
    /// SignedXml wrapper that supports reference targeting of internal ID's
    /// </summary>
    /// <remarks>See: http://www.w3.org/TR/2002/REC-xmldsig-core-20020212/ for details</remarks>
    internal class CustomSignedXml : SignedXml
    {
        /// <summary>
        /// Returns the XmlElement that matches the given id
        /// </summary>
        /// <param name="document"></param>
        /// <param name="idValue"></param>
        /// <returns>element if found, otherwise return null</returns>
        public override XmlElement GetIdElement(XmlDocument document, string idValue)
        {
            // Always let the base class have a first try at finding the element
            XmlElement elem = base.GetIdElement(document, idValue);

            // If not found then we will try to find it ourselves
            if (elem == null)
            {
                elem = SelectNodeByIdFromObjects(m_signature, idValue);
            }

            return elem;
        }

        /// <summary>
        /// Locate and return the node identified by idValue
        /// </summary>
        /// <param name="signature"></param>
        /// <param name="idValue"></param>
        /// <returns>node if found - else null</returns>
        /// <remarks>Tries to match each object in the Object list.</remarks>
        private static XmlElement SelectNodeByIdFromObjects(Signature signature, string idValue)
        {
            XmlElement node = null;

            // enumerate the objects
            foreach (DataObject dataObject in signature.ObjectList)
            {
                // direct reference to Object id - supported for all reference typs
                if (String.CompareOrdinal(idValue, dataObject.Id) == 0)
                {
                    // anticipate duplicate ID's and throw if any found
                    if (node != null)
                        throw new XmlException(SR.Get(SRID.DuplicateObjectId));

                    node = dataObject.GetXml();
                }
            }

            // now search for XAdES specific references
            if (node == null)
            {
                // For XAdES we implement special case where the reference may
                // be to an internal tag with matching "Id" attribute.
                node = SelectSubObjectNodeForXAdES(signature, idValue);
            }

            return node;
        }

        /// <summary>
        /// Locate any signed Object tag that matches the XAdES "target type"
        /// </summary>
        /// <param name="signature"></param>
        /// <param name="idValue"></param>
        /// <returns>element if found; null if not found</returns>
        /// <remarks>Special purpose code to support Sub-Object signing required by XAdES signatures</remarks>
        private static XmlElement SelectSubObjectNodeForXAdES(Signature signature, string idValue)
        {
            XmlElement node = null;

            // enumerate the References to determine if any are of type XAdES
            foreach (Reference reference in signature.SignedInfo.References)
            {
                // if we get a match by Type?
                if (String.CompareOrdinal(reference.Type, _XAdESTargetType) == 0)
                {
                    // now try to match by Uri
                    // strip off any preceding # mark to facilitate matching
                    string uri;
                    if ((reference.Uri.Length > 0) && (reference.Uri[0] == '#'))
                        uri = reference.Uri.Substring(1);
                    else
                        continue;   // ignore non-local references

                    // if we have a XAdES type reference and the ID matches the requested one
                    // search all object tags for the XML with this ID
                    if (String.CompareOrdinal(uri, idValue) == 0)
                    {
                        node = SelectSubObjectNodeForXAdESInDataObjects(signature, idValue);
                        break;
                    }
                }
            }

            return node;
        }

        /// <summary>
        /// Locates and selects the target XmlElement from all available Object tags
        /// </summary>
        /// <param name="signature"></param>
        /// <param name="idValue"></param>
        /// <returns>element if found; null if not found</returns>
        /// <remarks>relies on XPath query to search the Xml in each Object tag</remarks>
        private static XmlElement SelectSubObjectNodeForXAdESInDataObjects(Signature signature, string idValue)
        {
            XmlElement node = null;

            // now find an object tag that includes an element that matches
            foreach (DataObject dataObject in signature.ObjectList)
            {
                // skip the package object
                if (String.CompareOrdinal(dataObject.Id, XTable.Get(XTable.ID.OpcAttrValue)) != 0)
                {
                    XmlElement element = dataObject.GetXml();

                    // NOTE: this is executing an XPath query
                    XmlElement local = element.SelectSingleNode("//*[@Id='" + idValue + "']") as XmlElement;
                    if (local == null)
                        continue;

                    // node found?
                    if (local != null)
                    {
                        XmlNode temp = local;

                        // climb the tree towards the root until we find our namespace
                        while ((temp != null) && (temp.NamespaceURI.Length == 0))
                            temp = temp.ParentNode;

                        // only match if the target is in the XAdES namespace
                        if ((temp != null) && (String.CompareOrdinal(temp.NamespaceURI, _XAdESNameSpace) == 0))
                        {
                            node = local as XmlElement;
                            break;
                        }
                    }
                }
            }

            return node;
        }

        private const string _XAdESNameSpace = @"http://uri.etsi.org/01903/v1.2.2#";
        private const string _XAdESTargetType = _XAdESNameSpace + @"SignedProperties";
    }
}


// XpsFlatFile SDK Sample - RelationshipStream.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;


namespace XpsFlatFile
{
    // ======================= class RelationshipStream =======================
    /// <summary>
    ///   Represents an output stream for package relationships.</summary>
    public class RelationshipStream
    {
        // ------------------ RelationshipStream constructor ------------------
        /// <summary>
        ///   Initializes a new instance of the RelationshipStream class.</summary>
        /// <param name="path">
        ///   The path to where the relationship file should be stored.</param>
        public RelationshipStream(String path)
        {
            _path = path;
        }


        // ------------------------ CreateStartingPart ------------------------
        /// <summary>
        ///   Creates a package-level relationship to the FixedDocumentSequence.</summary>
        /// <param name="startingPartPath">
        ///   The path to the FixedDocumentSequence.</param>
        public void CreateStartingPart(String startingPartPath)
        {
            CreateRelationshipXmlWriter();

            _xmlTextWriter.WriteStartDocument();
            _xmlTextWriter.WriteStartElement(_relationshipsElement);
            _xmlTextWriter.WriteAttributeString("xmlns", _relationshipsNamespace);
            _xmlTextWriter.WriteStartElement(_relationshipElement);
            _xmlTextWriter.WriteAttributeString("Type", _startingPartType);
            _xmlTextWriter.WriteAttributeString("Target", startingPartPath);

            String id = GenerateRelationshipId();
            _xmlTextWriter.WriteAttributeString("Id", id);
            _xmlTextWriter.WriteEndElement();
        }


        // ----------------------- AddFixedRelationship -----------------------
        /// <summary>
        ///   Adds aspecified part-level relationship to the package.</summary>
        /// <param name="type">
        ///   The part-level relationship type.</param>
        /// <param name="target">
        ///   The target part.</param>
        public void AddFixedRelationship(String type, String target)
        {
            if (_xmlTextWriter == null)
            {
                CreateRelationshipXmlWriter();
                CreateFixedRelationshipsPart();
            }

            //Add relationship
            _xmlTextWriter.WriteStartElement(_relationshipElement);
            _xmlTextWriter.WriteAttributeString("Type", type);
            _xmlTextWriter.WriteAttributeString("Target", target);

            //Use guid as unique id for relationship
            String id = GenerateRelationshipId();
            _xmlTextWriter.WriteAttributeString("Id", id);
            _xmlTextWriter.WriteEndElement();
        }


        // ------------------------------ Close -------------------------------
        /// <summary>
        ///   Closes the relationship writer.</summary>
        public void Close()
        {
            if (_xmlTextWriter != null)
            {
                //Close out "Relationships" element
                _xmlTextWriter.WriteEndElement();
                _xmlTextWriter.Close();
                _xmlTextWriter = null;
            }
        }


        // ------------------- CreateRelationshipXmlWriter --------------------
        /// <summary>
        ///   Creates the relationship XmlWriter.</summary>
        private void CreateRelationshipXmlWriter()
        {
            String directory = Path.GetDirectoryName(_path);
            if (Directory.Exists(directory) != true)
            {
                Directory.CreateDirectory(directory);
            }
            _xmlTextWriter = new XmlTextWriter(_path, null);
        }


        // ------------------- CreateFixedRelationshipsPart -------------------
        /// <summary>
        ///   Creates a part-level relationship part.</summary>
        private void CreateFixedRelationshipsPart()
        {
            _xmlTextWriter.WriteStartDocument();
            _xmlTextWriter.WriteStartElement(_relationshipsElement);
            _xmlTextWriter.WriteAttributeString("xmlns", _relationshipsNamespace);
        }


        // --------------------- GenerateRelationshipId -----------------------
        /// <summary>
        ///   Creates a unique relationship ID.</summary>
        private String GenerateRelationshipId()
        {
            Guid relationshipGuid = Guid.NewGuid();
            return "R" + relationshipGuid.ToString();
        }


        private XmlTextWriter _xmlTextWriter;
        private String        _path;
        private const String  _relationshipsElement = "Relationships";
        private const String  _relationshipElement  = "Relationship";

        private const String  _relationshipsNamespace =
            "http://schemas.microsoft.com/package/2005/06/relationships";

        private const String  _startingPartType =
            "http://schemas.microsoft.com/xps/2005/06/fixedrepresentation";

        private const String  _requiredResourceType =
            "http://schemas.microsoft.com/xps/2005/06/required-resource";

    }// end:class RelationshipStream

}// end:namespace XpsFlatFile

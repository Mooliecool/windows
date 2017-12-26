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
** Class:  ResourceWriter
**
**
** Purpose: Default way to write strings to a CLR resource 
** file.
**
** 
===========================================================*/
namespace System.Resources {
    using System;
    using System.IO;
    using System.Text;
    using System.Collections;
    using System.Collections.Generic;
    using System.Runtime.Serialization;
    using System.Runtime.Serialization.Formatters.Binary;
    using System.Globalization;
    using System.Runtime.Versioning;

    // Generates a binary .resources file in the system default format 
    // from name and value pairs.  Create one with a unique file name,
    // call AddResource() at least once, then call Generate() to write
    // the .resources file to disk, then call Close() to close the file.
    // 
    // The resources generally aren't written out in the same order 
    // they were added.
    // 
    // See the RuntimeResourceSet overview for details on the system 
    // default file format.
    // 
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ResourceWriter : IResourceWriter
    {
        // For cases where users can't create an instance of the deserialized 
        // type in memory, and need to pass us serialized blobs instead.
        // LocStudio's managed code parser will do this in some cases.
        private class PrecannedResource
        {
            internal String TypeName;
            internal byte[] Data;

            internal PrecannedResource(String typeName, byte[] data)
            {
                TypeName = typeName;
                Data = data;
            }
        }

        // An initial size for our internal sorted list, to avoid extra resizes.
        private const int _ExpectedNumberOfResources = 1000;
        private const int AverageNameSize = 20 * 2;  // chars in little endian Unicode
        private const int AverageValueSize = 40;

        private Hashtable _resourceList;  // Uses FastResourceComparer
        private Stream _output;
        private Hashtable _caseInsensitiveDups; // To avoid names varying by case
        private Hashtable _preserializedData;   // For AddResourceData

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]        
        public ResourceWriter(String fileName)
        {
            if (fileName==null)
                throw new ArgumentNullException("fileName");
            _output = new FileStream(fileName, FileMode.Create, FileAccess.Write, FileShare.None);
            _resourceList = new Hashtable(_ExpectedNumberOfResources, FastResourceComparer.Default);
            _caseInsensitiveDups = new Hashtable(StringComparer.OrdinalIgnoreCase);
        }
    
        public ResourceWriter(Stream stream)
        {
            if (stream==null)
                throw new ArgumentNullException("stream");
            if (!stream.CanWrite)
                throw new ArgumentException(Environment.GetResourceString("Argument_StreamNotWritable"));
            _output = stream;
            _resourceList = new Hashtable(_ExpectedNumberOfResources, FastResourceComparer.Default);
            _caseInsensitiveDups = new Hashtable(StringComparer.OrdinalIgnoreCase);
        }
    
    
        // Adds a string resource to the list of resources to be written to a file.
        // They aren't written until Generate() is called.
        // 
        public void AddResource(String name, String value)
        {
            if (name==null)
                throw new ArgumentNullException("name");
            if (_resourceList == null)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceWriterSaved"));

            // Check for duplicate resources whose names vary only by case.
            _caseInsensitiveDups.Add(name, null);
            _resourceList.Add(name, value);
        }
        
        // Adds a resource of type Object to the list of resources to be 
        // written to a file.  They aren't written until Generate() is called.
        // 
        public void AddResource(String name, Object value)
        {
            if (name==null)
                throw new ArgumentNullException("name");
            if (_resourceList == null)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceWriterSaved"));

            // Check for duplicate resources whose names vary only by case.
            _caseInsensitiveDups.Add(name, null);
            _resourceList.Add(name, value);
        }
    
        // Adds a named byte array as a resource to the list of resources to 
        // be written to a file. They aren't written until Generate() is called.
        // 
        public void AddResource(String name, byte[] value)
        {
            if (name==null)
                throw new ArgumentNullException("name");
            if (_resourceList == null)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceWriterSaved"));

            // Check for duplicate resources whose names vary only by case.
            _caseInsensitiveDups.Add(name, null);
            _resourceList.Add(name, value);
        }
        
        public void AddResourceData(String name, String typeName, byte[] serializedData)
        {
            if (name == null)
                throw new ArgumentNullException("name");
            if (typeName == null)
                throw new ArgumentNullException("typeName");
            if (serializedData == null)
                throw new ArgumentNullException("serializedData");
            if (_resourceList == null)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceWriterSaved"));

            // Check for duplicate resources whose names vary only by case.
            _caseInsensitiveDups.Add(name, null);
            if (_preserializedData == null)
                _preserializedData = new Hashtable(FastResourceComparer.Default);

            _preserializedData.Add(name, new PrecannedResource(typeName, serializedData));
        }


        // Closes the output stream.
        public void Close()
        {
            Dispose(true);
        }

        private void Dispose(bool disposing)
        {
            if (disposing) {
                if (_resourceList != null) {
                    Generate();
                }
                if (_output != null) {
                    _output.Close();
                }
            }
            _output = null;
            _caseInsensitiveDups = null;
            // _resourceList is set to null by Generate.
        }

        public void Dispose()
        {
            Dispose(true);
        }

        // After calling AddResource, Generate() writes out all resources to the 
        // output stream in the system default format.
        // If an exception occurs during object serialization or during IO,
        // the .resources file is closed and deleted, since it is most likely
        // invalid.
        public void Generate()
        {
            if (_resourceList == null)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceWriterSaved"));

            BinaryWriter bw = new BinaryWriter(_output, Encoding.UTF8);
            List<String> typeNames = new List<String>();
                
            // Write out the ResourceManager header
            // Write out magic number
            bw.Write(ResourceManager.MagicNumber);
                
            // Write out ResourceManager header version number
            bw.Write(ResourceManager.HeaderVersionNumber);

            MemoryStream resMgrHeaderBlob = new MemoryStream(240);
            BinaryWriter resMgrHeaderPart = new BinaryWriter(resMgrHeaderBlob);

            // Write out class name of IResourceReader capable of handling 
            // this file.
            resMgrHeaderPart.Write(typeof(ResourceReader).AssemblyQualifiedName);

            // Write out class name of the ResourceSet class best suited to
            // handling this file.
            resMgrHeaderPart.Write(ResourceManager.ResSetTypeName);
            resMgrHeaderPart.Flush();

            // Write number of bytes to skip over to get past ResMgr header
            bw.Write((int)resMgrHeaderBlob.Length);

            // Write the rest of the ResMgr header
            bw.Write(resMgrHeaderBlob.GetBuffer(), 0, (int)resMgrHeaderBlob.Length);
            // End ResourceManager header


            // Write out the RuntimeResourceSet header
            // Version number
            bw.Write(RuntimeResourceSet.Version);
#if RESOURCE_FILE_FORMAT_DEBUG
            // Write out a tag so we know whether to enable or disable 
            // debugging support when reading the file.
            bw.Write("***DEBUG***");
#endif

            // number of resources
            int numResources = _resourceList.Count;
            if (_preserializedData != null)
                numResources += _preserializedData.Count;
            bw.Write(numResources);
                
            // Store values in temporary streams to write at end of file.
            int[] nameHashes = new int[numResources];
            int[] namePositions = new int[numResources];
            int curNameNumber = 0;
            MemoryStream nameSection = new MemoryStream(numResources * AverageNameSize);
            BinaryWriter names = new BinaryWriter(nameSection, Encoding.Unicode);
            MemoryStream dataSection = new MemoryStream(numResources * AverageValueSize);
            BinaryWriter data = new BinaryWriter(dataSection, Encoding.UTF8);
            IFormatter objFormatter = new BinaryFormatter(null, new StreamingContext(StreamingContextStates.File | StreamingContextStates.Persistence));
            
#if RESOURCE_FILE_FORMAT_DEBUG
            // Write NAMES right before the names section.
            names.Write(new byte[] { (byte) 'N', (byte) 'A', (byte) 'M', (byte) 'E', (byte) 'S', (byte) '-', (byte) '-', (byte) '>'});
            
            // Write DATA at the end of the name table section.
            data.Write(new byte[] { (byte) 'D', (byte) 'A', (byte) 'T', (byte) 'A', (byte) '-', (byte) '-', (byte)'-', (byte)'>'});
#endif

            // We've stored our resources internally in a Hashtable, which 
            // makes no guarantees about the ordering while enumerating.  
            // While we do our own sorting of the resource names based on their
            // hash values, that's only sorting the nameHashes and namePositions
            // arrays.  That's all that is strictly required for correctness,
            // but for ease of generating a patch in the future that 
            // modifies just .resources files, we should re-sort them.
            SortedList sortedResources = new SortedList(_resourceList, FastResourceComparer.Default);
            if (_preserializedData != null) {
                foreach(DictionaryEntry entry in _preserializedData)
                    sortedResources.Add(entry.Key, entry.Value);
            }
                

            IDictionaryEnumerator items = sortedResources.GetEnumerator();
            // Write resource name and position to the file, and the value
            // to our temporary buffer.  Save Type as well.
            while (items.MoveNext()) {
                nameHashes[curNameNumber] = FastResourceComparer.HashFunction((String)items.Key);
                namePositions[curNameNumber++] = (int) names.Seek(0, SeekOrigin.Current);
                names.Write((String) items.Key); // key
                names.Write((int)data.Seek(0, SeekOrigin.Current)); // virtual offset of value.
#if RESOURCE_FILE_FORMAT_DEBUG
                names.Write((byte) '*');
#endif
                Object value = items.Value;
                ResourceTypeCode typeCode = FindTypeCode(value, typeNames);

                // Write out type code
                Write7BitEncodedInt(data, (int) typeCode);

                // Write out value
                PrecannedResource userProvidedResource = value as PrecannedResource;
                if (userProvidedResource != null) {
                    data.Write(userProvidedResource.Data);
                }
                else {
                    WriteValue(typeCode, value, data, objFormatter);
                }

#if RESOURCE_FILE_FORMAT_DEBUG
                data.Write(new byte[] { (byte) 'S', (byte) 'T', (byte) 'O', (byte) 'P'});
#endif
            }
    
            // At this point, the ResourceManager header has been written.
            // Finish RuntimeResourceSet header
            //   Write size & contents of class table
            bw.Write(typeNames.Count);
            for(int i=0; i<typeNames.Count; i++)
                bw.Write(typeNames[i]);
            
            // Write out the name-related items for lookup.
            //  Note that the hash array and the namePositions array must
            //  be sorted in parallel.
            Array.Sort(nameHashes, namePositions);
            
            //  Prepare to write sorted name hashes (alignment fixup)
            //   Note: For 64-bit machines, these MUST be aligned on 8 byte 
            //   boundaries!  Pointers on IA64 must be aligned!  And we'll
            //   run faster on X86 machines too.
            bw.Flush();
            int alignBytes = ((int)bw.BaseStream.Position) & 7;
            if (alignBytes > 0) {
                for(int i=0; i<8 - alignBytes; i++)
                    bw.Write("PAD"[i % 3]);
            }
            
            //  Write out sorted name hashes.
            //   Align to 8 bytes.
            BCLDebug.Assert((bw.BaseStream.Position & 7) == 0, "ResourceWriter: Name hashes array won't be 8 byte aligned!  Ack!");
#if RESOURCE_FILE_FORMAT_DEBUG
            bw.Write(new byte[] { (byte) 'H', (byte) 'A', (byte) 'S', (byte) 'H', (byte) 'E', (byte) 'S', (byte) '-', (byte) '>'} );
#endif
            foreach(int hash in nameHashes)
                bw.Write(hash);
#if RESOURCE_FILE_FORMAT_DEBUG
            Console.Write("Name hashes: ");
            foreach(int hash in nameHashes)
                Console.Write(hash.ToString("x")+"  ");
            Console.WriteLine();
#endif

            //  Write relative positions of all the names in the file.
            //   Note: this data is 4 byte aligned, occuring immediately 
            //   after the 8 byte aligned name hashes (whose length may 
            //   potentially be odd).
            BCLDebug.Assert((bw.BaseStream.Position & 3) == 0, "ResourceWriter: Name positions array won't be 4 byte aligned!  Ack!");
#if RESOURCE_FILE_FORMAT_DEBUG
            bw.Write(new byte[] { (byte) 'P', (byte) 'O', (byte) 'S', (byte) '-', (byte) '-', (byte) '-', (byte) '-', (byte) '>' } );
#endif
            foreach(int pos in namePositions)
                bw.Write(pos);
#if RESOURCE_FILE_FORMAT_DEBUG
            Console.Write("Name positions: ");
            foreach(int pos in namePositions)
                Console.Write(pos.ToString("x")+"  ");
            Console.WriteLine();
#endif

            // Flush all BinaryWriters to MemoryStreams.
            bw.Flush();
            names.Flush();
            data.Flush();

            // Write offset to data section
            int startOfDataSection = (int) (bw.Seek(0, SeekOrigin.Current) + nameSection.Length);
            startOfDataSection += 4;  // We're writing an int to store this data, adding more bytes to the header
            BCLDebug.Log("RESMGRFILEFORMAT", "Generate: start of DataSection: 0x"+startOfDataSection.ToString("x", CultureInfo.InvariantCulture)+"  nameSection length: "+nameSection.Length);
            bw.Write(startOfDataSection);
            
            // Write name section.
            bw.Write(nameSection.GetBuffer(), 0, (int) nameSection.Length);
            names.Close();
            
            // Write data section.
            BCLDebug.Assert(startOfDataSection == bw.Seek(0, SeekOrigin.Current), "ResourceWriter::Generate - start of data section is wrong!");
            bw.Write(dataSection.GetBuffer(), 0, (int) dataSection.Length);
            data.Close();
            bw.Flush();

            // Indicate we've called Generate
            _resourceList = null;
        }

        // Finds the ResourceTypeCode for a type, or adds this type to the
        // types list.
        private ResourceTypeCode FindTypeCode(Object value, List<String> types)
        {
            if (value == null)
                return ResourceTypeCode.Null;

            Type type = value.GetType();
            if (type == typeof(String))
                return ResourceTypeCode.String;
            else if (type == typeof(Int32))
                return ResourceTypeCode.Int32;
            else if (type == typeof(Boolean))
                return ResourceTypeCode.Boolean;
            else if (type == typeof(Char))
                return ResourceTypeCode.Char;
            else if (type == typeof(Byte))
                return ResourceTypeCode.Byte;
            else if (type == typeof(SByte))
                return ResourceTypeCode.SByte;
            else if (type == typeof(Int16))
                return ResourceTypeCode.Int16;
            else if (type == typeof(Int64))
                return ResourceTypeCode.Int64;
            else if (type == typeof(UInt16))
                return ResourceTypeCode.UInt16;
            else if (type == typeof(UInt32))
                return ResourceTypeCode.UInt32;
            else if (type == typeof(UInt64))
                return ResourceTypeCode.UInt64;
            else if (type == typeof(Single))
                return ResourceTypeCode.Single;
            else if (type == typeof(Double))
                return ResourceTypeCode.Double;
            else if (type == typeof (Decimal))
                return ResourceTypeCode.Decimal;
            else if (type == typeof(DateTime))
                return ResourceTypeCode.DateTime;
            else if (type == typeof(TimeSpan))
                return ResourceTypeCode.TimeSpan;
            else if (type == typeof(byte[]))
                return ResourceTypeCode.ByteArray;
            else if (type == typeof(MemoryStream))
                return ResourceTypeCode.Stream;

            
            // This is a user type, or a precanned resource.  Find type 
            // table index.  If not there, add new element.
            String typeName;
            if (type == typeof(PrecannedResource)) {
                typeName = ((PrecannedResource) value).TypeName;
                if (typeName.StartsWith("ResourceTypeCode.", StringComparison.Ordinal)) {
                    typeName = typeName.Substring(17);  // Remove through '.'
                    ResourceTypeCode typeCode = (ResourceTypeCode) Enum.Parse(typeof(ResourceTypeCode), typeName);
                    return typeCode;
                }
            }
            else
                typeName = type.AssemblyQualifiedName;

            int typeIndex = types.IndexOf(typeName);
            if (typeIndex == -1) {
                typeIndex = types.Count;
                types.Add(typeName);
            }

            return (ResourceTypeCode)(typeIndex + ResourceTypeCode.StartOfUserTypes);
        }

    
        // WriteValue takes a value and writes it to stream.  It 
        // can take some specific action based on the type, such as write out a compact
        // version of the object if it's a type recognized by this ResourceWriter, or 
        // use Serialization to write out the object using the objFormatter.
        // For instance, the default implementation recognizes primitives such as Int32
        // as special types and calls WriteInt32(int) with the value of the object.  This
        // can be much more compact than serializing the same object.
        // 
        /*
        private void WriteValueV1(Type type, Object value, BinaryWriter writer, IFormatter objFormatter)
        {
            // For efficiency reasons, most of our primitive types will be explicitly
            // recognized here.  Some value classes are also special cased here.
            if (type == typeof(String))
                writer.Write((String) value);
            else if (type == typeof(Int32))
                writer.Write((int)value);
            else if (type == typeof(Byte))
                writer.Write((byte)value);
            else if (type == typeof(SByte))
                writer.Write((sbyte)value);
            else if (type == typeof(Int16))
                writer.Write((short)value);
            else if (type == typeof(Int64))
                writer.Write((long)value);
            else if (type == typeof(UInt16))
                writer.Write((ushort)value);
            else if (type == typeof(UInt32))
                writer.Write((uint)value);
            else if (type == typeof(UInt64))
                writer.Write((ulong)value);
            else if (type == typeof(Single))
                writer.Write((float)value);
            else if (type == typeof(Double))
                writer.Write((double)value);
            else if (type == typeof(DateTime)) {
                // Ideally we should use DateTime's ToBinary & FromBinary,
                // but we can't for compatibility reasons.
                writer.Write(((DateTime)value).Ticks);
            }
            else if (type == typeof(TimeSpan))
                writer.Write(((TimeSpan)value).Ticks);
            else if (type == typeof(Decimal)) {
                int[] bits = Decimal.GetBits((Decimal)value);
                BCLDebug.Assert(bits.Length == 4, "ResourceReader::LoadObject assumes Decimal's GetBits method returns an array of 4 ints");
                for(int i=0; i<bits.Length; i++)
                    writer.Write(bits[i]);
            }
            else if (type == null) {
                BCLDebug.Assert(value == null, "Type is null iff value is null");
#if RESOURCE_FILE_FORMAT_DEBUG
                writer.Write("<null value>");
#endif
            }
            else
                objFormatter.Serialize(writer.BaseStream, value);
        }
        */

        private void WriteValue(ResourceTypeCode typeCode, Object value, BinaryWriter writer, IFormatter objFormatter)
        {
            switch(typeCode) {
            case ResourceTypeCode.Null:
                break;

            case ResourceTypeCode.String:
                writer.Write((String) value);
                break;

            case ResourceTypeCode.Boolean:
                writer.Write((bool) value);
                break;

            case ResourceTypeCode.Char:
                writer.Write((UInt16) (char) value);
                break;

            case ResourceTypeCode.Byte:
                writer.Write((byte) value);
                break;

            case ResourceTypeCode.SByte:
                writer.Write((sbyte) value);
                break;
                
            case ResourceTypeCode.Int16:
                writer.Write((Int16) value);
                break;
                
            case ResourceTypeCode.UInt16:
                writer.Write((UInt16) value);
                break;

            case ResourceTypeCode.Int32:
                writer.Write((Int32) value);
                break;
                
            case ResourceTypeCode.UInt32:
                writer.Write((UInt32) value);
                break;

            case ResourceTypeCode.Int64:
                writer.Write((Int64) value);
                break;
                
            case ResourceTypeCode.UInt64:
                writer.Write((UInt64) value);
                break;

            case ResourceTypeCode.Single:
                writer.Write((Single) value);
                break;
                
            case ResourceTypeCode.Double:
                writer.Write((Double) value);
                break;

            case ResourceTypeCode.Decimal:
                writer.Write((Decimal) value);
                break;

            case ResourceTypeCode.DateTime:
                // Use DateTime's ToBinary & FromBinary.
                Int64 data = ((DateTime) value).ToBinary();
                writer.Write(data);
                break;
                
            case ResourceTypeCode.TimeSpan:
                writer.Write(((TimeSpan) value).Ticks);
                break;

            // Special Types
            case ResourceTypeCode.ByteArray:
                {
                    byte[] bytes = (byte[]) value;
                    writer.Write(bytes.Length);
                    writer.Write(bytes, 0, bytes.Length);
                    break;
                }

            case ResourceTypeCode.Stream:
                {
                    MemoryStream ms = (MemoryStream) value;
                    if (ms.Length > Int32.MaxValue)
                        throw new ArgumentException(Environment.GetResourceString("ArgumentOutOfRange_MemStreamLength"));
                    int offset, len;
                    ms.InternalGetOriginAndLength(out offset, out len);
                    byte[] bytes = ms.InternalGetBuffer();
                    writer.Write(len);
                    writer.Write(bytes, offset, len);
                    break;
                }

            default:
                BCLDebug.Assert(typeCode >= ResourceTypeCode.StartOfUserTypes, String.Format(CultureInfo.InvariantCulture, "ResourceReader: Unsupported ResourceTypeCode in .resources file!  {0}", typeCode));
                objFormatter.Serialize(writer.BaseStream, value);                
                break;
            }
        }

        private static void Write7BitEncodedInt(BinaryWriter store, int value) {
            // Write out an int 7 bits at a time.  The high bit of the byte,
            // when on, tells reader to continue reading more bytes.
            uint v = (uint) value;   // support negative numbers
            while (v >= 0x80) {
                store.Write((byte) (v | 0x80));
                v >>= 7;
            }
            store.Write((byte)v);
        }
    }
}

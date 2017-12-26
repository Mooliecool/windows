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
** Class:  ResourceReader
**
**
** Purpose: Default way to read streams of resources on 
** demand.
**
**         Version 2 support on October 6, 2003
** 
===========================================================*/
namespace System.Resources {
    using System;
    using System.IO;
    using System.Text;
    using System.Collections;
    using System.Collections.Generic;
    using System.Runtime.Serialization;
    using System.Runtime.Serialization.Formatters;
    using System.Runtime.Serialization.Formatters.Binary;
    using System.Reflection;
    using System.Security.Permissions;
    using System.Security;
    using System.Globalization;
    using System.Configuration.Assemblies;
    using System.Runtime.Versioning;

    // Provides the default implementation of IResourceReader, reading
    // .resources file from the system default binary format.  This class
    // can be treated as an enumerator once.
    // 
    // See the RuntimeResourceSet overview for details on the system 
    // default file format.
    // 

    internal struct ResourceLocator
    {
        internal Object _value;
        internal int _dataPos;

        internal ResourceLocator(int dataPos, Object value)
        {
            _dataPos = dataPos;
            _value = value;
        }

        internal int DataPosition {
            get { return _dataPos; }
            //set { _dataPos = value; }
        }

        // Allows adding in profiling data in a future version, or a special
        // resource profiling build.  We could also use WeakReference.
        internal Object Value {
            get { return _value; }
            set { _value = value; }
        }

        internal static bool CanCache(ResourceTypeCode value)
        {
            BCLDebug.Assert(value >= 0, "negative ResourceTypeCode.  What?");
            return value <= ResourceTypeCode.LastPrimitive;
        }   
    }


    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ResourceReader : IResourceReader
    {
        private BinaryReader _store;    // backing store we're reading from.
        // Used by RuntimeResourceSet and this class's enumerator.  Maps
        // resource name to a value, a ResourceLocator, or a 
        // LooselyLinkedManifestResource.
        internal Dictionary<String, ResourceLocator> _resCache;
        private long _nameSectionOffset;  // Offset to name section of file.
        private long _dataSectionOffset;  // Offset to Data section of file.

        // Note this class is tightly coupled with UnmanagedMemoryStream.
        // At runtime when getting an embedded resource from an assembly, 
        // we're given an UnmanagedMemoryStream referring to the mmap'ed portion
        // of the assembly.  The pointers here are pointers into that block of
        // memory controlled by the OS's loader.
        private int[] _nameHashes;    // hash values for all names.
        private unsafe int* _nameHashesPtr;  // In case we're using UnmanagedMemoryStream
        private int[] _namePositions; // relative locations of names
        private unsafe int* _namePositionsPtr;  // If we're using UnmanagedMemoryStream
        private Type[] _typeTable;    // Lazy array of Types for resource values.
        private int[] _typeNamePositions;  // To delay initialize type table
        private BinaryFormatter _objFormatter; // Deserialization stuff.
        private int _numResources;    // Num of resources files, in case arrays aren't allocated.        

        // We'll include a separate code path that uses UnmanagedMemoryStream to
        // avoid allocating String objects and the like.
        private UnmanagedMemoryStream _ums;

        // Version number of .resources file, for compatibility
        private int _version;

#if RESOURCE_FILE_FORMAT_DEBUG
        private bool _debug;   // Whether this file has debugging stuff in it.
#endif


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public ResourceReader(String fileName)
        {
            _resCache = new Dictionary<String, ResourceLocator>(FastResourceComparer.Default);
            _store = new BinaryReader(new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.Read), Encoding.UTF8);
            BCLDebug.Log("RESMGRFILEFORMAT", "ResourceReader .ctor(String).  UnmanagedMemoryStream: "+(_ums!=null));

            try {
                ReadResources();
            }
            catch {
                _store.Close(); // If we threw an exception, close the file.
                throw;
            }
        }
    
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        public ResourceReader(Stream stream)
        {
            if (stream==null)
                throw new ArgumentNullException("stream");
            if (!stream.CanRead)
                throw new ArgumentException(Environment.GetResourceString("Argument_StreamNotReadable"));

            _resCache = new Dictionary<String, ResourceLocator>(FastResourceComparer.Default);
            _store = new BinaryReader(stream, Encoding.UTF8);
            // We have a faster code path for reading resource files from an assembly.
            _ums = stream as UnmanagedMemoryStream;

            BCLDebug.Log("RESMGRFILEFORMAT", "ResourceReader .ctor(Stream).  UnmanagedMemoryStream: "+(_ums!=null));
            ReadResources();
        }
    
        // This is the constructor the RuntimeResourceSet calls,
        // passing in the stream to read from and the RuntimeResourceSet's 
        // internal hash table (hash table of names with file offsets
        // and values, coupled to this ResourceReader).
        internal ResourceReader(Stream stream, Dictionary<String, ResourceLocator> resCache)
        {
            BCLDebug.Assert(stream != null, "Need a stream!");
            BCLDebug.Assert(stream.CanRead, "Stream should be readable!");
            BCLDebug.Assert(resCache != null, "Need a Dictionary!");

            _resCache = resCache;
            _store = new BinaryReader(stream, Encoding.UTF8);

            _ums = stream as UnmanagedMemoryStream;

            BCLDebug.Log("RESMGRFILEFORMAT", "ResourceReader .ctor(Stream, Hashtable).  UnmanagedMemoryStream: "+(_ums!=null));
            ReadResources();
        }
        

        public void Close()
        {
            Dispose(true);
        }
        
        void IDisposable.Dispose()
        {
            Dispose(true);
        }

        private unsafe void Dispose(bool disposing)
        {
            if (_store != null) {
                _resCache = null;
                if (disposing) {
                    // Close the stream in a thread-safe way.  This fix means 
                    // that we may call Close n times, but that's safe.
                    BinaryReader copyOfStore = _store;
                    _store = null;
                    if (copyOfStore != null)
                        copyOfStore.Close();
                }
                _store = null;
                _namePositions = null;
                _nameHashes = null;
                _ums = null;
                _namePositionsPtr = null;
                _nameHashesPtr = null;
            }
        }
        
        internal static unsafe int ReadUnalignedI4(int* p)
        {
            byte* buffer = (byte*)p;
            // Unaligned, little endian format
            return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
        }

        private void SkipInt32() {
            _store.BaseStream.Seek(4, SeekOrigin.Current);
        }
            

        private void SkipString() {
            int stringLength = _store.Read7BitEncodedInt();
            _store.BaseStream.Seek(stringLength, SeekOrigin.Current);
        }

        private unsafe int GetNameHash(int index)
        {
   
            BCLDebug.Assert(index >=0 && index < _numResources, "Bad index into hash array.  index: "+index);
            BCLDebug.Assert((_ums == null && _nameHashes != null && _nameHashesPtr == null) || 
                            (_ums != null && _nameHashes == null && _nameHashesPtr != null), "Internal state mangled.");
            if (_ums == null)
                return _nameHashes[index];
            else
                return ReadUnalignedI4(&_nameHashesPtr[index]);
        }

        private unsafe int GetNamePosition(int index)
        {
            BCLDebug.Assert(index >=0 && index < _numResources, "Bad index into name position array.  index: "+index);
            BCLDebug.Assert((_ums == null && _namePositions != null && _namePositionsPtr == null) || 
                            (_ums != null && _namePositions == null && _namePositionsPtr != null), "Internal state mangled.");
            int r;
            if (_ums == null)
                r = _namePositions[index];
            else
                r = ReadUnalignedI4(&_namePositionsPtr[index]);
            if (r < 0 || r > _dataSectionOffset - _nameSectionOffset) {
                BCLDebug.Assert(false, "Corrupt .resources file!  NamePosition is outside of the name section!");
                throw new FormatException(Environment.GetResourceString("BadImageFormat_ResourcesNameOutOfSection", index, r.ToString("x", CultureInfo.InvariantCulture)));
            }
            return r;
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public IDictionaryEnumerator GetEnumerator()
        {
            if (_resCache == null)
                throw new InvalidOperationException(Environment.GetResourceString("ResourceReaderIsClosed"));
            return new ResourceEnumerator(this);
        }

        internal ResourceEnumerator GetEnumeratorInternal()
        {
            return new ResourceEnumerator(this);
        }

        // From a name, finds the associated virtual offset for the data.
        // To read the data, seek to _dataSectionOffset + dataPos, then
        // read the resource type & data.
        // This does a binary search through the names.
        internal int FindPosForResource(String name)
        {
            BCLDebug.Assert(_store != null, "ResourceReader is closed!");
            int hash = FastResourceComparer.HashFunction(name);
            BCLDebug.Log("RESMGRFILEFORMAT", "FindPosForResource for "+name+"  hash: "+hash.ToString("x", CultureInfo.InvariantCulture));
            // Binary search over the hashes.  Use the _namePositions array to 
            // determine where they exist in the underlying stream.
            int lo = 0;
            int hi = _numResources - 1;
            int index = -1;
            bool success = false;
            while (lo <= hi) {
                index = (lo + hi) >> 1;
                // Do NOT use subtraction here, since it will wrap for large
                // negative numbers. 
                int currentHash = GetNameHash(index);
                int c;
                if (currentHash == hash)
                    c = 0;
                else if (currentHash < hash)
                    c = -1;
                else
                    c = 1;
                //BCLDebug.Log("RESMGRFILEFORMAT", "  Probing index "+index+"  lo: "+lo+"  hi: "+hi+"  c: "+c);
                if (c == 0) {
                    success = true;
                    break;
                }
                if (c < 0)
                    lo = index + 1;
                else
                    hi = index - 1;
            }
            if (!success) {
#if RESOURCE_FILE_FORMAT_DEBUG
                String lastReadString;
                lock(this) {
                    _store.BaseStream.Seek(_nameSectionOffset + GetNamePosition(index), SeekOrigin.Begin);
                    lastReadString = _store.ReadString();
                }
                BCLDebug.Log("RESMGRFILEFORMAT", LogLevel.Status, "FindPosForResource for ", name, " failed.  i: ", index, "  lo: ", lo, "  hi: ", hi, "  last read string: \"", lastReadString, '\'');
#endif
                return -1;
            }
            
            // index is the location in our hash array that corresponds with a 
            // value in the namePositions array.
            // There could be collisions in our hash function.  Check on both sides 
            // of index to find the range of hash values that are equal to the
            // target hash value.
            if (lo != index) {
                lo = index;
                while (lo > 0 && GetNameHash(lo - 1) == hash)
                    lo--;
            }
            if (hi != index) {
                hi = index;
                while (hi < _numResources && GetNameHash(hi + 1) == hash)
                    hi++;
            }

            lock(this) {
                for(int i = lo; i<=hi; i++) {
                    _store.BaseStream.Seek(_nameSectionOffset + GetNamePosition(i), SeekOrigin.Begin);
                    if (CompareStringEqualsName(name)) {
                        int dataPos = _store.ReadInt32();
                        BCLDebug.Assert(dataPos >= 0 || dataPos < _store.BaseStream.Length - _dataSectionOffset, "Data section relative offset is out of the bounds of the data section!  dataPos: "+dataPos);
                        return dataPos;
                    }
                }
            }
            BCLDebug.Log("RESMGRFILEFORMAT", "FindPosForResource for "+name+": Found a hash collision, HOWEVER, neither of these collided values equaled the given string.");
            return -1;
        }

        // This compares the String in the .resources file at the current position
        // with the string you pass in. 
        // Whoever calls this method should make sure that they take a lock
        // so no one else can cause us to seek in the stream.
        private unsafe bool CompareStringEqualsName(String name)
        {
            BCLDebug.Assert(_store != null, "ResourceReader is closed!");
            //int byteLen = Read7BitEncodedInt(_store);
            int byteLen = _store.Read7BitEncodedInt();
            if (_ums != null) {
                //BCLDebug.Log("RESMGRFILEFORMAT", "CompareStringEqualsName using UnmanagedMemoryStream code path");
                byte* bytes = _ums.PositionPointer;
                // Skip over the data in the Stream, positioning ourselves right after it.
                _ums.Seek(byteLen, SeekOrigin.Current);

                if (_ums.Position > _ums.Length)
                    throw new BadImageFormatException(Environment.GetResourceString("BadImageFormat_ResourcesNameTooLong"));

                // On 64-bit machines, these char*'s may be misaligned.  Use a
                // byte-by-byte comparison instead.
                //return FastResourceComparer.CompareOrdinal((char*)bytes, byteLen/2, name) == 0;
                return FastResourceComparer.CompareOrdinal(bytes, byteLen, name) == 0;
            }
            else {
                // This code needs to be fast
                byte[] bytes = new byte[byteLen];
                int numBytesToRead = byteLen;
                while(numBytesToRead > 0) {
                    int n = _store.Read(bytes, byteLen - numBytesToRead, numBytesToRead);
                    if (n == 0)
                        throw new BadImageFormatException(Environment.GetResourceString("BadImageFormat_ResourceNameCorrupted"));
                    numBytesToRead -= n;
                }
                return FastResourceComparer.CompareOrdinal(bytes, byteLen/2, name) == 0;
            }
        }

        // This is used in the enumerator.  The enumerator iterates from 0 to n
        // of our resources and this returns the resource name for a particular
        // index.  The parameter is NOT a virtual offset.
        private unsafe String AllocateStringForNameIndex(int index, out int dataOffset)
        {
            BCLDebug.Assert(_store != null, "ResourceReader is closed!");
            byte[] bytes;
            int byteLen;
            long nameVA = GetNamePosition(index);
            lock (this) {
                _store.BaseStream.Seek(nameVA + _nameSectionOffset, SeekOrigin.Begin);
                // Can't use _store.ReadString, since it's using UTF-8!
                byteLen = _store.Read7BitEncodedInt();

                if (_ums != null) {
                    if (_ums.Position > _ums.Length - byteLen)
                        throw new BadImageFormatException(Environment.GetResourceString("BadImageFormat_ResourcesIndexTooLong", index));
                    
                    char* charPtr = (char*)_ums.PositionPointer;
                    String s = new String(charPtr, 0, byteLen/2);
                    _ums.Position += byteLen;
                    dataOffset = _store.ReadInt32();
                    return s;
                }

                bytes = new byte[byteLen];
                // We must read byteLen bytes, or we have a corrupted file.
                // Use a blocking read in case the stream doesn't give us back
                // everything immediately.
                int count = byteLen;
                while(count > 0) {
                    int n = _store.Read(bytes, byteLen - count, count);
                    if (n == 0)
                        throw new EndOfStreamException(Environment.GetResourceString("BadImageFormat_ResourceNameCorrupted_NameIndex", index));
                    count -= n;
                }
                dataOffset = _store.ReadInt32();
            }
            return Encoding.Unicode.GetString(bytes, 0, byteLen);
        }

        // This is used in the enumerator.  The enumerator iterates from 0 to n
        // of our resources and this returns the resource value for a particular
        // index.  The parameter is NOT a virtual offset.
        private Object GetValueForNameIndex(int index)
        {
            BCLDebug.Assert(_store != null, "ResourceReader is closed!");
            long nameVA = GetNamePosition(index);
            lock(this) {
                _store.BaseStream.Seek(nameVA + _nameSectionOffset, SeekOrigin.Begin);
                SkipString();
                //BCLDebug.Log("RESMGRFILEFORMAT", "GetValueForNameIndex for index: "+index+"  skip (name length): "+skip);
                int dataPos = _store.ReadInt32();
                BCLDebug.Log("RESMGRFILEFORMAT", "GetValueForNameIndex: dataPos: "+dataPos);
                ResourceTypeCode junk;
                if (_version == 1)
                    return LoadObjectV1(dataPos);
                else
                    return LoadObjectV2(dataPos, out junk);
            }
        }

        // This takes a virtual offset into the data section and reads a String
        // from that location.
        // Anyone who calls LoadObject should make sure they take a lock so 
        // no one can cause us to do a seek in here.
        internal String LoadString(int pos)
        {
            BCLDebug.Assert(_store != null, "ResourceReader is closed!");
            _store.BaseStream.Seek(_dataSectionOffset+pos, SeekOrigin.Begin);
            String s = null;
            int typeIndex = _store.Read7BitEncodedInt();
            if (_version == 1) {
                if (typeIndex == -1)
                    return null;
                if (FindType(typeIndex) != typeof(String))
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceNotString_Type", FindType(typeIndex).GetType().FullName));
                s = _store.ReadString();
            }
            else {
                ResourceTypeCode typeCode = (ResourceTypeCode) typeIndex;
                if (typeCode != ResourceTypeCode.String && typeCode != ResourceTypeCode.Null) {
                    String typeString;
                    if (typeCode < ResourceTypeCode.StartOfUserTypes)
                        typeString = typeCode.ToString();
                    else
                        typeString = FindType(typeCode - ResourceTypeCode.StartOfUserTypes).FullName;
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceNotString_Type", typeString));
                }
                if (typeCode == ResourceTypeCode.String) // ignore Null
                    s = _store.ReadString();
            }
            BCLDebug.Log("RESMGRFILEFORMAT", "LoadString("+pos.ToString("x", CultureInfo.InvariantCulture)+" returned "+(s==null ? "[a null string]" : s));
            return s;
        }

        // Called from RuntimeResourceSet
        internal Object LoadObject(int pos)
        {
            if (_version == 1)
                return LoadObjectV1(pos);
            ResourceTypeCode typeCode;
            return LoadObjectV2(pos, out typeCode);
        }

        internal Object LoadObject(int pos, out ResourceTypeCode typeCode)
        {
            if (_version == 1) {
                Object o = LoadObjectV1(pos);
                typeCode = (o is String) ? ResourceTypeCode.String : ResourceTypeCode.StartOfUserTypes;
                return o;
            }
            return LoadObjectV2(pos, out typeCode);
        }


        // This takes a virtual offset into the data section and reads an Object
        // from that location.
        // Anyone who calls LoadObject should make sure they take a lock so 
        // no one can cause us to do a seek in here.
        internal Object LoadObjectV1(int pos)
        {
            BCLDebug.Assert(_store != null, "ResourceReader is closed!");
            BCLDebug.Assert(_version == 1, ".resources file was not a V1 .resources file!");
            _store.BaseStream.Seek(_dataSectionOffset+pos, SeekOrigin.Begin);
            int typeIndex = _store.Read7BitEncodedInt();
            if (typeIndex == -1)
                return null;
            Type type = FindType(typeIndex);
            BCLDebug.Log("RESMGRFILEFORMAT", "LoadObject type: "+type.Name+"  pos: 0x"+_store.BaseStream.Position.ToString("x", CultureInfo.InvariantCulture));
            if (type == typeof(String))
                return _store.ReadString();
            else if (type == typeof(Int32))
                return _store.ReadInt32();
            else if (type == typeof(Byte))
                return _store.ReadByte();
            else if (type == typeof(SByte))
                return _store.ReadSByte();
            else if (type == typeof(Int16))
                return _store.ReadInt16();
            else if (type == typeof(Int64))
                return _store.ReadInt64();
            else if (type == typeof(UInt16))
                return _store.ReadUInt16();
            else if (type == typeof(UInt32))
                return _store.ReadUInt32();
            else if (type == typeof(UInt64))
                return _store.ReadUInt64();
            else if (type == typeof(Single))
                return _store.ReadSingle();
            else if (type == typeof(Double))
                return _store.ReadDouble();
            else if (type == typeof(DateTime)) {
                // Ideally we should use DateTime's ToBinary & FromBinary,
                // but we can't for compatibility reasons.
                return new DateTime(_store.ReadInt64());
            }
            else if (type == typeof(TimeSpan))
                return new TimeSpan(_store.ReadInt64());
            else if (type == typeof(Decimal)) {
                int[] bits = new int[4];
                for(int i=0; i<bits.Length; i++)
                    bits[i] = _store.ReadInt32();
                return new Decimal(bits);
            }
            else {
                return DeserializeObject(typeIndex);
            }
        }

        internal Object LoadObjectV2(int pos, out ResourceTypeCode typeCode)
        {
            BCLDebug.Assert(_store != null, "ResourceReader is closed!");
            BCLDebug.Assert(_version >= 2, ".resources file was not a V2 (or higher) .resources file!");
            _store.BaseStream.Seek(_dataSectionOffset+pos, SeekOrigin.Begin);
            typeCode = (ResourceTypeCode) _store.Read7BitEncodedInt();

            BCLDebug.Log("RESMGRFILEFORMAT", "LoadObjectV2 type: "+typeCode+"  pos: 0x"+_store.BaseStream.Position.ToString("x", CultureInfo.InvariantCulture));
            
            switch(typeCode) {
            case ResourceTypeCode.Null:
                return null;

            case ResourceTypeCode.String:
                return _store.ReadString();
                
            case ResourceTypeCode.Boolean:
                return _store.ReadBoolean();

            case ResourceTypeCode.Char:
                return (char) _store.ReadUInt16();

            case ResourceTypeCode.Byte:
                return _store.ReadByte();

            case ResourceTypeCode.SByte:
                return _store.ReadSByte();

            case ResourceTypeCode.Int16:
                return _store.ReadInt16();

            case ResourceTypeCode.UInt16:
                return _store.ReadUInt16();

            case ResourceTypeCode.Int32:
                return _store.ReadInt32();

            case ResourceTypeCode.UInt32:
                return _store.ReadUInt32();

            case ResourceTypeCode.Int64:
                return _store.ReadInt64();

            case ResourceTypeCode.UInt64:
                return _store.ReadUInt64();

            case ResourceTypeCode.Single:
                return _store.ReadSingle();

            case ResourceTypeCode.Double:
                return _store.ReadDouble();

            case ResourceTypeCode.Decimal:
                return _store.ReadDecimal();

            case ResourceTypeCode.DateTime:
                // Use DateTime's ToBinary & FromBinary.
                Int64 data = _store.ReadInt64();
                return DateTime.FromBinary(data);
               
            case ResourceTypeCode.TimeSpan:
                Int64 ticks = _store.ReadInt64();
                return new TimeSpan(ticks);

            // Special types
            case ResourceTypeCode.ByteArray:
                {
                    int len = _store.ReadInt32();
                    if (_ums == null)
                        return _store.ReadBytes(len);

                    if (len > _ums.Length - _ums.Position)
                        throw new BadImageFormatException(Environment.GetResourceString("BadImageFormat_ResourceDataTooLong"));

                    byte[] bytes = new byte[len];
                    int r = _ums.Read(bytes, 0, len);
                    BCLDebug.Assert(r == len, "ResourceReader needs to use a blocking read here.  (Call _store.ReadBytes(len)?)");
                    return bytes;
                }

            case ResourceTypeCode.Stream:
                {
                    int len = _store.ReadInt32();
                    if (_ums == null) {
                        byte[] bytes = _store.ReadBytes(len);
                        // Lifetime of memory == lifetime of this stream.
                        return new PinnedBufferMemoryStream(bytes);
                    }

                    // make sure we don't create an UnmanagedMemoryStream that is longer than the resource stream. 
                    if (len > _ums.Length - _ums.Position)
                        throw new BadImageFormatException(Environment.GetResourceString("BadImageFormat_ResourceDataTooLong"));

                    // For the case that we've memory mapped in the .resources
                    // file, just return a Stream pointing to that block of memory.
                    unsafe {
                        return new UnmanagedMemoryStream(_ums.PositionPointer, len, len, FileAccess.Read, true);
                    }
                }
                
            default:                
                BCLDebug.Assert(typeCode >= ResourceTypeCode.StartOfUserTypes, String.Format(CultureInfo.InvariantCulture, "ResourceReader: Unsupported ResourceTypeCode in .resources file!  {0}", typeCode));
                // Throw new exception?
                break;
            }

            // Normal serialized objects
            int typeIndex = typeCode - ResourceTypeCode.StartOfUserTypes;
            return DeserializeObject(typeIndex);
        }

        // Helper method to safely deserialize a type, using a type-limiting
        // deserialization binder to simulate a type-limiting deserializer.
        // This method handles types that are safe to deserialize, as well as
        // ensuring we only get back what we expect.
        private Object DeserializeObject(int typeIndex)
        {
            Type type = FindType(typeIndex);

            // Ensure that the object we deserialized is exactly the same
            // type of object we thought we should be deserializing.  This
            // will help prevent hacked .resources files from using our
            // serialization permission assert to deserialize anything
            // via a hacked type ID.   

            Object graph;
            graph = _objFormatter.Deserialize(_store.BaseStream);
            
            // This check is about correctness, not security at this point.
            if (graph.GetType() != type)
                throw new BadImageFormatException(Environment.GetResourceString("BadImageFormat_ResType&SerBlobMismatch", type.FullName, graph.GetType().FullName));

            return graph;
        }

        // Reads in the header information for a .resources file.  Verifies some
        // of the assumptions about this resource set, and builds the class table
        // for the default resource file format.
        private void ReadResources()
        {
            BCLDebug.Assert(_store != null, "ResourceReader is closed!");
            BinaryFormatter bf = new BinaryFormatter(null, new StreamingContext(StreamingContextStates.File | StreamingContextStates.Persistence));
            _objFormatter = bf;

            try {
                // Read ResourceManager header
                // Check for magic number
                int magicNum = _store.ReadInt32();
                if (magicNum != ResourceManager.MagicNumber)
                    throw new ArgumentException(Environment.GetResourceString("Resources_StreamNotValid"));
                // Assuming this is ResourceManager header V1 or greater, hopefully
                // after the version number there is a number of bytes to skip
                // to bypass the rest of the ResMgr header.
                int resMgrHeaderVersion = _store.ReadInt32();
                if (resMgrHeaderVersion > 1) {
                    int numBytesToSkip = _store.ReadInt32();
                    BCLDebug.Log("RESMGRFILEFORMAT", LogLevel.Status, "ReadResources: Unexpected ResMgr header version: {0}  Skipping ahead {1} bytes.", resMgrHeaderVersion, numBytesToSkip);
                    BCLDebug.Assert(numBytesToSkip >= 0, "numBytesToSkip in ResMgr header should be positive!");
                    _store.BaseStream.Seek(numBytesToSkip, SeekOrigin.Current);
                }
                else {
                    BCLDebug.Log("RESMGRFILEFORMAT", "ReadResources: Parsing ResMgr header v1.");
                    SkipInt32();    // We don't care about numBytesToSkip.

                    // Read in type name for a suitable ResourceReader
                    // Note ResourceWriter & InternalResGen use different Strings.
                    String readerType = _store.ReadString();
                    AssemblyName mscorlib = new AssemblyName(ResourceManager.MscorlibName);

                    if (!ResourceManager.CompareNames(readerType, ResourceManager.ResReaderTypeName, mscorlib))
                        throw new NotSupportedException(Environment.GetResourceString("NotSupported_WrongResourceReader_Type", readerType));

                    // Skip over type name for a suitable ResourceSet
                    SkipString();
                }

                // Read RuntimeResourceSet header
                // Do file version check
                int version = _store.ReadInt32();
                if (version != RuntimeResourceSet.Version && version != 1)
                    throw new ArgumentException(Environment.GetResourceString("Arg_ResourceFileUnsupportedVersion", RuntimeResourceSet.Version, version));
                _version = version;

#if RESOURCE_FILE_FORMAT_DEBUG
                // Look for ***DEBUG*** to see if this is a debuggable file.
                long oldPos = _store.BaseStream.Position;
                _debug = false;
                try {
                    String debugString = _store.ReadString();
                    _debug = String.Equals("***DEBUG***", debugString);
                }
                catch(IOException) {
                }
                catch(OutOfMemoryException) {
                }
                if (_debug) {
                    Console.WriteLine("ResourceReader is looking at a debuggable .resources file, version {0}", _version);
                }
                else {
                    _store.BaseStream.Position = oldPos;
                }
#endif

                _numResources = _store.ReadInt32();
                BCLDebug.Log("RESMGRFILEFORMAT", "ReadResources: Expecting "+ _numResources+ " resources.");
#if _DEBUG      
                if (ResourceManager.DEBUG >= 4)
                    Console.WriteLine("ResourceReader::ReadResources - Reading in "+_numResources+" resources");
#endif

                // Read type positions into type positions array.
                // But delay initialize the type table.
                int numTypes = _store.ReadInt32();
                _typeTable = new Type[numTypes];
                _typeNamePositions = new int[numTypes];
                for(int i=0; i<numTypes; i++) {
                    _typeNamePositions[i] = (int) _store.BaseStream.Position;

                    // Skip over the Strings in the file.  Don't create types.
                    SkipString();
                }

#if _DEBUG
                if (ResourceManager.DEBUG >= 5)
                    Console.WriteLine("ResourceReader::ReadResources - Reading in "+numTypes+" type table entries");
#endif

                // Prepare to read in the array of name hashes
                //  Note that the name hashes array is aligned to 8 bytes so 
                //  we can use pointers into it on 64 bit machines. (4 bytes 
                //  may be sufficient, but let's plan for the future)
                //  Skip over alignment stuff.  All public .resources files
                //  should be aligned   No need to verify the byte values.
                long pos = _store.BaseStream.Position;
                int alignBytes = ((int)pos) & 7;
                if (alignBytes != 0) {
                    for(int i=0; i<8 - alignBytes; i++) {
                        _store.ReadByte();
                    }
                }

                // Read in the array of name hashes
#if RESOURCE_FILE_FORMAT_DEBUG
                //  Skip over "HASHES->"
                if (_debug) {
                    _store.BaseStream.Position += 8;
                }
#endif

                if (_ums == null) {
                    _nameHashes = new int[_numResources];
                    for(int i=0; i<_numResources; i++)
                        _nameHashes[i] = _store.ReadInt32();
                }
                else {
                    unsafe {
                        _nameHashesPtr = (int*) _ums.PositionPointer;
                        // Skip over the array of nameHashes.
                        _ums.Seek(4 * _numResources, SeekOrigin.Current);
                        // get the position pointer once more to check that the whole table is within the stream
                        byte* junk = _ums.PositionPointer;
                    }
                }

                // Read in the array of relative positions for all the names.
#if RESOURCE_FILE_FORMAT_DEBUG
                // Skip over "POS---->"
                if (_debug) {
                    _store.BaseStream.Position += 8;
                }
#endif
                if (_ums == null) {
                    _namePositions = new int[_numResources];
                    for(int i=0; i<_numResources; i++)
                        _namePositions[i] = _store.ReadInt32();
                }
                else {
                    unsafe {
                        _namePositionsPtr = (int*) _ums.PositionPointer;
                        // Skip over the array of namePositions.
                        _ums.Seek(4 * _numResources, SeekOrigin.Current);
                        // get the position pointer once more to check that the whole table is within the stream
                        byte* junk = _ums.PositionPointer;
                    }
                }

                // Read location of data section.
                _dataSectionOffset = _store.ReadInt32();

                // Store current location as start of name section
                _nameSectionOffset = _store.BaseStream.Position;
                BCLDebug.Log("RESMGRFILEFORMAT", String.Format(CultureInfo.InvariantCulture, "ReadResources: _nameOffset = 0x{0:x}  _dataOffset = 0x{1:x}", _nameSectionOffset, _dataSectionOffset));
            }
            catch (EndOfStreamException eof) {
                throw new BadImageFormatException(Environment.GetResourceString("BadImageFormat_ResourcesHeaderCorrupted"), eof);
            }
            catch (IndexOutOfRangeException e) {
                throw new BadImageFormatException(Environment.GetResourceString("BadImageFormat_ResourcesHeaderCorrupted"), e);
            }
        }

        // This allows us to delay-initialize the Type[].  This might be a 
        // good startup time savings, since we might have to load assemblies
        // and initialize Reflection.
        private Type FindType(int typeIndex)
        {
            if (_typeTable[typeIndex] == null) {
                long oldPos = _store.BaseStream.Position;
                try {
                    _store.BaseStream.Position = _typeNamePositions[typeIndex];
                    String typeName = _store.ReadString();
                    _typeTable[typeIndex] = Type.GetType(typeName, true);
                }
                finally {
                    _store.BaseStream.Position = oldPos;
                }
            }
            BCLDebug.Assert(_typeTable[typeIndex] != null, "Should have found a type!");
            return _typeTable[typeIndex];
        }


        public void GetResourceData(String resourceName, out String resourceType, out byte[] resourceData)
        {
            if (resourceName == null)
                throw new ArgumentNullException("resourceName");
            if (_resCache == null)
                throw new InvalidOperationException(Environment.GetResourceString("ResourceReaderIsClosed"));

            // Get the type information from the data section.  Also,
            // sort all of the data section's indexes to compute length of
            // the serialized data for this type (making sure to subtract
            // off the length of the type code).
            int[] dataPositions = new int[_numResources];
            int dataPos = FindPosForResource(resourceName);
            if( dataPos == -1) {
                throw new ArgumentException(Environment.GetResourceString("Arg_ResourceNameNotExist", resourceName));
            }
            
            lock(this) {
                // Read all the positions of data within the data section.
                for(int i=0; i<_numResources; i++) {
                    _store.BaseStream.Position = _nameSectionOffset + GetNamePosition(i);
                    // Skip over name of resource
                    int numBytesToSkip = _store.Read7BitEncodedInt();
                    _store.BaseStream.Position += numBytesToSkip;

                    dataPositions[i] = _store.ReadInt32();
                }
                Array.Sort(dataPositions);

                int index = Array.BinarySearch(dataPositions, dataPos);
                BCLDebug.Assert(index >= 0 && index < _numResources, "Couldn't find data position within sorted data positions array!");
                long nextData = (index < _numResources - 1) ? dataPositions[index + 1] + _dataSectionOffset : _store.BaseStream.Length;
                int len = (int) (nextData - (dataPos + _dataSectionOffset));
                BCLDebug.Assert(len >= 0 && len <= (int) _store.BaseStream.Length - dataPos + _dataSectionOffset, "Length was negative or outside the bounds of the file!");

                // Read type code then byte[]
                _store.BaseStream.Position = _dataSectionOffset + dataPos;
                ResourceTypeCode typeCode = (ResourceTypeCode) _store.Read7BitEncodedInt();
                resourceType = TypeNameFromTypeCode(typeCode);

                // The length must be adjusted to subtract off the number 
                // of bytes in the 7 bit encoded type code.
                len -= (int) (_store.BaseStream.Position - (_dataSectionOffset + dataPos));
                byte[] bytes = _store.ReadBytes(len);
                if (bytes.Length != len)
                    throw new FormatException(Environment.GetResourceString("BadImageFormat_ResourceNameCorrupted"));
                resourceData = bytes;
            }
        }

        private String TypeNameFromTypeCode(ResourceTypeCode typeCode)
        {
            BCLDebug.Assert(typeCode >= 0, "can't be negative");
            if (typeCode < ResourceTypeCode.StartOfUserTypes) {
                BCLDebug.Assert(!String.Equals(typeCode.ToString(), "LastPrimitive"), "Change ResourceTypeCode metadata order so LastPrimitive isn't what Enum.ToString prefers.");
                return "ResourceTypeCode." + typeCode.ToString();
            }
            else {
                int typeIndex = typeCode - ResourceTypeCode.StartOfUserTypes;
                BCLDebug.Assert(typeIndex >= 0 && typeIndex < _typeTable.Length, "TypeCode is broken or corrupted!");
                long oldPos = _store.BaseStream.Position;
                try {
                    _store.BaseStream.Position = _typeNamePositions[typeIndex];
                    return _store.ReadString();
                }
                finally {
                    _store.BaseStream.Position = oldPos;
                }
            }
        }




        internal sealed class ResourceEnumerator : IDictionaryEnumerator
        {
            private const int ENUM_DONE = Int32.MinValue;
            private const int ENUM_NOT_STARTED = -1;

            private ResourceReader _reader;
            private bool _currentIsValid;
            private int _currentName;
            private int _dataPosition; // cached for case-insensitive table

            internal ResourceEnumerator(ResourceReader reader)
            {
                _currentName = ENUM_NOT_STARTED;
                _reader = reader;
                _dataPosition = -2;
            }

            public bool MoveNext()
            {
                if (_currentName == _reader._numResources - 1 || _currentName == ENUM_DONE) {
                    _currentIsValid = false;
                    _currentName = ENUM_DONE;
                    return false;
                }
                _currentIsValid = true;
                _currentName++;
                return true;
            }
        
            public Object Key {
                get {
                    if (_currentName == ENUM_DONE) throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumEnded));
                    if (!_currentIsValid) throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumNotStarted));
                    if (_reader._resCache == null) throw new InvalidOperationException(Environment.GetResourceString("ResourceReaderIsClosed"));

                    return _reader.AllocateStringForNameIndex(_currentName, out _dataPosition);
                }
            }
        
            public Object Current {
                get {
                    return Entry;
                }
            }

            // Warning: This requires that you call the Key or Entry property FIRST before calling it!
            internal int DataPosition {
                get {
                    return _dataPosition;
                }
            }

            public DictionaryEntry Entry {
                get {
                    if (_currentName == ENUM_DONE) throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumEnded));
                    if (!_currentIsValid) throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumNotStarted));
                    if (_reader._resCache == null) throw new InvalidOperationException(Environment.GetResourceString("ResourceReaderIsClosed"));

                    String key;
                    Object value = null;
                    lock (_reader._resCache) {
                        key = _reader.AllocateStringForNameIndex(_currentName, out _dataPosition);
                        ResourceLocator locator;
                        if (_reader._resCache.TryGetValue(key, out locator)) {
                            value = locator.Value;
                        }
                        if (value == null) {
                            if (_dataPosition == -1) 
                                value = _reader.GetValueForNameIndex(_currentName);
                            else 
                                value = _reader.LoadObject(_dataPosition);
                            // If enumeration and subsequent lookups happen very
                            // frequently in the same process, add a ResourceLocator
                            // to _resCache here.  But WinForms enumerates and
                            // just about everyone else does lookups.  So caching
                            // here may bloat working set.
                        }
                    }
                    return new DictionaryEntry(key, value);
                }
            }
    
            public Object Value {
                get {
                    if (_currentName == ENUM_DONE) throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumEnded));
                    if (!_currentIsValid) throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumNotStarted));
                    if (_reader._resCache == null) throw new InvalidOperationException(Environment.GetResourceString("ResourceReaderIsClosed"));

                    return _reader.GetValueForNameIndex(_currentName);
                }
            }

            public void Reset()
            {
                if (_reader._resCache == null) throw new InvalidOperationException(Environment.GetResourceString("ResourceReaderIsClosed"));
                _currentIsValid = false;
                _currentName = ENUM_NOT_STARTED;
            }
        }
    }
}

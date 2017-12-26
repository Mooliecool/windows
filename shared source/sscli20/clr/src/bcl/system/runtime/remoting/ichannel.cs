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
** File:    IChannel.cs
**
**
** Purpose: Defines the interfaces implemented by channels
**
**
===========================================================*/
namespace System.Runtime.Remoting.Channels {
    using System.Collections;
    using System.IO;
    using System.Runtime.Remoting;
    using System.Runtime.Remoting.Messaging;
    using System.Runtime.Remoting.Metadata;
    using System.Security.Permissions;
    using System;
    using System.Globalization;
    
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IChannel
    {
        int ChannelPriority 
	{
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
    	    get; 
	}
        String ChannelName
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
	    get; 
        }
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
        String Parse(String url, out String objectURI);
    }
    
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IChannelSender : IChannel
    {
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
        IMessageSink CreateMessageSink(String url, Object remoteChannelData, out String objectURI);
    } // interface IChannelSender
    
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IChannelReceiver : IChannel
    {
        Object ChannelData 
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
	    get;
	}
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
        String[] GetUrlsForUri(String objectURI);
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
        void StartListening(Object data);
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
        void StopListening(Object data);
    } // interface IChannelReceiver 


[System.Runtime.InteropServices.ComVisible(true)]
    public interface IChannelReceiverHook
    {
        String ChannelScheme
	{
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
            get;
	}

        bool WantsToListen
        { 
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
	    get;
        }

        IServerChannelSink ChannelSinkChain
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 	
	     get;
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
        void AddHookChannelUri(String channelUri);
        
    } // interface IChannelReceiverHook

[System.Runtime.InteropServices.ComVisible(true)]
    public interface IClientChannelSinkProvider
    {
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
        IClientChannelSink CreateSink(IChannelSender channel, String url, Object remoteChannelData);

        IClientChannelSinkProvider Next 
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 
	     get;
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	      set; 
        }
    } // interface IClientChannelSinkProvider


[System.Runtime.InteropServices.ComVisible(true)]
    public interface IServerChannelSinkProvider
    {
        // The sink provider should also chain this call to the next provider in the chain.
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        void GetChannelData(IChannelDataStore channelData);
        
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        IServerChannelSink CreateSink(IChannelReceiver channel);

        IServerChannelSinkProvider Next
	{ 
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	    get;
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
            set;
	}
    } // interface IServerChannelSinkProvider


    // This has the same exact functionality as IClientChannelSinkProvider.
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IClientFormatterSinkProvider : IClientChannelSinkProvider
    {
    } // interface IClientFormatterSinkProvider


    // This has the same exact functionality as IServerChannelSinkProvider.
    // There may be multiple formatter sinks. If there are more than one, then one
    // formatter sink may decide to delegate the formatting responsibilities to the next
    // one.
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IServerFormatterSinkProvider : IServerChannelSinkProvider
    {
    } // interface IServerFormatterSinkProvider



[System.Runtime.InteropServices.ComVisible(true)]
    public interface IClientChannelSink : IChannelSinkBase
    {
        // Parameters:
        //   msg - it's the "IMethodCallMessage"
        //   requestHeaders - headers to add to the outgoing message heading to server
        //   requestStream - stream headed towards the transport sink
        //   responseHeaders - headers that server returned
        //   responseStream - stream coming back from the transport sink
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        void ProcessMessage(IMessage msg,
                            ITransportHeaders requestHeaders, Stream requestStream,
                            out ITransportHeaders responseHeaders, out Stream responseStream);
                            
        // Parameters: (for async calls, the transport sink is responsible for doing the write
        //     and read asynchronously)
        //   sinkStack - channel sinks who called this one
        //   msg - it's the "IMessage" msg
        //   headers - headers to add to the outgoing message heading to server
        //   stream - stream headed towards the transport sink
        // Returns:
        //   IAsyncResult for this call.
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        void AsyncProcessRequest(IClientChannelSinkStack sinkStack, IMessage msg,
                                 ITransportHeaders headers, Stream stream);
                                 
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        void AsyncProcessResponse(IClientResponseChannelSinkStack sinkStack, Object state,
                                  ITransportHeaders headers, Stream stream);
                                  
        // Parameters:
        //   msg - it's the "IMethodCallMessage"
        //   headers - headers to add to the outgoing message heading to server
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        Stream GetRequestStream(IMessage msg, ITransportHeaders headers);


        // the next channel sink in the chain
        IClientChannelSink NextChannelSink
        { 
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	    get;
	}
        
    } // IClientChannelSink
    


    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum ServerProcessing
    {
        Complete, // server synchronously processed message
        OneWay,   // message dispatched and no response should be sent
        Async     // the call was dispatched asynchronously
                  //   (sink should store data on stack for later processing)
    } // ServerProcessing


    // Used for security sink and transport sinks.
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IServerChannelSink : IChannelSinkBase
    {
        // Parameters:
        //   sinkStack - channel sinks who called this one
        //   requestMsg - deserialized request message or null if the stream hasn't been deserialized
        //   requestHeaders - headers retrieved from the incoming message from client
        //   requestStream - stream to process and pass onto towards the deserialization sink.
        //   responseMsg - response message
        //   responseHeaders - headers to add to return message heading to client
        //   responseStream - stream heading back towards the transport sink
        // Returns:
        //   Provides information about how message was processed.
	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        ServerProcessing ProcessMessage(IServerChannelSinkStack sinkStack,
                                        IMessage requestMsg,
                                        ITransportHeaders requestHeaders, Stream requestStream,
                                        out IMessage responseMsg, out ITransportHeaders responseHeaders,
                                        out Stream responseStream);

        // Parameters: (for async calls)
        //   sinkStack - sink stack (leading back to the server transport sink)
        //   headers - headers to add to return message heading to client
        //   stream - stream heading back towards the transport sink
	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        void AsyncProcessResponse(IServerResponseChannelSinkStack sinkStack, Object state,
                                  IMessage msg, ITransportHeaders headers, Stream stream);                   

        // Parameters:
        //   sinkStack - sink stack (leading back to the server transport sink)  
        //   state - state that had been pushed to the stack by this sink
        //   msg - it's the "IMethodCallMessage"
        //   headers - headers to put in response message to client 
	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        Stream GetResponseStream(IServerResponseChannelSinkStack sinkStack, Object state,
                                 IMessage msg, ITransportHeaders headers);                               


        // the next channel sink in the chain
        IServerChannelSink NextChannelSink 
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	     get;
        }

    } // interface IServerChannelSink


[System.Runtime.InteropServices.ComVisible(true)]
    public interface IChannelSinkBase
    {
        // This returns a dictionary through which properties on the sink may be retrieved
        // or configured. If a property is not present in this sink, it should delegate the
        // the property get or set to the next sink in the chain.
        IDictionary Properties 
	{
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	     get;
	}  
    }



    // Client formatter sinks are both a message sink and a channel sink.
    // They initially transform the message into a stream.
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IClientFormatterSink : IMessageSink, IClientChannelSink
    {
    } // interface IClientFormatterSink




    // Channels wishing to use the channel sink architecture must implement this interface
    //   on their ChannelData object.
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IChannelDataStore
    {
        String[] ChannelUris 
	{
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 			
	     get;
	}
    
        Object this[Object key] 
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	     get;
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	     set;
	}
    } // interface IChannelDataStore


    // Used to store channel data for our remoting channels
    [Serializable,
    SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure),
    SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)
    ]
[System.Runtime.InteropServices.ComVisible(true)]
    public class ChannelDataStore : IChannelDataStore
    {
        String[]           _channelURIs;  // list of channel uri's that map to this channel
        
        DictionaryEntry[]  _extraData;    // sink data

        // private constructor for shallow clone    
        private ChannelDataStore(String[] channelUrls, DictionaryEntry[] extraData)
        {
            _channelURIs = channelUrls;
            _extraData = extraData;
        }

        public ChannelDataStore(String[] channelURIs)
        {
            _channelURIs = channelURIs;
            _extraData = null;
        }

        internal ChannelDataStore InternalShallowCopy()
        {
            return new ChannelDataStore(_channelURIs, _extraData);
        }


        public String[] ChannelUris
        {
            get { return _channelURIs; }
            set { _channelURIs = value; }
        }

        // implementation of IChannelDataStore
        public Object this[Object key]
        {
            get 
            {
                // look for matching key in extra data list
                foreach (DictionaryEntry entry in _extraData)
                {
                    if (entry.Key.Equals(key))
                        return entry.Value;                    
                }

                // entry not found
                return null;
            } // get

            set
            {
                if (_extraData == null)
                {
                    _extraData = new DictionaryEntry[1];
                    _extraData[0] = new DictionaryEntry(key, value);
                }
                else
                {
                    int length =_extraData.Length;
                    DictionaryEntry[] newList = new DictionaryEntry[length + 1];
                    int co = 0;
                    for (; co < length; co++)
                        newList[co] = _extraData[co];
                    newList[co] = new DictionaryEntry(key, value); // set last value
                    _extraData = newList;
                }
            } // set
        } // Object this[Object key]
        
    } // class ChannelDataStore




[System.Runtime.InteropServices.ComVisible(true)]
    public interface ITransportHeaders
    {
        // Should add headers in order, and remove them if somebody tries to set
        //   it to null.
        Object this[Object key]
	{ 
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	    get;
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	    set;
	}

        // This is required to handback an enumerator which iterates over all entries
        //   in the headers.
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        IEnumerator GetEnumerator();
    } // interface ITransportHeaders


    // TransportHeaders is used to store a collection of headers that is used in the channel sinks.
    // The header list preserves order, and keeps headers in the order that they were originally
    // added. You can use the foreach statement to iterate over the contents of the header list 
    // which contains objects of type DictionaryEntry. Keys are required to be strings, and
    // comparisons are done on a case-insensitive basis.
    [Serializable,    
    SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure),
    SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
[System.Runtime.InteropServices.ComVisible(true)]
    public class TransportHeaders : ITransportHeaders
    {
        private ArrayList _headerList; // array of type DictionaryEntry
    
        public TransportHeaders()
        {
            // The default capacity is larger than we'll typically need so start off at 6.
            _headerList = new ArrayList(6);
        } // TransportHeaders


        // dictionary like interface
        public Object this[Object key]
        {
            get 
            {
                String strKey = (String)key;
                
                // look for matching key in header list
                foreach (DictionaryEntry entry in _headerList)
                {
                    if (String.Compare((String)entry.Key, strKey, StringComparison.OrdinalIgnoreCase) == 0)
                        return entry.Value;                    
                }

                // entry not found
                return null;
            } // get

            set
            {
                if (key == null)
                    return;

                String strKey = (String)key;

                // remove this entry if it's already in the list
                int co = _headerList.Count - 1;
                while (co >= 0)
                {                
                    String headerKey = (String)((DictionaryEntry)_headerList[co]).Key;
                    if (String.Compare(headerKey, strKey, StringComparison.OrdinalIgnoreCase) == 0)
                    {
                        _headerList.RemoveAt(co);
                        break;
                    }
                    co--;
                }

                // otherwise, add this entry
                if (value != null)
                {
                    _headerList.Add(new DictionaryEntry(key, value));
                }
            } // set
        } // Object this[Object key]


        public IEnumerator GetEnumerator() 
        {
            return _headerList.GetEnumerator(); 
        } // GetEnumerator
        
    } // TransportHeaders



    // All sink providers must have a constructor that accepts a hashtable and 
    //   an ArrayList of these structures if they want to be used from a config file.
[System.Runtime.InteropServices.ComVisible(true)]
    public class SinkProviderData
    {
        private String _name;
        private Hashtable _properties = new Hashtable(StringComparer.InvariantCultureIgnoreCase);
        private ArrayList _children = new ArrayList();

        public SinkProviderData(String name) 
        {
            _name = name;
        }

        public String Name { get { return _name; } }
        public IDictionary Properties { get { return _properties; } }
        public IList Children { get { return _children; } }
    } // class SinkProviderData





    // Base implementation for channel sinks that want to supply properties.
    // The derived class only needs to implement the Keys property and this[].
    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure),
    SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class BaseChannelSinkWithProperties : BaseChannelObjectWithProperties
    {       
        protected BaseChannelSinkWithProperties() : base()
        {
        }    
    } // class BaseChannelSinkWithProperties


    // This provides a base implementation of a channel that wants
    // to provide a dictionary interface. It handles the complexity
    // of asking a channel sink chain for their properties.
    // Once the channel constructs its sink chain (usually only applicable
    // for server channels) it should set the SinksWithProperties property.
    // The properties only get chained for channel sinks when the users asks
    // for the "Properties" dictionary. If you just go through the dictionary interface
    // you'll only get the channel specific properties.
    // The derived class only needs to implement the Keys property and this[].
    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure),
    SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class BaseChannelWithProperties : BaseChannelObjectWithProperties
    {    
        protected IChannelSinkBase SinksWithProperties = null;

        protected BaseChannelWithProperties() : base()
        {
        }

        // This is overridden so that server channels can expose the properties
        //   of their channel sinks through a flat dictionary interface.
        public override IDictionary Properties 
        {
	        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		        
            get
            {
                // collect dictionaries for all channel sinks and return
                //   aggregate dictionary
                ArrayList dictionaries = new ArrayList();

                // add the channel itself
                dictionaries.Add(this);

                if (SinksWithProperties != null)
                {
                    IServerChannelSink srvSink = SinksWithProperties as IServerChannelSink;
                    if (srvSink != null)
                    {
                        while (srvSink != null)
                        {
                            IDictionary dict = srvSink.Properties;
                            if (dict != null)
                                dictionaries.Add(dict);

                            srvSink = srvSink.NextChannelSink;
                        }
                    }
                    else
                    {
                        // we know it's a client channel sink
                        IClientChannelSink chnlSink = (IClientChannelSink)SinksWithProperties;                        
                        
                        while (chnlSink != null)
                        {
                            IDictionary dict = chnlSink.Properties;
                            if (dict != null)
                                dictionaries.Add(dict);

                            chnlSink = chnlSink.NextChannelSink;
                        }
                    }                    
                }

                // return a dictionary that spans all dictionaries provided
                return new AggregateDictionary(dictionaries);
            }
        } 

    } // class BaseChannelWithProperties

    





    // Base implementation for channel sinks that want to supply properties.
    // The derived class only needs to implement the Keys property and this[].
    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure),
    SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class BaseChannelObjectWithProperties : IDictionary
    {
        protected BaseChannelObjectWithProperties()
        {
        }   

        public virtual IDictionary Properties { 
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
            get { return this; } 
        }

        // 
        // IDictionary implementation        
        //

        public virtual Object this[Object key]
        {
            get { return null; }            
            set { throw new NotImplementedException(); } 
        } // Object this[Object key]

        public virtual ICollection Keys { get { return null; } }
        
        public virtual ICollection Values
        {
            get
            {
                ICollection keys = Keys;
                if (keys == null)
                    return null;

                ArrayList values = new ArrayList();
                foreach (Object key in keys)
                {
                    values.Add(this[key]);
                }                

                return values;
            }
        }       

        public virtual bool Contains(Object key) 
        {
            if (key == null)
                return false;

            ICollection keySet = Keys;
            if (keySet == null)
                return false;

            String keyStr = key as String;
        
            foreach (Object someKey in keySet)
            {
                if (keyStr != null)
                {
                    String someKeyStr = someKey as String;
                    if (someKeyStr != null)
                    {
                        // compare the key strings case-insensitively
                        if (String.Compare(keyStr, someKeyStr, StringComparison.OrdinalIgnoreCase) == 0)
                            return true;

                        continue;
                    }
                }                
            
                if (key.Equals(someKey))
                    return true;
            }

            return false;
        } // Contains

        public virtual bool IsReadOnly { get { return false; } }
        public virtual bool IsFixedSize { get { return true; } } 

        // The following three methods should never be implemented because
        // they don't apply to the way IDictionary is being used in this case
        // (plus, IsFixedSize returns true.)
        public virtual void Add(Object key, Object value) { throw new NotSupportedException(); }
        public virtual void Clear() { throw new NotSupportedException(); }
        public virtual void Remove(Object key) { throw new NotSupportedException(); }
        
        public virtual IDictionaryEnumerator GetEnumerator()
        {
            return new DictionaryEnumeratorByKeys(this);
        } // GetEnumerator
                            

        //
        // end of IDictionary implementation 
        //

        //
        // ICollection implementation 
        //

         //ICollection

        public virtual void CopyTo(Array array, int index) { throw new NotSupportedException(); }

        public virtual int Count 
        {
            get 
            {
                ICollection keySet = Keys;
                if (keySet == null)
                    return 0;
                    
                return keySet.Count; 
            }
        } // Count
        
        public virtual Object SyncRoot { get { return this; } }
        public virtual bool IsSynchronized { get { return false; } }

        //
        // end of ICollection implementation
        //

        //IEnumerable
        /// <internalonly/>
        IEnumerator IEnumerable.GetEnumerator()
        {
            return new DictionaryEnumeratorByKeys(this);
        }
    
    } // class BaseChannelObjectWithProperties



    // an enumerator based off of a key set
    internal class DictionaryEnumeratorByKeys : IDictionaryEnumerator
    {
        IDictionary _properties;
        IEnumerator _keyEnum;
    
        public DictionaryEnumeratorByKeys(IDictionary properties)
        {
            _properties = properties;
            _keyEnum = properties.Keys.GetEnumerator();
        } // PropertyEnumeratorByKeys

        public bool MoveNext() { return _keyEnum.MoveNext(); }        
        public void Reset() { _keyEnum.Reset(); }        
        public Object Current { get { return Entry; } }

        public DictionaryEntry Entry { get { return new DictionaryEntry(Key, Value); } }
        
        public Object Key { get { return _keyEnum.Current; } }
        public Object Value { get { return _properties[Key]; } }       
        
    } // DictionaryEnumeratorByKeys


    // combines multiple dictionaries into one
    //   (used for channel sink properties
    internal class AggregateDictionary : IDictionary
    {
        private ICollection _dictionaries;
            
        public AggregateDictionary(ICollection dictionaries)
        { 
            _dictionaries = dictionaries;
        } // AggregateDictionary  

        // 
        // IDictionary implementation        
        //

        public virtual Object this[Object key]
        {
            get 
            {
                foreach (IDictionary dict in _dictionaries)
                {
                    if (dict.Contains(key))
                        return dict[key];
                }
            
                return null; 
            }
                
            set
            {
                foreach (IDictionary dict in _dictionaries)
                {
                    if (dict.Contains(key))
                        dict[key] = value;
                }
            } 
        } // Object this[Object key]

        public virtual ICollection Keys 
        {
            get
            {
                ArrayList keys = new ArrayList();
                // add keys from every dictionary
                foreach (IDictionary dict in _dictionaries)
                {
                    ICollection dictKeys = dict.Keys;
                    if (dictKeys != null)
                    {
                        foreach (Object key in dictKeys)
                        {
                            keys.Add(key);
                        }
                    }
                }

                return keys;
            }
        } // Keys
        
        public virtual ICollection Values
        {
            get
            {
                ArrayList values = new ArrayList();
                // add values from every dictionary
                foreach (IDictionary dict in _dictionaries)
                {
                    ICollection dictValues = dict.Values;
                    if (dictValues != null)
                    {
                        foreach (Object value in dictValues)
                        {
                            values.Add(value);
                        }
                    }
                }

                return values;
            }
        } // Values

        public virtual bool Contains(Object key) 
        {
            foreach (IDictionary dict in _dictionaries)
            {
                if (dict.Contains(key))
                    return true;
            }
            
            return false; 
        } // Contains

        public virtual bool IsReadOnly { get { return false; } }
        public virtual bool IsFixedSize { get { return true; } } 

        // The following three methods should never be implemented because
        // they don't apply to the way IDictionary is being used in this case
        // (plus, IsFixedSize returns true.)
        public virtual void Add(Object key, Object value) { throw new NotSupportedException(); }
        public virtual void Clear() { throw new NotSupportedException(); }
        public virtual void Remove(Object key) { throw new NotSupportedException(); }
        
        public virtual IDictionaryEnumerator GetEnumerator()
        {
            return new DictionaryEnumeratorByKeys(this);
        } // GetEnumerator
                            

        //
        // end of IDictionary implementation 
        //

        //
        // ICollection implementation 
        //

        //ICollection

        public virtual void CopyTo(Array array, int index) { throw new NotSupportedException(); }

        public virtual int Count 
        {
            get 
            {
                int count = 0;
            
                foreach (IDictionary dict in _dictionaries)
                {
                    count += dict.Count;
                }

                return count;
            }
        } // Count
        
        public virtual Object SyncRoot { get { return this; } }
        public virtual bool IsSynchronized { get { return false; } }

        //
        // end of ICollection implementation
        //

        //IEnumerable
        IEnumerator IEnumerable.GetEnumerator()
        {
            return new DictionaryEnumeratorByKeys(this);
        }
    
    } // class AggregateDictionary


    
} // namespace System.Runtime.Remoting

//------------------------------------------------------------------------------
// <copyright file="WebRequestModuleElementCollection.cs" company="Microsoft Corporation">
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

namespace System.Net.Configuration
{

    using System;
    using System.Configuration;
    using System.Security.Permissions;

    [ConfigurationCollection(typeof(WebRequestModuleElement))]
    public sealed class WebRequestModuleElementCollection : ConfigurationElementCollection
    {
        public WebRequestModuleElementCollection() 
        {
        }
        
        public WebRequestModuleElement this[int index]
        {
            get
            {
                return (WebRequestModuleElement)BaseGet(index);
            }
            set
            {
                if (BaseGet(index) != null)
                {
                    BaseRemoveAt(index);
                }
                BaseAdd(index,value);
            }
        }
         
        public new WebRequestModuleElement this[string name]
        {
            get
            {
                return (WebRequestModuleElement)BaseGet(name);
            }
            set
            {
                if (BaseGet(name) != null)
                {
                    BaseRemove(name);
                }
                BaseAdd(value);
            }
        }
         
        public void Add(WebRequestModuleElement element)
        {
            BaseAdd(element);
        }

        public void Clear()
        {
            BaseClear();
        }

        protected override ConfigurationElement CreateNewElement() 
        {
            return new WebRequestModuleElement();
        }

        protected override Object GetElementKey(ConfigurationElement element) 
        {
            if (element == null)
                throw new ArgumentNullException("element");
            return ((WebRequestModuleElement)element).Key;
        }

        public int IndexOf(WebRequestModuleElement element)
        {
            return BaseIndexOf(element);
        }
         
        public void Remove(WebRequestModuleElement element) 
        {
            if (element == null)
                throw new ArgumentNullException("element");
            BaseRemove(element.Key);
        }

        public void Remove(string name) 
        {
            BaseRemove(name);
        }

        public void RemoveAt(int index)
        {
            BaseRemoveAt(index);
        }

    } 

}



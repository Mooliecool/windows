/****************************** Module Header ******************************\
* Module Name:	TableStoragePagingUtility.cs
* Project:		AzureTableStoragePaging
* Copyright (c) Microsoft Corporation.
* 
* This class can be reused for other applications. If you want 
* to reuse the code, what you need to do is to implement custom ICachedDataProvider<T> 
* class to store data required by TableStoragePagingUtility<T>. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using Microsoft.WindowsAzure;
using Microsoft.WindowsAzure.StorageClient;
using System.Web.Mvc;

namespace MvcWebRole.Utilities
{
    public class TableStoragePagingUtility<T>
    {
        CloudStorageAccount _cloudStorageAccount;
        ICachedDataProvider<T> _provider;
        TableServiceContext _tableServiceContext;
        private ResultContinuation RC { get { return _provider.GetResultContinuation(); } set { _provider.SetResultContinuation(value); } }
        string _entitySetName;
        public int CurrentPageIndex { get { return _provider.GetCurrentIndex(); } private set { _provider.SetCurrentIndex(value); } }
        public int PageSize { get; private set; }
        public TableStoragePagingUtility(ICachedDataProvider<T> provider, CloudStorageAccount cloudStorageAccount, TableServiceContext tableServiceContext, int pageSize, string entitySetName)
        {
            this._provider = provider;
            this._cloudStorageAccount = cloudStorageAccount;
            this._entitySetName = entitySetName;
            this._tableServiceContext = tableServiceContext;
            if (pageSize <= 0) { throw new IndexOutOfRangeException("pageSize out of range"); }
            this.PageSize = pageSize;

        }
        /// <summary>
        /// Get the next page
        /// </summary>
        /// <returns>The next page. If current page is the last page it returns the last page.</returns>
        public IEnumerable<T> GetNextPage()
        {
            // Get cached data
            var cachedData = this._provider.GetCachedData();
            int pageCount = 0;
            if (cachedData != null)
            {
                pageCount = Convert.ToInt32(Math.Ceiling((double)cachedData.Count() / (double)this.PageSize));
            }
            // If there still has entities in table storage to read and the current page is the last page,
            // request table storage to get new data.
            if (!this._provider.HasReachedEnd && CurrentPageIndex == pageCount - 1)
            {
                var q = this._tableServiceContext.CreateQuery<T>(this._entitySetName).Take(PageSize).AsTableServiceQuery();
                IAsyncResult r;
                r = q.BeginExecuteSegmented(RC, (ar) => { }, q);
                r.AsyncWaitHandle.WaitOne();
                ResultSegment<T> result = q.EndExecuteSegmented(r);
                var results = result.Results;
                this._provider.AddDataToCache(results);
                // If there's any entity returns we need to increase pageCount
                if (results.Count() > 0)
                {
                    pageCount++;
                }
                RC = result.ContinuationToken;
                // If the returned token is null it means there's no more entities in table
                if (result.ContinuationToken == null)
                {
                    this._provider.HasReachedEnd = true;
                }
            }
            CurrentPageIndex = CurrentPageIndex + 1 < pageCount ? CurrentPageIndex + 1 : pageCount - 1;
            if (cachedData == null)
            {
                cachedData = this._provider.GetCachedData();
            }
            return cachedData.Skip((this.CurrentPageIndex) * this.PageSize).Take(this.PageSize);

        }
        /// <summary>
        /// Get the previous page
        /// </summary>
        /// <returns>The previous page. If current page is the first page it returns the first page. If there's no data in cache,
        /// returns an empty collection/></returns>
        public IEnumerable<T> GetPreviousPage()
        {

            var cachedData = this._provider.GetCachedData();
            if (cachedData != null && cachedData.Count() > 0)
            {
                CurrentPageIndex = CurrentPageIndex - 1 < 0 ? 0 : CurrentPageIndex - 1;
                return cachedData.Skip(this.CurrentPageIndex * this.PageSize).Take(this.PageSize);
            }
            else { return new List<T>(); }

        }
        /// <summary>
        /// If there're entities cached, return the current page. Else retrieve table storage and
        /// return the first page.
        /// </summary>
        /// <returns>The current page if there're entities cached. If there's no data cached first
        /// page will be retrieved from table storage and returned.</returns>
        public IEnumerable<T> GetCurrentOrFirstPage()
        {
            var cachedData = this._provider.GetCachedData();
            if (cachedData != null && cachedData.Count() > 0)
            {
                return cachedData.Skip(this.CurrentPageIndex * this.PageSize).Take(this.PageSize);
            }
            else
            {
                return GetNextPage();
            }
        }
    }

    /// <summary>
    /// The class implements this interface must take the responsibility of cache storage, including
    /// data, ResultContinuation and HasReachedEnd flag.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public interface ICachedDataProvider<T>
    {
        /// <summary>
        /// Return all cached data
        /// </summary>
        /// <returns></returns>
        IEnumerable<T> GetCachedData();
        /// <summary>
        /// Save data to cache
        /// </summary>
        /// <param name="data">Data that user of this provider wants to add to cache</param>
        void AddDataToCache(IEnumerable<T> data);
        /// <summary>
        /// Save Current index
        /// </summary>
        /// <param name="index">Current page index sent from user of this provider</param>
        void SetCurrentIndex(int index);
        /// <summary>
        /// Get Current index
        /// </summary>
        /// <returns>Current page index preserved in cache</returns>
        int GetCurrentIndex();
        /// <summary>
        /// Set continuation token
        /// </summary>
        /// <param name="rc">ResultContinuation sent from user of this provider</param>
        void SetResultContinuation(ResultContinuation rc);
        /// <summary>
        /// Get continuation token
        /// </summary>
        /// <returns>ResultContinuation preserved in cache</returns>
        ResultContinuation GetResultContinuation();
        /// <summary>
        /// A flag tells the user of this provider whether he can fully rely on cache without
        /// the need to fetch new data from table storage.
        /// </summary>
        bool HasReachedEnd { get; set; }
    }

    /// <summary>
    /// A sample implementation of ICachedDataProvider<T> that caches data in session for MVC
    /// applications. Because the implementation of it uses Session of MVC the user of this class
    /// need to be educated not to use reserved keywords of this class in their other session variables,
    /// (such as one starts with "currentindex"). If they have to use it they can specify a special id
    /// to distinguish them.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class MVCSessionCachedDataProvider<T> : ICachedDataProvider<T>
    {
        HttpSessionStateBase _session;
        string _id;
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="c">Generally specify this for this parameter</param>
        /// <param name="id">The id of the cache provider. You need to use the same id to access
        /// the same cache store</param>
        public MVCSessionCachedDataProvider(Controller c, string id)
        {
            _session = c.Session;
            _id = id;
            // Initialize currentindex
            if (_session["currentindex" + this._id] == null) { _session["currentindex" + this._id] = -1; }
            // Initialize hasreachedend flag to indicate whether there's no need to retrieve new data
            if (_session["hasreachedend" + this._id] == null) { _session["hasreachedend" + this._id] = false; }
        }
        public IEnumerable<T> GetCachedData()
        {
            return _session["inmemorycacheddata" + this._id] as List<T>;
        }
        public void AddDataToCache(IEnumerable<T> data)
        {
            var inmemorycacheddata = _session["inmemorycacheddata" + this._id] as List<T>;
            if (inmemorycacheddata == null)
            {
                inmemorycacheddata = new List<T>();
            }
            inmemorycacheddata.AddRange(data);
            _session["inmemorycacheddata" + this._id] = inmemorycacheddata;
        }
        public void SetCurrentIndex(int index)
        {
            _session["currentindex" + this._id] = index;
        }
        public int GetCurrentIndex()
        {
            return Convert.ToInt32(_session["currentindex" + this._id]);
        }
        public ResultContinuation GetResultContinuation()
        {
            return _session["resultcontinuation" + this._id] as ResultContinuation;
        }
        public void SetResultContinuation(ResultContinuation rc)
        {
            _session["resultcontinuation" + this._id] = rc;
        }

        public bool HasReachedEnd
        {
            get
            {
                return Convert.ToBoolean(_session["hasreachedend" + this._id]);
            }
            set
            {
                _session["hasreachedend" + this._id] = value;
            }
        }


    }

}
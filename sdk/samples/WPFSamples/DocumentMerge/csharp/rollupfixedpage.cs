// DocumentMerge SDK Sample - RollupFixedPage.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Documents;

namespace SdkSample
{
    /// <summary>
    /// Maintains data needed to generate a PageConent to be added to a RollUpDocument
    /// </summary>
    public class RollUpFixedPage
    {
        public RollUpFixedPage(FixedPage page)
        {
            _fixedPage = page;
        }

        public RollUpFixedPage(Uri source, Uri baseUri)
        {
            _source = source;
            _baseUri = baseUri;
        }
        private Uri _source;
        private Uri _baseUri;
        private FixedPage _fixedPage;

        public FixedPage FixedPage
        {
            get { return _fixedPage; }
            set { _fixedPage = value; }
        }

        public Uri BaseUri
        {
            get { return _baseUri; }
            set { _baseUri = value; }
        }

        public Uri Source
        {
            get { return _source; }
            set { _source = value; }
        }
    }
}

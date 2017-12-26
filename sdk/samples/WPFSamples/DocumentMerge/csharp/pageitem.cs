// DocumentMerge SDK Sample - PageItem.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Documents;
using System.Windows.Media;

namespace SdkSample
{
    class PageItem
    {
        public PageItem(PageContent pageContent)
        {
            _pageContent = pageContent;
            _fixedPage = _pageContent.GetPageRoot(false);
            _pageBrush = new VisualBrush(_fixedPage);
        }
        public PageItem(PageItem pageItem)
        {
            _pageContent = pageItem.PageContent;
            _fixedPage = pageItem.FixedPage;
            _pageBrush = new VisualBrush(_fixedPage);
        }


        public Brush PageBrush
        {
            get
            {
                return _pageBrush;
            }
        }

        public PageContent PageContent
        {
            get
            {
                return _pageContent;
            }
        }

        public FixedPage FixedPage
        {
            get
            {
                return _fixedPage;
            }
        }

        #region private data
        FixedPage _fixedPage;
        PageContent _pageContent;
        Brush _pageBrush;
        #endregion private data
    }
}

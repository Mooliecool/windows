// DocumentMerge SDK Sample - DocumentItem.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Documents;
using System.Windows.Media;

namespace SdkSample
{
    public class DocumentItem
    {
        public DocumentItem( string documentPath, DocumentReference documentReference)
        {
            _documentPath = documentPath;
            _documentReference = documentReference;
            _fixedDocument = documentReference.GetDocument(true);
            IEnumerator<PageContent> enumerator = _fixedDocument.Pages.GetEnumerator();
            enumerator.MoveNext();
            _firstPage = enumerator.Current.GetPageRoot(true);
        }
        public DocumentItem(DocumentItem documentItem)
        {
            _firstPage = documentItem._firstPage;
            _documentPath = documentItem.DocumentPath;
            _documentReference = documentItem.DocumentReference;
            _fixedDocument = documentItem.FixedDocument;
        }

        public DocumentItem(FixedPage fixedPage)
        {
            _firstPage = fixedPage;
        }

        #region Public properties
        public string Info
        {
            get
            {
                return System.IO.Path.GetFileName(_documentPath);
            }
        }

        public string TooltipString
        {
            get
            {
                return _documentPath;
            }
        }

        public Brush PageBrush
        {
            get
            {
                if (_pageBrush == null)
                {
                    _pageBrush = new VisualBrush(_firstPage);
                }
                return _pageBrush;
            }
            set
            {
                _pageBrush = value;
            }
        }

        public DocumentReference DocumentReference
        {
            get
            {
                return _documentReference;
            }
        }

        public FixedDocument FixedDocument
        {
            get
            {
                return _fixedDocument;
            }
        }
        public string DocumentPath
        {
            get
            {
                return _documentPath;
            }
        }

        #endregion
        #region priavate data
        DocumentReference _documentReference;
        string _documentPath;
        FixedDocument _fixedDocument;
        FixedPage _firstPage;
        Brush _pageBrush;
        #endregion private data
    }
}

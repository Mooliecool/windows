// DocumentMerge SDK Sample - RollUpFixedDocument.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Documents;
using System.Windows.Markup;

namespace SdkSample
{
    /// <summary>
    /// Maintains data needed to generate a DocumentReference to be added to a RollUpDocument
    /// </summary>
    public class RollUpFixedDocument
    {
        public RollUpFixedDocument()
        {
            Pages = new List<RollUpFixedPage>();
        }

        public RollUpFixedDocument(FixedDocument fixedDocument)
            : this()
        {
            _fixedDocument = fixedDocument;
        }

        public RollUpFixedDocument(Uri source, Uri baseUri)
            :this()
        {
            _source = source;
            _baseUri = baseUri;
        }

        private Uri _source;
        private Uri _baseUri;
        private List<RollUpFixedPage> _pages;
        private FixedDocument _fixedDocument;

        public FixedDocument FixedDocument
        {
            get { return _fixedDocument; }
            set { _fixedDocument = value; }
        }

        public List<RollUpFixedPage> Pages
        {
            get { return _pages; }
            set { _pages = value; }
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


        internal void CreatePagesFromSource()
        {
            DocumentReference documentReference = new DocumentReference();
            documentReference.Source = Source;
            (documentReference as IUriContext).BaseUri = BaseUri;
            FixedDocument fixedDocument = documentReference.GetDocument(true);
            PrivateCreatePagesFromFixedDocument(fixedDocument);
        }

        internal void CreatePagesFromFixedDocument()
        {
            PrivateCreatePagesFromFixedDocument(_fixedDocument);
        }

        private void PrivateCreatePagesFromFixedDocument(FixedDocument fixedDocument)
        {
            foreach (PageContent pageContent in fixedDocument.Pages)
            {
                if ((pageContent as IUriContext).BaseUri != null)
                {
                    Pages.Add(new RollUpFixedPage(pageContent.Source, (pageContent as IUriContext).BaseUri));
                }
                else
                {
                    Pages.Add(new RollUpFixedPage(pageContent.Child));
                }
            }
            Source = null;
            BaseUri = null;
        }
    }
}

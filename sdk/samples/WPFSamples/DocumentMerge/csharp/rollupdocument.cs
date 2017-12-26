// DocumentMerge SDK Sample - RollUpDocument.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Documents;
using System.Windows.Markup;
using System.Windows.Xps.Packaging;
using System.Windows.Xps;

namespace SdkSample
{
    public  class RollUpDocument
    {
        public RollUpDocument()
        {
            _documents = new List<RollUpFixedDocument>();
        }

        public int AddDocument()
        {
            _documents.Add(new RollUpFixedDocument());
            _fixedDocumentSequence = null;
            return DocumentCount - 1;
        }

        public int AddDocument(FixedDocument fixedDocument)
        {
            _documents.Add(new RollUpFixedDocument(fixedDocument));
            _fixedDocumentSequence = null;
            return DocumentCount - 1;
        }

        public int AddDocument(Uri source, Uri uri)
        {
            _documents.Add(new RollUpFixedDocument(source, uri));
            _fixedDocumentSequence = null;
            return DocumentCount - 1;
        }

        public void InsertDocument(int insertAfterDocIndex)
        {
            _documents.Insert(insertAfterDocIndex, new RollUpFixedDocument());
            _fixedDocumentSequence = null;
        }

        public void InsertDocument(int insertAfterDocIndex, FixedDocument fixedDocument)
        {
            _documents.Insert(insertAfterDocIndex, new RollUpFixedDocument(fixedDocument));
            _fixedDocumentSequence = null;
        }

        public void InsertDocument(int insertAfterDocIndex, Uri source, Uri uri)
        {
            _documents.Insert(insertAfterDocIndex, new RollUpFixedDocument(source, uri));
            _fixedDocumentSequence = null;
        }

        public void RemoveDocument(int docIndex)
        {
            _documents.RemoveAt(docIndex);
            _fixedDocumentSequence = null;
        }

        public void AddPage(int docIndex, FixedPage page)
        {
            RollUpFixedDocument rollUpFixedDocument = _documents[docIndex];
            TestForExistingPages(rollUpFixedDocument);
            rollUpFixedDocument.Pages.Add(new RollUpFixedPage(page));
            _fixedDocumentSequence = null;
        }

        private void TestForExistingPages(RollUpFixedDocument rollUpFixedDocument)
        {
            if (rollUpFixedDocument.BaseUri != null)
            {
                rollUpFixedDocument.CreatePagesFromSource();
            }
            else if (rollUpFixedDocument.FixedDocument != null)
            {
                rollUpFixedDocument.CreatePagesFromFixedDocument();
            }
        }

        public void AddPage(int docIndex, Uri source, Uri uri)
        {
            RollUpFixedDocument rollUpFixedDocument = _documents[docIndex];
            TestForExistingPages(rollUpFixedDocument);
            rollUpFixedDocument.Pages.Add(new RollUpFixedPage(source, uri));
            _fixedDocumentSequence = null;
        }

        public void InsertPage(int docIndex, int insertAfterPageIndex, FixedPage page)
        {
            RollUpFixedDocument rollUpFixedDocument = _documents[docIndex];
            TestForExistingPages(rollUpFixedDocument);
            rollUpFixedDocument.Pages.Insert(docIndex, new RollUpFixedPage(page));
            _fixedDocumentSequence = null;
        }

        public void InsertPage(int docIndex, int insertAfterPageIndex, Uri source, Uri uri)
        {
            RollUpFixedDocument rollUpFixedDocument = _documents[docIndex];
            TestForExistingPages(rollUpFixedDocument);
            rollUpFixedDocument.Pages.Insert(insertAfterPageIndex, new RollUpFixedPage(source, uri));
            _fixedDocumentSequence = null;
        }

        public void RemovePage(int docIndex, int pageIndex)
        {
            RollUpFixedDocument rollUpFixedDocument = _documents[docIndex];
            TestForExistingPages(rollUpFixedDocument);
            rollUpFixedDocument.Pages.RemoveAt(pageIndex);
            _fixedDocumentSequence = null;
        }

        public int GetPageCount(int docIndex)
        {
            RollUpFixedDocument fixedDocument =_documents[docIndex];
            if( fixedDocument.BaseUri != null )
            {
                fixedDocument.CreatePagesFromSource();
            }
            return fixedDocument.Pages.Count;
        }

        public PageContent GetPage(int docIndex, int pageIndex)
        {
            RollUpFixedDocument fixedDocument = _documents[docIndex];
            if (fixedDocument.BaseUri != null)
            {
                fixedDocument.CreatePagesFromSource();
            }

            PageContent pageContent = new PageContent();
            RollUpFixedPage fixedPage = fixedDocument.Pages[pageIndex];
            pageContent.Source = fixedPage.Source;
            (pageContent as IUriContext).BaseUri = fixedPage.BaseUri;
            return pageContent;
        }

        public void Save()
        {
            if (Uri == null)
            {
                throw new ArgumentException("Uri has not been specified");
            }

            XpsDocument xpsDocument = new XpsDocument(Uri.OriginalString, System.IO.FileAccess.ReadWrite);
            XpsDocumentWriter xpsDocumentWriter = XpsDocument.CreateXpsDocumentWriter(xpsDocument);
            xpsDocumentWriter.Write(FixedDocumentSequence);
            xpsDocument.Close();
        }

        public Uri Uri
        {
            get
            {
                return _uri;
            }

            set
            {
                _uri = value;
            }
        }
        public int DocumentCount
        {
            get
            {
                return _documents.Count;
            }
        }

        public FixedDocumentSequence FixedDocumentSequence
        {
            get
            {
                if (_fixedDocumentSequence == null)
                {
                    ReCreateFixedDocumentSequence();
                }
                return _fixedDocumentSequence;
            }
        }

        private void ReCreateFixedDocumentSequence()
        {
            _fixedDocumentSequence = new FixedDocumentSequence();
            foreach (RollUpFixedDocument document in _documents)
            {
                DocumentReference documentReference = new DocumentReference();
                FillDocumentReference(documentReference, document);
                FixedDocument fixedDocument = documentReference.GetDocument(true);
                _fixedDocumentSequence.References.Add(documentReference);
            }
        }

        private void FillDocumentReference(DocumentReference documentReference, RollUpFixedDocument document)
        {
            if (document.BaseUri != null)
            {
                documentReference.Source = document.Source;
                (documentReference as IUriContext).BaseUri = document.BaseUri;
            }
            else if (document.FixedDocument != null)
            {
                documentReference.SetDocument(document.FixedDocument);
            }
            else
            {
                AddPages(documentReference, document);
            }
        }

        private void AddPages(DocumentReference documentReference, RollUpFixedDocument document)
        {
            FixedDocument fixedDocument = new FixedDocument();
            documentReference.SetDocument(fixedDocument);
            foreach (RollUpFixedPage page in document.Pages)
            {
                PageContent pageContent = new PageContent();
                if (page.BaseUri == null)
                {
                    (pageContent as IAddChild).AddChild(page.FixedPage);
                }
                else
                {
                    pageContent.Source = page.Source;
                    (pageContent as IUriContext).BaseUri = page.BaseUri;
                }
                pageContent.GetPageRoot(true);
                fixedDocument.Pages.Add(pageContent);
            }
        }

        private Uri _uri;
        private List<RollUpFixedDocument> _documents;
        private FixedDocumentSequence _fixedDocumentSequence;

    }
}

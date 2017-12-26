//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.ServiceModel.Persistence;
using System.Threading;

namespace Microsoft.WorkflowServices.Samples
{
    public class FilePersistenceProviderFactory : PersistenceProviderFactory
    {
        const string InstanceFormatString = "{0}.bin";
        const string SubDirectoryName = "FilePersistenceProvider";
        const string SubStateFormatString = "{0}-{1}.bin";

        BinaryFormatter formatter;

        public FilePersistenceProviderFactory()
        {
            formatter = new BinaryFormatter();
        }

        protected override TimeSpan DefaultCloseTimeout
        {
            get { return TimeSpan.FromSeconds(15); }
        }

        protected override TimeSpan DefaultOpenTimeout
        {
            get { return TimeSpan.FromSeconds(15); }
        }

        public override PersistenceProvider CreateProvider(Guid id)
        {
            return new FilePersistenceProvider(id, this);
        }

        protected override void OnAbort()
        {
        }

        protected override IAsyncResult OnBeginClose(TimeSpan timeout, AsyncCallback callback, object state)
        {
            NoOpAsyncResult result = new NoOpAsyncResult(state);

            if (callback != null)
            {
                callback(result);
            }

            return result;
        }

        protected override IAsyncResult OnBeginOpen(TimeSpan timeout, AsyncCallback callback, object state)
        {
            NoOpAsyncResult result = new NoOpAsyncResult(state);

            if (callback != null)
            {
                callback(result);
            }

            return result;
        }

        protected override void OnClose(TimeSpan timeout)
        {
        }

        protected override void OnEndClose(IAsyncResult result)
        {
        }

        protected override void OnEndOpen(IAsyncResult result)
        {
        }

        protected override void OnOpen(TimeSpan timeout)
        {
        }

        IAsyncResult BeginDelete(Guid id, TimeSpan timeout, AsyncCallback callback, object state)
        {
            Delete(id, timeout);

            NoOpAsyncResult result = new NoOpAsyncResult(state);

            if (callback != null)
            {
                callback(result);
            }

            return result;
        }

        IAsyncResult BeginLoad(Guid id, TimeSpan timeout, AsyncCallback callback, object state)
        {
            object toReturn = Load(id, timeout);

            NoOpAsyncResult result = new NoOpAsyncResult(state, toReturn);

            if (callback != null)
            {
                callback(result);
            }

            return result;
        }

        IAsyncResult BeginCreate(Guid id, object instance, TimeSpan timeout, AsyncCallback callback, object state)
        {
            Create(id, instance, timeout);

            NoOpAsyncResult result = new NoOpAsyncResult(state);

            if (callback != null)
            {
                callback(result);
            }

            return result;
        }

        IAsyncResult BeginUpdate(Guid id, object instance, TimeSpan timeout, AsyncCallback callback, object state)
        {
            Update(id, instance, timeout);

            NoOpAsyncResult result = new NoOpAsyncResult(state);

            if (callback != null)
            {
                callback(result);
            }

            return result;
        }

        void Delete(Guid identifier, TimeSpan timeout)
        {
            string fName = GetFileName(identifier);

            string newFileName = Path.Combine(Path.GetDirectoryName(fName), "DELETED-" + Path.GetFileName(fName));

            File.Move(fName, newFileName);
        }

        void EndDelete(IAsyncResult result)
        {
        }

        object EndLoad(IAsyncResult result)
        {
            return ((NoOpAsyncResult)result).ToReturn;
        }

        void EndCreate(IAsyncResult result)
        {
        }

        void EndUpdate(IAsyncResult result)
        {
        }

        private string GetDirectory()
        {
            string dirName = Path.Combine(Path.GetTempPath(), SubDirectoryName);

            if (!Directory.Exists(dirName))
            {
                Directory.CreateDirectory(dirName);
            }

            return dirName;
        }

        private string GetFileName(Guid identifier)
        {
            return Path.Combine(GetDirectory(), string.Format(InstanceFormatString, identifier));
        }

        private string GetFileName(Guid identifier, int subStateId)
        {
            return Path.Combine(GetDirectory(), string.Format(SubStateFormatString, identifier, subStateId));
        }

        object Load(Guid identifier, TimeSpan timeout)
        {
            string fName = GetFileName(identifier);

            using (FileStream inFile = new FileStream(fName, FileMode.Open))
            {
                return formatter.Deserialize(inFile);
            }
        }

        void Create(Guid identifier, object toSave, TimeSpan timeout)
        {
            string fName = GetFileName(identifier);

            using (FileStream outFile = new FileStream(fName, FileMode.Create))
            {
                formatter.Serialize(outFile, toSave);
            }
        }

        void Update(Guid identifier, object toSave, TimeSpan timeout)
        {
            string fName = GetFileName(identifier);

            using (FileStream outFile = new FileStream(fName, FileMode.Create))
            {
                formatter.Serialize(outFile, toSave);
            }
        }

        class FilePersistenceProvider : PersistenceProvider
        {
            FilePersistenceProviderFactory factory;

            public FilePersistenceProvider(Guid id, FilePersistenceProviderFactory factory)
                : base(id)
            {
                this.factory = factory;
            }

            protected override TimeSpan DefaultCloseTimeout
            {
                get { return TimeSpan.FromSeconds(15); }
            }

            protected override TimeSpan DefaultOpenTimeout
            {
                get { return TimeSpan.FromSeconds(15); }
            }

            public override IAsyncResult BeginDelete(object instance, TimeSpan timeout, AsyncCallback callback, object state)
            {
                base.ThrowIfDisposedOrNotOpen();
                return this.factory.BeginDelete(this.Id, timeout, callback, state);
            }

            public override IAsyncResult BeginLoad(TimeSpan timeout, AsyncCallback callback, object state)
            {
                base.ThrowIfDisposedOrNotOpen();
                return this.factory.BeginLoad(this.Id, timeout, callback, state);
            }

            public override IAsyncResult BeginCreate(object instance, TimeSpan timeout, AsyncCallback callback, object state)
            {
                base.ThrowIfDisposedOrNotOpen();
                return this.factory.BeginCreate(this.Id, instance, timeout, callback, state);
            }

            public override IAsyncResult BeginUpdate(object instance, TimeSpan timeout, AsyncCallback callback, object state)
            {
                base.ThrowIfDisposedOrNotOpen();
                return this.factory.BeginUpdate(this.Id, instance, timeout, callback, state);
            }

            public override void Delete(object instance, TimeSpan timeout)
            {
                base.ThrowIfDisposedOrNotOpen();
                this.factory.Delete(this.Id, timeout);
            }

            public override void EndDelete(IAsyncResult result)
            {
                base.ThrowIfDisposedOrNotOpen();
                this.factory.EndDelete(result);
            }

            public override object EndLoad(IAsyncResult result)
            {
                base.ThrowIfDisposedOrNotOpen();
                return this.factory.EndLoad(result);
            }

            public override object EndCreate(IAsyncResult result)
            {
                base.ThrowIfDisposedOrNotOpen();
                this.factory.EndCreate(result);
                return null;
            }

            public override object EndUpdate(IAsyncResult result)
            {
                base.ThrowIfDisposedOrNotOpen();
                this.factory.EndUpdate(result);
                return null;
            }

            public override object Load(TimeSpan timeout)
            {
                base.ThrowIfDisposedOrNotOpen();
                return this.factory.Load(this.Id, timeout);
            }

            public override object Create(object instance, TimeSpan timeout)
            {
                base.ThrowIfDisposedOrNotOpen();
                this.factory.Create(this.Id, instance, timeout);
                return null;
            }

            public override object Update(object instance, TimeSpan timeout)
            {
                base.ThrowIfDisposedOrNotOpen();
                this.factory.Update(this.Id, instance, timeout);
                return null;
            }

            protected override void OnAbort()
            {
            }

            protected override IAsyncResult OnBeginClose(TimeSpan timeout, AsyncCallback callback, object state)
            {
                return new NoOpAsyncResult(callback, state);
            }

            protected override IAsyncResult OnBeginOpen(TimeSpan timeout, AsyncCallback callback, object state)
            {
                return new NoOpAsyncResult(callback, state);
            }

            protected override void OnClose(TimeSpan timeout)
            {
            }

            protected override void OnEndClose(IAsyncResult result)
            {
            }

            protected override void OnEndOpen(IAsyncResult result)
            {
            }

            protected override void OnOpen(TimeSpan timeout)
            {
            }
        }

        class NoOpAsyncResult : IAsyncResult
        {
            ManualResetEvent mre;
            object state;
            object toReturn;

            public NoOpAsyncResult(object state)
                : this(state, null)
            {
            }

            public NoOpAsyncResult(object state, object toReturn)
            {
                this.state = state;
                this.mre = new ManualResetEvent(true);
                this.toReturn = toReturn;
            }

            public object AsyncState
            {
                get { return state; }
            }

            public WaitHandle AsyncWaitHandle
            {
                get { return mre; }
            }

            public bool CompletedSynchronously
            {
                get { return true; }
            }

            public bool IsCompleted
            {
                get { return true; }
            }

            public object ToReturn
            {
                get { return toReturn; }
            }
        }
    }
}

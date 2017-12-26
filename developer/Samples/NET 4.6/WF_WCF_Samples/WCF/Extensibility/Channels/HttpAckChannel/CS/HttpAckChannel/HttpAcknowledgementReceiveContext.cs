//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Net;
using System.ServiceModel.Channels;
using System.Threading;
using System.Transactions;

namespace Microsoft.Samples.HttpAckChannel
{
    class HttpAcknowledgementReceiveContext : ReceiveContext
    {
        RequestContext requestContext;
        TransactionCompletedEventHandler transactionCompletedHandler;
        object internalStateLock = new object();

        public HttpAcknowledgementReceiveContext(RequestContext requestContext)
        {
            this.requestContext = requestContext;
            transactionCompletedHandler = new TransactionCompletedEventHandler(OnTransactionCompleted);
        }

        protected override void OnAbandon(TimeSpan timeout)
        {
            this.SendFailureMessage(timeout);
        }
        
        protected override IAsyncResult OnBeginAbandon(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return new AbandonAsyncResult(this, timeout, callback, state);
        }

        protected override void OnEndAbandon(IAsyncResult result)
        {
            AbandonAsyncResult.End(result);
        }

        protected override void OnComplete(TimeSpan timeout)
        {
            if (Transaction.Current == null)
            {
                this.SendSuccessMessage(timeout);
            }
            else
            {
                Transaction.Current.TransactionCompleted += this.transactionCompletedHandler;
            }
        }

        protected override IAsyncResult OnBeginComplete(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return new CompleteAsyncResult(this, timeout, callback, state);
        }

        protected override void OnEndComplete(IAsyncResult result)
        {
            if (Transaction.Current == null)
            {
                CompleteAsyncResult.End(result);
            }
            else
            {
                Transaction.Current.TransactionCompleted += this.transactionCompletedHandler;
            }
        }

        private void OnTransactionCompleted(object sender, TransactionEventArgs e)
        {
            e.Transaction.TransactionCompleted -= this.transactionCompletedHandler;
            lock (this.internalStateLock)
            {
                if (e.Transaction.TransactionInformation.Status == TransactionStatus.Committed)
                {
                    // using TimeSpan.MaxValue here
                    this.SendSuccessMessage(TimeSpan.MaxValue);
                }
            }
        }

        private void SendSuccessMessage(TimeSpan timeout)
        {
            SendMessageWithHttpStatusCode(timeout, HttpStatusCode.OK);
        }

        private void SendFailureMessage(TimeSpan timeout)
        {
            SendMessageWithHttpStatusCode(timeout, HttpStatusCode.InternalServerError);
        }

        private void SendMessageWithHttpStatusCode(TimeSpan timeout, HttpStatusCode httpStatusCode)
        {
            TimeoutHelper helper = new TimeoutHelper(timeout);
            Message replyMessage = Message.CreateMessage(MessageVersion.None, "");

            HttpResponseMessageProperty httpProperty = new HttpResponseMessageProperty();
            httpProperty.StatusCode = httpStatusCode;
            httpProperty.SuppressEntityBody = true;

            replyMessage.Properties.Add(HttpResponseMessageProperty.Name, httpProperty);

            this.requestContext.Reply(replyMessage, helper.RemainingTime());
            this.requestContext.Close();
        }

        static Message CreateMessageWithHttpStatusCode(HttpStatusCode httpStatusCode)
        {
            Message replyMessage = Message.CreateMessage(MessageVersion.None, "");

            HttpResponseMessageProperty httpProperty = new HttpResponseMessageProperty();
            httpProperty.StatusCode = httpStatusCode;
            httpProperty.SuppressEntityBody = true;

            replyMessage.Properties.Add(HttpResponseMessageProperty.Name, httpProperty);
            return replyMessage;
        }
        
        class AbandonAsyncResult : AsyncResult
        {
            Message reply;
            HttpAcknowledgementReceiveContext receiver;
            static AsyncCallback onReplyComplete = new AsyncCallback(OnReplyComplete);

            public AbandonAsyncResult(HttpAcknowledgementReceiveContext receiver, TimeSpan timeout, AsyncCallback callback, object state)
                : base(callback, state)
            {
                this.receiver = receiver;
                this.reply = HttpAcknowledgementReceiveContext.CreateMessageWithHttpStatusCode(HttpStatusCode.InternalServerError);
                bool cleanupMessage = true;
                try
                {
                    IAsyncResult result = this.receiver.requestContext.BeginReply(this.reply, timeout, onReplyComplete, this);
                    if (!result.CompletedSynchronously)
                    {
                        cleanupMessage = false;
                        return;
                    }
                    this.receiver.requestContext.EndReply(result);
                }
                finally
                {
                    if (cleanupMessage)
                    {
                        this.reply.Close();
                    }
                }

                this.Complete(true);
            }

            static void OnReplyComplete(IAsyncResult result)
            {
                if (result.CompletedSynchronously)
                {
                    return;
                }

                AbandonAsyncResult thisPtr = (AbandonAsyncResult)result.AsyncState;
                Exception completeException = null;

                try
                {
                    thisPtr.receiver.requestContext.EndReply(result);
                }
                catch (Exception e)
                {
                    completeException = e;
                }
                finally
                {
                    thisPtr.reply.Close();
                }

                thisPtr.Complete(false, completeException);
            }

            public static void End(IAsyncResult result)
            {
                AsyncResult.End<AbandonAsyncResult>(result);
            }
        }

        class CompleteAsyncResult : AsyncResult
        {
            Message reply;
            HttpAcknowledgementReceiveContext receiver;
            static AsyncCallback onReplyComplete = new AsyncCallback(OnReplyComplete);

            public CompleteAsyncResult(HttpAcknowledgementReceiveContext receiver, TimeSpan timeout, AsyncCallback callback, object state)
                : base(callback, state)
            {
                this.receiver = receiver;
                this.reply = HttpAcknowledgementReceiveContext.CreateMessageWithHttpStatusCode(HttpStatusCode.OK);
                bool cleanupMessage = true;
                try
                {
                    IAsyncResult result = this.receiver.requestContext.BeginReply(this.reply, timeout, onReplyComplete, this);
                    if (!result.CompletedSynchronously)
                    {
                        cleanupMessage = false;
                        return;
                    }
                    this.receiver.requestContext.EndReply(result);
                }
                finally
                {
                    if (cleanupMessage)
                    {
                        this.reply.Close();
                    }
                }

                this.Complete(true);
            }

            static void OnReplyComplete(IAsyncResult result)
            {
                if (result.CompletedSynchronously)
                {
                    return;
                }

                CompleteAsyncResult thisPtr = (CompleteAsyncResult)result.AsyncState;
                Exception completeException = null;

                try
                {
                    thisPtr.receiver.requestContext.EndReply(result);
                }
                catch (Exception e)
                {
                    completeException = e;
                }
                finally
                {
                    thisPtr.reply.Close();
                }

                thisPtr.Complete(false, completeException);
            }

            public static void End(IAsyncResult result)
            {
                AsyncResult.End<AbandonAsyncResult>(result);
            }
        }

        class ReceiveContextAsyncResult : AsyncResult
        {
            HttpAcknowledgementReceiveContext receiver;
            TimeoutHelper timeoutHelper;
            static WaitCallback onComplete = new WaitCallback(OnComplete);
            static WaitCallback onAbandon = new WaitCallback(OnAbandon);

            public ReceiveContextAsyncResult(HttpAcknowledgementReceiveContext receiver,
                 TimeSpan timeout, AsyncCallback callback, object state)
                : base(callback, state)
            {
                this.timeoutHelper = new TimeoutHelper(timeout);
                this.receiver = receiver;
            }

            public static IAsyncResult CreateComplete(HttpAcknowledgementReceiveContext receiver, TimeSpan timeout, AsyncCallback callback, object state)
            {
                IAsyncResult receiveContextAsyncResult = new ReceiveContextAsyncResult(receiver, timeout, callback, state);
                ThreadPool.QueueUserWorkItem(onComplete, receiveContextAsyncResult);
                return receiveContextAsyncResult;
            }

            public static IAsyncResult CreateAbandon(HttpAcknowledgementReceiveContext receiver, TimeSpan timeout, AsyncCallback callback, object state)
            {
                IAsyncResult receiveContextAsyncResult = new ReceiveContextAsyncResult(receiver, timeout, callback, state);
                ThreadPool.QueueUserWorkItem(onAbandon, receiveContextAsyncResult);
                return receiveContextAsyncResult;
            }

            static void OnComplete(object parameter)
            {
                ReceiveContextAsyncResult result = parameter as ReceiveContextAsyncResult;
                result.receiver.OnComplete(result.timeoutHelper.RemainingTime());
            }

            static void OnAbandon(object parameter)
            {
                ReceiveContextAsyncResult result = parameter as ReceiveContextAsyncResult;
                result.receiver.OnAbandon(result.timeoutHelper.RemainingTime());
            }

            public static void End(IAsyncResult result)
            {
                AsyncResult.End<ReceiveContextAsyncResult>(result);
            }
        }
    }
}

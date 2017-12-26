using System;
using System.Runtime.Serialization;
using System.Transactions.Diagnostics;


namespace System.Transactions
{
    /// <summary>
    /// Summary description for TransactionException.
    /// </summary>
    [Serializable]
    public class TransactionException : System.SystemException
    {
        internal static TransactionException Create( string traceSource, string message, Exception innerException )
        {
            if ( DiagnosticTrace.Error )
            {
                TransactionExceptionTraceRecord.Trace( traceSource,
                    message
                    );
            }

            return new TransactionException( message, 
                innerException );
        }


        internal static TransactionException CreateTransactionStateException( string traceSource, Exception innerException )
        {
            return TransactionException.Create( traceSource, SR.GetString( SR.TransactionStateException ),
                innerException );
        }


        internal static Exception CreateEnlistmentStateException( string traceSource, Exception innerException )
        {
            if ( DiagnosticTrace.Error )
            {
                InvalidOperationExceptionTraceRecord.Trace( traceSource,
                    SR.GetString( SR.EnlistmentStateException )
                    );
            }

            return new InvalidOperationException( SR.GetString( SR.EnlistmentStateException ),
                innerException );
        }


        internal static Exception CreateTransactionCompletedException( string traceSource )
        {
            if ( DiagnosticTrace.Error )
            {
                InvalidOperationExceptionTraceRecord.Trace( traceSource,
                    SR.GetString( SR.TransactionAlreadyCompleted )
                    );
            }

            return new InvalidOperationException( SR.GetString( SR.TransactionAlreadyCompleted ) );
        }


        internal static Exception CreateInvalidOperationException( string traceSource, string message, Exception innerException )
        {
            if ( DiagnosticTrace.Error )
            {
                InvalidOperationExceptionTraceRecord.Trace( traceSource,
                    message
                    );
            }              

            return new InvalidOperationException( message, innerException );
        }
        

        /// <summary>
        /// 
        /// </summary>
        public TransactionException()
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        public TransactionException(
            string message
            ) : base( message )
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        /// <param name="innerException"></param>
        public TransactionException(
            string message,
            Exception innerException
            ) : base( message, innerException )
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="info"></param>
        /// <param name="context"></param>
        protected TransactionException(
            SerializationInfo info,
            StreamingContext context
            ) : base( info, context )
        {
        }

    }



    /// <summary>
    /// Summary description for TransactionAbortedException.
    /// </summary>
    [Serializable]
    public class TransactionAbortedException : TransactionException
    {
        internal static new TransactionAbortedException Create( string traceSource, string message, Exception innerException )
        {
            if ( DiagnosticTrace.Error )
            {
                TransactionExceptionTraceRecord.Trace( traceSource,
                    message
                    );
            }

            return new TransactionAbortedException( message, 
                innerException );
        }

        internal static TransactionAbortedException Create( string traceSource, Exception innerException )
        {
            return TransactionAbortedException.Create( traceSource, SR.GetString( SR.TransactionAborted ), innerException );
        }

        /// <summary>
        /// 
        /// </summary>
        public TransactionAbortedException( ) : base( SR.GetString( SR.TransactionAborted ))
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        public TransactionAbortedException(
            string message
            ) : base( message )
        {

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        /// <param name="innerException"></param>
        public TransactionAbortedException(
            string message,
            Exception innerException
            ) : base( message, innerException )
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        /// <param name="innerException"></param>
        internal TransactionAbortedException(
            Exception innerException
            ) : base( SR.GetString( SR.TransactionAborted ), innerException )
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="info"></param>
        /// <param name="context"></param>
        protected TransactionAbortedException(
            SerializationInfo info,
            StreamingContext context
            ) : base( info, context )
        {
        }
    }



    /// <summary>
    /// Summary description for TransactionInDoubtException.
    /// </summary>
    [Serializable]
    public class TransactionInDoubtException : TransactionException
    {
        internal static new TransactionInDoubtException Create( string traceSource, string message, Exception innerException )
        {
            if ( DiagnosticTrace.Error )
            {
                TransactionExceptionTraceRecord.Trace( traceSource,
                    message
                    );
            }

            return new TransactionInDoubtException( message, 
                innerException );
        }

        internal static TransactionInDoubtException Create( string traceSource, Exception innerException )
        {
            return TransactionInDoubtException.Create( traceSource, SR.GetString( SR.TransactionIndoubt ), innerException );
        }

        /// <summary>
        /// 
        /// </summary>
        public TransactionInDoubtException( ) : base( SR.GetString( SR.TransactionIndoubt ))
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        public TransactionInDoubtException(
            string message
            ) : base( message )
        {

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        /// <param name="innerException"></param>
        public TransactionInDoubtException(
            string message,
            Exception innerException
            ) : base( message, innerException )
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="info"></param>
        /// <param name="context"></param>
        protected TransactionInDoubtException(
            SerializationInfo info,
            StreamingContext context
            ) : base( info, context )
        {
        }
    }

    /// <summary>
    /// Summary description for TransactionManagerCommunicationException.
    /// </summary>
    [Serializable]
    public class TransactionManagerCommunicationException : TransactionException
    {
        internal static new TransactionManagerCommunicationException Create( string traceSource, string message, Exception innerException )
        {
            if ( DiagnosticTrace.Error )
            {
                TransactionExceptionTraceRecord.Trace( traceSource,
                    message
                    );
            }

            return new TransactionManagerCommunicationException( message, 
                innerException );
        }

        internal static TransactionManagerCommunicationException Create( string traceSource, Exception innerException )
        {
            return TransactionManagerCommunicationException.Create( traceSource, SR.GetString( SR.TransactionManagerCommunicationException ), innerException );
        }

        /// <summary>
        /// 
        /// </summary>
        public TransactionManagerCommunicationException( ) : base( SR.GetString( SR.TransactionManagerCommunicationException ))
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        public TransactionManagerCommunicationException(
            string message
            ) : base( message )
        {

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        /// <param name="innerException"></param>
        public TransactionManagerCommunicationException(
            string message,
            Exception innerException
            ) : base( message, innerException )
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="info"></param>
        /// <param name="context"></param>
        protected TransactionManagerCommunicationException(
            SerializationInfo info,
            StreamingContext context
            ) : base( info, context )
        {
        }
    }


    [Serializable]
    public class TransactionPromotionException : TransactionException
    {
        /// <summary>
        /// 
        /// </summary>
        public TransactionPromotionException() : this( SR.GetString( SR.PromotionFailed ))
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        public TransactionPromotionException(
            string message
            ) : base( message )
        {

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        /// <param name="innerException"></param>
        public TransactionPromotionException(
            string message,
            Exception innerException
            ) : base( message, innerException )
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="info"></param>
        /// <param name="context"></param>
        protected TransactionPromotionException(
            SerializationInfo info,
            StreamingContext context
            ) : base( info, context )
        {
        }
    }

}

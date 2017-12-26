using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace Client
{
	[ServiceContract]
	public interface IProcessDataWorkflowService
	{
		[OperationContract]
		ProcessDataResponse ProcessData(ProcessDataRequest request);
	}

	public interface IProcessDataWorkflowServiceChannel :IProcessDataWorkflowService, IClientChannel
	{
	}

	// The message contract.
	[MessageContract(IsWrapped = false)]
	public partial class ProcessDataRequest
	{
		[MessageBodyMember(Namespace = "http://schemas.microsoft.com/2003/10/Serialization/", Order = 0)]
		public System.Nullable<int> @int;

		public ProcessDataRequest()
		{
		}

		public ProcessDataRequest(System.Nullable<int> @int)
		{
			this.@int = @int;
		}
	}

	[MessageContract(IsWrapped = false)]
	public partial class ProcessDataResponse
	{
		[MessageBodyMember(Namespace = "http://schemas.microsoft.com/2003/10/Serialization/", Order = 0)]
		public string @string;

		public ProcessDataResponse()
		{
		}

		public ProcessDataResponse(string @string)
		{
			this.@string = @string;
		}
	}
}

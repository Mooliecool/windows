using System.Data.Services;
using System.Data.Services.Common;
using System.ServiceModel;
using System.ServiceModel.Activation;
using System.ServiceModel.Web;
using Microsoft.SqlServer.Types;

namespace AzureBingMaps.WebRole.DataService
{
    // Recommend set IncludeExceptionDetailInFaults to false for production deployments.
    // But set it to true during development to debug the application.
    [ServiceBehavior(IncludeExceptionDetailInFaults = true, AddressFilterMode = AddressFilterMode.Any)]
    [AspNetCompatibilityRequirements(RequirementsMode = AspNetCompatibilityRequirementsMode.Allowed)]
    public class TravelDataService : DataService<TravelDataServiceContext>
    {
        public static void InitializeService(DataServiceConfiguration config)
        {
            config.SetEntitySetAccessRule("*", EntitySetRights.All);
            config.SetServiceOperationAccessRule("*", ServiceOperationRights.All);
            config.DataServiceBehavior.MaxProtocolVersion = DataServiceProtocolVersion.V2;
        }

        /// <summary>
        /// Expose a custom operation from the WCF Data Services.
        /// Not used by clients in this version of the sample.
        /// </summary>
        [WebGet]
        public double DistanceBetweenPlaces(double latitude1, double latitude2, double longitude1, double longitude2)
        {
            SqlGeography geography1 = SqlGeography.Point(latitude1, longitude1, 4326);
            SqlGeography geography2 = SqlGeography.Point(latitude2, longitude2, 4326);
            return geography1.STDistance(geography2).Value;
        }
    }
}

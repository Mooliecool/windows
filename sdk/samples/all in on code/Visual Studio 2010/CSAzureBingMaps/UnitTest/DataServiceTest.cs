using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using AzureBingMaps.UnitTest.TravelDataServiceReference;

namespace AzureBingMaps.UnitTest
{
    [TestClass]
    public class DataServiceTest
    {
        [TestMethod]
        public void AddToTravelTest()
        {
            TravelDataServiceContext ctx = new TravelDataServiceContext
                (new Uri("http://localhost:28452/DataService/TravelDataService.svc"));
            Travel travel = new Travel()
            {
                PartitionKey = "Lanxing",
                RowKey = Guid.NewGuid(),
                Place = "Shanghai",
                Latitude = 31,
                Longitude = 121,
                Time = DateTime.Now
            };
            ctx.AddObject("Travels", travel);
            ctx.SaveChanges();
        }

        [TestMethod]
        public void UpdateTravelTest()
        {
            TravelDataServiceContext ctx = new TravelDataServiceContext
                (new Uri("http://localhost:28452/DataService/TravelDataService.svc"));
            Travel travel = ctx.Travels.First();
            travel.Place = "New York";
            ctx.UpdateObject(travel);
            ctx.SaveChanges();
        }

        [TestMethod]
        public void DeleteTravelTest()
        {
            TravelDataServiceContext ctx = new TravelDataServiceContext
                (new Uri("http://localhost:28452/DataService/TravelDataService.svc"));
            Travel travel = ctx.Travels.First();
            ctx.DeleteObject(travel);
            ctx.SaveChanges();
        }
    }
}

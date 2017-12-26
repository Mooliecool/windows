using AzureBingMaps.DAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;
using Microsoft.SqlServer.Types;

namespace AzureBingMaps.UnitTest
{
    
    
    /// <summary>
    ///This is a test class for TravelViewTest and is intended
    ///to contain all TravelViewTest Unit Tests
    ///</summary>
    [TestClass()]
    public class TravelEFTest
    {


        private TestContext testContextInstance;

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region Additional test attributes
        // 
        //You can use the following additional attributes as you write your tests:
        //
        //Use ClassInitialize to run code before running the first test in the class
        //[ClassInitialize()]
        //public static void MyClassInitialize(TestContext testContext)
        //{
        //}
        //
        //Use ClassCleanup to run code after all tests in a class have run
        //[ClassCleanup()]
        //public static void MyClassCleanup()
        //{
        //}
        //
        //Use TestInitialize to run code before running each test
        //[TestInitialize()]
        //public void MyTestInitialize()
        //{
        //}
        //
        //Use TestCleanup to run code after each test has run
        //[TestCleanup()]
        //public void MyTestCleanup()
        //{
        //}
        //
        #endregion


        /// <summary>
        ///A test for TravelView Constructor
        ///</summary>
        [TestMethod()]
        public void TravelViewConstructorTest()
        {
            Travel target = new Travel();
            //Assert.Inconclusive("TODO: Implement code to verify target");
        }

        [TestMethod]
        public void InsertIntoTravelTest()
        {
            TravelModelContainer ctx = new TravelModelContainer();
            var travel = CreateTravelViewTest();
            ctx.Travels.AddObject(travel);
            int result = ctx.SaveChanges();
            Assert.AreEqual(result, 1);
            ctx.Dispose();
        }

        [TestMethod]
        public void UpdateTravelTest()
        {
            TravelModelContainer ctx = new TravelModelContainer();
            Travel travel = ctx.Travels.First();
            travel.Place = "New York";
            int result = ctx.SaveChanges();
            Assert.AreEqual(result, 1);
            Travel returnedTravel = this.QueryTravelItemTest(travel.PartitionKey, travel.RowKey);
            Assert.AreEqual(returnedTravel.Place, "New York");
            ctx.Dispose();
        }

        [TestMethod]
        public void DeleteFromTravelTest()
        {
            TravelModelContainer ctx = new TravelModelContainer();
            Travel travel = ctx.Travels.First();
            ctx.Travels.DeleteObject(travel);
            int result = ctx.SaveChanges();
            Assert.AreEqual(result, 1);
            Travel returnedTravel = this.QueryTravelItemTest(travel.PartitionKey, travel.RowKey, true);
            Assert.IsNull(returnedTravel);
            ctx.Dispose();
        }

        [TestMethod]
        public Travel QueryTravelItemTest(string partitionKey, Guid rowKey, bool allowNull = false)
        {
            TravelModelContainer ctx = new TravelModelContainer();
            var query = from t in ctx.Travels where t.PartitionKey == partitionKey && t.RowKey == rowKey select t;
            Travel travelView = query.FirstOrDefault();
            if (!allowNull)
            {
                Assert.IsNotNull(travelView);
            }
            ctx.Dispose();
            return travelView;
        }

        [TestMethod]
        public void QueryFirstTravelItemTest()
        {
            TravelModelContainer ctx = new TravelModelContainer();
            var query = from t in ctx.Travels select t;
            Travel travelView = query.FirstOrDefault();
            Assert.IsNotNull(travelView);
            ctx.Dispose();
        }

        /// <summary>
        ///A test for CreateTravelView
        ///</summary>
        [TestMethod()]
        public Travel CreateTravelViewTest()
        {
            string partitionKey = "Lanxing"; // TODO: Initialize to an appropriate value
            Guid rowKey = Guid.NewGuid(); // TODO: Initialize to an appropriate value
            string place = "Shanghai"; // TODO: Initialize to an appropriate value
            DateTime time = DateTime.Now; // TODO: Initialize to an appropriate value
            SqlGeography sqlGeography = SqlGeography.Point(31, 121, 4326);
            Travel travelView = new Travel() { PartitionKey = partitionKey, RowKey = rowKey, Place = place, Time = time, GeoLocationText = sqlGeography.ToString() };
            return travelView;
        }

        /// <summary>
        ///A test for GeoLocation
        ///</summary>
        [TestMethod()]
        public void GeoLocationTest()
        {
            Travel target = new Travel(); // TODO: Initialize to an appropriate value
            byte[] expected = null; // TODO: Initialize to an appropriate value
            byte[] actual;
            target.GeoLocation = expected;
            actual = target.GeoLocation;
            Assert.AreEqual(expected, actual);
            Assert.Inconclusive("Verify the correctness of this test method.");
        }

        /// <summary>
        ///A test for PartitionKey
        ///</summary>
        [TestMethod()]
        public void PartitionKeyTest()
        {
            Travel target = new Travel(); // TODO: Initialize to an appropriate value
            string expected = string.Empty; // TODO: Initialize to an appropriate value
            string actual;
            target.PartitionKey = expected;
            actual = target.PartitionKey;
            Assert.AreEqual(expected, actual);
            Assert.Inconclusive("Verify the correctness of this test method.");
        }

        /// <summary>
        ///A test for Place
        ///</summary>
        [TestMethod()]
        public void PlaceTest()
        {
            Travel target = new Travel(); // TODO: Initialize to an appropriate value
            string expected = string.Empty; // TODO: Initialize to an appropriate value
            string actual;
            target.Place = expected;
            actual = target.Place;
            Assert.AreEqual(expected, actual);
            Assert.Inconclusive("Verify the correctness of this test method.");
        }

        /// <summary>
        ///A test for RowKey
        ///</summary>
        [TestMethod()]
        public void RowKeyTest()
        {
            Travel target = new Travel(); // TODO: Initialize to an appropriate value
            Guid expected = new Guid(); // TODO: Initialize to an appropriate value
            Guid actual;
            target.RowKey = expected;
            actual = target.RowKey;
            Assert.AreEqual(expected, actual);
        }

        /// <summary>
        ///A test for Time
        ///</summary>
        [TestMethod()]
        public void TimeTest()
        {
            Travel target = new Travel(); // TODO: Initialize to an appropriate value
            DateTime expected = new DateTime(); // TODO: Initialize to an appropriate value
            DateTime actual;
            target.Time = expected;
            actual = target.Time;
            Assert.AreEqual(expected, actual);
            Assert.Inconclusive("Verify the correctness of this test method.");
        }
    }
}

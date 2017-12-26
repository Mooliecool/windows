using System;
using System.Windows;
using System.Collections.ObjectModel;

namespace BindingStringFormatSample
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }
    }

    // The type of objects that are added to the ItemsControl.
    public class PurchaseItem
    {
        public string Description { get; set; }
        public double Price { get; set; }
        public DateTime OfferExpires { get; set; }

        public PurchaseItem()
        {
        }

        public PurchaseItem(string desc, double price, DateTime endDate)
        {
            Description = desc;
            Price = price;
            OfferExpires = endDate;
        }

        public override string ToString()
        {
            return String.Format("{0}, {1:c}, {2:D}", Description, Price, OfferExpires);
        }
    }

    // The source of the ItemsControl.
    public class ItemsForSale : ObservableCollection<PurchaseItem>
    {

        public ItemsForSale()
        {
            Add((new PurchaseItem("Snowboard and bindings", 120, new DateTime(2009, 1, 1))));
            Add((new PurchaseItem("Inside C#, second edition", 10, new DateTime(2009, 2, 2))));
            Add((new PurchaseItem("Laptop - only 1 year old", 499.99, new DateTime(2009, 2, 28))));
            Add((new PurchaseItem("Set of 6 chairs", 120, new DateTime(2009, 2, 28))));
            Add((new PurchaseItem("My DVD Collection", 15, new DateTime(2009, 1, 1))));
            Add((new PurchaseItem("TV Drama Series", 39.985, new DateTime(2009, 1, 1))));
            Add((new PurchaseItem("Squash racket", 60, new DateTime(2009, 2, 28))));

        }

    }
}

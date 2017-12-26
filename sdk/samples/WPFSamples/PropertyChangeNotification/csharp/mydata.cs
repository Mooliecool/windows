using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Collections.ObjectModel;

namespace SDKSample
{
  public class Bid: INotifyPropertyChanged
  {
    private string biditemname = "Unset";
    private decimal biditemprice = (decimal) 0;

    public Bid(string newBidItemName, decimal newBidItemPrice)
    {
      biditemname = newBidItemName;
      biditemprice = newBidItemPrice;
    }

    public string BidItemName
    {
      get
      {
        return biditemname;
      }
      set
      {
        if(biditemname.Equals(value) == false)
        {
          biditemname = value;
          // Call OnPropertyChanged whenever the property is updated
          OnPropertyChanged("BidItemName");
        }
      }
    }

    public decimal BidItemPrice
    {
      get
      {
        return biditemprice;
      }
      set
      {
        if(biditemprice.Equals(value) == false)
        {
          biditemprice = value;
          // Call OnPropertyChanged whenever the property is updated
          OnPropertyChanged("BidItemPrice");
        }
      }
    }

    // Declare event
    public event PropertyChangedEventHandler PropertyChanged;
    // OnPropertyChanged to update property value in binding
    private void OnPropertyChanged(string propName)
    {
        PropertyChangedEventHandler handler = PropertyChanged;
        if (handler !=null)
        {
            handler(this, new PropertyChangedEventArgs(propName));
        }
    }
  }

  public class BidCollection: ObservableCollection<Bid>
  {
    private Bid item1 = new Bid("Perseus Vase", (decimal)24.95);
    private Bid item2 = new Bid("Hercules Statue", (decimal)16.05);
    private Bid item3 = new Bid("Odysseus Painting", (decimal)100.0);

    private void Timer1_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
    {
      item1.BidItemPrice += (decimal) 1.25;
      item2.BidItemPrice += (decimal) 2.45;
      item3.BidItemPrice += (decimal) 10.55;
    }

    private void CreateTimer()
    {
      System.Timers.Timer Timer1 = new System.Timers.Timer();
      Timer1.Enabled = true;
      Timer1.Interval = 2000;
      Timer1.Elapsed += new System.Timers.ElapsedEventHandler(Timer1_Elapsed);
    }

    public BidCollection():base()
    {
      Add(item1);
      Add(item2);
      Add(item3);
      CreateTimer();
    }
  }
}

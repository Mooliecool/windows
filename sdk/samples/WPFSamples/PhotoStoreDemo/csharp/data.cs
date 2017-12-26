using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Media.Imaging;
using System.Collections.Specialized;
using System.Windows.Controls;


namespace PhotoDemo
{
    public class ImageFile
    {
        public ImageFile(string path)
        {
            _path = path;
            _uri = new Uri(_path);
            _image = BitmapFrame.Create(_uri);
        }

        public override string ToString()
        {
            return Path;
        }

        private String _path;
        public String Path { get { return _path; } }
        private Uri _uri;
        public Uri Uri { get { return _uri; } }
        private BitmapFrame _image;
        public BitmapFrame Image { get { return _image; } }
    }

    public class PhotoList : ObservableCollection<ImageFile>
    {
        public PhotoList() { }

        public PhotoList(string path) : this(new DirectoryInfo(path)) { }

        public PhotoList(DirectoryInfo directory)
        {
            _directory = directory;
            Update();
        }

        public string Path
        {
            set
            {
                _directory = new DirectoryInfo(value);
                Update();
            }
            get { return _directory.FullName; }
        }

        public DirectoryInfo Directory
        {
            set
            {
                _directory = value;
                Update();
            }
            get { return _directory; }
        }
        private void Update()
        {
            foreach (FileInfo f in _directory.GetFiles("*.jpg"))
            {
                Add(new ImageFile(f.FullName));
            }
        }

        DirectoryInfo _directory;
    }

    public class PrintType
    {
        public PrintType(string description, double cost)
        {
            _description = description;
            _cost = cost;
        }

        public override string ToString()
        {
            return _description;
        }

        private string _description;
        public String Description {get {return _description; }}

        private double _cost;
        public double Cost { get { return _cost; } }
    }

    public class PrintTypeList : ObservableCollection<PrintType>
    {
        public PrintTypeList()
        {
            Add(new PrintType("4x6 Print", 0.15));
            Add(new PrintType("Greeting Card", 1.49));
            Add(new PrintType("T-Shirt", 14.99));
        }
    }

    public class PrintBase : INotifyPropertyChanged
    {
        public PrintBase(BitmapSource photo, PrintType printtype, int quantity)
        {
            Photo = photo;
            PrintType = printtype;
            Quantity = quantity;
        }

        public PrintBase(BitmapSource photo, string description, double cost)
        {
            Photo = photo;
            PrintType = new PrintType(description, cost);
            Quantity = 0;
        }

        private BitmapSource _photo;
        public BitmapSource Photo
        {
            set { _photo = value; OnPropertyChanged("Photo"); }
            get { return _photo; }
        }

        private PrintType _PrintType;
        public PrintType PrintType
        {
            set { _PrintType = value; OnPropertyChanged("PrintType"); }
            get { return _PrintType; }
        }

        private int _quantity;
        public int Quantity
        {
            set { _quantity = value; OnPropertyChanged("Quantity"); }
            get { return _quantity; }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(String info)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(info));
        }

        public override string ToString()
        {
            return PrintType.ToString();
        }
    }

    public class Print : PrintBase 
    {
        public Print(BitmapSource photo) : base(photo, "4x6 Print", 0.15) { }
    }

    public class GreetingCard : PrintBase
    {
        public GreetingCard(BitmapSource photo) : base(photo, "Greeting Card", 1.49) { }
    }

    public class TShirt : PrintBase
    {
        public TShirt(BitmapSource photo) : base(photo, "T-Shirt", 14.99) { }
    }

    public class PrintList : ObservableCollection<PrintBase> { }


}

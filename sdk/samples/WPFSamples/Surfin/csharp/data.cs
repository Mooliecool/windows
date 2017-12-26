using System;
using System.Collections.ObjectModel;  // ObserverableCollection
using System.IO;  // DirectoryInfo
using System.Windows; // MessageBox

namespace DataTemplatingLab
{
    // MyVideos is a collection of MyVideo objects
    // This class has a Directory string property
    // The Update() method takes all .wmv files from the specified directory
    // and adds them as MyVideo objects into the collection
    public class MyVideos : ObservableCollection<MyVideo>
    {
        private DirectoryInfo _directory;

        public MyVideos() { }
        public MyVideos(string directory)
        {
            this.Directory = directory;
        }

        public string Directory
        {
            set
            {
                // Don't set path if directory is invalid
                if (!System.IO.Directory.Exists(value))
                {
                    MessageBox.Show("No Such Directory");
                }

                _directory = new DirectoryInfo(value);

                Update();
            }
            get { return _directory.FullName; }
        }

        private void Update()
        {
            // Don't update if no directory to get files from
            if (_directory == null) return;

            // Remove all MyVideo objects from this collection
            this.Clear();

            // Create MyVideo objects
            foreach (FileInfo f in _directory.GetFiles("*.wmv"))
            {
                Add(new MyVideo(f.FullName, f.Name));
            }
        }
    }

    // MyVideo class
    // Properties: VideoTitle and Source
    public class MyVideo
    {
        private string _name;
        private string _path;
        private Uri _source;

        public MyVideo(string path)
        {
            _path = path;
            _source = new Uri(path);
        }

        public MyVideo(string path, string name)
        {
            _name = name;
            _path = path;
            _source = new Uri(path);
        }

        public string VideoTitle
        {
            get { return _name; }
            set
            {
                if (_name != value)
                {
                    _name = value;
                }
            }
        }

        public string Source
        {
            get { return _path; }
        }
    }
}
using System.Collections.ObjectModel;

namespace TabControlUsingItemTemplate
{
    public class TabItemData
    {

        private string _header;
        private string _content;

        public TabItemData(string header, string content)
        {

            _header = header;
            _content = content;

        }

        public string Header
        {
            get { return _header; }
        }



        public string Content
        {
            get { return _content; }
        }

    }




    public class TabList : ObservableCollection<TabItemData>
    {

        public TabList()
            : base()
        {

            Add(new TabItemData("Header 1", "Content 1"));
            Add(new TabItemData("Header 2", "Content 2"));
            Add(new TabItemData("Header 3", "Content 3"));

        }

    }
}

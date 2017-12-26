using System;
using System.Collections.ObjectModel;

namespace CustomContentStateNavigationSample
{
    public class Users : ObservableCollection<User>
    {
        public Users()
        {
            this.Add(new User("Sneezy"));
            this.Add(new User("Happy"));
            this.Add(new User("Dopey"));
            this.Add(new User("Grumpy"));
            this.Add(new User("Bashful"));
            this.Add(new User("Doc"));
            this.Add(new User("Sleepy"));
        }
    }
}

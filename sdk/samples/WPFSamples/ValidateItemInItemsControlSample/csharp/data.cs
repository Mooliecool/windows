using System.Collections.ObjectModel;
using System.Text;
using System.Windows.Controls;
using System.Windows.Data;

namespace ValidateItemInItemsControlSample
{
    public class Customers : ObservableCollection<Customer>
    {
        public Customers()
        {
            Add(new Customer());
        }
    }

    public enum Region
    {
        Africa,
        Antartica,
        Australia,
        Asia,
        Europe,
        NorthAmerica,
        SouthAmerica
    }

    public class Customer
    {
        public string Name { get; set; }
        public ServiceRep ServiceRepresentative { get; set; }
        public Region Location { get; set; }
    }

    public class ServiceRep
    {
        public string Name { get; set; }
        public Region Area { get; set; }

        public ServiceRep()
        {
        }

        public ServiceRep(string name, Region area)
        {
            Name = name;
            Area = area;
        }

        public override string ToString()
        {
            return Name + " - " + Area.ToString();
        }
    }

    public class Representantives : ObservableCollection<ServiceRep>
    {
        public Representantives()
        {
            Add(new ServiceRep("Haluk Kocak", Region.Africa));
            Add(new ServiceRep("Reed Koch", Region.Antartica));
            Add(new ServiceRep("Christine Koch", Region.Asia));
            Add(new ServiceRep("Alisa Lawyer", Region.Australia));
            Add(new ServiceRep("Petr Lazecky", Region.Europe));
            Add(new ServiceRep("Karina Leal", Region.NorthAmerica));
            Add(new ServiceRep("Kelley LeBeau", Region.SouthAmerica));
            Add(new ServiceRep("Yoichiro Okada", Region.Africa));
            Add(new ServiceRep("Tülin Oktay", Region.Antartica));
            Add(new ServiceRep("Preeda Ola", Region.Asia));
            Add(new ServiceRep("Carole Poland", Region.Australia));
            Add(new ServiceRep("Idan Plonsky", Region.Europe));
            Add(new ServiceRep("Josh Pollock", Region.NorthAmerica));
            Add(new ServiceRep("Daphna Porath", Region.SouthAmerica));
        }
    }

    // Check whether the customer and service representative are in the
    // same area.
    public class AreasMatch : ValidationRule
    {
        public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
        {
            BindingGroup bg = value as BindingGroup;
            Customer cust = bg.Items[0] as Customer;

            if (cust == null)
            {
                return new ValidationResult(false, "Customer is not the source object");
            }

            Region region = (Region)bg.GetValue(cust, "Location");
            ServiceRep rep = bg.GetValue(cust, "ServiceRepresentative") as ServiceRep;
            string customerName = bg.GetValue(cust, "Name") as string;

            if (region == rep.Area)
            {
                return ValidationResult.ValidResult;
            }
            else
            {

                StringBuilder sb = new StringBuilder();
                sb.AppendFormat("{0} must be assigned a sales representative that serves the {1} region. \n ", customerName, region);
                return new ValidationResult(false, sb.ToString());
            }
        }
    }

}

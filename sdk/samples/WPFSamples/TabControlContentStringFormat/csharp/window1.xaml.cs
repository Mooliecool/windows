using System;
using System.Text;
using System.Windows;
using System.Collections.ObjectModel;

namespace TabControlContentStringFormat
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

    public class Student : IFormattable
    {

        public string Name { get; set; }
        ObservableCollection<Course> Courses { get; set; }

        public Student()
            : this("")
        {

        }

        public Student(string name)
        {
            Name = name;
            Courses = new ObservableCollection<Course>();
        }

        // Add a course to the student's schedule.
        public void AddCourse(string name, int id, string desc)
        {
            Courses.Add(new Course(name, id, desc));
        }

        public string ToString(string format, IFormatProvider formatProvider)
        {
            // 'n': print the name only.
            if (format == "n")
            {
                return Name;
            }

            // 'cl': print the course list.
            if (format == "cl")
            {
                string stringFormat = "{0,-25}{1,-30}{2,-10}\r\n";

                StringBuilder str = new StringBuilder();

                str.AppendLine();
                str.AppendFormat(stringFormat, "Title", "Description", "ID");
                str.AppendLine();

                foreach (Course c in Courses)
                {
                    str.AppendFormat(stringFormat, c.Title, c.Description, c.SectionID);
                }

                return str.ToString();
            }

            return this.ToString();
        }
    }

    public class Course
    {
        public string Title { get; set; }
        public int SectionID { get; set; }
        public string Description { get; set; }

        public Course()
        {
        }

        public Course(string title, int section, string desc)
        {
            Title = title;
            SectionID = section;
            Description = desc;
        }
    }

    public class Students : ObservableCollection<Student>
    {
        public Students()
        {
            Student s1 = new Student("Sunil Uppal");
            s1.AddCourse("Calculus 303", 19, "Advanced Calculus");
            s1.AddCourse("History 110", 35, "Introduction to World History");
            s1.AddCourse("Psychology 110", 40, "Behavioral Psychology");
            s1.AddCourse("Physical Education 204", 80, "Racquetball");
            this.Add(s1);

            Student s2 = new Student("Alice Ciccu");
            s2.AddCourse("English 200", 50, "Advanced Composition");
            s2.AddCourse("English 315", 100, "Shakespear's Sonnets");
            s2.AddCourse("History 230", 38, "European History 1000-1500");
            this.Add(s2);


            Student s3 = new Student("Jeff Price");
            s3.AddCourse("History 230", 38, "European History 1000-1500");
            s3.AddCourse("History 110", 35, "Introduction to World History");
            s3.AddCourse("Physical Education 204", 80, "Racquetball");
            this.Add(s3);


        }
    }
}

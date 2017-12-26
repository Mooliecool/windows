//------------------------------------------------------------------------------
// <copyright file="managedresources.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using System.Diagnostics;
using System.IO;
using System.Resources;
using System.Reflection;
using System.Globalization;

/// <summary>
/// Main entry point class
/// </summary>
public class ManagedResources {
  
  /// <summary>
  /// This enum specifies the possible program options.
  /// </summary>
  public enum Options {GenerateResourceFiles, GenerateTextFiles, UseResources, Invalid, None, Help}

  public const string DEFILE = "strings.de";
  public const string EN_NZFILE = "strings.en-nz";
  
  /// <summary>
  /// Entry point for sample application.
  /// The Main method gets the current command-line options by calling GetArgs().
  /// It then uses the current options to determine which method to call.
  /// </summary>
  /// <param name="args"></param>
  public static void Main(string [] args) 
  {
    //Get arguments.
    Options currentOptions = GetArgs(args);
    //Execute relevant portion of program depending on arguments.
    switch (currentOptions)
    {
      case Options.GenerateResourceFiles:
        GenerateResources gr = new GenerateResources();
        gr.ResourceFileUsage();
        gr.GenerateResourceFiles();
        gr.BuildSatelliteAssemblies();
        return;

      case Options.GenerateTextFiles:
        GenerateText g = new GenerateText();
        g.TextFileUsage();
        g.GenerateTextFiles();
        return;

      case Options.UseResources:
        UseResources u = new UseResources();
        u.Run();
        return;

      case Options.Help:
        Usage();
        return;

      case Options.None:
        goto case Options.Help;

      case Options.Invalid:  //Prompt for 
        Console.WriteLine("Invalid option, please try again.");
        Usage();
        break;

      default:
        //Should not get here.
        Console.WriteLine("Invalid option in Main()");
        return;
    }//switch
  }//Main()

  /// <summary>
  /// Outputs usage information.
  /// </summary>
  public static void Usage()
  {
    Console.WriteLine(
      "\nUsage: managedresources <option>\n" +
      "  -?   Show usage information.\n" +
      "  -h   Show usage information.\n" +
      "  -u   Demonstrate resource file usage in prompts to user.\n" +
      "  -r   Generate .resources file directly from code, build resource assemblies.\n" +
      "  -t   Generate text resource files for user to compile with resgen.exe.\n"
      );
  }//Usage()

  /// <summary>
  /// Gets command-line and returns the user-selected option.
  /// </summary>
  /// <param name="args"></param>
  /// <returns>A value of enum Options</returns>
  public static Options GetArgs(string [] args)
  {
    if (args.Length == 0)
    {
      return Options.None;
    }//if

    foreach (string arg in args)
    {
      switch (arg.ToLower())
      {
        case "-?":
        case "/?":
        case "-h":
        case "/h":
          return Options.Help;

        case "-r":
        case "/r":
          return Options.GenerateResourceFiles;

        case "-t":
        case "/t":
          return Options.GenerateTextFiles;

        case "-u":
        case "/u":
          return Options.UseResources;

        default:
          return Options.Invalid;
      }//switch
    }//foreach
    return Options.Invalid;
  }//GetArgs()

}//class ManagedResources

/// <summary>
/// This class contains methods to generate resources.
/// </summary>
public class GenerateResources
{
  const string EN_NZRESFILE = ManagedResources.EN_NZFILE + ".resources";
  const string DERESFILE = ManagedResources.DEFILE + ".resources";

  /// <summary>
  /// This method generates resource files directly using the ResourceWriter class.
  /// </summary>
  public void GenerateResourceFiles()
  {
    // Generate the New Zealand resource file.
    ResourceWriter rw = new ResourceWriter(EN_NZRESFILE);
    rw.AddResource("promptValidDeg", "Please enter a viable outside temperature (-100 to 60).");
    rw.AddResource("dist1", "(in kilometres) ==>");
    rw.AddResource("degree1", "in Celsius ==>");
    rw.AddResource("degree2", "-100");
    rw.AddResource("degree3", "60");
    rw.Generate();
    rw.Close();
    Console.WriteLine("Generating resource file: {0}", EN_NZRESFILE);

    // Generate the Germand resource file.
    rw = new ResourceWriter(DERESFILE);
    rw.AddResource("promptName", "Geben Sie Ihren Namen ein ==>");
    rw.AddResource("promptAge", "Geben Sie Ihr Alter ein ==>");
    rw.AddResource("promptDegrees", "Geben Sie die aktuelle Temperatur ein,");
    rw.AddResource("promptMissing", "Stellen Sie sicher, dass Sie einen gültigen Wert eingeben.");
    rw.AddResource("promptValidAge", "Geben Sie ein gültiges Alter für einen Erwachsenen ein (15 und älter).");    
    rw.AddResource("promptDist", "Geben Sie an, wie weit Sie zur Arbeit fahren");
    rw.AddResource("promptValidDist", "Geben Sie einen gültigen Abstand ein (größer als Null).");
    rw.AddResource("promptValidDeg", "Geben Sie eine Außentemperatur ein (-60C bis 60F).");
    rw.AddResource("promptEntries", "Sie trugen die folgenden Informationen ein:");
    rw.AddResource("dist1", "(in den Kilometern) ==>");
    rw.AddResource("degree1", "in Celsius ==>");
    rw.AddResource("degree2", "-60");
    rw.AddResource("degree3", "60");
    rw.AddResource("outputName", "Name:");
    rw.AddResource("outputAge", "Alter:");
    rw.AddResource("outputDegrees", "Temperatur:");
    rw.AddResource("outputDist", "Abstand Zur Arbeit:");
    rw.Generate();
    rw.Close();
    Console.WriteLine("Generating resource file: {0}", DERESFILE);
  }//GenerateResourceFiles()

  /// <summary>
  /// This method builds satellite assemblies by calling the RunAlink methods.
  /// It creates the required directory for finding and loading the satellite assemblies.
  /// </summary>
  public void BuildSatelliteAssemblies()
  {
    if (!Directory.Exists("en-NZ"))
    {
      Directory.CreateDirectory("en-NZ");
    }//if
    if (!Directory.Exists("de"))
    {
      Directory.CreateDirectory("de");
    }//if
   
    RunAlink("en-NZ");
    RunAlink("de");

  }//BuildSatelliteAssemblies()
  
  /// <summary>
  /// This method creates a Process object to run the assembly linker tool.
  /// </summary>
  /// <param name="culture"></param>
  public void RunAlink(string culture)
  {
    string resourceFile = "strings." + culture + ".resources";
    if (File.Exists(culture + "\\managedresources.resources.dll"))
    {
      File.Delete(culture + "\\managedresources.resources.dll");
    }//if
    string args = "/t:lib /culture:" +
                  culture +
                  " /embed:" +
                  resourceFile +
                  " /out:" +
                  culture +
                  "\\managedresources.resources.dll";
    Console.WriteLine("\nBuilding satellite: \nal {0}\n", args);
    ProcessStartInfo psi = new ProcessStartInfo("al", args);
    psi.UseShellExecute = false;
    Process alProc = Process.Start(psi);
    alProc.WaitForExit();
    if (alProc.ExitCode != 0)
    {
      Console.WriteLine("Error running al.exe with arguments {0}\n", args);
    }//if
  }//RunAlink()

  /// <summary>
  /// This method outputs the usage banner for the -r option
  /// </summary>
  public void ResourceFileUsage()
  {
    Console.WriteLine
      ("\n" +
      "*============================================================================*\n" +
      "* This application assembly was build with a set of default resources        *\n" +
      "* defined in resources.txt.                                                  *\n" +
      "*                                                                            *\n" +
      "* The -r option programmatically create .resources files and then uses       *\n" +
      "* the assembly linker al.exe to build satellite assemblies in the            *\n" +
      "* correct direcory location.                                                 *\n" +
      "*                                                                            *\n" +
      "* Selecting the -r option outputs two resource files:                        *\n" +
      "*  - " + EN_NZRESFILE + "                                                 *\n" +
      "*  - " + DERESFILE + "                                                    *\n" +
      "*                                                                            *\n" +
      "* It then builds these files into satellite assemblies using the assembly    *\n" +
      "* linker utility via a Process objcect.                                      *\n" +
      "*============================================================================*\n"
      );
  }//ResourceFileUsage()
}//class GenerateResources

/// <summary>
/// This class contains methods that generate text files containing string resources so the user
/// can manually perform the operations of running the ResGen tool to build a .resources file and 
/// then running the assembly linker to build the .resource files into satellite assemblies.
/// </summary>
public class GenerateText
{
  const string DETEXTFILE = ManagedResources.DEFILE + ".txt";
  const string EN_NZTEXTFILE = ManagedResources.EN_NZFILE + ".txt";

  /// <summary>
  /// Creates text resource files for the user to experiment with.
  /// </summary>
  public void GenerateTextFiles()
  {
    string stringsEN_NZ = 
      ";prompts\n" +
      "promptValidDeg = Please enter a viable outside temperature (-100 to 60).\n" +
      "\n" +
      ";distances\n" +
      "dist1 = (in kilometres) ==>\n" +
      "\n" +
      ";degrees measurements\n" +
      "degree1 = in Celsius ==>\n" +
      "degree2 = -100\n" +
      "degree3 = 60\n";

    StreamWriter sw = new StreamWriter(EN_NZTEXTFILE);
    sw.Write(stringsEN_NZ);
    sw.Close();
    Console.WriteLine("Text resource file {0} generated.", EN_NZTEXTFILE);

    string stringsDE = 
      ";Hinweise\n" +
      "promptName = Geben Sie Ihren Namen ein ==>\n" +
      "promptAge = Geben Sie Ihr Alter ein ==>\n" +
      "promptDegrees = Geben Sie die aktuelle Temperatur ein,\n" +
      "promptMissing = Stellen Sie sicher, dass Sie einen gültigen Wert eingeben.\n" +
      "promptValidAge = Geben Sie ein gültiges Alter für einen Erwachsenen ein (15 und älter). \n" +
      "promptDist = Geben Sie an, wie weit Sie zur Arbeit fahren\n" +
      "promptValidDist =Geben Sie einen gültigen Abstand ein (größer als Null). \n" +
      "promptValidDeg = Geben Sie eine Außentemperatur ein (-60C bis 60F).\n" +
      "promptEntries = Sie trugen die folgenden Informationen ein:\n" +
      "\n" +
      ";Abstände\n" +
      "dist1 = (in den Kilometern) ==>\n" +
      "\n" +
      ";Gradmessen\n" +
      "degree1 = in Celsius ==>\n" +
      "degree2 = -60\n" +
      "degree3 = 60\n" +
      "\n" +
      ";Ausgaben\n" +
      "outputName = Name:\n" +
      "outputAge = Alter:\n" +
      "outputDegrees = Temperatur:\n" +
      "outputDist = Abstand Zur Arbeit:\n";

    sw = new StreamWriter(DETEXTFILE);
    sw.Write(stringsDE);
    sw.Close();
    Console.WriteLine("Text resource file {0} generated.", DETEXTFILE);

    return;
  }//GenerateTextFiles()

  /// <summary>
  /// Outputs usage banner for -t option.
  /// </summary>
  public void TextFileUsage()
  {
    Console.WriteLine
      ("\n" +
      "*============================================================================*\n" +
      "* This application assembly was build with a set of default resources        *\n" +
      "* defined in resources.txt.                                                  *\n" +
      "*                                                                            *\n" +
      "* The -t option allows you manually build .resources files from text files   *\n" +
      "* and then manually build the satellite assemblies.                          *\n" +
      "*                                                                            *\n" +
      "* Selecting the -t option outputs two text resource files:                   *\n" +
      "*  - " + DETEXTFILE + "                                                          *\n" +
      "*  - " + EN_NZTEXTFILE + "                                                       *\n" +
      "*                                                                            *\n" +
      "* Build these files into .resources files using the resgen.exe utility:      *\n" +
      "*                                                                            *\n" +
      "*  clix resgen.exe <path_to_text_file>                                       *\n" +
      "*                                                                            *\n" +
      "* Create two new directories with names that match the locales: en-NZ and de *\n" +
      "*                                                                            *\n" +
      "* Copy the strings.en-NZ.resources file into the en-NZ directory and         *\n" +
      "* the strings.de.resources file into the de directory.                       *\n" +
      "*                                                                            *\n" +
      "* Then in the respective directory create new satellite assemblies with the  *\n" +
      "* .resources files using al.exe:                                             *\n" +
      "*                                                                            *\n" +
      "*  al /t:lib /culture:en-NZ /embed:strings.en-NZ.resources                   *\n" +
      "*     /out:managedresources.resources.dll                                    *\n" +
      "*                                                                            *\n" +
      "*  al /t:lib /culture:de /embed:strings.de.resources                         *\n" +
      "*    /out:managedresources.resources.dll                                     *\n" +
      "*============================================================================*\n"
      );
  }//TextFileUsage()
}//class GenerateTextFiles

/// <summary>
/// This class contains methods to allow the user to see the results of changing the current threads
/// CultureInfo and the automatic loading of the correct satellite assembly.
/// </summary>
public class UseResources
{
  private ResourceManager m_rm = new ResourceManager("strings", Assembly.GetExecutingAssembly());
  private CultureInfo m_ci = new CultureInfo(CultureInfo.CurrentUICulture.Name);

  private int m_age = 0;
  private int m_culture = 0;
  private int m_degrees = 0;
  private int m_distToWork = 0;
  private string m_name = "";

  /// <summary>
  /// Gets user input and sets CultureInfo accordingly.
  /// Then outputs some text to demonstrate the text that is obtained from the satellite assemblies.
  /// </summary>
  public void Run()
  {
    // Prompt the user for the culture to run under.
    ShowPrompt("promptCulture", 1, 3);
    Console.WriteLine("\n");

    //set the current culture to the choice made!
    switch (m_culture)
    {
      case 1: // New Zealand
        m_ci = new CultureInfo("en-NZ");
        break;

      case 3: // German
        m_ci = new CultureInfo("de");
        break;

      default: // use the default...
        m_ci = new CultureInfo("en");
        break;
    }//switch
    Console.WriteLine("Current culture set to {0}", m_ci.EnglishName);
    ShowName();
    ShowAge(15, 120);
    ShowDegrees();
    ShowDistance();
    PrintOutputs();
  }//UseResources()

  /// <summary>
  /// Prompts the user for their name.
  /// </summary>
  public void ShowName() 
  {
    Console.WriteLine();

    // show the inital prompt until a valid entry is made...
    while (true) 
    {
      Console.Write(m_rm.GetString("promptName", m_ci) + " ");

      m_name = Console.ReadLine();

      if (m_name != "")
        return;

      Console.WriteLine();
      Console.WriteLine(m_rm.GetString("promptMissing", m_ci));
    }//while (true)
  }//ShowName()

  /// <summary>
  /// Prompts the user for the temperature.
  /// </summary>
  public void ShowDegrees() 
  {
    Console.WriteLine();

    // show the inital prompt until a valid entry is made...
    while (true) 
    {
      Console.Write(m_rm.GetString("promptDegrees", m_ci) + " " + m_rm.GetString("degree1", m_ci) + " ");

      try 
      {
        m_degrees = Convert.ToInt32(Console.ReadLine());
      }
      catch (Exception) {}

      if (m_degrees >= Convert.ToInt32(m_rm.GetString("degree2", m_ci)) &&
        m_degrees <= Convert.ToInt32(m_rm.GetString("degree3", m_ci)))
        return;

      Console.WriteLine();
      Console.WriteLine(m_rm.GetString("promptValidDeg", m_ci));
    }//while (true)
  }//ShowDegrees()

  /// <summary>
  /// Prompts the user for their age.
  /// </summary>
  /// <param name="lb"></param>
  /// <param name="ub"></param>
  public void ShowAge(int lb, int ub) 
  {
    Console.WriteLine();

    // show the inital prompt until a valid entry is made...
    while (true) 
    {
      Console.Write(m_rm.GetString("promptAge", m_ci) + " ");

      try 
      {
        m_age = Convert.ToInt32(Console.ReadLine());
      }
      catch (Exception) {}

      if (m_age >= lb && m_age <= ub)
        return;

      Console.WriteLine();
      Console.WriteLine(m_rm.GetString("promptValidAge", m_ci));
    }//while (true)
  }//ShowAge()

  /// <summary>
  /// Prompt the user for the culture to run under.  Input from user must be between 1 and 3.
  /// </summary>
  /// <param name="RetrieveString"></param>
  /// <param name="lb"></param>
  /// <param name="ub"></param>
  public void ShowPrompt(string RetrieveString, Int32 lb, Int32 ub) 
  {
    Console.WriteLine("1. New Zealand English");
    Console.WriteLine("2. US English");
    Console.WriteLine("3. German (Germany)\n");

    // show the inital prompt until a valid entry is made...
    while (true) 
    {
      Console.Write(m_rm.GetString(RetrieveString, m_ci) + " ");

      try 
      {
        m_culture = Convert.ToInt32(Console.ReadLine());
      }//try
      catch (Exception) {}

      if (m_culture >= lb && m_culture <= ub)
        break;

      Console.WriteLine();
      Console.WriteLine(m_rm.GetString("promptMissing", m_ci));
    }//while (true)
  }//ShowPrompt()

  /// <summary>
  /// Prompts the user for the distance to work.
  /// </summary>
  public void ShowDistance() 
  {
    Console.WriteLine();

    // show the inital prompt until a valid entry is made...
    while (true) 
    {
      Console.Write(m_rm.GetString("promptDist", m_ci) + " " + m_rm.GetString("dist1", m_ci) + " ");

      try 
      {
        m_distToWork = Convert.ToInt32(Console.ReadLine());
      }
      catch (Exception) {}

      if (m_distToWork > 0 && m_distToWork <= 2000)
        break;

      Console.WriteLine();
      Console.WriteLine(m_rm.GetString("promptValidDist", m_ci));
    }//while (true)
  }//ShowDistance()

  /// <summary>
  /// Output user input to console with correct culture output strings.
  /// </summary>
  public void PrintOutputs() 
  {
    Console.WriteLine();
    Console.Write(m_rm.GetString("promptEntries", m_ci));

    Console.WriteLine();
    Console.WriteLine("{0,-20}{1}", m_rm.GetString("outputName", m_ci), m_name);
    Console.WriteLine("{0,-20}{1}", m_rm.GetString("outputAge", m_ci), m_age);
    Console.WriteLine("{0,-20}{1}", m_rm.GetString("outputDegrees", m_ci), m_degrees);
    Console.WriteLine("{0,-20}{1}", m_rm.GetString("outputDist", m_ci), m_distToWork);
  }//PrintOutputs() 

}//class UseResources

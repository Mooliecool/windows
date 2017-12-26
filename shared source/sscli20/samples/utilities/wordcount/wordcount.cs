//------------------------------------------------------------------------------
// <copyright file="wordcount.cs" company="Microsoft">
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


// Add the classes in the following namespaces to our namespace
using System;
using System.IO;
using System.Text;
using System.Collections;


class WordCountArgParser : ArgParser {

    // Members identifying command-line argument settings
    private Boolean showAlphabeticalWordUsage;
    private Boolean showOccurrenceWordUsage;
    private String  outputFile;
	private Encoding fileEncoding = Encoding.UTF8;
    private ArrayList pathnames = new ArrayList();


    // Give the set of valid command-line switches to the base class
    public WordCountArgParser() : base(new string[] { "?", "a", "o", "f", "c" }) { 
    }


    // Returns the name of the user-specified output file or null if not specified
    public String OutputFile { get { return outputFile; } }


    // Indicates whether the user wanted to see all the words sorted alphabetically
    public Boolean ShowAlphabeticalWordUsage {
        get { return showAlphabeticalWordUsage; }
    }


    // Indicates whether the user wanted to see all the words sorted by occurrence
    public Boolean ShowOccurrenceWordUsage {
        get { return showOccurrenceWordUsage; }
    }

	public Encoding FileEncoding 
	{
		get { return fileEncoding; }
	}

    // Shows application's usage info and also reports command-line argument errors.
    public override void OnUsage(String errorInfo) {
        if (errorInfo != null) {
            // An command-line argument error occurred, report it to user
            // errInfo identifies the argument that is in error.
            Console.WriteLine("Command-line switch error: {0}\n", errorInfo);
        }

	    Console.WriteLine();
      Console.WriteLine("Usage: WordCount [-a] [-o] [-f<output-pathname>] [-c<codepage>] filename");
	    Console.WriteLine("   -?            Show  usage information");
	    Console.WriteLine("   -a            Word usage sorted alphabetically");
      Console.WriteLine("   -c            Specify the codepage to use to read the file");
      Console.WriteLine("   -f:<outfile>  Send output to specified file instead of the console");
	    Console.WriteLine("   -o            Word usage sorted by occurrence and then alphabetically");

    }


    // Called for each non-switch command-line argument (filespecs)
    protected override SwitchStatus OnNonSwitch(String switchValue) {
        SwitchStatus ss = SwitchStatus.NoError;
        // Add command-line argument to array of pathnames.
        // Convert switchValue to set of pathnames, add each pathname to the pathnames ArrayList.
        try {
			String d = Path.GetDirectoryName(switchValue);
			DirectoryInfo dir = new DirectoryInfo((d.Length == 0) ? "." : d);
			foreach (FileInfo f in dir.GetFiles(Path.GetFileName(switchValue))) 
			{
                pathnames.Add(f.FullName);
            }
	}
	catch(System.Security.SecurityException)
	{
		Console.WriteLine("This sample has failed to run due to a security limitation!\n"+
			"Try running the sample from a local drive or using CasPol.exe to turn off security.");
		ss = SwitchStatus.Error;
	}
	catch {
		ss = SwitchStatus.Error;
	} 
	if (pathnames.Count == 0)
	{
		Console.WriteLine("None of the specified files exists.");
		ss = SwitchStatus.Error;
	}

        return(ss);
    }


    // Returns an enumerator that includes all the user-desired files.
    public IEnumerator GetPathnameEnumerator() {
        return pathnames.GetEnumerator(0, pathnames.Count);
    }


    // Called for each switch command-line argument
    protected override SwitchStatus OnSwitch(String switchSymbol, String switchValue) {
        // NOTE: For case-insensitive switches, 
        //       switchSymbol will contain all lower-case characters

        SwitchStatus ss = SwitchStatus.NoError;
        switch (switchSymbol) {  

        case "?":   // User wants to see Usage
            ss = SwitchStatus.ShowUsage; 
            break;

        case "a":   // User wants to see all words sorted alphabetically
            showAlphabeticalWordUsage = true;
            break;

        case "o":   // User wants to see all words sorted by occurrence
            showOccurrenceWordUsage = true;
            break;

        case "f":   // User wants output redirected to a specified file
            if (switchValue.Length < 1) {
                Console.WriteLine("No output file specified.");
                ss = SwitchStatus.Error; 
            } else {
                outputFile = switchValue.Substring(1, switchValue.Length - 1);
            }
            break;
		case "c": // User wants a specific codepage to be used to open the file
			if (switchValue.Length < 1) 
			{
				Console.WriteLine("No codepage specified.");
				ss = SwitchStatus.Error;
			}
			else
			{				
				try
				{
					int codePage = System.Int32.Parse(switchValue);
					fileEncoding = System.Text.Encoding.GetEncoding(codePage);
				}
				catch
				{
					Console.WriteLine("No valid codepage specified.");
					ss = SwitchStatus.Error;
				}
			}
			break;
        default:
    	    Console.WriteLine("Invalid switch: \"" + switchSymbol + "\".\n");
            ss = SwitchStatus.Error; 
            break;
        }
        return(ss);
    }


    // Called when all command-line arguments have been parsed
    protected override SwitchStatus OnDoneParse() {
        SwitchStatus ss = SwitchStatus.NoError;
        if (pathnames.Count == 0) {
            // No pathnames were specified
            ss = SwitchStatus.Error;
        } else {
            // Sort all the pathnames in the list
            pathnames.Sort(0, pathnames.Count, null);
        }
        return(ss);
    }
}


///////////////////////////////////////////////////////////////////////////////


// The WordCounter class
public class WordCounter {
    public WordCounter() { /* No interesting construction */ }

    // Each object of this class keeps a running total of the files its processed
    // The following members hold this running information
    Int64 totalLines = 0; 
    Int64 totalWords = 0;
    Int64 totalChars = 0;
    Int64 totalBytes = 0;

    // The set of all words seen (sorted alphabetically)
    SortedList wordCounter = new SortedList();

    // The following methods return the running-total info
    public Int64 TotalLines { get { return totalLines; } }
    public Int64 TotalWords { get { return totalWords; } }
    public Int64 TotalChars { get { return totalChars; } }
    public Int64 TotalBytes { get { return totalBytes; } }

    // This method calculates the statistics for a single file.
    // This file's info is returned via the out parameters
    // The running total of all files is maintained in the data members
    public Boolean CountStats(String pathname, Encoding  fileEncoding, 
        out Int64 numLines, out Int64 numWords, out Int64 numChars, out Int64 numBytes) {

        Boolean Ok = true;  // Assume success
        numLines = numWords = numChars = numBytes = 0;  // Initialize out params to zero
        try {
            // Attempt to open the input file for read-only access
            FileStream fsIn = new FileStream(pathname, FileMode.Open, FileAccess.Read, FileShare.Read);
            numBytes = fsIn.Length;
            StreamReader sr = new StreamReader(fsIn, fileEncoding, true);

            // Process every line in the file
            for (String Line = sr.ReadLine(); Line != null; Line = sr.ReadLine()) {
                numLines++;
                numChars += Line.Length;
                String[] Words = Line.Split(null);  // Split the line into words
                for (int Word = 0; Word < Words.Length; Word++) {
					if (Words[Word] != "") {	// Don't count empty strings
						numWords++;
						if (!wordCounter.ContainsKey(Words[Word])) {
                            // If we've never seen this word before, add it to the sorted list with a count of 1
                            wordCounter.Add(Words[Word], 1);
                        } else {
                            // If we have seen this word before, just increment its count
                            wordCounter[Words[Word]] = (Int32) wordCounter[Words[Word]] + 1;
                        }
					}
                }
            }
            // Explicitly close the StreamReader to properly flush all buffers
            sr.Close(); // This also closes the FileStream (fsIn)
        }
        catch (FileNotFoundException) {
            // The specified input file could not be opened
            Ok = false;
        }
        // Increment the running totals with whatever was discovered about this file
        totalLines += numLines;
        totalWords += numWords;
        totalChars += numChars;
        totalBytes += numBytes;
        return(Ok);
    }

    // Returns an enumerator for the words (sorted alphabetically)
    public IDictionaryEnumerator GetWordsAlphabeticallyEnumerator() {
        return (IDictionaryEnumerator) wordCounter.GetEnumerator();
    }


    // This nested class is only used to sort the words by occurrence
    // An instance of this class is created for each word
    public class WordOccurrence : IComparable {

        // Members indicating the number of times this word occurred and the word itself
        private int occurrences;
        private String word;

        // Constructor
        public WordOccurrence(int occurrences, String word) {
            this.occurrences = occurrences;
            this.word = word;
        }

        // Sorts two WordOccurrence objects by occurrence first, then by word
        public int CompareTo(Object o) {
            // Compare the occurance of the two objects
            int n = occurrences - ((WordOccurrence)o).occurrences;
            if (n == 0) {
                // Both objects have the same ccurrance, sort alphabetically by word
                n = String.Compare(word, ((WordOccurrence)o).word);
            }
            return(n);
        }

        // Return the occurrence value of this word
        public int Occurrences { get { return occurrences; } }

        // Return this word
        public String Word { get { return word; } }
    }


    // Returns an enumerator for the words (sorted by occurrence)
    public IDictionaryEnumerator GetWordsByOccurrenceEnumerator() {
        // To create a list of words sorted by occurrence, we need another SortedList object
        SortedList sl = new SortedList();

        // Now, we'll iterate through the words alphabetically
        IDictionaryEnumerator de = GetWordsAlphabeticallyEnumerator();
        while (de.MoveNext()) {

            // For each word, we create a new WordOccurrence object which
            // contains the word and its occurrence value.
            // The WordOccurrence class contains a CompareTo method which knows
            // to sort WordOccurrence objects by occurrence value and then alphabetically by the word itself.
            sl.Add(new WordOccurrence((int)de.Value, (string)de.Key), null);
        }
        // Return an enumerator for the words (sorted by occurrence)
        return (IDictionaryEnumerator) sl.GetEnumerator();        
    }

    // Returns the number of unique words processed
    public int UniqueWords {
        get { return wordCounter.Count; }
    }
}


///////////////////////////////////////////////////////////////////////////////


// This class represents the application itself
class App {
    public static int Main(String[] args) {

		try
		{
			// Parse the command-line arguments
			WordCountArgParser ap = new WordCountArgParser();
			if (!ap.Parse(args)) 
			{
				// An error occurrend while parsing
				return 1;
			}

			// If an output file was specified on the command-line, use it
			FileStream fsOut = null;
			StreamWriter sw = null;
			if (ap.OutputFile != null) 
			{
				fsOut = new FileStream(ap.OutputFile, FileMode.Create, FileAccess.Write, FileShare.None);
				sw = new StreamWriter(fsOut, ap.FileEncoding);

				// By associating the StreamWriter with the console, the rest of 
				// the code can think it's writing to the console but the console 
				// object redirects the output to the StreamWriter
				Console.SetOut(sw);
			}

			// Create a WordCounter object to keep running statistics
			WordCounter wc = new WordCounter();

			// Write the table header
			Console.WriteLine("Lines\tWords\tChars\tBytes\tPathname");

			// Iterate over the user-specified files
			IEnumerator e = ap.GetPathnameEnumerator();
			while (e.MoveNext()) 
			{
				Int64 NumLines, NumWords, NumChars, NumBytes;
				// Calculate the words stats for this file
				wc.CountStats((String)e.Current, ap.FileEncoding, out NumLines, out NumWords, out NumChars, out NumBytes);

				// Display the results
				String[] StrArgs = new String[] { 
													NumLines.ToString(), 
													NumWords.ToString(), 
													NumChars.ToString(), 
													NumBytes.ToString(), 
													(String) e.Current 
												};
				Console.WriteLine(String.Format("{0,5}\t{1,5}\t{2,5}\t{3,5}\t{4,5}", StrArgs));
			}

			// Done processing all files, show the totals
			Console.WriteLine("-----\t-----\t-----\t-----\t---------------------");
			Console.WriteLine(String.Format("{0,5}\t{1,5}\t{2,5}\t{3,5}\tTotal in all files", 
				new object[] { wc.TotalLines, wc.TotalWords, wc.TotalChars, wc.TotalBytes } ));

			// If the user wants to see the word usage alphabetically, show it
			if (ap.ShowAlphabeticalWordUsage) 
			{
				IDictionaryEnumerator de = wc.GetWordsAlphabeticallyEnumerator();
				Console.WriteLine(String.Format("Word usage sorted alphabetically ({0} unique words)", wc.UniqueWords));
				while (de.MoveNext()) 
				{
					Console.WriteLine(String.Format("{0,5}: {1}", de.Value, de.Key));
				}
			}

			// If the user wants to see the word usage by occurrence, show it
			if (ap.ShowOccurrenceWordUsage) 
			{
				IDictionaryEnumerator de = wc.GetWordsByOccurrenceEnumerator();
				Console.WriteLine(String.Format("Word usage sorted by occurrence ({0} unique words)", wc.UniqueWords));
				while (de.MoveNext()) 
				{
					Console.WriteLine(String.Format("{0,5}: {1}", 
						((WordCounter.WordOccurrence)de.Key).Occurrences, 
						((WordCounter.WordOccurrence)de.Key).Word));
				}
			}

			// Explicitly close the console to guarantee that the StreamWriter object (sw) is flushed
			Console.Out.Close();         
			if (fsOut != null) fsOut.Close();
		}
		catch(Exception e)
		{
			Console.WriteLine("Exception: " + e.Message);
		}
		return 0;
    } 
}


///////////////////////////////// End of File /////////////////////////////////

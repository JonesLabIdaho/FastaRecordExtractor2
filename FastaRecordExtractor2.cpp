// FastaRecordExtractor inputs a list of text strings, one per line
// It also inputs a fasta file.
// Any record in the fasta file with a header including any of the text strings
// is saved to a new file.
// Version 2 allows the user to specify that an exact match is required (that is, the header must match the query text in its entirety).
// Contact Adam Jones, adamjones@uidaho.edu, with questions.

// Usage:
// -q:	Query Text File (the list of strings corresponding to records to get from the fasta file)
// -i:	Fasta Input Filename
// -o:	Output Filename
// -h: Help
// -e: exact match (y or n)
// no arguments: Interactive

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class fasta_record {
public:
	string header;
	string sequence;
};

int main( int argc, char* argv[] )
{
	int i;
	string infile_name_string, outfile_name_string, queryfile_name_string;
	char* infile_name;
	char* outfile_name;
	char* queryfile_name;
	bool exact_match;
	vector<string> querylist;

	string query;
	string tempstring1, tempstring2;

	if (argc == 1)
	{
		cout << "\n(I)nteractive or (H)elp?\n";
		cin >> query;
		if (query == "H" || query == "h")
		{
			cout << "\nFastaRecordExtractor\n";
			cout << "-i:\tinput file\n";
			cout << "-o:\toutput file\n";
			cout << "-q:\tquery file with list of header sequences to retrieve\n";
			cout << "-e:\trequire exact match (y or n)\n";
			cout << "no arguments:\tinteractive mode\n";
			return 0;
		}
	}

	if (argc > 1)
	{
		tempstring1 = argv[1];
		if (tempstring1 == "-h")
		{
			cout << "\nFastaRecordExtractor\n";
			cout << "-i:\tinput file\n";
			cout << "-o:\toutput file\n";
			cout << "-q:\tquery file with list of header sequences to retrieve\n";
			cout << "-e:\trequire exact match (y or n, default n)\n";
			cout << "no arguments:\tinteractive mode\n";
			return 0;
		}
	}

	infile_name_string = "default";
	outfile_name_string = "default";
	queryfile_name_string = "default";
	

	infile_name = new char[1000];
	outfile_name = new char[1000];
	queryfile_name = new char[1000];
	exact_match = false;

	for (i = 1; i < argc-1; i++)
	{
		tempstring1 = argv[i];
		tempstring2 = argv[i+1];
		if (tempstring1 == "-i")
			infile_name_string = tempstring2;
		if (tempstring1 == "-o")
			outfile_name_string = tempstring2;
		if (tempstring1 == "-q")
			queryfile_name_string = tempstring2;
		if (tempstring1 == "-e")
		{
			if (tempstring2[0] == 'y' || tempstring2[0] == 'Y')
				exact_match = true;
		}
	}

	bool interactive = false;
	if (infile_name_string == "default")
	{
		cout << "Fasta Filename:\n";
		cin >> infile_name_string;	
		interactive = true;
	}

	if (outfile_name_string == "default")
	{
		cout << "\nOutput Filename:\n";
		cin >> outfile_name_string;
		interactive = true;
	}

	if (queryfile_name_string == "default")
	{
		cout << "\nQuery Filename:\n";
		cin >> queryfile_name_string;
		interactive = true;
	}

	std::string query_exact;
	if (interactive == true)
	{
		cout << "\nExact Match (y or n)?";
		cin >> query_exact;
		if (query_exact[0] == 'y' || query_exact[0] == 'Y')
			exact_match = true;
		else
			exact_match = false;
	}

	for (i = 0; i < static_cast<int>(infile_name_string.length()); i++)
		infile_name[i] = infile_name_string[i];
	infile_name[i] = '\0';

	for (i = 0; i < static_cast<int>(outfile_name_string.length()); i++)
		outfile_name[i] = outfile_name_string[i];
	outfile_name[i] = '\0';

	for (i = 0; i < static_cast<int>(queryfile_name_string.length()); i++)
		queryfile_name[i] = queryfile_name_string[i];
	queryfile_name[i] = '\0';

	cout << "\n\nInput File:\t" << infile_name;
	cout << "\nOutput File:\t" << outfile_name;
	cout << "\nQuery File:\t" << queryfile_name;
	if (exact_match)
		cout << "\nExact Match:\tYes";
	else
		cout << "\nExact Match:\tNo";

	/*
	cout << "\n\nProceed? (y to proceed)\n";
	cin >> query;

	if (query != "y" && query != "Y")
	{
		cout << "\n\nEnter an integer to exit!!";
		cin >> i;
		return 0;
	} */
	cout << "\n\nProceeding...\n";

	// Load the query sequences and put them in the vector

	char *line = new char[200001];
	string querystring;
	ifstream queryfile;
	queryfile.open(queryfile_name);
	int querycounter = 0;
	while(!queryfile.eof())
	{
		queryfile.getline(line,20000);
		querystring = line;
		if (!querystring.empty())
		{
			querylist.push_back(querystring);
			querycounter++;
		}
	}
	queryfile.close();

	string firstheader;
	string currentheader;
	string nextheader;
	string currentsequence;
	unsigned int saved_counter, removed_counter;
	bool headerflag;
	bool On_List;
	size_t foundflag;

	ifstream infile;
	ofstream outfile;

	fasta_record current_record;

	infile.open(infile_name);
	outfile.open(outfile_name);

	saved_counter = 0;
	removed_counter = 0;

	infile.getline(line,200000);
	if (line[0] == '>')
		firstheader = line;
	else
	{
		cout << "\nFirst Line:\n" << line;
		cout << "\n\nThis is not a fasta file!\n";
		//cout << "\n\nEnter an integer to exit!!";
		//cin >> i;
		return 0;
	}
	
	cout << "\nFirst Header:\n" << firstheader << "\n";
	nextheader = firstheader;

	while (!infile.eof())
	{
		headerflag = false;
		currentheader = nextheader;
		headerflag = false;
		currentsequence.clear();
		current_record.header = currentheader;
		while (!headerflag && !infile.eof())
		{
			infile.getline(line, 200000);
			if (line[0] == '>')
			{
				headerflag = true;
				nextheader = line;
			}
			else
			{
				currentsequence = currentsequence + line;
			}
		}
		current_record.sequence = currentsequence;

		// Check to see if this record is on the query list
		if (!exact_match)
		{
			On_List = false;
			for (i = 0; i < querycounter; i++)
			{
				foundflag = current_record.header.find(querylist[i]);
				if (foundflag != string::npos)
					On_List = true;
			}
		}
		else
		{
			On_List = false;
			for (i = 0; i < querycounter; i++)
			{
				if (querylist[i] == current_record.header)
					On_List = true;
			}
		}
	
		if (On_List)
		{
			outfile << current_record.header << "\n";
			outfile << current_record.sequence << "\n";
			saved_counter++;
		}
		else
		{
			removed_counter++;
		}

		if ((removed_counter+saved_counter)%1000 == 0)
		{
			cout << "\n" << removed_counter+saved_counter << " reads processed. " << saved_counter << " saved. " << removed_counter << " removed.";
		}
	} // end of while not eof


	cout << "\n\nDone!\n";
	cout << removed_counter+saved_counter << " reads processed. " << saved_counter << " saved. " << removed_counter << " removed.\n\n";

	infile.close();
	outfile.close();

	delete[] line;
	delete[] infile_name;
	delete[] outfile_name;
	delete[] queryfile_name;

	//cout << "\n\nEnter an integer to exit!!";
	//cin >> i;

	return 0;
}
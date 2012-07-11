/* Bryan Orr & Alex Dunn at APS, ANL
/*
/* Converts streamer file collected with Measurement Computing device
/*   to csv format.
*/

#include <iostream>	// cout()
#include <fstream>	// out()
#include <time.h>	// time()
#include "cbw.h"	// MC Universal Lib

using std::cout;
using std::endl;
using std::ofstream;

// TODO: Make the OUTFILE name automatic

int main(int argc, char* argv[]) {
	// Check usage or print help
	if(argc != 3) {
		cout << "\n\tdat2csv.exe [INFILE] [OUTFILE]" << endl;
		cout << "[INFILE] is the path to the file to be converted.\n" << endl;
		cout << "[OUTFILE] is the path of the file which should be"
			 << "Created to dump CSV data.\n" << endl;
		return 0;
	}

	// Declarations
	int ULStat, Gain;
	float RevLevel = (float)CURRENTREVNUM;
	char* FileName = argv[1];
	short HighChan, LowChan;
	long PreTrigCount, TotalCount, Rate;
	long FirstPoint, NumPoints;
	ofstream out;
	// char timestr[80];
	WORD *DataBuffer;

	// Required for MC UL
	ULStat = cbDeclareRevision(&RevLevel);
	cbErrHandling (PRINTALL, DONTSTOP);

	// Get fileinfo from the dat file
	ULStat = cbFileGetInfo (FileName, &LowChan, &HighChan, &PreTrigCount, &TotalCount, &Rate, &Gain);

	// Set the number of channels variable
	int NumChans = HighChan - LowChan + 1;

	// Open filestream for writing fileinfo header
	out.open(argv[2]);
	if(out.is_open()) {
		// Show fileinfo as header
		out << "Streamer File Name," << FileName << endl;
		out << "Low Channel," << LowChan << endl;
		out << "High Channel," << HighChan << endl;
		out << "No. of Pretrigger samples," << PreTrigCount << endl;
		out << "No. of Samples," << TotalCount << endl;
		out << "No. of Samples/Channel," << TotalCount/(HighChan+1) << endl;
		out << "Collection Rate (Hz)," << Rate << endl;
		out << "Gain," << Gain << endl;
		out << endl;
		
		// Print header with count and num channels
		out << "Count,"; // Print count header
		for(int i = LowChan; i <= HighChan; i++) {
			out << "Channel " << i << ","; // Print channel headers
		}
		out << endl; // newline to start printing data

		// Size of chunk to process in number of data points
		NumPoints = TotalCount/NumChans; // MUST be a multiple of NumChans
		// Size of DataBuffer MUST be 1 greater or equal to NumPoints (?)
		DataBuffer = new WORD[NumPoints-1]; 
		// required indices
		FirstPoint = 0;
		int k = 0;

		// Iterate through all data points.  Each iteration takes care of NumPoints
		//		worth of data.  So, make TotalCount/NumPoints iterations.
		for(long i = FirstPoint; i <= (TotalCount/NumPoints); i++) {
			// Index for the current point in the DataBuffer array
			// Restart at zero on every iteration and go up to NumPoints
			k = 0;

			// Buffer NumPoints worth of data
			ULStat = cbFileRead(FileName, FirstPoint, &NumPoints, DataBuffer);

			// Check for error before proceeding
			if (ULStat != 0) {
				printf("Error %d\n", ULStat);
				return 0;
			}

			// Iterate through rows
			while (k < NumPoints-1) {
				// Output one data point per channel
				for(int j = LowChan; j <= HighChan; j++) { 
					if (j != HighChan) out << DataBuffer[k] << ',';	// Print channel data into column
					else out << DataBuffer[k];
					k++;							// Move to next data point
				}
				out << endl; // 
			}

			FirstPoint += NumPoints;	// Increment starting point for next set
			delete [] DataBuffer;
		}
	} out.close();

	return 0;
}
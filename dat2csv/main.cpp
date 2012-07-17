/* Bryan Orr & Alex Dunn at APS, ANL
/*
/* Converts streamer file collected with Measurement Computing device
/*   to csv format.
*/

#include <stdio.h>
#include <time.h>	// time()
#include "cbw.h"	// MC Universal Lib

void clock_tick(int &hour, int &mins, int &secs);

// TODO: Make the OUTFILE name automatic

int main(int argc, char* argv[]) {
	// Check usage or print help
	if(argc != 3) {
		printf("\n\tdat2csv.exe [INFILE] [OUTFILE]\n");
		printf("[INFILE] is the path to the file to be converted.\n");
		printf("[OUTFILE] is the path of the file which should be created to dump CSV data.\n");
		return 0;
	}

	// Declarations
	int ULStat, Gain;
	float RevLevel = (float)CURRENTREVNUM;
	char* FileName = argv[1];
	short HighChan, LowChan;
	long PreTrigCount, TotalCount, Rate;
	long NumPoints, FirstPoint;
	FILE *writeFile;
	WORD *DataBuffer;

	// Required for MC UL
	ULStat = cbDeclareRevision(&RevLevel);
	cbErrHandling (PRINTALL, DONTSTOP);

	// Get fileinfo from the dat file
	ULStat = cbFileGetInfo (FileName, &LowChan, &HighChan, &PreTrigCount, &TotalCount, &Rate, &Gain);
	int NumChans = HighChan - LowChan + 1;

	// Open CSV and print data
	writeFile = fopen(argv[2], "w");

	// Convert FileName to time
	char hours[3];
	char mins[3];
	char secs[3];
	hours[0] = FileName[14];
	hours[1] = FileName[15];
	hours[2] = NULL;
	mins[0] = FileName[17];
	mins[1] = FileName[18];
	mins[2] = NULL;
	secs[0] = FileName[20];
	secs[1] = FileName[21];
	secs[2] = NULL;
	int h = atoi(hours);
	int m = atoi(mins);
	int s = atoi(secs);

	// Increment value
	double increment = 1/(double)Rate;
	double seccount = 0;

	if(writeFile == NULL) {
		fprintf(stderr, "Unable to open output file %s\n", argv[2]);
		return 1;
	} else {
		fprintf(writeFile, "Streamer File Name, %s\n", FileName);
		fprintf(writeFile, "Low Channel,%d\n", LowChan);
		fprintf(writeFile, "High Channel,%d\n", HighChan);
		fprintf(writeFile, "No. of Pretrigger samples,%d\n", PreTrigCount);
		fprintf(writeFile, "No. of Samples,%d\n", TotalCount);
		fprintf(writeFile, "No. of Samples/Channel,%d\n", TotalCount/(HighChan+1));
		fprintf(writeFile, "Collection Rate (Hz),%d\n", Rate);
		fprintf(writeFile, "Gain,%d\n\n", Gain);

		// Print header with count and num channels
		for(int i = LowChan; i <= HighChan; i++) {
			fprintf(writeFile, "Channel %d,", i); // Print channel headers
		}
		fprintf(writeFile, "\n"); // newline to start printing data

		// Size of chunk to process in number of data points
		NumPoints = TotalCount/NumChans; // MUST be a multiple of NumChans
		// Size of DataBuffer MUST be 1 greater or equal to NumPoints (?)
		DataBuffer = new WORD[NumPoints-1]; 
		// required indices
		FirstPoint = 0;
		int k = 0;
		int checksec = 0;

		// Iterate through all data points.  Each iteration takes care of NumPoints
		//		worth of data.  So, make TotalCount/NumPoints iterations.
		for(long i = FirstPoint; i < (TotalCount/NumPoints); i++) {
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
				// Print time and seconds for each row
				fprintf(writeFile, "%02d:%02d:%02d,%f,", h, m, s, seccount);
				// Output one data point per channel
				for(int j = LowChan; j <= HighChan; j++) { 
					if (j != HighChan) fprintf(writeFile, "%d,", DataBuffer[k]);	// Print channel data into column
					else fprintf(writeFile, "%d\n", DataBuffer[k]);					// Don't print comma on last column
					k++;
				}
					seccount = seccount + increment;								// Increment by 1/Rate for each row
					checksec++;
					if (checksec == Rate) {
						clock_tick(h, m, s);
						checksec = 0;
					}
			}

			FirstPoint += NumPoints;	// Increment starting point for next set
		}

	} fclose(writeFile);
	delete [] DataBuffer;

	return 0;
}

// Intelligence to handle time incrementation
void clock_tick(int &hour, int &mins, int &secs) {
	secs++;
	if (secs == 60) {
		mins++;
		secs = 0;
	}
	if (mins == 60) {
		hour++;
		mins = 0;
	}
	// Go to the next day
	if (hour == 25) {
		hour = 0;
	}
}
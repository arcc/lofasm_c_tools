/*Lofasm raw data file header */
#ifndef LOFASM_FILE_INFO_H_
#define LOFASM_FILE_INFO_H_
#define MAX_FILENAME_SIZE 80
#include <string>
using namespace std;

class LofasmFileInfo
/*
A class for lofasm file information. It is a base class for different files.
*/
{
    public:
        string filename; // file Name
        string fileSig;            // Header signature
        int hdrVersion;          // File header version, mostly for raw data
        int hdrLength;           // File header lenght
        string lofasmStation;     // LoFASM station name
        int numfBin;                 // Number of Channels/bins
        double freqStart;              // Start Frequency
        double freqStep;               // Frequency step
        double startMJD;             // file start mjd date to milliseconds unit in day
        double endMJD;               // File end mjde data
        double intgrTime;              // Integration total time for each integration
        int dataFmtVersion;         // data format version
        char *notes;             // notes for the file
        long fileSize;               // File size
        int numIntgr;               // Number of integration in the file
        int readFlag;           // If header has been correctly read, readFlag == 1
        int fileCheckFlag;
        ifstream* file = new ifstream(); 
        LofasmFileInfo () : numIntgr(0),fileSize(0),readFlag(0),fileCheckFlag(0) {};
};
#endif

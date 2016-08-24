/*Lofasm files header */
#ifndef LOFASM_FILES_H_
#define LOFASM_FILES_H_

#define MAX_FILENAME_SIZE 80
#define PKT_PER_INTGR 17
#include <string>
#include "lofasm_data_class.h"
#include "lofasm_raw_file.h"
using namespace std;


class LofasmFile
/*
A class for a single lofasm file.
*/
{
    public:
        string filename; // file Name
        int fileType;            // Data file type

        LofasmRawFile rawFile;
        //TODO : add other file type;

        double startMJD;
        double startFreq;
        double freqStep;
        int numFreqBin;
        double timeStep;
        int numIntgr;
        long fileSize;

        int check_file_type(string filename);
        void read_hdr(string filename);
        void print_hdr();
        void check_file_info(string filename);
};



class LofasmDataFiles
/*
A class for lofasm data files. A sorted file list will be provided. Add the file
header information will be included.

Public Members
----------
fileList

*/
{
    public:
        vector <LofasmFile> fileList;
        string currFilename;
        int currFileIdx;
        int currFileType;
        double currMjd;
        double currTimeStep;
        long currFileSize;
        int numfbin;   // number of frequency bin for the file
        int numtbin;
        LofasmFile * currFileInfo;
        LofasmIntgr itgr;
        ifstream currFile;

        void add_file(string filename);   // add file to fileList
        //void open_next_file();
        void open_file(int fileIdx);
        void read_one_intgr(int intgrIdx);
        //TODO: add read data block
};
#endif

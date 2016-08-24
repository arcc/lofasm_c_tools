/*Lofasm raw data file header */
#ifndef LOFASM_RAW_FILE_H_
#define LOFASM_RAW_FILE_H_

#define SEC_PER_DAY 86400.0
#define MAX_FILENAME_SIZE 80
#define PKT_PER_INTGR 17
#include <string>
#include "lofasm_file_info.h"
#include "lofasm_data_class.h"
using namespace std;


class IntgrInfo
/*
A class for raw integration information.
*/
{
    public:
        double MJD;
        char badIntgr;
        long intgrID;
        long intgrPos;
};

class LofasmRawFile : public LofasmFileInfo
/*
A class for lofasm raw file.
*/
{
    public:
        int mjdDay;                 // Data taking date in mjd formate,
                                    //   the integer part or mjd
        int mjdMsec;                // Data taking date in mjd formate,
                                    //   the fractional part in milliseconds
        int packetSize;             // Size of each data packet in bytes
        int intgrSize;              // Size of each data integration in bytes
        int hdrPktSig;

        // Current lenght of _bad_ integration ID
        vector<IntgrInfo> intgrList;

        
        int open_raw_file(string fname);
        void get_raw_hdr(string fname);
        int check_raw_file(string fname);
        int is_hdr_pkt(ifstream* infile);
        void get_lofasm_raw_intgr(LofasmIntgr& intgr, int intgrIdx);
};

#endif

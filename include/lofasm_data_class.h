/*Dedispersion cpp version header file */
#ifndef LOFASM_DATA_CLASS_H_
#define LOFASM_DATA_CLASS_H_

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>

#define SEC_PER_DAY 86400.0
#define MAX_FILENAME_SIZE 80
using namespace std;

class LofasmIntgr
/* A class for storing lofasm one data integration */
{
  public :
      int numfBin;
      double starFreq;
      double freqStep;
      vector <unsigned int> AA, BB, CC, DD;
      vector <signed int> ABrl, ACrl, ADrl, BCrl, BDrl, CDrl;
      vector <signed int> ABim, ACim, ADim, BCim, BDim, CDim;
      vector <signed int> AB, AC, AD, BC, BD, CD;
      vector <double> freqAxis;
      LofasmIntgr () : numfBin(0) {};
      void init_polar(int numFreqBin);
      void set_freqAxis(double fstart, double fstep);
      void form_beam();
      void get_polar_cross();
};


/*Define the filtbank data class*/
class FilterBank
/*
A class for filter bank data, which is a block of integrations
Data will be stored in a 2D c++ vector, x axis time and
y axis frequency.  The information of the data will also
be stored in the class members.
Public Members:
----------
numFreqBin : int
    Number of frequency bins/channels
numTimeBin : int
    Number of time sample
fltdata : 2D Vector float/vertor<vector <float>>
    Filter bank data
freqAxis : Vector double/verotr<double>
    frequency value/y axis unit(MHz)
freqStep : double
    Step for frequency axis unit(MHz)
timeAxis : double
    Time sample/x axis unit (s)
timeStep : double
    Step for time axis unit(s)
Public Method:
-------------
FilterBank (int numfbin,int numtbin):
    Initialize FilterBank class
void set_freqAxis(double fStart, double fStep):
    Set the frequency Axis
void set_timeAxis(double tStart, double tStep);
    Set the time Axis
*/
{
    public:
        int numFreqBin;
        int numTimeBin;
        vector< vector<double> > fltdata;
        vector<double> freqAxis;
        double freqStep;

        vector<double> timeAxis;
        double timeStep;
        FilterBank (int numfbin,int numtbin);
        void set_freqAxis(double fStart, double fStep);
        void set_timeAxis(double tStart, double tStep);
        void resize_time_bin(int newTimeBin);
        FilterBank* get_freq_band(double startBandFreq, double endBandFreq);
};
/* Finish define FilterBank data class*/


#endif

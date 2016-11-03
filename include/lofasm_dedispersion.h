/*Dedispersion cpp version header file */
#ifndef LOFASM_DEDISPERSION_H_
#define LOFASM_DEDIDPERSION_H_

#include<math.h>
#include<string>
#include<vector>

#include "lofasm_data_class.h"
using namespace std;

/*Define DM with select index class*/
class DM_sltIndex
/*
A class for one DM value and its selected data index depend
on the frequency and time step.
Backgroud:
Due to the dispersion effect, radio signal have different
velocity in the media. Higher frequency signals travel faster
then lower ones. The signal arrival time differency will have
a relationship:
dt = 4.15e3*DM*(1/f^2-1/f_ref)
Here DM is called dispersion measure unit ( pc*cm^-3)
Depend on the time step and frequency and frequency step. Data
could smear out for more then one time bin for one frequency bin.

The select index include the beginning smear index and end index in
filter bank data. In our de-dispersion algorithm. All the smeared
data will be selected and added to the non-disperion signal arrival time.
So we call it select index.

Here the reference frequency is the highest frequency

Public Members:
----------
DM : double
    DM value
timeDelay : double
    Signal arrival time delay at some frequency based on reference frequency.
tStep: double
    Time step for the filter bank data
lastSft1Idx : int
    For calculate the lowest frequency smear out index. It is for a imaginary
    frequency bin which has one frequency step below the lowest frequency.
normNum : int
    Total number of selected data. For the purpose of normalizing data.
freqCutTree : int
    Cutting frequency for tree method(our algorithm). Any frequency bin
    high then the cutting frequency will not do any operation in our algorithm.
sftIdx : vector<int>
    Singal arrival time delay in the sense of data index.
    shift index  = time delay/time step
smearSize : vector<int>
    Smear size. For the purpose of simulate data.
sltIdx : vector<vetor<int> >/numFreqBin*2 vector int
    Selected index vector
    sltIdx[frequency index][0] is the beginning index of signal for one freqency
    slfIdx[frequency index][1] is the end index of signal for one frequency
Public Method
----------
DM_sltIndex :
    Initialize DM_sltIndex class
void cal_sftIdx(vector<double> freqAxis, double timeStep, double refFreq) :
    Calculate shift index
void cal_sltIdx(vector<double> freqAxis, double timeStep, double refFreq) :
    Calculate selected index
void cal_normNum() :
    Calculate normalize number
void get_smearSize() :
    get the smearSize
*/
{
    public:
        double DM;
        double timeDelay;
        double tStep;
        int lastSft1Idx;
        int normNum;
        int freqCutTree; // Cutting frequency for tree method.
        vector<int> sftIdx;
        vector<int> smearSize; // Change to smearSize
        vector<vector<int> > sltIdx; /* For tree method to select the right data to add */


        DM_sltIndex (double dm);
        void cal_sftIdx(vector<double> freqAxis, double timeStep, double refFreq);
        // Calculate select index based on higher frequency.
        void cal_sltIdx(vector<double> freqAxis, double timeStep, double refFreq);
        // Calculate the normalize number
        void cal_normNum();
        void get_smearSize();
};
/*Finish define DM_sltIndex*/

/* Define DM vs time class*/
class DMTime
/*
A class for DM and time 2D array for DM searching. X axis time, y axis DM number.
Data is the total power for DM searchinge.

Public Members
-----------
dmAxis : vector<double>
    dm axis/y axis
dmStep : double
    dm step for dm axis
numDM : int
    Total DM trail
dmStart : double
    Start DM for searching
timeAxis : vector<double>
    time axis/x axis
timeStart : double
    Start time for time axis
timeStep : double
    Time step for time axis
numTimeBin : int
    The number of time bins
DM_time_power : vector< vector<float> >
    2D power vector, x direction is for each time bin
    y direction is for each dm bin
    It is the result for didispersion.
normArray : vector<float>
    Normalize number for each DM

Public Method :
----------
DMTime : Initialize DMTime Class
set_timeAxis : Allocate time axis for DM time power
set_dmAxis : Allocate dm axis for DM time power
set_normArray : Allocate the normalize number array
set_DM_time_power : Allocate the DM time power vector
*/
{
    public:
        vector<double> dmAxis;
        double dmStep;
        int numDM;
        double dmStart;

        vector<double> timeAxis;
        double timeStart;
        double timeStep;
        int numTimeBin;

        vector< vector<float> > DM_time_power;
        vector<float> normArray;

        DMTime (int numDMbin, int numTBin, double tStep);
        void set_timeAxis(double timeStart);
        void set_dmAxis(double dmStart,double dmStep);
        void set_normArray();
        void set_DM_time_power();

};

class dedispersion_config
/* This is a class for storing the dedispersion configuraiton information*/
{
    public:
        string config_file;
        double dm_start;
        double dm_end;
        double dm_step;
        int dm_num;
        int read_config(string filename);

};

/********************* Define lofasm dedispersion functions *******************/
/* Smooth data */
vector<double> smooth_data(vector<double> &data, int smearSize);

/* Simulate data*/
FilterBank simulate_flt_ez(double dm, double fstart, double fStep, double tstart,\
                     double tStep, int numfBin, int numtBin,  float noiseAmp, \
                     float noiseBias, float SNR, double highFreqTOA);

int do_dedsps_curve(FilterBank & indata, FilterBank & outdata, \
                    DM_sltIndex & DMsft);

int do_dedsps(FilterBank & data, FilterBank & outdata, DM_sltIndex & DMsft);

double compute_time_delay(double freq, double freqRef, double dm);

double cal_dmStep_min(double freqMax, double freqMin, double timeStep);

int check_data_size(FilterBank & data, DMTime & DMT, \
                    vector<DM_sltIndex> & DMsftArray);

int compute_DM_t_power_dommy(FilterBank & data, DMTime & DMT, \
                             vector<DM_sltIndex> & DMsftArray);

int compute_DM_t_power_tree_dommy(FilterBank & data, DMTime & DMT, \
                                  vector<DM_sltIndex> & DMsftArray);

int compute_DM_t_power_tree(FilterBank & data, DMTime & DMT, \
                            vector<DM_sltIndex> & DMsftArray);

int compute_DM_t_power_tree_add(FilterBank & data, DMTime & DMT, \
                                vector<DM_sltIndex> & DMsftArray);

int cal_cut_freq_index(DM_sltIndex & DMsft0, DM_sltIndex & DMsft1);

DMTime* dm_search_tree(FilterBank & indata, double dmStart, double dmEnd,\
                        double dmStep);


#endif

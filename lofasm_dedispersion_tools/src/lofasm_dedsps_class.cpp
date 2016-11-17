#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string>
#include<vector>
#include<algorithm>
#include <sstream>
#include<fstream>
#include"lofasm_dedispersion.h"
using namespace std;

/*Class method for dedsps_config*/
void dedsps_config::read_config_file(char *filename){
    ifstream file(filename);
    for(string line; getline(file, line);){
        parse_config_line(line);
    }
}

void dedsps_config::parse_config_line(string line){
    string arr[2];
    int i = 0;
    // Split line
    stringstream ssin(line);
    while (ssin.good() && i < 2){
        ssin >> arr[i];
        ++i;
    }

    if (arr[0][0] == '#')
        return;
    else if (!arr[0].compare("dm_start"))
        dm_start = atof(arr[1].c_str());
    else if (!arr[0].compare("dm_end"))
        dm_end = atof(arr[1].c_str());
    else if (!arr[0].compare("freq_start"))
        freq_start = atof(arr[1].c_str());
    else if (!arr[0].compare("freq_end"))
        freq_end = atof(arr[1].c_str());
    else if (!arr[0].compare("time_start"))
        time_start = atof(arr[1].c_str());
    else if (!arr[0].compare("time_end"))
        time_end = atof(arr[1].c_str());
    else
        return;
    return;
}

/*Class methods for DM_sltIndextIndex*/
DM_sltIndex::DM_sltIndex(double dm){
    DM = dm;

}

void DM_sltIndex::cal_sftIdx(vector<double> freqAxis, double timeStep, double refFreq){
    int Nf;   //Number of frequency bin
    int i;
    double sftbin;
    tStep = timeStep;
    Nf = freqAxis.size();
    sftIdx.resize(Nf,0);

    for(i=0;i<Nf;i++){
        timeDelay = -4.15e3*DM*(1.0/(freqAxis[i]*freqAxis[i])
                    -1.0/(refFreq*refFreq));
        sftbin = timeDelay/tStep;
        sftIdx[i] = (int)trunc(sftbin);
    }


}

void DM_sltIndex::get_smearSize(){
    int i;
    int idxDiff;
    int size;
    if(sftIdx.size()<1){
        cout<<"Plesase calculate sht shift index first!"<<endl;
    }
    smearSize.resize(sftIdx.size(),0);
    for(i=0;i<smearSize.size()-1;i++){
        idxDiff = sftIdx[i+1]-sftIdx[i];
        size = idxDiff-1;
        if(size<0) size = 0;
        smearSize[i+1] = size;

    }
}

void DM_sltIndex::cal_sltIdx(vector<double> freqAxis, double timeStep, double refFreq){
    /* This function will assume that the positive select Index means signal arrive
       later. */
    int Nf;   //Number of frequency bin
    int i;
    double sftbin;
    double chan0;  //A imaginary freqency channal before the first channel
    int chan0SltIdx;
    int idxDiff;
    int size;  // Smear size;

    tStep = timeStep;
    Nf = freqAxis.size();
    sltIdx.resize(Nf, vector<int> (2,0));

    for(i=0;i<Nf;i++){
        timeDelay = 4.15e3*DM*(1.0/(freqAxis[i]*freqAxis[i])
                    -1.0/(refFreq*refFreq));
        sftbin = timeDelay/tStep;
        sltIdx[i][0] = (int)trunc(sftbin);

    }

    /* calcaulate smear out */
    /* Get first channel smear out*/
    chan0 = freqAxis[0]-freqAxis[1]+freqAxis[0];
    timeDelay = 4.15e3*DM*(1.0/(chan0*chan0)-1.0/(refFreq*refFreq));
    chan0SltIdx = (int)trunc(timeDelay/tStep);
    idxDiff = chan0SltIdx - sltIdx[0][0];
    size = idxDiff-1;
    if(size<0) size = 0;
    /* Get smear out size and second select index*/
    for(i=0;i<Nf-1;i++){
        sltIdx[i][1] = sltIdx[i][0]+size;
        // For the next channel;
        idxDiff = sltIdx[i][0]- sltIdx[i+1][0];
        size = idxDiff-1;
        if(size<0) size = 0;
    }

    // Get the last channel select index end.
    sltIdx[Nf-1][1] = sltIdx[Nf-1][0]+size;

}


void DM_sltIndex::cal_normNum(){
/* The sltIdx, select index, should be calculated first*/
    int slcNumPfreq;
    int i;
    for(i=0;i<sltIdx.size();i++){
        slcNumPfreq = sltIdx[i][1]-sltIdx[i][0]+1;
        normNum += slcNumPfreq;
    }
}
/* Finish define the DM_sftIndex class methods*/

/* Define class function for DMTime class*/
DMTime::DMTime(int numDMbin, int numTBin, double tStep){
    numDM = numDMbin;
    numTimeBin = numTBin;
    timeStep = tStep;
}

void DMTime::set_timeAxis(double timeStart){
    if(numTimeBin<1){
        cout<<"Please make sure number of time bin is not zero."<<endl;
        exit(1);
    }
    timeAxis.resize(numTimeBin,0.0);
    for(int i =0;i<numTimeBin;i++){
        timeAxis[i] = timeStart+i*timeStep;
    }
}

void DMTime::set_dmAxis(double dmStart,double DMStep){
    int i;
    if(numDM<1){
        cout<<"Please make sure number of DM is not zero."<<endl;
        exit(1);
    }
    dmAxis.resize(numDM,0.0);
    for(i =0;i<numDM;i++){
        dmAxis[i] = dmStart+DMStep*i;
    }
}

void DMTime::set_normArray(){
    normArray.resize(numTimeBin,0.000001);// initial as 0.000001 a small number
}

void DMTime::set_DM_time_power(){
    DM_time_power.resize(numDM, vector<double> (numTimeBin,0.0));
}
/* Finish define the class function for class DMTime*/

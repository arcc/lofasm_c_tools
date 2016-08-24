#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>

#include "lofasm_data_class.h"
#include "lofasm_files.h"
using namespace std;

/* Class methods for FilterBank class*/
/* Initialize the filter bank data class */
FilterBank::FilterBank(int numfbin,int numtbin){
    numFreqBin = numfbin;
    numTimeBin = numtbin;
    // Initial values are in 0.0
    fltdata.resize(numFreqBin, vector<float> (numTimeBin,0.0));
}
/* Create frequency axis */
void FilterBank::set_freqAxis(double fStart, double fStep){
    if(numFreqBin <= 0){
        cout<<"The number of frequency bin should be more than zero"<<endl;
        exit(1);
    }
    freqAxis.resize(numFreqBin,0.0);
    freqStep = fStep;
    for(int i =0;i<numFreqBin;i++){
        freqAxis[i] = fStart+i*freqStep;
    }
}
/* Create time axis */
void FilterBank::set_timeAxis(double tStart, double tStep){
    if(numTimeBin <= 0){
        cout<<"The number of time bin should be more than zero"<<endl;
        exit(1);
    }
    timeAxis.resize(numTimeBin,0.0);
    timeStep = tStep;
    for(int i =0;i<numTimeBin;i++){
        timeAxis[i] = tStart+i*timeStep;
    }
}

/* Resize filter bank data */
void FilterBank::resize_time_bin(int newTimeBin){
    /* Resize the number of time bin. It use the vector resize function.
      If newTimeBin bigger then the old time bin, it will add zeros in the end
      of the time bin.
      If newTimeBin smaller then the old time bin, it will truncate end of the
      data.

      The fltdata, timeAxis, freqAxis have to be initialize*/
    int i;
    double tstart;
    numTimeBin = newTimeBin;
    for (i = 0;i<numFreqBin;i++){
        fltdata[i].resize(numTimeBin,0.0);
    }
    tstart = timeAxis.front();
    set_timeAxis(tstart,timeStep);
}


/* Take part of frequency band of data */
FilterBank* FilterBank::get_freq_band(double startBandFreq, double endBandFreq){
    /* Get one frequency band of data.
       The FilterBank class's time axis and frequency axis have to be set before
       using this function.
       Parameters
       ----------
       startBandFreq : double
                   Start frequency of the band. If it is smaller then the start
                   frequency of the filter band data, it will take the start
                   frequency.
       endBandFreq : double
                     End frequency of the band. If it is bigger then the end
                     frequency of the filter bank data, it will take the end
                     frequency.

        Return
        ---------
        FilterBank class pointer point to a new memory with band of filter bank data.
    */
    int numNewFbin;
    int i = 0;
    int j;
    double temp;
    int startIdx;
    int endIdx;
    if (startBandFreq>endBandFreq){
        cout<<"Start band frequency smaller end band frequency. Take end band";
        cout<<" frequency as start band frequency."<<endl;
        temp = startBandFreq;
        startBandFreq = endBandFreq;
        endBandFreq = temp;
    }

    if (numFreqBin<=0){
        cout<<"The number of frequency bin should be more than zero."<<endl;
        cout<<"Check if the frequency axis has been initialized."<<endl;
        exit(1);
    }
    if (startBandFreq<freqAxis.front()){
        cout<<"Band start frequency smaller then the filter bank data start ";
        cout<<"frequency. Using filter bank data start frequency "<<freqAxis.front();
        cout<<" MHz instead."<<endl;
        startBandFreq = freqAxis.front();
    }
    if (endBandFreq>freqAxis.back()){
        cout<<"Band end frequency bigger then the filter bank data end ";
        cout<<"frequency. Using filter bank data end frequency "<<freqAxis.back();
        cout<<" MHz instead."<<endl;
        endBandFreq = freqAxis.back();
    }

    if (startBandFreq>freqAxis.back()||endBandFreq<=freqAxis.front()){
        cout<<"Selected band exceed the frequency band from "<<freqAxis.front();
        cout<<" to "<<freqAxis.back()<<" MHz."<<endl;
        exit(1);
    }
    /*Calculate the number of frequency bin*/
    //Find start frequency in freqAxis
    while(!(freqAxis[i]<=startBandFreq && freqAxis[i+1]>startBandFreq)){
        i++;
    }

    startIdx = i;
    //Find end frequency in freqAxis
    i = 0;
    while(!(freqAxis[i]<=endBandFreq && freqAxis[i+1]>endBandFreq)){
        i++;
        if (i==numFreqBin){
            i--;
            break;
        }
    }

    endIdx = i;
    numNewFbin = endIdx-startIdx+1;
    /* Get and set up a new FilterBank class*/
    FilterBank* banddata = new FilterBank(numNewFbin,numTimeBin);

    banddata->set_freqAxis(freqAxis[startIdx],freqStep);
    banddata->set_timeAxis(timeAxis.front(),timeStep);

    /*Set values*/
    for(i=0;i<numNewFbin;i++)
    {
        for(j=0;j<numTimeBin;j++)
        {

            banddata->fltdata[i][j] = fltdata[i+startIdx][j];
        }
    }

    return banddata;
}
/* Finish define the FilterBank data class methods*/


/* Class function for LofasmIntgr */

void LofasmIntgr::init_polar(int numFreqBin){
	  numfBin = numFreqBin;
		AA.resize(numfBin,0);
		BB.resize(numfBin,0);
		CC.resize(numfBin,0);
		DD.resize(numfBin,0);
		ABrl.resize(numfBin,0);
		ACrl.resize(numfBin,0);
		ADrl.resize(numfBin,0);
		BCrl.resize(numfBin,0);
		BDrl.resize(numfBin,0);
		CDrl.resize(numfBin,0);
		ABim.resize(numfBin,0);
		ACim.resize(numfBin,0);
		ADim.resize(numfBin,0);
		BCim.resize(numfBin,0);
		BDim.resize(numfBin,0);
		CDim.resize(numfBin,0);
}

void LofasmIntgr::set_freqAxis(double fstart, double fstep){
	  int i;
	  if (numfBin == 0){
			  cout<< "The number of frequency bins can not be zero."<<endl;
				exit(1);
		}
		freqAxis.resize(numfBin,0);
		for (i=0;i<numfBin;i++){
			  freqAxis[i] = fstart+i*fstep;
		}
}

void LofasmIntgr::form_beam(){
	  return;
}
/* Define function for read auto polarization from raw file*/

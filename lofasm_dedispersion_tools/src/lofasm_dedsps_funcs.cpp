#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <time.h>
#include <utility>
#include <stdio.h>
#include <unistd.h>
#include "lofasm_dedispersion.h"
#include "lofasm_data_class.h"
#include "lofasm_utils.h"
using namespace std;


/* Smooth data function */
vector<double> smooth_data(vector<double> &data, int smearSize){
	/*
	data is a 1-D vector.
	*/
	int i,j;
	size_t lenData;

    vector<double> result(data.size(),0.0);
	result = data;
    /* smooth size smaller then 0, no need to smooth*/
    if(smearSize<=0){
        return data;
    }
    /*Check data length*/
    lenData = data.size();
	if((int)lenData <= smearSize){
		cout<<"Not enought data for smoothing.\n"<<endl;
	    exit(1);
	}

    vector<double> smthblk(smearSize,0.0);  // smooth data block

    /* Initialize the smooth block with the first smooth size
     data.*/

    copy(data.begin(),data.begin()+smearSize,smthblk.begin());

	for(i=0;i<lenData-smearSize+1;i++){

		result[i] = accumulate(smthblk.begin(),smthblk.end(),0); // Sum up smooth block
	    // Shift smooth block to next five data.

	    rotate(smthblk.begin(),smthblk.begin()+1,smthblk.end());
	    smthblk.back() = data[i+smearSize];

	}
	return result;
}

double compute_time_delay(double freq, double freqRef, double dm){
    double timeDelay;
    timeDelay = 4.15e3*dm*(1.0/(freq*freq)-1.0/(freqRef*freqRef));
    return timeDelay;
}

double cal_dmStep_min(double freqMax, double freqMin, double timeStep){
    /* Calculate the smallest dm step*/
    double dmstep;
    cout<<"Time resolution for the input filter bank data is: "<<timeStep << \
		" (second)"<<endl;
    dmstep = timeStep/(-4.15e3*(1.0/(freqMax*freqMax)
                       -1.0/(freqMin*freqMin)));
		cout<<"Dispersion measure search step is: " << dmstep << " (pc/cm3)" <<endl;
    return dmstep;
}

int check_data_size(FilterBank & data, DMTime & DMT, vector<DM_sltIndex> & DMsftArray){
    return 0;
}


/* Create DM_T Plot*/
int compute_DM_t_power_dommy(FilterBank & data, DMTime & DMT, vector<DM_sltIndex> & DMsftArray){
    int status;
    int i,j,k;
    int numfBin, numtBin, numDM;
    int sftI;
    numfBin = data.numFreqBin;
    numtBin = data.numTimeBin;
    numDM = DMT.numDM;

    /**
    for(i=0;i<numDM;i++){
        cout<<DMsftArray[i].DM<<endl;
        for(j=0;j<numfBin;j++){
            cout<<DMsftArray[i].sftIdx[j]<<" ";
        }
        cout<<endl;
    }
    */

    /* Dommy way of do it */
    for(i=0;i<numDM;i++){
        for(j=0;j<numfBin;j++){
            sftI = DMsftArray[i].sftIdx[j];
            for(k=0;k<numtBin;k++){
                DMT.DM_time_power[i][k+sftI] += data.fltdata[j][k];
                DMT.normArray[k+sftI]+= 1.0;
            }
        }
        /*Normalize */
        for(k=0;k<DMT.numTimeBin;k++){
            DMT.DM_time_power[i][k] = DMT.DM_time_power[i][k]/DMT.normArray[k];
        }
        fill(DMT.normArray.begin(), DMT.normArray.end(), 0.000001);
    }
    return 0;
}

int compute_DM_t_power_tree_dommy(FilterBank & data, DMTime & DMT, vector<DM_sltIndex> & DMsftArray){
    int status;
    int i,j,k;
    int loop1,loop2;
    int dmIdx;
    int numfBin, numtBin, numDM;
    int sftI;
    int sltI[2] = {0,0};
    int sltIp[2] = {0,0};   //selected index from last dm
    int numSub;
    int numAdd;
    float lastPower;
    float curPower;
    numfBin = data.numFreqBin;
    numtBin = data.numTimeBin;
    numDM = DMT.numDM;

    /* Tree method of computing DM_T_POWER */
    // Calculate first DM for T_Power array
    /* loop over time bin last*/

    /* Do first dm */
    for(i=0;i<numtBin;i++){
        for(j=0;j<numfBin;j++){
            sftI = DMsftArray[0].sftIdx[j];
            sltI[0] = DMsftArray[0].sltIdx[j][0];
            sltI[1] = DMsftArray[0].sltIdx[j][1];
            /* Do summation for this time bin */
            for(k=0;k<sltI[1]-sltI[0]+1;k++){
                DMT.DM_time_power[0][i] += data.fltdata[j][i+k+sltI[0]];
            }
        }
        DMT.DM_time_power[0][i] = DMT.DM_time_power[0][i]/(float)DMsftArray[0].normNum;
    }
    /*Dommy way */
    for(dmIdx=1;dmIdx<numDM;dmIdx++){
        for(i=0;i<numtBin;i++){
            for(j=0;j<numfBin;j++){
                sltI[0] = DMsftArray[dmIdx].sltIdx[j][0];
                sltI[1] = DMsftArray[dmIdx].sltIdx[j][1];
                /* Do summation for this time bin */
                for(k=0;k<sltI[1]-sltI[0]+1;k++){
                    DMT.DM_time_power[dmIdx][i] += data.fltdata[j][i+k+sltI[0]];
                }
            }
        DMT.DM_time_power[dmIdx][i] = DMT.DM_time_power[dmIdx][i]/(float)DMsftArray[dmIdx].normNum;
        }
    }

    // /*Do other dm*/

    // cout<<"Start tree method."<<endl;
    // for(dmIdx=1;dmIdx<numDM;dmIdx++){
    //     for(i=0;i<numtBin;i++){
    //         lastPower = DMT.DM_time_power[dmIdx-1][i]*DMsftArray[dmIdx-1].normNum;
    //         curPower = lastPower;
    //         for(j=0;j<numfBin;j++){
    //             sftI = DMsftArray[dmIdx].sftIdx[j];
    //             sltI[0] = DMsftArray[dmIdx].sltIdx[j][0];
    //             sltI[1] = DMsftArray[dmIdx].sltIdx[j][1];
    //             sltIp[0] = DMsftArray[dmIdx-1].sltIdx[j][0];
    //             sltIp[1] = DMsftArray[dmIdx-1].sltIdx[j][1];
    //             numSub = sltI[0]-sltIp[0];
    //             numAdd = sltI[1]-sltIp[1];

    //             /*Substract the power we don't need*/
    //             for(loop1=0;loop1<numSub;loop1++){
    //                 curPower = curPower - data.fltdata[j][i+sltIp[0]+loop1];
    //             }
    //             /*Add new powers */
    //             for(loop2=0;loop2<numSub;loop2++){
    //                 curPower = curPower+ data.fltdata[j][i+sltIp[1]-loop2];
    //             }
    //         }
    //         DMT.DM_time_power[dmIdx][i] = curPower/(float)DMsftArray[dmIdx].normNum;
    //     }
    // }
    return 0;
}

int compute_DM_t_power_tree(FilterBank & data, DMTime & DMT, vector<DM_sltIndex> & DMsftArray){
    /* Tree method for LoFASM dedispersion. */
    int status;
    int i,j,k;
    int loop1,loop2;
    int dmIdx;
    int numfBin, numtBin, numDM;
    int sftI;
    int numSub;
    int numAdd;
    int fcutIndex;
    float lastPower;
    float curPower;

    numfBin = data.numFreqBin;
    numtBin = data.numTimeBin;
    numDM = DMT.numDM;

    vector< vector<int> > sltdiff;
    vector<int> sltIStart(numfBin,0);
    vector<int> sltIEnd(numfBin,0);

    vector<int> sltIpStart(numfBin,0);   //selected index from last dm
    vector<int> sltIpEnd(numfBin,0);
    sltdiff.resize(numfBin, vector<int>(2,0));

    /* Tree method of computing DM_T_POWER */
    // Calculate first DM for T_Power array
    /* loop over time bin last*/

    /* Do first dm */

    for(i=0;i<numtBin;i++){
        for(j=0;j<numfBin;j++){
            sftI = DMsftArray[0].sftIdx[j];
            sltIStart[j] = DMsftArray[0].sltIdx[j][0];
            sltIEnd[j] = DMsftArray[0].sltIdx[j][1];
            /* Do summation for this time bin */
            for(k=0;k<sltIEnd[j]-sltIStart[j]+1;k++){
                DMT.DM_time_power[0][i] += data.fltdata[j][i+k+sltIStart[j]];
            }
        }
        DMT.DM_time_power[0][i] = DMT.DM_time_power[0][i]/(float)DMsftArray[0].normNum;

    }

    /*Do other dm*/

    cout<<"Start tree method."<<endl;
    for(dmIdx=1;dmIdx<numDM;dmIdx++){
        //cout<<" dm "<<dmIdx<<endl;
        fcutIndex = DMsftArray[dmIdx].freqCutTree;
        for(j=0;j<numfBin;j++){
            sltIStart[j] = DMsftArray[dmIdx].sltIdx[j][0];
            sltIEnd[j] = DMsftArray[dmIdx].sltIdx[j][1];
            sltIpStart[j] = DMsftArray[dmIdx-1].sltIdx[j][0];
            sltIpEnd[j] = DMsftArray[dmIdx-1].sltIdx[j][1];
            sltdiff[j][0] = sltIStart[j]-sltIpStart[j];
            sltdiff[j][1] = sltIEnd[j]-sltIpEnd[j];
        }

        for(i=0;i<numtBin;i++){

            lastPower = DMT.DM_time_power[dmIdx-1][i]*DMsftArray[dmIdx-1].normNum;
            curPower = lastPower;

            for(j=0;j<=fcutIndex;j++){
                /*Substract the power we don't need*/
                for(loop1=0;loop1<sltdiff[j][0];loop1++){
                    curPower = curPower - data.fltdata[j][i+sltIpStart[j]+loop1];
                }

                /*Add new powers */
                for(loop2=0;loop2<sltdiff[j][1];loop2++){
                    curPower = curPower+ data.fltdata[j][i+sltIEnd[j]-loop2];
                }

            }

            DMT.DM_time_power[dmIdx][i] = curPower/(float)DMsftArray[dmIdx].normNum;
        }

    }
    return 0;
}

int compute_DM_t_power_tree_smrt(FilterBank & data, DMTime & DMT, vector<DM_sltIndex> & DMsftArray,\
                                int terminate_idx){
    /* Tree method for LoFASM dedispersion. */
    int status;
    int i,j,k;
    int loop1,loop2;
    int dmIdx;
    int numfBin, numtBin, numDM;
    int sftI;
    int numSub;
    int numAdd;
    int fcutIndex;
    float lastPower;
    float curPower;

    numfBin = data.numFreqBin;
    numtBin = data.numTimeBin;
    numDM = DMT.numDM;

    vector< vector<int> > sltdiff;
    vector<int> sltIStart(numfBin,0);
    vector<int> sltIEnd(numfBin,0);

    vector<int> sltIpStart(numfBin,0);   //selected index from last dm
    vector<int> sltIpEnd(numfBin,0);
    sltdiff.resize(numfBin, vector<int>(2,0));

    /* Tree method of computing DM_T_POWER */
    // Calculate first DM for T_Power array
    /* loop over time bin last*/

		/* Do first dm */
    cout<<"Start tree method."<<endl;
		//printf("%%%d dedispersion process finished.\r", 0.0/numDM);
		//fflush(stdout);
		for(i=0;i<terminate_idx;i++){
				for(j=0;j<numfBin;j++){
						sftI = DMsftArray[0].sftIdx[j];
						sltIStart[j] = DMsftArray[0].sltIdx[j][0];
						sltIEnd[j] = DMsftArray[0].sltIdx[j][1];
						/* Do summation for this time bin */
						for(k=0;k<sltIEnd[j]-sltIStart[j]+1;k++){
								DMT.DM_time_power[0][i] += data.fltdata[j][i+k+sltIStart[j]];
						}
				}
				DMT.DM_time_power[0][i] = DMT.DM_time_power[0][i]/(float)DMsftArray[0].normNum;

		}
    //printf("%%%d dedispersion process finished.\r", 1.0/numDM);
		//fflush(stdout);
		/*Do other dm*/
		for(dmIdx=1;dmIdx<numDM;dmIdx++){
				//cout<<" dm "<<dmIdx<<endl;
				fcutIndex = DMsftArray[dmIdx].freqCutTree;
				for(j=0;j<numfBin;j++){
						sltIStart[j] = DMsftArray[dmIdx].sltIdx[j][0];
						sltIEnd[j] = DMsftArray[dmIdx].sltIdx[j][1];
						sltIpStart[j] = DMsftArray[dmIdx-1].sltIdx[j][0];
						sltIpEnd[j] = DMsftArray[dmIdx-1].sltIdx[j][1];
						sltdiff[j][0] = sltIStart[j]-sltIpStart[j];
						sltdiff[j][1] = sltIEnd[j]-sltIpEnd[j];
				}

				for(i=0;i<terminate_idx;i++){

						lastPower = DMT.DM_time_power[dmIdx-1][i]*DMsftArray[dmIdx-1].normNum;
						curPower = lastPower;

						for(j=0;j<=fcutIndex;j++){
								/*Substract the power we don't need*/
								for(loop1=0;loop1<sltdiff[j][0];loop1++){
										curPower = curPower - data.fltdata[j][i+sltIpStart[j]+loop1];
								}

								/*Add new powers */
								for(loop2=0;loop2<sltdiff[j][1];loop2++){
										curPower = curPower+ data.fltdata[j][i+sltIEnd[j]-loop2];
								}

						}

						DMT.DM_time_power[dmIdx][i] = curPower/(float)DMsftArray[dmIdx].normNum;
				}
				printf("  %%%.1f dedispersion processed.\r", (float)dmIdx/(float)numDM * 100);
				fflush(stdout);
		}
		printf("\n");
		fflush(stdout);
		return 0;
}


int do_dedsps_check(FilterBank & indata, FilterBank & outdata, DM_sltIndex & DMsft){
    /* Check the status of input parameters*/
    return 0;
}

int do_dedsps_curve(FilterBank & indata, FilterBank & outdata, DM_sltIndex & DMsft){
    /* The shift index should be calculated
       Input data freqency size should be the same with sftIndex size*/
    int status;
    int i,j,k;
    int numfBin,numtBin;
    int targetIndex;
    int sftI;
    int sltI;
    vector<int> selectIdx;
    vector<int> maxSlt;
		vector<int>::iterator it;
    double tStep;

    numfBin = indata.freqAxis.size();

    numtBin = indata.timeAxis.size();
    tStep = indata.timeStep;

    DMsft.cal_sltIdx(indata.freqAxis,indata.timeStep,indata.freqAxis.back());
		cout<<"Hello3"<<endl;
    maxSlt = DMsft.sltIdx.front();
		cout<<maxSlt[1]<<endl;

		for (i=0;i<indata.fltdata.size();i++){
			  indata.fltdata[i].resize(numtBin+maxSlt[1],0.0);
				//it = indata.fltdata[i].begin();
				//indata.fltdata[i].insert(it,maxSlt[1],0.0);
		}
    for(j=0;j<numtBin;j++){
			  for(i=0;i<numfBin;i++){
					  //cout<<DMsft.sltIdx[i][0]<<" "<<DMsft.sltIdx[i][1]<<" ";
						//cout<<endl;
					  for(k=DMsft.sltIdx[i][0];k<DMsft.sltIdx[i][1];k++){
							  //cout<<k<<" "<<endl;
				        outdata.fltdata[i][j] += indata.fltdata[i][j+k];
            }
				}
				//cout<<endl;
		}

		/*
		for(j=0;j<3;j++){
        targetIndex = j+maxSft;
        for(i=0;i<numfBin;i++){
            sftI = DMsft.sftIdx[i];
            //cout<<"freq bin "<<i<<" sSize "<<DMsft.smearSize[i]<<" shft "<<sftI;
            sltI = selectIdx[i];
            for(k=0;k<=DMsft.smearSize[i];k++){
                //cout<<" index "<<j<<" "<<targetIndex-sftI+k<<" ";
                cout<<sltI<<" "<<indata.fltdata[i][sltI+k+j]<<" ";
                outdata.fltdata[i][targetIndex] += indata.fltdata[i][sltI+k+j];
            }
            cout<<endl;
        }
    }
		*/

}

/* Do dedispersion */
int do_dedsps(FilterBank & indata, FilterBank & outdata, DM_sltIndex & DMsft){
    /* The shift index should be calculated
       Input data freqency size should be the same with sftIndex size*/
    int status;
    int i,j;
    int numfBin,numtBin;

    if(indata.freqAxis.size()!=DMsft.sftIdx.size()){
    	cout<<"Input data Freqency bins not match shift index array bins"<<endl;
    	exit(1);
    }

    /* Adjust out put time length*/
    if(outdata.timeAxis.size()<indata.timeAxis.size()+ DMsft.sftIdx.back()){
    	cout<<"Not enough size for dedispersion. At least ";
    	cout<<indata.timeAxis.size()+ DMsft.sftIdx.back()<<" Time bins are needed";
    	cout<<endl;
    	exit(1);
    }

    for(i=0;i<indata.freqAxis.size();i++){
    	for(j=0;j<indata.timeAxis.size();j++){
    		outdata.fltdata[i][j+DMsft.sftIdx[i]]=indata.fltdata[i][j];
    	}
    }

	return status;
}

int cal_cut_freq_index(DM_sltIndex & DMsftNow, DM_sltIndex & DMsftPre)
{
    /* This function calculates the stopping freqency index in for tree method
       depend previous dm select index
       Requirement:
       The select index has to be calculated for two class DMsft0 and DMsft1
       The length of sltIdx of two DM should be the same*/
    int diffSltStart=0;
    int diffSltEnd=0;
    int i;

    for(i=DMsftNow.sltIdx.size()-1; i>=0 ;i--){
        diffSltStart = DMsftNow.sltIdx[i][0]- DMsftPre.sltIdx[i][0];
        diffSltEnd = DMsftNow.sltIdx[i][1]- DMsftPre.sltIdx[i][1];

        if(diffSltStart!=0 || diffSltEnd!=0){
            break;
        }

    }

    return i;
}

/* Simulate data */
FilterBank simulate_flt_ez(double dm, double fstart, double fStep, double tstart, \
                     double tStep, int numfBin, int numtBin, float noiseAmp,   \
                     float noiseBias,float SNR, double highFreqTOA)
{

	FilterBank result(numfBin,numtBin);
	DM_sltIndex DMsft(dm);

    float signalAmp;
    int TOAindex;
    int i,j;
    int smear;
    int sft;

    double timeDelay;
    double freqCal;            // A fake freqency for calculate the last channal smear out
    int chan1smear;


    signalAmp = noiseAmp*SNR;

    /* Set filter bank data axises */
    result.set_freqAxis(fstart,fStep);
    result.set_timeAxis(tstart,tStep);

    freqCal = result.freqAxis.front()-fStep;
    /* Check signal arrival time */
    if(highFreqTOA<=result.timeAxis.front()||
    	highFreqTOA>=result.timeAxis.back()){
    	cout<<"Highest freqency arrival time overflow the time axis of filter";
        cout<<"bank data"<<endl;
        exit(1);
    }

    /* Fill data with noise first */
    /* initialize random seed: */
    /* Add noise */
    srand (time(NULL));
    for(i=0;i<numfBin;i++){
    	for(j=0;j<numtBin;j++){
    		result.fltdata[i][j] = ((static_cast <double> (rand())/   \
    			                    static_cast <double>(RAND_MAX))   \
    			                    *noiseAmp*2-noiseAmp+noiseBias);
    	}
    }
    /* Add signals */
    TOAindex =  (int)highFreqTOA/(double)tStep;


    /* Get smear*/
    DMsft.cal_sftIdx(result.freqAxis, tStep,result.freqAxis.front());
    DMsft.get_smearSize();

    /* Get smear for the first channal */
    int chan1sft;
    chan1sft = (int)trunc(compute_time_delay(result.freqAxis.front(), freqCal, dm)/tStep);

    /*get shift index*/
    for(i=0;i<numfBin;i++){
        timeDelay = 4.15e3*DMsft.DM*(1.0/(result.freqAxis[i]*result.freqAxis[i])
                       -1.0/(result.freqAxis.back()*result.freqAxis.back()));

        DMsft.sftIdx[i] = (int)trunc(timeDelay/tStep);
    }

    /* Add signal */
    for(i=numfBin-1;i>=0;i--){
    	smear = DMsft.smearSize[i]+1;
    	sft = DMsft.sftIdx[i];
    	result.fltdata[i][TOAindex+sft] = signalAmp;
    	for(j=1;j<smear;j++){
    		result.fltdata[i][TOAindex+sft+j] = signalAmp;
    	}
    }
    /* Add first channal smear out */
    for(i=0;i<chan1sft-1;i++){
        result.fltdata[0][TOAindex+sft+i-1] = signalAmp;
    }
    return result;

}

/* Simulate data */
FilterBank simulate_flt_RFI(double dm, double fstart, double fStep, double tstart, \
                     double tStep, int numfBin, int numtBin, float noiseAmp,   \
                     float noiseBias,float SNR, double highFreqTOA, float RFIAmp,\
									   double RFIfreq, double RFI_band_width)
{

	FilterBank result(numfBin,numtBin);
	DM_sltIndex DMsft(dm);

    float signalAmp;
    int TOAindex;
    int i,j;
    int smear;
    int sft;

    double timeDelay;
    double freqCal;            // A fake freqency for calculate the last channal smear out
    int chan1smear;
		double RFI_freq_start;
		double RFI_freq_end;
		int RFI_idx_start;
		int RFI_idx_end;


    signalAmp = noiseAmp*SNR;

    /* Set filter bank data axises */
    result.set_freqAxis(fstart,fStep);
    result.set_timeAxis(tstart,tStep);

    freqCal = result.freqAxis.front()-fStep;
    /* Check signal arrival time */
    if(highFreqTOA<=result.timeAxis.front()||
    	highFreqTOA>=result.timeAxis.back()){
    	cout<<"Highest freqency arrival time overflow the time axis of filter";
        cout<<"bank data"<<endl;
        exit(1);
    }

    /* Fill data with noise first */
    /* initialize random seed: */
    /* Add noise */
    srand (time(NULL));
    for(i=0;i<numfBin;i++){
    	for(j=0;j<numtBin;j++){
    		result.fltdata[i][j] = ((static_cast <double> (rand())/   \
    			                    static_cast <double>(RAND_MAX))   \
    			                    *noiseAmp*2-noiseAmp+noiseBias);
    	}
    }
    /* Add signals */
    TOAindex =  (int)highFreqTOA/(double)tStep;


    /* Get smear*/
    DMsft.cal_sftIdx(result.freqAxis, tStep,result.freqAxis.front());
    DMsft.get_smearSize();

    /* Get smear for the first channal */
    int chan1sft;
    chan1sft = (int)trunc(compute_time_delay(result.freqAxis.front(), freqCal, dm)/tStep);

    /*get shift index*/
    for(i=0;i<numfBin;i++){
        timeDelay = 4.15e3*DMsft.DM*(1.0/(result.freqAxis[i]*result.freqAxis[i])
                       -1.0/(result.freqAxis.back()*result.freqAxis.back()));

        DMsft.sftIdx[i] = (int)trunc(timeDelay/tStep);
    }

    /* Add signal */
    for(i=numfBin-1;i>=0;i--){
    	smear = DMsft.smearSize[i]+1;
    	sft = DMsft.sftIdx[i];
    	result.fltdata[i][TOAindex+sft] = signalAmp;
    	for(j=1;j<smear;j++){
    		result.fltdata[i][TOAindex+sft+j] = signalAmp;
    	}
    }
    /* Add first channal smear out */
    for(i=0;i<chan1sft-1;i++){
        result.fltdata[0][TOAindex+sft+i-1] = signalAmp;
    }
    if (RFIAmp > 0){
			  RFI_freq_start = RFIfreq - RFI_band_width/2.0;
				RFI_freq_end = RFIfreq + RFI_band_width/2.0;
		    if (( RFI_freq_start <result.timeAxis.front()) \
				     ||(RFI_freq_end >result.timeAxis.back())){
			      cerr << "RFI frequency is not in data freqency range. "<<endl;
						exit(1);
		    }

				RFI_idx_start = (int)((RFI_freq_start - result.timeAxis.front())/fStep);
        RFI_idx_end = (int)((RFI_freq_end - result.timeAxis.front())/fStep);
		    for(j=0;j<numtBin;j++)
		    {
					  for(i=RFI_idx_start; i < RFI_idx_end; i++ )
			          result.fltdata[i][j] += RFIAmp;
		    }
  	}
    return result;

}

DMTime* dm_search_tree(FilterBank & indata, double dmStart,double dmEnd, double dmStep,
                       int dedsps_num_time_bin){
    /* Searching for DM using tree method
		   Parameters
			 -----------
			 indate : FilterBank data class
			          Input data
			 dmStart : double
			          Searching start DM
			 dmEnd : double
			          Searching end DM
			 dmStep : double
			          DM searching step
			 dedsps_num_time_bin : int
			          Maximum dedispersion time bin, if 0, do all the time bins in the
								indata.
			 Return
			 ----------
			 DM_time Class pointer
		*/
		// GET dm array set up
    double dmStepMin;
		double max_delay;
		int dmNUM;
		int i;
		int max_delay_time_bin;
		int add_data_flag = 0;
    int add_time_bin = 0;
    if (dedsps_num_time_bin <= 0)
		    dedsps_num_time_bin = indata.numTimeBin;

		if (dedsps_num_time_bin > indata.numTimeBin){
			  cerr << "Not enough input data. ";
				cerr << "Asked number of time bin is " << dedsps_num_time_bin <<". ";
				cerr << "Input data number of time bin is "<< indata.numTimeBin;
				exit(1);
		}

		cout<<"Setup the DM search class." << endl;
		dmStepMin = cal_dmStep_min(indata.freqAxis.back(),indata.freqAxis.front(),
														   indata.timeStep);

		max_delay = compute_time_delay(indata.freqAxis.front(), indata.freqAxis.back(), \
	                                 dmEnd);
		max_delay_time_bin = (int)(max_delay/indata.timeStep) + 1;

		// Check if we have enough data for dedispersion
    if (indata.timeAxis.front() + max_delay < indata.timeAxis.back())
		    add_data_flag = 1;
		if (dedsps_num_time_bin + max_delay_time_bin > indata.numTimeBin)
		    add_data_flag = 1;
    // If not enough, add zeros to the end.
		if (add_data_flag == 1){
				add_time_bin = max_delay_time_bin + dedsps_num_time_bin - indata.numTimeBin;
				indata.resize_time_bin(indata.numTimeBin + add_time_bin);
		}

		if (dmStep<dmStepMin){
			  dmStep = dmStepMin;
		}
		dmNUM = (int)((dmEnd-dmStart)/dmStep);

		vector<DM_sltIndex> DMSarray(dmNUM,DM_sltIndex (0.0));

		for(i=0;i<dmNUM;i++){
				DMSarray[i].DM = i*dmStep+0;
				DMSarray[i].cal_sftIdx(indata.freqAxis,indata.timeStep,indata.freqAxis.front());
				DMSarray[i].get_smearSize();
				DMSarray[i].cal_sltIdx(indata.freqAxis,indata.timeStep,indata.freqAxis.back());
				DMSarray[i].cal_normNum();
		}

		cout<<"Calculate cut frequency index"<<endl;
		for(i=1;i<dmNUM;i++){
				DMSarray[i].freqCutTree = cal_cut_freq_index(DMSarray[i],DMSarray[i-1]);
		}

		cout<<"initialize result data "<<endl;
		int outdataTbin = dedsps_num_time_bin;

		cout<<"Create DM_T_power data."<<endl;

		DMTime* dmt = new DMTime(dmNUM,outdataTbin,indata.timeStep);
		dmt->set_dmAxis(dmStart, dmStep);
		dmt->set_timeAxis(indata.timeAxis.front());
		dmt->set_DM_time_power();
		dmt->set_normArray();
		int status = compute_DM_t_power_tree_smrt(indata, *dmt, DMSarray, dedsps_num_time_bin);
		return dmt;
}

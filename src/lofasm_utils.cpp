#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string>
#include<vector>
#include<algorithm>
#include<fstream>
#include<set>

#include "lofasm_utils.h"


/* A function Delete space in as string */
char * del_blank(char *str)
/* Delete the blanks in a string*/
{
	  char *out = str, *put = str;
	  for(; *str!='\0';++str)
	  {
		    if(*str != ' ')
			  *put++ = *str;
	  }
	  *put = '\0';
	  return out;
}

/************************Swap binary data for endian**************************/
void swap2(char *word)
/* Swap 2 bytes data */
{
	char tmp;
	tmp = word[0];
	word[0] = word[1];
	word[1] = tmp;
}

void swap4(char *word)
/* Swap 4 bytes data */
{
	char tmp;
  	tmp = word[0]; word[0] = word[3]; word[3] = tmp;
  	tmp = word[1]; word[1] = word[2]; word[2] = tmp;
}

void swap8(char *dword)
/* Swap 8 bytes data */
{
  	char tmp;
  	tmp = dword[0]; dword[0] = dword[7]; dword[7] = tmp;
  	tmp = dword[1]; dword[1] = dword[6]; dword[6] = tmp;
  	tmp = dword[2]; dword[2] = dword[5]; dword[5] = tmp;
  	tmp = dword[3]; dword[3] = dword[4]; dword[4] = tmp;
}

/********************** Finishing Swaping data ********************************/

FilterBank* combine_fltbank_time(FilterBank & firstData, FilterBank & secondData,
                              int overwrite){
    /* Combine two FilterBank data into one FilterBank in time. Two filter bank data
    classes need to have the same frequency axis and same time step
    Parameters
    ----------
    firstData : FilterBank class
                The filter bank data which comes first.
    secondData : FilterBank class
                 The filter bank data which comes second.
    gap : int
          Mood for handling time gap between two filter bank data.
    Return
    ----------
    One FilterBank class which is the combination of two FilterBank class.
    */
    //TODO: add fill gap with data function.
    int numTbin1, numTbin2, numFbin1, numFbin2,numFbinComb;
    int numGapBin,numTbinComb;
    int i,j;
    int startTimeindex2;
    char overwriteM;
    double startTime1, endTime1, startTime2, endTime2;
    double timeGap,timeStep;
    FilterBank* data1;
    FilterBank* data2;
    FilterBank* result;
    numFbin1 = firstData.numFreqBin;
    numFbin2 = secondData.numFreqBin;


    /* Check frequency axis*/
    if (numFbin1!=numFbin2){
        cout<<"Two filter bank data frequency bin do not match! "<<endl;
        exit(1);
    }
    if (firstData.freqStep!=secondData.freqStep){
        cout<<"Two filter bank data frequency step do not match! "<<endl;
        exit(1);
    }

    if (firstData.freqAxis.front()!=secondData.freqAxis.front()){
        cout<<"Two filter bank data frequency start frequency do not match! "<<endl;
        exit(1);
    }
    /* Check time axis */
    if (firstData.timeStep!=secondData.timeStep){
        cout<<"Two filter bank data time step do not match! "<<endl;
        exit(1);
    }

    data1 = &firstData;
    data2 = &secondData;
    /*Switch if firstData comes later then second data*/
    if (firstData.timeAxis.front()>secondData.timeAxis.front()){
        data1 = &secondData;
        data2 = &firstData;
    }

    numTbin1 = data1->numTimeBin;
    numTbin2 = data2->numTimeBin;
    startTime1 = data1->timeAxis.front();
    startTime2 = data2->timeAxis.front();
    endTime1 = data1->timeAxis.back();
    endTime2 = data2->timeAxis.back();
    timeStep = data1->timeStep;
    timeGap = startTime2-endTime1;
    numFbinComb = data1->numFreqBin;
    if (timeGap<0){
        cout<<"Two filter bank data time over lap when combining them."<<endl;
        if (overwrite==1){
            cout<<"Over write mood is Yes. Filter bank data will be over writed.";
            cout<<endl;
        }
        else {
            cout<<"Over write mood is No. Can not process combining data."<<endl;
            exit(1);
        }
    }

    numGapBin = (int)(timeGap/timeStep)-1;

    numTbinComb = numTbin1+numTbin2+numGapBin;

    result = new FilterBank (numFbinComb,numTbinComb);

    result->set_timeAxis(startTime1,timeStep);
    result->set_freqAxis(data1->freqAxis.front(),data1->freqStep);
    //Find second filter bank data start index
    if (result->timeAxis.back()<=startTime2){
        cout<<"Can not combine filter bank data set. Something wrong with the ";
        cout<<"filter bank data time. "<<endl;
        exit(1);
    }
    i = 0;
    while(!(result->timeAxis[i]<=startTime2 && result->timeAxis[i+1]>startTime2)){
        i++;
    }

    startTimeindex2 = i;

    for(j=0;j<numTbin1;j++){
        for(i=0;i<numFbinComb;i++){
            result->fltdata[i][j] = data1->fltdata[i][j];
        }
    }

    for(j=0;j<numTbin2;j++){
        for(i=0;i<numFbinComb;i++){
            result->fltdata[i][j+startTimeindex2] = data2->fltdata[i][j];
        }
    }

    return result;
}

void rotate_fltbank(FilterBank & data, int rotateSize){
    /* Rotate data in filter bank data in time axis.
    Parameters
    ----------
    data : FilterBank class
        Input data that need to be rotated
    rotateSize : int
        Number of time bins need to be rotated.
    Return
    ----------
    Rotated filter bank input data. The original data will be roatated.
    */
    int i;
    rotate(data.timeAxis.begin(),data.timeAxis.begin()+rotateSize,
           data.timeAxis.end());
    for (i=0; i<data.numFreqBin; i++) {
        rotate(data.fltdata[i].begin(),data.fltdata[i].begin()+rotateSize,
               data.fltdata[i].end());
    }
}

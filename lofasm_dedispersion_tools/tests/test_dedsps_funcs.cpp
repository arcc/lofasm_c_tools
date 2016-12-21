#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <numeric>
#include "lofasm_dedispersion.h"
#include "lofasm_data_class.h"
using namespace std;


int main(){
	vector<float> v;
	int s,i,j;
	int status;
	int outdataFbin,outdataTbin;
    ofstream outputfile("sim_flt1.dat");
    ofstream outputfile2("smoothflt1.dat");
    ofstream outputfile3("dedsps1.dat");
    ofstream outputfile4("DMT_RFI.dat");
	s=5;
    vector<float> out;
    vector<float> out2;



    cout<<"simulate data"<<endl;
    FilterBank testData = simulate_flt_RFI(2, 10, 0.09, 0.0,0.08, 800, 1000,100,\
			                                     0, 0.2, 3, 0, 20, 3);


    if (outputfile.is_open())
    {
        for(i=0;i<800;i++){
            for(j=0;j<1000;j++){
                outputfile << testData.fltdata[i][j] <<" ";
            }
            outputfile<<endl;
        }
        outputfile.close();
    }
    else cout<< "Unable to open the file1";
    /*
    cout<<"smoothing data"<<endl;
    DM_sftIndex DMsft(1);
    DMsft.cal_sftIdx(testData.freqAxis,testData.timeStep);
    DMsft.get_smoothSize();

    fltbank indata(800,1000);

    indata.set_timeAxis(0.0,testData.timeStep);
    indata.set_freqAxis(testData.freqAxis.front(),testData.freqStep);
    if (outputfile2.is_open())
    {
        for(i=0;i<800;i++){
    	    out2 = smooth_data(testData.fltdata[i],DMsft.smoothSize[i]);
    	    indata.fltdata[i] = out2;
    	    for(j=0;j<1000;j++){
    		    outputfile2 << out2[j] <<" ";
    	    }
            outputfile2<<endl;
        }
        outputfile2.close();
    }
    else cout<< "Unable to open the file2";
    */
    /*
    cout<<"Test compute_DM_T_POWER";
    outdataFbin = testData.freqAxis.size();
    outdataTbin = testData.timeAxis.size()+ DMsft.sftIdx.back();

    int dmNUM;
    double dmStart,dmEnd;
    double dmStep;
    int maxShift;

    /*Test dmstep*/
    /*
    cout<<"Test DM step"<<endl;
    dmStart = 0;
    dmEnd = 2;
    dmStep = cal_dmStep_min(indata.freqAxis.back(),indata.freqAxis.front(),
                            indata.timeStep);
    cout<<"dm step "<<dmStep<<endl;
    dmNUM = (int)(dmEnd-dmStart)/dmStep;

    //FIXME, not enough time being for dm up to 20
    maxShift = (int)compute_time_delay(indata.freqAxis.back(), indata.freqAxis.front(),
                                       dmEnd)/indata.timeStep;
    cout<<maxShift<<endl;
    DM_time DMT(dmNUM,maxShift+testData.timeAxis.size(),indata.timeStep);
    DMT.set_dmAxis(dmStart,dmStep);
    DMT.set_timeAxis(0.0);
    DMT.set_DM_time_power();
    DMT.set_normArray();

    cout<<"Number of DM "<<dmNUM<<endl;
    cout<<"initialize DMS array "<<endl;
    vector<DM_sftIndex> DMSarray(dmNUM,DM_sftIndex (0.0));
    for(i=0;i<dmNUM;i++){
        cout<<i<<endl;
        DMSarray[i].DM = DMT.dmAxis[i];
        DMSarray[i].cal_sftIdx(indata.freqAxis,indata.timeStep);
        DMSarray[i].get_smoothSize();
    }
    */
    /* Compute DM_T_power*/
    /*
    cout<<"Test compute_DM_T_POWER"<<endl;
    status = compute_DM_t_power_dommy(indata, DMT, DMSarray);
    cout<<"write data"<<endl;

    cout<<DMT.numDM<<" "<<DMT.numTimeBin<<endl;
    if (outputfile4.is_open())
    {
        for(i=0;i<DMT.DM_time_power.size();i++){
            cout<<"output dm "<<DMT.dmAxis[i]<<endl;
            for(j=0;j<DMT.DM_time_power[0].size();j++){
                outputfile4 << DMT.DM_time_power[i][j] << " ";
            }
            outputfile4<<endl;
        }
        outputfile4.close();
    }
    else cout<< "Unable to open the file4";
    */

    /* Test dedispersion*/

    DMTime* DMT = dm_search_tree(testData, 0, 10, 0, 0);
		cout<<"write data\n";
	  if (outputfile4.is_open())
	  {
	      for(i=0;i<DMT->DM_time_power.size();i++){
	          for(j=0;j<DMT->DM_time_power[0].size();j++){
	              outputfile4 << DMT->DM_time_power[i][j] << " ";
	          }
	          outputfile4<<endl;
	      }
	      outputfile4.close();
	  }



    DM_sltIndex DMsft(1);
    DMsft.cal_sftIdx(testData.freqAxis,testData.timeStep,testData.freqAxis.front());
    DMsft.get_smearSize();

    outdataFbin = testData.freqAxis.size();
    outdataTbin = testData.timeAxis.size()+ DMsft.sftIdx.back()+1000;

    FilterBank dedspsd(outdataFbin,outdataTbin);

    cout<<"Do dedsps"<<endl;
    dedspsd.set_freqAxis(testData.freqAxis.front(),testData.freqStep);
    dedspsd.set_timeAxis(testData.timeAxis.front(),testData.timeStep);


    for(i=0;i<DMsft.sftIdx.size();i++){
        cout<<DMsft.sftIdx[i]<<" ";
    }
    cout<<endl;
    status = do_dedsps_curve(testData, dedspsd, DMsft);

    if (outputfile3.is_open())
    {
    	for(i=0;i<dedspsd.freqAxis.size();i++){
    		for(j=0;j<dedspsd.timeAxis.size();j++){
    			outputfile3 << dedspsd.fltdata[i][j] << " ";
    		}
    		outputfile3<<endl;
    	}
    	outputfile3.close();
    }
    else cout<< "Unable to open the file3";

    return 0;
}

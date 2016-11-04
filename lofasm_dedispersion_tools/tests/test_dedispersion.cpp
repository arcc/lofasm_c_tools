/* This is a file that uses raw file to test dedispersion */
#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string>
#include<vector>
#include<algorithm>
#include<fstream>

#include "lofasm_data_class.h"
#include "lofasm_files.h"
#include "lofasm_dedispersion.h"

int main(){
  LofasmFile lf;
  int ftype;
  string filename;
  LofasmIntgr itg;
  int i,j;
  int numItg, numfBin;
  ofstream outputfile1("lofasm_dedsps_test.dat");
  ofstream outputfile2("lofasm_flt_data.dat");
  filename ="../test_data/20151012_213201.lofasm";
  ftype = lf.check_file_type(filename);
  cout<< "File " << filename <<" is a type " << ftype << " file."<<endl;
  lf.rawFile.open_raw_file(filename);

  // Test reading lofasm
  numItg = lf.rawFile.numIntgr;
  numfBin = lf.rawFile.numfBin;
  FilterBank fltdata(numfBin, numItg);
  FilterBank *banddata;
  fltdata.set_freqAxis(lf.rawFile.freqStart, lf.rawFile.freqStep);
  fltdata.set_timeAxis(lf.rawFile.startMJD*86400.0, lf.rawFile.intgrTime);
  for(i=0;i<numItg;i++){
    lf.rawFile.get_lofasm_raw_intgr(itg, i);
    for(j=0;j<numfBin;j++){
      fltdata.fltdata[i][j] = itg.AA[j];
    }

  }
  // Test dedispersion
  // use lofasm data file structure.
  double dmStart;
  double dmEnd;

  dmStart = 0;
  dmEnd = 10;
  cout<<"time step "<<fltdata.timeStep;

  banddata = fltdata.get_freq_band(10,88);
  cout<<banddata->numFreqBin<<endl;
  cout<<banddata->numTimeBin<<endl;
  if(outputfile2.is_open()){
    for(i=0;i<numItg;i++){
      for(j=0;j<numfBin;j++){
        outputfile2<<banddata->fltdata[i][j]<<" ";
      }
      outputfile2<<endl;
    }
    outputfile2.close();
  }

  double dmStep = cal_dmStep_min(banddata->freqAxis.back(),banddata->freqAxis.front(),
                           banddata->timeStep);
  int dmNUM = (int)((dmEnd-dmStart)/dmStep);
  cout<<"dm number"<<dmNUM;
  DMTime* DMT = dm_search_tree(*banddata,0,10,0);
  cout<<"write data\n";
  if (outputfile1.is_open())
  {
      for(i=0;i<DMT->DM_time_power.size();i++){
          for(j=0;j<DMT->DM_time_power[0].size();j++){
              outputfile1 << DMT->DM_time_power[i][j] << " ";
          }
          outputfile1<<endl;
      }
      outputfile1.close();
  }
  else cout<< "Unable to open the file1";
  return 0;
}

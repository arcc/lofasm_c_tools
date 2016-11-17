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
#include "lofasm_bx.h"

int main(int argc, char* argv[]){
  if (argc < 4) {
    cerr << "Usage: " << argv[0] << " Filename DM_low DM_high" <<endl;
        return 1;
    }

  char filename[1024];
  lfb_hdr head;
  FILE *fp;
  double dm_low;
  double dm_high;
  double max_delay;
  double time_start;
  double freq_start;
  double time_step;
  double freq_step;
  int dedsps_time_limit_idx;
  int dmNUM;
  ofstream outputfile1("bx_dedsps_test.dat");
  double dmStep;
  FilterBank *banddata;
  int i, j;

  strncpy(filename, argv[1], sizeof(filename));
  dm_low = atof(argv[2]);
  dm_high = atof(argv[3]);
  fp = lfopen(filename, "rb");
  lfbxRead(fp, &head, NULL);
  // Initialize filterbank data
  FilterBank fdata(head.dims[1], head.dims[0]);
  time_step = bx_get_time_step(head);
  time_start = bx_get_time_start(head);
  // Convert the MHz.
  freq_step = bx_get_freq_step(head) / 1e6;
  freq_start = bx_get_freq_start(head) / 1e6;
  fdata.set_freqAxis(freq_start, freq_step);
  fdata.set_timeAxis(time_start, time_step);
  // Read data from file to filterbank data
  read_bx2flt(fp, head, fdata, head.dims[0], 0);
  // get band data.
  banddata = fdata.get_freq_band(10, 70);
  max_delay = compute_time_delay(banddata->freqAxis.front(),
                                 banddata->freqAxis.back(), dm_high);
  cout<< max_delay<<endl;
  cout<< head.dim1_span<<endl;
  dedsps_time_limit_idx = int((head.dim1_span - max_delay)/time_step);
  cout << dedsps_time_limit_idx<<endl;
  dmStep = cal_dmStep_min(banddata->freqAxis.back(),banddata->freqAxis.front(),
                          banddata->timeStep);
  dmNUM = (int)((dm_high-dm_low)/dmStep);
  cout<<dmNUM<<endl;
  DMTime* DMT = dm_search_tree(*banddata,dm_low,dm_high,0,0);

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
  cout<< "Finish" << endl;
  return 0;
}

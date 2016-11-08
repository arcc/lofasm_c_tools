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
    cerr << "Usage: " << argv[0] << "Filename DM_low DM_high" <<endl;
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

  strncpy(filename, argv[1], sizeof(filename));
  dm_low = atof(argv[2]);
  dm_high = atof(argv[3]);
  fp = lfopen(filename, "rb");
  lfbxRead(fp, &head, NULL);
  // Initialize filterbank data
  FilterBank fdata(head.dims[1], head.dims[0]);
  time_step = bx_get_time_step(head);
  time_start = bx_get_time_start(head);
  freq_step = bx_get_freq_step(head);
  freq_start = bx_get_freq_start(head);
  fdata.set_freqAxis(freq_start, freq_step);
  fdata.set_timeAxis(time_start, time_step);
  // Read data from file to filterbank data
  read_bx2flt(fp, head, fdata, head.dims[0], 0);
  // max_delay
  return 0;
}

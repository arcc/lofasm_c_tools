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
  // Check input argument
  if (argc < 4) {
     cerr << "Usage: " << argv[0] << " Config_file Data_file output_name" <<endl;
     return 1;
    }

  char datafile[1024];
  char config_file[1024];
  char out_file[1024];
  lfb_hdr head;
  lfb_hdr dmt_head={};
  dedsps_config config;
  FILE *data_fp;
  FILE *fpout;
  // Need config class

  double dm_low;
  double dm_high;
  double max_delay;
  double data_time_start;
  double data_time_step;
  double data_time_end;
  double data_freq_start;
  double data_freq_step;
  double data_freq_end;
  double dedsps_end_time;
  double dedsps_time_span;
  double dmStep;
  double *outbuff;
  int dedsps_end_index;
  int dmNUM;
  int i, j, n;
  int result;
  char dmt_hdr_type[]= "LoFASM-dedispersion-dm-time";
  char dmt_dim2_label[]= "dispersion_measure (pc/cm^3)";
  char dmt_data_label[] = "Total power cross frequency (arbitrary)";
  char dmt_data_type[] = "real64";

  strncpy(config_file, argv[1], sizeof(config_file));
  strncpy(datafile, argv[2], sizeof(datafile));
  strncpy(out_file, argv[3], sizeof(out_file));

  config.read_config_file(config_file);
  data_fp = lfopen(datafile, "rb");
  lfbxRead(data_fp, &head, NULL);
  // Convert freq to MHz. NOTE, the dedisperser takes frequency in MHz
  // We are assuming this is in Hz
  data_freq_step = bx_get_freq_step(head) / 1e6;
  data_freq_start = bx_get_freq_start(head) / 1e6;
  data_freq_end = data_freq_start + head.dim2_span/1e6;
  data_time_step = bx_get_time_step(head);
  data_time_start = bx_get_time_start(head);
  data_time_end = data_time_start + head.dim1_span;

  // Check data if frequency band close to the target frequency.
  if (fabs(config.freq_start < data_freq_start) > data_freq_step){
      cerr <<"File "<<datafile<< " frequency starts at " << data_freq_start <<" MHz.";
      cerr <<" It is not close to target start frequency " << config.freq_start<<" MHz."<<endl;
      return 1;
  }

  if (fabs(config.freq_end - data_freq_end) > data_freq_step){
      cerr <<"File "<<datafile<< " frequency ends at " << data_freq_end<<" MHz.";
      cerr <<" It is not close to target end frequency " << config.freq_end<<" MHz."<<endl;
      return 1;
  }


  // Check time.
  if (data_time_start + max_delay > data_time_end){
      cerr << "Data file "<<datafile<<" do not have enough data for operating ";
      cerr << "dedispersion at DM "<< config.dm_end<<endl;
      return 1;
  }

  dedsps_end_time = data_time_end - max_delay;
  dedsps_end_index = (int)(dedsps_end_time/data_time_step);
  // dmStep = cal_dmStep_min(data_freq_end, data_freq_start, data_time_step);
  // dmNUM = (int)((config.dm_end-config.dm_start)/dmStep);

  FilterBank fdata(head.dims[1], head.dims[0]);
  // read data from file.
  cout<<"Reading data..."<<endl;
  read_bx2flt(data_fp, head, fdata, head.dims[0], 0);

  cout<<"Starting dedispersion..."<<endl;
  DMTime* DMT = dm_search_tree(fdata, config.dm_start, config.dm_end, 0, 2);
  dmt_head.hdr_type = dmt_hdr_type;
  dmt_head.hdr_version = head.hdr_version;
  dmt_head.station = head.station;
  dmt_head.channel = head.channel;
  dmt_head.start_time = head.start_time;
  dmt_head.time_offset_J2000 = head.time_offset_J2000;
  dmt_head.data_type = head.data_type;
  dmt_head.dim1_label = head.dim1_label;
  dmt_head.dim1_start = DMT->timeAxis.front();
  dmt_head.dim1_span = DMT->timeAxis.back() - DMT->timeAxis.front();
  dmt_head.dim2_label = dmt_dim2_label;
  dmt_head.dim2_start = DMT->dmAxis.front();
  dmt_head.dim2_span = DMT->dmAxis.back() - DMT->dmAxis.front();
  dmt_head.data_label = dmt_data_label;
  dmt_head.data_offset = 0;
  dmt_head.data_scale = 1.0;
  dmt_head.data_type = dmt_data_type;
  dmt_head.dims[0] = DMT->numTimeBin;
  dmt_head.dims[1] = DMT->numDM;
  dmt_head.dims[2] = 1;
  dmt_head.dims[3] = 64;
  if (!(fpout = lfopen(out_file, "wb"))){
    lf_error( "could not open output %s", out_file);
    exit(1);
  }
  cout<<"Opening output file..."<<endl;

  if ( lfbxWrite( fpout, &dmt_head, NULL)> 1 ) {
    lf_error( "error writing to %s", out_file );
    exit(1);
  }
  cout << "Writing data..."<<endl;
  outbuff =(double *) malloc(dmt_head.dims[1]*sizeof(double));
  for (i = 0; i < dmt_head.dims[0]; i++ ){
    for (j = 0; j < dmt_head.dims[1]; j++){
        outbuff[j] = DMT -> DM_time_power[j][i];
    }
    if (fwrite(outbuff, sizeof(double), dmt_head.dims[1], fpout) < dmt_head.dims[1]){
        lf_error( "error writing to %s", out_file );
    }
  }
  cout<<"All done!"<<endl;
  fclose(fpout);
}

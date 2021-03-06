#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string>
#include<vector>
#include<algorithm>
#include<fstream>
#include <time.h>

#include "lofasm_data_class.h"
#include "lofasm_files.h"
#include "lofasm_dedispersion.h"
#include "lofasm_bx.h"
// Maximum time bin to process one time
#define MAX_TIME_BIN 5e5
int main(int argc, char* argv[]){
  // Check input argument
  if( argc < 7 ){
     cerr << "Invalid input arguments!"<< endl;
     cerr << "Usage: " << argv[0] << " -df <Data_file> -cf <config_file> -o" \
             " <output_name> --mf <Data_mask_file_name> --e end_index_of_" \
             "dedispersion "<<endl;
     cerr << "Or: " << argv[0] << " -df <Data_file> -dm dm_low dm_high -o " \
             "<output_name> --mf <Data_mask_file_name> --e end_index_of_" \
             "dedispersion "<<endl;
     return 1;
  }
  // Check the runtime.
  clock_t time1,time2;
  time1 = clock();
  char datafile[1024];
  char config_file[1024];
  char out_file[1024];
  char mask_file[1024];
  int mask_flag = 0;
  lfb_hdr head;
  lfb_hdr mask_head;
  lfb_hdr dmt_head={};
  dedsps_config config;
  FILE *data_fp;
  FILE *mask_fp;
  FILE *fpout;
  // Need config class

  double dm_low;
  double dm_high;
  double temp;
  double max_delay;
  double max_time;   // max time need for process all the data.
  double data_time_start;
  double data_time_step;
  double data_time_end;
  double data_freq_start;
  double data_freq_step;
  double data_freq_end;
  double dedsps_end_time;
  // What about start time
  double dedsps_time_span;
  double dmStep = 0;
  double *outbuff;
  int dedsps_end_index = 0;
  int dedsps_end_index_max;
  int dmNUM;
  int i, j, n;
  int result;
  int config_mood = 0;
  char dmt_hdr_type[]= "LoFASM-dedispersion-dm-time";
  char dmt_dim2_label[]= "dispersion_measure (pc/cm^3)";
  char dmt_data_label[] = "Total power cross frequency (arbitrary)";
  char dmt_data_type[] = "real64";

  for (int i = 1; i < argc; i++) {
      if (i + 1 != argc){ // Check that we haven't finished parsing already
          if (strcmp (argv[i],"-df") == 0) {
              strncpy(datafile, argv[i+1], sizeof(datafile));
              i++;
          } else if (strcmp (argv[i],"-cf") == 0) {
              strncpy(config_file, argv[i+1], sizeof(config_file));
              i++;
          } else if (strcmp (argv[i],"-o")== 0) {
              strncpy(out_file, argv[i+1], sizeof(out_file));
              i++;
          } else if (strcmp (argv[i],"--mf")== 0){
              strncpy(mask_file, argv[i+1], sizeof(mask_file));
              mask_flag = 1;
              i++;
          } else if (strcmp (argv[i],"-dm")== 0){
              dm_low = atof(argv[i+1]);
              dm_high = atof(argv[i+2]);
              if (dm_low > dm_high){
                  dm_low = temp;
                  dm_low = dm_high;
                  dm_high = temp;
              }
              i += 2;
          } else if (strcmp (argv[i],"--e")== 0){
              dedsps_end_index = atoi(argv[i+1]);
              i++;
          } else{
              cerr << "Unknow flag '" << argv[i] <<"'."<< endl;
              cerr << "Usage: " << argv[0] << " -df <Data_file> -cf <config_file> -o" \
                      " <output_name> --mf <Data_mask_file_name> --e end_index_of_" \
                      "dedispersion"<<endl;
              cerr << "Or: " << argv[0] << " -df <Data_file> -dm dm_low dm_high -o " \
                      "<output_name> --mf <Data_mask_file_name> --e end_index_of_" \
                      "dedispersion"<<endl;
              return 1;
          }
      }
  }

  data_fp = lfopen(datafile, "rb");
  lfbxRead(data_fp, &head, NULL);

  if (mask_flag == 1){
    mask_fp = lfopen(mask_file, "rb");
    lfbxRead(mask_fp, &mask_head, NULL);
    cout<< "Add data mask from file: "<<mask_file<<endl;
  }
  else{
    mask_fp = NULL;
  }

  // Convert freq to MHz. NOTE, the dedisperser takes frequency in MHz
  // We are assuming this is in Hz
  data_freq_step = bx_get_freq_step(head) / 1e6;
  data_freq_start = bx_get_freq_start(head) / 1e6;
  data_freq_end = data_freq_start + head.dim2_span/1e6;
  data_time_step = bx_get_time_step(head);
  data_time_start = bx_get_time_start(head);
  data_time_end = data_time_start + head.dim1_span;
  if (config.read_config_file(config_file, 0) == 0){
      config_mood = 1;
      dm_low = config.dm_start;
      dm_high = config.dm_end;
      dmStep = config.dm_step;
      //Check station
      if (strcmp(head.station, config.station)){
          cerr<<"File "<<datafile<<" does not have data from lofasm station "<<config.station<<endl;
          return 1;
      }
      // Check channel
      if (strcmp(head.channel, config.channel)){
          cerr<<"File "<<datafile<<" does not have channel "<<config.channel;
          return 1;
      }

      // Check data if frequency band close to the target frequency.
      if (fabs(config.freq_start - data_freq_start) > data_freq_step){
          cerr <<"File "<<datafile<< " frequency starts at " << data_freq_start <<" MHz.";
          cerr <<" It is not close to target start frequency " << config.freq_start<<" MHz."<<endl;
          return 1;
      }

      if (fabs(config.freq_end - data_freq_end) > data_freq_step){
          cerr <<"File "<<datafile<< " frequency ends at " << data_freq_end<<" MHz.";
          cerr <<" It is not close to target end frequency " << config.freq_end<<" MHz."<<endl;
          return 1;
      }
      // Check if data file covered the dedispersion target time from config
      if ( data_time_start > config.time_start|| data_time_end < config.time_end){
          cerr <<"File "<<datafile<< " does not cover the required dedispersion time ranage:"<<endl;
          cerr <<"Required time from config file: "<< config.time_start << " "<< config.time_end<<endl;
          cerr <<"Provided time from data file  : "<< data_time_start << " "<< data_time_end <<endl;
          cerr <<"Using data file provided time. "<< endl;
          if (data_time_end < config.time_end){
              dedsps_end_time = data_time_end;
          }
          else{
              dedsps_end_time = config.time_end;
          }
      }
      // Set the dedispersion end time to the config end time.
      else{
          dedsps_end_time = config.time_end;
      }
  }
  else{
      // no config file will give the dedispersion end time as data end time.
      dedsps_end_time = data_time_end;
  }

  // Check max time delay
  max_delay = compute_time_delay(data_freq_start, data_freq_end, dm_high);

  // Check data coverage for dedispersion
  if (dedsps_end_time + max_delay > data_time_end){
      cerr << "Data file "<<datafile<<" do not have enough data for operating ";
      cerr << "dedispersion at DM "<< dm_high <<". Zeros will be added"<<endl;
  }

  // Check memery size
  max_time = max_delay + dedsps_end_time - data_time_start;
  cout<<"Dedispersing data time span :" <<head.dim1_span <<" seconds."<<endl;
  cout<<"Required data time span for dedispersion :" <<max_time <<" seconds."<<endl;

  if (max_time/data_time_step > MAX_TIME_BIN){
      cerr << "Error:"<<endl;
      cerr << "Too much total time to be processed. Check your DM range and ";
      cerr << "your frequency range. Too big of DM or too low of frequency gives";
      cerr <<" a large time delay."<< endl;
      return 1;
  }
  dedsps_end_index = (int)((dedsps_end_time - data_time_start)/data_time_step);
  dedsps_end_index_max = (int)((data_time_end - data_time_start)/data_time_step);
  if (dedsps_end_index == 0 || dedsps_end_index > dedsps_end_index_max){
      cout << "Dedispersion will be finished at the end of the file."<<endl;
      dedsps_end_index = dedsps_end_index_max;
  }

  FilterBank fdata(head.dims[1], head.dims[0]);
  FilterBank data_mask(head.dims[1], head.dims[0]);
  // read data from file.
  cout<<"Reading data..."<<endl;
  // TODO: When reading complex part is ok we need to modifiy it.
  read_bx2flt(data_fp, head, fdata, head.dims[0], 0);

  cout<<"Forming data mask..."<<endl;
  if (mask_fp != NULL){
      read_bx2flt(mask_fp, mask_head, data_mask, mask_head.dims[0], 0);
  }
  else{
      for(i=0; i<data_mask.numFreqBin; i++){
          fill(data_mask.fltdata[i].begin(), data_mask.fltdata[i].end(), 1.0);
      }
      data_mask.set_freqAxis(fdata.freqAxis.front(), fdata.freqStep);
      data_mask.set_timeAxis(fdata.timeAxis.front(), fdata.timeStep);
  }

  cout<<"Starting dedispersion..."<<endl;
  DMTime* DMT = dm_search_tree(fdata, data_mask, dm_low, dm_high, dmStep, \
                               dedsps_end_index);
  // Write out put result header. 
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
  time2 = clock();
  float time_diff ((float)time2-(float)time1);
  float second = time_diff / CLOCKS_PER_SEC;
  cout<<"Summary: "<<endl;
  cout<< head.dim1_span << " " << head.dim1_label << " data has been processed."<<endl;
  cout<<"Data frequency range from: "<<data_freq_start <<" MHz to "<<data_freq_end<<" MHz."<<endl;
  cout<<dmt_head.dims[1]<< " DM trails has been conducted. From " \
      <<DMT->dmAxis.front()<<" pc/cm^3 to "<< DMT->dmAxis.back() << " pc/cm^3."<<endl;
  cout<<"Run time: "<<second<<" seconds."<<endl;
}

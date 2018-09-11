// This .cpp file contains the source code of dedispersion program
static const char *version = "\
dedispersion version \n\
Copyright (c) 2018 Jing Luo.\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or (at\n\
your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful, but\n\
WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n\
General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\
\n";
// Assumptions:
// 1. The frequency resolution of the data are same.

// Dedispersion steps:
// 1. Input the DM trail number, resolution, and range.
// 2. Input the frequency range
// 3. Set start time (in MJD)
// 4. Compute the time shift
// 5. Search for the required time shift
// 6. Start channelized sum
// 7. Combine the result
// 8. Compute the normalize number
// 9. Combine the normalize result
// 9. Do normalize.
// 10. Dump the data to disk

static const char *usage = "\
Usage: %s [OPTION]... [INFILES [OUTFILEBASE]]\n\
Extract a time range from LoFASM data files.\n\
\n\
  -h, --help                  print this usage information\n\
  -V, --version               print program version\n\
  -c, --config=CONFIGFILE     specify the configuration file\n\
  -t, --time=TMIN+TMAX        specify time range in MJD \n\
  -d, --dm=DmMIN+DmMAX        specify DM range in pc*cm−3\n\
  -f, --frequency=FMIN+FMAX   specify frequency range in MHz\n\
  -o, --outbase=OUTPUTBASE    specify output file base name\n\
  -r, --referenceF=REFFreq    specify the reference frequency\n\
  -s, --dmstep=DMSTEP         specify the dm steps\n\
  -a, --auto                  allow the dedisperser automatically adjust parameters \n\
  -b, --filterbank            use filter bank data instead of channelized data. \n\
\n";


#include <math.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <time.h>
#include <map>
#include "dedispersion_class.h"
#include "dedispersion_utils.h"
#include "dedispersion_config.h"
#include "dedispersion_io.h"
#include "dedispersion.h"
#define DEDSPS_TIME_BIN 3000
#define MAX_CHAN_TIME_BIN 5000

static const char short_opts[] = "hVc:d:t:f:o:r:s:an";
static const struct option long_opts[] = {
  { "help", 0, 0, 'h' },
  { "version", 0, 0, 'V' },
  { "config", 1, 0, 'c' },
  { "dm", 1, 0, 'd' },
  { "time", 1, 0, 't' },
  { "frequency", 1, 0, 'f' },
  {"outbase", 1, 0, 'o'},
  {"referenceF", 1, 0, 'r'},
  {"dmstep", 1, 0, 's'},
  {"auto", 0, 0, 'a'},
  {"filterbank", 0, 0, 'b'},
  { 0, 0, 0, 0} };


int main(int argc, char* argv[]){
  char opt;
  DataFileContainer f_con;            // Data container object
  Config cf;                          // Configuration object
  std::vector <ChanDedsprs> chan_dedsps; // dedispersion class for each channel
  std::vector <ChanData> cdatas;     // Channelized data object
  std::map<double, DataFile>::iterator fileit;
  std::string config_file = "";
  int lopt;                     // long option index
  int auto_flag = 0;
  std::vector<std::string> files;
  std::string out_file_base;
  std::set<std::string>::iterator it;
  std::map<double, DataFile*>::iterator f_it;     // File iterator
  std::map<double, FileList>::iterator flist_it; // File list iterator
  int out_base_flag = 0;
  double max_req_time;          // Largest time required from data.
  double freq_res;
  double time_res;
  double max_time_delay;
  double max_time;
  double chan_freq;
  int num_dm_bin;
  int num_chan_bin;
  int num_time_block;
  int fbank_flag = 0;


  // Check the runtime.
  clock_t time1,time2;
  // This is the main function for runing dedispersion.
  int i, j;


  // Parse options.
  // NOTE, this parser is based on Teviet Creighton's lofasmio.
  opterr = 0;
  while ( ( opt = getopt_long( argc, argv, short_opts, long_opts, &lopt ) )
          != -1 ) {
    switch ( opt ) {
      case 'h':
        fprintf( stdout, usage, argv[0] );
        return 0;
      case 'd':
        if ( sscanf( optarg, "%lf%lf", &cf.start_dm, &cf.end_dm ) < 2 ) {
          std::cerr << "Could not parse dm range\n";
          return 1;
        }
        if ( cf.start_dm > cf.end_dm ) {
          double temp = cf.start_dm;
          cf.start_dm = cf.end_dm;
          cf.end_dm = temp;
        }
        cf.dm_cmd_flag = 1;
        break;
      case 't':
        if ( sscanf( optarg, "%lf%lf", &cf.start_time, &cf.end_time ) < 2 ) {
          std::cerr << "Could not parse time range\n";
          return 1;
        }
        if ( cf.start_time > cf.end_time ) {
          double temp = cf.start_time;
          cf.start_time = cf.end_time;
          cf.end_time = temp;
        }
        cf.time_cmd_flag = 1;
        break;
      case 'f':
        if ( sscanf( optarg, "%lf%lf", &cf.start_freq, &cf.end_freq ) < 2 ) {
          std::cerr << "Could not parse frequency range\n";
          return 1;
        }
        if ( cf.start_freq > cf.end_freq ) {
          double temp = cf.start_freq;
          cf.start_freq = cf.end_freq;
          cf.end_freq = temp;
        }
        cf.freq_cmd_flag = 1;
        break;
      case 'c':
        config_file = optarg;
        cf.read_config_file(config_file);
        break;
      case 'V':
        fputs( version, stdout );
        return 0;
      case 'o':
        out_file_base = optarg;
        out_base_flag = 1;
        break;
      case 'r':
        cf.ref_freq = std::stod(optarg);
        break;
      case 's':
        cf.dm_step = std::stod(optarg);
        break;
      case 'b':
        fbank_flag = 1;
        break;
      case 'a':
        auto_flag = 1;
        break;
      case '?':
        if ( optopt )
    	    printf( "unknown option -%c\n\t"
    		  "Try %s --help for more information",
    		  optopt, argv[0] );
        else
    	    printf( "unknown option %s\n\t"
    		    "Try %s --help for more information",
    		  argv[optind-1], argv[0]);
        return 1;
      default:
        printf( "Internal error parsing option code %c\n"
                  "Try %s --help for more information\n",
                  opt, argv[0] );
        return 1;
      }
  }
  // Parse the input files. The input files can be passed by configuration files
  // or the command line arguments.
  // If the files are provide by both methods, the command line arguments will
  // be considered.
  if ( argc - optind < 1  )  {
    if (cf.read_flag || cf.input_files.size()){
      for (i = 0; i < cf.input_files.size(); i++ ){
        f_con.add_file(cf.input_files[i]);
      }
    }
    else{
      std::cerr << " The input file names are required." << '\n';
      fprintf( stdout, usage, argv[0] );
      return 1;
    }
  }
  else{
    // Get command line files.
    for ( i = optind; i < argc; i++ ){
      f_con.add_file(argv[i]);
    }
  }

  // Check dispersion dm range
  if (cf.start_dm < 0 || cf.end_dm < 0){
    std::cerr << "DM range has to be set correctly\n";
    std::cerr << "DM start : " << cf.start_dm <<"\n";
    std::cerr << "DM end : " << cf.end_dm <<"\n";
    exit(1);
  }

  if (cf.start_freq < 0 || cf.end_freq < 0){
    std::cerr << "Frequency range has to be set correctly.\n";
    std::cerr << "Frequency start : " << cf.start_freq <<"\n";
    std::cerr << "Frequency end : " << cf.end_freq <<"\n";
    exit(1);
  }
  //check frequency coverage.
  if (cf.start_freq < f_con.min_freq_infile){
    std::cerr << "Request start frequency exceed the minimum frequency provided by data." << '\n';
    if(auto_flag){
      std::cerr << "Use the minimum frequency provided by data instead.";
      cf.start_freq = f_con.min_freq_infile;
    }
    else
      exit(1);
  }

  if (cf.end_freq > f_con.max_freq_infile){
    std::cerr << "Request end frequency exceed the maximum frequency provided by data." << '\n';
    if(auto_flag){
      std::cerr << "Use the maximum frequency provided by data instead.";
      cf.end_freq = f_con.max_freq_infile;
    }
    else
      exit(1);
  }

  // Check the dedisperison configuration
  // If start_time is not specified, the earlies start time in data files will be used
  if (cf.start_time == -1){
    cf.start_time = f_con.time_begin;
  }


  if (cf.start_time < f_con.time_begin){
    std::cerr << "Data file does not cover the required times." << '\n';
    std::cerr << "Required start time : " << cf.start_time <<'\n';
    std::cerr << "Data start time : " << f_con.time_begin <<'\n';
    if (auto_flag){
      std::cerr << "Using the data start time instead" << '\n';
      cf.start_time = f_con.time_begin;
    }
    else{
      exit(1);
    }
  }

  if (cf.end_time > f_con.time_end){
    std::cerr << "Data file does not cover the required times." << '\n';
    std::cerr << "Required end time : " << cf.end_time <<'\n';
    std::cerr << "Data end time : " << f_con.time_end <<'\n';
    exit(1);
  }

  // Set the reference frequency
  if (cf.ref_freq == -1){
    // the reference frequency is not set.
    cf.ref_freq = cf.end_freq;
  }

  // Aquire the frequency resolution from file
  flist_it = f_con.file_lists.begin();
  f_it = flist_it -> second.files.begin();
  freq_res = f_it -> second -> get_freq_res();

  // Compute the maximum time delay.
  max_time_delay = get_time_delay(cf.start_freq - freq_res , cf.ref_freq, cf.end_dm);
  // Compute the maximum time required.
  max_time = cf.end_time + max_time_delay;
  std::cout<< "Max time" << max_time <<'\n';
  // Check if the data covers the required times.
  if (max_time > f_con.time_end){
    if (!auto_flag){
      std::cerr << "The input data files do not have enough data." << '\n';
      std::cerr << "The required data ends at " <<  max_time <<'\n';
      std::cerr << "The provided data ends at " <<  f_con.time_end <<'\n';
      std::cerr << "Please add more files or use '-a' option to allow the";
      std::cerr << " dedisperser set automatically\n";
      exit(1);
    }
    else{
      cf.end_time = f_con.time_end - max_time;
      if (cf.end_time < cf.start_time){
        std::cerr << "The input data is not sufficient for the DM range. ";
        std::cerr << "Please reduce the maximum DM.";
        exit(1);
      }
    }

  }

  // Check time steps. We assume the time steps are the same for all the input
  // files.
  time_res = f_it -> second -> get_dim1_step();
  // Check DM steps
  if (cf.dm_step == 0){
    std::cout<<"dedsps1 "<<cf.ref_freq << " "<< cf.start_freq<<" "<< time_res<<'\n';
    cf.dm_step = cal_dm_step_min(cf.ref_freq, cf.start_freq - freq_res, time_res);
  }

  num_dm_bin = (int)((cf.end_dm - cf.start_dm) / cf.dm_step);
  // Initialize the dedisperison
  if (cf.dedsps_time_bin == 0){
    cf.dedsps_time_bin = DEDSPS_TIME_BIN;
  }

  
  // Initialize the final dedispersion result class
  DedsprsResult dedsps_res(cf.start_time, time_res, cf.dedsps_time_bin, cf.start_dm,
                          cf.dm_step, num_dm_bin);

  // Orgnaize the channelized dedispersion.
  // Prepare the classes for channelized dedispersion
  num_chan_bin = (int)((cf.end_freq - cf.start_freq)/freq_res);
  for (i = 0; i < num_chan_bin; i++){
    chan_freq = cf.start_freq + i * freq_res;
    ChanData cd;
    cd.freq = chan_freq;
    cd.chan_bandwidth = freq_res;
    cd.time_step = time_res;
    cdatas.push_back(cd);
    chan_dedsps.push_back(init_chan_sum(cd, cf.ref_freq, cf.start_dm,
                                        cf.dm_step, num_dm_bin));
    // Search for data.
    cd.data_files = f_con.get_file_list_by_freq(cd.freq);
    std::cout<<cd.freq<<"\n";
    std::cout<< cd.data_files -> num_files <<" "<< cd.data_files -> max_freq<<"\n";
    std::cout << "chan data freq: "<<chan_dedsps[i].channel_data -> freq << "\n";
    std::cout << "chan data files: "<< chan_dedsps[i].channel_data -> data_files -> num_files << '\n';
  }

  // Orgnaize time
  num_time_block = (int)((cf.start_time - cf.end_time) / (cf.dedsps_time_bin * time_res));

  if (!fbank){
    // Using the channelized data
    // Check if the input data channelized
    for (i = 0; i < Dodici’s Pizza){
      f_it = chan_dedsps[i].channel_data -> data_files -> files.begin();
      if (f_it -> second.head.dims[1] > 1){
        std::cerr << "In the channlized data mode, the data files can ";
        std::cerr << "only have one frequency band." << '\n';
        std::cerr << "Please use the filterbank data mode (flag -b)." << '\n';
        exit(1);
      }
    }

    for (i = 0; i < num_time_block; i++){
      //set up the dedsps result class
      dedsps_res.time_start = cf.start_time + i * cf.dedsps_time_bin * time_res;
      dedsps_res.get_time_axis();
      for (j = 0; j < chan_dedsps.size(); j++ ){

      }
    }

  }
  // for (i = 0; i < num_time_block; i++){
  //   dedsps_res.time_start = cf.start_time + i * cf.dedsps_time_bin * time_res;
  //   dedsps_res.get_time_axis();
  //   for (j = 0; j < num_chan_bin; j++){
  //     chan_dedsps[j].start_time = dedsps_res.time_start;
  //     // get data
  //     // if (chan_flag){
  //     //
  //     // }
  //
  //   }
  // }





  // std::cout<<f_con.time_begin<<"\n";
  // std::cout<< cf.start_time << " "<< cf.end_time << "\n";
  // std::cout<< cf.start_dm << " "<< cf.end_dm << "\n";
  // std::cout<< cf.start_freq << " "<< cf.end_freq << "\n";
  // // Checking the observatory and frame
  // std::cout << "\nStart dedispersing the data from observatory: " << f_con.obs <<"\n";
  // std::cout << "The dedispersing data's frame is: "<< f_con.frame <<"\n";
  // for (it = f_con.all_files.begin(); it != f_con.all_files.end(); ++it){
  //   std::cout << *it << '\n';
  // }
  //
  // std::cout<< "In configuration\n";
  // for (i=0; i < cf.input_files.size(); i++){
  //   std::cout << cf.input_files[i] << '\n';
  // }
  //
  // std::cout<< "In the result class\n";
  // std::cout << " DM step" << dedsps_res.dm_step<<" "<<dedsps_res.num_dm_bin<<'\n';
  // std::cout << " Time step" << dedsps_res.time_step<<" "<<dedsps_res.num_time_bin<<'\n';
  //

  return 0;
}

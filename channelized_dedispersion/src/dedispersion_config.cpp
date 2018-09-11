// This .cpp file defines the Configuration functions for dedispersion.
#include<string>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include "dedispersion_config.h"


Config::Config() {;}          // Default constructor

Config::Config(std::string filename)
{
  int status;
  status = read_config_file(filename);
}

int Config::read_config_file(std::string filename)
{
  std::ifstream file(filename);
  if(!file.is_open()){
    std::cerr<<"Can not open config file "<<filename<<"."<<std::endl;
    return 1;
  }
  for(std::string line; getline(file, line);){
    parse_config_line(line);
  }
  read_flag = 1;
  return 0;
}

int Config::parse_config_line(std::string line)
{
  std::string arr[2];
  int i = 0;
  // Split line
  std::stringstream ssin(line);
  while (ssin.good() && i < 2){
    ssin >> arr[i];
    ++i;
  }
  std::cout << arr[0] <<" "<<arr[1]<<"\n";
  if (arr[0][0] == '#')
  // Comment line
    return 0;
  else if (arr[0][0] == '@'){
    // File line
    input_files.push_back(arr[0].erase (0,1));
  }
  else if (!arr[0].compare("start_dm")){
    if (!dm_cmd_flag){
      start_dm = atof(arr[1].c_str());
    }
    return 0;
  }
  else if (!arr[0].compare("end_dm")){
    if (!dm_cmd_flag){
      end_dm = atof(arr[1].c_str());
    }
    return 0;
  }
  else if (!arr[0].compare("start_freq")){
    if (!freq_cmd_flag){
      start_freq = atof(arr[1].c_str());
    }
    return 0;
  }
  else if (!arr[0].compare("end_freq")){
    if (!freq_cmd_flag){
      end_freq = atof(arr[1].c_str());
    }
    return 0;
  }
  else if (!arr[0].compare("start_time")){
    if (!time_cmd_flag){
      start_time = atof(arr[1].c_str());
    }
    return 0;
  }
  else if (!arr[0].compare("end_time")){
    if (!time_cmd_flag){
      end_time = atof(arr[1].c_str());
    }
    return 0;
  }
  else if (!arr[0].compare("data_file")){
    return 0;
  }
  else if (!arr[0].compare("dm_step")){
    dm_step = atof(arr[1].c_str());
    return 0;
  }
  else if (!arr[0].compare("frame")){
    frame.assign(arr[1]);
    return 0;
  }
  else if (!arr[0].compare("station")){
    station.assign(arr[1]);
    return 0;
  }
  else if (!arr[0].compare("ref_freq")){
    ref_freq = atof(arr[1].c_str());
    return 0;
  }
  else if (!arr[0].compare("dedsps_time_bin")){
    dedsps_time_bin = atoi(arr[1].c_str());
    return 0;
  }
  else if (!arr[0].compare("time_format")){
    time_format.assign(arr[1]);
    return 0;
  }
  else{
    std::cerr<< "Unrecognized configuration parameter: " << arr[0]<< " \n";
    return 1;
  }
}

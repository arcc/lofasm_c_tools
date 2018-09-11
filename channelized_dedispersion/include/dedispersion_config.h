// This is a header file to declear the dedispersion Configuration functions
// Author: Jing Luo
#ifndef DEDISPERSION_CONFIG_H_
#define DEDISPERSION_CONFIG_H_
#include<string>
#include<vector>
#include<map>
#include<set>


class Config
{
  public:
    std::string config_file;
    int file_input_flag;   // If the cofiguration is input by a file
    // Configuration parameters
    std::string time_format;
    std::string station;
    std::string frame;
    std::vector<std::string> input_files;
    double start_time = -1;
    double end_time = -1;
    int time_cmd_flag = 0;      // Command line input start and end time flag
    double start_dm = -1;
    double end_dm = -1;
    double dm_step = 0;
    int dm_cmd_flag = 0;        // Command line input start and end dm flag
    double start_freq = -1;
    double end_freq = -1;
    int freq_cmd_flag = 0;      // Command line input start and end frequency flag
    double ref_freq=-1;     // Reference frequency
    int dedsps_time_bin = 0;
    int read_flag;      // Flag shows if the Configuration is read from a file.

    // Public method
    Config();          // Default constructor
    Config(std::string filename);    // Construct from a file
    int parse_config_line(std::string line);
    int read_config_file(std::string filename);

};

#endif   // DEDISPERSION_CONFIG_H_

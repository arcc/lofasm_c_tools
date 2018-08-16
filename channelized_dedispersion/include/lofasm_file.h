// This is a file to define the interface class for lofasm .bbx file
//
// Author: Jing Luo

#ifndef LF_BBX_H_
#define LF_BBX_H_

#include<string>
#include<vector>
#include "lofasmIO.h"
#include "dedispersion_io.h"

class LfBbx: public DataFile
// An api class for reading and writing the lofasm .bbx file format.
{
  public:
    lfb_hdr head;

    // Public method
    LfBbx (const char *fname, const char *input_mode);
    double get_dim1_start();
    double get_dim1_step();
    double get_dim2_start();
    double get_dim2_step();


    int read_head();
    int open_file();
    int close_file();
    std::string get_obs();
    std::string get_frame();
    double get_max_freq();
    double get_min_freq();
    double get_bandwidth();
    int read_next_subint_1d (int num_next_time_bin,
                             std::vector <double> & data)
                             { return 0; };
    int write_next_subint_1d (int num_next_time_bin,
                              std::vector <double> & data)
                              { return 0; };
    int read_next_subint_2d (int num_next_time_bin, int cplx_flag,
                             std::vector < std:: vector <double> > & real_data,
                             std::vector < std:: vector <double> > & cplx_data);
    int write_next_subint_2d (int num_next_time_bin,
                              std::vector < std:: vector <double> > & data)
                              { return 0; };

    int read_data_real(int request_time_bin,
                       std::vector< std::vector<double> > & data);

    int read_data_cplx(int request_time_bin,
                        std::vector< std::vector<double> > & real_data,
                        std::vector< std::vector<double> > & cplx_data);
};


#endif  //end lofasm_file.h

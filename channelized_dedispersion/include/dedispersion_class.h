// This is a header file to declear the data classes for dedispersion
// Author: Jing Luo

#ifndef DEDISPERSION_CLASS_H_
#define DEDISPERSION_CLASS_H_

#include<math.h>
#include<string>
#include<vector>


// class for the channelized dedispersion result container.
// It contains a 2D array. dimsion one is DM, and dimsion two is time.
// It only saves the result of one frequency channel.
class ChanDMT
{
  public:
    int num_time_bin;
    int num_dm_bin;
    double freq;
    double chan_bandwidth;
    double start_time;
    double time_step;
    double start_dm;
    double dm_step;
    double dm_duplicat_idx;
    
    std::vector< std::vector<double> > data;
    std::vector< std::vector<int> > sum_idxs;
    // Constructor
    ChanDMT (double input_freq, double input_bandwidth); 
    // Public method
    std::vector<double> get_time_axis();
    std::vector<double> get_dm_axis();
    void init_data(int target_num_time_bin, int target_num_dm_bin );
    void get_sum_idxs(double ref_freq);
}; // Finish define the ChanDMT class



#endif  // DEDISPERSION_CLASS_H_

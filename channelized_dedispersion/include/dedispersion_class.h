// This is a header file to declear the data classes for dedispersion
// Author: Jing Luo

#ifndef DEDISPERSION_CLASS_H_
#define DEDISPERSION_CLASS_H_

#include<math.h>
#include<string>
#include<vector>
#include<boost/function.hpp>

// A class that saves the information for different dedispersion method.
// This class will be used by the ChanDedsprs class.
class DedsprsMethod
{
  public:
    int ref_dm_idx;
    int curr_dm_idx;
    int method_id;           // This is the ID the summation method
    int result_idx;          // The index where the result is saved in the results array
    int ref_result_idx;      // The reference index of result array

    boost::function8 < int, int, int, int, int, int, std::vector<double>&, \
                       std::vector<double>&, std::vector<double>& > method;

    DedsprsMethod (int input_ref_dm_idx, int input_curr_dm_idx);
};

// class for the channelized dedispersion result container.
// It contains a 2D array. dimsion one is DM, and dimsion two is time.
// It only saves the result of one frequency channel.
class ChanDedsprs
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
    double ref_freq;
    // This is a vector that store the dm intervals that have the same sum index
    // EXAMPLE:
    // if dm index 0 to dm index 10 have the same sum index for this frequency,
    // this vector will save the dm indices information as {{0,10},}
    std::vector< std::vector <int> > identical_sum_intervals;
    // this is the result data.
    std::vector< std::vector<double> > data;
    std::vector< std::vector<int> > sum_idxs;
    std::vector<int> dedsprs_idxs_dm; // The indices that needs dedispersion
    std::vector<int> dedsprs_ref_map; // Reference DM trail indices
    std::vector<int> dedspsr_result_map;  // The DM result indices
    std::vector< DedsprsMethod > dm_method; // DM method.
    std::vector<int> normalize_num; // The number of normalizing for each DM trail.


    // Constructor
    ChanDedsprs (double input_freq, double input_bandwidth);
    // Public method
    std::vector<double> get_time_axis();
    std::vector<double> get_dm_axis();
    void init_data(int target_num_time_bin, int target_num_dm_bin );
    void get_sum_idxs();
    void get_identical_sum_idx();
    void get_dedispersion_idx();
    void organize_dedispersion();
    void get_result_map();
}; // Finish define the ChanDedsprs class

// This is a class for storing the channelized data. The data should be an 1D
// vector array that reprsents the power for one frequency channel over time.
class ChanData
{
  public:
    double freq;
    double chan_bandwidth;
    double start_time;
    double time_step;
    int num_time_bin;
    std::vector<double> data;

    std::vector<double> get_time_axis();
    //int read_data();
}; // Finish define the ChanData class

class DedsprsResult
{
  public:
    std::vector<double> time_axis;
    double time_start;
    double time_step;
    int num_time_bin;

    std::vector<double> dm_axis;
    double dm_start;
    double dm_step;
    int num_dm_bin;

    std::vector< std::vector<double> > data;
    std::vector< std::vector<double> > normArray;

    // Constructor
    DedsprsResult (double time_start, double time_step, int num_time_bin,
                   double dm_start, double dm_step, int num_dm_bin);
    // Class methods
    int map_time_idx(double time);
    int map_dm_idx(double dm);

};


#endif  // DEDISPERSION_CLASS_H_

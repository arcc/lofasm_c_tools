// This is a header file to declear the functions for dedispersion
// Author: Jing Luo

#ifndef DEDISPERSION_H_
#define DEDISPERSION_H_

#include<math.h>
#include<string>
#include<vector>

double get_time_delay(double freq, double freqRef, double dm);

// Function to sum a channelized data array
int sum_channel(int num_dedsps_time_bin, int start_sum_idx, int end_sum_idx,
                std::vector<double> & channel_data, std::vector<double> & result);

// Function to compute the summation for one channel of data from one dm to
// another dm. The DM is parameterized by the the start sum index and end sum index
int sum_next_dm(int start_sum_idx_last, int end_sum_idx_last,
                int start_sum_idx_curr, int end_sum_idx_curr,
                std::vector<double> & channel_data,
                std::vector<double> & result_last,
                std::vector<double> & result_curr);

// Function to compute the summation for one channel of data from one dm to
// another dm. In the funciton, only the first time bin are computed using the
// last dm; the other time bins are computed by the running summation on time.
// It is designed for the case that the current DM largely deviate the last DM.
int sum_next_dm_big_diff(int start_sum_idx_last, int end_sum_idx_last,
                         int start_sum_idx_curr, int end_sum_idx_curr,
                         std::vector<double> & channel_data,
                         std::vector<double> & result_last,
                         std::vector<double> & result_curr);

//








#endif  // DEDISPERSION_H_

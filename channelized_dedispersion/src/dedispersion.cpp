// This files defines the dedispersion functions.
// Author: Jing Luo

#include<stdio.h>
#include<iostream>
#include<math.h>
#include<string>
#include<vector>
#include "dedispersion.h"
#include "dedispersion_class.h"


double get_time_delay(double freq, double freqRef, double dm)
// Compute the time delay following the dispersion law.
{
    double timeDelay;
    timeDelay = 4.15e3*dm*(1.0/(freq*freq)-1.0/(freqRef*freqRef));
    return timeDelay;
}


int sum_channel(int num_dedsps_time_bin, int start_sum_idx, int end_sum_idx,
                std::vector<double> & channel_data, std::vector<double> & result)
// This is a function to perfix summarize a channelized data array
// NOTE (luojing1211@gmail.com) the result array is passing by reference.
// So it is not guaranteed that result array starts with zeros.
{
  int i;
  // get the summation of the first time bin
  // The end summation index is inclusive
  for ( i = start_sum_idx ; i <= end_sum_idx; i++ ){
    result[0] += channel_data[i];
  }
  // get the summation for other time bins
  for ( i = 1; i < num_dedsps_time_bin; i++ ){
    result[i] = result[i-1] - channel_data[start_sum_idx + i - 1];
    result[i] += channel_data[end_sum_idx + i];
  }
  return 0;
}

int sum_next_dm(int start_sum_idx_last, int end_sum_idx_last,
                int start_sum_idx_curr, int end_sum_idx_curr,
                std::vector<double> & channel_data,
                std::vector<double> & result_last,
                std::vector<double> & result_curr)
// This is a function to compute the summation from a smaller DM to a bigger DM
// for a channelized data with the assumption that bigger DM's result is based
// on smaller DM's result.
// The DM informaiton is provided by the start and end summation index.
{
  int i, j;
  int start_idx_diff;
  int end_idx_diff;
  // Compare the sum index
  start_idx_diff = start_sum_idx_curr - start_sum_idx_last;
  end_idx_diff = end_sum_idx_curr - end_sum_idx_last;
  // Get summations from last DM's result
  for (i = 0; i < result_last.size(); i++ ){
    // remove the data that are only in the last DM
    result_curr[i] = result_last[i];
    for (j = 0; j < start_idx_diff; j++){
      result_curr[i] -= channel_data[i + start_sum_idx_last +j];
    }
    // Add the data that are only in the current DM
    for (j = 1; j <= end_idx_diff; j++){
      result_curr[i] += channel_data[i + end_sum_idx_last + j];
    }
  }
  return 0;
}

int sum_next_dm_big_diff(int start_sum_idx_last, int end_sum_idx_last,
                         int start_sum_idx_curr, int end_sum_idx_curr,
                         std::vector<double> & channel_data,
                         std::vector<double> & result_last,
                         std::vector<double> & result_curr)
// This fucntion has the same functionality with the one above.
// However, in this funciton, only the first time bin are computed using the
// last dm; the other time bins are computed by the running summation on time.
// It is designed for the case that the current DM largely deviate the last DM.
{
  int j, i;
  int start_idx_diff;
  int end_idx_diff;
  // Compare the sum index
  start_idx_diff = start_sum_idx_curr - start_sum_idx_last;
  end_idx_diff = end_sum_idx_curr - end_sum_idx_last;
  // Get the first time bin from the last DM's result
  result_curr[0] = result_last[0];
  for (j = 0; j < start_idx_diff; j++){
    result_curr[0] -= channel_data[start_sum_idx_last +j];
  }
  // Add the data that are only in the current DM
  for (j = 1; j <= end_idx_diff; j++){
    result_curr[0] += channel_data[end_sum_idx_last + j];
  }
  // Get the results of the other time bins.
  for ( i = 1; i < result_last.size(); i++ ){
    result_curr[i] = result_curr[i-1] - channel_data[start_sum_idx_curr + i - 1];
    result_curr[i] += channel_data[end_sum_idx_curr + i];
  }
  return 0;
}

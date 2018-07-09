// This is a file for testing the dedispersion classes

#include<iostream>
#include<math.h>
#include<string>
#include<vector>
#include "dedispersion_class.h"
#include "dedispersion.h"
#include "dedispersion_utils.h"

int main(){
  double start_freq;
  double freq_step;
  int num_freq;
  double max_freq;
  double start_dm;
  double dm_step;
  int num_dm;
  double start_time;
  double time_step;
  int num_time_bin;
  double dt;
  int i, j;
  std::cout<<"Start"<<std::endl;
  std::vector<ChanDedsprs> chan_results;

  start_time = 0;
  num_time_bin = 40;
  time_step = 0.08;      // Time step is 1s
  std::vector<double> test_chan (num_time_bin, 0.0); // Channelized data
  // fill up the frequency vector
  start_freq = 2e7;
  freq_step = 0.09e6;
  num_freq = 500;
  std::vector<double> freqs (num_freq, 0.0);

  start_dm = 0.0;
  dm_step = 0.5;
  num_dm = 10;
  // Fill up the result class for each channels
  for ( i = 0; i < num_freq; i++ ){
    freqs[i] = start_freq + freq_step * i;
    chan_results.push_back(ChanDedsprs(freqs[i], freq_step));
    chan_results[i].start_time = start_time;
    chan_results[i].time_step = time_step;
    chan_results[i].num_time_bin = num_time_bin;
    chan_results[i].start_dm = start_dm;
    chan_results[i].dm_step = dm_step;
    chan_results[i].num_dm_bin = num_dm;
  }
  // compute the time shift
  for ( i = 0; i < chan_results.size() ; i++ ){
    chan_results[i].ref_freq = freqs.back();
    chan_results[i].get_sum_idxs();
  }
  std::vector<double> dm_axis = chan_results[0].get_dm_axis();
  std::cout<< "Freq is :" << chan_results[0].freq << std::endl;
    std::cout<< "Ref Freq is :" << chan_results.back().freq << std::endl;
  std::cout<< "DM   sum_idx1   sum_idx2  dt"<<std::endl;
  for (i=0; i < dm_axis.size(); i++ ){
    dt = get_time_delay(chan_results[0].freq, freqs.back(), dm_axis[i]);
    std::cout<< dm_axis[i] << " "<< chan_results[0].sum_idxs[i][0] << " ";
    std::cout<< chan_results[0].sum_idxs[i][1]<<" "<<dt<<std::endl;
  }

  // std::cout<<" Test identical sum interval."<<std::endl;
  // for ( i = 0; i < chan_results.size() ; i++ ){
  //   chan_results[i].get_identical_sum_idx();
  //   std::cout<< "For frequency : " << chan_results[i].freq << std::endl;
  //   for (j = 0; j < chan_results[i].identical_sum_intervals.size(); j++ ){
  //     std::cout<<"Pair " <<chan_results[i].identical_sum_intervals[j][0];
  //     std::cout<< " "<<chan_results[i].identical_sum_intervals[j][1]<<std::endl;
  //   }
  // }

  for ( i=0; i < chan_results.size() ; i++ ){
    chan_results[i].get_identical_sum_idx();
    chan_results[i].get_dedispersion_idx();
    chan_results[i].organize_dedispersion();
  }

  std::cout<<" Dedispersion method"<<std::endl;
  std::cout<< "DM sum_idx1 sum_idx2, method "<<std::endl;

  for (i=0; i < dm_axis.size(); i++ ){
    std::cout<< i <<" "<<dm_axis[i] << " "<< chan_results[0].sum_idxs[i][0] << " ";
    std::cout<< chan_results[0].sum_idxs[i][1]<<" ";
    std::cout<< chan_results[0].dm_method[i].method_id<<std::endl;
  }

  std::cout<< "Intervals" << std::endl;
  for (i=0; i < chan_results[0].identical_sum_intervals.size();i++){
    std::cout<< chan_results[0].identical_sum_intervals[i][0] << " ";
    std::cout<< chan_results[0].identical_sum_intervals[i][1] << std::endl;
  }

  for (i=0 ; i < chan_results[0].dedsprs_idxs_dm.size(); i++ ){
    std::cout<< chan_results[0].dedsprs_idxs_dm[i] << std::endl;
  }

  return 0;
}

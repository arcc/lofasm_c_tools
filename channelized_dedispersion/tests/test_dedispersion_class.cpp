// This is a file for testing the dedispersion classes

#include<iostream>
#include<math.h>
#include<string>
#include<vector>
#include "dedispersion_class.h"

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
  int i;
  std::cout<<"Start"<<std::endl;
  std::vector<ChanDMT> chan_results;

  start_time = 0;
  num_time_bin = 40;
  time_step = 1;      // Time step is 1s
  std::vector<double> test_chan (num_time_bin, 0.0); // Channelized data
  // fill up the frequency vector
  start_freq = 2e7;
  freq_step = 0.09e6;
  num_freq = 100;
  std::vector<double> freqs (num_freq, 0.0);

  start_dm = 0.0;
  dm_step = 0.5;
  num_dm = 10;
  // Fill up the result class for each channels
  for ( i = 0; i < num_freq; i++ ){
    freqs[i] = start_freq + freq_step * i;
    chan_results.push_back(ChanDMT(freqs[i], freq_step));
    chan_results[i].start_time = start_time;
    chan_results[i].time_step = time_step;
    chan_results[i].num_time_bin = num_time_bin;
    chan_results[i].start_dm = start_dm;
    chan_results[i].dm_step = dm_step;
    chan_results[i].num_dm_bin = num_dm;
  }
  std::cout<<"Shift"<<std::endl;
  // compute the time shift
  //std::cout<<"chan_class "<<chan_results.size()<<std::endl;
  for ( i = 0; i < chan_results.size() ; i++ ){
    std::cout<<" Channel "<<i<<" "<<chan_results[i].freq<<std::endl;
    chan_results[i].get_sum_idxs(freqs.back());
    std::cout<<" Finish sum idx channel "<<std::endl;
  }
  // std::cout<<"Print out"<<std::endl;
  // std::vector<double> dm_axis = chan_results[0].get_dm_axis();
  // std::cout<< "Freq is :" << chan_results[0].freq << std::endl;
  // std::cout<< "DM   sum_idx1   sum_idx2 "<<std::endl;
  // for (i=0; i < dm_axis.size(); i++ ){
  //   std::cout<< dm_axis[i] << " "<< chan_results[0].sum_idxs[i][0] << " ";
  //   std::cout<< chan_results[0].sum_idxs[i][1]<< std::endl;
  // }
  return 0;
}

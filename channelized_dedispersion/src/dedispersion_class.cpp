// This is a file that defines class methods for the dedispersion classes.
// Author: Jing Luo

#include<stdio.h>
#include<iostream>
#include<math.h>
#include<string>
#include<vector>
#include "dedispersion_class.h"
#include "dedispersion.h"

// Class method for ChanDMT class
ChanDMT::ChanDMT(double input_freq, double input_bandwidth){
  freq = input_freq;
  chan_bandwidth = input_bandwidth;
}

std::vector<double> ChanDMT::get_time_axis(){
  int i;
  std::vector<double> time_axis(num_time_bin, 0.0);
  for(i = 0 ; i < time_axis.size(); i++){
    time_axis[i] = start_time + i * time_step;
  }
  return time_axis;
}

std::vector<double> ChanDMT::get_dm_axis(){
  int i;
  std::vector<double> dm_axis(num_dm_bin, 0.0);
  for(i = 0 ; i < dm_axis.size(); i++){
    dm_axis[i] = start_dm + i * dm_step;
  }
  return dm_axis;
}
// Initialize the result data
void ChanDMT::init_data(int target_num_time_bin, int target_num_dm_bin ){
  data.resize(target_num_dm_bin, std::vector<double> (target_num_time_bin, 0.0));
}
// Compute the time delay index shift
void ChanDMT::get_sum_idxs(double ref_freq){
  int i;
  double time_delay_curr;
  double time_delay_low_freq;
  // The shift index is 2D vector. The row is the DM, and two columns are
  // start shift/sum index and end shift/sum index
  std::cout<<"Start the sum idx "<<std::endl;
  sum_idxs.resize(num_dm_bin, std::vector<int> (2, 0));
  for (i = 0; i < num_dm_bin ; i++ ){
    std::cout<<" init sum idx "<<sum_idxs[i][0]<<std::endl;
  }
  std::cout<<"sum idx 0 010 "<<sum_idxs[0][0]<<std::endl;
  std::vector<double> dms = get_dm_axis();
  std::cout<<"sum idx 0 020 "<<sum_idxs[0][0]<<std::endl;
  std::cout<<"Finish DM computation"<<std::endl;
  if (freq == 0){
    std::cerr<< "Can not compute time delay for 0.0 Hz frequency"<<std::endl;
    std::exit(1);
  }

  std::cout<<"GET sum indices"<<std::endl;

  // get sum indices for the current frequency channle.
  for (i = 0; i<dms.size() ; i++ ){
    std::cout<<"DMS " <<dms[i]<<" "<< freq<<" "<< ref_freq<<std::endl;
    time_delay_curr = get_time_delay(freq, ref_freq, dms[i]);
    std::cout<<" time delay curr "<<time_delay_curr<<std::endl;
    std::cout<<" start idx "<<sum_idxs[0][0]<<std::endl;
    //sum_idxs[i][0] = (int)(time_delay_curr/time_step);
    std::cout<<"Start sum index finished"<<std::endl;
    time_delay_low_freq = get_time_delay(freq - chan_bandwidth, ref_freq, dms[i]);
    //sum_idxs[i][1] = (int)time_delay_low_freq/time_step;
    std::cout<<" s_idx " <<  sum_idxs[i][0] << " e_idx " << sum_idxs[i][1] << std::endl;
  }
  std::cout<<"Sum idx complete"<<std::endl;
}
// Finish defining class method for ChanDMT

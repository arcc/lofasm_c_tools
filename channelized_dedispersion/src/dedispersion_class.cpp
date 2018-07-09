// This is a file that defines class methods for the dedispersion classes.
// Author: Jing Luo

#include<stdio.h>
#include<iostream>
#include<math.h>
#include<string>
#include<vector>
#include "dedispersion_class.h"
#include "dedispersion_utils.h"
#include "dedispersion.h"

DedsprsMethod::DedsprsMethod(int input_ref_dm_idx, int input_curr_dm_idx){
  ref_dm_idx = input_ref_dm_idx;
  curr_dm_idx = input_curr_dm_idx;
}


// Class constructor and method for ChanDedsprs class
ChanDedsprs::ChanDedsprs(double input_freq, double input_bandwidth){
  freq = input_freq;
  chan_bandwidth = input_bandwidth;
}

std::vector<double> ChanDedsprs::get_time_axis(){
  int i;
  std::vector<double> time_axis(num_time_bin, 0.0);
  for(i = 0 ; i < time_axis.size(); i++){
    time_axis[i] = start_time + i * time_step;
  }
  return time_axis;
}

std::vector<double> ChanDedsprs::get_dm_axis(){
  int i;
  std::vector<double> dm_axis(num_dm_bin, 0.0);
  for(i = 0 ; i < dm_axis.size(); i++){
    dm_axis[i] = start_dm + i * dm_step;
  }
  return dm_axis;
}
// Initialize the result data
void ChanDedsprs::init_data(int target_num_time_bin, int target_num_dm_bin ){
  data.resize(target_num_dm_bin, std::vector<double> (target_num_time_bin, 0.0));
}
// Compute the time delay index shift
void ChanDedsprs::get_sum_idxs(){
  int i;
  double time_delay_curr;
  double time_delay_low_freq;
  // The shift index is 2D vector. The row is the DM, and two columns are
  // start shift/sum index and end shift/sum index
  sum_idxs.resize(num_dm_bin, std::vector<int> (2, 0));

  std::vector<double> dms = get_dm_axis();

  if (freq == 0){
    std::cerr<< "Can not compute time delay for 0.0 Hz frequency"<<std::endl;
    std::exit(1);
  }

  // get sum indices for the current frequency channle.
  for (i = 0; i<dms.size() ; i++ ){
    time_delay_curr = get_time_delay(freq, ref_freq, dms[i]);
    std::cout<<"ref freq "<< ref_freq<< " ";
    std::cout<< "TIME DELAY"<<time_delay_curr<<" ";
    sum_idxs[i][0] = (int)(time_delay_curr/time_step);
    std::cout<<" Index :" << sum_idxs[i][0]<<std::endl;
    // get the end sum index
    time_delay_low_freq = get_time_delay(freq - chan_bandwidth, ref_freq, dms[i]);
    sum_idxs[i][1] = (int)(time_delay_low_freq/time_step);
    std::cout<< "TIME DELAY2 "<<time_delay_low_freq;
    std::cout<<" Index 2 :" << sum_idxs[i][1]<<std::endl;;
  }
}

void ChanDedsprs::get_identical_sum_idx(){
  int i;
  int interval_front;
  int interval_back;
  int size_sum_idx;
  int ref_dm_idx;
  std::vector<int> interval(2, 0);
  if (sum_idxs.size() == 0){
    std::cerr<< "Please compute the sum indices first.";
    std::exit(1);
  }
  interval_front = 0;
  ref_dm_idx = 0;
  dedsprs_ref_map.push_back(ref_dm_idx);
  size_sum_idx = sum_idxs.size();
  for ( i=1; i < size_sum_idx; i++ ){
    // The sum indices are the same for two DMs.
    if (sum_idxs[i][0] == sum_idxs[interval_front][0] && \
        sum_idxs[i][1] == sum_idxs[interval_front][1]){
          // Check if i is the last index
          if (i != size_sum_idx - 1 ){
            dedsprs_ref_map.push_back(ref_dm_idx);
            continue;
          }
          // All the DM sum indices are indentical.
          else{
            interval[0] = interval_front;
            interval[1] = i;
            identical_sum_intervals.push_back(interval);
            dedsprs_ref_map.push_back(ref_dm_idx);
          }
    }
    else{
      // If it is two neighborhood DM move the interval front
      if (i - interval_front <= 1){
        interval_front = i;
      }
      else{
        interval_back = i - 1;
        interval[0] = interval_front;
        interval[1] = interval_back;
        identical_sum_intervals.push_back(interval);
        interval_front = i;
      }
      dedsprs_ref_map.push_back(ref_dm_idx);
      ref_dm_idx = i;
    }
  }
}

void ChanDedsprs::organize_dedispersion(){
  // This is a function that choose the dedispersion methods all the DM trails.
  // If the current DM trail has big difference in summation index with the
  // last DM trail, the current DM trail will use the sum_next_dm_big_diff()
  // method.
  // If the current DM trail has them same summation index with the last DM
  // trail, It will not do any thing.
  // Otherwise, it uses the sum_next_dm() function.
  // Here we only save the DM indices corresponding to the two methods.
  // DM trail will

  int i, j;
  int start_idx_diff;
  int end_idx_diff;
  int num_sums; // number of summation needed to finish one time bin's dedispersion
  int size_sum_idx;
  int idx_in_result;

  if (sum_idxs.size() == 0){
    std::cerr<< "Please compute the sum indices first.";
    std::exit(1);
  }
  // Clean the vector
  dm_method.clear();
  // Orgnaize the first DM trial.
  dm_method.push_back(DedsprsMethod(0,0));
  dm_method[0].method = sum_one_dm;
  dm_method[0].method_id = 1;
  idx_in_result = 0;
  dm_method[0].result_idx = idx_in_result;
  dm_method[0].ref_result_idx = 0;


  size_sum_idx = sum_idxs.size();
  for ( i=1; i < size_sum_idx; i++ ){
    // dedsprs_ref_map[i] should always be smaller than i
    dm_method.push_back(DedsprsMethod(dedsprs_ref_map[i] , i));
    start_idx_diff = sum_idxs[i][0] - sum_idxs[i-1][0];
    end_idx_diff = sum_idxs[i][1] - sum_idxs[i-1][1];
    num_sums = start_idx_diff + end_idx_diff;
    // If the sum index is the same with last dm trail, use no sum go to next dm
    if (start_idx_diff == 0 && end_idx_diff == 0){
      dm_method[i].method = no_sum;
      dm_method[i].method_id = 0;
    }
    // If the sum index has small difference with last dm trail, use small sum
    else if (num_sums < 3){
      dm_method[i].method = sum_next_dm;
      dm_method[i].method_id = 2;
      idx_in_result += 1;
    }
    // If the sum index has big difference with last dm trail, use big sum.
    else{
      dm_method[i].method = sum_next_dm_big_diff;
      dm_method[i].method_id = 3;
      idx_in_result += 1;
    }
    dm_method[i].result_idx = idx_in_result;
    dm_method[i].ref_result_idx = dm_method[dedsprs_ref_map[i]].result_idx;
  }
}

void ChanDedsprs::get_dedispersion_idx(){
  int i, j;
  int push_index;
  // push the first index into dedispersion indices.
  push_index = -1;
  //dedsprs_idxs_dm.push_back(push_index);
  // subtruck the DMs that has identical sum index.
  for ( i = 0 ; i < identical_sum_intervals.size(); i++){
    // add indces between the same index intervals
    for ( j = push_index + 1; j <= identical_sum_intervals[i][0]; j++){
      dedsprs_idxs_dm.push_back(j);
      push_index = identical_sum_intervals[i][1];
    }
  }
  // Add indices to the end
  for (j = push_index + 1; j < num_dm_bin ; j++){
    dedsprs_idxs_dm.push_back(j);
  }
}


// Finish defining class method for ChanDedsprs
//////////////////////////////////////////////////
std::vector<double> ChanData::get_time_axis(){
  int i;
  std::vector<double> time_axis(num_time_bin, 0.0);
  for(i = 0 ; i < time_axis.size(); i++){
    time_axis[i] = start_time + i * time_step;
  }
  return time_axis;
}

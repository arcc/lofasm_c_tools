// This is a file for testing the dedispersion functions

#include<iostream>
#include<math.h>
#include<string>
#include<vector>
#include "dedispersion.h"
#include "dedispersion_utils.h"

int main(){
  int i;
  int start_idx;                            // start summation index
  int end_idx;                              // end summation index
  int start_idx2;                           // start summation index for next dm
  int end_idx2;                             // end summation index for next dm
  int num_dedsps_time_bin;                  // Number of dedisperison time bin
  // target result;
  double targe[] = {12, 15, 18, 21, 24, 27, 30, 33, 36, 39};
  std::cout << "Testing dedisperison functions:" << std::endl;
  std::vector<double> test_chan (20, 0.0); // Channelized data
  std::vector<double> result (10, 0.0);    // Result vector
  std::vector<double> result2 (10, 0.0);    // Result vector2
  std::vector<double> target2 (10, 0.0);
  std::vector<double> result3 (10, 0.0);    // Result vector2
  // Fill the test array from 1 to 200
  for ( i=0 ; i < 20; i++ ){
    test_chan[i] = i + 1;
    std::cout<< test_chan[i]<<std::endl;
  }
  // Test the prefix summation
  start_idx = 2;
  end_idx = 4;
  num_dedsps_time_bin = 10;
  std::cout << "Dedisperison start index : "<<start_idx<<std::endl;
  std::cout << "Dedisperison end index : "<<end_idx<<std::endl;
  sum_channel(num_dedsps_time_bin, start_idx, end_idx, test_chan, result);
  std::cout<< "Result " << " Target"<<std::endl;
  for (i = 0; i < 10 ; i++ ){
    std::cout<< result[i]<<" "<<targe[i]<<std::endl;
    if (result[i] != targe[i]){
      std::cout<<" Find discrepancy between result and target."<<std::endl;
      return 1;
    }
  }
  start_idx2 = 7;
  end_idx2 = 9;
  std::cout << "Dedisperison start index new : "<<start_idx2<<std::endl;
  std::cout << "Dedisperison end index new : "<<end_idx2<<std::endl;
  sum_next_dm(start_idx, end_idx, start_idx2, end_idx2, num_dedsps_time_bin, \
              test_chan, result, result2);
  sum_channel(num_dedsps_time_bin, start_idx2, end_idx2, test_chan, target2);
  std::cout<< "Result2 " << " Target"<<std::endl;
  for (i = 0; i < 10 ; i++ ){
    std::cout<< result2[i]<<" "<<target2[i]<<std::endl;
    if (result2[i] != target2[i]){
      std::cout<<" Find discrepancy between result and target."<<std::endl;
      return 1;
    }
  }
  std::cout<<"Test function 'sum_next_dm_big_diff'. "<<std::endl;
  sum_next_dm_big_diff(start_idx, end_idx, start_idx2, end_idx2,  \
                       num_dedsps_time_bin, test_chan, result, result3);
  std::cout<< "Result3 " << " Target"<<std::endl;
  for (i = 0; i < 10 ; i++ ){
    std::cout<< result3[i]<<" "<<target2[i]<<std::endl;
    if (result3[i] != target2[i]){
      std::cout<<" Find discrepancy between result and target."<<std::endl;
      return 1;
    }
  }

  return 0;
}

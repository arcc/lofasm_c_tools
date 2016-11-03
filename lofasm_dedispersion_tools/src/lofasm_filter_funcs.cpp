#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <time.h>
#include <utility>
#include "lofasm_filter.h"
#include "lofasm_data_class.h"
#include "lofasm_utils.h"
using namespace std;

int average_time_filter(FilterBank * fltdata, int window_size){
/* This function provides an average filter on the filter bank data time axis.
It takes the data divid by an average number in a window of time bins.
The data will be located at the center of the window.
Filtered_data = data/ window_average
*/
  int midway;

  midway = window_size/2;

}


int average_freq_filter(FilterBank * fltdata, int window_size){
  
}

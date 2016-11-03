/*Dedispersion cpp version header file */
#ifndef LOFASM_DEDISPERSION_H_
#define LOFASM_DEDIDPERSION_H_

#include<math.h>
#include<string>
#include<vector>

#include "lofasm_data_class.h"
#include "lofasm_dedispersion.h"
using namespace std;

int average_filter(FilterBank * fltdata, vector<int> max_time_window);

int median_filter(FilterBank * fltdata, vector<int> max_time_window);

#endif

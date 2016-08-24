#ifndef LOFASM_UTILS_H_
#define LOFASM_UTILS_H_
#include"lofasm_data_class.h"
using namespace std;

char * del_blank(char *str);

/* Function for swaping binary data. bigEndian*/
void swap2(char *word);
void swap4(char *word);
void swap8(char *dword);

FilterBank* combine_fltbank_time(FilterBank & firstData, FilterBank & secondData, int overwrite);

void rotate_fltbank(FilterBank & data, int rotateSize );

#endif

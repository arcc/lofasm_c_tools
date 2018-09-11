// This is the source file for dedispersion I/O API
#include<math.h>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<iterator>
#include "lofasmIO.h"
#include "lofasm_file.h"
#include "dedispersion_io.h"


DataFile::DataFile (){;}

// Defining class methods for DataFile class
DataFile::DataFile(std::string fnamecpp, const char *input_mode)
// The constructor of data file
{
  filenamecpp = fnamecpp;
  const char * fname = fnamecpp.c_str();

  if(strlen(fname)>1024){
    std::cerr << "File name : "<< *(fname) << " is too long" ;
    exit(1);
  }
  else{
    strncpy(filename, fname, 1024);
  }

  if(strlen(input_mode)>3){
    std::cerr << "Mode flag : "<< *(input_mode) << " is too long"<<std::endl;
    exit(1);
  }
  else{
    strncpy(mode, input_mode, 3);
  }
}

int DataFile::close_file()
{
  fclose(f_ptr);
  f_ptr = NULL;
  return 0;
}

void FileList::add_to_map(DataFile * data_file){
  double start_time;
  start_time = data_file->start_time;
  files[start_time] = data_file;
  num_files = files.size();
}

void FileList::get_files_by_time(double stime, double etime,
                                 std::map<double, DataFile*>::iterator &itlow,
                                 std::map<double, DataFile*>::iterator &itup){
  int num_file;
  itlow=files.lower_bound (stime);  // itlow points to b
  itlow --;
  itup=files.upper_bound (etime);
  itup --;
  std::cout<<" Selected start file : "<<itlow->second->filenamecpp <<"\n";
  std::cout<<" Selected end file : "<< itup->second->filenamecpp <<"\n";
}

void DataFileContainer::add_file(std::string filename){
  DataFile * f_obj = get_file_info(filename);
  std::string f_obs;
  std::string f_frame;
  double max_freq;
  double min_freq;
  double freq_bandwidth;
  std::map<double, FileList>::iterator search;

  f_obs = f_obj -> get_obs();
  f_frame = f_obj -> get_frame();
  max_freq = f_obj -> get_max_freq();
  freq_bandwidth = f_obj -> get_bandwidth();
  min_freq = f_obj -> get_min_freq();


  if (f_obj->start_time < time_begin || time_begin == 0){
    time_begin = f_obj->start_time;
  }

  if (f_obj->end_time > time_end){
    time_end = f_obj->end_time;
  }

  if (max_freq > max_freq_infile){
    max_freq_infile = max_freq;
  }

  if (min_freq < min_freq_infile || min_freq_infile == 0){
    min_freq_infile = min_freq;
  }

  if (obs.empty()){
    obs = f_obs;
  }

  if (frame.empty()){
    frame = f_frame;
  }

  if (std::strcmp(f_obs.c_str(), obs.c_str()) != 0){
    std::cerr << "File : " << filename << "does not have the same observatory.\n";
    std::cerr << "File : " << filename << "is not added to the list.\n";
    return;
  }
  if (std::strcmp(f_frame.c_str(), frame.c_str()) != 0){
    std::cerr << "File : " << filename << "does not have the same polarization.\n";
    std::cerr << "File : " << filename << "is not added to the list.\n";
    return;
  }
  // Add the file object to the corresponding frequency list.
  // Here we assume the bandwidth for each frequency are the same.
  search = file_lists.find(max_freq);
  if (search != file_lists.end()) {
    search -> second.add_to_map(f_obj);
  }
  // Build list and add file to the file list.
  else {
    FileList f1;
    f1.max_freq = max_freq;
    f1.freq_band = freq_bandwidth;
    file_lists[max_freq] = f1;
    file_lists[max_freq].add_to_map(f_obj);
  }
  all_files.insert(filename);
}


FileList * DataFileContainer::get_file_list_by_freq(double freq)
// Get the file lists by the frequency
{
  FileList * result = NULL;
  std::map <double, FileList>::iterator it;
  for (it = file_lists.begin(); it != file_lists.end(); ++it){
    if (freq < it -> first && freq > it -> second.max_freq - it -> first)
      result = &(it -> second);
    else
      continue;
  }
  return result;
}

int check_file_type(std::string filename){
  int filetype;
  char fileSig [10];
  FILE *f;
	f = lfopen(filename.c_str(), "rb");
	if (f != NULL){
		// Read header
    if (!fgets(fileSig, 10, f)){
      std::cout<< "Can not read file : "<< filename << "\n";
    }
		if (strncmp(fileSig, "LoCo",4) == 0)
			filetype = 1;
    else if (strcmp(fileSig, "%\002BX\n") == 0)
      filetype = 2;
		else if (strncmp(fileSig, "sigproc", 7) == 0 )
			filetype = 4;
		else if (strncmp(fileSig, "ASCII", 5) == 0 )   /*LoFASM ASCII data*/
			filetype = 4;
		else{
			filetype = 0;
			std::cout<< "File "<<filename<<" type is unknown."<<std::endl;
    }
	}
  else{
    std::cout<< "File: "<< filename <<" did not open\n";
  }
	fclose(f);
	return filetype;
}

DataFile * get_file_info(std::string filename){
  int ftype;
  DataFile *file_obj;
  ftype = check_file_type(filename);
  if (ftype == 1){
    std::cout<<"LoFASM RAW file is not supported"<< std::endl;
    file_obj = new DataFile (filename.c_str(), "r");
    return file_obj;
  }
	else if (ftype == 2){
    file_obj = new LfBbx (filename.c_str(), "r");
	}
	else if (ftype == 3){
    file_obj = new DataFile (filename.c_str(), "r");
    return file_obj;
  }
	else{
		std::cout<<"Unknown file type. Please check your file type."<<std::endl;
    file_obj = new DataFile (filename.c_str(), "r");
    return file_obj;
  }
  file_obj->read_head();
  // TODO: remove the assumption that dim1 is time axis.
  file_obj->start_time = file_obj->get_dim1_start();
  file_obj->time_span = file_obj->get_dim1_span();
  file_obj->end_time = file_obj->start_time + file_obj->time_span;
  file_obj->close_file();
  return file_obj;
}

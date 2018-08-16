// This is a header file to declear the io functions for reading data and
// channelize the data.
//
// Author: Jing Luo

#ifndef DEDISPERSION_IO_H_
#define DEDISPERSION_IO_H_

#include <math.h>
#include <string>
#include <vector>
#include <map>
#include <set>


class DataFile
// this is a base class for different type of files.
// NOTE: we assume the dedisperison data file will alway have a time axis.
// Add it will be dimension one.
// This data file class do not store the data. It only provides the API to
// write or read data.
{
  public:
    std::string filenamecpp;
    char filename[1024];   // file name
    char mode[3];          // I/O mode, write or read.
    int file_type;     // File type code
    FILE *f_ptr;       // file pointer
    int curr_time_index; // The time position/index where the file pointer is at
    int curr_dim2_index; // The dim2 position/index where the file pointer is at.
    double start_time; // Start time of the file
    double time_span;  // End time of the file
    DataFile * next_file;
    // Publich method
    DataFile (std::string fnamecpp, const char *input_mode);
    DataFile ();
    virtual int read_head() { return 0; };
    virtual int open_file() {return 0;};
    virtual int close_file(){return 0;};
    virtual double get_dim1_start(){return 0;};
    virtual double get_dim1_step(){return 0;};
    virtual double get_dim2_start(){return 0;};
    virtual double get_dim2_step(){return 0;};
    virtual std::string get_obs(){return 0;};
    virtual std::string get_frame(){return 0;};
    virtual double get_max_freq(){return 0;};
    virtual double get_min_freq(){return 0;};
    virtual double get_bandwidth(){return 0;}
    // Functions for read and write data.
    // a subint means several time bins
    virtual int read_next_subint_1d (int num_next_time_bin,
                                     std::vector <double> & data)
                                     { return 0; };
    virtual int write_next_subint_1d (int num_next_time_bin,
                                      std::vector <double> & data)
                                     { return 0; };
    virtual int read_next_subint_2d (int num_next_time_bin, int cplx_flag,
                                  std::vector < std:: vector <double> > & real_data,
                                  std::vector < std:: vector <double> > & cplx_data)
                                     { return 0; };
    virtual int write_next_subint_2d (int num_next_time_bin,
                                  std::vector < std:: vector <double> > & data)
                                     { return 0; };
};

class FileList
// This is a linked list for files at the same observatory, same polarization and
// the same frequency band.
// The files in the list will be sorted by time.
{
  public:
    std::map<double, DataFile> files;
    int num_files;
    double max_freq;
    double freq_band;
    void add_to_map(DataFile data_file);
    void get_files_by_time(double stime, double etime,
                           std::map<double, DataFile>::iterator &itlow,
                           std::map<double, DataFile>::iterator &itup);
};



class DataFileContainer
// This container only works for one station and one polarization.
{
  public:
    std::set <std::string> all_files;     // All the files in the directory
    std::set <std::string> processed_files;  // The files have been processed
    //std::set <string> waiting_files;    // waiting_files = all_files - processed - in queue
    std::set <std::string> files_in_queue;   // The files are waiting to be processed
    std::string obs;                    // observatory of the data files
    std::string frame;                  // Frame/polarization for the data files
    std::map <double, FileList> file_lists;     // The processing file queue.
                                        // The file queue is ascending sorted by time
    // NOTE disable for now, since we are not doing streaming
    //std::set <string> check_new_files();
    void add_file(std::string filename);
    void remove_from_list(std::string filename);
};


int check_file_type(std::string filename);
DataFile * get_file_info(std::string filename);

#endif  // Finish defining dedispersion_io.h

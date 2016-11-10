/* This is a test for reading the lofasm archive data formate. */
#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string>
#include<vector>
#include<algorithm>
#include<fstream>

#include "lofasm_data_class.h"
#include "lofasm_files.h"
#include "lofasmIO.h"
#include "lofasm_bx.h"

int main(){
    string filename;
    lfb_hdr head;
    FILE *fp;
    double *d, *data = NULL;
    char fn[1024];
    int i, j;
    double da;


    filename = "../../lofasmio-1.0/testdata/20160619_000326_AA.bbx.gz";
    strncpy(fn, filename.c_str(), sizeof(fn));
    cout<<fn<<endl;
    fp = lfopen(fn, "rb");
    lfbxRead(fp, &head, NULL);

    cout<<head.station<<endl;
    cout<<head.dims[0]<<" "<<head.dims[1]<<endl;
    FilterBank fdata(head.dims[1], head.dims[0]);
    cout<<"fdata "<<fdata.fltdata[0][0]<<endl;
    read_bx2flt(fp, head, fdata, head.dims[0], 0);
    cout<< "Finish reading"<<endl;
    cout<<fdata.numFreqBin<<endl;
    cout<<fdata.numTimeBin<<endl;
    cout<<head.dim1_label<<endl;
    cout<<head.dim2_label<<endl;
    // for (i=0; i<fdata.numTimeBin; i++){
    //   for (j=0; j<fdata.numFreqBin; j++){
    //     cout<< fdata.fltdata[j][i]<< " ";
    //   }
    //   cout<<endl;
    // }

    cout<<endl;
    return 0;
}

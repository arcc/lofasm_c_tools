#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <string.h>

#include "lofasm_raw_file.h"
#include "lofasm_data_class.h"
#include "lofasm_utils.h"
using namespace std;

int byte3_2_int(char *input)
/* A function to read 3 bytes data */
{
	unsigned int x = 0;
	char *y = (char *)(&x);
	y[0] = input[2];
	y[1] = input[1];
	y[2] = input[0];

 	return x;
}

void LofasmRawFile::get_raw_hdr(string fname){

  /*
  To read LoFASM raw data header from one file.

  */
    int status = 0;
    char *buffer = 0;
    size_t result;
    char Temp[20];

    ifstream infile(fname.c_str(), ios::binary);
    if (infile.is_open()){
        // Read header
        infile >> fileSig;
        if (fileSig != "LoCo") {
           cout << "This is not a LOFASM file!\n";
           exit(1);
        }
        buffer = new char[9];
        filename = fname;
        // Read header version
        infile.read(buffer,8);
        hdrVersion = atoi(buffer);
        cout<< " Header Version "<<hdrVersion<<endl;
        // Read header lenght
        infile.read(buffer,8);
        hdrLength = atoi(buffer);
        cout<< " Header Length "<<hdrLength<<endl;
        // Read LoFASM station
        infile.read(buffer,8);
        lofasmStation = buffer;
        cout<<" LoFAMS station "<<lofasmStation<<endl;

        // Read Number of frequency bins/Channels
        infile.read(buffer,8);
        numfBin = atoi(buffer);
        cout<<" Number of freq bin "<<numfBin<<endl;
        // Read start frequency in unit of Mhz
        infile.read(buffer,8);
        freqStart = atof(buffer);
        cout<<" Start Frequency "<<freqStart<<endl;

        // Read frequency steps in unit of Mhz
        infile.read(buffer,8);
        strcpy(Temp,"0.");
        strcat(Temp,buffer);
        freqStep = atof(Temp);
        cout<<" Start Step "<<freqStep<<endl;

        // Read MJD integer part in days
        infile.read(buffer,8);
        mjdDay = atoi(buffer);
        cout<<" Mjd Day "<<mjdDay<<endl;

        // Read MJD fractional part in millisecond
        infile.read(buffer,8);
        mjdMsec = atoi(buffer);
        cout<<" Mjd SEC "<<mjdMsec<<endl;
        // Compute the full mjd date
        startMJD = (double)mjdDay + (double)mjdMsec/SEC_PER_DAY/1000.0;
        // Read integration time for each integration
        infile.read(buffer,8);
        intgrTime = atof(buffer);
        cout<<" integration time  "<<intgrTime<<endl;
        // Read data format version
        infile.read(buffer,8);
        dataFmtVersion = atoi(buffer);
        cout<<" Data Formate version "<<dataFmtVersion<<endl;
        // Read notes
        infile.read(buffer,8);
        notes = buffer;
        cout<<" Notes "<<notes<<endl;

        free(buffer);
        readFlag = 1;
        infile.close();
    }
    else{
        cout<<"Unable to open file "<<fname<<endl;
        exit(1);
    }

}

/* Check if a packet a header packet*/
int LofasmRawFile::is_hdr_pkt(ifstream* infile)
/*
	Check if the pointer points to the begin of a packet a header packet.
	After this function, the pointer will be
	brought to the bigining of the initial point.
	Input:    ifstream infile
	OutPut:   hdrpkt == 1 it is a header packet
			  hdrpkt == 0 it is not a header packet
*/
{
	int hdrPkt;
	char buffer3Bytes[4];
	int hdrPktSig,hdrPktSig2;   // Two header signatures
	int backCheck;
	size_t result;
	/* First check if the pointer at begining of an integration */
	// Check condition 1. First 3 bytes matchs the signature.
	//    			   2. After 5 bytes, the signature repeats.




	// Read first 3 bytes packet signature
	infile->read(buffer3Bytes,3);
	hdrPktSig = byte3_2_int(buffer3Bytes);
	// Move 5 bytes to the next section
	infile->seekg(5, infile->cur);
	// If it is matches packet signature
	infile->read(buffer3Bytes,3);
	hdrPktSig2 = byte3_2_int(buffer3Bytes);

	// Bring pointer back to the early data see if it different
  if (infile->tellg()<19){
      hdrPkt = 0;
      return hdrPkt;
  }
  infile->seekg(-19, infile->cur);	//
  infile->read(buffer3Bytes,3);
	backCheck = byte3_2_int(buffer3Bytes);
	/* Check the header signature if the header signature matches */
	if (hdrPktSig == 14613675
	  	&& hdrPktSig == hdrPktSig2
	  	&& backCheck != hdrPktSig)
	{
	  	hdrPkt = 1;
  }
	else
	{
		  hdrPkt = 0;
	}
	/* Move back to the begining point*/
	infile->seekg(5,infile->cur);
	return hdrPkt;
}

int LofasmRawFile::check_raw_file(string fname){
    double numPacket;
    int packetCounter = 0;
    int hdrPktSig;               // The signature for the header packet
    int i;						 // Loop counter
    int intgrIndex;             // The index of intgration
    char buffer3Bytes[4];		 // A buffer for 3 bytes
    int status = 0;
    IntgrInfo itgINFO;

    /* Check if the header have been read */
    if(readFlag != 1)
    {
        cout<<"File Header has not been read. Start reading header."<<endl;
        get_raw_hdr(fname);
    }

    cout<<"Checking raw file "<<fname<<endl;

    ifstream infile(fname.c_str(), ios::binary);
    if (infile.is_open()){
        /* Get file size; */
        infile.seekg (0, infile.end);
        fileSize = infile.tellg();

        /* Calculate the packet size */
        packetSize = numfBin * 4;

        numPacket = ((double)fileSize - (double)hdrLength)/(double)packetSize;

        /* Check if the data has uncompleted packet*/
        if(fmod(numPacket,1.0) != 0.0)
        {
            cout<<" File "<<fname<<" has incompleted packet."<<endl;
            status = 2;
        }

        /* Check integration status*/
        /* Each integration contains 17 packets. First packet is the header packet,
         which contains repeatly 3 bytes header signature, 3 bytes integration
         number, 2 bytes header counter
         1. We have to determine if the packet is header packet.
         2. We will check the information in header packet
         3. The missing packet will be calculated
        */
			  /* Bring the pointer back to the end of header */
				infile.seekg (hdrLength, infile.beg);
    		packetCounter = PKT_PER_INTGR;
    		intgrIndex = -1;

				for(i=0;i<numPacket;i++)
				{
						/* Process the header packet. */
						if(is_hdr_pkt(&infile))
				    {
						    intgrIndex++;
		            /* Record the integration postion */
		 					  itgINFO.intgrPos = infile.tellg();
								/* Skip 3 bytes of header packet signature */
		 					  infile.seekg(3,infile.cur);

		 					  /* Read the integration ID */
		 					  infile.read(buffer3Bytes,3);
								itgINFO.intgrID = byte3_2_int(buffer3Bytes);

								/* Calculate the integration MJD*/
		 					  itgINFO.MJD = startMJD + intgrIndex * intgrTime/SEC_PER_DAY;
		            intgrList.push_back(itgINFO);

								// Checking if the last integration has missing packet.
							  if(packetCounter != PKT_PER_INTGR && intgrIndex != 0)
							  {
								    printf("File %s has missing/additional packet at"
									  " integration %d and it has %d packets.\n",
									  fname.c_str(),intgrIndex-1,packetCounter);
									  if(intgrIndex != 0)
											  intgrList[intgrIndex-1].badIntgr = 1;
							  }

		            // Reset packetCounter to 1
							  packetCounter = 1;
							  /* Bring the pointer to the next packet */
							  infile.seekg(packetSize-6,infile.cur);

						}
						else
						{
								packetCounter++;
								/*Go to next packet*/
								infile.seekg(packetSize,infile.cur);
						}

						if (infile.eof()){
						    if(packetCounter != PKT_PER_INTGR){
									  printf("File %s has missing/additional packet at"
									  " integration %d and it has %d packets.\n",
									  fname.c_str(),intgrIndex,packetCounter);
										intgrList[intgrIndex].badIntgr = 1;
								}
						}

			  }

    }


		if(numIntgr !=0 && intgrIndex >= numIntgr )
		{
				fprintf(stderr, "In file %s, the number of integration from header"
							 " is not match the file\n",fname.c_str());
				numIntgr = intgrIndex+1;
		}
		else{
		    numIntgr = intgrIndex+1;
		}

		infile.close();
		fileCheckFlag = 1;
    return status;
}

int LofasmRawFile::open_raw_file(string fname)
/*
  Open a lofasm raw file and put raw file information to the class.
*/
{
	if ((file->is_open())){
		file -> close();
	}
	filename = fname;
	cout<<"filename "<<filename.c_str()<<endl;
	file->open(filename.c_str(), ios::binary);
	cout<<"I am here 3"<<endl;
	get_raw_hdr(filename);
  check_raw_file(filename);
	return 0;
}


void LofasmRawFile::get_lofasm_raw_intgr(LofasmIntgr& intgr, int intgrIdx){
	  long itgPos;
    int numfreqBin;
    int i;
		int index, offset;
		numfreqBin = numfBin;
		vector<unsigned int> unsgnBuff1(numfreqBin*2), unsgnBuff2(numfreqBin*2);
		vector<signed int> sgnBuff1(numfreqBin*6), sgnBuff2(numfreqBin*6);
	  if (fileCheckFlag!=1){
			  cout<<"Please check your file first. Using class method ";
				cout<<"int lofasm_raw_file::check_raw_file(string fname)."<<endl;
				exit(1);
		}

		if (!(file->is_open())){
			  cout<<"Open file "<<filename<<endl;
				file->open(filename.c_str());
				if(!(file->is_open())){
					  cout<<"Can not open file "<<filename;
						exit(1);
				}
		}

		if (intgrIdx>=numIntgr){
			  cout<<" The integration requested is exceed file."<<endl;
				exit(1);
		}
		itgPos = intgrList[intgrIdx].intgrPos;

		file->seekg(itgPos,file->beg);

		if (is_hdr_pkt(file)==0){
			  cout<<" The integration position is not correct."<<endl;
				exit(1);
		}
    /* initialize vectors for all the polarizations*/
		if (intgr.numfBin!=numfBin){
			  cout<<" Polarization vector has not been initialized to the right size.";
				cout<<" initializing polarization vectors."<<endl;
				intgr.init_polar(numfBin);

		}
    /* Deal with bad Intergration*/
		if (intgrList[intgrIdx].badIntgr == 1){
			  cout<< "Intergration "<< intgrIdx<<" is a bad Intergration."<<endl;
				cout<< " In file "<<filename<<endl;
				fill (intgr.AA.begin(),intgr.AA.end(),0);
				fill (intgr.BB.begin(),intgr.BB.end(),0);
				fill (intgr.CC.begin(),intgr.CC.end(),0);
				fill (intgr.DD.begin(),intgr.DD.end(),0);
				fill (intgr.ABrl.begin(),intgr.ABrl.end(),0);
				fill (intgr.ACrl.begin(),intgr.ACrl.end(),0);
				fill (intgr.ADrl.begin(),intgr.ADrl.end(),0);
				fill (intgr.BCrl.begin(),intgr.BCrl.end(),0);
				fill (intgr.BDrl.begin(),intgr.BDrl.end(),0);
				fill (intgr.CDrl.begin(),intgr.CDrl.end(),0);
				fill (intgr.ABim.begin(),intgr.ABim.end(),0);
				fill (intgr.ACim.begin(),intgr.ACim.end(),0);
				fill (intgr.ADim.begin(),intgr.ADim.end(),0);
				fill (intgr.BCim.begin(),intgr.BCim.end(),0);
				fill (intgr.BDim.begin(),intgr.BDim.end(),0);
				fill (intgr.CDim.begin(),intgr.CDim.end(),0);
				return;
		}

		/*Read data */
		file->seekg(packetSize, file->cur); //skip the header packet.
    file->read(reinterpret_cast<char*>(unsgnBuff1.data()), unsgnBuff1.size()*sizeof(unsigned int));
		file->read(reinterpret_cast<char*>(sgnBuff1.data()), sgnBuff1.size()*sizeof(signed int));
		file->read(reinterpret_cast<char*>(unsgnBuff2.data()), unsgnBuff2.size()*sizeof(unsigned int));
		file->read(reinterpret_cast<char*>(sgnBuff2.data()), sgnBuff2.size()*sizeof(signed int));

		/* Get each polarization by arrange data buffer */
    // Get AA data;
		for (i=0;i<numfreqBin/2;i++){
			  // In unsigned data packet 1,9
				// Offset is the index of data packet in the Buffer
				offset = 0;
			  intgr.AA[2*i] = unsgnBuff1[2*i+offset*numfreqBin];
				intgr.AA[2*i+1] = unsgnBuff2[2*i+offset*numfreqBin];
				intgr.BB[2*i] = unsgnBuff1[2*i+1+offset*numfreqBin];
				intgr.BB[2*i+1] = unsgnBuff2[2*i+1+offset*numfreqBin];
				// In unsigned data packet 2,10
				offset = 1;
			  intgr.CC[2*i] = unsgnBuff1[2*i+offset*numfreqBin]; // Jump to data packet 2
				intgr.CC[2*i+1] = unsgnBuff2[2*i+offset*numfreqBin]; // Jump to data packet 10
				intgr.DD[2*i] = unsgnBuff1[2*i+1+offset*numfreqBin];
				intgr.DD[2*i+1] = unsgnBuff2[2*i+1+offset*numfreqBin];
				// In signed data packet 3,11
				offset = 0;
        intgr.ABrl[2*i] = sgnBuff1[2*i+offset*numfreqBin];
				intgr.ABrl[2*i+1] = sgnBuff2[2*i+offset*numfreqBin];
				intgr.ABim[2*i] = sgnBuff1[2*i+1+offset*numfreqBin];
				intgr.ABim[2*i+1] = sgnBuff2[2*i+1+offset*numfreqBin];
				// In signed data packet 4,12
				offset = 1;
				intgr.ACrl[2*i] = sgnBuff1[2*i+offset*numfreqBin];
				intgr.ACrl[2*i+1] = sgnBuff2[2*i+offset*numfreqBin];
				intgr.ACim[2*i] = sgnBuff1[2*i+1+offset*numfreqBin];
				intgr.ACim[2*i+1] = sgnBuff2[2*i+1+offset*numfreqBin];

				// In signed data packet 5,13
				offset = 2;
				intgr.ADrl[2*i] = sgnBuff1[2*i+offset*numfreqBin];
				intgr.ADrl[2*i+1] = sgnBuff2[2*i+offset*numfreqBin];
				intgr.ADim[2*i] = sgnBuff1[2*i+1+offset*numfreqBin];
				intgr.ADim[2*i+1] = sgnBuff2[2*i+1+offset*numfreqBin];

				// In signed data packet 6,14
				offset = 3;
				intgr.BCrl[2*i] = sgnBuff1[2*i+offset*numfreqBin];
				intgr.BCrl[2*i+1] = sgnBuff2[2*i+offset*numfreqBin];
				intgr.BCim[2*i] = sgnBuff1[2*i+1+offset*numfreqBin];
				intgr.BCim[2*i+1] = sgnBuff2[2*i+1+offset*numfreqBin];

				// In signed data packet 7,15
				offset = 4;
				intgr.BDrl[2*i] = sgnBuff1[2*i+offset*numfreqBin];
				intgr.BDrl[2*i+1] = sgnBuff2[2*i+offset*numfreqBin];
				intgr.BDim[2*i] = sgnBuff1[2*i+1+offset*numfreqBin];
				intgr.BDim[2*i+1] = sgnBuff2[2*i+1+offset*numfreqBin];

				// In signed data packet 8,16
				offset = 5;
				intgr.CDrl[2*i] = sgnBuff1[2*i+offset*numfreqBin];
				intgr.CDrl[2*i+1] = sgnBuff2[2*i+offset*numfreqBin];
				intgr.CDim[2*i] = sgnBuff1[2*i+1+offset*numfreqBin];
				intgr.CDim[2*i+1] = sgnBuff2[2*i+1+offset*numfreqBin];
		}

	  // swap big endient
		for (i=0;i<numfreqBin;i++){
			  swap4((char *)&intgr.AA[i]);
				swap4((char *)&intgr.BB[i]);
				swap4((char *)&intgr.CC[i]);
				swap4((char *)&intgr.DD[i]);
				swap4((char *)&intgr.ABrl[i]);
				swap4((char *)&intgr.ABim[i]);
				swap4((char *)&intgr.ACrl[i]);
				swap4((char *)&intgr.ACim[i]);
				swap4((char *)&intgr.ADrl[i]);
				swap4((char *)&intgr.ADim[i]);
				swap4((char *)&intgr.BCrl[i]);
				swap4((char *)&intgr.BCim[i]);
				swap4((char *)&intgr.BDrl[i]);
				swap4((char *)&intgr.BDim[i]);
				swap4((char *)&intgr.CDrl[i]);
				swap4((char *)&intgr.CDim[i]);

		}
    return;

}

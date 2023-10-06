//
//  rpdz.cpp
//  
//
//  Created by Lee Drake on 5/16/18.
//  Updated by Lee Drake with help from Frits Vossman on 10/05/2023

//
#include <Rcpp.h>
#include <fstream>

using namespace Rcpp;
// [[Rcpp::export()]]
NumericVector readPDZ24(std::string fileName, int start, int size) {
    uint32_t a[size];
    std::ifstream file (fileName.c_str(), std::ios::in | std::ios::binary);
    if (file.is_open()) {
        file.seekg(start);
        file.read ((char*)&a, sizeof(a));
        file.close();
    }
    NumericVector res(size);
    for (unsigned long long int i = 0; i < size; ++i)
    res(i) = (a[i]) ;
    return res;
}

// [[Rcpp::export]]
NumericVector readPDZ(std::string fileName, int start, int size) {
    uint32_t a[size];
    std::ifstream file (fileName.c_str(), std::ios::in | std::ios::binary);
    if (file.is_open()) {
        file.seekg(start);
        file.read ((char*)&a, sizeof(a));
        file.close();
    }
    NumericVector res(size);
    for (unsigned long long int i = 0; i < size; ++i)
    res(i) = (a[i]) ;
    return res;
}

// [[Rcpp::export]]
std::vector<uint32_t> readPDZ25(std::string fileName) {
    int fileIndex;          // index in to file position
    bool looping = false;   // used as a flag to control while loop, initialized as false
    short recordID;
    int recordLength;
    uint32_t spectrum[2048];
    char record[5000];               // I hope this is big enough
    
    std::ifstream file(fileName, std::ios::binary); // Declare 'file' as a std::ifstream object and open the binary file

    if (file.is_open()) {
        // I read the first 20 bytes, you could check if a[0] = 0x19  (= 25 decimal), as it is for PDZ v25.
        file.read ((char*)&record, 20);
        fileIndex = 20;
        while (!looping) { // changed looping condition to use boolean value instead of integer
            // read the ID and length of  the record, 2 bytes for the ID, 4 bytes for the length
            file.read((char *)&record, 6);
            recordID =  *(short *) (record);       // changed pointer arithmetic here
            recordLength = *(int *) (record + 2);  // changed pointer arithmetic here
            
            if (recordID == 3) {
                looping = true;        // set flag to break while loop
                file.read((char*)&record, recordLength);
                int spOff = (*(int*)(record + 114)) * 2;  // changed pointer arithmetic here and removed unnecessary casts
                 spOff += 120;

                
                for (int i=0; i<2048; i++) {
                    spectrum[i] = *(uint32_t *)(&record[spOff+i*sizeof(uint32_t)]); // Fixed the pointer arithmetic here to correctly point to each uint32_t element in 'record' array.
                }
            } else if (recordID == 4) {
                looping = true;          // set flag to break while loop
                int spOff = 164;
                
                for (int i=0; i<2048; i++) {
                    spectrum[i] = *(uint32_t *)(&record[spOff+i*sizeof(uint32_t)]); // Fixed the pointer arithmetic here to correctly point to each uint32_t element in 'record' array.
                }
            } else {
                fileIndex += recordLength + 6;  // updated to add ID and length back
                file.seekg(fileIndex);
            }
        }    // end while
        file.close();
    } else {
       // some message the file did not open
    }
   return std::vector<uint32_t>(spectrum, spectrum + 2048);  // Convert array to vector before returning
}

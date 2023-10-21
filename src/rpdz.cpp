//
//  rpdz.cpp
//  
//
//  Created by Lee Drake on 5/16/18.
//  Updated by Lee Drake with help from Frits Vossman on 10/05/2023

//
#include <Rcpp.h>
#include <fstream>
#include <vector>

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
    int fileIndex;
    bool looping = false;
    short recordID;
    int recordLength;
    uint32_t spectrum[2048];
    std::vector<char> record(10000000); // This is okay
    
    std::ifstream file(fileName, std::ios::binary);

    if (file.is_open()) {
        file.read(record.data(), 20); // Change here: use record.data() to get the underlying array
        fileIndex = 20;
        while (!looping) {
            file.read(record.data(), 6); // Change here: use record.data()
            
            // Use reinterpret_cast for strict type aliasing, ensuring correct pointer types
            recordID = *reinterpret_cast<short*>(record.data());
            recordLength = *reinterpret_cast<int*>(record.data() + 2);
            
            if (recordID == 3) {
                looping = true;
                file.read(record.data(), recordLength); // Change here: use record.data()
                int spOff = (*reinterpret_cast<int*>(record.data() + 114)) * 2;
                spOff += 120;

                for (int i = 0; i < 2048; i++) {
                    // Access the elements of 'record' correctly
                    spectrum[i] = *reinterpret_cast<uint32_t*>(record.data() + spOff + i * sizeof(uint32_t));
                }
            } else if (recordID == 4) {
                looping = true;
                int spOff = 164;

                for (int i = 0; i < 2048; i++) {
                    // Access the elements of 'record' correctly
                    spectrum[i] = *reinterpret_cast<uint32_t*>(record.data() + spOff + i * sizeof(uint32_t));
                }
            } else {
                fileIndex += recordLength + 6;
                file.seekg(fileIndex);
            }
        }
        file.close();
    } else {
        // Handle the error: the file did not open
        Rcpp::stop("Could not open file");  // This is how you throw an error in Rcpp
    }
    
    // Convert array to vector before returning
    return std::vector<uint32_t>(spectrum, spectrum + 2048);
}

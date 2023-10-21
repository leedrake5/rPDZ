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
float readPDZ24FloatFetch(std::string fileName, int position) { // Note the change in parameters
    float result; // This will store our single float value

    std::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary); // Open the file in binary mode

    if (file.is_open()) {
        file.seekg(position); // Move to the correct position in the file
        file.read(reinterpret_cast<char*>(&result), sizeof(float)); // Read one float's worth of data
        file.close(); // Close the file
    } else {
        Rcpp::stop("Unable to open file"); // If we fail to open the file, stop and return an error message
    }

    return result; // Return the single float we read
}

// [[Rcpp::export]]
double readPDZ24DoubleFetch(std::string fileName, int position) { // Adjusted the function's signature to return double
    double result; // This will store our single double value

    std::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary); // Open the file in binary mode

    if (file.is_open()) {
        file.seekg(position); // Move to the correct position in the file
        file.read(reinterpret_cast<char*>(&result), sizeof(double)); // Read one double's worth of data
        file.close(); // Close the file
    } else {
        Rcpp::stop("Unable to open file"); // If we fail to open the file, stop and return an error message
    }

    return result; // Return the single double we read
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

// [[Rcpp::export]]
float readPDZ25eVCH(std::string fileName) {  // changed return type to float
    int fileIndex;
    bool looping = true;  // changed to true for the purpose of the while loop
    short recordID;
    int recordLength;
    float singleValue;  // to hold the single float value
    std::vector<char> record(10000000);  // This is okay
    
    std::ifstream file(fileName, std::ios::binary);

    if (file.is_open()) {
        file.read(record.data(), 20);  // reading initial part of the file
        fileIndex = 20;

        // Changed to do-while to ensure the loop executes at least once
        do {
            file.read(record.data(), 6);  // reading record header

            // Using reinterpret_cast for the correct type interpretation
            recordID = *reinterpret_cast<short*>(record.data());
            recordLength = *reinterpret_cast<int*>(record.data() + 2);

            if (recordID == 3) {  // condition for specific record type
                file.read(record.data(), recordLength);  // reading the entire record

                // proper casting and pointer arithmetic to read the float value
                singleValue = *reinterpret_cast<float*>(record.data() + 74);  // assuming the float is at position 76
                break;  // break the loop as we found the value we were looking for

            } else {  // if the record is not the type we're looking for
                fileIndex += recordLength + 6;
                file.seekg(fileIndex);  // skip to the next record
            }
        } while (looping);  // using the condition variable to control the loop

        file.close();  // closing the file after reading
    } else {
        Rcpp::stop("Could not open file");  // error handling for file opening
    }

    return singleValue;  // returning the single float value
}

// [[Rcpp::export]]
float readPDZ25LiveTime(std::string fileName) {  // changed return type to float
    int fileIndex;
    bool looping = true;  // changed to true for the purpose of the while loop
    short recordID;
    int recordLength;
    float singleValue;  // to hold the single float value
    std::vector<char> record(10000000);  // This is okay
    
    std::ifstream file(fileName, std::ios::binary);

    if (file.is_open()) {
        file.read(record.data(), 20);  // reading initial part of the file
        fileIndex = 20;

        // Changed to do-while to ensure the loop executes at least once
        do {
            file.read(record.data(), 6);  // reading record header

            // Using reinterpret_cast for the correct type interpretation
            recordID = *reinterpret_cast<short*>(record.data());
            recordLength = *reinterpret_cast<int*>(record.data() + 2);

            if (recordID == 3) {  // condition for specific record type
                file.read(record.data(), recordLength);  // reading the entire record

                // proper casting and pointer arithmetic to read the float value
                singleValue = *reinterpret_cast<float*>(record.data() + 36);  // assuming the float is at position 76
                break;  // break the loop as we found the value we were looking for

            } else {  // if the record is not the type we're looking for
                fileIndex += recordLength + 6;
                file.seekg(fileIndex);  // skip to the next record
            }
        } while (looping);  // using the condition variable to control the loop

        file.close();  // closing the file after reading
    } else {
        Rcpp::stop("Could not open file");  // error handling for file opening
    }

    return singleValue;  // returning the single float value
}

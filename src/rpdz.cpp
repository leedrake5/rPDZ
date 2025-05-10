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
#include <cstdint>

using namespace Rcpp;
// [[Rcpp::export()]]
std::vector<float> readPDZ24(const std::string& fileName) {
    float spectrum[2048] = {0.0f}; // Initialize to zeros
    
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Seek to the position where spectrum starts (361)
    file.seekg(478L, std::ios_base::beg);
    
    for (int i = 0; i < 2048; ++i) {
        uint32_t count_value;
        file.read(reinterpret_cast<char*>(&count_value), sizeof(uint32_t));
        
        if (!file) { Rcpp::stop("Failed to read record"); }

        spectrum[i] = static_cast<float>(count_value) / 65536.0f;
    }
    
    file.close();

    return std::vector<float>(spectrum, spectrum + 2048);
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
std::vector<float> readPDZ25(std::string fileName) {
    short recordID;
    int recordLength;
    float spectrum[2048] = {0.0f}; // Initialize to zeros
    std::vector<char> record(100000000); // This is okay
    
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    file.read(record.data(), 20);
    if (!file) { Rcpp::stop("Failed to read initial 20 bytes"); }

    while (true) {  // Using a plain while loop for clarity
        file.read(record.data(), 6);
        if (!file) { Rcpp::stop("Failed to read header"); }

        recordID = *reinterpret_cast<short*>(record.data());
        recordLength = *reinterpret_cast<int*>(record.data() + 2);

        if (recordID == 3 || recordID == 4) {
            if (recordLength > record.size()) { Rcpp::stop("Record length too large"); }

            file.read(record.data(), recordLength);
            if (!file) { Rcpp::stop("Failed to read record"); }

            int spOff = (*reinterpret_cast<int*>(record.data() + 114)) * 2;
            spOff += (recordID == 3) ? 120 : 164;

            if (spOff + 2048 * sizeof(uint32_t) > record.size()) {
                Rcpp::stop("Spectrum offset out of bounds");
            }

            for (int i = 0; i < 2048; i++) {
                uint32_t count_value = *reinterpret_cast<uint32_t*>(record.data() + spOff + i * sizeof(uint32_t));
                spectrum[i] = static_cast<float>(count_value) / 65536.0f;
            }
            
            break;  // Exit the loop once you've found and read the spectrum
        } else {
            // Seek past the current record to find the next one
            if (!file.seekg(recordLength, std::ios_base::cur)) {
                Rcpp::stop("Failed to seek in file");
            }
        }
    }
    file.close();

    return std::vector<float>(spectrum, spectrum + 2048);
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

// [[Rcpp::export]]
float readPDZ25FloatFetch(std::string fileName, int position) {  // changed return type to float
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
                singleValue = *reinterpret_cast<float*>(record.data() + position);  // assuming the float is at position 76
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

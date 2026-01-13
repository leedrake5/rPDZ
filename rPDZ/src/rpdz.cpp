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

    // Seek to the position where spectrum starts (358 bytes header)
    file.seekg(358L, std::ios_base::beg);
    
    for (int i = 0; i < 2048; ++i) {
        uint32_t count_value;
        file.read(reinterpret_cast<char*>(&count_value), sizeof(uint32_t));
        
        if (!file) { Rcpp::stop("Failed to read record"); }

        spectrum[i] = static_cast<float>(count_value);
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
                spectrum[i] = static_cast<float>(count_value);
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

// =============================================================================
// NEW GENERALIZABLE PDZ FUNCTIONS (added 2026-01-12)
// =============================================================================

// PDZ Format Constants
// These are for the "simple" PDZ format found in example files (not v25 record-based)
const int PDZ_HEADER_SIZE = 358;           // Fixed header size in bytes
const int PDZ_SPECTRUM_CHANNELS = 2048;    // Number of channels per spectrum
const int PDZ_SPECTRUM_SIZE = 8192;        // 2048 * sizeof(uint32_t)
const int PDZ_INTER_SPECTRUM_GAP = 192;    // Metadata between spectra in multi-spectrum files

// Metadata Position Constants (simple_v2 format)
//
// eVCh (energy per channel calibration):
//   Position 50 in main header (double, 8 bytes)
//   SHARED across all spectra in the file
//
// Main header metadata (spectrum 0):
//   Position 50:  eVCh (double, 8 bytes) - shared
//   Position 162: Tube voltage in kV (float, 4 bytes)
//   Position 354: LiveTime in seconds (float, 4 bytes)
//
// Inter-spectrum gap metadata (spectrum N, N>0):
//   Gap block for spectrum N starts at: 8550 + (N-1) * (8192 + 192)
//   Gap structure (offsets from gap start):
//     Offset 152: DeadTime in seconds (float, 4 bytes)
//     Offset 160: LiveTime in seconds (float, 4 bytes)
//     Offset 164: Tube voltage in kV (float, 4 bytes)
//     Offset 168: Filament current in uA (float, 4 bytes)
//
// Example positions for dual spectrum file:
//   Spectrum 0: LiveTime=354, Voltage=162
//   Spectrum 1: LiveTime=8710, Voltage=8714
//
const int PDZ_SPEC0_EVCH_POS = 50;         // eVCh position (shared, main header, double)
const int PDZ_CAL_VALUE1_POS = 58;         // First calibration value (double)
const int PDZ_CAL_VALUE2_POS = 66;         // Second calibration value (double)
const int PDZ_FILTER1_ELEMENT_POS = 114;   // Filter 1 atomic number (byte)
const int PDZ_FILTER1_THICK_POS = 116;     // Filter 1 thickness in um (short)
const int PDZ_FILTER2_ELEMENT_POS = 118;   // Filter 2 atomic number (byte)
const int PDZ_FILTER2_THICK_POS = 120;     // Filter 2 thickness in um (short)
const int PDZ_FILTER3_ELEMENT_POS = 122;   // Filter 3 atomic number (byte)
const int PDZ_FILTER3_THICK_POS = 124;     // Filter 3 thickness in um (short)
const int PDZ_DATETIME_POS = 146;          // Acquisition datetime (SYSTEMTIME, 16 bytes)
const int PDZ_SPEC0_VOLTAGE_POS = 162;     // Tube voltage position for spectrum 0 (float)
const int PDZ_SPEC0_CURRENT_POS = 166;     // Tube current position for spectrum 0 (float)
const int PDZ_SERIAL_POS = 186;            // Serial number string (ASCII, null-terminated)
const int PDZ_SERIAL_LEN = 16;             // Max serial number length
const int PDZ_SPEC0_REALTIME_POS = 342;    // RealTime/TotalPacketTime for spectrum 0 (float)
const int PDZ_SPEC0_DEADTIME_POS = 346;    // DeadTime for spectrum 0 (float)
const int PDZ_SPEC0_RESETTIME_POS = 350;   // ResetTime for spectrum 0 (float)
const int PDZ_SPEC0_LIVETIME_POS = 354;    // LiveTime position for spectrum 0 (float)
const int PDZ_GAP_REALTIME_OFFSET = 148;   // RealTime offset within inter-spectrum gap
const int PDZ_GAP_DEADTIME_OFFSET = 152;   // DeadTime offset within inter-spectrum gap
const int PDZ_GAP_RESETTIME_OFFSET = 156;  // ResetTime offset within inter-spectrum gap
const int PDZ_GAP_LIVETIME_OFFSET = 160;   // LiveTime offset within inter-spectrum gap
const int PDZ_GAP_VOLTAGE_OFFSET = 164;    // Tube voltage offset within inter-spectrum gap
const int PDZ_GAP_CURRENT_OFFSET = 168;    // Filament current offset within inter-spectrum gap

// [[Rcpp::export]]
int getPDZSpectrumCount(const std::string& fileName) {
    // Returns number of spectra in file (1 or 2, potentially more in future)
    // Byte 1 of file contains spectrum count
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    file.seekg(1);  // Spectrum count is at byte offset 1
    unsigned char count;
    file.read(reinterpret_cast<char*>(&count), 1);
    file.close();

    if (count < 1 || count > 10) {
        // If value seems unreasonable, might be different format
        Rcpp::warning("Unexpected spectrum count value: %d - file may be different format", count);
    }

    return static_cast<int>(count);
}

// [[Rcpp::export]]
int getPDZFormatVersion(const std::string& fileName) {
    // Returns format version byte (byte 0)
    // Known values: 0x02 for simple format
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    unsigned char version;
    file.read(reinterpret_cast<char*>(&version), 1);
    file.close();

    return static_cast<int>(version);
}

// [[Rcpp::export]]
bool isPDZv25Format(const std::string& fileName) {
    // Check if file is v25 format by looking for "pdz25" marker
    // v25 files have record-based structure with "pdz25" in unicode at start
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Read first 30 bytes to check for markers
    char buffer[30];
    file.read(buffer, 30);
    file.close();

    // Check for "pdz25" in UTF-16LE (little endian unicode)
    // "pdz25" = 0x70 0x00 0x64 0x00 0x7a 0x00 0x32 0x00 0x35 0x00
    const unsigned char pdz25_marker[] = {0x70, 0x00, 0x64, 0x00, 0x7a, 0x00, 0x32, 0x00, 0x35, 0x00};

    for (int offset = 0; offset <= 20; offset++) {
        bool match = true;
        for (int i = 0; i < 10 && match; i++) {
            if (static_cast<unsigned char>(buffer[offset + i]) != pdz25_marker[i]) {
                match = false;
            }
        }
        if (match) return true;
    }

    return false;
}

// [[Rcpp::export]]
std::vector<float> readPDZSpectrum(const std::string& fileName, int spectrumIndex) {
    // Read spectrum by index (0-based) from PDZ file
    // Works with both single and multi-spectrum files

    float spectrum[PDZ_SPECTRUM_CHANNELS] = {0.0f};

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Get spectrum count to validate index
    file.seekg(1);
    unsigned char count;
    file.read(reinterpret_cast<char*>(&count), 1);

    if (spectrumIndex < 0 || spectrumIndex >= count) {
        file.close();
        Rcpp::stop("Spectrum index %d out of range (file has %d spectra)", spectrumIndex, count);
    }

    // Calculate offset for requested spectrum
    // First spectrum: offset 358
    // Second spectrum: offset 358 + 8192 + 192 = 8742
    // Formula: 358 + spectrumIndex * (8192 + 192)
    long offset = PDZ_HEADER_SIZE + spectrumIndex * (PDZ_SPECTRUM_SIZE + PDZ_INTER_SPECTRUM_GAP);

    file.seekg(offset, std::ios_base::beg);

    for (int i = 0; i < PDZ_SPECTRUM_CHANNELS; ++i) {
        uint32_t count_value;
        file.read(reinterpret_cast<char*>(&count_value), sizeof(uint32_t));

        if (!file) {
            file.close();
            Rcpp::stop("Failed to read channel %d of spectrum %d", i, spectrumIndex);
        }

        spectrum[i] = static_cast<float>(count_value);
    }

    file.close();
    return std::vector<float>(spectrum, spectrum + PDZ_SPECTRUM_CHANNELS);
}

// [[Rcpp::export]]
Rcpp::List getPDZInfo(const std::string& fileName) {
    // Returns comprehensive info about PDZ file
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Get file size
    file.seekg(0, std::ios::end);
    long fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read header bytes
    unsigned char header[10];
    file.read(reinterpret_cast<char*>(header), 10);
    file.close();

    int formatVersion = header[0];
    int spectrumCount = header[1];

    // Check if v25 format
    bool isV25 = isPDZv25Format(fileName);

    // Determine format type string
    std::string formatType;
    if (isV25) {
        formatType = "v25_record";
    } else if (formatVersion == 2) {
        formatType = "simple_v2";
    } else {
        formatType = "unknown";
    }

    return Rcpp::List::create(
        Rcpp::Named("file_size") = fileSize,
        Rcpp::Named("format_version") = formatVersion,
        Rcpp::Named("spectrum_count") = spectrumCount,
        Rcpp::Named("is_v25") = isV25,
        Rcpp::Named("format_type") = formatType,
        Rcpp::Named("header_size") = PDZ_HEADER_SIZE,
        Rcpp::Named("channels_per_spectrum") = PDZ_SPECTRUM_CHANNELS
    );
}

// [[Rcpp::export]]
float getPDZLiveTime(const std::string& fileName, int spectrumIndex) {
    // Returns LiveTime (in seconds) for the specified spectrum
    //
    // Position calculation:
    //   Spectrum 0: position 354 (in main header)
    //   Spectrum N (N>0): 8550 + (N-1) * 8384 + 160
    //                   = 8710 + (N-1) * 8384
    //
    // Example positions:
    //   Spectrum 0: 354
    //   Spectrum 1: 8710
    //   Spectrum 2: 17094 (if 3+ spectra exist)

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Validate spectrum index
    file.seekg(1);
    unsigned char count;
    file.read(reinterpret_cast<char*>(&count), 1);

    if (spectrumIndex < 0 || spectrumIndex >= count) {
        file.close();
        Rcpp::stop("Spectrum index %d out of range (file has %d spectra)", spectrumIndex, count);
    }

    // Calculate position based on spectrum index
    long position;
    if (spectrumIndex == 0) {
        position = PDZ_SPEC0_LIVETIME_POS;  // 354
    } else {
        // Inter-spectrum metadata block starts after first spectrum ends
        // Block for spectrum N starts at: 8550 + (N-1) * (8192 + 192)
        long blockStart = PDZ_HEADER_SIZE + PDZ_SPECTRUM_SIZE +
                          (spectrumIndex - 1) * (PDZ_SPECTRUM_SIZE + PDZ_INTER_SPECTRUM_GAP);
        position = blockStart + PDZ_GAP_LIVETIME_OFFSET;  // +160
    }

    file.seekg(position);
    float liveTime;
    file.read(reinterpret_cast<char*>(&liveTime), sizeof(float));

    if (!file) {
        file.close();
        Rcpp::stop("Failed to read LiveTime at position %ld", position);
    }

    file.close();
    return liveTime;
}

// [[Rcpp::export]]
double getPDZeVCh(const std::string& fileName, int spectrumIndex) {
    // Returns eVCh (energy per channel calibration) for the specified spectrum
    //
    // In simple_v2 format, eVCh is stored as a double at position 50 in the
    // main header. This value is SHARED across all spectra in the file.
    //
    // Evidence from manufacturer CSV exports shows that both spectra in a dual
    // file report the same eVCh value, confirming the shared calibration.
    //
    // Behavior:
    //   All spectra: Returns value at position 50 (shared calibration)

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Validate spectrum index
    file.seekg(1);
    unsigned char count;
    file.read(reinterpret_cast<char*>(&count), 1);

    if (spectrumIndex < 0 || spectrumIndex >= count) {
        file.close();
        Rcpp::stop("Spectrum index %d out of range (file has %d spectra)", spectrumIndex, count);
    }

    // Read shared eVCh from position 50 (applies to all spectra)
    file.seekg(PDZ_SPEC0_EVCH_POS);  // 50
    double eVCh;
    file.read(reinterpret_cast<char*>(&eVCh), sizeof(double));

    if (!file) {
        file.close();
        Rcpp::stop("Failed to read eVCh at position 50");
    }

    file.close();
    return eVCh;
}

// [[Rcpp::export]]
float getPDZTubeVoltage(const std::string& fileName, int spectrumIndex) {
    // Returns tube voltage in kV for the specified spectrum
    //
    // Position calculation:
    //   Spectrum 0: position 162 (in main header)
    //   Spectrum N (N>0): gap_start + 164
    //     where gap_start = 8550 + (N-1) * (8192 + 192)
    //
    // Example positions:
    //   Spectrum 0: 162
    //   Spectrum 1: 8714

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Validate spectrum index
    file.seekg(1);
    unsigned char count;
    file.read(reinterpret_cast<char*>(&count), 1);

    if (spectrumIndex < 0 || spectrumIndex >= count) {
        file.close();
        Rcpp::stop("Spectrum index %d out of range (file has %d spectra)", spectrumIndex, count);
    }

    // Calculate position based on spectrum index
    long position;
    if (spectrumIndex == 0) {
        position = PDZ_SPEC0_VOLTAGE_POS;  // 162
    } else {
        // Inter-spectrum metadata block starts after first spectrum ends
        long blockStart = PDZ_HEADER_SIZE + PDZ_SPECTRUM_SIZE +
                          (spectrumIndex - 1) * (PDZ_SPECTRUM_SIZE + PDZ_INTER_SPECTRUM_GAP);
        position = blockStart + PDZ_GAP_VOLTAGE_OFFSET;  // +164
    }

    file.seekg(position);
    float voltage;
    file.read(reinterpret_cast<char*>(&voltage), sizeof(float));

    if (!file) {
        file.close();
        Rcpp::stop("Failed to read tube voltage at position %ld", position);
    }

    file.close();
    return voltage;
}

// [[Rcpp::export]]
Rcpp::List getPDZMetadata(const std::string& fileName) {
    // Returns comprehensive metadata from PDZ file as a named list
    //
    // This extracts all available metadata from the file header and
    // per-spectrum metadata blocks. The structure follows the manufacturer's
    // CSV export format where possible.
    //
    // Returns a list with:
    //   - File-level metadata (serial number, format info, calibration)
    //   - Per-spectrum metadata (voltage, current, livetime, filters)
    //
    // R usage:
    //   meta <- getPDZMetadata("file.pdz")
    //   meta$serial_number    # Instrument serial
    //   meta$eVCh             # Energy calibration (shared)
    //   meta$spectra[[1]]     # Metadata for first spectrum
    //   meta$spectra[[1]]$tube_voltage_kV

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Get file size
    file.seekg(0, std::ios::end);
    long fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read format version and spectrum count
    unsigned char formatVersion, spectrumCount;
    file.read(reinterpret_cast<char*>(&formatVersion), 1);
    file.read(reinterpret_cast<char*>(&spectrumCount), 1);

    // Read eVCh (shared across all spectra)
    file.seekg(PDZ_SPEC0_EVCH_POS);
    double eVCh;
    file.read(reinterpret_cast<char*>(&eVCh), sizeof(double));

    // Read calibration values
    file.seekg(PDZ_CAL_VALUE1_POS);
    double calValue1;
    file.read(reinterpret_cast<char*>(&calValue1), sizeof(double));

    file.seekg(PDZ_CAL_VALUE2_POS);
    double calValue2;
    file.read(reinterpret_cast<char*>(&calValue2), sizeof(double));

    // Read filter information (spectrum 0 / main header)
    file.seekg(PDZ_FILTER1_ELEMENT_POS);
    unsigned char filter1Element;
    file.read(reinterpret_cast<char*>(&filter1Element), 1);

    file.seekg(PDZ_FILTER1_THICK_POS);
    short filter1Thickness;
    file.read(reinterpret_cast<char*>(&filter1Thickness), sizeof(short));

    file.seekg(PDZ_FILTER2_ELEMENT_POS);
    unsigned char filter2Element;
    file.read(reinterpret_cast<char*>(&filter2Element), 1);

    file.seekg(PDZ_FILTER2_THICK_POS);
    short filter2Thickness;
    file.read(reinterpret_cast<char*>(&filter2Thickness), sizeof(short));

    file.seekg(PDZ_FILTER3_ELEMENT_POS);
    unsigned char filter3Element;
    file.read(reinterpret_cast<char*>(&filter3Element), 1);

    file.seekg(PDZ_FILTER3_THICK_POS);
    short filter3Thickness;
    file.read(reinterpret_cast<char*>(&filter3Thickness), sizeof(short));

    // Read serial number
    file.seekg(PDZ_SERIAL_POS);
    char serialBuffer[PDZ_SERIAL_LEN + 1] = {0};
    file.read(serialBuffer, PDZ_SERIAL_LEN);
    std::string serialNumber(serialBuffer);

    // Read acquisition datetime (SYSTEMTIME format at position 146)
    // SYSTEMTIME structure: 16 bytes (8 shorts)
    //   wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds
    file.seekg(PDZ_DATETIME_POS);
    uint16_t sysTime[8];
    file.read(reinterpret_cast<char*>(sysTime), 16);

    int acqYear = sysTime[0];
    int acqMonth = sysTime[1];
    int acqDay = sysTime[3];  // wDay is at index 3, wDayOfWeek is at index 2
    int acqHour = sysTime[4];
    int acqMinute = sysTime[5];
    int acqSecond = sysTime[6];
    int acqMilliseconds = sysTime[7];

    // Format datetime string (ISO 8601 format)
    char datetimeStr[30];
    snprintf(datetimeStr, sizeof(datetimeStr), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             acqYear, acqMonth, acqDay, acqHour, acqMinute, acqSecond, acqMilliseconds);
    std::string acquisitionDatetime(datetimeStr);

    // Check if v25 format
    bool isV25 = isPDZv25Format(fileName);

    // Determine format type string
    std::string formatType;
    if (isV25) {
        formatType = "v25_record";
    } else if (formatVersion == 2) {
        formatType = "simple_v2";
    } else {
        formatType = "unknown";
    }

    // Create per-spectrum metadata list
    Rcpp::List spectraList(spectrumCount);
    Rcpp::CharacterVector spectraNames(spectrumCount);

    for (int specIdx = 0; specIdx < spectrumCount; specIdx++) {
        float liveTime, voltage, current, realTime, deadTime, resetTime;

        if (specIdx == 0) {
            // Read from main header
            file.seekg(PDZ_SPEC0_REALTIME_POS);
            file.read(reinterpret_cast<char*>(&realTime), sizeof(float));

            file.seekg(PDZ_SPEC0_DEADTIME_POS);
            file.read(reinterpret_cast<char*>(&deadTime), sizeof(float));

            file.seekg(PDZ_SPEC0_RESETTIME_POS);
            file.read(reinterpret_cast<char*>(&resetTime), sizeof(float));

            file.seekg(PDZ_SPEC0_LIVETIME_POS);
            file.read(reinterpret_cast<char*>(&liveTime), sizeof(float));

            file.seekg(PDZ_SPEC0_VOLTAGE_POS);
            file.read(reinterpret_cast<char*>(&voltage), sizeof(float));

            file.seekg(PDZ_SPEC0_CURRENT_POS);
            file.read(reinterpret_cast<char*>(&current), sizeof(float));
        } else {
            // Read from inter-spectrum gap
            long blockStart = PDZ_HEADER_SIZE + PDZ_SPECTRUM_SIZE +
                              (specIdx - 1) * (PDZ_SPECTRUM_SIZE + PDZ_INTER_SPECTRUM_GAP);

            file.seekg(blockStart + PDZ_GAP_REALTIME_OFFSET);
            file.read(reinterpret_cast<char*>(&realTime), sizeof(float));

            file.seekg(blockStart + PDZ_GAP_DEADTIME_OFFSET);
            file.read(reinterpret_cast<char*>(&deadTime), sizeof(float));

            file.seekg(blockStart + PDZ_GAP_RESETTIME_OFFSET);
            file.read(reinterpret_cast<char*>(&resetTime), sizeof(float));

            file.seekg(blockStart + PDZ_GAP_LIVETIME_OFFSET);
            file.read(reinterpret_cast<char*>(&liveTime), sizeof(float));

            file.seekg(blockStart + PDZ_GAP_VOLTAGE_OFFSET);
            file.read(reinterpret_cast<char*>(&voltage), sizeof(float));

            file.seekg(blockStart + PDZ_GAP_CURRENT_OFFSET);
            file.read(reinterpret_cast<char*>(&current), sizeof(float));
        }

        // Calculate dead time percentage: DeadTime / (LiveTime + DeadTime) * 100
        // This represents the fraction of time the detector was unable to register counts
        float deadTimePct = 0.0f;
        if ((liveTime + deadTime) > 0) {
            deadTimePct = (deadTime / (liveTime + deadTime)) * 100.0f;
        }

        // Create spectrum metadata list
        Rcpp::List specMeta = Rcpp::List::create(
            Rcpp::Named("spectrum_index") = specIdx,
            Rcpp::Named("tube_voltage_kV") = voltage,
            Rcpp::Named("tube_current_uA") = current,
            Rcpp::Named("real_time_s") = realTime,
            Rcpp::Named("dead_time_s") = deadTime,
            Rcpp::Named("reset_time_s") = resetTime,
            Rcpp::Named("live_time_s") = liveTime,
            Rcpp::Named("dead_time_pct") = deadTimePct,
            Rcpp::Named("eVCh") = eVCh
        );

        // Add filter info only for spectrum 0 (from main header)
        if (specIdx == 0) {
            specMeta["filter1_element"] = static_cast<int>(filter1Element);
            specMeta["filter1_thickness_um"] = static_cast<int>(filter1Thickness);
            specMeta["filter2_element"] = static_cast<int>(filter2Element);
            specMeta["filter2_thickness_um"] = static_cast<int>(filter2Thickness);
            specMeta["filter3_element"] = static_cast<int>(filter3Element);
            specMeta["filter3_thickness_um"] = static_cast<int>(filter3Thickness);
        }

        spectraList[specIdx] = specMeta;
        spectraNames[specIdx] = "spectrum_" + std::to_string(specIdx + 1);
    }
    spectraList.attr("names") = spectraNames;

    file.close();

    // Build final result
    // Note: Tube target element is NOT stored in simple_v2 format.
    // The target material (e.g., Rh=45) is not encoded in the file header.
    return Rcpp::List::create(
        Rcpp::Named("file_size") = fileSize,
        Rcpp::Named("format_version") = static_cast<int>(formatVersion),
        Rcpp::Named("format_type") = formatType,
        Rcpp::Named("is_v25") = isV25,
        Rcpp::Named("spectrum_count") = static_cast<int>(spectrumCount),
        Rcpp::Named("serial_number") = serialNumber,
        Rcpp::Named("acquisition_datetime") = acquisitionDatetime,
        Rcpp::Named("eVCh") = eVCh,
        Rcpp::Named("cal_value1") = calValue1,
        Rcpp::Named("cal_value2") = calValue2,
        Rcpp::Named("header_size") = PDZ_HEADER_SIZE,
        Rcpp::Named("channels_per_spectrum") = PDZ_SPECTRUM_CHANNELS,
        Rcpp::Named("spectra") = spectraList
    );
}

// [[Rcpp::export]]
Rcpp::List readAllPDZSpectra(const std::string& fileName) {
    // Convenience function to read all spectra from a file
    // Auto-detects format (v25 record-based vs simple format)

    // Check if this is a v25 record-based file
    if (isPDZv25Format(fileName)) {
        // v25 format - use record-based reader
        // Note: v25 typically has one spectrum per record type 3/4
        Rcpp::List result(1);
        Rcpp::CharacterVector names(1);
        result[0] = readPDZ25(fileName);
        names[0] = "spectrum_1";
        result.attr("names") = names;
        return result;
    }

    // Simple format (v2) - use direct offset reading
    int count = getPDZSpectrumCount(fileName);

    Rcpp::List result(count);
    Rcpp::CharacterVector names(count);

    for (int i = 0; i < count; i++) {
        result[i] = readPDZSpectrum(fileName, i);
        names[i] = "spectrum_" + std::to_string(i + 1);
    }

    result.attr("names") = names;
    return result;
}

// [[Rcpp::export]]
Rcpp::List getPDZDataFrames(const std::string& fileName) {
    // Universal function to read PDZ file and return ready-to-use data frames
    //
    // For each spectrum in the file:
    //   - Reads raw counts
    //   - Divides by LiveTime to get CPS (counts per second)
    //   - Calculates Energy using eVCh calibration
    //   - Returns DataFrame with Energy and CPS columns
    //
    // Always returns a list of data frames (even for single spectrum)
    // Access: result[[1]] for first spectrum, result[[2]] for second, etc.
    //
    // R usage:
    //   spectra <- getPDZDataFrames("file.pdz")
    //   plot(spectra[[1]]$Energy, spectra[[1]]$CPS)  # plot first spectrum
    //   length(spectra)  # number of spectra in file

    // Check if this is a v25 record-based file
    if (isPDZv25Format(fileName)) {
        // v25 format - use record-based reader with v25 metadata functions
        std::vector<float> spectrum = readPDZ25(fileName);
        float liveTime = readPDZ25LiveTime(fileName);
        float eVCh = readPDZ25eVCH(fileName);

        // Create Energy and CPS vectors
        Rcpp::NumericVector energy(PDZ_SPECTRUM_CHANNELS);
        Rcpp::NumericVector cps(PDZ_SPECTRUM_CHANNELS);

        for (int i = 0; i < PDZ_SPECTRUM_CHANNELS; i++) {
            energy[i] = (i + 1) * (eVCh / 1000.0);  // Channel 1-2048, eVCh in eV -> keV
            cps[i] = spectrum[i] / liveTime;
        }

        // Create DataFrame
        Rcpp::DataFrame df = Rcpp::DataFrame::create(
            Rcpp::Named("Energy") = energy,
            Rcpp::Named("CPS") = cps
        );

        // Return as list with one element
        Rcpp::List result(1);
        Rcpp::CharacterVector names(1);
        result[0] = df;
        names[0] = "spectrum_1";
        result.attr("names") = names;
        return result;
    }

    // Simple format (v2) - use direct offset reading
    int count = getPDZSpectrumCount(fileName);

    Rcpp::List result(count);
    Rcpp::CharacterVector names(count);

    for (int specIdx = 0; specIdx < count; specIdx++) {
        // Read spectrum data
        std::vector<float> spectrum = readPDZSpectrum(fileName, specIdx);

        // Get metadata for this spectrum
        float liveTime = getPDZLiveTime(fileName, specIdx);
        double eVCh = getPDZeVCh(fileName, specIdx);

        // Create Energy and CPS vectors
        Rcpp::NumericVector energy(PDZ_SPECTRUM_CHANNELS);
        Rcpp::NumericVector cps(PDZ_SPECTRUM_CHANNELS);

        for (int i = 0; i < PDZ_SPECTRUM_CHANNELS; i++) {
            energy[i] = (i + 1) * (eVCh / 1000.0);  // Channel 1-2048, eVCh in eV -> keV
            cps[i] = spectrum[i] / liveTime;
        }

        // Create DataFrame for this spectrum
        Rcpp::DataFrame df = Rcpp::DataFrame::create(
            Rcpp::Named("Energy") = energy,
            Rcpp::Named("CPS") = cps
        );

        result[specIdx] = df;
        names[specIdx] = "spectrum_" + std::to_string(specIdx + 1);
    }

    result.attr("names") = names;
    return result;
}

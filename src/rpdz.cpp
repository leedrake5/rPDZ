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
#include <cstring>
#include <algorithm>

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

// readPDZ25 definition moved below the v25 record-walking helpers.

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

// ---------------------------------------------------------------------------
// v25 record-walking helpers (shared by the high-level readers below).
//
// A v25 file starts with a "File Header" record of type 25 whose body carries
// the UTF-16LE string "pdz25" + an InstrumentType int. Records then follow in
// the form: short recordID, uint32 recordLength, recordLength bytes of body.
//
// Record type 3 (XRF Spectrum) has a fixed 110-byte prefix, then a 4-byte
// IlluminationLength (count of UTF-16 chars), then the Illumination string
// (2 * IlluminationLength bytes), then a 2-byte NormalPacketStart, then the
// spectrum counts (uint32 per channel, NumChannels channels, which lives at
// body offset 104 as a short).
//
// The old readPDZ25() treated body[114] as an int offset-to-spectrum with a
// hardcoded +120/+164 adjustment. That formula only accidentally worked when
// IlluminationLength == 0 (and even then was off by several bytes). Files
// with a non-empty Illumination string (e.g. 03106-Mudrock Air.pdz, which
// also holds two type 3 records) got garbage. These helpers replace that
// logic with a proper walk of the record structure.
// ---------------------------------------------------------------------------

static bool v25_readRecordHeader(std::ifstream& file, long pos,
                                 short& recordID, int& recordLength) {
    file.clear();
    file.seekg(pos);
    if (!file) return false;
    file.read(reinterpret_cast<char*>(&recordID), sizeof(short));
    file.read(reinterpret_cast<char*>(&recordLength), sizeof(int));
    return file.good();
}

// Count type 3 (XRF Spectrum) records in a v25 file. Caller should have
// already confirmed the file is v25 via isPDZv25Format().
static int v25_countType3Records(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) return 0;

    file.seekg(0, std::ios::end);
    long fileSize = file.tellg();

    short rid;
    int rlen;
    if (!v25_readRecordHeader(file, 0, rid, rlen)) return 0;
    if (rid != 25) return 0;

    long pos = 6 + rlen;
    int count = 0;
    while (pos + 6 <= fileSize) {
        if (!v25_readRecordHeader(file, pos, rid, rlen)) break;
        if (rlen < 0 || pos + 6 + (long)rlen > fileSize) break;
        if (rid == 3) count++;
        pos += 6 + rlen;
    }
    return count;
}

// Locate the Nth (0-based) type 3 record. Writes body start offset and body
// length into the out-parameters. Returns false if not found.
static bool v25_locateType3Record(const std::string& fileName, int spectrumIndex,
                                  long& bodyStart, int& bodyLen) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) return false;

    file.seekg(0, std::ios::end);
    long fileSize = file.tellg();

    short rid;
    int rlen;
    if (!v25_readRecordHeader(file, 0, rid, rlen)) return false;
    if (rid != 25) return false;

    long pos = 6 + rlen;
    int seen = 0;
    while (pos + 6 <= fileSize) {
        if (!v25_readRecordHeader(file, pos, rid, rlen)) return false;
        if (rlen < 0 || pos + 6 + (long)rlen > fileSize) return false;
        if (rid == 3) {
            if (seen == spectrumIndex) {
                bodyStart = pos + 6;
                bodyLen = rlen;
                return true;
            }
            seen++;
        }
        pos += 6 + rlen;
    }
    return false;
}

// Compute the byte offset within a type 3 record body where the spectrum
// counts begin. Returns -1 on malformed input.
//
// Body layout:
//   0..109    : 110 bytes of fixed-size scalar fields
//   110       : uint32   IlluminationLength (chars)
//   114       : wchar_t  Illumination[IlluminationLength]  (2 bytes each)
//   114+2L    : short    NormalPacketStart
//   116+2L    : uint32   SpectrumData[NumChannels]
static long v25_computeSpectrumOffset(const char* body, int bodyLen) {
    if (bodyLen < 114) return -1;
    int illLen = *reinterpret_cast<const int*>(body + 110);
    if (illLen < 0 || illLen > 65535) return -1;
    long off = 110L + 4L + 2L * illLen + 2L;
    if (off > bodyLen) return -1;
    return off;
}

// Read NumChannels (short at body offset 104), defaulting to 2048 if the
// value looks implausible.
static int v25_getNumChannels(const char* body, int bodyLen) {
    if (bodyLen < 106) return PDZ_SPECTRUM_CHANNELS;
    short nc = *reinterpret_cast<const short*>(body + 104);
    if (nc <= 0 || nc > 16384) return PDZ_SPECTRUM_CHANNELS;
    return static_cast<int>(nc);
}

// Load a type 3 record's body bytes into `out`. Returns false on failure.
static bool v25_readType3Body(const std::string& fileName, int spectrumIndex,
                              std::vector<char>& out) {
    long bodyStart;
    int bodyLen;
    if (!v25_locateType3Record(fileName, spectrumIndex, bodyStart, bodyLen))
        return false;

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) return false;
    out.resize(bodyLen);
    file.seekg(bodyStart);
    file.read(out.data(), bodyLen);
    return file.good() || (file.eof() && file.gcount() == bodyLen);
}

// Extract the spectrum counts from a type 3 body into a fixed-size
// PDZ_SPECTRUM_CHANNELS float buffer, zero-padding extra channels and
// clipping if NumChannels > PDZ_SPECTRUM_CHANNELS.
static std::vector<float> v25_extractSpectrum(const std::vector<char>& body) {
    std::vector<float> spectrum(PDZ_SPECTRUM_CHANNELS, 0.0f);
    long spOff = v25_computeSpectrumOffset(body.data(), (int)body.size());
    if (spOff < 0) return spectrum;
    int numCh = v25_getNumChannels(body.data(), (int)body.size());
    int toRead = std::min(numCh, PDZ_SPECTRUM_CHANNELS);
    long available = (long)body.size() - spOff;
    int fit = (int)std::min<long>(toRead, available / (long)sizeof(uint32_t));
    for (int i = 0; i < fit; i++) {
        uint32_t v = *reinterpret_cast<const uint32_t*>(
            body.data() + spOff + (long)i * sizeof(uint32_t));
        spectrum[i] = static_cast<float>(v);
    }
    return spectrum;
}

// [[Rcpp::export]]
std::vector<float> readPDZ25(std::string fileName) {
    // Returns the first type 3 (XRF Spectrum) record's counts as 2048 floats.
    // For multi-phase v25 files, use getPDZDataFrames() / readPDZSpectrum() /
    // readAllPDZSpectra() to access additional phases.
    std::vector<char> body;
    if (!v25_readType3Body(fileName, 0, body)) {
        Rcpp::stop("Could not locate XRF Spectrum (type 3) record in v25 file");
    }
    return v25_extractSpectrum(body);
}

// [[Rcpp::export]]
int getPDZSpectrumCount(const std::string& fileName) {
    // Returns number of spectra in file.
    // For v25 record-based files, counts XRF Spectrum (type 3) records.
    // For the simple format, reads the count byte at offset 1.
    int v25Count = v25_countType3Records(fileName);
    if (v25Count > 0) return v25Count;

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    file.seekg(1);  // Spectrum count is at byte offset 1
    unsigned char count;
    file.read(reinterpret_cast<char*>(&count), 1);
    file.close();

    if (count < 1 || count > 10) {
        // If value seems unreasonable, might be different format.
        // Warn once, then return 1 so callers still get a best-effort read
        // instead of an empty list that blows up downstream consumers.
        Rcpp::warning("Unexpected spectrum count value: %d - file may be different format; treating as 1", count);
        return 1;
    }

    return static_cast<int>(count);
}

// [[Rcpp::export]]
int getPDZFormatVersion(const std::string& fileName) {
    // Returns format version byte (byte 0)
    // Known values: 0x01 and 0x02 for simple format (identical layout)
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
    // Works with both single and multi-spectrum files, v25 record-based files,
    // and the simple format.

    // v25 branch: locate the Nth type 3 record and extract its spectrum.
    if (isPDZv25Format(fileName)) {
        std::vector<char> body;
        if (!v25_readType3Body(fileName, spectrumIndex, body)) {
            int total = v25_countType3Records(fileName);
            Rcpp::stop("Spectrum index %d out of range (v25 file has %d spectra)",
                       spectrumIndex, total);
        }
        return v25_extractSpectrum(body);
    }

    float spectrum[PDZ_SPECTRUM_CHANNELS] = {0.0f};

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Get spectrum count to validate index. Some files have a bogus 0 in
    // this byte; treat 0 as 1 so a single best-effort read is still served.
    file.seekg(1);
    unsigned char count;
    file.read(reinterpret_cast<char*>(&count), 1);
    int effective = (count < 1) ? 1 : static_cast<int>(count);

    if (spectrumIndex < 0 || spectrumIndex >= effective) {
        file.close();
        Rcpp::stop("Spectrum index %d out of range (file has %d spectra)", spectrumIndex, effective);
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
    // Format versions 1 and 2 share identical binary layout (358-byte header,
    // 2048 channels per spectrum, same metadata offsets). Version byte varies
    // by instrument firmware but the structure is the same.
    std::string formatType;
    if (isV25) {
        formatType = "v25_record";
    } else if (formatVersion == 1 || formatVersion == 2) {
        formatType = "simple";
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

    // v25 record-based files have a completely different header layout from
    // the simple format; reading simple-format offsets would return garbage
    // (e.g. the pdz25 UTF-16 marker reinterpreted as a double for eVCh).
    // Build the metadata list from the type 3 records instead.
    if (isPDZv25Format(fileName)) {
        file.close();

        int count = v25_countType3Records(fileName);
        Rcpp::List spectraList(count);
        Rcpp::CharacterVector spectraNames(count);

        double sharedEVCh = 20.0;
        std::string acquisitionDatetime = "";

        for (int specIdx = 0; specIdx < count; specIdx++) {
            std::vector<char> body;
            if (!v25_readType3Body(fileName, specIdx, body)) continue;
            const char* b = body.data();
            int blen = (int)body.size();

            auto readF = [&](int off, float def = 0.0f) -> float {
                if (off < 0 || off + 4 > blen) return def;
                return *reinterpret_cast<const float*>(b + off);
            };
            auto readS = [&](int off, short def = 0) -> short {
                if (off < 0 || off + 2 > blen) return def;
                return *reinterpret_cast<const short*>(b + off);
            };
            auto readI = [&](int off, int def = 0) -> int {
                if (off < 0 || off + 4 > blen) return def;
                return *reinterpret_cast<const int*>(b + off);
            };

            // Per v25 XRF Spectrum record layout (offsets into body).
            float totalRealTime = readF(20);
            float totalPacket   = readF(24);
            float totalDead     = readF(28);
            float totalReset    = readF(32);
            float totalLive     = readF(36);
            float tubeVoltage   = readF(40);
            float tubeCurrent   = readF(44);
            short f1e = readS(48), f1t = readS(50);
            short f2e = readS(52), f2t = readS(54);
            short f3e = readS(56), f3t = readS(58);
            float evPerChan     = readF(74);
            int vacuum          = readI(70);

            if (specIdx == 0) {
                sharedEVCh = evPerChan;
                // SYSTEMTIME at body offset 84, 16 bytes
                if (blen >= 84 + 16) {
                    uint16_t t[8];
                    std::memcpy(t, b + 84, 16);
                    char buf[32];
                    snprintf(buf, sizeof(buf),
                             "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                             t[0], t[1], t[3], t[4], t[5], t[6], t[7]);
                    acquisitionDatetime = buf;
                }
            }

            float deadPct = 0.0f;
            if (totalLive + totalDead > 0)
                deadPct = (totalDead / (totalLive + totalDead)) * 100.0f;

            Rcpp::List specMeta = Rcpp::List::create(
                Rcpp::Named("spectrum_index") = specIdx,
                Rcpp::Named("tube_voltage_kV") = tubeVoltage,
                Rcpp::Named("tube_current_uA") = tubeCurrent,
                Rcpp::Named("real_time_s") = totalRealTime,
                Rcpp::Named("packet_time_s") = totalPacket,
                Rcpp::Named("dead_time_s") = totalDead,
                Rcpp::Named("reset_time_s") = totalReset,
                Rcpp::Named("live_time_s") = totalLive,
                Rcpp::Named("dead_time_pct") = deadPct,
                Rcpp::Named("eVCh") = evPerChan,
                Rcpp::Named("vacuum") = vacuum
            );
            specMeta["filter1_element"] = static_cast<int>(f1e);
            specMeta["filter1_thickness_um"] = static_cast<int>(f1t);
            specMeta["filter2_element"] = static_cast<int>(f2e);
            specMeta["filter2_thickness_um"] = static_cast<int>(f2t);
            specMeta["filter3_element"] = static_cast<int>(f3e);
            specMeta["filter3_thickness_um"] = static_cast<int>(f3t);

            spectraList[specIdx] = specMeta;
            spectraNames[specIdx] = "spectrum_" + std::to_string(specIdx + 1);
        }
        spectraList.attr("names") = spectraNames;

        return Rcpp::List::create(
            Rcpp::Named("file_size") = fileSize,
            Rcpp::Named("format_version") = 25,
            Rcpp::Named("format_type") = std::string("v25_record"),
            Rcpp::Named("is_v25") = true,
            Rcpp::Named("spectrum_count") = count,
            Rcpp::Named("serial_number") = std::string(""),
            Rcpp::Named("acquisition_datetime") = acquisitionDatetime,
            Rcpp::Named("eVCh") = sharedEVCh,
            Rcpp::Named("channels_per_spectrum") = PDZ_SPECTRUM_CHANNELS,
            Rcpp::Named("spectra") = spectraList
        );
    }

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
    // Format versions 1 and 2 share identical binary layout
    std::string formatType;
    if (isV25) {
        formatType = "v25_record";
    } else if (formatVersion == 1 || formatVersion == 2) {
        formatType = "simple";
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

    // Works for both v25 (multi-phase supported) and simple format —
    // readPDZSpectrum() dispatches on isPDZv25Format() internally and
    // getPDZSpectrumCount() counts type 3 records for v25 files.
    int count = getPDZSpectrumCount(fileName);
    if (count < 1) count = 1;

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
        int count = v25_countType3Records(fileName);
        if (count < 1) count = 1;

        Rcpp::List result(count);
        Rcpp::CharacterVector names(count);

        for (int specIdx = 0; specIdx < count; specIdx++) {
            // Load the Nth type 3 record body once and pull spectrum +
            // eVCh + LiveTime from it, so dual-phase v25 files return
            // per-phase calibration and timing (e.g. 03106-Mudrock Air.pdz).
            std::vector<char> body;
            if (!v25_readType3Body(fileName, specIdx, body)) {
                Rcpp::stop("Failed to read v25 spectrum record %d", specIdx);
            }

            std::vector<float> spectrum = v25_extractSpectrum(body);
            float eVCh = (body.size() >= 78)
                ? *reinterpret_cast<float*>(body.data() + 74) : 20.0f;
            float liveTime = (body.size() >= 40)
                ? *reinterpret_cast<float*>(body.data() + 36) : 1.0f;
            if (liveTime <= 0.0f) liveTime = 1.0f;

            Rcpp::NumericVector energy(PDZ_SPECTRUM_CHANNELS);
            Rcpp::NumericVector cps(PDZ_SPECTRUM_CHANNELS);
            for (int i = 0; i < PDZ_SPECTRUM_CHANNELS; i++) {
                energy[i] = (i + 1) * (eVCh / 1000.0);
                cps[i] = spectrum[i] / liveTime;
            }

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

    // Simple format (v2) - use direct offset reading
    int count = getPDZSpectrumCount(fileName);
    if (count < 1) count = 1;  // defensive: never hand back an empty list

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

//
//  rpdz.cpp
//  
//
//  Created by Lee Drake on 5/16/18.
//  Updated by Lee Drake with help from Frits Vossman on 10/05/2023

//
// [[Rcpp::depends(Rcpp)]]
#include <Rcpp.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <cstring>
#include <locale>
#include <codecvt>

using namespace Rcpp;
// Function to convert UTF-16 string to UTF-8
std::string utf16_to_utf8(const std::u16string& u16str) {
    // Note: std::wstring_convert is deprecated in C++17 and may be removed in future versions.
    // This code works for now, but consider using other libraries for future-proofing.
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(u16str);
}

// Functions to read data in little-endian format
uint16_t readLEUInt16(std::istream& stream) {
    uint8_t bytes[2];
    stream.read(reinterpret_cast<char*>(bytes), 2);
    if (!stream) {
        throw std::runtime_error("Failed to read uint16_t");
    }
    return (uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8);
}

uint32_t readLEUInt32(std::istream& stream) {
    uint8_t bytes[4];
    stream.read(reinterpret_cast<char*>(bytes), 4);
    if (!stream) {
        throw std::runtime_error("Failed to read uint32_t");
    }
    return (uint32_t)bytes[0] |
           ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
}

int16_t readLEInt16(std::istream& stream) {
    return (int16_t)readLEUInt16(stream);
}

int32_t readLEInt32(std::istream& stream) {
    return (int32_t)readLEUInt32(stream);
}

uint8_t readUInt8(std::istream& stream) {
    uint8_t value;
    stream.read(reinterpret_cast<char*>(&value), 1);
    if (!stream) {
        throw std::runtime_error("Failed to read uint8_t");
    }
    return value;
}

float readLEFloat(std::istream& stream) {
    uint32_t temp = readLEUInt32(stream);
    float value;
    std::memcpy(&value, &temp, sizeof(float));
    return value;
}

std::string readLEWString(std::istream& stream, uint32_t length) {
    if (length == 0) {
        return "";
    }
    size_t numChars = length / 2;
    std::u16string u16str(numChars, '\0');
    for (size_t i = 0; i < numChars; ++i) {
        uint16_t ch = readLEUInt16(stream);
        u16str[i] = ch;
    }
    // Convert UTF-16 to UTF-8
    std::string utf8str = utf16_to_utf8(u16str);
    return utf8str;
}

// [[Rcpp::export]]
Rcpp::List readPDZ25(std::string fileName) {
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Function to read data from the file
    auto readBytes = [&](char* buffer, size_t size) {
        file.read(buffer, size);
        if (!file) {
            Rcpp::stop("Failed to read from file");
        }
    };

    // Main data structure to store records
    Rcpp::List records;

    // Read the initial 20 bytes (file header)
    char initialHeader[20];
    readBytes(initialHeader, 20);

    // Main loop to read records
    while (file.peek() != EOF) {
        uint16_t RecordType = 0;
        uint32_t DataLength = 0;
        try {
            // Read RecordType (uint16_t) and DataLength (uint32_t)
            RecordType = readLEUInt16(file);
            DataLength = readLEUInt32(file);

            // Debugging: Print RecordType and DataLength
            Rcpp::Rcout << "Reading RecordType: " << RecordType << ", DataLength: " << DataLength << std::endl;

            // Read the entire record data into a buffer
            std::vector<char> recordData(DataLength);
            readBytes(recordData.data(), DataLength);

            // Create a memory stream from the buffer
            std::istringstream recordStream(std::string(recordData.begin(), recordData.end()), std::ios::binary);

            // Create a list to store fields for this record
            Rcpp::List recordFields;

            if (RecordType == 1) {
                // RecordType 1 Parsing
                uint32_t SerialNumberLength = 0;
                std::string SerialNumber;
                uint32_t BuildNumberLength = 0;
                std::string BuildNumber;
                uint8_t TubeTargetElement = 0;
                uint8_t AnodeTakeoffAngle = 0;
                uint8_t SampleIncidenceAngle = 0;
                uint8_t SampleTakeoffAngle = 0;
                int16_t BeThickness = 0;
                uint32_t DetectorTypeLength = 0;
                std::string DetectorType;
                uint32_t TubeTypeLength = 0;
                std::string TubeType;
                uint8_t HW_SpotSize = 0;
                uint8_t SW_SpotSize = 0;
                uint32_t CollimatorTypeLength = 0;
                std::string CollimatorType;
                uint32_t NumVersions = 0;
                Rcpp::List FirmwareVersions;

                try {
                    SerialNumberLength = readLEUInt32(recordStream);
                    Rcpp::Rcout << "SerialNumberLength: " << SerialNumberLength << std::endl;
                    SerialNumber = readLEWString(recordStream, SerialNumberLength);
                    Rcpp::Rcout << "SerialNumber: " << SerialNumber << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read SerialNumber: " << ex.what() << std::endl;
                }

                try {
                    BuildNumberLength = readLEUInt32(recordStream);
                    Rcpp::Rcout << "BuildNumberLength: " << BuildNumberLength << std::endl;
                    BuildNumber = readLEWString(recordStream, BuildNumberLength);
                    Rcpp::Rcout << "BuildNumber: " << BuildNumber << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read BuildNumber: " << ex.what() << std::endl;
                }

                try {
                    TubeTargetElement = readUInt8(recordStream);
                    Rcpp::Rcout << "TubeTargetElement: " << static_cast<int>(TubeTargetElement) << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TubeTargetElement: " << ex.what() << std::endl;
                }

                try {
                    AnodeTakeoffAngle = readUInt8(recordStream);
                    Rcpp::Rcout << "AnodeTakeoffAngle: " << static_cast<int>(AnodeTakeoffAngle) << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read AnodeTakeoffAngle: " << ex.what() << std::endl;
                }

                try {
                    SampleIncidenceAngle = readUInt8(recordStream);
                    Rcpp::Rcout << "SampleIncidenceAngle: " << static_cast<int>(SampleIncidenceAngle) << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read SampleIncidenceAngle: " << ex.what() << std::endl;
                }

                try {
                    SampleTakeoffAngle = readUInt8(recordStream);
                    Rcpp::Rcout << "SampleTakeoffAngle: " << static_cast<int>(SampleTakeoffAngle) << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read SampleTakeoffAngle: " << ex.what() << std::endl;
                }

                try {
                    BeThickness = readLEInt16(recordStream);
                    Rcpp::Rcout << "BeThickness: " << BeThickness << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read BeThickness: " << ex.what() << std::endl;
                }

                try {
                    DetectorTypeLength = readLEUInt32(recordStream);
                    Rcpp::Rcout << "DetectorTypeLength: " << DetectorTypeLength << std::endl;
                    DetectorType = readLEWString(recordStream, DetectorTypeLength);
                    Rcpp::Rcout << "DetectorType: " << DetectorType << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read DetectorType: " << ex.what() << std::endl;
                }

                try {
                    TubeTypeLength = readLEUInt32(recordStream);
                    Rcpp::Rcout << "TubeTypeLength: " << TubeTypeLength << std::endl;
                    TubeType = readLEWString(recordStream, TubeTypeLength);
                    Rcpp::Rcout << "TubeType: " << TubeType << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TubeType: " << ex.what() << std::endl;
                }

                try {
                    HW_SpotSize = readUInt8(recordStream);
                    Rcpp::Rcout << "HW_SpotSize: " << static_cast<int>(HW_SpotSize) << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read HW_SpotSize: " << ex.what() << std::endl;
                }

                try {
                    SW_SpotSize = readUInt8(recordStream);
                    Rcpp::Rcout << "SW_SpotSize: " << static_cast<int>(SW_SpotSize) << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read SW_SpotSize: " << ex.what() << std::endl;
                }

                try {
                    CollimatorTypeLength = readLEUInt32(recordStream);
                    Rcpp::Rcout << "CollimatorTypeLength: " << CollimatorTypeLength << std::endl;
                    CollimatorType = readLEWString(recordStream, CollimatorTypeLength);
                    Rcpp::Rcout << "CollimatorType: " << CollimatorType << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read CollimatorType: " << ex.what() << std::endl;
                }

                try {
                    NumVersions = readLEUInt32(recordStream);
                    Rcpp::Rcout << "NumVersions: " << NumVersions << std::endl;
                    FirmwareVersions = Rcpp::List(NumVersions);
                    for (uint32_t i = 0; i < NumVersions; ++i) {
                        uint16_t RecordNumber = 0;
                        uint32_t VersionLength = 0;
                        std::string Version;
                        try {
                            RecordNumber = readLEUInt16(recordStream);
                            Rcpp::Rcout << "Firmware RecordNumber: " << RecordNumber << std::endl;
                            VersionLength = readLEUInt32(recordStream);
                            Rcpp::Rcout << "Firmware VersionLength: " << VersionLength << std::endl;
                            Version = readLEWString(recordStream, VersionLength);
                            Rcpp::Rcout << "Firmware Version: " << Version << std::endl;
                            FirmwareVersions[i] = Rcpp::List::create(
                                Rcpp::Named("RecordNumber") = RecordNumber,
                                Rcpp::Named("Version") = Version
                            );
                        } catch (std::exception& ex) {
                            Rcpp::Rcerr << "Failed to read Firmware Version " << i << ": " << ex.what() << std::endl;
                        }
                    }
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read FirmwareVersions: " << ex.what() << std::endl;
                }

                // Store fields in the list
                recordFields["RecordType"] = RecordType;
                recordFields["SerialNumber"] = SerialNumber;
                recordFields["BuildNumber"] = BuildNumber;
                recordFields["TubeTargetElement"] = TubeTargetElement;
                recordFields["AnodeTakeoffAngle"] = AnodeTakeoffAngle;
                recordFields["SampleIncidenceAngle"] = SampleIncidenceAngle;
                recordFields["SampleTakeoffAngle"] = SampleTakeoffAngle;
                recordFields["BeThickness"] = BeThickness;
                recordFields["DetectorType"] = DetectorType;
                recordFields["TubeType"] = TubeType;
                recordFields["HW_SpotSize"] = HW_SpotSize;
                recordFields["SW_SpotSize"] = SW_SpotSize;
                recordFields["CollimatorType"] = CollimatorType;
                recordFields["FirmwareVersions"] = FirmwareVersions;

            } else if (RecordType == 2) {
                // RecordType 2 Parsing
                uint32_t NumberOfPhases = 0;
                uint32_t RawCounts = 0;
                uint32_t ValidCounts = 0;
                uint32_t ValidCountsInRange = 0;
                uint32_t ResetCounts = 0;
                float TotalRealTime = 0.0;
                float TotalPacketTime = 0.0;
                float TotalDead = 0.0;
                float TotalReset = 0.0;
                float TotalLive = 0.0;
                float ElapsedTime = 0.0;
                uint32_t AppNameLength = 0;
                std::string ApplicationName;
                uint32_t AppPNLength = 0;
                std::string ApplicationPartNumber;

                try {
                    NumberOfPhases = readLEUInt32(recordStream);
                    Rcpp::Rcout << "NumberOfPhases: " << NumberOfPhases << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read NumberOfPhases: " << ex.what() << std::endl;
                }

                try {
                    RawCounts = readLEUInt32(recordStream);
                    Rcpp::Rcout << "RawCounts: " << RawCounts << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read RawCounts: " << ex.what() << std::endl;
                }

                try {
                    ValidCounts = readLEUInt32(recordStream);
                    Rcpp::Rcout << "ValidCounts: " << ValidCounts << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ValidCounts: " << ex.what() << std::endl;
                }

                try {
                    ValidCountsInRange = readLEUInt32(recordStream);
                    Rcpp::Rcout << "ValidCountsInRange: " << ValidCountsInRange << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ValidCountsInRange: " << ex.what() << std::endl;
                }

                try {
                    ResetCounts = readLEUInt32(recordStream);
                    Rcpp::Rcout << "ResetCounts: " << ResetCounts << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ResetCounts: " << ex.what() << std::endl;
                }

                try {
                    TotalRealTime = readLEFloat(recordStream);
                    Rcpp::Rcout << "TotalRealTime: " << TotalRealTime << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TotalRealTime: " << ex.what() << std::endl;
                }

                try {
                    TotalPacketTime = readLEFloat(recordStream);
                    Rcpp::Rcout << "TotalPacketTime: " << TotalPacketTime << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TotalPacketTime: " << ex.what() << std::endl;
                }

                try {
                    TotalDead = readLEFloat(recordStream);
                    Rcpp::Rcout << "TotalDead: " << TotalDead << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TotalDead: " << ex.what() << std::endl;
                }

                try {
                    TotalReset = readLEFloat(recordStream);
                    Rcpp::Rcout << "TotalReset: " << TotalReset << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TotalReset: " << ex.what() << std::endl;
                }

                try {
                    TotalLive = readLEFloat(recordStream);
                    Rcpp::Rcout << "TotalLive: " << TotalLive << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TotalLive: " << ex.what() << std::endl;
                }

                try {
                    ElapsedTime = readLEFloat(recordStream);
                    Rcpp::Rcout << "ElapsedTime: " << ElapsedTime << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ElapsedTime: " << ex.what() << std::endl;
                }

                try {
                    AppNameLength = readLEUInt32(recordStream);
                    Rcpp::Rcout << "ApplicationNameLength: " << AppNameLength << std::endl;
                    ApplicationName = readLEWString(recordStream, AppNameLength);
                    Rcpp::Rcout << "ApplicationName: " << ApplicationName << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ApplicationName: " << ex.what() << std::endl;
                }

                try {
                    AppPNLength = readLEUInt32(recordStream);
                    Rcpp::Rcout << "ApplicationPartNumberLength: " << AppPNLength << std::endl;
                    ApplicationPartNumber = readLEWString(recordStream, AppPNLength);
                    Rcpp::Rcout << "ApplicationPartNumber: " << ApplicationPartNumber << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ApplicationPartNumber: " << ex.what() << std::endl;
                }

                // Store fields in the list
                recordFields["RecordType"] = RecordType;
                recordFields["NumberOfPhases"] = NumberOfPhases;
                recordFields["RawCounts"] = RawCounts;
                recordFields["ValidCounts"] = ValidCounts;
                recordFields["ValidCountsInRange"] = ValidCountsInRange;
                recordFields["ResetCounts"] = ResetCounts;
                recordFields["TotalRealTime"] = TotalRealTime;
                recordFields["TotalPacketTime"] = TotalPacketTime;
                recordFields["TotalDead"] = TotalDead;
                recordFields["TotalReset"] = TotalReset;
                recordFields["TotalLive"] = TotalLive;
                recordFields["ElapsedTime"] = ElapsedTime;
                recordFields["ApplicationName"] = ApplicationName;
                recordFields["ApplicationPartNumber"] = ApplicationPartNumber;

            } else if (RecordType == 3) {
                // RecordType 3 Parsing
                uint32_t PhaseNumber = 0;
                uint32_t RawCounts = 0;
                uint32_t ValidCounts = 0;
                uint32_t ValidCountsInRange = 0;
                uint32_t ResetCounts = 0;
                float TimeSinceTrigger = 0.0;
                float TotalPacketTime = 0.0;
                float TotalDead = 0.0;
                float TotalReset = 0.0;
                float TotalLive = 0.0;
                float TubeVoltage = 0.0;
                float TubeCurrent = 0.0;
                int16_t Filter1Element = 0;
                int16_t Filter1Thickness = 0;
                int16_t Filter2Element = 0;
                int16_t Filter2Thickness = 0;
                int16_t Filter3Element = 0;
                int16_t Filter3Thickness = 0;
                float FilterWheelNumber = 0.0;
                float DetectorTemp = 0.0;
                int16_t AmbientTemp = 0;
                int32_t Vacuum = 0;
                float EVPerChannel = 0.0;
                int16_t GainDriftAlgorithm = 0;
                float ChannelStart = 0.0;
                // Skip SYSTEMTIME (16 bytes)
                float AtmosphericPressure = 0.0;
                int16_t NumChannels = 0;
                int16_t NoseTemp = 0;
                int16_t Environment = 0;
                int16_t IlluminationLength = 0;
                std::string Illumination;
                int16_t NormalPacketStart = 0;
                int16_t manualOffset = 24;
                std::vector<uint32_t> SpectrumData;

                try {
                    PhaseNumber = readLEUInt32(recordStream);
                    Rcpp::Rcout << "PhaseNumber: " << PhaseNumber << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read PhaseNumber: " << ex.what() << std::endl;
                }

                try {
                    RawCounts = readLEUInt32(recordStream);
                    Rcpp::Rcout << "RawCounts: " << RawCounts << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read RawCounts: " << ex.what() << std::endl;
                }

                try {
                    ValidCounts = readLEUInt32(recordStream);
                    Rcpp::Rcout << "ValidCounts: " << ValidCounts << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ValidCounts: " << ex.what() << std::endl;
                }

                try {
                    ValidCountsInRange = readLEUInt32(recordStream);
                    Rcpp::Rcout << "ValidCountsInRange: " << ValidCountsInRange << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ValidCountsInRange: " << ex.what() << std::endl;
                }

                try {
                    ResetCounts = readLEUInt32(recordStream);
                    Rcpp::Rcout << "ResetCounts: " << ResetCounts << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ResetCounts: " << ex.what() << std::endl;
                }

                try {
                    TimeSinceTrigger = readLEFloat(recordStream);
                    Rcpp::Rcout << "TimeSinceTrigger: " << TimeSinceTrigger << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TimeSinceTrigger: " << ex.what() << std::endl;
                }

                try {
                    TotalPacketTime = readLEFloat(recordStream);
                    Rcpp::Rcout << "TotalPacketTime: " << TotalPacketTime << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TotalPacketTime: " << ex.what() << std::endl;
                }

                try {
                    TotalDead = readLEFloat(recordStream);
                    Rcpp::Rcout << "TotalDead: " << TotalDead << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TotalDead: " << ex.what() << std::endl;
                }

                try {
                    TotalReset = readLEFloat(recordStream);
                    Rcpp::Rcout << "TotalReset: " << TotalReset << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TotalReset: " << ex.what() << std::endl;
                }

                try {
                    TotalLive = readLEFloat(recordStream);
                    Rcpp::Rcout << "TotalLive: " << TotalLive << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TotalLive: " << ex.what() << std::endl;
                }

                try {
                    TubeVoltage = readLEFloat(recordStream);
                    Rcpp::Rcout << "TubeVoltage: " << TubeVoltage << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TubeVoltage: " << ex.what() << std::endl;
                }

                try {
                    TubeCurrent = readLEFloat(recordStream);
                    Rcpp::Rcout << "TubeCurrent: " << TubeCurrent << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read TubeCurrent: " << ex.what() << std::endl;
                }

                try {
                    Filter1Element = readLEInt16(recordStream);
                    Rcpp::Rcout << "Filter1Element: " << Filter1Element << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read Filter1Element: " << ex.what() << std::endl;
                }

                try {
                    Filter1Thickness = readLEInt16(recordStream);
                    Rcpp::Rcout << "Filter1Thickness: " << Filter1Thickness << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read Filter1Thickness: " << ex.what() << std::endl;
                }

                try {
                    Filter2Element = readLEInt16(recordStream);
                    Rcpp::Rcout << "Filter2Element: " << Filter2Element << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read Filter2Element: " << ex.what() << std::endl;
                }

                try {
                    Filter2Thickness = readLEInt16(recordStream);
                    Rcpp::Rcout << "Filter2Thickness: " << Filter2Thickness << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read Filter2Thickness: " << ex.what() << std::endl;
                }

                try {
                    Filter3Element = readLEInt16(recordStream);
                    Rcpp::Rcout << "Filter3Element: " << Filter3Element << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read Filter3Element: " << ex.what() << std::endl;
                }

                try {
                    Filter3Thickness = readLEInt16(recordStream);
                    Rcpp::Rcout << "Filter3Thickness: " << Filter3Thickness << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read Filter3Thickness: " << ex.what() << std::endl;
                }

                try {
                    FilterWheelNumber = readLEInt16(recordStream);
                    Rcpp::Rcout << "FilterWheelNumber: " << FilterWheelNumber << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read FilterWheelNumber: " << ex.what() << std::endl;
                }

                try {
                    DetectorTemp = readLEFloat(recordStream);
                    Rcpp::Rcout << "DetectorTemp: " << DetectorTemp << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read DetectorTemp: " << ex.what() << std::endl;
                }
                
                try {
                    AmbientTemp = readLEFloat(recordStream);
                    Rcpp::Rcout << "AmbientTemp: " << AmbientTemp << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read AmbientTemp: " << ex.what() << std::endl;
                }

                try {
                    Vacuum = readLEInt32(recordStream);
                    Rcpp::Rcout << "Vacuum: " << Vacuum << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read Vacuum: " << ex.what() << std::endl;
                }

                try {
                    EVPerChannel = readLEFloat(recordStream);
                    Rcpp::Rcout << "EVPerChannel: " << EVPerChannel << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read EVPerChannel: " << ex.what() << std::endl;
                }

                try {
                    GainDriftAlgorithm = readLEInt16(recordStream);
                    Rcpp::Rcout << "GainDriftAlgorithm: " << GainDriftAlgorithm << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read GainDriftAlgorithm: " << ex.what() << std::endl;
                }

                try {
                    ChannelStart = readLEFloat(recordStream);
                    Rcpp::Rcout << "ChannelStart: " << ChannelStart << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read ChannelStart: " << ex.what() << std::endl;
                }

                // Skip SYSTEMTIME (16 bytes)
                recordStream.seekg(16, std::ios_base::cur);

                try {
                    AtmosphericPressure = readLEFloat(recordStream);
                    Rcpp::Rcout << "AtmosphericPressure: " << AtmosphericPressure << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read AtmosphericPressure: " << ex.what() << std::endl;
                }

                try {
                    NumChannels = readLEInt16(recordStream);
                    Rcpp::Rcout << "NumChannels: " << NumChannels << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read NumChannels: " << ex.what() << std::endl;
                }

                try {
                    NoseTemp = readLEFloat(recordStream);
                    Rcpp::Rcout << "NoseTemp: " << NoseTemp << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read NoseTemp: " << ex.what() << std::endl;
                }

                try {
                    Environment = readLEInt16(recordStream);
                    Rcpp::Rcout << "Environment: " << Environment << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read Environment: " << ex.what() << std::endl;
                }

                try {
                    IlluminationLength = readLEUInt16(recordStream);
                    Rcpp::Rcout << "IlluminationLength: " << IlluminationLength << std::endl;
                    Illumination = readLEWString(recordStream, IlluminationLength);
                    Rcpp::Rcout << "Illumination: " << Illumination << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read Illumination: " << ex.what() << std::endl;
                }

                try {
                    NormalPacketStart = readLEInt16(recordStream);
                    Rcpp::Rcout << "NormalPacketStart: " << NormalPacketStart << std::endl;
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read NormalPacketStart: " << ex.what() << std::endl;
                }

                // Read SpectrumData
                try {
                    Rcpp::Rcout << "Reading SpectrumData with NumChannels: " << NumChannels << std::endl;

                    // Initialize SpectrumData with zeros
                    SpectrumData.resize(NumChannels, 0);

                    // Calculate the number of counts to read
                    int countsToRead = NumChannels;

                    // Adjust the stream position based on manualOffset
                    // Calculate the byte offset
                    std::streamoff byteOffset = manualOffset * sizeof(uint32_t);

                    // Move the stream position
                    if (byteOffset != 0) {
                        if (byteOffset > 0) {
                            // Move forward
                            recordStream.seekg(byteOffset, std::ios_base::cur);
                        } else {
                            // Move backward
                            recordStream.seekg(byteOffset, std::ios_base::cur);
                            if (recordStream.tellg() < 0) {
                                Rcpp::Rcerr << "Cannot seek before the beginning of the stream." << std::endl;
                                recordStream.seekg(0, std::ios_base::beg);
                            }
                        }
                    }

                    // Read counts from the stream
                    for (int i = 0; i < countsToRead; ++i) {
                        if (recordStream.eof()) {
                            Rcpp::Rcerr << "End of stream reached while reading SpectrumData." << std::endl;
                            break;
                        }
                        SpectrumData[i] = readLEUInt32(recordStream);
                    }
                } catch (std::exception& ex) {
                    Rcpp::Rcerr << "Failed to read SpectrumData: " << ex.what() << std::endl;
                }


                // Store fields in the list
                recordFields["RecordType"] = RecordType;
                recordFields["PhaseNumber"] = PhaseNumber;
                recordFields["RawCounts"] = RawCounts;
                recordFields["ValidCounts"] = ValidCounts;
                recordFields["ValidCountsInRange"] = ValidCountsInRange;
                recordFields["ResetCounts"] = ResetCounts;
                recordFields["TimeSinceTrigger"] = TimeSinceTrigger;
                recordFields["TotalPacketTime"] = TotalPacketTime;
                recordFields["TotalDead"] = TotalDead;
                recordFields["TotalReset"] = TotalReset;
                recordFields["TotalLive"] = TotalLive;
                recordFields["TubeVoltage"] = TubeVoltage;
                recordFields["TubeCurrent"] = TubeCurrent;
                recordFields["Filter1Element"] = Filter1Element;
                recordFields["Filter1Thickness"] = Filter1Thickness;
                recordFields["Filter2Element"] = Filter2Element;
                recordFields["Filter2Thickness"] = Filter2Thickness;
                recordFields["Filter3Element"] = Filter3Element;
                recordFields["Filter3Thickness"] = Filter3Thickness;
                recordFields["FilterWheelNumber"] = FilterWheelNumber;
                recordFields["DetectorTemp"] = DetectorTemp;
                recordFields["AmbientTemp"] = AmbientTemp;
                recordFields["Vacuum"] = Vacuum;
                recordFields["EVPerChannel"] = EVPerChannel;
                recordFields["GainDriftAlgorithm"] = GainDriftAlgorithm;
                recordFields["ChannelStart"] = ChannelStart;
                recordFields["AtmosphericPressure"] = AtmosphericPressure;
                recordFields["NumChannels"] = NumChannels;
                recordFields["NoseTemp"] = NoseTemp;
                recordFields["Environment"] = Environment;
                recordFields["Illumination"] = Illumination;
                recordFields["NormalPacketStart"] = NormalPacketStart;
                recordFields["SpectrumData"] = SpectrumData;
            } else {
                // For other RecordTypes, skip the data
                file.seekg(DataLength, std::ios::cur);
                Rcpp::Rcout << "Unhandled RecordType: " << RecordType << ". Skipping..." << std::endl;
            }

            // Add the record to the list
            records.push_back(recordFields);
        } catch (std::exception& ex) {
            // Handle exceptions during parsing
            Rcpp::Rcerr << "Error parsing RecordType " << RecordType << ": " << ex.what() << std::endl;
            // Store an error message instead of the record
            Rcpp::List errorRecord;
            errorRecord["RecordType"] = RecordType;
            errorRecord["Error"] = ex.what();
            records.push_back(errorRecord);
            // Continue to the next record
            continue;
        }
    }

    file.close();

    return records;
}


// [[Rcpp::export()]]
NumericVector readPDZManual(std::string fileName, int start, int size) {
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

using namespace Rcpp;
//  [[Rcpp::export()]]
std::vector<float> readPDZ24(const std::string& fileName, int start) {
    float spectrum[2048] = {0.0f}; // Initialize to zeros
    
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        Rcpp::stop("Could not open file");
    }

    // Seek to the position where spectrum starts
    file.seekg(start, std::ios_base::beg);
    
    for (int i = 0; i < 2048; ++i) {
        uint32_t count_value;
        file.read(reinterpret_cast<char*>(&count_value), sizeof(uint32_t));
        
        if (!file) {
            Rcpp::stop("Failed to read record");
        }

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
std::vector<float> readPDZ25OG(std::string fileName) {
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

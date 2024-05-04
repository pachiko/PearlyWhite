#pragma once
#include <fstream>
#include <vector>


class DICOMReader {
public:
	DICOMReader(const std::string&);
	~DICOMReader();

private:
	std::ifstream file;
	std::vector<char> buffer;

	bool littleEndian = true;
	bool explicitVR = true;
};
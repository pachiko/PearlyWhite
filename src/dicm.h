#pragma once
#include <fstream>
#include <vector>


// Reads a DICOM file and displays its image
class DICOMReader {
public:
	DICOMReader(const std::string&);
	~DICOMReader();

private:
	std::ifstream file;
	std::vector<char> buffer;

	bool littleEndian = true;
	bool explicitVR = true;

	size_t rows, cols;
	size_t bpp; // bytes per pixel

	bool checkPrefix();
	bool parseTransferSyntax();
	bool parseImageRowsCols();
	bool parsePixelData();

	bool readElement(const std::array<char, 4>&);
};
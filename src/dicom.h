#pragma once
#include <fstream>
#include <vector>
#include <opencv2/core.hpp>


// Reads a DICOM file and displays its image
class DICOMReader {
public:
	DICOMReader(const std::string&);

	const cv::Mat& getImage() const;
private:
	std::stringstream fileData;
	cv::Mat image;

	std::string buffer;

	bool littleEndian = true;
	bool explicitVR = true;

	size_t rows, cols;
	size_t bpp; // bytes per pixel

	bool readFileData(const std::string& path);
	bool checkPrefix();
	bool parseTransferSyntax();
	bool parseImageRowsCols();
	bool parsePixelData();

	bool readElement(const std::array<char, 4>&);
};
#include "dicm.h"
#include <iostream>
#include <array>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

DICOMReader::DICOMReader(const std::string& path) {
	file.open(path, std::ios::binary);
	if (!file) {
		std::cout << "ERROR: Cannot open file!\n";
		return;
	}

	// Prefix
	const unsigned int prefixPos = 128;
	const unsigned int prefixSize = 4;
	file.seekg(prefixPos, file.beg);

	buffer.resize(prefixSize);
	file.read(buffer.data(), prefixSize);
	std::string prefix = "DICM";
	if (std::memcmp(buffer.data(), prefix.data(), prefixSize)) {
		std::cout << "ERROR: Not a DICOM file!\n";
		file.close();
		return;
	}

	// Transfer Syntax
	std::array<char, 4> transferSyntaxTag = { 0x02, 0x00, 0x10, 0x00 }; // Little Endian
	auto it = std::search(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), transferSyntaxTag.begin(), transferSyntaxTag.end());
	if (it == std::istreambuf_iterator<char>()) {
		std::cout << "ERROR: Could not find Transfer Syntax!\n";
		file.close();
		return;
	}

	buffer.resize(2);
	file.read(buffer.data(), 2);
	std::string transferSyntaxVR = "UI";
	if (std::memcmp(buffer.data(), transferSyntaxVR.data(), 2)) {
		std::cout << "ERROR: Transfer Syntax is not of UID Value-type!\n";
		file.close();
		return;
	}

	file.read(buffer.data(), 2);
	unsigned char b1 = buffer[1];
	unsigned char b0 = buffer[0];
	unsigned int transferLength = b0 + (b1 << 8);

	buffer.resize(transferLength);
	file.read(buffer.data(), transferLength);
	std::string transferSyntax = "1.2.840.10008.1.2.1";
	if (std::memcmp(buffer.data(), transferSyntax.data(), 2)) {
		std::cout << "ERROR: Transfer Syntax is not of Explicit VR Little Endian!\n";
		file.close();
		return;
	}

	// Pixel rows & columns
	std::array<char, 4> pixelRowTag = { 0x28, 0x00, 0x10, 0x00 }; // Little Endian
	it = std::search(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), pixelRowTag.begin(), pixelRowTag.end());
	if (it == std::istreambuf_iterator<char>()) {
		std::cout << "ERROR: Could not find Pixel Rows!\n";
		file.close();
		return;
	}
	file.read(buffer.data(), 2);
	std::string pixelRowVR = "US";
	if (std::memcmp(buffer.data(), pixelRowVR.data(), 2)) {
		std::cout << "ERROR: PixelRow is not of US Value-type!\n";
		file.close();
		return;
	}
	file.read(buffer.data(), 2);
	b1 = buffer[1];
	b0 = buffer[0];
	b1 = b1 << 8;
	unsigned int rowLength = b0 + b1;
	file.read(buffer.data(), rowLength);
	b1 = buffer[1];
	b0 = buffer[0];
	unsigned int numRows = b0 + (b1 << 8);

	std::array<char, 4> pixelColTag = { 0x28, 0x00, 0x11, 0x00 }; // Little Endian
	it = std::search(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), pixelColTag.begin(), pixelColTag.end());
	if (it == std::istreambuf_iterator<char>()) {
		std::cout << "ERROR: Could not find Pixel Cols!\n";
		file.close();
		return;
	}
	file.read(buffer.data(), 2);
	std::string pixelColVR = "US";
	if (std::memcmp(buffer.data(), pixelColVR.data(), 2)) {
		std::cout << "ERROR: PixelCol is not of US Value-type!\n";
		file.close();
		return;
	}
	file.read(buffer.data(), 2);
	b1 = buffer[1];
	b0 = buffer[0];
	unsigned int colLength = b0 + (b1 << 8);
	file.read(buffer.data(), colLength);
	b1 = buffer[1];
	b0 = buffer[0];
	unsigned int numCols = b0 + (b1 << 8);

	// Pixel Data
	std::array<char, 4> pixelTag = { 0xe0, 0x7f, 0x10, 0x00 };
	it = std::search(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), pixelTag.begin(), pixelTag.end());
	if (it == std::istreambuf_iterator<char>()) {
		std::cout << "ERROR: Could not find Pixel Data!\n";
		file.close();
		return;
	}
	
	file.read(buffer.data(), 2);
	std::string pixelVR = "OW";
	if (std::memcmp(buffer.data(), pixelVR.data(), 2)) {
		std::cout << "ERROR: Pixel Data is not of OW Value-type!\n";
		file.close();
		return;
	}

	file.seekg(2, file.cur); // skip reserved
	buffer.resize(4);
	file.read(buffer.data(), 4);
	b0 = buffer[0];
	b1 = buffer[1];
	unsigned char b2 = buffer[2];
	unsigned char b3 = buffer[3];
	unsigned int numBytes = b0 + (b1 << 8) + (b2 << 16) + (b3 << 24);

	if (numRows * numCols * 2 != numBytes) {
		std::cout << "ERROR: NUmber of Bytes in pixel data does not match numbers of rows and columns!\n";
		file.close();
		return;
	}

	cv::Mat image = cv::Mat::zeros(cv::Size(numCols, numRows), CV_16UC1);
	file.read(reinterpret_cast<char*>(image.ptr()), numBytes);
	cv::imshow("WTF", image);
	cv::waitKey(0);
}


DICOMReader::~DICOMReader() {
	if (file.is_open()) file.close();
}
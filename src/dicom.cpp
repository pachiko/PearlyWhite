#include "dicom.h"
#include <iostream>
#include <array>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


// Byte swaps an integer
inline uint byteSwap(char* bytes, size_t length) {
	if (length > 4) {
		std::cout << "Overflow!\n";
		return 0;
	}

	uchar* data = reinterpret_cast<uchar*>(bytes);
	uint res = 0;

	for (size_t i = 0; i < length; i++) {
		res += data[i] << (i * 8);
	}

	return res;
}


// Checks the prefix in the DICOM file. Must be "DICM"
bool DICOMReader::checkPrefix() {
	const uint prefixPos = 128;
	const uint prefixSize = 4;
	file.seekg(prefixPos, file.beg);

	buffer.resize(prefixSize);
	file.read(buffer.data(), prefixSize);

	const std::string prefix = "DICM";

	if (std::memcmp(buffer.data(), prefix.data(), prefixSize)) {
		std::cout << "ERROR: Not a DICOM file!\n";
		file.close();
		return false;
	}

	return true;
}


// Searches the file for the tag and reads the data into 'buffer'
bool DICOMReader::readElement(const std::array<char, 4>& tag) {
	auto it = std::search(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), tag.begin(), tag.end());
	if (it == std::istreambuf_iterator<char>()) {
		return false;
	}

	// Read and check the explicit VR (2 Bytes)
	const uint vrSize = 2;
	buffer.resize(vrSize);
	file.read(buffer.data(), vrSize);

	bool diffLayout = false;

	const std::array<std::string, 6> diffLayouts = {"OB", "OW", "OF", "SQ", "UT", "UN"};
	for (auto& vr : diffLayouts) {
		if (!std::memcmp(buffer.data(), vr.data(), vrSize)) {
			diffLayout = true;
			break;
		}
	}
	
	// Read the value length and value field
	if (diffLayout) {
		file.seekg(2, file.cur); // Skip reserved bytes [00 00]

		buffer.resize(4);
		file.read(buffer.data(), 4); // Value length is 4 Bytes
		uint length = byteSwap(buffer.data(), 4);

		buffer.resize(length);
		file.read(buffer.data(), length);
	} else {
		file.read(buffer.data(), 2); // Value length is 2 Bytes
		uint length = byteSwap(buffer.data(), 2);

		buffer.resize(length);
		file.read(buffer.data(), length);
	}

	return true;
}


// Checks the transfer syntax to set endianness and explicitVR flag.
bool DICOMReader::parseTransferSyntax() {
	std::array<char, 4> transferSyntaxTag = { 0x02, 0x00, 0x10, 0x00 };

	if (!readElement(transferSyntaxTag)) {
		std::cout << "ERROR: Could not find Transfer Syntax!\n";
		file.close();
		return false;
	}

	std::string explicitVRLittleEndian = "1.2.840.10008.1.2.1";

	// TODO: support other syntaxes
	if (std::memcmp(buffer.data(), explicitVRLittleEndian.data(), buffer.size())) {
		std::cout << "ERROR: Unknown Transfer Syntax! ";
		std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(std::cout));
		file.close();
		return false;
	}

	return true;
}


// Determine the number of image rows and columns
bool DICOMReader::parseImageRowsCols() {
	std::array<char, 4> pixelRowTag = { 0x28, 0x00, 0x10, 0x00 };
	if (!readElement(pixelRowTag)) {
		std::cout << "ERROR: Could not find Pixel Rows!\n";
		file.close();
		return false;
	}
	rows = byteSwap(buffer.data(), buffer.size());

	std::array<char, 4> pixelColTag = { 0x28, 0x00, 0x11, 0x00 };
	if (!readElement(pixelColTag)) {
		std::cout << "ERROR: Could not find Pixel Columns!\n";
		file.close();
		return false;
	}
	cols = byteSwap(buffer.data(), buffer.size());
	
	return true;
}


// Parse pixel data
bool DICOMReader::parsePixelData() {
	std::array<char, 4> pixelTag = { 0xe0, 0x7f, 0x10, 0x00 };

	if (!readElement(pixelTag)) {
		std::cout << "ERROR: Could not find Pixel Data!\n";
		file.close();
		return false;
	}

	bpp = buffer.size() / (rows * cols);

	return true;
}


DICOMReader::DICOMReader(const std::string& path) {
	file.open(path, std::ios::binary);
	if (!file) {
		std::cout << "ERROR: Cannot open file!\n";
		return;
	}

	if (!checkPrefix()) {
		return;
	}

	if (!parseTransferSyntax()) {
		return;
	}

	if (!parseImageRowsCols()) {
		return;
	}

	if (!parsePixelData()) {
		return;
	}

	if (bpp == 2) {
		cv::Mat image(cv::Size(cols, rows), CV_16UC1, reinterpret_cast<uchar*>(buffer.data()));
		cv::imshow("DICOM", image);
		cv::waitKey(0);
	} // TODO: support other bit-depths & number of channels
}


DICOMReader::~DICOMReader() {
	if (file.is_open()) file.close();
}
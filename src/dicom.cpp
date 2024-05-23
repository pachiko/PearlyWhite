#include "dicom.h"
#include <iostream>
#include <array>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// #define DEBUG_DICOM

DICOMReader::DICOMReader(const std::string& path) {
	if (!readFileData(path)) throw std::invalid_argument("Invalid File");

	if (!checkPrefix()) throw std::runtime_error("Invalid Prefix");

	if (!parseTransferSyntax()) throw std::runtime_error("Invalid Transfer Syntax");

	if (!parseImageRowsCols()) throw std::runtime_error("Invalid Rows and Columns");

	if (!parsePixelData()) throw std::runtime_error("Invalid Pixel Data");

	if (bpp == 2) {
		image = cv::Mat(cv::Size(cols, rows), CV_16UC1, reinterpret_cast<uchar*>(buffer.data()));
#ifdef DEBUG_DICOM
		cv::imshow("DICOM", image);
		cv::waitKey(0);
		cv::destroyWindow("DICOM");
#endif
	} // TODO: support other bit-depths & number of channels
}

const cv::Mat& DICOMReader::getImage() const {
	return image;
}

bool DICOMReader::readFileData(const std::string& path) {
	std::ifstream file(path, std::ios::binary);

	if (!file) {
		std::cout << "ERROR: Cannot open file!\n";
		file.close();
		return false;
	}
	else {
		fileData << file.rdbuf();
		file.close();
		return true;
	}
}

bool DICOMReader::checkPrefix() {
	const uint prefixPos = 128;
	const uint prefixSize = 4;
	fileData.seekg(prefixPos, fileData.beg);
	fileData.read(buffer.data(), prefixSize);

	const std::string prefix = "DICM";

	if (std::strcmp(buffer.data(), prefix.data())) {
		std::cout << "ERROR: Not a DICOM file!\n";
		return false;
	}

	return true;
}

bool DICOMReader::parseTransferSyntax() {
	const std::array<char, 4> transferSyntaxTag = { 0x02, 0x00, 0x10, 0x00 };

	if (!readElement(transferSyntaxTag)) {
		std::cout << "ERROR: Could not find Transfer Syntax!\n";
		return false;
	}

	const std::string explicitVRLittleEndian = "1.2.840.10008.1.2.1";

	// TODO: support other syntaxes
	if (std::strcmp(buffer.data(), explicitVRLittleEndian.data())) {
		std::cout << "ERROR: Unknown Transfer Syntax! ";
		std::cout << buffer << "\n";
		return false;
	}

	return true;
}

bool DICOMReader::parseImageRowsCols() {
	const std::array<char, 4> pixelRowTag = { 0x28, 0x00, 0x10, 0x00 };
	if (!readElement(pixelRowTag)) {
		std::cout << "ERROR: Could not find Pixel Rows!\n";
		return false;
	}
	rows = readUint(buffer.data(), buffer.size());

	const std::array<char, 4> pixelColTag = { 0x28, 0x00, 0x11, 0x00 };
	if (!readElement(pixelColTag)) {
		std::cout << "ERROR: Could not find Pixel Columns!\n";
		return false;
	}
	cols = readUint(buffer.data(), buffer.size());

	return true;
}

bool DICOMReader::parsePixelData() {
	const std::array<char, 4> pixelTag = { 0xe0, 0x7f, 0x10, 0x00 };

	if (!readElement(pixelTag)) {
		std::cout << "ERROR: Could not find Pixel Data!\n";
		return false;
	}

	bpp = buffer.size() / (rows * cols);

	return true;
}

bool DICOMReader::readElement(const std::array<char, 4>& tag) {
	auto it = std::search(std::istreambuf_iterator<char>(fileData), std::istreambuf_iterator<char>(), tag.begin(), tag.end());
	if (it == std::istreambuf_iterator<char>()) {
		return false;
	}

	// Read and check the explicit VR (2 Bytes)
	const uint vrSize = 2;
	buffer.resize(vrSize);
	fileData.read(buffer.data(), vrSize);

	bool diffLayout = false;

	const std::array<std::string, 6> diffLayouts = {"OB", "OW", "OF", "SQ", "UT", "UN"};
	for (auto& vr : diffLayouts) {
		if (!std::strcmp(buffer.data(), vr.data())) {
			diffLayout = true;
			break;
		}
	}
	
	// Read the value length and value field
	if (diffLayout) {
		fileData.seekg(2, fileData.cur); // Skip reserved bytes [00 00]

		buffer.resize(4);
		fileData.read(buffer.data(), 4); // Value length is 4 Bytes
		uint length = readUint(buffer.data(), 4);

		buffer.resize(length);
		fileData.read(buffer.data(), length);
	} else {
		fileData.read(buffer.data(), 2); // Value length is 2 Bytes
		uint length = readUint(buffer.data(), 2);

		buffer.resize(length);
		fileData.read(buffer.data(), length);
	}

	return true;
}


uint DICOMReader::readUint(const char* bytes, const size_t length) {
	if (length > 4) {
		std::cout << "Overflow!\n";
		return 0;
	}

	const uchar* data = reinterpret_cast<const uchar*>(bytes);
	uint res = 0;
	
	if (littleEndian) {
		for (size_t i = 0; i < length; i++) {
			res += data[i] << (i * 8);
		}
	} else {
		for (size_t i = length - 1; i > -1; i--) {
			res += data[i] << (i * 8);
		}
	}

	return res;
}

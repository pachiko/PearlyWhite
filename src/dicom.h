#pragma once
#include <fstream>
#include <vector>
#include <opencv2/core.hpp>


/// Reads a DICOM file and extracts its image
/// Example: 
///		DICOMReader reader("file.dcm");
///		cv::Mat image = reader.getImage();
class DICOMReader {
public:
	/// @brief Constructs a DICOM Reader and reads the data of a DICOM file into the fields.
	/// @param The path of the DICOM file
	DICOMReader(const std::string&);

	/// @brief Returns the image extracted from the DICOM File
	/// @return The image as a const reference
	const cv::Mat& getImage() const;
private:
	// The entire file's data. Uses stringstream because it may need to skip bytes sometimes
	std::stringstream fileData;

	// The extracted image
	cv::Mat image;

	// The buffer to contain extracted data from fileData
	std::string buffer;

	// Indicates whether the numbers in the file data are in little-Endian or not
	bool littleEndian = true;

	// Indicates whether the tag elements in the file have a value-representation field or not
	bool explicitVR = true;

	// Number of rows and columns for the image
	size_t rows, cols;

	// Bytes per pixel of the image
	size_t bpp;

	/// @brief Reads all data from file into fileData
	/// @param The path of the DICOM file
	/// @return True on success
	bool readFileData(const std::string& path);

	/// @brief Check the file data for the 'DICM' prefix. Not a DICOM file if absent.
	/// @return True if prefix is found
	bool checkPrefix();

	/// @brief Parse the type of transfer syntax used by the file. Some syntaxes are not supported.
	/// @return True if the transfer syntax used is known and supported.
	bool parseTransferSyntax();

	/// @brief Extract the number of rows and columns used by the image in the file.
	/// @return True if this information can be found in the file.
	bool parseImageRowsCols();

	/// @brief Extract the pixel data from the image in the file.
	/// @return True if this information can be found in the file.
	bool parsePixelData();

	/// @brief Extracts an element's data based on the tag provided.
	/// @param The 4-byte array containing the element's tag to be searched
	/// @return True if this element can be found in the file.
	bool readElement(const std::array<char, 4>&);

	/// @brief Reads an unsigned integer from the array of bytes.
	/// @param const char*: The array of bytes
	/// @param const size_t: the length of the array
	/// @return The unsigned integer. 0 if the length is greater than 4 bytes
	uint readUint(const char*, const size_t);
};
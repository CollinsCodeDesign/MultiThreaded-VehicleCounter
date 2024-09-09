
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "modules/config.h"
#include <string>


using namespace std;

cv::Mat tryResizeImage(cv::Mat image, int targetWidth, int targetHeight) {
	if (targetWidth <= 0 || targetHeight <= 0) {
		cerr << "Error: Invalid target dimensions. Width and height must be positive." << endl;
		return cv::Mat();
	}

	int originalWidth = image.cols;
	int originalHeight = image.rows;

	if (originalWidth == 0 || originalHeight == 0) {
		cerr << "Error: Input image has invalid dimensions." << endl;
		return cv::Mat();
	}
	try {
		// static_cast used to convert one data type to another
		// others are dynamic_cast, const_cast, reinterpret_cast
		double scaleFactorX = static_cast<double>(targetWidth) / originalWidth;
		double scaleFactorY = static_cast<double>(targetHeight) / originalHeight;
		double scaleFactor = min(scaleFactorX, scaleFactorY);
		int newWidth = static_cast<int>(originalWidth * scaleFactor);
		int newHeight = static_cast<int>(originalHeight * scaleFactor);
		cv::Mat resizedImageMat;
		cv::resize(image, resizedImageMat, cv::Size(newWidth, newHeight));
		return resizedImageMat;
	}
	catch (const cv::Exception& e) {
		cerr << "Error during image resizing: " << e.what() << endl;
		return cv::Mat();
	}
	catch (const std::exception& e) {
		cerr << "General error: " << e.what() << endl;
		return cv::Mat();
	}
}

void resetFile(fstream& file) {
	file.clear(); // Clear the EOF flag
	file.seekg(0); // Return to the beginning of the file
}
int countVehicles() {
	Config config("src\\config\\config.ini");
	std::cout << "movement_threshold: " << config.getMovementThreshold() << std::endl;
	cv::VideoCapture video(config.getLocalVideo()); // Replace with your video file path

	// Check if video opened successfully
	if (!video.isOpened()) {
		std::cerr << "Error: Could not open video." << std::endl;
		return -1;
	}

	// Get the original frame width and height
	int originalWidth = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
	int originalHeight = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));

	// Define the target width and height for resizing
	int targetWidth = 1920;
	int targetHeight = 1080;

	// Calculate the scale factors
	double scaleFactorX = static_cast<double>(targetWidth) / originalWidth;
	double scaleFactorY = static_cast<double>(targetHeight) / originalHeight;
	double scaleFactor = std::min(scaleFactorX, scaleFactorY);

	int newWidth = static_cast<int>(originalWidth * scaleFactor);
	int newHeight = static_cast<int>(originalHeight * scaleFactor);

	cv::Mat frame, resizedFrame, grayFrame, prevGrayFrame, diffFrame;
	cv::Rect lane1DetectionBox(
		config.getLane1DetectionBox()[0],
		config.getLane1DetectionBox()[1],
		config.getLane1DetectionBox()[2],
		config.getLane1DetectionBox()[3]
	);
	int detectionCounter = 0;
	bool lastFrameDetected = false;

	while (true) {
		// Capture each frame from the video
		video >> frame;

		// If the frame is empty, break the loop
		if (frame.empty()) {
			break;
		}

		// Resize the frame
		cv::resize(frame, resizedFrame, cv::Size(newWidth, newHeight));

		// Convert the frame to grayscale
		cv::cvtColor(resizedFrame, grayFrame, cv::COLOR_BGR2GRAY);
		if (prevGrayFrame.empty()) {
			grayFrame.copyTo(prevGrayFrame);
			continue;
		}
		cv::absdiff(grayFrame, prevGrayFrame, diffFrame);
		cv::threshold(diffFrame, diffFrame, 30, 255, cv::THRESH_BINARY);
		cv::Mat roi = diffFrame(lane1DetectionBox);
		double movement = cv::sum(roi)[0]; // Sum of all pixel values in the ROI

		if (movement > config.getMovementThreshold()) {
			if (!lastFrameDetected) {
				detectionCounter++;
			}
			lastFrameDetected = true;
			cout << "Movement detected in the box!" << endl;
			cv::rectangle(resizedFrame, lane1DetectionBox, cv::Scalar(0, 0, 255), 2); // Change color if movement is detected
		}
		else {
			lastFrameDetected = false;
			cv::rectangle(resizedFrame, lane1DetectionBox, cv::Scalar(0, 255, 0), 2); // Green if no movement
		}
		std::string text = "Count: " + std::to_string(detectionCounter);
		cv::putText(resizedFrame, text, cv::Point(lane1DetectionBox.x, lane1DetectionBox.y - 10),
			cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
		cv::imshow("Video with Movement Detection", resizedFrame);

		// Wait for 25 ms and break the loop if the user presses a key
		if (cv::waitKey(25) >= 0) {
			break;
		}
		grayFrame.copyTo(prevGrayFrame);
	}
	video.release();
	cv::destroyAllWindows();
	return 0;
}
Config setConfigVars() {
	Config config("src\\config\\config.ini");
	std::string username = config.get(std::string("username"), std::string("default_user"));
	int timeout = config.get<int>("timeout", 60);
	bool debug = config.get<bool>("debug", false);
	return config;
}
int main() {
	countVehicles();
	return 0;
}

//int main() {
//	// countVehicles();
//
//	// init object and set to read
//	std::fstream configInputFile("src\\config\\config.ini");
//	// check if file object is open
//	if (!configInputFile.is_open()) {
//		cerr << "Error opening file for reading." << endl;
//	}
//	// create string data type for scratch pad to store each line temporarily 
//	string line;
//	while (getline(configInputFile, line)) {
//		cout << line << endl;
//	}
//
//	resetFile(configInputFile);
//	string word;
//	if (configInputFile >> word) {
//		cout << "Read operation successful" << endl;
//	}
//	else {
//		cout << "Read operation failed" << endl;
//	}
//	while (configInputFile >> word) {
//		cout << word << endl;
//	}
//
//	configInputFile.close();
//	std::ofstream outputFile("output.txt");
//
//
//
//	return 0;
//}

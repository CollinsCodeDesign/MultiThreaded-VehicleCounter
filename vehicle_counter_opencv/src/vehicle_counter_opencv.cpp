
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "modules/config.h"
#include <string>
#include <libpq-fe.h>


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
class VehicleCounter {
private:
	cv::Rect _detectBox;
	int _counter;
	double _movement;
	bool _lastFrameDetected;
public:
	VehicleCounter(cv::Rect box, int movement) : _detectBox(box), _counter(0), _movement(movement), _lastFrameDetected(false) {}

};
int countVehicles() {
	JsonConfigLoader config("src\\config\\config.json");
	cv::VideoCapture video(config.getStringValue("localVideo")); // Replace with your video file path

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
	// Add a way to add multiple boxes
	cv::Rect lane1DetectionBox(
		config.getIntValue("lane1X"),
		config.getIntValue("lane1Y"),
		config.getIntValue("lane1Width"),
		config.getIntValue("lane1Height")
	);

	cv::Rect lane2DetectionBox(
		config.getIntValue("lane2X"),
		config.getIntValue("lane2Y"),
		config.getIntValue("lane2Width"),
		config.getIntValue("lane2Height")
	);

	cv::Rect lane3DetectionBox(
		config.getIntValue("lane3X"),
		config.getIntValue("lane3Y"),
		config.getIntValue("lane3Width"),
		config.getIntValue("lane3Height")
	);
	int lane1DetectionCounter = 0;
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
		// Add pipeline feature that makes it configurable 
		cv::cvtColor(resizedFrame, grayFrame, cv::COLOR_BGR2GRAY);
		if (prevGrayFrame.empty()) {
			grayFrame.copyTo(prevGrayFrame);
			continue;
		}
		cv::absdiff(grayFrame, prevGrayFrame, diffFrame);
		cv::threshold(diffFrame, diffFrame, 30, 255, cv::THRESH_BINARY);
		cv::Mat lane1Roi = diffFrame(lane1DetectionBox);
		cv::Mat lane2Roi = diffFrame(lane2DetectionBox);
		cv::Mat lane3Roi = diffFrame(lane3DetectionBox);
		// Sum of all pixel values in the ROI
		double lane1Movement = cv::sum(lane1Roi)[0];
		double lane2Movement = cv::sum(lane2Roi)[0];
		double lane3Movement = cv::sum(lane3Roi)[0];

		if (lane1Movement > config.getIntValue("movementThreshold")) {
			if (!lastFrameDetected) {
				lane1DetectionCounter++;
			}
			lastFrameDetected = true;
			cv::rectangle(resizedFrame, lane1DetectionBox, cv::Scalar(0, 0, 255), 2); // Change color if movement is detected
		}
		else {
			lastFrameDetected = false;
			cv::rectangle(resizedFrame, lane1DetectionBox, cv::Scalar(0, 255, 0), 2); // Green if no movement
		}
		std::string text = "Count: " + std::to_string(lane1DetectionCounter);
		cv::putText(resizedFrame, text, cv::Point(lane1DetectionBox.x, lane1DetectionBox.y - 10),
			cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
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

int main() {
	// Add a more secure way of logging into the database
	const char* conninfo = "dbname=vehicle_counter user=postgres password=Vann3299 hostaddr=127.0.0.1 port=5432";
	PGconn* conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return 1;
	}
	std::cout << "Connected to database successfully!" << std::endl;

	// Execute an SQL query
	// Create table for storing count alerts
	PGresult* res = PQexec(conn, "SELECT version();");

	// Check query result status
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		PQfinish(conn);
		return 1;
	}

	// Print the result of the query (PostgreSQL version)
	std::cout << "PostgreSQL version: " << PQgetvalue(res, 0, 0) << std::endl;

	// Clean up
	PQclear(res);
	PQfinish(conn);

	countVehicles();
	return 0;
}
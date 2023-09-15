#ifndef DOCUMENT_SCANNER_H
#define DOCUMENT_SCANNER_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

namespace document_scanner {
	class Scanner {
	public:
		Scanner();
		cv::Mat PreProcessing(cv::Mat);
		std::vector<cv::Point> GetContours(cv::Mat);
		void DrawPoints(std::vector<cv::Point>, cv::Scalar);
		std::vector<cv::Point> Reorder(std::vector<cv::Point>);
		cv::Mat GetWarp(cv::Mat img, std::vector<cv::Point>, float, float);
		void ScanDocument(std::string);

	private:
		cv::Mat img_original, img_gray, img_blur, img_canny, img_threshold, img_dilated,
			img_erode, img_warp, img_crop;
		std::vector<cv::Point> initial_points, doc_points;
		float w, h;
	};

}

#endif
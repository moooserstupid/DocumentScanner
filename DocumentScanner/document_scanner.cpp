#include "document_scanner.hpp"

namespace ds = document_scanner;

ds::Scanner::Scanner() {
	w = 420, h = 596;
}

cv::Mat ds::Scanner::PreProcessing(cv::Mat img) {

	cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
	GaussianBlur(img_gray, img_blur, cv::Size(3, 3), 3, 0);
	Canny(img_blur, img_canny, 25, 75);
	cv::Mat kernel = getStructuringElement(cv::MORPH_RECT,
		cv::Size(3, 3));
	dilate(img_canny, img_dilated, kernel);
	return img_dilated;
}

std::vector<cv::Point> ds::Scanner::GetContours(cv::Mat img) {
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	findContours(img, contours, hierarchy, cv::RETR_EXTERNAL,
		cv::CHAIN_APPROX_SIMPLE);
	std::vector<std::vector<cv::Point>> con_poly(contours.size());
	std::vector<cv::Rect> bound_rect(contours.size());

	std::vector<cv::Point> biggest;
	int max_area = 0;

	for (int i = 0; i < contours.size(); ++i) {
		int area = contourArea(contours[i]);

		std::string objectType;

		if (area > 1000) {
			float perimeter = arcLength(contours[i], true);

			approxPolyDP(contours[i], con_poly[i],
				0.02 * perimeter, true);

			if (area > max_area && con_poly[i].size() == 4) {
				biggest = { con_poly[i][0], con_poly[i][1],
					con_poly[i][2], con_poly[i][3] };
				max_area = area;
			}

		}
	}
	return biggest;
}

void ds::Scanner::DrawPoints(std::vector<cv::Point> points, cv::Scalar color) {

	for (int i = 0; i < points.size(); ++i) {
		circle(img_original, points[i], 10, color, cv::FILLED);
		putText(img_original, std::to_string(i), points[i],
			cv::FONT_HERSHEY_PLAIN, 4, color, 4);
	}
}
std::vector<cv::Point> ds::Scanner::Reorder(std::vector<cv::Point> points) {
	std::vector<cv::Point> new_points;
	std::vector<int> sum_points, sub_points;

	for (int i = 0; i < 4; ++i) {
		sum_points.push_back(points[i].x + points[i].y);
		sub_points.push_back(points[i].x - points[i].y);
	}

	new_points.push_back(points[std::min_element(sum_points.begin(),
		sum_points.end()) - sum_points.begin()]);
	new_points.push_back(points[std::max_element(sub_points.begin(),
		sub_points.end()) - sub_points.begin()]);
	new_points.push_back(points[std::min_element(sub_points.begin(),
		sub_points.end()) - sub_points.begin()]);
	new_points.push_back(points[std::max_element(sum_points.begin(),
		sum_points.end()) - sum_points.begin()]);

	return new_points;
}

cv::Mat ds::Scanner::GetWarp(cv::Mat img, std::vector<cv::Point> points, float w, float h) {

	cv::Point2f src[4] = { points[0], points[1], points[2], points[3] };
	cv::Point2f dst[4] = { {0.0f, 0.0f}, {w, 0.0f}, {0.0f, h}, {w, h} };

	cv::Mat matrix = cv::getPerspectiveTransform(src, dst);
	cv::warpPerspective(img, img_warp, matrix, cv::Point(w, h));

	return img_warp;
}

void ds::Scanner::ScanDocument(std::string path) {


	img_original = cv::imread(path);

	// Preprocessing - Step 1
	img_threshold = PreProcessing(img_original);

	// Get Contours - Biggest - Step 2
	initial_points = GetContours(img_threshold);
	doc_points = Reorder(initial_points);

	// Warp - Step 3
	img_warp = GetWarp(img_original, doc_points, w, h);

	// Crop - Step 4
	int crop_value = 5;
	cv::Rect roi(crop_value, crop_value, w - (2 * crop_value),
		h - (2 * crop_value));
	img_crop = img_warp(roi);

	imshow("Image", img_original);
	imshow("Image Dilated", img_dilated);
	imshow("Image Warp", img_warp);
	imshow("Image Crop", img_crop);

	cv::waitKey(0);
}

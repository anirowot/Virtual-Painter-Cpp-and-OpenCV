#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

Mat img;
VideoCapture cap(0);
vector<vector<int>> newpoints;

// hmin, smin, vmin hmax, smax, vmax
vector<vector<int>> mycolors{ {62,75,62,84,196,191},//green
								{37,132,142,179,175,175},//red
								  {94,127,144,113,255,255} };//blue
vector<Scalar> mycolorvalues{ {0,255,0},//green
	{0,0,255},//red
	{255,0,0} };//blue
Point getcontours(Mat imgDil) {

	vector<vector<Point>> contours;		//border of images
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);		//(input_matrix, contours, heirarchy, effects)

	vector<vector<Point>> conPoly(contours.size());		//sides of an image using contours
	vector<Rect> BoundRect(contours.size());			//rectangle boundry around the image

	Point mypoint(0, 0);			//origin

	for (int i = 0; i < contours.size(); i++) {

		int area = contourArea(contours[i]);			//area of the image
		cout << area << endl;

		string objectType;

		if (area > 1000) {

			float peri = arcLength(contours[i], true);				//perimenter - true = image closed
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);		//adds no. of sides of images to conPoly

			cout << conPoly[i].size() << endl;
			BoundRect[i] = boundingRect(conPoly[i]);				//adds rectangle boundry based on no. of sides to BoundRect
			mypoint.x = BoundRect[i].x + BoundRect[i].width / 2;
			mypoint.y = BoundRect[i].y;

			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);		//(input_matrix, sides, i, colorBGR, thickness)
			rectangle(img, BoundRect[i].tl(), BoundRect[i].br(), Scalar(0, 255, 0), 3);
		}
	}
	return mypoint;
}

vector<vector<int>> findcolour(Mat img) {

	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < mycolors.size(); i++)
	{
		Scalar lower(mycolors[i][0], mycolors[i][1], mycolors[i][2]);
		Scalar upper(mycolors[i][3], mycolors[i][4], mycolors[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);
		Point mypoint = getcontours(mask);

		if (mypoint.x != 0 && mypoint.y != 0)
		{
			newpoints.push_back({ mypoint.x, mypoint.y, i });
		}
	}
	return newpoints;
}

void drawoncanvas(vector<vector<int>> newpoints, vector<Scalar> mycolorvalues) {

	for (int i = 0; i < newpoints.size(); i++)
	{
		circle(img, Point(newpoints[i][0], newpoints[i][1]), 7, mycolorvalues[newpoints[i][2]], FILLED);
	}
}

void main() {

	while (true) {

		cap.read(img);
		newpoints = findcolour(img);
		drawoncanvas(newpoints, mycolorvalues);

		imshow("Webcam", img);
		waitKey(1);
	}
}
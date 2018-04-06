/*The deployment environment is vs2013 + OpenCV 2.4.11(Nuget), the vs2015 + OpenCV 3.2.0 is very hard*/
//#include <opencv/highgui.h> // For VS2013
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//#include <opencv2/highgui/highgui.hpp> // For VS2015

#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <limits.h>

#include <sstream>
#include <fstream>
#include <iostream>
#include <omp.h>

#include <string.h>
#include <time.h>

#include <ctime>

#define K_KNN 4
#define H_Start_Number 0
#define H_End_Number 4
#define ITERATIVE 1
#define RANSAC_DISTANCE 3
//#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__), 0) )
#define NUM_REQ_HOMOGRAPHY (4)					    // required points to build a homography
#define HOMOGRAPHY_H_HEIGHT (NUM_REQ_HOMOGRAPHY<<1)	// height of H when build homography
#define HOMOGRAPHY_H_WIDTH		(9)					// width of H when build homography

#define THRESHOLD_GOODRESULT	(5)		
////#define puzzle
//
cv::Mat H[256];
int H_InlierNumber[256];

int key1, key2;
std::vector<cv::KeyPoint> keypoints1, keypoints2;

using namespace std;
using namespace cv;
Mat surfDescriptorGet(string path){
	// Step 1: Image read
	Mat Image = imread(path, IMREAD_COLOR);

	// Step 2: Surf feature detector and feature extractor
	SurfFeatureDetector detector(2500);
	SurfDescriptorExtractor extractor;
	// Step 3: Feature detection
	vector<KeyPoint> keypoints;
	detector.detect(Image, keypoints);
	cout << "After SURF, the Keypoints' number =      " << keypoints.size() << endl;

	// Step 4: Feature descriptor computation
	Mat descriptor;
	extractor.compute(Image, keypoints, descriptor);
	cout << "After SURF, the Descriptor's size = " << descriptor.size() << endl;

	// Step 5: Feature display on image
	Mat feature;
	drawKeypoints(Image, keypoints, feature, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	return descriptor;
}

vector<KeyPoint> surfKeypoint(string path){
	// Step 1: Image read
	Mat Image = imread(path, IMREAD_COLOR);

	// Step 2: Surf feature detector and feature extractor
	SurfFeatureDetector detector(2500);
	SurfDescriptorExtractor extractor;
	// Step 3: Feature detection
	vector<KeyPoint> keypoints;
	detector.detect(Image, keypoints);
	cout << "After SURF, the Keypoints' number =      " << keypoints.size() << endl;

	// Step 4: Feature descriptor computation
	Mat descriptor;
	extractor.compute(Image, keypoints, descriptor);
	cout << "After SURF, the Descriptor's size = " << descriptor.size() << endl;

	// Step 5: Feature display on image
	Mat feature;
	drawKeypoints(Image, keypoints, feature, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	return keypoints;
}

Mat siftDescriptorGet(string path) {
	// Step 1: Image read
	Mat Image = imread(path, IMREAD_COLOR);

	// Step 2: SIFT feature detector and feature extractor
	SiftFeatureDetector detector(0.001);
	SiftDescriptorExtractor extractor(0.05);

	// Step 3: Feature detection
	vector<KeyPoint> keypoints;
	detector.detect(Image, keypoints);
	cout << "After SIFT, the Keypoints' number =      " << keypoints.size() << endl;

	// Step 4: Feature descriptor computation
	Mat descriptor;
	extractor.compute(Image, keypoints, descriptor);
	cout << "After SIFT, the Descriptor's size = " << descriptor.size() << endl;

	// Step 5: Feature display on image
	Mat feature;
	drawKeypoints(Image, keypoints, feature, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	return descriptor;
}

vector<KeyPoint> siftKeypoint(string path) {
	// Image read
	Mat Image = imread(path, IMREAD_COLOR);

	// SIFT feature detector and feature extractor
	SiftFeatureDetector detector(0.001);
	SiftDescriptorExtractor extractor(0.05);

	// Feature detection
	vector<KeyPoint> keypoints;
	detector.detect(Image, keypoints);
	cout << "After SIFT, the Keypoints' number =     " << keypoints.size() << endl;

	// Feature descriptor computation
	Mat descriptor;
	extractor.compute(Image, keypoints, descriptor);
	cout << "After SIFT, the Descriptor's size = " << descriptor.size() << endl;

	// Feature display on image
	Mat feature;
	drawKeypoints(Image, keypoints, feature, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	return keypoints;
}

float EucliDistance(Point& p, Point& q)
{
	Point diff = p - q;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

double ComputeDistance(double x1, double y1, double x2, double y2)
{
	double distance;
	double x_diff = x1 - x2;
	double y_diff = y1 - y2;

	distance = sqrt(x_diff*x_diff + y_diff*y_diff);
	return distance;
}

/* At first, I want to use this way to caculate the best homography matrix, but I can use projective projective mappings well,
so then I use affine mapping and change to another way*/
//Mat ComputeH(Mat ObjectImage, Mat TargetImage){
//	/*Mat ObjectImage = imread("test.bmp", 1);
//	Mat TargetImage = imread("test.bmp", 1);*/
//	int diff_vector = 0;
//	// SIFT feature detector and feature extractor
//	SiftFeatureDetector detector(0.05, 5.0);
//	SiftDescriptorExtractor extractor(3.0);
//
//	// Feature detection
//	vector<KeyPoint> keypoints1, keypoints2;
//	detector.detect(ObjectImage, keypoints1);
//	detector.detect(TargetImage, keypoints2);
//
//	// Feature display
//	Mat feat1, feat2;
//	drawKeypoints(ObjectImage, keypoints1, feat1, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//	drawKeypoints(TargetImage, keypoints2, feat2, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//	imwrite("feat1.bmp", feat1);
//	imwrite("feat2.bmp", feat2);
//	//imshow("feat2",feat2);
//
//	int key1 = keypoints1.size();   //object
//	int key2 = keypoints2.size();   //target
//	printf("\nKeypoint1 = %d \n", key1);
//	printf("Keypoint2 = %d \n", key2);
//
//	// Feature descriptor computation
//	Mat descriptor1, descriptor2;
//	extractor.compute(ObjectImage, keypoints1, descriptor1);
//	extractor.compute(TargetImage, keypoints2, descriptor2);
//
//	printf("Descriptor1=(%d,%d)\n", descriptor1.size().height, descriptor1.size().width);
//	printf("Descriptor2=(%d,%d)\n", descriptor2.size().height, descriptor2.size().width);
//
//	cout << "Computing KNN" << endl;
//
//	int **KeyPoint_Neighborhood;    //int KeyPoint_Neighborhood[key1][key2];
//	int **K_NearestNeighbor;   //int K_NearestNeighbor[key1][K_KNN];
//
//	KeyPoint_Neighborhood = (int **)malloc(key1*sizeof(int*));
//	for (int i = 0; i < key1; ++i)
//		KeyPoint_Neighborhood[i] = (int *)malloc(key2*sizeof(int));
//
//	K_NearestNeighbor = (int **)malloc(key1*sizeof(int*));
//	for (int i = 0; i < key1; ++i)
//		K_NearestNeighbor[i] = (int *)malloc(K_KNN*sizeof(int));
//
//
//	cout << "computing KNN" << endl;
//
//	for (size_t i = 0; i < key1; ++i)
//	{
//		for (size_t j = 0; j < key2; ++j)
//		{
//			for (size_t k = 0; k < 128; ++k)
//			{
//				diff_vector += (descriptor1.at<float>(i, k) - descriptor2.at<float>(j, k))*(descriptor1.at<float>(i, k) - descriptor2.at<float>(j, k));
//			}
//			KeyPoint_Neighborhood[i][j] = sqrt(diff_vector);
//			diff_vector = 0;
//		}
//	}
//
//	int Min_Distance;
//	int Min_Index = 0;
//
//	for (size_t i = 0; i < key1; ++i)
//	{
//		for (size_t j = 0; j < K_KNN; ++j)
//		{
//			Min_Distance = KeyPoint_Neighborhood[i][0];
//			for (size_t k = 0; k < key2; ++k)
//			{
//				if (Min_Distance > KeyPoint_Neighborhood[i][k])
//				{
//					Min_Distance = KeyPoint_Neighborhood[i][k];
//					Min_Index = k;
//				}
//			}
//			K_NearestNeighbor[i][j] = Min_Index;
//			KeyPoint_Neighborhood[i][Min_Index] = INT_MAX;
//		}
//	}
//
//
//	//for (size_t i = 0; i < 4; ++i)
//	//{
//	//	cout <<  keypoints1[i].pt.x << " " << keypoints2[i].pt.y << endl;
//	//}
//
//	//for (int i = 0; i < key1; ++i)
//	//{
//	//	//printf("K_NearestNeighbor[%d] = ",i);
//	//		for (size_t j = 0; j < K_KNN; ++j)
//	//			printf(" %d ",K_NearestNeighbor[i][j]);
//	//			printf("\n");
//	//}
//	cout << "-------------------------------------------------------------------------------------" << endl;
//	std::vector<Point2f> obj[256];
//	std::vector<Point2f> scene[256];
//
//	int Extract_Index[256][4];
//	int m = 0;
//	int IndexForKNN;
//	int IndexForScene;
//
//	cout << "arrange the index of neighbors from KNN" << endl;
//	for (int i = 0; i < 4; ++i)
//	for (int j = 0; j < 4; ++j)
//	for (int k = 0; k < 4; ++k)
//	for (int l = 0; l < 4; l++)
//	{
//		Extract_Index[m][0] = i;
//		Extract_Index[m][1] = j;
//		Extract_Index[m][2] = k;
//		Extract_Index[m][3] = l;
//		m++;
//	}
//
//	memset(H_InlierNumber, 0, sizeof(int));
//	Mat H_local[256];
//	int Extract = 0;
//
//	int count = 0;
//	//There are 4*4*4*4 Homography matrix for each four points, we just call them Candidate_H
//	Mat Candidate_H[ITERATIVE];
//	int Candidate_InlierNumber[ITERATIVE];
//	int store[256][4];
//	int Candidate_store[ITERATIVE][4];
//	srand(time(NULL));
//
//	while (count < ITERATIVE)
//	{
//		Mat local_H[256];
//		cout << "computing Homography" << endl;
//		for (int i = 0; i < 256; ++i)
//		{
//			for (int j = 0; j < 4; ++j)
//			{
//				int CorespondIndex = (rand() % keypoints1.size());
//				store[i][j] = CorespondIndex;
//				IndexForKNN = Extract_Index[i][Extract];
//				IndexForScene = K_NearestNeighbor[CorespondIndex][IndexForKNN];
//				scene[i].push_back(keypoints2[IndexForScene].pt);
//				obj[i].push_back(keypoints1[CorespondIndex].pt);
//				Extract++;
//				if (Extract > 3)
//					Extract = 0;
//				//printf("CorespondIndex = %d ",CorespondIndex );
//			}
//			//printf("\n");
//			local_H[i] = findHomography(obj[i], scene[i]);
//			//Mat affine;
//			//calc_Homography(obj[i], scene[i], affine);
//			obj[i].clear();
//			scene[i].clear();
//			//cout << "local_H[" << i << "] = " << local_H[i] << endl;
//		}
//
//		double Candidate[3];
//		//cout << key1 << endl;
//		cout << "--------------------------------------Need to be checked here--------------------------------------" << endl;
//		//int tempKey1 = key1;
//		Mat TargetKeypoint[5000];
//		double Object_X, Object_Y;
//		double Target_X, Target_Y;
//		double distance;
//
//		int Max_InlierNumber = 0;
//		int Max_InlierIndex = 0;
//
//		cout << "Computing the best Homography using RANSAC" << endl;
//
//		for (int i = 0; i < 256; ++i)
//		{
//			for (int j = 0; j < key1; j++)
//			{
//				Candidate[0] = keypoints1[j].pt.x;
//				Candidate[1] = keypoints1[j].pt.y;
//				Candidate[2] = 1;
//
//				Mat Before(3, 1, CV_64FC1, Candidate);
//				TargetKeypoint[j] = local_H[i] * Before;
//				//cout << Before << endl;
//
//				/****************** Computing inlier using RANSAC ***************/
//
//				auto Target_wX = TargetKeypoint[j].at<double>(0);
//				auto Target_wY = TargetKeypoint[j].at<double>(1);
//				auto Target_w = TargetKeypoint[j].at<double>(2);
//
//				for (int k = 0; k < key2; k++)
//				{
//					Object_X = keypoints2[k].pt.x;
//					Object_Y = keypoints2[k].pt.y;
//					distance = ComputeDistance(Target_wX, Target_wY, Object_X, Object_Y);
//					//printf("distance = %f \n",distance );
//
//					if (distance < RANSAC_DISTANCE)
//					{
//						H_InlierNumber[i]++;
//						break;
//					}
//				}
//				//cout << "TargetKeypoint [" << j << "] = " << TargetKeypoint[j] << endl;
//				//cout << "----------------------------------------------------" << endl;
//				/*********************************************************/
//			}
//
//			if (H_InlierNumber[i] > Max_InlierNumber)
//			{
//				Max_InlierNumber = H_InlierNumber[i];
//				Max_InlierIndex = i;
//			}
//
//			//printf("H_InlierNumber[%d] = %d\n",i,H_InlierNumber[i]);
//		}
//
//		//printf("the best Candidate Homography[%d] = %d\n", count, Max_InlierNumber);
//		//printf("store = %d %d %d %d\n", store[Max_InlierIndex][0], store[Max_InlierIndex][1], store[Max_InlierIndex][2], store[Max_InlierIndex][3]);
//		// Use ransac to get the best candidate_H
//		Candidate_H[count] = local_H[Max_InlierIndex].clone();
//		Candidate_InlierNumber[count] = Max_InlierNumber;
//
//		for (int ii = 0; ii < 4; ++ii)
//		{
//			Candidate_store[count][ii] = store[Max_InlierIndex][ii];
//		}
//
//		count++;
//	}
//
//	int Max_Candidate_InlierNumber = 0;
//	int Max_Candidate_InlierIndex = 0;
//
//	for (int i = 0; i < ITERATIVE; ++i)
//	{
//		if (Candidate_InlierNumber[i] > Max_Candidate_InlierNumber)
//		{
//			Max_Candidate_InlierNumber = Candidate_InlierNumber[i];
//			Max_Candidate_InlierIndex = i;
//		}
//	}
//
//	//printf("the best candidate H[%d] is : %d \n", Max_Candidate_InlierIndex, Max_Candidate_InlierNumber);
//	//printf("store_corespond = %d %d %d %d\n", Candidate_store[Max_Candidate_InlierIndex][0], Candidate_store[Max_Candidate_InlierIndex][1], Candidate_store[Max_Candidate_InlierIndex][2], Candidate_store[Max_Candidate_InlierIndex][3]);
//	// Give Candidate_H[Max_Candidate_InlierIndex] a new name Reconverted_H
//	Mat Reconverted_H(Candidate_H[Max_Candidate_InlierIndex]);
//	return Reconverted_H;
//}

Mat KNN(Mat puzzleDescriptor, Mat sampleDescriptor)
{
	int k = 3, b = 0;
	Mat knnVector(puzzleDescriptor.rows, k, CV_32F);
	Mat knnDist(puzzleDescriptor.rows, k, CV_32F);
	float totalDiffer = 0, knnDiffer = 0, d1 = 0, d2 = 0;
	for (int i = 0; i < puzzleDescriptor.rows; i++){
		for (int j = 0; j < sampleDescriptor.rows; j++){
			for (int d = 0; d < puzzleDescriptor.cols; d++){
				d1 = puzzleDescriptor.at<float>(i, d) - sampleDescriptor.at<float>(j, d);
				d2 = d1*d1;
				totalDiffer = d2 + totalDiffer;
			}
			knnDiffer = sqrt(totalDiffer);
			totalDiffer = 0;
			if (j < k){
				knnVector.at<float>(i, j) = j;
				knnDist.at<float>(i, j) = knnDiffer;
			}
			else{
				float max_distance = 0;
				for (int m = 0; m < k; m++){
					if (knnDiffer < knnDist.at<float>(i, m)){
						if (knnDist.at<float>(i, m) > max_distance){
							max_distance = knnDist.at<float>(i, m);
							b = m;
						}
					}
				}
				if (knnDiffer < knnDist.at<float>(i, b)){
					knnDist.at<float>(i, b) = knnDiffer;
					knnVector.at<float>(i, b) = j;
				}
			}
		}
	}
	return knnVector;
}

Mat affineMappingMatrix(Mat puzzleDescriptor, vector<KeyPoint> puzzleKeypoints, vector<KeyPoint> sampleKeypoints, Mat KNNP)
{
	printf("---------------------This is another round to find the best Homography matrix---------------------\n");
	int k = 3, kk, nn, inlier = 0, Th = 1, max_inlier = 0;
	float diffVector1, diffVector2, totalDiff, sqrtTotalDiff;
	vector<int> point(3);
	Mat A(6, 6, CV_32F);
	Mat U(6, 1, CV_32F);
	Mat hOther(3, 1, CV_32F);
	Mat best_H(3, 3, CV_32F);
	Mat H(3, 3, CV_32F);
	Mat A2(3, 1, CV_32F);
	Mat A2Sure(3, 1, CV_32F);
	/*When t = 10000, the result is very good for the table, the time is 210s;
	When t = 5000, the result is very good for the table too, the time is 143s;
	When t = 3500, the result is good enough for the table, the time is 129s;
	when t = 2000, the result is good for the first 5 puzzles, but not good for the next 2, the time is 107s;*/
	for (int t = 0; t < 20000; t++){
		do
	    {
			point[0] = rand() % puzzleDescriptor.rows;
			point[1] = rand() % puzzleDescriptor.rows;
			point[2] = rand() % puzzleDescriptor.rows;

		} while ((puzzleKeypoints[point[0]].pt.x == 0 && puzzleKeypoints[point[0]].pt.y == 0) ||
			(puzzleKeypoints[point[1]].pt.x == 0 && puzzleKeypoints[point[1]].pt.y == 0) ||
			(puzzleKeypoints[point[2]].pt.x == 0 && puzzleKeypoints[point[2]].pt.y == 0));

		for (int r = 0, m = 0; r < k, m < k; r = r + 2, m++){
			A.at<float>(r, 0) = puzzleKeypoints[point[m]].pt.x;
			A.at<float>(r, 1) = puzzleKeypoints[point[m]].pt.y;
			A.at<float>(r, 2) = 1;
			A.at<float>(r, 3) = 0;
			A.at<float>(r, 4) = 0;
			A.at<float>(r, 5) = 0;
			A.at<float>(r + 1, 0) = 0;
			A.at<float>(r + 1, 1) = 0;
			A.at<float>(r + 1, 2) = 0;
			A.at<float>(r + 1, 3) = puzzleKeypoints[point[m]].pt.x;
			A.at<float>(r + 1, 4) = puzzleKeypoints[point[m]].pt.y;
			A.at<float>(r + 1, 5) = 1;
			nn = rand() % 3;
			switch (nn)
			{
			case 0:
				kk = 0;
				break;
			case 1:
				kk = 1;
				break;
			case 2:
				kk = 2;
				break;
			}
			U.at<float>(r, 0) = sampleKeypoints[KNNP.at<float>(point[m], kk)].pt.x;
			U.at<float>(r + 1, 0) = sampleKeypoints[KNNP.at<float>(point[m], kk)].pt.y;
		}
		Mat ATRAN = A.t();

		Mat H1 = ((ATRAN*A).inv())*ATRAN*U;
		H.at<float>(0, 0) = H1.at<float>(0, 0);
		H.at<float>(0, 1) = H1.at<float>(1, 0);
		H.at<float>(0, 2) = H1.at<float>(2, 0);
		H.at<float>(1, 0) = H1.at<float>(3, 0);
		H.at<float>(1, 1) = H1.at<float>(4, 0);
		H.at<float>(1, 2) = H1.at<float>(5, 0);
		H.at<float>(2, 0) = 0;
		H.at<float>(2, 1) = 0;
		H.at<float>(2, 2) = 1;
		inlier = 0;
		for (int i = 0; i < puzzleDescriptor.rows; i++){
			if (puzzleKeypoints[i].pt.x != 0 && puzzleKeypoints[i].pt.y != 0){
				hOther.at<float>(0, 0) = puzzleKeypoints[i].pt.x;
				hOther.at<float>(1, 0) = puzzleKeypoints[i].pt.y;
				hOther.at<float>(2, 0) = 1;
				A2 = H*hOther;
				/*This procedure is try to make sure A2(2,0) is definitely be 1*/
				A2Sure = A2 / A2.at<float>(2, 0);
				for (int j = 0; j < k; j++) {
					diffVector1 = A2Sure.at<float>(0, 0) - sampleKeypoints[KNNP.at<float>(i, j)].pt.x;
					diffVector1 = diffVector1*diffVector1;
					diffVector2 = A2Sure.at<float>(1, 0) - sampleKeypoints[KNNP.at<float>(i, j)].pt.y;
					diffVector2 = diffVector2*diffVector2;
					totalDiff = diffVector1 + diffVector2;
					sqrtTotalDiff = sqrt(totalDiff);
					if (sqrtTotalDiff < Th){
						inlier = inlier + 1;
					}
				}
			}
		}
		if (inlier > max_inlier){
			max_inlier = inlier;
			H.copyTo(best_H);
		}
	}
	return best_H;
}

Mat Warping(Mat tmp, Mat best_H, Mat in, Mat Hstot)
{
	Mat H = Hstot*best_H;
	Mat object(3, 1, CV_32F);
	Mat target(3, 1, CV_32F);
	Mat target2(3, 1, CV_32F);
	float green, blue, red;
	for (int z = 0; z < tmp.rows; z++)
	{
		for (int x = 0; x < tmp.cols; x++)
		{
			green = tmp.at<Vec3b>(z, x)[0];
			blue = tmp.at<Vec3b>(z, x)[1];
			red = tmp.at<Vec3b>(z, x)[2];
			if (green != 255 && blue != 255 && red != 255)
			{
				object.at<float>(0, 0) = x;
				object.at<float>(1, 0) = z;
				object.at<float>(2, 0) = 1;
				target = H*object;
				target2 = target / target.at<float>(2, 0);

				if (0 <= target2.at<float>(1, 0) && target2.at<float>(1, 0) < in.rows && 0 <= target2.at<float>(0, 0) && target2.at<float>(0, 0) < in.cols)
				{
					//cout << target2.t() << endl;
					in.at<Vec3b>((int)target2.at<float>(1, 0), (int)target2.at<float>(0, 0))[0] = green;
					in.at<Vec3b>((int)target2.at<float>(1, 0), (int)target2.at<float>(0, 0))[1] = blue;
					in.at<Vec3b>((int)target2.at<float>(1, 0), (int)target2.at<float>(0, 0))[2] = red;
				}
			}
		}
	}
	return in;
}

Mat backwardWarping(Mat puzzle, Mat best_H, Mat sample, Mat homographyM)
{
	Mat H = homographyM*best_H;
	Mat hInv = H.inv();
	Mat samO(3, 1, CV_32F);
	Mat puzzP(3, 1, CV_32F);
	Mat puzzleP2(3, 1, CV_32F);
	//cout << "-----===-----" << H << endl;
	//cout << "-------=-=-=-121314325=-" << hInv << endl;
	/*Find that when I use SURF method, the sample is all zero */
	//cout << "---------------------------" <<sample << endl;
	for (int i = 0; i < sample.rows; i++)
	{
		for (int j = 0; j < sample.cols; j++)
		{
			samO.at<float>(0, 0) = j;
			samO.at<float>(1, 0) = i;
			samO.at<float>(2, 0) = 1;
			puzzP = hInv*samO;
			/* puzzleP2 is in order to make sure that puzzP is 1*/
			puzzleP2 = puzzP / puzzP.at<float>(2, 0);
			if (0 <= puzzleP2.at<float>(1, 0) && puzzleP2.at<float>(1, 0) < puzzle.rows && 0 <= puzzleP2.at<float>(0, 0) && puzzleP2.at<float>(0, 0) < puzzle.cols)
			{
				if (puzzle.at<Vec3b>((int)puzzleP2.at<float>(1, 0), (int)puzzleP2.at<float>(0, 0))[0] != 0 && puzzle.at<Vec3b>((int)puzzleP2.at<float>(1, 0), (int)puzzleP2.at<float>(0, 0))[1] != 0 && puzzle.at<Vec3b>((int)puzzleP2.at<float>(1, 0), (int)puzzleP2.at<float>(0, 0))[2] != 0) {
					sample.at<Vec3b>(i, j)[0] = puzzle.at<Vec3b>((int)puzzleP2.at<float>(1, 0), (int)puzzleP2.at<float>(0, 0))[0];//g
					sample.at<Vec3b>(i, j)[1] = puzzle.at<Vec3b>((int)puzzleP2.at<float>(1, 0), (int)puzzleP2.at<float>(0, 0))[1];//r
					sample.at<Vec3b>(i, j)[2] = puzzle.at<Vec3b>((int)puzzleP2.at<float>(1, 0), (int)puzzleP2.at<float>(0, 0))[2];//b
				}
			}
		}
	}
	return sample;
}

Mat mappingPicture(Mat puzzleDescriptor, vector<KeyPoint> puzzleKeypoint, Mat sampleDescriptor, vector<KeyPoint> sampleKeypoint, Mat homographyM, Mat temp, Mat sample)
{
	Mat knnVector = KNN(puzzleDescriptor, sampleDescriptor);
	Mat best_H(3, 3, CV_32F);
	best_H = affineMappingMatrix(puzzleDescriptor, puzzleKeypoint, sampleKeypoint, knnVector);
	//best_H = ComputeH()
	//best_H = Homography_Ransac(puzzleDescriptor, puzzleKeypoint, sampleKeypoint, knnVector);
	//Mat mapping_picture = backward_warping(temp, best_H, sample);
	Mat mappingPicture = backwardWarping(temp, best_H, sample, homographyM);
	//Mat mappingPicture = Warping(temp, best_H, sample, homographyM);
	//cout << "---------------------------------" << mappingPicture << endl;
	return mappingPicture;
}

Mat goodHomography(Mat puzzleDescriptor, vector<KeyPoint> puzzleKeypoint, Mat sampleDescriptor, vector<KeyPoint> sampleKeypoint)
{
	Mat knnVector = KNN(puzzleDescriptor, sampleDescriptor);
	Mat best_H(3, 3, CV_32F);
	//best_H = ComputeH( Mat )
	best_H = affineMappingMatrix(puzzleDescriptor, puzzleKeypoint, sampleKeypoint, knnVector);
	//best_H = Homography_Ransac(puzzleDescriptor, puzzleKeypoint, sampleKeypoint, knnVector);
	//cout << "--------------------------------" << best_H << endl;
	return best_H;
}

int main() {
	// Start time
	time_t startTime = time(NULL);
	/*-------------------------------------This is the part for the method 1's test table's result begin-------------------------------------*/
	//Mat puzzle1 = imread("test/table/puzzle1.bmp", 1);
	//Mat puzzle1Descriptor = siftDescriptorGet("test/table/puzzle1.bmp");
	//vector<KeyPoint> puzzle1Keypoints = siftKeypoint("test/table/puzzle1.bmp");
	///*Mat puzzle1Descriptor = surfDescriptorGet("test/table/puzzle1.bmp");
	//vector<KeyPoint> puzzle1Keypoints = surfKeypoint("test/table/puzzle1.bmp");*/

	//Mat puzzle2 = imread("test/table/puzzle2.bmp", 1);
	//Mat puzzle2Descriptor = siftDescriptorGet("test/table/puzzle2.bmp");
	//vector<KeyPoint> puzzle2Keypoints = siftKeypoint("test/table/puzzle2.bmp");
	////Mat puzzle2Descriptor = surfDescriptorGet("test/table/puzzle2.bmp");
	////vector<KeyPoint> puzzle2Keypoints = surfKeypoint("test/table/puzzle2.bmp");

	//Mat puzzle3 = imread("test/table/puzzle3.bmp", 1);
	//Mat puzzle3Descriptor = siftDescriptorGet("test/table/puzzle3.bmp");
	//vector<KeyPoint> puzzle3Keypoints = siftKeypoint("test/table/puzzle3.bmp");
	////Mat puzzle3Descriptor = surfDescriptorGet("test/table/puzzle3.bmp");
	////vector<KeyPoint> puzzle3Keypoints = surfKeypoint("test/table/puzzle3.bmp");

	//Mat puzzle4 = imread("test/table/puzzle4.bmp", 1);
	//Mat puzzle4Descriptor = siftDescriptorGet("test/table/puzzle4.bmp");
	//vector<KeyPoint> puzzle4Keypoints = siftKeypoint("test/table/puzzle4.bmp");
	////Mat puzzle4Descriptor = surfDescriptorGet("test/table/puzzle4.bmp");
	////vector<KeyPoint> puzzle4Keypoints = surfKeypoint("test/table/puzzle4.bmp");

	//Mat puzzle5 = imread("test/table/puzzle5.bmp", 1);
	//Mat puzzle5Descriptor = siftDescriptorGet("test/table/puzzle5.bmp");
	//vector<KeyPoint> puzzle5Keypoints = siftKeypoint("test/table/puzzle5.bmp");
	////Mat puzzle5Descriptor = surfDescriptorGet("test/table/puzzle5.bmp");
	////vector<KeyPoint> puzzle5Keypoints = surfKeypoint("test/table/puzzle5.bmp");

	//Mat puzzle6 = imread("test/table/puzzle6.bmp", 1);
	//Mat puzzle6Descriptor = siftDescriptorGet("test/table/puzzle6.bmp");
	//vector<KeyPoint> puzzle6Keypoints = siftKeypoint("test/table/puzzle6.bmp");
	////Mat puzzle6Descriptor = surfDescriptorGet("test/table/puzzle6.bmp");
	////vector<KeyPoint> puzzle6Keypoints = surfKeypoint("test/table/puzzle6.bmp");

	//Mat puzzle7 = imread("test/table/puzzle7.bmp", 1);
	//Mat puzzle7Descriptor = siftDescriptorGet("test/table/puzzle7.bmp");
	//vector<KeyPoint> puzzle7Keypoints = siftKeypoint("test/table/puzzle7.bmp");
	////Mat puzzle7Descriptor = surfDescriptorGet("test/table/puzzle7.bmp");
	////vector<KeyPoint> puzzle7Keypoints = surfKeypoint("test/table/puzzle7.bmp");

	//Mat sample = imread("test/table/sample.bmp", 1);
	//Mat sampleDescriptor = siftDescriptorGet("test/table/sample.bmp");
	//vector<KeyPoint> sampleKeypoints = siftKeypoint("test/table/sample.bmp");
	////Mat sampleDescriptor = surfDescriptorGet("test/table/sample.bmp");
	////vector<KeyPoint> sampleKeypoints = surfKeypoint("test/table/sample.bmp");

	//Mat target = imread("test/table/target.bmp", 1);
	//Mat targetDescriptor = siftDescriptorGet("test/table/target.bmp");
	//vector<KeyPoint> targetKeypoints = siftKeypoint("test/table/target.bmp");
	////Mat targetDescriptor = surfDescriptorGet("test/table/target.bmp");
	////vector<KeyPoint> targetKeypoints = surfKeypoint("test/table/target.bmp");

	//Mat homographyM = goodHomography(sampleDescriptor, sampleKeypoints, targetDescriptor, targetKeypoints);
	//
	//// I just use 7 steps to map the puzzle(i) picture to the sample picture 
	//Mat step1 = mappingPicture(puzzle1Descriptor, puzzle1Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle1, target);
	////cout << "-----------------------------------" << step1 << "-----------------------------------" << endl;
	//imwrite("resultImageOfStep1.bmp", step1);
	////namedWindow("resultImageOfStep1", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep1.bmp", step1);

	//Mat step2 = mappingPicture(puzzle2Descriptor, puzzle2Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle2, target);
	//imwrite("resultImageOfStep2.bmp", step2);
	////namedWindow("resultImageOfStep2", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep2.bmp", step2);

	//Mat step3 = mappingPicture(puzzle3Descriptor, puzzle3Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle3, target);
	//imwrite("resultImageOfStep3.bmp", step3);
	////namedWindow("resultImageOfStep3", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep3.bmp", step3);

	//Mat step4 = mappingPicture(puzzle4Descriptor, puzzle4Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle4, target);
	//imwrite("resultImageOfStep4.bmp", step4);
	////namedWindow("resultImageOfStep4", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep4.bmp", step4);

	//Mat step5 = mappingPicture(puzzle5Descriptor, puzzle5Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle5, target);
	//imwrite("resultImageOfStep5.bmp", step5);
	////namedWindow("resultImageOfStep5", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep5.bmp", step5);

	//Mat step6 = mappingPicture(puzzle6Descriptor, puzzle6Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle6, target);
	//imwrite("resultImageOfStep6.bmp", step6);
	////namedWindow("resultImageOfStep6", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep6.bmp", step6);

	//Mat step7 = mappingPicture(puzzle7Descriptor, puzzle7Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle7, target);
	//imwrite("resultImageOfTestTable.bmp", step7);
	////namedWindow("resultImageOfTestTable", CV_WINDOW_AUTOSIZE);
	//imshow("resultImageOfTestTable.bmp", step7);
	/*-------------------------------------This is the part for the test table's result end-------------------------------------*/
	/*-------------------------------------This is the part for the test logo's result begin-------------------------------------*/
	Mat puzzle1 = imread("test/logo/puzzle1.bmp", 1);
	Mat puzzle1Descriptor = siftDescriptorGet("test/logo/puzzle1.bmp");
	vector<KeyPoint> puzzle1Keypoints = siftKeypoint("test/logo/puzzle1.bmp");
	//Mat puzzle1Descriptor = surfDescriptorGet("test/logo/puzzle1.bmp");
	//vector<KeyPoint> puzzle1Keypoints = surfKeypoint("test/logo/puzzle1.bmp");

	Mat puzzle2 = imread("test/logo/puzzle2.bmp", 1);
	Mat puzzle2Descriptor = siftDescriptorGet("test/logo/puzzle2.bmp");
	vector<KeyPoint> puzzle2Keypoints = siftKeypoint("test/logo/puzzle2.bmp");
	//Mat puzzle2Descriptor = surfDescriptorGet("test/logo/puzzle2.bmp");
	//vector<KeyPoint> puzzle2Keypoints = surfKeypoint("test/logo/puzzle2.bmp");

	Mat puzzle3 = imread("test/logo/puzzle3.bmp", 1);
	Mat puzzle3Descriptor = siftDescriptorGet("test/logo/puzzle3.bmp");
	vector<KeyPoint> puzzle3Keypoints = siftKeypoint("test/logo/puzzle3.bmp");
	//Mat puzzle3Descriptor = surfDescriptorGet("test/logo/puzzle3.bmp");
	//vector<KeyPoint> puzzle3Keypoints = surfKeypoint("test/logo/puzzle3.bmp");

	Mat puzzle4 = imread("test/logo/puzzle4.bmp", 1);
	Mat puzzle4Descriptor = siftDescriptorGet("test/logo/puzzle4.bmp");
	vector<KeyPoint> puzzle4Keypoints = siftKeypoint("test/logo/puzzle4.bmp");
	//Mat puzzle4Descriptor = surfDescriptorGet("test/logo/puzzle4.bmp");
	//vector<KeyPoint> puzzle4Keypoints = surfKeypoint("test/logo/puzzle4.bmp");

	Mat sample = imread("test/logo/sample.bmp", 1);
	Mat sampleDescriptor = siftDescriptorGet("test/logo/sample.bmp");
	vector<KeyPoint> sampleKeypoints = siftKeypoint("test/logo/sample.bmp");
	//Mat sampleDescriptor = surfDescriptorGet("test/logo/sample.bmp");
	//vector<KeyPoint> sampleKeypoints = surfKeypoint("test/logo/sample.bmp");

	Mat target = imread("test/logo/target.bmp", 1);
	Mat targetDescriptor = siftDescriptorGet("test/logo/target.bmp");
	vector<KeyPoint> targetKeypoints = siftKeypoint("test/logo/target.bmp");
	//Mat targetDescriptor = surfDescriptorGet("test/logo/target.bmp");
	//vector<KeyPoint> targetKeypoints = surfKeypoint("test/logo/target.bmp");

	Mat homographyM = goodHomography(sampleDescriptor, sampleKeypoints, targetDescriptor, targetKeypoints);
	//cout << "sakjggksahdhjafgdfkshjagfdja" << homographyM << "kjshALKUGDIYFSGUYafksngADFSHGd" << endl;
	// I just use 4 steps to map the puzzle(i) picture to the sample picture 
	Mat step1 = mappingPicture(puzzle1Descriptor, puzzle1Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle1, target);
	imwrite("resultImageOfStep1.bmp", step1);
	//namedWindow("resultImageOfStep1", CV_WINDOW_AUTOSIZE);
	//imshow("resultImageOfStep1.bmp", step1);

	Mat step2 = mappingPicture(puzzle2Descriptor, puzzle2Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle2, target);
	imwrite("resultImageOfStep2.bmp", step2);
	//namedWindow("resultImageOfStep2", CV_WINDOW_AUTOSIZE);
	//imshow("resultImageOfStep2.bmp", step2);

	Mat step3 = mappingPicture(puzzle3Descriptor, puzzle3Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle3, target);
	imwrite("resultImageOfStep3.bmp", step3);
	//namedWindow("resultImageOfStep3", CV_WINDOW_AUTOSIZE);
	//imshow("resultImageOfStep3.bmp", step3);

	Mat step4 = mappingPicture(puzzle4Descriptor, puzzle4Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle4, target);
	imwrite("resultImageOfTestTable.bmp", step4);
	//namedWindow("resultImageOfTestTable", CV_WINDOW_AUTOSIZE);
	imshow("resultImageOfTestTable.bmp", step4);
	/*-------------------------------------This is the part for the test logo's result end-------------------------------------*/
   /*-------------------------------------This is the part for the dataset's result begin-------------------------------------*/
	//Mat puzzle1 = imread("test/dataset/puzzle1.bmp", 1);
	//Mat puzzle1Descriptor = siftDescriptorGet("test/dataset/puzzle1.bmp");
	//vector<KeyPoint> puzzle1Keypoints = siftKeypoint("test/dataset/puzzle1.bmp");
	///*Mat puzzle1Descriptor = surfDescriptorGet("test/dataset/puzzle1.bmp");
	//vector<KeyPoint> puzzle1Keypoints = surfKeypoint("test/dataset/puzzle1.bmp");*/

	//Mat puzzle2 = imread("test/dataset/puzzle2.bmp", 1);
	//Mat puzzle2Descriptor = siftDescriptorGet("test/dataset/puzzle2.bmp");
	//vector<KeyPoint> puzzle2Keypoints = siftKeypoint("test/dataset/puzzle2.bmp");
	////Mat puzzle2Descriptor = surfDescriptorGet("test/dataset/puzzle2.bmp");
	////vector<KeyPoint> puzzle2Keypoints = surfKeypoint("test/dataset/puzzle2.bmp");

	//Mat puzzle3 = imread("test/dataset/puzzle3.bmp", 1);
	//Mat puzzle3Descriptor = siftDescriptorGet("test/dataset/puzzle3.bmp");
	//vector<KeyPoint> puzzle3Keypoints = siftKeypoint("test/dataset/puzzle3.bmp");
	////Mat puzzle3Descriptor = surfDescriptorGet("test/dataset/puzzle3.bmp");
	////vector<KeyPoint> puzzle3Keypoints = surfKeypoint("test/dataset/puzzle3.bmp");

	//Mat puzzle4 = imread("test/dataset/puzzle4.bmp", 1);
	//Mat puzzle4Descriptor = siftDescriptorGet("test/dataset/puzzle4.bmp");
	//vector<KeyPoint> puzzle4Keypoints = siftKeypoint("test/dataset/puzzle4.bmp");
	////Mat puzzle4Descriptor = surfDescriptorGet("test/dataset/puzzle4.bmp");
	////vector<KeyPoint> puzzle4Keypoints = surfKeypoint("test/dataset/puzzle4.bmp");

	//Mat puzzle5 = imread("test/dataset/puzzle5.bmp", 1);
	//Mat puzzle5Descriptor = siftDescriptorGet("test/dataset/puzzle5.bmp");
	//vector<KeyPoint> puzzle5Keypoints = siftKeypoint("test/dataset/puzzle5.bmp");
	////Mat puzzle5Descriptor = surfDescriptorGet("test/dataset/puzzle5.bmp");
	////vector<KeyPoint> puzzle5Keypoints = surfKeypoint("test/dataset/puzzle5.bmp");

	//Mat puzzle6 = imread("test/dataset/puzzle6.bmp", 1);
	//Mat puzzle6Descriptor = siftDescriptorGet("test/dataset/puzzle6.bmp");
	//vector<KeyPoint> puzzle6Keypoints = siftKeypoint("test/dataset/puzzle6.bmp");
	////Mat puzzle6Descriptor = surfDescriptorGet("test/dataset/puzzle6.bmp");
	////vector<KeyPoint> puzzle6Keypoints = surfKeypoint("test/dataset/puzzle6.bmp");

	//Mat sample = imread("test/dataset/sample.bmp", 1);
	//Mat sampleDescriptor = siftDescriptorGet("test/dataset/sample.bmp");
	//vector<KeyPoint> sampleKeypoints = siftKeypoint("test/dataset/sample.bmp");
	////Mat sampleDescriptor = surfDescriptorGet("test/dataset/sample.bmp");
	////vector<KeyPoint> sampleKeypoints = surfKeypoint("test/dataset/sample.bmp");

	//Mat target = imread("test/dataset/target.bmp", 1);
	//Mat targetDescriptor = siftDescriptorGet("test/dataset/target.bmp");
	//vector<KeyPoint> targetKeypoints = siftKeypoint("test/dataset/target.bmp");
	////Mat targetDescriptor = surfDescriptorGet("test/dataset/target.bmp");
	////vector<KeyPoint> targetKeypoints = surfKeypoint("test/dataset/target.bmp");

	//Mat homographyM = goodHomography(sampleDescriptor, sampleKeypoints, targetDescriptor, targetKeypoints);
	//
	//// I just use 6 steps to map the puzzle(i) picture to the sample picture 
	//Mat step1 = mappingPicture(puzzle1Descriptor, puzzle1Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle1, target);
	////cout << "-----------------------------------" << step1 << "-----------------------------------" << endl;
	//imwrite("resultImageOfStep1.bmp", step1);
	////namedWindow("resultImageOfStep1", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep1.bmp", step1);

	//Mat step2 = mappingPicture(puzzle2Descriptor, puzzle2Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle2, target);
	//imwrite("resultImageOfStep2.bmp", step2);
	////namedWindow("resultImageOfStep2", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep2.bmp", step2);

	//Mat step3 = mappingPicture(puzzle3Descriptor, puzzle3Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle3, target);
	//imwrite("resultImageOfStep3.bmp", step3);
	////namedWindow("resultImageOfStep3", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep3.bmp", step3);

	//Mat step4 = mappingPicture(puzzle4Descriptor, puzzle4Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle4, target);
	//imwrite("resultImageOfStep4.bmp", step4);
	////namedWindow("resultImageOfStep4", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep4.bmp", step4);

	//Mat step5 = mappingPicture(puzzle5Descriptor, puzzle5Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle5, target);
	//imwrite("resultImageOfStep5.bmp", step5);
	////namedWindow("resultImageOfStep5", CV_WINDOW_AUTOSIZE);
	////imshow("resultImageOfStep5.bmp", step5);

	//Mat step6 = mappingPicture(puzzle6Descriptor, puzzle6Keypoints, sampleDescriptor, sampleKeypoints, homographyM, puzzle6, target);
	//imwrite("resultImageOfTestDataSet.bmp", step6);
	////namedWindow("resultImageOfTestTable", CV_WINDOW_AUTOSIZE);
	//imshow("resultImageOfTestDataSet.bmp", step6);
	/*-------------------------------------This is the part for the test dataset's result end-------------------------------------*/
    // End time
	time_t endTime = time(NULL);
	cout << "time: " << endTime - startTime << " s" << endl;
	waitKey();
	return EXIT_SUCCESS;
}
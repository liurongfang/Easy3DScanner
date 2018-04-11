// cvCalibrateProCam.cpp: Functions for projector-camera calibration.
//
// Overview:
//   This file implements the functions for projector-camera calibration. Camera calibration is
//   implemented using basic OpenCV functions for chessboard detection and homography-based
//   calibration, following the method of Zhang [ICCV 2009]. Projector calibration is performed
//   using a two-image sequence. First, a physical (i.e., printed) chessboard pattern is
//   automatically detected. Afterwards, a virtual chessboard is projected. The first image is
//   subtracted from the second. Once again, the built-in OpenCV functions for chessboard 
//   detection are used to find the virtual chessboard corners. A prior camera calibration
//   allows 3D coordinates of each virtual chessboard corner to be reconstructed. Finally, the
//   complete set of 2D-to-3D correspondences, mapping 2D projector pixels to 3D points on the
//   calibration plane, is used to evaluate the intrinsic projector calibration. A single 
//   image pair, comprising a static printed chessboard with a projected virtual chessboard
//   visible on its surface, is used to estimate the projector-camera extrinsic calibration.
//
// Details:
//   Please read the SIGGRAPH 2009 course notes for additional details.
//
//     Douglas Lanman and Gabriel Taubin
//     "Build Your Own 3D Scanner: 3D Photography for Beginners"
//     ACM SIGGRAPH 2009 Course Notes
//
// Author:
//   Douglas Lanman
//   Brown University
//   July 2009

#include "stdafx.h"
#include "cvStructuredLight.h"
#include "cvCalibrateProCam.h"
#include "cvUtilProCam.h"
#include "MX500IPL.h"


extern tDSImageSize m_dsImg;


// Display the camera calibration results to the console. 显示摄像机标定结果到控制台。
void displayCamCalib(struct slCalib* sl_calib){
	printf("Camera calibration:\n");
	if(sl_calib->cam_intrinsic_calib){
		printf("+ Intrinsic parameters = \n");//显示摄像机内部参数矩阵
		for(int i=0; i<3; i++){
			printf("   ");
			for(int j=0; j<3; j++)
				printf("%7.3f ", cvmGet(sl_calib->cam_intrinsic, i, j));
			printf("\n");
		}
		printf("+ Distortion coefficients = \n   ");//显示摄像机4个畸变参数
		for(int i=0; i<5; i++)
			printf("%7.3f ", cvmGet(sl_calib->cam_distortion, i, 0));
		printf("\n");
	}
	else
		printf("+ Camera has not been calibrated!\n");
}

// Display the projector calibration results to the console.  显示投影仪标定结果到控制台。
void displayProjCalib(struct slCalib* sl_calib){
	printf("Projector calibration:\n");
	if(sl_calib->proj_intrinsic_calib){
		printf("+ Intrinsic parameters = \n");//显示投影仪内部参数矩阵
		for(int i=0; i<3; i++){
			printf("   ");
			for(int j=0; j<3; j++)
				printf("%7.3f ", cvmGet(sl_calib->proj_intrinsic, i, j));
			printf("\n");
		}
		printf("+ Distortion coefficients = \n   ");
		for(int i=0; i<5; i++)
			printf("%7.3f ", cvmGet(sl_calib->proj_distortion, i, 0));//显示投影仪4个畸变参数
		printf("\n");
	}
	else
		printf("+ Projector has not been calibrated!\n");
}

// Generate a chessboard pattern for projector calibration.生成一个投影仪标定棋盘格。
int generateChessboard(struct slParams* sl_params, IplImage*& board, int& border_cols, int& border_rows){

	// Calculate chessboard border.计算棋盘边界。
	border_cols = (int)floor((board->width -(sl_params->proj_board_w+1)*sl_params->proj_board_w_pixels)/2.0);
	border_rows = (int)floor((board->height-(sl_params->proj_board_h+1)*sl_params->proj_board_h_pixels)/2.0);

	// Check for chessboard errors.检查棋盘错误。
	if( (border_cols < 0) || (border_rows < 0) ){
		printf("ERROR: Cannot create chessboard with user-requested dimensions!\n");
		return -1;
	}

	// Initialize chessboard with white image.初始化棋盘与白色的图像。
	cvSet(board, cvScalar(255));

	// Create odd black squares.创建奇数的黑色方块。
	uchar* data = (uchar*)board->imageData;
	int step = board->widthStep/sizeof(uchar);
	for(int r=0; r<(sl_params->proj_board_h+1); r+=2)
		for(int c=0; c<(sl_params->proj_board_w+1); c+=2)
			for(int i=(r*sl_params->proj_board_h_pixels+border_rows); 
				i<((r+1)*sl_params->proj_board_h_pixels+border_rows); i++)
				for(int j=(c*sl_params->proj_board_w_pixels+border_cols); 
					j<((c+1)*sl_params->proj_board_w_pixels+border_cols); j++)
					data[i*step+j] = 0;

	// Create even black squares. 创建偶数的黑色方块。
	for(int r=1; r<sl_params->proj_board_h+1; r+=2)
		for(int c=1; c<sl_params->proj_board_w+1; c+=2)
			for(int i=(r*sl_params->proj_board_h_pixels+border_rows); 
				i<((r+1)*sl_params->proj_board_h_pixels+border_rows); i++)
				for(int j=(c*sl_params->proj_board_w_pixels+border_cols); 
					j<((c+1)*sl_params->proj_board_w_pixels+border_cols); j++)
					data[i*step+j] = 0;

	// Return without errors.返回没有错误
	return 0;
}

// Detect chessboard corners (with subpixel refinement).检测棋盘角(亚像素细分)。
// Note: Returns 1 if chessboard is found, 0 otherwise.发现棋盘返回1，否则返回0
int detectChessboard(IplImage* frame, CvSize board_size,
                     CvPoint2D32f* corners,
                     int* corner_count){

	// Find chessboard corners.发现棋盘角点
	int found = cvFindChessboardCorners(
		frame, board_size, corners, corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);//粗略得到角点坐标

	// Refine chessboard corners.提取棋盘角点
	IplImage* gray_frame = cvCreateImage(cvGetSize(frame), frame->depth, 1);
	if(frame->nChannels > 1)
		cvCvtColor(frame, gray_frame, CV_BGR2GRAY);
	else
		cvCopyImage(frame, gray_frame);
	cvFindCornerSubPix(gray_frame, corners, *corner_count, 
		cvSize(11,11), cvSize(-1,-1), 
		cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));//精确得到角点坐标

	// Release allocated resources.释放分配资源
	cvReleaseImage(&gray_frame);

	// Return without errors (i.e., chessboard was found).	返回没有错误（角点被发现）					 
	return found;
}

// Run camera calibration.//运行摄像机标定
int runCameraCalibration(
						 struct slParams* sl_params, 
					     struct slCalib* sl_calib){

	// Reset camera calibration status (will be set again, if successful).重置摄像机标定状态
	sl_calib->cam_intrinsic_calib = false;

	// Create camera calibration directory (clear previous calibration first).创建摄像机标定目录（清除以前的标定）
	printf("Creating camera calibration directory (overwrites existing data)...\n");
	char str[1024], calibDir[1024];
	sprintf(calibDir, "%s\\calib\\cam", sl_params->outdir);
	sprintf(str, "%s\\calib", sl_params->outdir);
	_mkdir(str);
	_mkdir(calibDir);
	sprintf(str, "rd /s /q \"%s\"", calibDir);
	system(str);
	if(_mkdir(calibDir) != 0){
		printf("ERROR: Cannot open output directory!\n");
		printf("Camera calibration was not successful and must be repeated.\n");
		return -1;
	}

	// Prompt user for maximum number of calibration boards.提示用户标定板的最大数量。
	printf("Enter the maximum number of calibration images, then press return.\n");
	printf("+ Maximum number of images = ");
	int n_boards;
	scanf("%d", &n_boards);
	if(n_boards<2){
		printf("ERROR: At least two images are required!\n");
	    printf("Camera calibration was not successful and must be repeated.\n");
		return -1;
	}

	// Evaluate derived chessboard parameters and allocate storage.测量导出棋盘参数和分配存储。
	int board_n            = sl_params->cam_board_w*sl_params->cam_board_h;
	CvSize board_size      = cvSize(sl_params->cam_board_w, sl_params->cam_board_h); /* 定标板上每行、列的角点数 标定板尺寸*/
	CvMat* image_points    = cvCreateMat(n_boards*board_n, 2, CV_32FC1);/* 保存定标板的二维坐标 */
    CvMat* object_points   = cvCreateMat(n_boards*board_n, 3, CV_32FC1); /* 保存定标板的三维坐标 */
    CvMat* point_counts    = cvCreateMat(n_boards, 1, CV_32SC1);/* 每幅图像中角点的数量 */
	IplImage** calibImages = new IplImage* [n_boards];

	// Initialize capture and allocate storage.初始化获取和分配储存
	printf("Press 'n' (in 'camWindow') to capture next image, or 'ESC' to quit.\n");
	//IplImage* frame = cvQueryFrame2(capture, sl_params);
	IplImage *frame = NULL;
	QueryFrameMX500(&frame);
	CvSize frame_size = cvGetSize(frame);
	for(int i=0; i<n_boards; i++)
		calibImages[i] = cvCreateImage(cvSize(m_dsImg.iWidth, m_dsImg.iHeight), IPL_DEPTH_8U, 3);

	// Create a window to display captured frames.创建一个窗口去显示获取图像
	cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("Cam. Gain", "camWindow", &sl_params->cam_gain, 100, NULL);
	HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
	BringWindowToTop(camWindow);
	//cvWaitKey(1);

	// Capture live image stream, until "ESC" is pressed or calibration is complete.获取动态图像，直到按“ESC”或标定完成
	int successes = 0;
	bool captureFrame = false;
	while(successes < n_boards){

		// Get next available frame.得到下一张有效的图像
		//frame = cvQueryFrame2(capture, sl_params);
	    QueryFrameMX500(&frame);
		cvScale(frame, frame, 2.*(sl_params->cam_gain/100.), 0);

		// Find chessboard corners.发现棋盘角点
		CvPoint2D32f* corners = new CvPoint2D32f[board_n]; /* 缓存每幅图像上检测到的角点 */
		int corner_count;
		int found = detectChessboard(frame, board_size, corners, &corner_count);

		// If chessboard is detected, then add points to calibration list.如果检测到棋盘，添加角点到标定列表
		/* 将角点的存储结构转换成矩阵形式 */
		if(captureFrame & (corner_count == board_n)){
			for(int i=successes*board_n, j=0; j<board_n; ++i,++j){
			    CV_MAT_ELEM(*image_points,  float, i, 0) = corners[j].x;
				CV_MAT_ELEM(*image_points,  float, i, 1) = corners[j].y;
				CV_MAT_ELEM(*object_points, float, i, 0) = sl_params->cam_board_w_mm*float(j/sl_params->cam_board_w);
				CV_MAT_ELEM(*object_points, float, i, 1) = sl_params->cam_board_h_mm*float(j%sl_params->cam_board_w);
				CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
			}
			CV_MAT_ELEM(*point_counts, int, successes, 0) = board_n;
			cvCopyImage(frame, calibImages[successes]);
			successes++;
			printf("+ Captured frame %d of %d.\n", successes, n_boards);
			captureFrame = false;
		}

		// Display frame.显示图像
		cvDrawChessboardCorners(frame, board_size, corners, corner_count, found);
	    cvShowImageResampled("camWindow", frame, sl_params->window_w, sl_params->window_h);

		// Free allocated resources.清除分配资源
		delete[] corners;

		// Process user input.进行用户输入
		int cvKey = cvWaitKey(10);
		if(cvKey==27)
			break;
		else if(cvKey=='n')//按“n”获取下一张图像
			captureFrame = true;
	}

	// Close the display window.关闭显示窗口
	cvDestroyWindow("camWindow");

	// Calibrate camera, if minimum number of frames are available.标定摄像机，如果图像的最小数量有效
	if(successes >= 2){

		// Allocate calibration matrices.分配标定矩阵
		CvMat* object_points2      = cvCreateMat(successes*board_n, 3, CV_32FC1);/* 保存定标板上角点的三维坐标 */
		CvMat* image_points2       = cvCreateMat(successes*board_n, 2, CV_32FC1);/* 保存定标板上角点的二维坐标 */
		CvMat* point_counts2       = cvCreateMat(successes, 1, CV_32SC1); /* 每幅图像中角点的数量 */
		CvMat* rotation_vectors    = cvCreateMat(successes, 3, CV_32FC1); /* 每幅图像的旋转向量 */
  	    CvMat* translation_vectors = cvCreateMat(successes, 3, CV_32FC1); /* 每幅图像的平移向量 */

		// Transfer calibration data from captured values.从获取值里转换标定数据
		for(int i=0; i<successes*board_n; ++i){
			CV_MAT_ELEM(*image_points2,  float, i, 0) = CV_MAT_ELEM(*image_points,  float, i, 0);
			CV_MAT_ELEM(*image_points2,  float, i, 1) = CV_MAT_ELEM(*image_points,  float, i, 1);
			CV_MAT_ELEM(*object_points2, float, i, 0) =	CV_MAT_ELEM(*object_points, float, i, 0);
			CV_MAT_ELEM(*object_points2, float, i, 1) =	CV_MAT_ELEM(*object_points, float, i, 1);
			CV_MAT_ELEM(*object_points2, float, i, 2) = CV_MAT_ELEM(*object_points, float, i, 2);
		}
		for(int i=0; i<successes; ++i)
			CV_MAT_ELEM(*point_counts2, int, i, 0) = CV_MAT_ELEM(*point_counts, int, i, 0);

		// Calibrate the camera and save calibration parameters.标定摄像机并储存标定参数
		printf("Calibrating camera...\n");
		int calib_flags = 0;
		if(!sl_params->cam_dist_model[0])
			calib_flags |= CV_CALIB_ZERO_TANGENT_DIST;
		if(!sl_params->cam_dist_model[1]){
			cvmSet(sl_calib->cam_distortion, 4, 0, 0);
			calib_flags |= CV_CALIB_FIX_K3;
		}
		cvCalibrateCamera2(
			object_points2, image_points2, point_counts2, frame_size, 
			sl_calib->cam_intrinsic, sl_calib->cam_distortion,
			rotation_vectors, translation_vectors, calib_flags);
		printf("Saving calibration images and parameters...\n");
		CvMat* R = cvCreateMat(3, 3, CV_32FC1);/* 保存每幅图像的旋转矩阵 */
		CvMat* r = cvCreateMat(1, 3, CV_32FC1);/* 保存每幅图像的平移矩阵 */
		for(int i=0; i<successes; ++i){
			sprintf(str,"%s\\%0.2d.png", calibDir, i);
			cvSaveImage(str, calibImages[i]);
			cvGetRow(rotation_vectors, r, i);
			cvRodrigues2(r, R, NULL);
			sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
			//cvSave(str, R);
		}
		sprintf(str,"%s\\cam_intrinsic.xml", calibDir);	
		cvSave(str, sl_calib->cam_intrinsic);
		sprintf(str,"%s\\cam_distortion.xml", calibDir);
		cvSave(str, sl_calib->cam_distortion);
		sprintf(str,"%s\\cam_rotation_vectors.xml", calibDir);
		cvSave(str, rotation_vectors);
		sprintf(str,"%s\\cam_translation_vectors.xml", calibDir);
		cvSave(str, translation_vectors);
		sprintf(str,"%s\\config.xml", calibDir);
		writeConfiguration(str, sl_params);

		// Release allocated resources.释放分配资源
		cvReleaseMat(&object_points2);
		cvReleaseMat(&image_points2);
		cvReleaseMat(&point_counts2);
		cvReleaseMat(&rotation_vectors);
  	    cvReleaseMat(&translation_vectors);
		cvReleaseMat(&R);
		cvReleaseMat(&r);
	}
	else{
		printf("ERROR: At least two detected chessboards are required!\n");
	    printf("Camera calibration was not successful and must be repeated.\n");
		return -1;
	}

	// Free allocated resources.清空分配资源
	cvReleaseMat(&image_points);
    cvReleaseMat(&object_points);
    cvReleaseMat(&point_counts);
	for(int i=0; i<n_boards; i++)
		cvReleaseImage(&calibImages[i]);
	delete[] calibImages;

	// Set camera calibration status.释放摄像机标定状态
	sl_calib->cam_intrinsic_calib = true;

	// Return without errors.返回没有错误
	printf("Camera calibration was successful.\n");
	displayCamCalib(sl_calib);
	return 0;
}

// Run projector-camera calibration (including intrinsic and extrinsic parameters).//运行 投影仪-摄像机标定（包括内外参数）
int runProjectorCalibration( 
						    struct slParams* sl_params, 
					        struct slCalib* sl_calib,
							bool calibrate_both){

	// Reset projector (and camera) calibration status (will be set again, if successful.重置投影仪-摄像机标定状态（如果成功将会重启）
	sl_calib->proj_intrinsic_calib   = false;
	sl_calib->procam_extrinsic_calib = false;
	if(calibrate_both)
		sl_calib->cam_intrinsic_calib = false;

	// Create camera calibration directory (clear previous calibration first).创建摄像机标定目录（首相清除以前的标定）
	char str[1024], calibDir[1024];
	if(calibrate_both){
		printf("Creating camera calibration directory (overwrites existing data)...\n");
		sprintf(calibDir, "%s\\calib\\cam", sl_params->outdir);
		sprintf(str, "%s\\calib", sl_params->outdir);
		_mkdir(str);
		_mkdir(calibDir);
		sprintf(str, "rd /s /q \"%s\"", calibDir);
		system(str);
		if(_mkdir(calibDir) != 0){
			printf("ERROR: Cannot open output directory!\n");
			printf("Projector-camera calibration was not successful and must be repeated.\n");
			return -1;
		}
	}
	else{
		if(!sl_calib->cam_intrinsic_calib){
			printf("ERROR: Camera must be calibrated first or simultaneously!\n");
			printf("Projector calibration was not successful and must be repeated.\n");
			return -1;	
		}
	}

	// Create projector calibration directory (clear previous calibration first).创建投影仪标定目录（首相清除以前的标定）
	printf("Creating projector calibration directory (overwrites existing data)...\n");
	sprintf(calibDir, "%s\\calib\\proj", sl_params->outdir);
	sprintf(str, "%s\\calib", sl_params->outdir);
	_mkdir(str);
	_mkdir(calibDir);
	sprintf(str, "rd /s /q \"%s\"", calibDir);
	system(str);
	if(_mkdir(calibDir) != 0){
		printf("ERROR: Cannot open output directory!\n");
		if(calibrate_both)
			printf("Projector-camera calibration was not successful and must be repeated.\n");
		else
			printf("Projector calibration was not successful and must be repeated.\n");
		return -1;
	}

	// Prompt user for maximum number of calibration boards.提示用户最大标定板数目
	printf("Enter the maximum number of calibraiton images, then press return.\n");
	printf("+ Maximum number of images = ");
	int n_boards;
	scanf("%d", &n_boards);
	if(n_boards<2){
		printf("ERROR: At least two images are required!\n");
	    if(calibrate_both)
			printf("Projector-camera calibration was not successful and must be repeated.\n");
		else
			printf("Projector calibration was not successful and must be repeated.\n");
		return -1;
	}
	
	// Evaluate derived camera parameters and allocate storage.测量导出摄像机参数和分配存储。
	int cam_board_n            = sl_params->cam_board_w*sl_params->cam_board_h;
	CvSize cam_board_size      = cvSize(sl_params->cam_board_w, sl_params->cam_board_h); /* 定标板上每行、列的角点数 标定板尺寸*/
	CvMat* cam_image_points    = cvCreateMat(n_boards*cam_board_n, 2, CV_32FC1);/* 保存定标板上的二维坐标 */
    CvMat* cam_object_points   = cvCreateMat(n_boards*cam_board_n, 3, CV_32FC1);/* 保存定标板上的三维坐标 */
    CvMat* cam_point_counts    = cvCreateMat(n_boards, 1, CV_32SC1);   /* 每幅图像中角点的数量 */
	IplImage** cam_calibImages = new IplImage* [n_boards];/* 缓存每幅图像上检测到的角点 */

	// Evaluate derived projector parameters and allocate storage.测量导出投影仪参数和分配存储。
	int proj_board_n            = sl_params->proj_board_w*sl_params->proj_board_h;
	CvSize proj_board_size      = cvSize(sl_params->proj_board_w, sl_params->proj_board_h);/* 实际测量得到的定标板上每个棋盘格的大小 */
	CvMat* proj_image_points    = cvCreateMat(n_boards*proj_board_n, 2, CV_32FC1);/* 保存定标板上的二维坐标 */
    CvMat* proj_point_counts    = cvCreateMat(n_boards, 1, CV_32SC1);/* 每幅图像中角点的数量 */
	IplImage** proj_calibImages = new IplImage* [n_boards];/* 缓存每幅图像上检测到的角点 */

	// Generate projector calibration chessboard pattern.生成投影仪标定棋盘模式
	IplImage* proj_chessboard = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
	int proj_border_cols, proj_border_rows;
	if(generateChessboard(sl_params, proj_chessboard, proj_border_cols, proj_border_rows) == -1){
		if(calibrate_both)
			printf("Projector-camera calibration was not successful and must be repeated.\n");
		else
			printf("Projector calibration was not successful and must be repeated.\n");
		return -1;
	}

	// Initialize capture and allocate storage.初始化获取和分配储存
	printf("Press 'n' (in 'camWindow') to capture next image, or 'ESC' to quit.\n");
	//IplImage* cam_frame   = cvQueryFrame2(capture, sl_params);
	IplImage *cam_frame = NULL;
	QueryFrameMX500(&cam_frame);
	IplImage* cam_frame_1 = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);
	IplImage* cam_frame_2 = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);
	IplImage* cam_frame_3 = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);
	for(int i=0; i<n_boards; i++)
		cam_calibImages[i]  = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);
	for(int i=0; i<n_boards; i++)
		proj_calibImages[i] = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);

	// Create a window to display capture frames.创建一个窗口去显示获取图像
	cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("Cam. Gain",  "camWindow", &sl_params->cam_gain,  100, NULL);
	cvCreateTrackbar("Proj. Gain", "camWindow", &sl_params->proj_gain, 100, NULL);
	HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
	BringWindowToTop(camWindow);
	cvWaitKey(1);

    // Create a window to display projector image.创建一个窗口去显示投影仪的图片
	IplImage* proj_frame = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
	cvSet(proj_frame, cvScalar(255.0));
	cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
	cvShowImage("projWindow", proj_frame);
	cvWaitKey(1);

	// Allocate storage for grayscale images.分配储存灰度图像
	IplImage* cam_frame_1_gray = cvCreateImage(cvGetSize(cam_frame), IPL_DEPTH_8U, 1);
	IplImage* cam_frame_2_gray = cvCreateImage(cvGetSize(cam_frame), IPL_DEPTH_8U, 1);

	// Capture live image stream, until "ESC" is pressed or calibration is complete.获取动态图像，直到按“ESC”或标定完成
	int successes = 0;
	bool captureFrame = false;
	int cvKey = -1, cvKey_temp = -1;
	while(successes < n_boards){

		// Get next available "safe" frame.得到下一张有效的图像
		//cam_frame = cvQueryFrameSafe(capture, sl_params);
	    QueryFrameMX500(&cam_frame);
		cvScale(cam_frame, cam_frame, 2.*(sl_params->cam_gain/100.), 0);
		cvCopyImage(cam_frame, cam_frame_1);

		// Find camera chessboard corners.发现相机棋盘角点
		CvPoint2D32f* cam_corners = new CvPoint2D32f[cam_board_n];
		int cam_corner_count;
		int cam_found =	detectChessboard(cam_frame_1, cam_board_size, cam_corners, &cam_corner_count);
		
		// If camera chessboard is found, attempt to detect projector chessboard.如果相机标定被发现，尝试查找投影仪棋盘
		if(cam_corner_count == cam_board_n){
		
			// Display projector chessboard.显示投影仪棋盘
			cvCopy(proj_chessboard, proj_frame);
			cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
			cvShowImage("projWindow", proj_frame);

			// Get next available "safe" frame (after appropriate delay).得到下一张有效的图像（在适合的延迟以后）
			cvKey_temp = cvWaitKey(sl_params->delay);
			if(cvKey_temp != -1) 
				cvKey = cvKey_temp;
			//cam_frame = cvQueryFrameSafe(capture, sl_params);
	        QueryFrameMX500(&cam_frame);
			cvScale(cam_frame, cam_frame, 2.*(sl_params->cam_gain/100.), 0);
			cvCopyImage(cam_frame, cam_frame_2);
			cvCopyImage(cam_frame, cam_frame_3);

			// Convert frames to grayscale and apply background subtraction.转换为灰度图像并消除背景
			cvCvtColor(cam_frame_1, cam_frame_1_gray, CV_RGB2GRAY);
			cvCvtColor(cam_frame_2, cam_frame_2_gray, CV_RGB2GRAY);
			cvSub(cam_frame_1_gray, cam_frame_2_gray, cam_frame_2_gray);

			// Invert chessboard image.转化标定板图片
			double min_val, max_val;
			cvMinMaxLoc(cam_frame_2_gray, &min_val, &max_val);//找到灰度图像的最大值和最小值
			cvConvertScale(cam_frame_2_gray, cam_frame_2_gray, 
				-255.0/(max_val-min_val), 255.0+((255.0*min_val)/(max_val-min_val)));

			// Find projector chessboard corners.寻找投影仪棋盘角点
			CvPoint2D32f* proj_corners = new CvPoint2D32f[proj_board_n];
			int proj_corner_count;
			int proj_found = detectChessboard(cam_frame_2_gray, proj_board_size, proj_corners, &proj_corner_count);

			// Display current projector tracking results.显示当前投影仪跟踪结果
			cvDrawChessboardCorners(cam_frame_3, proj_board_size, proj_corners, proj_corner_count, proj_found);
			cvShowImageResampled("camWindow", cam_frame_3, sl_params->window_w, sl_params->window_h);

			// If chessboard is detected, then update calibration lists.如果检测到棋盘,那么更新标定列表。
			/* 将角点的存储结构转换成矩阵形式 */
			if(captureFrame & (proj_corner_count == proj_board_n)){

				// Add camera calibration data.添加摄像机标定数据
				for(int i=successes*cam_board_n, j=0; j<cam_board_n; ++i,++j){
					CV_MAT_ELEM(*cam_image_points,  float, i, 0) = cam_corners[j].x;
					CV_MAT_ELEM(*cam_image_points,  float, i, 1) = cam_corners[j].y;
					CV_MAT_ELEM(*cam_object_points, float, i, 0) = sl_params->cam_board_w_mm*float(j/sl_params->cam_board_w);
					CV_MAT_ELEM(*cam_object_points, float, i, 1) = sl_params->cam_board_h_mm*float(j%sl_params->cam_board_w);
					CV_MAT_ELEM(*cam_object_points, float, i, 2) = 0.0f;
				}
				CV_MAT_ELEM(*cam_point_counts, int, successes, 0) = cam_board_n;
				cvCopyImage(cam_frame_1, cam_calibImages[successes]);

				// Add projector calibration data.添加投影仪标定数据
				for(int i=successes*proj_board_n, j=0; j<proj_board_n; ++i,++j){
					CV_MAT_ELEM(*proj_image_points, float, i, 0) = proj_corners[j].x;
					CV_MAT_ELEM(*proj_image_points, float, i, 1) = proj_corners[j].y;
				}
				CV_MAT_ELEM(*proj_point_counts, int, successes, 0) = proj_board_n;
				cvCopyImage(cam_frame_2, proj_calibImages[successes]);

				// Update display.更新显示
				successes++;
				printf("+ Captured frame %d of %d.\n",successes,n_boards);
				captureFrame = false;
			}

			// Free allocated resources.清空分配资源
			delete[] proj_corners;

			// Display white image for next camera capture frame.为下一副获取图像显示白色图片
			cvSet(proj_frame, cvScalar(255));
			cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
			cvShowImage("projWindow", proj_frame);
			cvKey_temp = cvWaitKey(sl_params->delay);
			if(cvKey_temp != -1) 
				cvKey = cvKey_temp;
		}
		else{
			
			// Camera chessboard not found, display current camera tracking results.相机棋盘没有找到,显示当前摄像机跟踪结果
			cvDrawChessboardCorners(cam_frame_1, cam_board_size, cam_corners, cam_corner_count, cam_found);
			cvShowImageResampled("camWindow", cam_frame_1, sl_params->window_w, sl_params->window_h);

			// Display white image for next camera capture frame.为下一副获取图像显示白色图片
			cvSet(proj_frame, cvScalar(255));
			cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
			cvShowImage("projWindow", proj_frame);
			cvKey_temp = cvWaitKey(1);
			if(cvKey_temp != -1) 
				cvKey = cvKey_temp;
		}

		// Free allocated resources.清空分配资源
		delete[] cam_corners;

		// Process user input.进行用户输入
		cvKey_temp = cvWaitKey(10);
		if(cvKey_temp != -1)
			cvKey = cvKey_temp;
		if(cvKey==27)
			break;
		else if(cvKey=='n')//按“n”获取下一张图像
			captureFrame = true;
		cvKey_temp = -1;
		cvKey = -1;
	}

	// Close the display window.关闭显示窗口
	cvDestroyWindow("camWindow");

	// Calibrate projector, if minimum number of frames are available.标定投影仪，如果图像的最小数量有效
	if(successes >= 2){
		
		// Allocate calibration matrices.分配标定矩阵
		CvMat* cam_object_points2       = cvCreateMat(successes*cam_board_n, 3, CV_32FC1);/* 保存定标板上角点的三维坐标 */
		CvMat* cam_image_points2        = cvCreateMat(successes*cam_board_n, 2, CV_32FC1);/* 保存定标板上角点的二维坐标 */
		CvMat* cam_point_counts2        = cvCreateMat(successes, 1, CV_32SC1);/* 每幅图像中角点的数量 */
	    CvMat* cam_rotation_vectors     = cvCreateMat(successes, 3, CV_32FC1);/* 每幅图像的旋转向量 */
  	    CvMat* cam_translation_vectors  = cvCreateMat(successes, 3, CV_32FC1);/* 每幅图像的平移向量 */	
		CvMat* proj_object_points2      = cvCreateMat(successes*proj_board_n, 3, CV_32FC1);/* 保存定标板上角点的三维坐标 */
		CvMat* proj_image_points2       = cvCreateMat(successes*proj_board_n, 2, CV_32FC1);/* 保存定标板上角点的二维坐标 */
		CvMat* proj_point_counts2       = cvCreateMat(successes, 1, CV_32SC1);/* 每幅图像中角点的数量 */
	    CvMat* proj_rotation_vectors    = cvCreateMat(successes, 3, CV_32FC1);/* 每幅图像的旋转向量 */
  	    CvMat* proj_translation_vectors = cvCreateMat(successes, 3, CV_32FC1);/* 每幅图像的平移向量 */

		// Transfer camera calibration data from captured values.从获取值里转换摄像机标定数据
		for(int i=0; i<successes*cam_board_n; ++i){
			CV_MAT_ELEM(*cam_image_points2,  float, i, 0) = CV_MAT_ELEM(*cam_image_points,  float, i, 0);
			CV_MAT_ELEM(*cam_image_points2,  float, i, 1) = CV_MAT_ELEM(*cam_image_points,  float, i, 1);
			CV_MAT_ELEM(*cam_object_points2, float, i, 0) =	CV_MAT_ELEM(*cam_object_points, float, i, 0);
			CV_MAT_ELEM(*cam_object_points2, float, i, 1) =	CV_MAT_ELEM(*cam_object_points, float, i, 1);
			CV_MAT_ELEM(*cam_object_points2, float, i, 2) = CV_MAT_ELEM(*cam_object_points, float, i, 2);
		}
		for(int i=0; i<successes; ++i)
			CV_MAT_ELEM(*cam_point_counts2, int, i, 0) = CV_MAT_ELEM(*cam_point_counts, int, i, 0);

		// Calibrate the camera and save calibration parameters (if camera calibration is enabled).标定摄像机并储存标定参数（如果摄像机标定结束）
		if(calibrate_both){
			printf("Calibrating camera...\n");
			int calib_flags = 0;
			if(!sl_params->cam_dist_model[0])
				calib_flags |= CV_CALIB_ZERO_TANGENT_DIST;
			if(!sl_params->cam_dist_model[1]){
				cvmSet(sl_calib->cam_distortion, 4, 0, 0);
				calib_flags |= CV_CALIB_FIX_K3;
			}
			cvCalibrateCamera2(
				cam_object_points2, cam_image_points2, cam_point_counts2, 
				cvSize(sl_params->cam_w, sl_params->cam_h), 
				sl_calib->cam_intrinsic, sl_calib->cam_distortion,
				cam_rotation_vectors, cam_translation_vectors, calib_flags);
			printf("Saving calibration images and parameters...\n");
			sprintf(calibDir, "%s\\calib\\cam", sl_params->outdir);
			CvMat* R = cvCreateMat(3, 3, CV_32FC1);/* 保存每幅图像的旋转矩阵 */
			CvMat* r = cvCreateMat(1, 3, CV_32FC1);/* 保存每幅图像的平移矩阵 */
			for(int i=0; i<successes; ++i){
				sprintf(str,"%s\\%0.2d.png", calibDir, i);
				cvSaveImage(str, cam_calibImages[i]);
				cvGetRow(cam_rotation_vectors, r, i);
				cvRodrigues2(r, R, NULL);
				sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
				//cvSave(str, R);
			}
			sprintf(str,"%s\\cam_intrinsic.xml", calibDir);	
			cvSave(str, sl_calib->cam_intrinsic);
			sprintf(str,"%s\\cam_distortion.xml", calibDir);
			cvSave(str, sl_calib->cam_distortion);
			sprintf(str,"%s\\cam_rotation_vectors.xml", calibDir);
			cvSave(str, cam_rotation_vectors);
			sprintf(str,"%s\\cam_translation_vectors.xml", calibDir);
			cvSave(str, cam_translation_vectors);
			cvReleaseMat(&R);
			cvReleaseMat(&r);
			sl_calib->cam_intrinsic_calib = true;
		}

		// Transfer projector calibration data from captured values.从获取值里转换投影仪标定数据
		for(int i=0; i<successes; ++i){

			// Define image points corresponding to projector chessboard (i.e., considering projector as an inverse camera).
			//定义图像点对应于投影仪棋盘(即考虑到投影仪作为反相机)
			if(!sl_params->proj_invert){
				for(int j=0; j<proj_board_n; ++j){
					CV_MAT_ELEM(*proj_image_points2, float, proj_board_n*i+j, 0) = 
						sl_params->proj_board_w_pixels*float(j%sl_params->proj_board_w) + (float)proj_border_cols + (float)sl_params->proj_board_w_pixels - (float)0.5;
					CV_MAT_ELEM(*proj_image_points2, float, proj_board_n*i+j, 1) = 
						sl_params->proj_board_h_pixels*float(j/sl_params->proj_board_w) + (float)proj_border_rows + (float)sl_params->proj_board_h_pixels - (float)0.5;
				}
			}
			else{
				for(int j=0; j<proj_board_n; ++j){
					CV_MAT_ELEM(*proj_image_points2, float, proj_board_n*i+j, 0) = 
						sl_params->proj_board_w_pixels*float((proj_board_n-j-1)%sl_params->proj_board_w) + (float)proj_border_cols + (float)sl_params->proj_board_w_pixels - (float)0.5;
					CV_MAT_ELEM(*proj_image_points2, float, proj_board_n*i+j, 1) = 
						sl_params->proj_board_h_pixels*float((proj_board_n-j-1)/sl_params->proj_board_w) + (float)proj_border_rows + (float)sl_params->proj_board_h_pixels - (float)0.5;
				}
			}

			// Evaluate undistorted image pixels for both the camera and the projector chessboard corners.测定不失真图像像素的摄像机和投影机棋盘角
			CvMat* cam_dist_image_points    = cvCreateMat(cam_board_n,  1, CV_32FC2);/*保存摄像机失真的像素点*/
			CvMat* cam_undist_image_points  = cvCreateMat(cam_board_n,  1, CV_32FC2);/*保存摄像机不失真的像素点*/
			CvMat* proj_dist_image_points   = cvCreateMat(proj_board_n, 1, CV_32FC2);/*保存投影仪失真的像素点*/
			CvMat* proj_undist_image_points = cvCreateMat(proj_board_n, 1, CV_32FC2);/*保存投影仪不失真的像素点*/
			for(int j=0; j<cam_board_n; ++j)
				cvSet1D(cam_dist_image_points, j, 
					cvScalar(CV_MAT_ELEM(*cam_image_points, float, cam_board_n*i+j, 0), 
					         CV_MAT_ELEM(*cam_image_points, float, cam_board_n*i+j, 1)));
			for(int j=0; j<proj_board_n; ++j)
				cvSet1D(proj_dist_image_points, j, 
					cvScalar(CV_MAT_ELEM(*proj_image_points, float, proj_board_n*i+j, 0), 
					         CV_MAT_ELEM(*proj_image_points, float, proj_board_n*i+j, 1)));
			cvUndistortPoints(cam_dist_image_points, cam_undist_image_points, 
				sl_calib->cam_intrinsic, sl_calib->cam_distortion, NULL, NULL);
			cvUndistortPoints(proj_dist_image_points, proj_undist_image_points, 
				sl_calib->cam_intrinsic, sl_calib->cam_distortion, NULL, NULL);
			cvReleaseMat(&cam_dist_image_points);
			cvReleaseMat(&proj_dist_image_points);

			// Estimate homography that maps undistorted image pixels to positions on the chessboard.测定单应性映射不失真图像像素在棋盘上位置。
			CvMat* homography = cvCreateMat(3, 3, CV_32FC1);
			CvMat* cam_src    = cvCreateMat(cam_board_n, 3, CV_32FC1);
			CvMat* cam_dst    = cvCreateMat(cam_board_n, 3, CV_32FC1);
			for(int j=0; j<cam_board_n; ++j){
				CvScalar pd = cvGet1D(cam_undist_image_points, j);
				CV_MAT_ELEM(*cam_src, float, j, 0) = (float)pd.val[0];
				CV_MAT_ELEM(*cam_src, float, j, 1) = (float)pd.val[1];
				CV_MAT_ELEM(*cam_src, float, j, 2) = 1.0;
				CV_MAT_ELEM(*cam_dst, float, j, 0) = CV_MAT_ELEM(*cam_object_points, float, cam_board_n*i+j, 0);
				CV_MAT_ELEM(*cam_dst, float, j, 1) = CV_MAT_ELEM(*cam_object_points, float, cam_board_n*i+j, 1);
				CV_MAT_ELEM(*cam_dst, float, j, 2) = 1.0;
			}
			cvReleaseMat(&cam_undist_image_points);
			cvFindHomography(cam_src, cam_dst, homography);
			cvReleaseMat(&cam_src);
			cvReleaseMat(&cam_dst);

			// Map undistorted projector image corners to positions on the chessboard plane.图上不失真投影仪图像角点在棋盘上位置。
			CvMat* proj_src = cvCreateMat(proj_board_n, 1, CV_32FC2);
			CvMat* proj_dst = cvCreateMat(proj_board_n, 1, CV_32FC2);
			for(int j=0; j<proj_board_n; j++)
				cvSet1D(proj_src, j, cvGet1D(proj_undist_image_points, j));
			cvReleaseMat(&proj_undist_image_points);
			cvPerspectiveTransform(proj_src, proj_dst, homography);
			cvReleaseMat(&homography);
			cvReleaseMat(&proj_src);
			
			// Define object points corresponding to projector chessboard.定义标定板角点对应于投影仪棋盘。
			for(int j=0; j<proj_board_n; j++){
				CvScalar pd = cvGet1D(proj_dst, j);
				CV_MAT_ELEM(*proj_object_points2, float, proj_board_n*i+j, 0) = (float)pd.val[0];
				CV_MAT_ELEM(*proj_object_points2, float, proj_board_n*i+j, 1) = (float)pd.val[1];
				CV_MAT_ELEM(*proj_object_points2, float, proj_board_n*i+j, 2) = 0.0f;
			}
			cvReleaseMat(&proj_dst);
		}
		for(int i=0; i<successes; ++i)
			CV_MAT_ELEM(*proj_point_counts2, int, i, 0) = CV_MAT_ELEM(*proj_point_counts, int, i, 0);

		// Calibrate the projector and save calibration parameters (if camera calibration is enabled).标定投影仪并储存标定参数（如果摄像机标定结束）
		printf("Calibrating projector...\n");
		int calib_flags = 0;
		if(!sl_params->proj_dist_model[0])
			calib_flags |= CV_CALIB_ZERO_TANGENT_DIST;
		if(!sl_params->proj_dist_model[1]){
			cvmSet(sl_calib->proj_distortion, 4, 0, 0);
			calib_flags |= CV_CALIB_FIX_K3;
		}
		cvCalibrateCamera2(
			proj_object_points2, proj_image_points2, proj_point_counts2, 
			cvSize(sl_params->proj_w, sl_params->proj_h), 
			sl_calib->proj_intrinsic, sl_calib->proj_distortion,
			proj_rotation_vectors, proj_translation_vectors, calib_flags);
		printf("Saving calibration images and parameters...\n");
		sprintf(calibDir, "%s\\calib\\proj", sl_params->outdir);
		CvMat* R = cvCreateMat(3, 3, CV_32FC1);
		CvMat* r = cvCreateMat(1, 3, CV_32FC1);
		for(int i=0; i<successes; ++i){
			sprintf(str,"%s\\%0.2d.png", calibDir, i);
			cvSaveImage(str, proj_calibImages[i]);
			sprintf(str,"%s\\%0.2db.png", calibDir, i);
			cvSaveImage(str, cam_calibImages[i]);
			cvGetRow(proj_rotation_vectors, r, i);
			cvRodrigues2(r, R, NULL);
			sprintf(str,"%s\\proj_rotation_matrix_%0.2d.xml", calibDir, i);
			//cvSave(str, R);
		}
		sprintf(str,"%s\\proj_intrinsic.xml", calibDir);	
		cvSave(str, sl_calib->proj_intrinsic);
		sprintf(str,"%s\\proj_distortion.xml", calibDir);
		cvSave(str, sl_calib->proj_distortion);
		sprintf(str,"%s\\proj_rotation_vectors.xml", calibDir);
		cvSave(str, proj_rotation_vectors);
		sprintf(str,"%s\\proj_translation_vectors.xml", calibDir);
		cvSave(str, proj_translation_vectors);

		// Save the camera calibration parameters (in case camera is recalibrated).储存摄像机标定参数（如果摄像机标定结束）
		sprintf(calibDir, "%s\\calib\\proj", sl_params->outdir);
		for(int i=0; i<successes; ++i){
			cvGetRow(cam_rotation_vectors, r, i);
			cvRodrigues2(r, R, NULL);
			sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
			//cvSave(str, R);
		}
		sprintf(str,"%s\\cam_intrinsic.xml", calibDir);	
		cvSave(str, sl_calib->cam_intrinsic);
		sprintf(str,"%s\\cam_distortion.xml", calibDir);
		cvSave(str, sl_calib->cam_distortion);
		sprintf(str,"%s\\cam_rotation_vectors.xml", calibDir);
		cvSave(str, cam_rotation_vectors);
		sprintf(str,"%s\\cam_translation_vectors.xml", calibDir);
		cvSave(str, cam_translation_vectors);

		// Save extrinsic calibration of projector-camera system.储存摄像机-投影仪外部标定参数
		// Note: First calibration image is used to define extrinsic calibration.首先标定图片用于定义外部标定
		CvMat* cam_object_points_00      = cvCreateMat(cam_board_n, 3, CV_32FC1);
		CvMat* cam_image_points_00       = cvCreateMat(cam_board_n, 2, CV_32FC1);
		CvMat* cam_rotation_vector_00    = cvCreateMat(1, 3, CV_32FC1);
  	    CvMat* cam_translation_vector_00 = cvCreateMat(1, 3, CV_32FC1);
		if(!calibrate_both){
			for(int i=0; i<cam_board_n; ++i){
				CV_MAT_ELEM(*cam_image_points_00,  float, i, 0) = CV_MAT_ELEM(*cam_image_points2,  float, i, 0);
				CV_MAT_ELEM(*cam_image_points_00,  float, i, 1) = CV_MAT_ELEM(*cam_image_points2,  float, i, 1);
				CV_MAT_ELEM(*cam_object_points_00, float, i, 0) = CV_MAT_ELEM(*cam_object_points2, float, i, 0);
				CV_MAT_ELEM(*cam_object_points_00, float, i, 1) = CV_MAT_ELEM(*cam_object_points2, float, i, 1);
				CV_MAT_ELEM(*cam_object_points_00, float, i, 2) = CV_MAT_ELEM(*cam_object_points2, float, i, 2);
			}
			cvFindExtrinsicCameraParams2(
				cam_object_points_00, cam_image_points_00, 
				sl_calib->cam_intrinsic, sl_calib->cam_distortion,
				cam_rotation_vector_00, cam_translation_vector_00);
			for(int i=0; i<3; i++)
				CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 0, i) = (float)cvmGet(cam_rotation_vector_00, 0, i);
			for(int i=0; i<3; i++)
				CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 1, i) = (float)cvmGet(cam_translation_vector_00, 0, i);
		}
		else{
			for(int i=0; i<3; i++)
				CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 0, i) = (float)cvmGet(cam_rotation_vectors, 0, i);
			for(int i=0; i<3; i++)
				CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 1, i) = (float)cvmGet(cam_translation_vectors, 0, i);
		}
		sprintf(str, "%s\\cam_extrinsic.xml", calibDir);
		cvSave(str, sl_calib->cam_extrinsic);
		for(int i=0; i<3; i++)
			CV_MAT_ELEM(*sl_calib->proj_extrinsic, float, 0, i) = (float)cvmGet(proj_rotation_vectors, 0, i);
		for(int i=0; i<3; i++)
			CV_MAT_ELEM(*sl_calib->proj_extrinsic, float, 1, i) = (float)cvmGet(proj_translation_vectors, 0, i);
		sprintf(str, "%s\\proj_extrinsic.xml", calibDir);
		cvSave(str, sl_calib->proj_extrinsic);
		sprintf(str,"%s\\config.xml", calibDir);
		writeConfiguration(str, sl_params);

		// Free allocated resources.清空分配资源
		cvReleaseMat(&cam_object_points2);
		cvReleaseMat(&cam_image_points2);
		cvReleaseMat(&cam_point_counts2);
	    cvReleaseMat(&cam_rotation_vectors);
  	    cvReleaseMat(&cam_translation_vectors);
		cvReleaseMat(&proj_object_points2);
		cvReleaseMat(&proj_image_points2);
		cvReleaseMat(&proj_point_counts2);
	    cvReleaseMat(&proj_rotation_vectors);
  	    cvReleaseMat(&proj_translation_vectors);
		cvReleaseMat(&R);
		cvReleaseMat(&r);
		cvReleaseMat(&cam_object_points_00);
		cvReleaseMat(&cam_image_points_00);
		cvReleaseMat(&cam_rotation_vector_00);
  	    cvReleaseMat(&cam_translation_vector_00);
	}
	else{
		printf("ERROR: At least two detected chessboards are required!\n");
	    if(calibrate_both)
			printf("Projector-camera calibration was not successful and must be repeated.\n");
		else
			printf("Projector calibration was not successful and must be repeated.\n");
		return -1;
	}

	// Update calibration status.更新标定状态
	sl_calib->proj_intrinsic_calib   = true;
	sl_calib->procam_extrinsic_calib = true;

	// Evaluate projector-camera geometry.测定投影仪-摄像机几何关系
	evaluateProCamGeometry(sl_params, sl_calib);

	// Free allocated resources.清空分配资源
	cvReleaseMat(&cam_image_points);
    cvReleaseMat(&cam_object_points);
    cvReleaseMat(&cam_point_counts);
	cvReleaseMat(&proj_image_points);
    cvReleaseMat(&proj_point_counts);
	cvReleaseImage(&proj_chessboard);
	cvReleaseImage(&cam_frame_1);
	cvReleaseImage(&cam_frame_2);
	cvReleaseImage(&cam_frame_3);
	cvReleaseImage(&proj_frame);
	cvReleaseImage(&cam_frame_1_gray);
	cvReleaseImage(&cam_frame_2_gray);
	for(int i=0; i<n_boards; i++){
		cvReleaseImage(&cam_calibImages[i]);
		cvReleaseImage(&proj_calibImages[i]);
	}
	delete[] cam_calibImages;
	delete[] proj_calibImages;

	// Return without errors.返回没有错误
	if(calibrate_both){
		printf("Projector-camera calibration was successful.\n");
		displayCamCalib(sl_calib);
	}
	else
		printf("Projector calibration was successful.\n");
	displayProjCalib(sl_calib);
	return 0;
}

// Run projector-camera extrinsic calibration.运行投影仪-摄像机外部标定
int runProCamExtrinsicCalibration( 
							      struct slParams* sl_params, 
					              struct slCalib* sl_calib){

	// Reset projector-camera calibration status (will be set again, if successful).重置投影仪-摄像机标定状态（如果成功将重启）
	sl_calib->procam_extrinsic_calib = false;

    // Check that projector and camera have already been calibrated.检查投影仪-摄像机已经完成标定
	if(!sl_calib->cam_intrinsic_calib || !sl_calib->proj_intrinsic_calib){
		printf("ERROR: Camera and projector must be calibrated first!\n");
		printf("Projector-camera alignment was not successful and must be repeated.\n");
		return -1;
	}

	// Define number of calibration boards (one for now).定义标定板数量（目前只有一个）
	int n_boards = 1;

	// Evaluate derived camera chessboard parameters and allocate storage.测定导处摄像机标定参数并分配资源
	int cam_board_n                = sl_params->cam_board_w*sl_params->cam_board_h;
	CvSize cam_board_size          = cvSize(sl_params->cam_board_w, sl_params->cam_board_h);
	CvMat* cam_image_points        = cvCreateMat(n_boards*cam_board_n, 2, CV_32FC1);
    CvMat* cam_object_points       = cvCreateMat(n_boards*cam_board_n, 3, CV_32FC1);
    CvMat* cam_point_counts        = cvCreateMat(n_boards, 1, CV_32SC1);
	IplImage** cam_calibImages     = new IplImage* [n_boards];
	CvMat* cam_rotation_vectors    = cvCreateMat(n_boards, 3, CV_32FC1);
  	CvMat* cam_translation_vectors = cvCreateMat(n_boards, 3, CV_32FC1);

	// Evaluate derived projector chessboard parameters and allocate storage.测定导处投影仪标定参数并分配资源
	int proj_board_n                = sl_params->proj_board_w*sl_params->proj_board_h;
	CvSize proj_board_size          = cvSize(sl_params->proj_board_w, sl_params->proj_board_h);
	CvMat* proj_image_points        = cvCreateMat(n_boards*proj_board_n, 2, CV_32FC1);
    CvMat* proj_object_points       = cvCreateMat(n_boards*proj_board_n, 3, CV_32FC1);
    CvMat* proj_point_counts        = cvCreateMat(n_boards, 1, CV_32SC1);
	IplImage** proj_calibImages     = new IplImage* [n_boards];
	CvMat* proj_rotation_vectors    = cvCreateMat(n_boards, 3, CV_32FC1);
  	CvMat* proj_translation_vectors = cvCreateMat(n_boards, 3, CV_32FC1);

	// Generate projector calibration chessboard pattern.生成投影仪标定棋盘模式。
	IplImage* proj_chessboard = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
	int proj_border_cols, proj_border_rows;
	if(generateChessboard(sl_params, proj_chessboard, proj_border_cols, proj_border_rows) == -1){
		printf("Projector-camera alignment was not successful and must be repeated.\n");
		return -1;
	}
	
	// Initialize capture and allocate storage.初始化获取并分配资源
	printf("Press 'n' (in 'camWindow') to capture alignment image, or 'ESC' to quit.\n");
	IplImage *cam_frame = NULL;
	QueryFrameMX500(&cam_frame);
	IplImage* cam_frame_1 = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);
	IplImage* cam_frame_2 = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);
	IplImage* cam_frame_3 = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);
	for(int i=0; i<n_boards; i++)
		cam_calibImages[i]  = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);
	for(int i=0; i<n_boards; i++)
		proj_calibImages[i] = cvCreateImage(cvGetSize(cam_frame), cam_frame->depth, cam_frame->nChannels);

	// Create a window to display captured frames.创建一个窗口去显示获取图像
	cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("Cam. Gain",  "camWindow", &sl_params->cam_gain,  100, NULL);
	cvCreateTrackbar("Proj. Gain", "camWindow", &sl_params->proj_gain, 100, NULL);
	HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
	BringWindowToTop(camWindow);
	cvWaitKey(1);

	// Create a window to display projector image.创建一个窗口去显示投影仪图片
	IplImage* proj_frame = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
	cvSet(proj_frame, cvScalar(255.0));
	cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
	cvShowImage("projWindow", proj_frame);
	cvWaitKey(1);

	// Allocate storage for grayscale images.为灰度图像分配存储。
	IplImage* cam_frame_1_gray = cvCreateImage(cvGetSize(cam_frame), IPL_DEPTH_8U, 1);
	IplImage* cam_frame_2_gray = cvCreateImage(cvGetSize(cam_frame), IPL_DEPTH_8U, 1);

	// Capture live image stream, until "ESC" is pressed or calibration is complete.获取动态图像，直到按“ESC”或标定完成
	int successes = 0;
	bool captureFrame = false;
	int cvKey = -1, cvKey_temp = -1;
	while(successes < n_boards){

		// Get next available "safe" frame.得到下一副图像
		//cam_frame = cvQueryFrameSafe(capture, sl_params);
	    QueryFrameMX500(&cam_frame);
		cvScale(cam_frame, cam_frame, 2.*(sl_params->cam_gain/100.), 0);
		cvCopyImage(cam_frame, cam_frame_1);

		// Find camera chessboard corners.寻找相机标定角点
		CvPoint2D32f* cam_corners = new CvPoint2D32f[cam_board_n];
		int cam_corner_count;
		int cam_found =	detectChessboard(cam_frame_1, cam_board_size, cam_corners, &cam_corner_count);

		// If camera chessboard is found, attempt to detect projector chessboard.如果找到相机棋盘，尝试寻找投影仪棋盘
		if(cam_corner_count == cam_board_n){
		
			// Display projector chessboard.显示投影仪棋盘
			cvCopy(proj_chessboard, proj_frame);
			cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
			cvShowImage("projWindow", proj_frame);

			// Get next available "safe" frame (after appropriate delay).得到下一副图像（在合适的延迟后）
			cvKey_temp = cvWaitKey(sl_params->delay);
			if(cvKey_temp != -1) 
				cvKey = cvKey_temp;
	        QueryFrameMX500(&cam_frame);
			cvScale(cam_frame, cam_frame, 2.*(sl_params->cam_gain/100.), 0);
			cvCopyImage(cam_frame, cam_frame_2);

			// Convert to grayscale and apply background subtraction.转换为灰度图像并去除背景
			cvCvtColor(cam_frame_1, cam_frame_1_gray, CV_RGB2GRAY);
			cvCvtColor(cam_frame_2, cam_frame_2_gray, CV_RGB2GRAY);
			cvSub(cam_frame_1_gray, cam_frame_2_gray, cam_frame_2_gray);

			// Invert chessboard image.转化棋盘图像
			double min_val, max_val;
			cvMinMaxLoc(cam_frame_2_gray, &min_val, &max_val);
			cvSubRS(cam_frame_2_gray, cvScalar(max_val), cam_frame_2_gray);

			// Find projetor chessboard corners.寻找投影仪棋盘角点
			CvPoint2D32f* proj_corners = new CvPoint2D32f[proj_board_n];
			int proj_corner_count;
			int proj_found = detectChessboard(cam_frame_2_gray, proj_board_size, proj_corners, &proj_corner_count);

			// Display current projector tracking results.显示当前投影仪跟踪结果。
			cvCopyImage(cam_frame_2, cam_frame_3);
			cvDrawChessboardCorners(cam_frame_3, proj_board_size, proj_corners, proj_corner_count, proj_found);
			cvShowImageResampled("camWindow", cam_frame_3, sl_params->window_w, sl_params->window_h);

			// If chessboard is detected, then update calibration lists.如果检测到棋盘,那么更新标定列表。
			if(captureFrame & (proj_corner_count == proj_board_n)){

				// Add camera calibration data.添加摄像机标定数据
				for(int i=successes*cam_board_n, j=0; j<cam_board_n; ++i,++j){
					CV_MAT_ELEM(*cam_image_points,  float, i, 0) = cam_corners[j].x;
					CV_MAT_ELEM(*cam_image_points,  float, i, 1) = cam_corners[j].y;
					CV_MAT_ELEM(*cam_object_points, float, i, 0) = sl_params->cam_board_w_mm*float(j/sl_params->cam_board_w);
					CV_MAT_ELEM(*cam_object_points, float, i, 1) = sl_params->cam_board_h_mm*float(j%sl_params->cam_board_w);
					CV_MAT_ELEM(*cam_object_points, float, i, 2) = 0.0f;
				}
				CV_MAT_ELEM(*cam_point_counts, int, successes, 0) = cam_board_n;
				cvCopyImage(cam_frame_1, cam_calibImages[successes]);

				// Evaluate undistorted image pixels for both the camera and the projector chessboard corners.测定不失真图像像素的摄像机和投影机棋盘角。
				CvMat* cam_dist_image_points    = cvCreateMat(cam_board_n,  1, CV_32FC2);
				CvMat* cam_undist_image_points  = cvCreateMat(cam_board_n,  1, CV_32FC2);
				CvMat* proj_dist_image_points   = cvCreateMat(proj_board_n, 1, CV_32FC2);
				CvMat* proj_undist_image_points = cvCreateMat(proj_board_n, 1, CV_32FC2);
				for(int i=0; i<cam_board_n; ++i)
					cvSet1D(cam_dist_image_points,  i, cvScalar(float(cam_corners[i].x), float(cam_corners[i].y)));
				for(int i=0; i<proj_board_n; ++i)
					cvSet1D(proj_dist_image_points, i, cvScalar(float(proj_corners[i].x), float(proj_corners[i].y)));
				cvUndistortPoints(cam_dist_image_points, cam_undist_image_points, 
					sl_calib->cam_intrinsic, sl_calib->cam_distortion, NULL, NULL);
				cvUndistortPoints(proj_dist_image_points, proj_undist_image_points, 
					sl_calib->cam_intrinsic, sl_calib->cam_distortion, NULL, NULL);
				cvReleaseMat(&cam_dist_image_points);
				cvReleaseMat(&proj_dist_image_points);

				// Estimate homography that maps undistorted image pixels to positions on the chessboard.测定单应性映射不失真图像像素位置在棋盘上。
				CvMat* homography = cvCreateMat(3, 3, CV_32FC1);
				CvMat* cam_src    = cvCreateMat(cam_board_n, 3, CV_32FC1);
				CvMat* cam_dst    = cvCreateMat(cam_board_n, 3, CV_32FC1);
				for(int i=0; i<cam_board_n; ++i){
					CvScalar pd = cvGet1D(cam_undist_image_points, i);
					CV_MAT_ELEM(*cam_src, float, i, 0) = (float)pd.val[0];
					CV_MAT_ELEM(*cam_src, float, i, 1) = (float)pd.val[1];
					CV_MAT_ELEM(*cam_src, float, i, 2) = 1.0;
					CV_MAT_ELEM(*cam_dst, float, i, 0) = CV_MAT_ELEM(*cam_object_points, float, cam_board_n*successes+i, 0);
					CV_MAT_ELEM(*cam_dst, float, i, 1) = CV_MAT_ELEM(*cam_object_points, float, cam_board_n*successes+i, 1);
					CV_MAT_ELEM(*cam_dst, float, i, 2) = 1.0;
				}
				cvReleaseMat(&cam_undist_image_points);
				cvFindHomography(cam_src, cam_dst, homography);
				cvReleaseMat(&cam_src);
				cvReleaseMat(&cam_dst);

				// Map undistorted projector image corners to positions on the chessboard plane.图上不失真投影仪图像角点在棋盘上位置。
				CvMat* proj_src = cvCreateMat(proj_board_n, 1, CV_32FC2);
				CvMat* proj_dst = cvCreateMat(proj_board_n, 1, CV_32FC2);
				for(int i=0; i<proj_board_n; i++)
					cvSet1D(proj_src, i, cvGet1D(proj_undist_image_points, i));
				cvReleaseMat(&proj_undist_image_points);
				cvPerspectiveTransform(proj_src, proj_dst, homography);
				cvReleaseMat(&homography);
				cvReleaseMat(&proj_src);
				
				// Add projector calibration data.添加投影仪标定数据
				for(int i=successes*proj_board_n, j=0; j<proj_board_n; ++i,++j){
					CvScalar pd = cvGet2D(proj_dst, j, 0);
					if(!sl_params->proj_invert){
						CV_MAT_ELEM(*proj_image_points,  float, i, 0) = sl_params->proj_board_w_pixels*float(j%sl_params->proj_board_w) + (float)proj_border_cols + (float)sl_params->proj_board_w_pixels - (float)0.5;
						CV_MAT_ELEM(*proj_image_points,  float, i, 1) = sl_params->proj_board_h_pixels*float(j/sl_params->proj_board_w) + (float)proj_border_rows + (float)sl_params->proj_board_h_pixels - (float)0.5;
					}
					else{
						CV_MAT_ELEM(*proj_image_points,  float, i, 0) = sl_params->proj_board_w_pixels*float((proj_board_n-j-1)%sl_params->proj_board_w) + (float)proj_border_cols + (float)sl_params->proj_board_w_pixels - (float)0.5;
						CV_MAT_ELEM(*proj_image_points,  float, i, 1) = sl_params->proj_board_h_pixels*float((proj_board_n-j-1)/sl_params->proj_board_w) + (float)proj_border_rows + (float)sl_params->proj_board_h_pixels - (float)0.5;
					}
					CV_MAT_ELEM(*proj_object_points, float, i, 0) = (float)pd.val[0];
					CV_MAT_ELEM(*proj_object_points, float, i, 1) = (float)pd.val[1];
					CV_MAT_ELEM(*proj_object_points, float, i, 2) = 0.0f;
				}
				CV_MAT_ELEM(*proj_point_counts, int, successes, 0) = proj_board_n;
				cvCopyImage(cam_frame_2, proj_calibImages[successes]);
				cvReleaseMat(&proj_dst);

				// Update display.更新显示
				successes++;
				printf("+ Captured frame %d of %d.\n",successes,n_boards);
				captureFrame = false;
			}

			// Free allocated resources.清空分配资源
			delete[] proj_corners;

			// Display white image for next camera capture frame.为下一副获取图像显示白色图像
			cvSet(proj_frame, cvScalar(255));
			cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
			cvShowImage("projWindow", proj_frame);
			cvKey_temp = cvWaitKey(sl_params->delay);
			if(cvKey_temp != -1) 
				cvKey = cvKey_temp;
		}
		else{
			
			// Camera chessboard not found, display current camera tracking results.摄像机标定没有发现，显示当前摄像机跟踪结果
			cvDrawChessboardCorners(cam_frame_1, cam_board_size, cam_corners, cam_corner_count, cam_found);
			cvShowImageResampled("camWindow", cam_frame_1, sl_params->window_w, sl_params->window_h);

			// Display white image for next camera capture frame.为下一副获取图像显示白色图像
			cvSet(proj_frame, cvScalar(255));
			cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
			cvShowImage("projWindow", proj_frame);
			cvKey_temp = cvWaitKey(1);
			if(cvKey_temp != -1) 
				cvKey = cvKey_temp;
		}

		// Free allocated resources.清空分配资源
		delete[] cam_corners;
		
		// Process user input.提示用户的输入。
		cvKey_temp = cvWaitKey(10);
		if(cvKey_temp != -1)
			cvKey = cvKey_temp;
		if(cvKey==27)
			break;
		else if(cvKey=='n')
			captureFrame = true;
		cvKey_temp = -1;
		cvKey = -1;
	}

	// Close the display window.关闭显示窗口
	cvDestroyWindow("camWindow");

	// Calibrate projector-camera alignment, if a single frame is available.标定投影仪-摄像机校准,如果单个图像有效。
	if(successes == 1){
		
		// Estimate extrinsic camera parameters.测定相机外部参数。
		cvFindExtrinsicCameraParams2(
			cam_object_points, cam_image_points, 
			sl_calib->cam_intrinsic, sl_calib->cam_distortion,
			cam_rotation_vectors, cam_translation_vectors);

		// Estimate extrinsic projector parameters.测定外部投影仪参数。
		cvFindExtrinsicCameraParams2(
			proj_object_points, proj_image_points, 
			sl_calib->proj_intrinsic, sl_calib->proj_distortion,
			proj_rotation_vectors, proj_translation_vectors);

		// Save extrinsic calibration of projector-camera system.保存外部标定的投影仪-摄像头系统。
		// Note: First calibration image is used to define extrinsic calibration.首先标定图像用于定义外部标定
		for(int i=0; i<3; i++)
			CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 0, i) = (float)cvmGet(cam_rotation_vectors, 0, i);
		for(int i=0; i<3; i++)
			CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 1, i) = (float)cvmGet(cam_translation_vectors, 0, i);
		char str[1024], calibDir[1024];
		sprintf(calibDir, "%s\\calib\\proj", sl_params->outdir);
		sprintf(str, "%s\\cam_extrinsic.xml", calibDir);
		cvSave(str, sl_calib->cam_extrinsic);
		for(int i=0; i<3; i++)
			CV_MAT_ELEM(*sl_calib->proj_extrinsic, float, 0, i) = (float)cvmGet(proj_rotation_vectors, 0, i);
		for(int i=0; i<3; i++)
			CV_MAT_ELEM(*sl_calib->proj_extrinsic, float, 1, i) = (float)cvmGet(proj_translation_vectors, 0, i);
		sprintf(str, "%s\\proj_extrinsic.xml", calibDir);
		cvSave(str, sl_calib->proj_extrinsic);
		sprintf(str,"%s\\config.xml", calibDir);
		writeConfiguration(str, sl_params);
	}
	else{
		printf("ERROR: At least one chessboard is required!\n");
		printf("Projector-camera alignment was not successful and must be repeated.\n");
		return -1;
	}

	// Set projector-camera calibration status.设置投影仪--摄像机标定状态
	sl_calib->procam_extrinsic_calib = true;

	// Evaluate projector-camera geometry.测定投影仪-摄像机几何关系
	evaluateProCamGeometry(sl_params, sl_calib);

	// Free allocated resources.清空分配资源
	cvReleaseMat(&cam_image_points);
    cvReleaseMat(&cam_object_points);
    cvReleaseMat(&cam_point_counts);
	cvReleaseMat(&cam_rotation_vectors);
  	cvReleaseMat(&cam_translation_vectors);
	cvReleaseMat(&proj_image_points);
    cvReleaseMat(&proj_object_points);
    cvReleaseMat(&proj_point_counts);
	cvReleaseMat(&proj_rotation_vectors);
  	cvReleaseMat(&proj_translation_vectors);
	cvReleaseImage(&proj_chessboard);
	cvReleaseImage(&cam_frame_1);
	cvReleaseImage(&cam_frame_2);
	cvReleaseImage(&cam_frame_3);
	cvReleaseImage(&proj_frame);
	cvReleaseImage(&cam_frame_1_gray);
	cvReleaseImage(&cam_frame_2_gray);
	for(int i=0; i<n_boards; i++){
		cvReleaseImage(&cam_calibImages[i]);
		cvReleaseImage(&proj_calibImages[i]);
	}
	delete[] cam_calibImages;
	delete[] proj_calibImages;

	// Return without errors.返回没有错误
	printf("Projector-camera alignment was successful.\n");
	return 0;
}

// Evaluate geometry of projector-camera optical rays and planes.测定投影仪-摄像机光学几何结构的线与面
int evaluateProCamGeometry(struct slParams* sl_params, struct slCalib* sl_calib){

	// Check for input errors (no intrinsic or extrinsic calibration, etc.).检查输入错误(没有内部或外部标定等)。
	if(!sl_calib->cam_intrinsic_calib || !sl_calib->proj_intrinsic_calib || !sl_calib->procam_extrinsic_calib){
		printf("ERROR: Intrinsic and extrinsic projector-camera calibration must be completed first!\n");
		printf("Projector-camera geometry calculations were not successful and must be repeated.\n");
		return -1;
	}

	// Extract extrinsic calibration parameters.提取外部标定参数
	CvMat* r                = cvCreateMat(1, 3, CV_32FC1);
	CvMat* cam_rotation     = cvCreateMat(3, 3, CV_32FC1);
	CvMat* cam_translation  = cvCreateMat(3, 1, CV_32FC1);
	CvMat* proj_rotation    = cvCreateMat(3, 3, CV_32FC1);
	CvMat* proj_translation = cvCreateMat(3, 1, CV_32FC1);
	cvGetRow(sl_calib->cam_extrinsic, r, 0);
	cvRodrigues2(r, cam_rotation, NULL);
	for(int i=0; i<3; i++)
		cvmSet(cam_translation, i, 0, cvmGet(sl_calib->cam_extrinsic, 1, i));
	cvGetRow(sl_calib->proj_extrinsic, r, 0);
	cvRodrigues2(r, proj_rotation, NULL);
	for(int i=0; i<3; i++)
		cvmSet(proj_translation, i, 0, cvmGet(sl_calib->proj_extrinsic, 1, i));
	cvReleaseMat(&r);

	// Determine centers of projection.确定中心投影。
	// Note: All positions are in coordinate system of the first camera.所有位置的坐标系统为第一个摄像机。
	cvSet(sl_calib->cam_center, cvScalar(0));
	cvGEMM(proj_rotation, proj_translation, -1, NULL, 0, sl_calib->proj_center, CV_GEMM_A_T);
	cvGEMM(cam_rotation, sl_calib->proj_center, 1, cam_translation, 1, sl_calib->proj_center, 0);

	// Pre-compute optical rays for each camera pixel.为每个摄像机像素提前计算光线
	int    cam_nelems        = sl_params->cam_w*sl_params->cam_h;
	CvMat* cam_dist_points   = cvCreateMat(cam_nelems, 1, CV_32FC2);
	CvMat* cam_undist_points = cvCreateMat(cam_nelems, 1, CV_32FC2);
	for(int r=0; r<sl_params->cam_h; r++)
		for(int c=0; c<sl_params->cam_w; c++)
			cvSet1D(cam_dist_points, (sl_params->cam_w)*r+c, cvScalar(float(c), float(r)));
	cvUndistortPoints(cam_dist_points, cam_undist_points, sl_calib->cam_intrinsic, sl_calib->cam_distortion, NULL, NULL);
	for(int i=0; i<cam_nelems; i++){
		CvScalar pd = cvGet1D(cam_undist_points, i);
		float norm = (float)sqrt(pow(pd.val[0],2)+pow(pd.val[1],2)+1.0);
		sl_calib->cam_rays->data.fl[i]              = (float)pd.val[0]/norm;
		sl_calib->cam_rays->data.fl[i+  cam_nelems] = (float)pd.val[1]/norm;
		sl_calib->cam_rays->data.fl[i+2*cam_nelems] = (float)1.0/norm;
	}
	cvReleaseMat(&cam_dist_points);
	cvReleaseMat(&cam_undist_points);

	// Pre-compute optical rays for each projector pixel.为每个投影仪像素提前计算光线
	int    proj_nelems        = sl_params->proj_w*sl_params->proj_h;
	CvMat* proj_dist_points   = cvCreateMat(proj_nelems, 1, CV_32FC2);
	CvMat* proj_undist_points = cvCreateMat(proj_nelems, 1, CV_32FC2);
	for(int r=0; r<sl_params->proj_h; r++)
		for(int c=0; c<sl_params->proj_w; c++)
			cvSet1D(proj_dist_points, (sl_params->proj_w)*r+c, cvScalar(float(c), float(r)));
	cvUndistortPoints(proj_dist_points, proj_undist_points, sl_calib->proj_intrinsic, sl_calib->proj_distortion, NULL, NULL);
	for(int i=0; i<proj_nelems; i++){
		CvScalar pd = cvGet1D(proj_undist_points, i);
		float norm = (float)sqrt(pow(pd.val[0],2)+pow(pd.val[1],2)+1.0);
		sl_calib->proj_rays->data.fl[i]               = (float)pd.val[0]/norm;
		sl_calib->proj_rays->data.fl[i+  proj_nelems] = (float)pd.val[1]/norm;
		sl_calib->proj_rays->data.fl[i+2*proj_nelems] = (float)1.0/norm;
	}
	cvReleaseMat(&proj_dist_points);
	cvReleaseMat(&proj_undist_points);

	// Rotate projector optical rays into the camera coordinate system.旋转投影光线进入相机坐标系
	CvMat* R = cvCreateMat(3, 3, CV_32FC1);
	cvGEMM(cam_rotation, proj_rotation, 1, NULL, 0, R, CV_GEMM_B_T);
	cvGEMM(R, sl_calib->proj_rays, 1, NULL, 0, sl_calib->proj_rays, 0);
	cvReleaseMat(&R);

	// Evaluate scale factor (to assist in plane-fitting).测定比例关系
	float scale = 0;
	for(int i=0; i<3; i++)
		scale += pow((float)sl_calib->proj_center->data.fl[i],(float)2.0);
	scale = sqrt(scale);

	// Estimate plane equations describing every projector column.测定位置方程描述每条投影仪竖的光栅
    // Note: Resulting coefficient vector is in camera coordinate system.产生的系数向量在相机坐标系
	for(int c=0; c<sl_params->proj_w; c++){
		CvMat* points = cvCreateMat(sl_params->proj_h+1, 3, CV_32FC1);
		for(int ro=0; ro<sl_params->proj_h; ro++){
			int ri = (sl_params->proj_w)*ro+c;
			for(int i=0; i<3; i++)
				points->data.fl[3*ro+i] = 
					sl_calib->proj_center->data.fl[i] + scale*sl_calib->proj_rays->data.fl[ri+proj_nelems*i];
		}
		for(int i=0; i<3; i++)
			points->data.fl[3*sl_params->proj_h+i] = sl_calib->proj_center->data.fl[i];
		float plane[4];
		cvFitPlane(points, plane);
		for(int i=0; i<4; i++)
			sl_calib->proj_column_planes->data.fl[4*c+i] = plane[i];
		cvReleaseMat(&points);
	}

	// Estimate plane equations describing every projector row.测定位置方程描述每条投影仪横的光栅
    // Note: Resulting coefficient vector is in camera coordinate system.产生的系数向量在相机坐标系
	for(int r=0; r<sl_params->proj_h; r++){
		CvMat* points = cvCreateMat(sl_params->proj_w+1, 3, CV_32FC1);
		for(int co=0; co<sl_params->proj_w; co++){
			int ri = (sl_params->proj_w)*r+co;
			for(int i=0; i<3; i++)
				points->data.fl[3*co+i] = 
					sl_calib->proj_center->data.fl[i] + scale*sl_calib->proj_rays->data.fl[ri+proj_nelems*i];
		}
		for(int i=0; i<3; i++)
			points->data.fl[3*sl_params->proj_w+i] = sl_calib->proj_center->data.fl[i];
		float plane[4];
		cvFitPlane(points, plane);
		for(int i=0; i<4; i++)
			sl_calib->proj_row_planes->data.fl[4*r+i] = plane[i];
		cvReleaseMat(&points);
	}
	
	// Release allocated resources.释放分配资源
	cvReleaseMat(&cam_rotation);
	cvReleaseMat(&cam_translation);
	cvReleaseMat(&proj_rotation);
	cvReleaseMat(&proj_translation);

	// Return without errors.返回没有错误
	return 0;
}
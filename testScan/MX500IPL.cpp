/************************************************************************/
/* 实现对MX500相机的初始化，采集图像的封装									*/
/* 输出IplImage图像														*/
/* Code RFL																*/
/************************************************************************/

#include "MX500Ipl.h"

//私有变量
static int m_nCameraID;						//设备ID
static tDSCameraDevInfo m_dsCam[10];		//设备信息结构体
/*static */tDSImageSize m_dsImg;				//相机尺寸信息
IplImage *_retireImage = NULL;


/*************************************************************************************/
/* 功能：配置MX500																	 */
/* 返回值：成功返回0，失败返回-1														 */
/*************************************************************************************/
int ConfigMX500(void)
{
	//图像信息
	BOOL bROI;			//是否有ROI

	//查找相机
	int nTotal = 0;
	emDSCameraStatus status = CameraGetDevList(m_dsCam, &nTotal);
	if (status == STATUS_OK && nTotal > 0)
	{
		status = STATUS_NOT_SUPPORTED;					//置状态为无效
		printf("[MX500] camera:%s\n", m_dsCam[0].acFriendlyName);
	}
	else
	{
		printf("[MX500] found camera error\n");
		return -1;
	}

	// 初始化相机(不预览图像)，单张捕捉模式	
	status = CameraInitEx(m_dsCam[0].acFriendlyName, &m_nCameraID);			
	if (status == STATUS_OK)
	{
		//打开相机电源
		CameraPowerUp(m_nCameraID);

		//设置相机分辨率,原始分辨率/2^n
		//CameraSetImageSizeSel(m_nCameraID, 1, FALSE);  // 预览分辨率

		// 获取图像尺寸
		CameraGetImageSize(m_nCameraID, &bROI, &(m_dsImg.iHOffset), &(m_dsImg.iVOffset), &(m_dsImg.iWidth), &(m_dsImg.iHeight));		

		status = STATUS_NOT_SUPPORTED;					//置状态为无效
	}
	else
	{
		printf("[MX500] init camera error\n");
		return -1;
	}

	// 图像垂直翻转,因为这个格式和IplImage*顺序不同，需要竖直翻转
	CameraSetMirror(m_nCameraID, MIRROR_DIRECTION_VERTICAL, TRUE);


	//打开相机
	status = CameraPlay(m_nCameraID);		// 开启视频流
	if (status == STATUS_OK)
	{
		printf("[MX500] playing...\n");
		status = STATUS_NOT_SUPPORTED;			//置状态为无效
	}
	else
	{
		printf("[MX500]play camera error\n");
		return -1;
	}


	// 设置帧速
	// FRAME_SPEED_NORMAL:低速, FRAME_SPEED_HIGH:高速, FRAME_SPEED_SUPER:超高速
	emDSFrameSpeed emFrameSpeed = FRAME_SPEED_SUPER;       
	CameraSetFrameSpeed(m_nCameraID, emFrameSpeed);

	//创建一个内部缓存图像
	_retireImage = cvCreateImage(cvSize(m_dsImg.iWidth, m_dsImg.iHeight), IPL_DEPTH_8U, 3);

	return 0;
}

/************************************************************************/
/* 功能：读取下一帧图像                                                   */
/* 输出：成功返回0,出错返回-1，输出img                                    */
/************************************************************************/
int QueryFrameMX500(IplImage **img)
{
	BYTE* byImageBuf = NULL;
	emDSCameraStatus status = CameraGetImageBuffer(m_nCameraID, DATA_TYPE_RGB8, &byImageBuf);		// 获取RGB 24位图像数据
	if (status == STATUS_OK)						//上述步骤完成，开始生产IplImage图像
	{
		//转为IplImage*图像
		cvSetData(_retireImage, byImageBuf, m_dsImg.iWidth*3);
		if (NULL == *img)
		{
			*img = cvCreateImage(cvSize(m_dsImg.iWidth, m_dsImg.iHeight), IPL_DEPTH_8U, 3);
			cvCopy(_retireImage, *img, NULL);				//复制输出
		}
		else
		{
			cvResize(_retireImage, *img);					//调整输出
		}

		//处理完成
		CameraReleaseImageBuffer(m_nCameraID, DATA_TYPE_BGR8, byImageBuf);		// 释放缓存
		byImageBuf = NULL;
	}
	else
	{
		//printf("[MX500] read error\n");
		return -1;
	}

	return 0;
}

/************************************************************************************/
/* 功能：反初始化相机																	*/
/* 返回值：成功返回0，失败返回-1														*/
/************************************************************************************/
int UnConfigMX500()
{
	emDSCameraStatus status = CameraPlay(m_nCameraID);
	if (status == STATUS_OK)
	{
		CameraPowerDown(m_nCameraID);											//关闭电源
		if (status == STATUS_OK)
		{
			cvReleaseImage(&_retireImage);											//释放缓存图像

			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		printf("[MX500] stop camera error\n");
		return -1;
	}
}
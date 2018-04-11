/*------------------------------------------------------------------------*\
| 功能:  声明DVP_CamAPI.dll的函数接口                            
| 版权:  版权所有                                                     
\*------------------------------------------------------------------------*/

#ifndef _CAMAPI_H_
#define _CAMAPI_H_
#include "DVP_Define.H"

#define DLL_EXPORT
#ifdef DLL_EXPORT
#define DS_API extern "C" __declspec(dllexport) 
#else
#define DS_API extern "C" __declspec(dllimport) 
#endif


/*------------------------------------------------------------------------*\
| 功  能：获取相机列表                                                     
|                                                                          
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                          
|                                                                          
| 参  数：                                                                 
|         1  tDSCameraDevInfo  *pDSCameraList  相机基本信息首地址 输出参数 
|         2  INT               *piNums         相机数量           输出参数 
|                                                                          
| 注  意：                                                                 
|         *piNums为连接本计算机并可访问的相机数量                          
\*------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetDevList(tDSCameraDevInfo *pDSCameraList, INT *piNums);


/*--------------------------------------------------------------------*\
| 功  能：初始化相机（可预览图像）                                     
|                                                                      
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                      
|                                                                      
| 参  数：                                                             
|         1  DS_SNAP_PROC  pCallbackFunction  回调函数指针 输入参数    
|         2  LPCSTR        lpszFriendlyName   相机昵称     输入参数    
|         3  HWND          hWndDisplay        显示窗口句柄 输入参数   
|         4  INT           *piCameraID        相机编号     输出参数    
|                                                                      
| 注  意：
|         1、相机昵称从tDSCameraDevInfo中获取
|         2、摄像头ID，1至n为有效ID，-n至0为无效；                     
|         3、每调用CameraInit成功一次，ID值加1，依次累加，避免ID重复。 
\*--------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraInit(DS_SNAP_PROC pCallbackFunction, 
                                         LPCSTR lpszFriendlyName,
                                           HWND hWndDisplay,
                                            INT *piCameraID);


/*----------------------------------------------------------------------*\
| 功  能：初始化相机（不可预览图像）                                     
|                                                                        
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                        
|                                                                        
| 参  数：                                                               
|         1  LPCSTR  lpszFriendlyName  相机昵称 输入参数               
|         2  INT     *piCameraID       相机编号 输出参数               
|                                                                     
| 注  意：
|         1、相机昵称从tDSCameraDevInfo中获取
|         2、摄像头ID，1至n为有效ID，-n至0为无效；                      
|         3、每调用CameraInitEx成功一次，ID值加1，依次累加，避免ID重复。 
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraInitEx(LPCSTR lpszFriendlyName, INT *piCameraID);


/*-----------------------------------------------------------------------------------*\
| 功  能：获取相机设备参数范围                                                       
|                                                                                  
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                                     
|                                                                                  
| 参  数：                                                                          
|         1  INT           iCameraID      相机编号（由相机初始化成功后返回） 输入参数 
|         2  tDSCameraCap  &sDSCameraCap  相机设备参数范围结构               输出参数
|                                                                                  
| 注  意：                                                                          
|         本函数可获取相机相关参数的数值范围和默认值                                                                        
\*-----------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetCapability(INT iCameraID, tDSCameraCapability &sDSCameraCap);


/*----------------------------------------------------------------------*\
| 功  能：反初始化相机                                                 
|                                                                     
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                       
|                                                                     
| 参  数：                                                            
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数 
|                                                                     
| 注  意：                                                            
|         用于释放内存分配空间，必须在程序退出前调用                     
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraUnInit(INT iCameraID);


/*----------------------------------------------------------------------*\
| 功  能：打开视频流                                                    
|                                                                     
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                        
|                                                                     
| 参  数：                                                            
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数 
|                                                                      
| 注  意：                                                             
|         无                                                           
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraPlay(INT iCameraID);


/*----------------------------------------------------------------------*\
| 功  能：暂停视频流                                                   
|                                                                     
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                        
|                                                                     
| 参  数：                                                            
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数 
|                                                                     
| 注  意：                                                            
|         无                                                          
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraPause(INT iCameraID);


/*----------------------------------------------------------------------*\
| 功  能：停止视频流                                                  
|                                                                    
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                       
|                                                                    
| 参  数：                                                           
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|                                                                    
| 注  意：                                                           
|         无                                                         
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraStop(INT iCameraID);


/*----------------------------------------------------------------------*\
| 功  能：打开相机电源                                                 
|                                                                     
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                       
|                                                                     
| 参  数：                                                            
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数 
|                                                                     
| 注  意：                                                            
|         无                                                          
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraPowerUp(INT iCameraID);


/*----------------------------------------------------------------------*\
| 功  能：关闭相机电源                                                 
|                                                                     
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                       
|                                                                     
| 参  数：                                                            
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数 
|                                                                     
| 注  意：                                                            
|         无                                                          
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraPowerDown(INT iCameraID);


/*---------------------------------------------------------------------------------*\
| 功  能：图像数据处理                                                            
|                                                                                
| 返回值：调用成功返回RGB24图像数据首地址，否则返回NULL                             
|                                                                                
| 参  数：                                                                       
|         1  INT           iCameraID  相机编号（回调函数输入参数）         输入参数
|         2  BYTE          *pbyRAW    图像数据首地址，回调函数参数         输入参数
|         3  tDSFrameInfo  *psFrInfo  图像帧数据信息，参见tDSFrameInfo定义 输入参数
|                                                                                
| 注  意：                                                                       
|         无                                                                     
\*---------------------------------------------------------------------------------*/
DS_API BYTE* CameraISP(INT iCameraID, BYTE *pbyRAW, tDSFrameInfo *psFrInfo);


/*---------------------------------------------------------------------------------*\
| 功  能：RAW转RGB算法选择                                                            
|                                                                                
| 返回值：调用成功返回RGB24图像数据首地址，否则返回NULL                             
|                                                                                
| 参  数：                                                                       
|         1   In  INT    iCameraID		摄像头ID，回调函数参数			输入参数
|         2   In  INT	 iRawToRGBType   RAW转RGB算法编号				输入参数
|                                                                                
| 注  意：                                                                       
|         无                                                                     
\*---------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetBayertoRGBType(INT iCameraID, INT iRawToRGBType);


/*---------------------------------------------------------------------------------*\
| 功  能：获取当前RAW转RGB算法编号                                                            
|                                                                                
| 返回值：调用成功返回RGB24图像数据首地址，否则返回NULL                            
|                                                                                
| 参  数：                                                                       
|         1   In  INT    iCameraID		   摄像头ID，回调函数参数			输入参数
|         2   In  INT	 *piRawToRGBType   RAW转RGB算法编号				输入参数
|                                                                                
| 注  意：                                                                       
|         无                                                                     
\*---------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetBayertoRGBType(INT iCameraID, INT *piRawToRGBType);


/*-----------------------------------------------------------------------------------*\
| 功  能：显示RGB24位图像数据                                                        
|                                                                                   
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                                     
|                                                                                   
| 参  数：                                                                          
|         1  INT           iCameraID  相机编号（回调函数输入参数）           输入参数 
|         2  BYTE          *pbyRGB24  RGB24图像数据首地址                   输入参数
|         3  tDSFrameInfo  *psFrInfo  图像数据详细信息，参见tDSFrameInfo定义 输入参数 
|                                                                                   
| 注  意：                                                                          
|         无                                                                        
\*-----------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraDisplayRGB24(INT iCameraID, BYTE *pbyRGB24, tDSFrameInfo *psFrInfo);


/*-----------------------------------------------------------------------*\
| 功  能：显示ROI区域选择框                                             
|                                                                      
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                        
|                                                                      
| 参  数：                                                             
|         1  INT         iCameraID  相机编号（回调函数输入参数） 输入参数
|         2  tDSROISize  sROISize   ROI区域选择框位置和尺寸      输入参数
|                                                                      
| 注  意：                                                             
|         无                                                           
\*-----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraDisplayROISel(INT iCameraID, tDSROISize sROISize);


/*---------------------------------------------------------------------------*\
| 功  能：设置预览图像尺寸                                                 
|                                                                        
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                           
|                                                                        
| 参  数：                                                                
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数    
|         2  INT  iWidth     预览图像宽度                      输入参数                         
|         3  INT  iHeight    预览图像高度                      输入参数                         
|                                                                        
| 注  意：                                                                
|         无                                                              
\*---------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetDisplaySize(INT iCameraID, 
                                             INT iWidth, 
                                             INT iHeight);


/*----------------------------------------------------------------------------*\
| 功  能：开启录影功能                                                       
|                                                                           
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                             
|                                                                           
| 参  数：                                                                  
|         1  INT     iCameraID     相机编号（由相机初始化成功后返回） 输入参数 
|         2  LPCSTR  lpszFileName  视频文件的路径和名称               输入参数       
|                                                                           
| 注  意：                                                                  
|         无                                                                
\*----------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraStartRecordVideo(INT iCameraID, LPCSTR lpszFileName);

/*----------------------------------------------------------------------------*\
| 功  能：开启录影功能，不自动添加相机名和时间到录影文件名                                                     
|                                                                           
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                             
|                                                                           
| 参  数：                                                                  
|         1  INT     iCameraID     相机编号（由相机初始化成功后返回） 输入参数 
|         2  LPCSTR  lpszFileName  视频文件的路径和名称               输入参数       
|                                                                           
| 注  意：                                                                  
|         无                                                                
\*----------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraStartRecordVideoEx(INT iCameraID, LPCSTR lpszFileName);

/*----------------------------------------------------------------------*\
| 功  能：停止录影功能                                                  
|                                                                      
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                        
|                                                                      
| 参  数：                                                             
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数 
|                                                                      
| 注  意：                                                             
|         无                                                           
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraStopRecordVideo(INT iCameraID);


/*----------------------------------------------------------------------*\
| 功  能：设置视频文件最大占用空间                                      
|                                                                     
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                        
|                                                                     
| 参  数：                                                            
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回）	 输入参数 
|         2  INT  iFileSize  视频文件最大占用空间，单位：Mega BYTE 输入参数
|                                                                     
| 注  意：                                                            
|         录制时，当文件达到设置的最大占用空间，将生成新的视频文件       
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetRecordFileSize(INT iCameraID, INT iFileSize);


/*------------------------------------------------------------------------*\
| 功  能：获取视频文件的最大占用空间                                      
|                                                                       
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                         
|                                                                       
| 参  数：                                                              
|         1  INT  iCameraID    相机编号（由相机初始化成功后返回）		输入参数 
|         2  INT  *piFileSize  视频文件最大占用空间，单位：Mega BYTE  输出参数
|                                                                       
| 注  意：                                                              
|         无                                                            
\*------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetRecordFileSize(INT iCameraID, INT *piFileSize);


/*----------------------------------------------------------------------*\
| 功  能：设置视频文件的图像质量                                        
|                                                                     
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                        
|                                                                     
| 参  数：                                                            
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数 
|         2  INT  iQuality   视频文件的图像质量                输入参数
|                                                                     
| 注  意：                                                            
|         iQuality 值为 1：低等质量；2：中等质量；3：高等质量            
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetRecordAVIQuality(INT iCameraID, INT iQuality);


/*-----------------------------------------------------------------------*\
| 功  能：获取视频文件的图像质量                                         
|                                                                      
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                         
|                                                                      
| 参  数：                                                             
|         1  INT  iCameraID   相机编号（由相机初始化成功后返回） 输入参数 
|         2  INT  *piQuality  视频文件的图像质量                输出参数
|                                                                      
| 注  意：                                                             
|         *piQuality 值为 1：低等质量；2：中等质量；3：高等质量           
\*-----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetRecordAVIQuality(INT iCameraID, INT *piQuality);


/*----------------------------------------------------------------------*\
| 功  能：设置视频文件的图像压缩编码器                                  
|                                                                     
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                       
|                                                                     
| 参  数：                                                            
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数 
|         2  INT  iCodeType  视频文件的图像压缩编码器          输入参数
|                                                                     
| 注  意：                                                            
|         iCodeType 值为 1：MPEG4；                                   
\*----------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetRecordEncoder(INT iCameraID, INT iCodeType);


/*------------------------------------------------------------------------*\
| 功  能：获取视频文件的图像压缩编码器                                    
|                                                                       
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                         
|                                                                       
| 参  数：                                                              
|         1  INT  iCameraID    相机编号（由相机初始化成功后返回） 输入参数 
|         2  INT  *piCodeType  视频文件的图像压缩编码器          输出参数
|                                                                       
| 注  意：                                                              
|         *piCodeType 值为 1：MPEG4；                                   
\*------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetRecordEncoder(INT iCameraID, INT *piCodeType);


/*------------------------------------------------------------------------*\
| 功  能：获取录影的帧率                                                 
|                                                                       
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                         
|                                                                       
| 参  数：                                                              
|         1  INT    iCameraID  相机编号（由相机初始化成功后返回） 输入参数 
|         2  float  *pfRate    录影的帧率，单位：FPS             输出参数
|                                                                       
| 注  意：                                                              
|         无                                                            
\*------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetRecordRate(INT iCameraID, float *pfRate);


/*-----------------------------------------------------------------------------------*\
| 功  能：将图像数据写入视频文件                                                     
|                                                                                  
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                                     
|                                                                                  
| 参  数：                                                                         
|         1  INT           iCameraID  相机编号（回调函数输入参数）           输入参数 
|         2  BYTE          *pbyRGB    RGB24位图像数据首地址                 输入参数
|         3  tDSFrameInfo  *psFrInfo  图像数据详细信息，参见tDSFrameInfo定义 输入参数
|                                                                                  
| 注  意：                                                                         
|         无                                                                       
\*-----------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraRecordFrame(INT iCameraID, BYTE *pbyRGB, tDSFrameInfo *psFrInfo);


/*---------------------------------------------------------------------------------*\
| 功  能：获取相机图像数据首地址                                                   
|                                                                                
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                                   
|                                                                                
| 参  数：                                                                       
|         1  INT           iCameraID    相机编号（由相机初始化成功后返回） 输入参数 
|         2  emDSDataType  emDataType   图像数据类型                      输入参数
|         3  BYTE          **pbyBuffer  图像数据首地址                    输出参数
|                                                                                
| 注  意：                                                                       
|         1、emDataType参考emDSDataType枚举类型                                   
|         2、与CameraReleaseImageBuffer配对使用                                   
\*---------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetImageBuffer(INT iCameraID, emDSDataType emDataType, BYTE **pbyBuffer);


/*--------------------------------------------------------------------------------------------*\
| 功  能：释放相机图像数据首地址                                                            
|                                                                                         
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                                            
|                                                                                         
| 参  数：                                                                                
|         1  INT           iCameraID   相机编号（由相机初始化成功后返回） 输入参数           
|         2  emDSDataType  emDataType  图像数据类型                      输入参数         
|         3  BYTE          *pbyBuffer  图像数据首地址                    输入参数          
|                                                                                         
| 注  意：                                                                                
|         1、emDataType参考emDSDataType枚举类型                                            
|         2、与CameraGetImageBuffer配对使用，pbyBuffer指向的数据处理完成，需调用此接口释放数据
|         3、CameraReleaseImageBuffer的数据类型必需与CameraGetImageBuffer的一致              
\*--------------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraReleaseImageBuffer(INT iCameraID, emDSDataType emDataType, BYTE *pbyBuffer);


/*------------------------------------------------------------------------------------*\
| 功  能：获取相机帧数统计信息                                                        
|                                                                                   
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                                      
|                                                                                   
| 参  数：                                                                           
|         1  INT            iCameraID      相机编号（由相机初始化成功后返回） 输入参数 
|         2  tDSFrameCount  *psFrameCount  帧数统计信息                      输出参数
|                                                                                   
| 注  意：                                                                           
|         无                                                                        
\*------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetFrameCount(INT iCameraID, tDSFrameCount *psFrameCount);


/*---------------------------------------------------------------------------------*\
| 功  能：保存相机当前图像数据到指定文件                                           
|                                                                               
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                                  
|                                                                               
| 参  数：                                                                       
|         1  INT     iCameraID     相机编号（由相机初始化成功后返回） 输入参数     
|         2  LPCSTR  lpszFileName  图像文件路径和名称                输入参数    
|         3  BYTE    byFileType    图像文件类型                      输入参数    
|         4  BYTE    byQuality     JPEG类型图像文件的图像质量        输入参数     
|                                                                               
| 注  意：                                                                       
|         1、byFileType参考emDSFileType枚举类型，对各值作或运算，即可保存多类型文件 
|         2、byQuality只在JPEG类型中生效，范围是1~100，其他类型设置任意值。
\*---------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraCaptureFile(INT iCameraID, LPCSTR lpszFileName, BYTE byFileType, BYTE byQuality);


/*--------------------------------------------------------------------------------------*\
| 功  能：保存RGB24位图像数据到BMP文件                                                  
|                                                                                     
| 返回值：调用成功返回STATUS_OK，否则返回错误代码                                        
|                                                                                     
| 参  数：                                                                             
|         1  INT           iCameraID     相机编号（回调函数输入参数）           输入参数 
|         2  LPCSTR        lpszFileName  图像文件路径和名称                     输入参数
|         3  BYTE          *pbyRGB24     RGB24位图像数据首地址                  输入参数
|         4  tDSFrameInfo  *psFrInfo     图像数据详细信息，参见tDSFrameInfo定义 输入参数 
|                                                                                     
| 注  意：                                                                             
|         无                                                                          
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSaveBMPFile(INT iCameraID, LPCSTR lpszFileName, BYTE *pbyRGB24, tDSFrameInfo *psFrInfo);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取相机图像数据缓存
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT           iCameraID        相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE          *pbyImageBuffer  相机图像数据缓存首地址             输出参数
|         3  emDSDataType  emDataType       相机图像数据类型                   输入参数
|         4  tDSFrameInfo  &sFrInfo         本帧图像数据的基本信息             输出参数
|
| 注  意：
|         1、10位、12位、16位的图像数据均采用16位（即2字节）存储；
|         2、16位数据的排列方式是“Little-Endian”，即低位数据在前字节，高位数据在后字节；
|         3、图像宽度、高度和数据位数均可在“&sFrInfo”中获得；
|         4、图像数据总字节数 = 图像宽度 X 图象高度 X 每像素字节数。
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetImageData(INT iCameraID, 
                                          BYTE *pbyImageBuffer,
                                  emDSDataType emDataType,
                                  tDSFrameInfo &sFrInfo);


/*--------------------------------------------------------------------------------------*\
| 功  能：保存当前显示图像
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT           iCameraID        相机编号（由相机初始化成功后返回） 输入参数
|         2  LPCSTR        lpFileName       图像文件的路径和名称               输入参数
|         3  BYTE          *pbyImageBuffer  图像数据缓存首地址                 输入参数
|         4  tDSFrameInfo  *psFrInfo        图像数据的帧信息                   输入参数
|         5  BYTE          byFileType       图像文件类型                       输入参数
|         6  BYTE          byQuality        JPEG类型图像文件的图像质量         输入参数
|
| 注  意：
|         1、byQuality只在JPEG类型中生效，范围是1~100，其他类型设置任意值。
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSaveCurrentImage(INT iCameraID, 
                                            LPCSTR lpszFileName,
                                              BYTE *pbyImageBuffer,
                                      tDSFrameInfo *psFrInfo,
                                              BYTE byFileType, 
                                              BYTE byQuality);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置图像翻转状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSMirrorDirection  emDir      图像翻转方向标识（水平或垂直）     输入参数
|         3  BOOL                 bEnable    图像翻转状态                      输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetMirror(INT iCameraID, emDSMirrorDirection emDir, BOOL bEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取图像翻转状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSMirrorDirection  emDir      图像翻转方向标识（水平或垂直）     输入参数
|         3  BOOL                 *pbEnable  图像翻转状态                       输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetMirror(INT iCameraID, emDSMirrorDirection emDir, BOOL *pbEnable);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置图像选择状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID  相机编号（由相机初始化成功后返回）				输入参数
|         2  INT	iRotation   图像旋转参数									输入参数
|
| 注  意：
|        旋转参数定义：	0:未旋转 1:顺时针旋转90度 2:逆时针旋转90度
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetRotation(INT iCameraID, INT iRotation);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取图像选择状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID	相机编号（由相机初始化成功后返回）				输入参数
|         2  INT	*piRotation	图像旋转参数									输出参数
|
| 注  意：
|       旋转参数定义：	0:不旋转 1:顺时针旋转90度 2:逆时针旋转90度
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetRotation(INT iCameraID, INT *piRotation);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置图像单色状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bEnable    图像单色状态                       输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetMonochrome(INT iCameraID, BOOL bEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取图像单色状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbEnable  图像单色状态                       输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetMonochrome(INT iCameraID, BOOL *pbEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置图像反色状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bEnable    图像反色状态                       输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetInverse(INT iCameraID, BOOL bEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取图像反色状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbEnable  图像反色状态                       输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetInverse(INT iCameraID, BOOL *pbEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置自动曝光状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bState     自动曝光状态                       输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetAeState(INT iCameraID, BOOL bState);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取自动曝光状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbAeState  自动曝光状态                       输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetAeState(INT iCameraID, BOOL *pbAeState);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置自动曝光目标值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  byAeTarget  自动曝光目标值                     输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetAeTarget(INT iCameraID, BYTE byAeTarget);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取自动曝光目标值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID     相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  *pbyAeTarget  自动曝光目标值                     输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetAeTarget(INT iCameraID, BYTE *pbyAeTarget);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置自动曝光参考区域
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iHOff      水平偏移量，单位：像素             输入参数
|         3  INT  iVOff      垂直偏移量，单位：像素             输入参数
|         4  INT  iWidth     参考区域宽度，单位：像素           输入参数
|         5  INT  iHeight    参考区域高度，单位：像素           输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetAEWindow(INT iCameraID, 
                                          INT iHOff, 
                                          INT iVOff, 
                                          INT iWidth, 
                                          INT iHeight);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置曝光时间
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT     iCameraID      相机编号（由相机初始化成功后返回） 输入参数
|         2  UINT64  uExposureTime  曝光时间，单位：0.1us              输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetExposureTime(INT iCameraID, UINT64 uExposureTime);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取曝光时间
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT     iCameraID        相机编号（由相机初始化成功后返回） 输入参数
|         2  UINT64  *puExposureTime  曝光时间，单位：0.1us              输出参数
|         3  UINT64  *puExpTimeMax    曝光时间最大值，单位：0.1us        输出参数
|         4  UINT64  *puExpTimeMin    曝光时间最小值，单位：0.1us        输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetExposureTime(INT iCameraID, 
                                           UINT64 *puExposureTime,
                                           UINT64 *puExpTimeMax, 
                                           UINT64 *puExpTimeMin);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置自动曝光模式
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  INT	iMode		曝光模式编号                     输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetAeMode(INT iCameraID, INT iMode);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取自动曝光模式
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  INT	*piMode		曝光模式编号						输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetAeMode(INT iCameraID, INT *piMode);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置自动曝光算法
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  INT	iScheme		自动曝光算法编号                     输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetAeScheme(INT iCameraID, INT iScheme);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取自动曝光算法
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  INT	*piScheme	自动曝光算法编号                  输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetAeScheme(INT iCameraID, INT *piScheme);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置曝光增益
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID    相机编号（由相机初始化成功后返回） 输入参数
|         2  float  fAnalogGain  曝光增益值，单位：倍数             输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetAnalogGain(INT iCameraID, float fAnalogGain);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取曝光增益
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID      相机编号（由相机初始化成功后返回） 输入参数
|         2  float  *pfAnalogGain  曝光增益值，单位：倍数             输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetAnalogGain(INT iCameraID, float *pfAnalogGain);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置消除图像杂点状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bEnable    消除图像杂点状态                   输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetDeadPixelCorrectionState(INT iCameraID, BOOL bEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取消除图像杂点状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbEnable  消除图像杂点状态                   输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetDeadPixelCorrectionState(INT iCameraID, BOOL *pbEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：更新图像杂点位置坐标
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|
| 注  意：
|         用于在特别环境时，更新相机保存的杂点位置坐标
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraCreateDefectCorrectionInfo(INT iCameraID);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置图像锐化状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bEnable    图像锐化状态                       输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetEdgeEnhance(INT iCameraID, BOOL bEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取图像锐化状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbEnable  图像锐化状态                       输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetEdgeEnhance(INT iCameraID, BOOL *pbEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置图像锐化目标值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  byEdgeGain  图像锐化目标值                     输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetEdgeGain(INT iCameraID, BYTE byEdgeGain);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取图像锐化目标值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID     相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  *pbyEdgeGain  图像锐化目标值                     输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetEdgeGain(INT iCameraID, BYTE *pbyEdgeGain);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置图像降噪状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bReduction  图像降噪状态                       输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetNoiseReductionState(INT iCameraID, BOOL bReduction);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取图像降噪状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID     相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbReduction  图像降噪状态                       输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetNoiseReductionState(INT iCameraID, BOOL *pbReduction);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置图像降噪目标值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iReduction  图像降噪目标值                     输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetNoiseReductionGain(INT iCameraID, INT iReduction);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取图像降噪目标值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID     相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  *piReduction  图像降噪目标值                     输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetNoiseReductionGain(INT iCameraID, INT *piReduction);


/*--------------------------------------------------------------------------------------*\
| 功  能：一次性白平衡功能
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetOnceWB(INT iCameraID);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置自动白平衡状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bAWBState  自动白平衡状态                     输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
// 暂不开放
//DS_API emDSCameraStatus CameraSetAWBState(INT iCameraID, BOOL bAWBState);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取自动白平衡状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID    相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbAWBState  自动白平衡状态                     输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
// 暂不开放
//DS_API emDSCameraStatus CameraGetAWBState(INT iCameraID, BOOL *pbAWBState);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置白平衡参考区域
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iHOff      水平偏移量，单位：像素             输入参数
|         3  INT  iVOff      垂直偏移量，单位：像素             输入参数
|         4  INT  iWidth     参考区域宽度，单位：像素           输入参数
|         5  INT  iHeight    参考区域高度，单位：像素           输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetWBWindow(INT iCameraID, 
                                          INT iHOff, 
                                          INT iVOff, 
                                          INT iWidth, 
                                          INT iHeight);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置一次黑电平自动调节
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraBlackLevelOnce(INT iCameraID);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置黑电平调节使能状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回）		输入参数
|         2  BOOL  bBlackLevel黑电平使能状态 TRUE：使能，FALSE：取消  输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetBlackLevelState(INT iCameraID, BOOL bBlackLevel);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取黑电平调节使能状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID		相机编号（由相机初始化成功后返回）		输入参数
|         2  BOOL  *pbBlackLevel	黑电平使能状态                       输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetBlackLevelState(INT iCameraID, BOOL *pbBlackLevel);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置黑电平参数
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID		相机编号(由相机初始化成功后返回)  输入参数
|         2  INT iLevelForR     红色分量黑电平					输入参数
|         3  INT iLevelForG     绿色分量黑电平					输入参数
|         4  INT iLevelForB     蓝色分量黑电平			        输入参数
|         5  INT iMaxValue      最大允许值   			        输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetBlackLevel(INT iCameraID, 
											INT iLevelForR,
											INT iLevelForG,
											INT iLevelForB,
											INT iMaxValue=4095);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取黑电平参数
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID		相机编号(由相机初始化成功后返回)  输入参数
|         2  INT *piLevelForR   红色分量黑电平					输出参数
|         3  INT *piLevelForG   绿色分量黑电平					输出参数
|         4  INT *piLevelForB   蓝色分量黑电平			        输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetBlackLevel(INT iCameraID, 
											INT *piLevelForR,
											INT *piLevelForG,
											INT *piLevelForB);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置各色彩通道的增益
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  float  fRGain     红通道增益                         输入参数
|         3  float  fGGain     绿通道增益                         输入参数
|         4  float  fBGain     蓝通道增益                         输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetGain(INT iCameraID, 
                                    float fRGain, 
                                    float fGGain, 
                                    float fBGain);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取各色彩通道的增益
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  float  *pfRGain   红通道增益                         输出参数
|         3  float  *pfGGain   绿通道增益                         输出参数
|         4  float  *pfBGain   蓝通道增益                         输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetGain(INT iCameraID, 
                                    float *pfRGain, 
                                    float *pfGGain, 
                                    float *pfBGain);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置色彩饱和度状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bEnable    色彩饱和度状态                     输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetColorEnhancement(INT iCameraID, BOOL bEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取色彩饱和度状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbEnable  色彩饱和度状态                     输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetColorEnhancement(INT iCameraID, BOOL *pbEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置色彩饱和度值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID     相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  bySaturation  色彩饱和度值                       输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetSaturation(INT iCameraID, BYTE bySaturation);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取色彩饱和度值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID       相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  *pbySaturation  色彩饱和度值                       输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetSaturation(INT iCameraID, BYTE *pbySaturation);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置伽玛调节使能状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回）		输入参数
|         2  BOOL  bGamma     伽玛使能状态 TRUE：势能，FALSE：取消    输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetGammaState(INT iCameraID, BOOL bGamma);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取伽玛调节使能状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回）		输入参数
|         2  BOOL  *pbGamma   伽玛使能状态指针					    输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetGammaState(INT iCameraID, BOOL *pbGamma);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置伽玛值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  byGamma    伽玛值                             输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetGamma(INT iCameraID, BYTE byGamma);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取伽玛值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  *pbyGamma  伽玛值                             输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetGamma(INT iCameraID, BYTE *pbyGamma);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置对比度调节使能状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回）		输入参数
|         2  BOOL  bContrast  对比度使能状态 TRUE：势能，FALSE：取消  输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetContrastState(INT iCameraID, BOOL bContrast);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取对比度调节使能状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回）		输入参数
|         2  BOOL  *pbContast 对比度使能状态指针					    输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetContrastState(INT iCameraID, BOOL *pbContrast);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置对比度值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  byContrast  对比度值                           输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetContrast(INT iCameraID, BYTE byContrast);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取对比度值
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID     相机编号（由相机初始化成功后返回） 输入参数
|         2  BYTE  *pbyContrast  对比度值                           输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetContrast(INT iCameraID, BYTE *pbyContrast);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置帧率
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT             iCameraID     相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSFrameSpeed  emFrameSpeed  帧率                               输入参数
|
| 注  意：
|         emFrameSpeed参考emDSFrameSpeed枚举类型
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetFrameSpeed(INT iCameraID, emDSFrameSpeed emFrameSpeed);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取帧率
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT             iCameraID       相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSFrameSpeed  *pemFrameSpeed  帧率                               输出参数
|
| 注  意：
|         emFrameSpeed参考emDSFrameSpeed枚举类型
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetFrameSpeed(INT iCameraID, emDSFrameSpeed *pemFrameSpeed);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置抗频闪状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bEnable    抗频闪状态                         输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetAntiFlick(INT iCameraID, BOOL bEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取抗频闪状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbEnable  抗频闪状态                         输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetAntiFlick(INT iCameraID, BOOL *pbEnable);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置抗频闪参数
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                 iCameraID    相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSLightFrequency  emFrequency  抗频闪参数（灯光频率）             输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetLightFrequency(INT iCameraID, emDSLightFrequency emFrequency);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取抗频闪参数
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                 iCameraID      相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSLightFrequency  *pemFrequency  抗频闪参数（灯光频率）             输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetLightFrequency(INT iCameraID, emDSLightFrequency *pemFrequency);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置相机预定义的分辨率模式
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT   iSel       预定义的分辨率索引号               输入参数
|         3  BOOL  bCapture   预览或拍照模式的分辨率             输入参数
|
| 注  意：
|         1、相机支持的预定义分辨率模式通过函数CameraGetCapability获取
|         2、bCapture为TRUE时设置拍照模式的分辨率，否则设置预览模式的分辨率
|         3、使用该函数，设置相机的分辨率后，需调用函数CameraPlay，启动视频流
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetImageSizeSel(INT iCameraID, INT iSel, BOOL bCapture = FALSE);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取相机预定义的分辨率模式
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT   *piSel     预定义的分辨率索引号               输出参数
|         3  BOOL  bCapture   预览或拍照模式的分辨率             输入参数
|
| 注  意：
|         1、bCapture为TRUE时获取拍照模式的分辨率，否则获取预览模式的分辨率
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetImageSizeSel(INT iCameraID, INT *piSel, BOOL bCapture = FALSE);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置相机的分辨率模式（ROI）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bReserve   保留参数，暂未启用			       输入参数
|         3  INT   iHOff      水平偏移量，单位：像素             输入参数
|         4  INT   iVOff      垂直偏移量，单位：像素             输入参数
|         5  INT   iWidth     区域宽度，单位：像素               输入参数
|         6  INT   iHeight    区域高度，单位：像素               输入参数
|         7  BOOL  bCapture   设置的分辨率是预览或拍照           输入参数
|
| 注  意：
|         1、bCapture为TRUE时设置拍照模式的分辨率，否则设置预览模式的分辨率
\*--------------------------------------------------------------------------------------*/
DS_API  emDSCameraStatus CameraSetImageSize(INT iCameraID,
                                           BOOL bReserve,
                                            INT iHOff,
                                            INT iVOff,
                                            INT iWidth,
                                            INT iHeight, 
                                           BOOL bCapture = FALSE);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取相机的分辨率模式（ROI）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbROI     设置的分辨率是预定义或ROI          输出参数
|         3  INT   *piHOff    水平偏移量，单位：像素             输出参数
|         4  INT   *piVOff    垂直偏移量，单位：像素             输出参数
|         5  INT   *piWidth   区域宽度，单位：像素               输出参数
|         6  INT   *piHeight  区域高度，单位：像素               输出参数
|         7  BOOL  bCapture   设置的分辨率是预览或拍照           输入参数
|
| 注  意：
|         1、bROI为TRUE时当前是ROI分辨率，否则当前是预定义分辨率
|         2、bCapture为TRUE时获取拍照模式的分辨率，否则获取预览模式的分辨率
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetImageSize(INT iCameraID, 
                                          BOOL *pbROI,
                                           INT *piHOff,
                                           INT *piVOff,
                                           INT *piWidth,
                                           INT *piHeight,
                                          BOOL bCapture = FALSE);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置传输的图像数据包长度（千兆网相机）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iPack      数据包长度索引号                   输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetTransPackLen(INT iCameraID, INT iPack);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取传输的图像数据包长度（千兆网相机）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  *piPack    数据包长度索引号                   输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetTransPackLen(INT iCameraID, INT *piPack);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置时间戳单位（千兆网相机）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iUnit      时间戳单位索引号                   输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetTimeStampUnit(INT iCameraID, INT iUnit);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取时间戳单位（千兆网相机）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  *piUnit    时间戳单位索引号                   输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetTimeStampUnit(INT iCameraID, INT *piUnit);


/*--------------------------------------------------------------------------------------*\
| 功  能：保存相机的参数设置
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSParameterTeam  emTeam     保存参数的组别（共4组）            输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSaveParameter(INT iCameraID, emDSParameterTeam emTeam);


/*--------------------------------------------------------------------------------------*\
| 功  能：读取相机的参数设置，并使其生效
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSParameterTeam  emTeam     保存参数的组别（共4组）            输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraReadParameter(INT iCameraID, emDSParameterTeam emTeam);

/*--------------------------------------------------------------------------------------*\
| 功  能：保存相机的参数到配置文件
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSaveParameterToIni(INT iCameraID);

/*--------------------------------------------------------------------------------------*\
| 功  能：从配置文件读取相机的参数，并使其生效
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraLoadParameterFromIni(INT iCameraID);

/*--------------------------------------------------------------------------------------*\
| 功  能：保存相机的参数到ini文件，并使其生效
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|		  2	 LPCSTR				lpszIniPath 保存参数的ini文件的路径
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSaveParameterToIniEx(INT iCameraID, LPCSTR lpszIniPath);

/*--------------------------------------------------------------------------------------*\
| 功  能：从配置文件读取相机的参数，并使其生效
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|		  2	 LPCSTR				lpszIniPath 保存参数的ini文件的路径
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraLoadParameterFromIniEx(INT iCameraID, LPCSTR lpszIniPath);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取当前使用的保存参数组别
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSParameterTeam  *pemTeam   保存参数的组别（共4组）            输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetCurrentParameterTeam(INT iCameraID, emDSParameterTeam *pemTeam);


/*--------------------------------------------------------------------------------------*\
| 功  能：读出相机默认出厂参数设置
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraLoadDefaultParameter(INT iCameraID);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置相机输出的图像数据类型
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT           iCameraID    相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSDataType  emMediaType  图像数据类型                       输入参数
|
| 注  意：
|         各款相机支持的数据类型，请参考相关手册
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetMediaType(INT iCameraID, emDSDataType emMediaType);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取相机输出的图像数据类型
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT           iCameraID      相机编号（由相机初始化成功后返回） 输入参数
|         2  emDSDataType  *pemMediaType  图像数据类型                       输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetMediaType(INT iCameraID, emDSDataType *pemMediaType);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置相机输出的图像数据类型
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iSel       图像数据类型索引号                 输入参数
|
| 注  意：
|         各款相机支持的数据类型，请参考相关手册
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetMediaTypeSel(INT iCameraID, INT iSel);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取相机输出的图像数据类型
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  *piSel     图像数据类型索引号                 输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetMediaTypeSel(INT iCameraID, INT *piSel);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置色彩矫正矩阵
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iSel       图像色彩矩阵索引号                 输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetRgbMatrixSel(INT iCameraID, INT iSel);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取当前的色彩矫正矩阵
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  *piSel     图像色彩矩阵索引号                 输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetRgbMatrixSel(INT iCameraID, INT *piSel);


/*--------------------------------------------------------------------------------------*\
| 功  能：使对应的色彩矫正矩阵数据生效
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT     iCameraID      相机编号（由相机初始化成功后返回） 输入参数
|         2  float   fMatrix[3][3]  图像色彩矩阵数据                   输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraApplyColorMatrix(INT iCameraID, float fMatrix[3][3]);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置工作模式（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iSnapMode  工作模式                          输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetTriggerMode(INT iCameraID, INT iSnapMode);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取工作模式（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID     相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  *piSnapMode   工作模式                         输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetTriggerMode(INT iCameraID, INT *piSnapMode);


/*--------------------------------------------------------------------------------------*\
| 功  能：触发相机输出一帧图像（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraTrigger(INT iCameraID);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置触发电平参数
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|		  1  INT  iCameraID  相机编号（由相机初始化成功后返回）	输入参数
|         2  INT  iPulseStyle		触发电平边缘极性				输入参数
|	      3  UINT uPulseWidth		脉冲电平时间					输入参数
|         4	 UINT uDelay			延时信息						输入参数
|		  5  UINT uFilterWidth		滤波器宽度					输入参数
|       
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetTriggerPulse(INT iCameraID, 
											  INT iPulseStyle,
											 UINT uPulseWidth,
											 UINT uDelay,
											 UINT uFilterWidth);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取触发电平参数
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回）	输入参数
|         2  INT  *piPulseStyle		触发电平边缘极性				输出参数
|	      3  UINT *puPulseWidth		脉冲电平时间					输出参数
|         4	 UINT *puDelay			延时信息						输出参数
|		  5  UINT *puFilterWidth	滤波器宽度				    输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetTriggerPulse(INT iCameraID, 
	                                          INT *piPulseStyle,
											 UINT *puPulseWidth,
											 UINT *puDelay,
											 UINT *puFilterWidth);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置闪光信号参数
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|		  1  INT  iCameraID  相机编号（由相机初始化成功后返回）	输入参数
|         2  INT  iPulseStyle		闪光信号边缘极性				输入参数
|	      3  UINT uPulseWidth		脉冲电平时间					输入参数
|         4	 UINT uDelay			延时信息						输入参数
|       
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetStrobePulse(INT iCameraID, INT iPulseStyle,UINT uPulseWidth,UINT uDelay);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取闪光信号参数信息
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回）	输入参数
|         2  INT  *piPulseStyle		闪光信号边缘极性				输出参数
|	      3  UINT *puPulseWidth		脉冲电平时间					输出参数
|         4	 UINT *puDelay			延时信息						输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetStrobePulse(INT iCameraID, INT *piPulseStyle,UINT *puPulseWidth,UINT *puDelay);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置触发模式的曝光模式索引
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回）	输入参数
|         2  INT  iSel				曝光模式索引   				输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetExposureSelForTrig(INT iCameraID, INT iSel);

/*--------------------------------------------------------------------------------------*\
| 功  能：获取触发模式的曝光模式索引闪光信号参数信息
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回）	输入参数
|         2  INT  *piSel			曝光模式索引					输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetExposureSelForTrig(INT iCameraID, INT *piSel);

/*--------------------------------------------------------------------------------------*\
| 功  能：设置图像采集的帧数（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回）	输入参数
|         2  BYTE  byCount    采集的帧数                         输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetTriggerCount(INT iCameraID, BYTE byCount);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置分辨率模式（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iSel       分辨率模式索引号                   输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetResolutionModeSelForTrig(INT iCameraID, INT iSel);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取分辨率模式（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  *piSel     分辨率模式索引号                   输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetResolutionModeSelForTrig(INT iCameraID, INT *piSel);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置预定义的图像尺寸（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  iSel       图像尺寸索引号                     输入参数
|
| 注  意：
|         iSel对应tDSImageSize的iIndex成员
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetImageSizeSelForTrig(INT iCameraID, INT iSel);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取预定义的图像尺寸（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT  iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT  *piSel     图像尺寸索引号                     输出参数
|
| 注  意：
|         *piSel对应tDSImageSize的iIndex成员
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetImageSizeSelForTrig(INT iCameraID, INT *piSel);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置曝光时间（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT     iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  UINT64  uExpTime   曝光时间，单位：0.1us              输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetExpTimeForTrig(INT iCameraID, UINT64 uExpTime);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取曝光时间（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT     iCameraID      相机编号（由相机初始化成功后返回） 输入参数
|         2  UINT64  *puExpTime     曝光时间，单位：0.1us              输出参数
|         3  UINT64  *puExpTimeMax  最大曝光时间，单位：0.1us          输出参数
|         4  UINT64  *puExpTimeMin  最小曝光时间，单位：0.1us          输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetExpTimeForTrig(INT iCameraID, 
                                             UINT64 *puExpTime,
                                             UINT64 *puExpTimeMax,
                                             UINT64 *puExpTimeMin);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置曝光增益（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  float  fGain      曝光增益，单位：倍数               输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetAnalogGainForTrig(INT iCameraID, float fGain);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取曝光增益（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT    iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  float  *pfGain    曝光增益，单位：倍数               输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetAnalogGainForTrig(INT iCameraID, float *pfGain);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置分辨率模式（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  bROI       是否为ROI模式或预定义模式          输入参数
|         3  INT   iHOff      水平偏移量，单位：像素             输入参数
|         4  INT   iVOff      垂直偏移量，单位：像素             输入参数
|         5  INT   iWidth     图像宽度，单位：像素               输入参数
|         6  INT   iHeight    图像高度，单位：像素               输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetImageSizeForTrig(INT iCameraID, 
                                                 BOOL bROI,
                                                  INT iHOff,
                                                  INT iVOff,
                                                  INT iWidth,
                                                  INT iHeight);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取分辨率模式（触发模式）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  BOOL  *pbROI     是否为ROI模式或预定义模式          输出参数
|         3  INT   *piHOff    水平偏移量，单位：像素             输出参数
|         4  INT   *piVOff    垂直偏移量，单位：像素             输出参数
|         5  INT   *piWidth   图像宽度，单位：像素               输出参数
|         6  INT   *piHeight  图像高度，单位：像素               输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetImageSizeForTrig(INT iCameraID, 
                                                 BOOL *pbROI,
                                                  INT *piHOff,
                                                  INT *piVOff,
                                                  INT *piWidth,
                                                  INT *piHeight);


/*--------------------------------------------------------------------------------------*\
| 功  能：写入相机序列号
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  INT   iLevel      序列号级别                         输入参数
|         3  BYTE  *pbySN      序列号数据                         输入参数
|         4  INT   iSize       序列号字节数                       输入参数
|         5  INT   *piWritten  实际写入的字节数                   输出参数
|
| 注  意：
|         1、iLevel的值为 1：一级序列号；2：二级序列号
|         2、序列号的字节数最大为：32字节
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraWriteSN(INT iCameraID, INT iLevel, BYTE *pbySN, INT iSize, INT *piWritten);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取相机序列号
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID  相机编号（由相机初始化成功后返回） 输入参数
|         2  INT   iLevel     序列号级别                         输入参数
|         3  BYTE  *pbySN     序列号数据                         输出参数
|         4  INT   iSize      序列号字节数                       输入参数
|         5  INT   *piReaded  实际获取的字节数                   输出参数
|
| 注  意：
|         1、iLevel的值为 1：一级序列号；2：二级序列号
|         2、序列号的字节数最大为：32字节
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraReadSN(INT iCameraID, INT iLevel, BYTE *pbySN, INT iSize, INT *piReaded);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取相机高级配置窗口
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT                iCameraID    相机编号（由相机初始化成功后返回） 输入参数
|         2  tPropertyPageInfo  *psPageInfo  相机高级配置窗口信息               输出参数
|         3  BYTE               byType       高级配置窗口类型                   输入参数
|
| 注  意：
|         byType的值为 0：设备逻辑配置窗口；1：图像处理模块窗口
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetPropertyPageInfo(INT iCameraID, tDSPropertyPageInfo *psPageInfo, BYTE byType);



/*--------------------------------------------------------------------------------------*\
| 功  能：显示相机高级配置窗口
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT   iCameraID    相机编号（由相机初始化成功后返回） 输入参数
|         2  HWND  hwndDisplay  预览窗口句柄                       输入参数
|         3  BYTE  byType       高级配置窗口类型                   输入参数
|
| 注  意：
|         byType的值为 0：设备逻辑配置窗口；1：图像处理模块窗口
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraShowSettingPage(INT iCameraID, HWND hwndDisplay, BYTE byType);


/*--------------------------------------------------------------------------------------*\
| 功  能：预留接口（可通过控制码实现不同的功能）
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT     iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  DWORD   dwCtrlCode  控制码                             输入参数
|         3  DWORD   dwParam     控制码的参数                       输入参数
|         4  LPVOID  lpData      控制码的数据                       输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraControl(INT iCameraID, 
                                    DWORD dwCtrlCode,
                                    DWORD dwParam,
                                   LPVOID lpData);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取SDK版本号
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  DWORD  adwVersion  SDK版本号 输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetSDKVersion(DWORD adwVersion[4]);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取用户自定义的GPIO输入口状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT     iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  USHORT  *puGPIO      GPIO输入口状态(高低电平)          输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetUserGPIOIn(INT iCameraID, USHORT *puGPIO);


/*--------------------------------------------------------------------------------------*\
| 功  能：获取用户自定义的GPIO输出口状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT     iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  USHORT  *puGPIO      GPIO输出口状态(高低电平)          输出参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraGetUserGPIOOut(INT iCameraID, USHORT *puGPIO);


/*--------------------------------------------------------------------------------------*\
| 功  能：设置用户自定义的GPIO输出口状态
|
| 返回值：调用成功返回STATUS_OK，否则返回错误代码
|
| 参  数：
|         1  INT     iCameraID   相机编号（由相机初始化成功后返回） 输入参数
|         2  USHORT  uGPIO      GPIO输出口状态(高低电平)            输入参数
|
| 注  意：
|         无
\*--------------------------------------------------------------------------------------*/
DS_API emDSCameraStatus CameraSetUserGPIOOut(INT iCameraID, USHORT uGPIO);


#endif
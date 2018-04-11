/*------------------------------------------------------------------------*\
| 功能:  定义相机操作用到的数据结构
| 版权:  版权所有                                                     
\*------------------------------------------------------------------------*/

#ifndef _DEV_PARAM_DEF
#define _DEV_PARAM_DEF

#define NAME_LEN_MAX    64
#define DESCR_LEN_MAX   32


/*------------------------------------------------------------------------*\
|                             摄像头硬件信息
\*------------------------------------------------------------------------*/
typedef struct
{
    UINT  uVendorID;                         // 厂商编号
    char  acVendorName[NAME_LEN_MAX];        // 厂商名称
    char  acProductSeries[NAME_LEN_MAX];     // 产品系列
    char  acProductName[NAME_LEN_MAX];       // 产品名称
    char  acFriendlyName[NAME_LEN_MAX];      // 产品昵称，用于区分同系列的多个产品
    char  acDevFileName[NAME_LEN_MAX];       // 逻辑驱动文件名称
    char  acFileName[NAME_LEN_MAX];          // 内核驱动逻辑设备名
    char  acFirmwareVersion[NAME_LEN_MAX];   // 产品固件版本
    char  acSensorType[NAME_LEN_MAX];        // 图像传感器类型
    char  acPortType[NAME_LEN_MAX];          // 接口类型
} tDSCameraDevInfo;


/*------------------------------------------------------------------------*\
|                             图像传感器信息
\*------------------------------------------------------------------------*/
typedef struct
{
    UINT  uSensorType;    // 传感器类型
    UINT  uSensorCount;   // 传感器数量
    BOOL  bMono;          // 传感器色彩类型（黑白或彩色）
    UINT  uPixForm;       // 传感器像素排列方式。例RGRG或GRGR等
    INT   iFieldCount;    // 传感器扫描方式，1为逐行扫描，大于1的为对应场数的隔行扫描
} tDSSensorInfo;


/*------------------------------------------------------------------------*\
|                             帧数据信息
\*------------------------------------------------------------------------*/
typedef struct
{
    UINT    uiMediaType;       // 图像数据类型，参考“DVP_Define.h”头文件中枚举类型“emDSDataType”的值
    UINT    uBytes;            // 图像数据字节数
    UINT    uiHeight;          // 图像高度
    UINT    uiWidth;           // 图像宽度
    UINT    uHOff;             // 图像水平偏移量
    UINT    uVOff;             // 图像垂直偏移量
    BOOL    bHFlip;            // 描述是否水平翻转
    BOOL    bVFlip;            // 描述是否垂直翻转
    BOOL    bTriggered;        // 描述是否为触发帧
    BOOL    bTimeStampValid;   // 描述时间戳有效
    BOOL    bExpTimeValid;     // 描述曝光时间有效
	UINT	uFrameID;		   // 帧ID号，一般情况下，每帧加1
    UINT64  uiTimeStamp;       // 时间戳，单位：由具体设备确定
    UINT64  uiExpTime;         // 曝光时间，单位：由具体设备确定
} tDSFrameInfo;


/*------------------------------------------------------------------------*\
|                            ROI描述结构
\*------------------------------------------------------------------------*/
typedef struct
{
    INT  iWidth;     // ROI宽度
    INT  iHeight;    // ROI高度
    INT  iHOffset;   // ROI水平偏移量
    INT  iVOffset;   // ROI垂直偏移量
} tDSROISize;


/*------------------------------------------------------------------------*\
|                            图像尺寸结构
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;                         // 图像尺寸索引号，值从0开始
    char  acDescription[DESCR_LEN_MAX];   // 图像尺寸描述名称
    BOOL  bIsBIN;                         // 是否为BIN模式的图像尺寸
    BOOL  bIsSkip;                        // 是否为SKIP模式的图像尺寸
    BOOL  bIsZoom;                        // 是否为缩放的图像尺寸
    INT	  iResolutionMode;                // 分辨率模式，值为tDSResolutionMode的iIndex成员
    INT   iWidth;                         // 图像宽度
    INT   iHeight;                        // 图像高度
    INT	  iHOffset;                       // 图像垂直偏移量
    INT   iVOffset;                       // 图像水平偏移量
} tDSImageSize;


/*------------------------------------------------------------------------*\
|                            视频尺寸范围
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iHeightMax;       // 图像高度最大值
    INT   iHeightMin;       // 图像高度最小值
    INT   iHeightDefault;   // 图像高度默认值
    INT   iWidthMax;        // 图像宽度最大值
    INT   iWidthMin;        // 图像宽度最小值
    INT   iWidthDefault;    // 图像宽度默认值
    BOOL  bRoi;             // 是否支持ROI模式。TRUE:支持；FALSE:不支持
} tDSImageSizeRange;


/*------------------------------------------------------------------------*\
|                               帧率信息
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;                         // 帧率索引号，值从0开始
    char  acDescription[DESCR_LEN_MAX];   // 帧率描述名称
} tDSFrameSpeed;


/*------------------------------------------------------------------------*\
|                             曝光时间信息
\*------------------------------------------------------------------------*/
typedef struct
{
    INT     iIndex;                         // 曝光时间索引号，值从0开始
    char    acDescription[DESCR_LEN_MAX];   // 曝光时间描述名称
    UINT64  uiTargetMin;                    // 最小自动曝光目标值
    UINT64  uiTargetMax;                    // 最大自动曝光目标值
    UINT64  uiTargetDefault;                // 默认自动曝光目标值
    float   fAnalogGainMin;                 // 最小曝光增益
    float   fAnalogGainMax;                 // 最大曝光增益
    float   fAnalogGainDefault;             // 默认增益
    float   fAnalogGainStep;                // 增益调节的步进值(最小单位) 
    UINT64  uiExposureTimeMin;              // 最小曝光时间
    UINT64  uiExposureTimeMax;              // 最大曝光时间
    UINT64  uiExposureTimeDefault;          // 默认曝光时间
    UINT64  uiExposureTimeStep;             // 曝光时间调节的步进值（最小单位）
} tDSExposure;


/*------------------------------------------------------------------------*\
|                                曝光参数
\*------------------------------------------------------------------------*/
typedef struct 
{
    BOOL    bAutoEnable;         // 是否自动曝光
    UINT64  iAutoExposureTarget;   // 自动曝光目标值
    float   fAnalogGain;         // 曝光增益
    UINT64  iExposureTime;         // 曝光时间
} tDSExposureParam;


/*------------------------------------------------------------------------*\
|                               触发模式信息
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;                         // 触发模式索引号，值从0开始
    char  acDescription[DESCR_LEN_MAX];   // 触发模式的描述名称
} tDSTrigger;


/*------------------------------------------------------------------------*\
|                               帧数据统计信息
\*------------------------------------------------------------------------*/
typedef struct
{
    INT  iCap;     // 计算机获取的帧数
    INT  iTotal;   // 相机产生的总帧数（计算机获取的帧数 与 相机丢失的帧数 之和）
    INT  iLost;    // 相机丢失的帧数
} tDSFrameCount;


/*------------------------------------------------------------------------*\
|                               图像数据类型
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;                         // 图像数据类型索引号，值从0开始
    char  acDescription[DESCR_LEN_MAX];   // 图像数据类型的描述名称
    INT   iMediaType;                     // 图像数据类型，参考“DVP_Define.h”头文件中枚举类型“emDSDataType”的值
} tDSMediaType;


/*------------------------------------------------------------------------*\
|                                分辨率模式
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;                         // 分辨率模式索引号，值从0开始
    char  acDescription[DESCR_LEN_MAX];   // 分辨率模式的描述名称
    BOOL  bBin;                           // 是否为BIN模式
    BOOL  bSkip;                          // 是否为SKIP模式
    BOOL  bZoom;                          // 是否为缩放模式
    INT   iWidthMax;                      // 该模式中图像宽度最大值
    INT   iHeightMax;                     // 该模式中图像高度最大值
    INT   iHSampling;                     // 水平采样模式，该值的最终解释由具体设备决定
    INT   iVSampling;                     // 垂直采样模式，该值的最终解释由具体设备决定
    INT   iReadOut;                       // 读取模式，该值的最终解释由具体设备决定
} tDSResolutionMode;


/*------------------------------------------------------------------------*\
|                              图像伽玛数值范围
\*------------------------------------------------------------------------*/
typedef struct
{
    INT  iMin;       // 图像伽玛最小值
    INT  iMax;       // 图像伽玛最大值
    INT  iDefault;   // 图像伽玛默认值
} tDSGammaRange;


/*------------------------------------------------------------------------*\
|                             图像对比度数值范围
\*------------------------------------------------------------------------*/
typedef struct
{
    INT  iMin;       // 图像对比度最小值
    INT  iMax;       // 图像对比度最大值
    INT  iDefault;   // 图像对比度默认值
} tDSContrastRange;


/*------------------------------------------------------------------------*\
|                              RGB通道增益范围
\*------------------------------------------------------------------------*/
typedef struct
{
    float  fRGainMin;       // 红通道增益最小值
    float  fRGainMax;       // 红通道增益最大值
    float  fRGainDefault;   // 红通道增益默认值
    float  fGGainMin;       // 绿通道增益最小值
    float  fGGainMax;       // 绿通道增益最大值
    float  fGGainDefault;   // 绿通道增益默认值
    float  fBGainMin;       // 蓝通道增益最小值
    float  fBGainMax;       // 蓝通道增益最大值
    float  fBGainDefault;   // 蓝通道增益默认值
} tDSRgbGainRange;


/*------------------------------------------------------------------------*\
|                            色彩饱和度数值范围
\*------------------------------------------------------------------------*/
typedef struct
{
    INT  iMin;       // 色彩饱和度最小值
    INT  iMax;       // 色彩饱和度最大值
    INT  iDefault;   // 色彩饱和度默认值
} tDSSaturationRange;


/*------------------------------------------------------------------------*\
|                             图像锐度数值范围
\*------------------------------------------------------------------------*/
typedef struct
{
    INT  iMin;       // 图像锐度最小值
    INT  iMax;       // 图像锐度最大值
    INT  iDefault;   // 图像锐度默认值
} tDSSharpnessRange;


/*------------------------------------------------------------------------*\
|                            图像降噪目标值范围
\*------------------------------------------------------------------------*/
typedef struct
{
    INT  iMin;       // 图像降噪最小值
    INT  iMax;       // 图像降噪最大值
    INT  iDefault;   // 图像降噪默认值
} tDSNoiseReductionRange;


/*------------------------------------------------------------------------*\
|                      传输数据分包长度（千兆网相机）
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;                         // 数据分包长度索引号，值从0开始
    char  acDescription[DESCR_LEN_MAX];   // 数据分包长度的描述名称
} tDSPackLength;


/*------------------------------------------------------------------------*\
|                         时间戳信息（千兆网相机）
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;                         // 时间戳索引号，值从0开始
    char  acDescription[DESCR_LEN_MAX];   // 时间戳的描述名称
} tDSTimeStamp;


/*------------------------------------------------------------------------*\
|                             颜色矫正矩阵描述
\*------------------------------------------------------------------------*/
typedef struct  
{
    INT   iIndex;                         // 颜色矩阵索引号，值从0开始
    char  acDescription[DESCR_LEN_MAX];   // 颜色矩阵的描述名称
}tDSRgbMatrixType;


/*------------------------------------------------------------------------*\
|                             高级设置界面信息
\*------------------------------------------------------------------------*/
typedef struct
{
    BOOL  bPage;                          // 若存在对应的设置界面，值为TRUE。否则为FALSE
    char  acDescription[DESCR_LEN_MAX];   // 高级设置界面的描述名称
} tDSPropertyPageInfo;


/*------------------------------------------------------------------------*\
|                  定义模式描述，为各种可能的模式，多选一
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;
    char  acModeName[DESCR_LEN_MAX];
} tDSModeDesc;


/*------------------------------------------------------------------------*\
|                定义开关描述，为各种可能的选项，开启或关闭
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;
    char  acSwitchName[DESCR_LEN_MAX];
    BOOL  bDefault;
} tDSSwitchDesc;


/*------------------------------------------------------------------------*\
|                  定义编辑值描述，可修改数值的一些参数
\*------------------------------------------------------------------------*/
typedef struct
{
    INT   iIndex;
    char  acValueName[DESCR_LEN_MAX];
    INT   iDefault;
} tDSValueDesc;


/*------------------------------------------------------------------------*\
|                       描述相机的各种信息和参数属性。
\*------------------------------------------------------------------------*/
typedef struct
{
    tDSSensorInfo  sSensorInfo;   // 图像传感器信息

    /* 相机参数限定范围和默认值 */
    tDSImageSizeRange       sImageSizeRange;        // 图像尺寸的限定范围和默认值
    tDSRgbGainRange         sRgbGainRange;          // RGB增益的限定范围和默认值
    tDSSaturationRange      sSaturationRange;       // 色彩饱和度的限定范围和默认值
    tDSGammaRange           sGammaRange;            // 图像伽玛的限定范围和默认值
    tDSContrastRange        sContrastRange;         // 图像对比度的限定范围和默认值
    tDSSharpnessRange       sSharpnessRange;        // 图像锐度的限定范围和默认值
    tDSNoiseReductionRange  sNoiseReductionRange;   // 图像降噪声的限定范围和默认值

    /* 可选的选项和默认选项 */
    tDSExposure  *pExposureDesc;   // 曝光功能
    INT        iExposureDesc;      // 曝光功能的数目
    INT        iExposureDefault;   // 曝光功能默认项（索引号）

    tDSTrigger  *pTriggerDesc;     // 触发功能
    INT         iTriggerDesc;      // 触发功能的数目
    INT         iTriggerDefault;   // 触发功能默认项（索引号）

    tDSImageSize  *pImageSizeDesc;     // 图像尺寸
    INT           iImageSizeDesc;      // 图像尺寸的数目
    INT           iImageSizeDefault;   // 图像尺寸的默认项（索引号）

    tDSMediaType  *pMediaTypeDesc;     // 相机输出的图像数据类型
    INT           iMediaTypdeDesc;     // 图像数据类型的数目
    INT           iMediaTypeDefault;   // 图像数据类型的默认项（索引号）

    tDSResolutionMode  *pResolutionModeDesc;     // 分辨率模式
    INT                iResolutionModeDesc;      // 分辨率模式的数目
    INT                iResolutionModeDefault;   // 分辨率模式的默认项（索引号）

    tDSFrameSpeed  *pFrameSpeedDesc;     // 帧速
    INT            iFrameSpeedDesc;      // 帧速的数目
    INT            iFrameSpeedDefault;   // 帧速的默认项（索引号）

    tDSTimeStamp  *pTimeStampDesc;     // 时间戳（千兆网相机）
    INT           iTimeStampDesc;      // 时间戳的数目
    INT           iTimeStampDefault;   // 时间戳的默认项（索引号）

    tDSPackLength  *pPackLenDesc;     // 数据包长度
    INT            iPackLenDesc;      // 数据包长度的数目
    INT            iPackLenDefault;   // 数据包长度的默认项（索引号）

    tDSRgbMatrixType  *pRgbMatrixDesc;     // 颜色矩阵功能
    INT               iRgbMaxtrixDesc;     // 颜色矩阵功能的数目
    INT               iRgbMatrixDefault;   // 颜色矩阵功能的默认项（索引号）

    BOOL  bRomForSave;   // 设备是否支持参数存档

    /* 模式选项和默认模式 */
    tDSModeDesc  *pModeDesc;     // 模式
    INT          iModeDesc;      // 模式的数目
    INT          iModeDefault;   // 模式的默认项（索引号）

    /* 开关选项和及其默认值 */
    tDSSwitchDesc  *pSwitchDesc;   // 开关选项
    INT            iSwitchDesc;    // 开关选项的数目

    /* 数值描述 */
    tDSValueDesc  *pValueDesc;   // 数值描述
    INT           iValueDesc;    // 数值描述的数目
} tDSCameraCapability;

#endif

#include <iostream>  
#include <opencv2/opencv.hpp>  
  
using namespace std;  
using namespace cv;  
  
//对轮廓按面积降序排列  
bool biggerSort(vector<Point> v1, vector<Point> v2)  
{  
    return contourArea(v1)>contourArea(v2);  
}  
  
int main()  
{  
    //视频不存在，就返回  
    VideoCapture cap("3.AVI");  
    if(cap.isOpened()==false)  
        return 0;  
  
    //定义变量  
    int i;  
  
    Mat frame;          //当前帧  
    Mat foreground;     //前景  
    Mat bw;             //中间二值变量  
    Mat se;             //形态学结构元素  
  
    //用混合高斯模型训练背景图像  
    BackgroundSubtractorMOG mog;      
    for(i=0;i<10;++i)  
    {  
        cout<<"正在训练背景:"<<i<<endl;  
        cap>>frame;  
        if(frame.empty()==true)  
        {  
            cout<<"视频帧太少，无法训练背景"<<endl;  
            getchar();  
            return 0;  
        }  
        mog(frame,foreground,0.01);   
    }  
      
    //目标外接框、生成结构元素（用于连接断开的小目标）  
    Rect rt;  
    se=getStructuringElement(MORPH_RECT,Size(5,5));  
  
    //统计目标直方图时使用到的变量  
    vector<Mat> vecImg;  
    vector<int> vecChannel;  
    vector<int> vecHistSize;  
    vector<float> vecRange;  
    Mat mask(frame.rows,frame.cols,DataType<uchar>::type);  
    //变量初始化  
    vecChannel.push_back(0);  
    vecHistSize.push_back(32);  
    vecRange.push_back(0);  
    vecRange.push_back(180);  
          
    Mat hsv;        //HSV颜色空间，在色调H上跟踪目标（camshift是基于颜色直方图的算法）  
    MatND hist;     //直方图数组  
    double maxVal;      //直方图最大值，为了便于投影图显示，需要将直方图规一化到[0 255]区间上  
    Mat backP;      //反射投影图  
    Mat result;     //跟踪结果  
      
    //视频处理流程  
    while(1)  
    {  
        //读视频  
        cap>>frame;  
        if(frame.empty()==true)  
            break;        
      
        //生成结果图  
        frame.copyTo(result);  
  
        //检测目标(其实是边训练边检测)  
        mog(frame,foreground,0.01);  
        imshow("混合高斯检测前景",foreground);  
        moveWindow("混合高斯检测前景",400,0);  
        //对前景进行中值滤波、形态学膨胀操作，以去除伪目标和接连断开的小目标       
        medianBlur(foreground,foreground,5);  
        imshow("中值滤波",foreground);  
        moveWindow("中值滤波",800,0);  
        morphologyEx(foreground,foreground,MORPH_DILATE,se);  
  
        //检索前景中各个连通分量的轮廓  
        foreground.copyTo(bw);  
        vector<vector<Point>> contours;  
        findContours(bw,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE);  
        if(contours.size()<1)  
            continue;  
        //对连通分量进行排序  
        std::sort(contours.begin(),contours.end(),biggerSort);  
  
        //结合camshift更新跟踪位置（由于camshift算法在单一背景下，跟踪效果非常好；  
        //但是在监控视频中，由于分辨率太低、视频质量太差、目标太大、目标颜色不够显著  
        //等各种因素，导致跟踪效果非常差。  因此，需要边跟踪、边检测，如果跟踪不够好，  
        //就用检测位置修改  
        cvtColor(frame,hsv,COLOR_BGR2HSV);  
        vecImg.clear();  
        vecImg.push_back(hsv);  
        for(int k=0;k<contours.size();++k)  
        {  
            //第k个连通分量的外接矩形框  
            if(contourArea(contours[k])<contourArea(contours[0])/5)  
                break;  
            rt=boundingRect(contours[k]);                 
            mask=0;  
            mask(rt)=255;  
  
            //统计直方图  
            calcHist(vecImg,vecChannel,mask,hist,vecHistSize,vecRange);               
            minMaxLoc(hist,0,&maxVal);  
            hist=hist*255/maxVal;  
            //计算反向投影图  
            calcBackProject(vecImg,vecChannel,hist,backP,vecRange,1);  
            //camshift跟踪位置  
            Rect search=rt;  
            RotatedRect rrt=CamShift(backP,search,TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,10,1));  
            Rect rt2=rrt.boundingRect();  
            rt&=rt2;  
  
            //跟踪框画到视频上  
            rectangle(result,rt,Scalar(0,255,0),2);           
        }  
  
        //结果显示  
        imshow("原图",frame);  
        moveWindow("原图",0,0);  
  
        imshow("膨胀运算",foreground);  
        moveWindow("膨胀运算",0,350);  
  
        imshow("反向投影",backP);  
        moveWindow("反向投影",400,350);  
  
        imshow("跟踪效果",result);  
        moveWindow("跟踪效果",800,350);  
        waitKey(30);      
    }  
          
    getchar();  
    return 0;  
}  
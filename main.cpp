#include<iostream>
#include<opencv2/opencv.hpp>
#include<darknet.h>
#include<improcess.h>

using namespace std;
using namespace cv;

float colors[6][3] = { {1,0,1}, {0,0,1},{0,1,1},{0,1,0},{1,1,0},{1,0,0} };

float get_color(int c, int x, int max)
{
    float ratio = ((float)x/max)*5;
    int i = floor(ratio);
    int j = ceil(ratio);
    ratio -= i;
    float r = (1-ratio) * colors[i][c] + ratio*colors[j][c];
    return r;
}


int main()
{
    string cfgfile = "/home/gaokechen/darknet/cfg/yolov3.cfg";//读取模型文件，请自行修改相应路径
    string weightfile = "/home/gaokechen/darknet/yolov3.weights";
    float thresh=0.5;//参数设置
    float nms=0.35;
    int classes=80;

    network *net=load_network((char*)cfgfile.c_str(),(char*)weightfile.c_str(),0);//加载网络模型
    set_batch_network(net, 1);
    VideoCapture capture(0);//读取视频，请自行修改相应路径
    capture.set(CV_CAP_PROP_FRAME_WIDTH,1920);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
    Mat frame;
    Mat rgbImg;

    vector<string> classNamesVec;
    ifstream classNamesFile("/home/gaokechen/darknet/data/voc.names");//标签文件coco有80类

    if (classNamesFile.is_open()){
        string className = "";
        while (getline(classNamesFile, className))
            classNamesVec.push_back(className);
    }

    bool stop=false;
    while(!stop)
    {
        cout<<frame.size<<endl;
        if (!capture.read(frame))
        {
            printf("fail to read.\n");
            return 0;
        }
        cvtColor(frame, rgbImg, cv::COLOR_BGR2RGB);

        float* srcImg;
        size_t srcSize=rgbImg.rows*rgbImg.cols*3*sizeof(float);
        srcImg=(float*)malloc(srcSize);

        imgConvert(rgbImg,srcImg);//将图像转为yolo形式

        float* resizeImg;
        size_t resizeSize=net->w*net->h*3*sizeof(float);
        resizeImg=(float*)malloc(resizeSize);
        imgResize(srcImg,resizeImg,frame.cols,frame.rows,net->w,net->h);//缩放图像

        network_predict(net,resizeImg);//网络推理
        int nboxes=0;
        detection *dets=get_network_boxes(net,rgbImg.cols,rgbImg.rows,thresh,0.5,0,1,&nboxes);

        if(nms){
            do_nms_sort(dets,nboxes,classes,nms);
        }

        vector<cv::Rect>boxes;
        boxes.clear();
        vector<int>classNames;

        for (int i = 0; i < nboxes; i++){
            bool flag=0;
            int className;
            for(int j=0;j<classes;j++){
                if(dets[i].prob[j]>thresh){
                    if(!flag){
                        flag=1;
                        className=j;
                    }
                }
            }
            if(flag)
            {
                int left = (dets[i].bbox.x - dets[i].bbox.w / 2.)*frame.cols;
                int right = (dets[i].bbox.x + dets[i].bbox.w / 2.)*frame.cols;
                int top = (dets[i].bbox.y - dets[i].bbox.h / 2.)*frame.rows;
                int bot = (dets[i].bbox.y + dets[i].bbox.h / 2.)*frame.rows;

                if (left < 0)
                    left = 0;
                if (right > frame.cols - 1)
                    right = frame.cols - 1;
                if (top < 0)
                    top = 0;
                if (bot > frame.rows - 1)
                    bot = frame.rows - 1;

                Rect box(left, top, fabs(left - right), fabs(top - bot));
                boxes.push_back(box);
                classNames.push_back(className);
            }
        }
        free_detections(dets, nboxes);

        for(int i=0;i<boxes.size();i++)
        {
            int offset = classNames[i]*123457 % 80;
            float red = 255*get_color(2,offset,80);
            float green = 255*get_color(1,offset,80);
            float blue = 255*get_color(0,offset,80);

            rectangle(frame,boxes[i],Scalar(blue,green,red),2);

            String label = String(classNamesVec[classNames[i]]);
            int baseLine = 0;
            Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
            putText(frame, label, Point(boxes[i].x, boxes[i].y + labelSize.height),
                    FONT_HERSHEY_SIMPLEX, 1, Scalar(red, blue, green),2);
        }
//        Mat resize_img;
//        resize(frame,resize_img,cv::Size(f_width,f_height),(0,0),(0,0),cv::INTER_LINEAR);
//        cout<<frame.size<<endl;
        namedWindow("video",0);
        imshow("video",frame);


        int c=waitKey(30);
              if((char)c==27)
                  break;
              else if(c>=0)
                  waitKey(0);

        free(srcImg);
        free(resizeImg);
    }
    free_network(net);
    capture.release();
    return 1;
}

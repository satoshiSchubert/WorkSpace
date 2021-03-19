#include <iostream>
#include "opencv2/opencv.hpp"
#include <vector>
#include <queue>
#include <thread>
#include <exception>

using namespace std;
using namespace cv;

queue<Mat> RawBuf;//这个应该是全局的

//两个类，一个是VideoCapture2Buf类，调用了cv::VideoCapture将数据从后面读进
//大小为2的队列RawBuf中。
//另一个是FrameBuf类，调用该类将返回此时RawBuf中的首部一帧图像（RawBuf总共就2帧，因此延迟不大）
//videocapture应独立开一个线程

class VideoCapture2Buf
{
private:
	int bufsize = 2;//RawBuf的大小
	int camera_id = 0;//相机id
	Mat frame;
	Mat frame_copy;

public:
	VideoCapture2Buf():
		camera_id(0) {}
	VideoCapture2Buf(int camera_id_):
		camera_id(camera_id_){}
public:
	bool capture()
	{
		VideoCapture capture(camera_id);
		try {
			if (!capture.isOpened())
				throw "cam failed to open.Check your cam id.";
			while (1)
			{

				capture >> frame;
				frame.copyTo(frame_copy);
				RawBuf.push(frame_copy);
				cout << RawBuf.size() << endl;
				if (RawBuf.size() >= bufsize)
				{
					RawBuf.pop();
				}
			}
		}
		catch (char*&e) {
			std::cerr << "try catch test" << endl;
		}
		
		return true;
	}

};

class ReadFrameBuf
{
private:
	int FPS = 200;//初始设定FPS=200
	int state = 0;//0:off;1:read
public:
	ReadFrameBuf() :FPS(200) {}
	ReadFrameBuf(int FPS_) :FPS(FPS_) {}
public:
	void set(int state_){state = state_;}//int 1:start
	void readbuf()
	{
		int i;
		int rate = 0;
		while (state)
		{
			double t = (double)cv::getTickCount();//开始计时
			if (RawBuf.empty()) { break; }

			imshow("BufReadTest", RawBuf.front());
			rate = (1000 * (1.0 / FPS));
			cv::waitKey(rate);
			cout << "BufsizeRemain:" << ' ' << RawBuf.size() << ' ' << endl;

			//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();//结束计时
			//int fps = int(1.0 / t);//转换为帧率
			//cout << "FPS: " << fps << endl;//输出帧率
		}
	}
};


int main()
{
	VideoCapture2Buf usbcamera(0);
	ReadFrameBuf cambuf(150);
	cambuf.set(1);

	std::thread camreadthd(&VideoCapture2Buf::capture,&usbcamera);
	cv::waitKey(3000);//先保证Rawbuf中有东西
	cambuf.readbuf();

	camreadthd.join();
	return 0;
}




//
////////////////////////////////////////////////////////
//const string window_name = "用户界面";
///*
//研究一下hx的代码时如何可以取出最后一帧的
//*/
//queue<Mat> RawBuf;
//int bufsize = 2;
//int FPS = 200;//100 fps
//
//void consumer()
//{
//	int i;
//	int rate = 0;
//	for (i = 0; ; i++)
//	{
//		double t = (double)cv::getTickCount();//开始计时
//		if (RawBuf.empty()) { break; }
//
//		imshow("BufReadTest", RawBuf.front());
//		rate = (1000 * (1.0 / FPS));
//		cv::waitKey(rate);
//		cout << "BufsizeRemain:" << ' ' << RawBuf.size() << ' ' << endl;
//
//		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();//结束计时
//		int fps = int(1.0 / t);//转换为帧率
//		cout << "FPS: " << fps << endl;//输出帧率
//	}
//}
//
//void productor()
//{
//	Mat frame;
//	VideoCapture capture(0);
//	int i = 0;
//	char text[10];
//	Mat frame_copy;
//
//	while (capture.isOpened())
//	{
//		i += 1;
//		capture >> frame;
//		frame.copyTo(frame_copy);
//		/*_itoa_s(i, text, 10);
//		putText(frame_copy, text, Point(50, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);*/
//		RawBuf.push(frame_copy);
//		if (i >= bufsize){ RawBuf.pop();}
//
//		cout << RawBuf.size() << ' ' << endl;
//		if (i == bufsize + 1000) { break; }
//	}
//	cv::waitKey(0);
//	capture.release();
//}
//
//
//int main()
//{
//	thread generator(productor);
//	waitKey(3000);
//	thread reader(consumer);
//
//	generator.join();
//	reader.join();
//
//	return 0;
//}
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\objdetect\objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include <iostream>


using namespace cv;
using namespace std;

std::string ExePath()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL,buffer,MAX_PATH);
	string::size_type pos=string(buffer).find_last_of("\\/");
	return string(buffer).substr(0,pos);
}

int main(int argc, const char** argv)
{

 // add your file name
 VideoCapture cap("C:\\Users\\huzhu\\workspace\\MotionInterpolation\\EggJumpy.mp4");


 Mat flow;
 // some faster than mat image container
 UMat  flowUmat, prevgray;
 Rect Rec(640,210,650,650);//ADJUSTED FOR 6946 ONLY

 namedWindow("resultDisplay",1);
 VideoWriter outputVideo;
 Size S= Size(650,650);
 outputVideo.open("outputVideo.avi",CV_FOURCC('M','J','P','G'),50,S,true);
 if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: "  << endl;
        return -1;
    }

 std::string DataFolder = ExePath()+ "\\VideoScreenShot\\";
 CreateDirectory(DataFolder.c_str(),NULL);
 for (int i=0;i<400;i++)
 {

  bool Is = cap.grab();
  if (Is == false) {
                         // if video capture failed
   cout << "Video Capture Fail" << endl;
   break;
  }
  else {

   Mat img;
   Mat original;
   Mat estimation;
   Mat recover1;

   // capture frame from video file
   cap.retrieve(img, CV_CAP_OPENNI_BGR_IMAGE);
  // resize(img, img, Size(640, 480));
   //crop video to only active region-For OV6946 ONLY
   img=img(Rec);


   // save original for later
   img.copyTo(original);

   // just make current frame gray
   cvtColor(img, img, COLOR_BGR2GRAY);


    //optical flow alg.

   if (prevgray.empty() == false ) {

    // calculate optical flow
	double pyr_scale =0.4;
	int levels =1;
	int winsize=9;//12
	int iterations =2;
	int poly_n=5;//8
	double poly_sigma=1.1;//1.2;
	int flags =0;
    calcOpticalFlowFarneback(prevgray, img, flowUmat,pyr_scale,levels,winsize,iterations,poly_n,poly_sigma,flags);
    //initialize estimation as frame#2


    namedWindow("first",WINDOW_AUTOSIZE);
    imshow("first",prevgray);
    namedWindow("second",WINDOW_AUTOSIZE);
    imshow("second",img);

    // copy Umat container to standard Mat
    flowUmat.copyTo(flow);

    estimation=Mat::zeros(img.size(),img.type());
    recover1=Mat::zeros(img.size(),img.type());
   // int imgType=img.type();
   // cout<< "imgType="<<imgType<<endl;


           // By y += 5, x += 5 you can specify the grid
    for (int y = 0; y < original.rows; y += 1) {
     for (int x = 0; x < original.cols; x += 1)
     {
              // get the flow from y, x position * 10 for better visibility
              const Point2f flowatxy = flow.at<Point2f>(y, x);
                     // draw line at flow direction
      // line(original, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(255,0,0));
                                                         // draw initial point
      // circle(original, Point(x, y), 1, Scalar(0, 255, 0), -1);
      // circle(original, Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), 1, Scalar(0, 0, 255), -1);
      //estimation.at<uchar>(Point(y,x))=img.at<uchar>(Point(y,x));
      estimation.at<uchar>(Point(x,y))=img.at<uchar>(Point(cvRound(x + (flowatxy.x)/2),cvRound(y + (flowatxy.y)/2)));
      recover1.at<uchar>(Point(x,y))=img.at<uchar>(Point(cvRound(x + (flowatxy.x)),cvRound(y + (flowatxy.y))));
      /*
      if (x!=cvRound(x+(flowatxy.x)/2)){
     cout<<"x="<<x<<",y="<<y<<endl;
     cout<<"x + (flowatxy.x)/2="<<cvRound(x + (flowatxy.x)/2)<<"y+(flowatxy.y)/2="<<cvRound(y+(flowatxy.y)/2)<<endl;
      }
      */

      }

     }
                                // draw the results
    //namedWindow("prew", WINDOW_AUTOSIZE);
    //imshow("prew", original);
    /*
    namedWindow("estimation",WINDOW_AUTOSIZE);
    imshow("estimation",estimation);
    namedWindow("recover1",WINDOW_AUTOSIZE);
    imshow("recover1",recover1);
    */

    Mat c_estimation, c_img;
    cvtColor(estimation,c_estimation,CV_GRAY2RGB);
    cvtColor(img,c_img,CV_GRAY2RGB);
    std::string imageName1;
    ostringstream convert1;
    convert1<<i;
    imageName1=DataFolder+convert1.str()+ ".bmp";

    //out put image
    imwrite(imageName1,c_estimation);

    i++;
    std::string imageName2;
    ostringstream convert2;
    convert2<<i;
    imageName2=DataFolder+convert2.str()+ ".bmp";

    imwrite(imageName2,c_img);

 //   imshow("resultDisplay",c_estimation);
 //   if( (char)waitKey(15) ==27) break;
 //   imshow("resultDisplay",c_img);
 //   if( (char)waitKey(15) ==27) break;



                                  // fill previous image again
    img.copyTo(prevgray);

   }
   else {

                                 // fill previous image in case prevgray.empty() == true
    img.copyTo(prevgray);

   }

  }
 }
 cout<<"Finish writing"<<endl;
 waitKey();
}

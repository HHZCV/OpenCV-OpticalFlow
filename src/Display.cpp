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



int main(int argc, const char** argv)
{

 // add your file name
 VideoCapture cap("C:\\Users\\huzhu\\workspace\\MotionInterpolation\\EggJumpy.mp4");

 Mat flow;
 // some faster than mat image container
 UMat  flowUmat, prevgray;
 Rect Rec(640,210,650,650);//ADJUSTED FOR 6946 ONLY

 namedWindow("resultDisplay",1);

 for (;;)
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


   // For all optical flow you need a sequence of images.. Or at least 2 of them. Previous                           //and current frame
   //if there is no current frame
   // go to this part and fill previous frame
   //else {
   // img.copyTo(prevgray);
   //   }
   // if previous frame is not empty.. There is a picture of previous frame. Do some                                  //optical flow alg.

   if (prevgray.empty() == false ) {

    // calculate optical flow
    calcOpticalFlowFarneback(prevgray, img, flowUmat, 0.4, 1, 12, 2, 8, 1.2, 0);
    //initialize estimation as frame#2

/*
    namedWindow("first",WINDOW_AUTOSIZE);
    imshow("first",prevgray);
    namedWindow("second",WINDOW_AUTOSIZE);
    imshow("second",img);
*/
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
    /*                             // draw the results
    namedWindow("prew", WINDOW_AUTOSIZE);
    imshow("prew", original);
    namedWindow("estimation",WINDOW_AUTOSIZE);
    imshow("estimation",estimation);
    namedWindow("recover1",WINDOW_AUTOSIZE);
    imshow("recover1",recover1);
    */

    Mat c_estimation, c_img;
    cvtColor(estimation,c_estimation,CV_GRAY2RGB);
    cvtColor(img,c_img,CV_GRAY2RGB);

    imshow("resultDisplay",c_estimation);
    if( (char)waitKey(15) ==27) break;
    imshow("resultDisplay",c_img);
    if( (char)waitKey(15) ==27) break;

                                  // fill previous image again
    img.copyTo(prevgray);

   }
   else {

                                 // fill previous image in case prevgray.empty() == true
    img.copyTo(prevgray);

   }

  }
 }
 waitKey();
}

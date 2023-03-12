/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <ctime>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

#include <opencv2/core/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <pthread.h>

using namespace cv;
using namespace std;
using namespace face;

struct framebuffer_info
{
    uint32_t bits_per_pixel; // depth of framebuffer
    uint32_t xres_virtual;   // how many pixel in a row in virtual screen
};

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path);

static void read_csv(const string &filename, vector<Mat> &images, vector<int> &labels, char separator = ';')
{
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file)
    {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line))
    {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if (!path.empty() && !classlabel.empty())
        {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

string g_listname_t[] =
    {
        "hy",
        "jason",
};
/*------------------------------------------------------------------------*/
typedef struct PTHREAD_PARA
{
    //void *original_frame;
    //void *para_haar_cascade;
    //void *para_face;
    //void *para_detect_item;
    //void *para_face_i;
    Mat *original_frame;
    CascadeClassifier *para_haar_cascade;
    Mat *para_face;
    vector<Rect_<int> > *para_detect_item;
    Rect *para_face_i;
} pthread_para, *pthread_para_ptr;
void *eyes_detect(void *arg)
{
    pthread_para_ptr para;
    // pthread_para *para;
    para = (pthread_para_ptr)arg; // 取得資料
    
    (*(para->para_haar_cascade)).detectMultiScale((*(para->para_face)), (*(para->para_detect_item)));
    // haar_cascade_eyes.detectMultiScale(face, eyes);
    for (int i = 0; i < (*(para->para_detect_item)).size(); i++)
    {
        Rect eyes_i = (*(para->para_detect_item))[i];
        // Rect eyes_i = (para->para_detect_item)[i];
        //  Rect eyes_i = eyes[i];
        eyes_i.x += (*(para->para_face_i)).x;
        eyes_i.y += (*(para->para_face_i)).y;
        rectangle((*(para->original_frame)), eyes_i, CV_RGB(255, 0, 0), 1);
    }
    pthread_exit(NULL); // 傳回結果
    
    //(*((CascadeClassifier *)(para->para_haar_cascade))).detectMultiScale((*((Mat *)(para->para_face))), (*((vector<Rect_<int> > *)(para->para_detect_item))));
    // haar_cascade_eyes.detectMultiScale(face, eyes);
    //for (int i = 0; i < (*((vector<Rect_<int> > *)(para->para_detect_item))).size(); i++)
    //{
    //    Rect eyes_i = (*((vector<Rect_<int> > *)(para->para_detect_item)))[i];
        // Rect eyes_i = (para->para_detect_item)[i];
        //  Rect eyes_i = eyes[i];
    //   eyes_i.x += (*((Rect *)(para->para_face_i))).x;
    //    eyes_i.y += (*((Rect *)(para->para_face_i))).y;
    //    rectangle((*((Mat *)(para->original_frame))), eyes_i, CV_RGB(255, 0, 0), 1);
    //}
    //pthread_exit(NULL); // 傳回結果
    
}
void *nose_detect(void *arg)
{
    pthread_para_ptr para;
    // pthread_para *para;
    para = (pthread_para_ptr)arg; // 取得資料
    (*(para->para_haar_cascade)).detectMultiScale((*(para->para_face)), (*(para->para_detect_item)));
    // haar_cascade_eyes.detectMultiScale(face, eyes);
    for (int i = 0; i < (*(para->para_detect_item)).size(); i++)
    {
        Rect eyes_i = (*(para->para_detect_item))[i];
        // Rect eyes_i = (para->para_detect_item)[i];
        //  Rect eyes_i = eyes[i];
        eyes_i.x += (*(para->para_face_i)).x;
        eyes_i.y += (*(para->para_face_i)).y;
        rectangle((*(para->original_frame)), eyes_i, CV_RGB(255, 0, 0), 1);
    }
    pthread_exit(NULL); // 傳回結果
}
void *mouth_detect(void *arg)
{
    pthread_para_ptr para;
    // pthread_para *para;
    para = (pthread_para_ptr)arg; // 取得資料
    (*(para->para_haar_cascade)).detectMultiScale((*(para->para_face)), (*(para->para_detect_item)));
    // haar_cascade_eyes.detectMultiScale(face, eyes);
    for (int i = 0; i < (*(para->para_detect_item)).size(); i++)
    {
        Rect eyes_i = (*(para->para_detect_item))[i];
        // Rect eyes_i = (para->para_detect_item)[i];
        //  Rect eyes_i = eyes[i];
        eyes_i.x += (*(para->para_face_i)).x;
        eyes_i.y += (*(para->para_face_i)).y;
        rectangle((*(para->original_frame)), eyes_i, CV_RGB(255, 0, 0), 1);
    }
    pthread_exit(NULL); // 傳回結果
}

int main(int argc, const char *argv[])
{
    // please set the correct path based on your folder
    string fn_haar = "haarcascade_frontalface_default.xml";
    string fn_eyes = "haarcascade_eye.xml";
    string fn_mouth = "haarcascade_mcs_mouth.xml";
    string fn_nose = "haarcascade_mcs_nose.xml";

    string fn_csv = "csv.ext";
    int deviceId = 2; // here is my webcam Id.
    // These vectors hold the images and corresponding labels:
    vector<Mat> images;
    vector<int> labels;
    // Read in the data (fails if no valid input filename is given, but you'll get an error message):
    try
    {
        read_csv(fn_csv, images, labels);
    }
    catch (cv::Exception &e)
    {
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }
    // Get the height from the first image. We'll need this
    // later in code to reshape the images to their original
    // size AND we need to reshape incoming faces to this size:
    int im_width = images[0].cols;
    int im_height = images[0].rows;
    // Create a FaceRecognizer and train it on the given images:
    // Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
    Ptr<LBPHFaceRecognizer> model = LBPHFaceRecognizer::create(1, 8, 8, 8, 150.0);

    model->train(images, labels);
    // That's it for learning the Face Recognition model. You now
    // need to create the classifier for the task of Face Detection.
    // We are going to use the haar cascade you have specified in the
    // command line arguments:

    CascadeClassifier haar_cascade;
    haar_cascade.load(fn_haar);

    CascadeClassifier haar_cascade_eyes;
    haar_cascade_eyes.load(fn_eyes);

    CascadeClassifier haar_cascade_noses;
    haar_cascade_noses.load(fn_nose);

    CascadeClassifier haar_cascade_mouths;
    haar_cascade_mouths.load(fn_mouth);

    // Get a handle to the Video device:
    VideoCapture cap(deviceId);
    // Check if we can use this device at all:
    if (!cap.isOpened())
    {
        cerr << "Capture Device ID " << deviceId << "cannot be opened." << endl;
        return -1;
    }

    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");
    std::ofstream ofs("/dev/fb0");

    const int frame_width = 640;
    const int frame_height = 480; // Can not >= 664
    const int frame_rate = 30;

    cap.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
    cap.set(CV_CAP_PROP_FPS, frame_rate);

    // Holds the current frame from the Video device:
    Mat frame;
    for (;;)
    {
        cap >> frame;
        // Clone the current frame:
        Mat original = frame.clone();
        // Convert the current frame to grayscale:
        Mat gray;
        cvtColor(original, gray, CV_BGR2GRAY);
        // Find the faces in the frame:
        vector<Rect_<int> > faces;
        vector<Rect_<int> > eyes;
        vector<Rect_<int> > nose;
        vector<Rect_<int> > mouth;
        haar_cascade.detectMultiScale(gray, faces);
        // At this point you have the position of the faces in
        // faces. Now we'll get the faces, make a prediction and
        // annotate it in the video. Cool or what?
        std::clock_t start;
        start = std::clock();
        for (int i = 0; i < faces.size(); i++)
        {
            // Process face by face:
            Rect face_i = faces[i];
            // Crop the face from the image. So simple with OpenCV C++:
            Mat face = gray(face_i);
            // Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
            // verify this, by reading through the face recognition tutorial coming with OpenCV.
            // Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
            // input data really depends on the algorithm used.
            //
            // I strongly encourage you to play around with the algorithms. See which work best
            // in your scenario, LBPH should always be a contender for robust face recognition.
            //
            // Since I am showing the Fisherfaces algorithm here, I also show how to resize the
            // face you have just found:
            Mat face_resized;
            cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
            // Now perform the prediction, see how easy that is:
            // int prediction = model->predict(face_resized);

            // To get the confidence of a prediction call the model with:
            int prediction = -1;
            double confidence = 0.0;
            model->predict(face_resized, prediction, confidence);

            string result_message = format("Predicted class = %d / Actual class = %d / Confidence = %lf", prediction, 1, confidence);
            cout << result_message << endl;

            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!before pthread assign" << std::endl;
            // pthread_para *para1;
            // pthread_para *para2;
            // pthread_para *para3;
            pthread_para_ptr para1 = (pthread_para_ptr)malloc(sizeof(PTHREAD_PARA));
            pthread_para_ptr para2 = (pthread_para_ptr)malloc(sizeof(PTHREAD_PARA));
            pthread_para_ptr para3 = (pthread_para_ptr)malloc(sizeof(PTHREAD_PARA));
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!0000000000" << std::endl;
            para1->original_frame = &original;
            //(para1->original_frame) = original;
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!1111111111" << std::endl;
            para1->para_haar_cascade = &haar_cascade_eyes;
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!2222222222" << std::endl;
            para1->para_face = &face;
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!3333333333" << std::endl;
            para1->para_detect_item = &eyes;
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!4444444444" << std::endl;
            para1->para_face_i = &face_i;
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!5555555555" << std::endl;

            para2->original_frame = &original;
            para2->para_haar_cascade = &haar_cascade_noses;
            para2->para_face = &face;
            para2->para_detect_item = &nose;
            para2->para_face_i = &face_i;
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!after pthread assign22222" << std::endl;

            para3->original_frame = &original;
            para3->para_haar_cascade = &haar_cascade_mouths;
            para3->para_face = &face;
            para3->para_detect_item = &mouth;
            para3->para_face_i = &face_i;
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!after pthread assign333333" << std::endl;

            pthread_t eyes_t, nose_t, mouth_t;
            pthread_create(&eyes_t, NULL, eyes_detect, para1);
            pthread_create(&nose_t, NULL, nose_detect, para2);
            pthread_create(&mouth_t, NULL, mouth_detect, para3);
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!BBBBBBBBB" << std::endl;

            // And finally write all we've found out to the original image!
            // First of all draw a green rectangle around the detected face:
            rectangle(original, face_i, CV_RGB(255, 255, 255), 1);
            // Create the text we will annotate the box with:
            string box_text;
            box_text = format("Prediction = ");
            // Get stringname
            if (prediction >= 0 && prediction <= 1)
            {
                box_text.append(g_listname_t[prediction]);
            }
            else
                box_text.append("Unknown");
            // Calculate the position for annotated text (make sure we don't
            // put illegal values in there):
            int pos_x = std::max(face_i.tl().x - 10, 0);
            int pos_y = std::max(face_i.tl().y - 10, 0);
            // And now put it into the image:
            putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);

            // // eyes
            // haar_cascade_eyes.detectMultiScale(face, eyes);
            // for (int i = 0; i < eyes.size(); i++)
            // {
            //     Rect eyes_i = eyes[i];
            //     eyes_i.x += face_i.x;
            //     eyes_i.y += face_i.y;
            //     rectangle(original, eyes_i, CV_RGB(255, 0, 0), 1);
            // }
            // for (int i = 0; i < nose.size(); i++)
            // {
            //     Rect nose_i = nose[i];
            //     nose_i.x += face_i.x;
            //     nose_i.y += face_i.y;
            //     rectangle(original, nose_i, CV_RGB(0, 255, 0), 1);
            // }
            // for (int i = 0; i < mouth.size(); i++)
            // {
            //     Rect mouth_i = mouth[i];
            //     mouth_i.x += face_i.x;
            //     mouth_i.y += face_i.y;
            //     rectangle(original, mouth_i, CV_RGB(0, 0, 255), 1);
            // }
            pthread_join(eyes_t, NULL);
            pthread_join(nose_t, NULL);
            pthread_join(mouth_t, NULL);
            //std::cout << "hey there is okey!!!!!!!!!!!!!!!!!!!!!CCCCCCCCCCC" << std::endl;
        }
        std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
        // Show the result:
        int framebuffer_width = fb_info.xres_virtual;
        int framebuffer_depth = fb_info.bits_per_pixel;
        cv::Size2f frame_size = frame.size();
        cv::Mat framebuffer_compat;

        cv::cvtColor(original, framebuffer_compat, cv::COLOR_BGR2BGR565);

        for (int y = 0; y < frame_size.height; y++)
        {
            ofs.seekp(y * framebuffer_width * 2);
            ofs.write(reinterpret_cast<char *>(framebuffer_compat.ptr(y)), frame_size.width * 2);
        }

        // imshow("face_recognizer", original);
        // // And display it:
        // char key = (char)waitKey(20);
        // // Exit this loop on escape:
        // if (key == 27)
        //     break;
    }
    return 0;
}

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path)
{
    struct framebuffer_info fb_info;      // Used to return the required attrs.
    struct fb_var_screeninfo screen_info; // Used to get attributes of the device from OS kernel.

    // open deive with linux system call "open( )"
    // https://man7.org/linux/man-pages/man2/open.2.html

    // get attributes of the framebuffer device thorugh linux system call "ioctl()"
    // the command you would need is "FBIOGET_VSCREENINFO"
    // https://man7.org/linux/man-pages/man2/ioctl.2.html
    // https://www.kernel.org/doc/Documentation/fb/api.txt

    // put the required attributes in variable "fb_info" you found with "ioctl() and return it."
    // fb_info.xres_virtual = ......
    // fb_info.bits_per_pixel = ......

    int fd = -1;
    fd = open(framebuffer_device_path, O_RDWR);
    if (fd >= 0)
    {
        if (!ioctl(fd, FBIOGET_VSCREENINFO, &screen_info))
        {
            fb_info.xres_virtual = screen_info.xres_virtual;
            fb_info.bits_per_pixel = screen_info.bits_per_pixel;
        }
    }

    return fb_info;
};
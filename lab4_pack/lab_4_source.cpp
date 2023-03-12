#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/ioctl.h>
#include <string>
using namespace std;

struct framebuffer_info
{
    uint32_t bits_per_pixel;    // depth of framebuffer
    uint32_t xres_virtual;      // how many pixel in a row in virtual screen
};

struct framebuffer_info get_framebuffer_info ( const char *framebuffer_device_path );

int main ( int argc, const char *argv[] )
{
    // variable to store the frame get from video stream
    cv::Mat frame;

    // open video stream device
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a5d5f5dacb77bbebdcbfb341e3d4355c1
    cv::VideoCapture camera ( 2 );

    // get info of the framebuffer
    // fb_info = ......
    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");

    // open the framebuffer device
    // http://www.cplusplus.com/reference/fstream/ofstream/ofstream/
    // ofs = ......
    std::ofstream ofs("/dev/fb0");

    // check if video stream device is opened success or not
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a9d2ca36789e7fcfe7a7be3b328038585
    // if( !check )
    // {
    //     std::cerr << "Could not open video device." << std::endl;
    //     return 1;
    // }
    if( !camera.isOpened() )
    {
        std::cerr << "Could not open video device." << std::endl;
        return 1;
    }

    // set propety of the frame
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a8c6d8c2d37505b5ca61ffd4bb54e9a7c
    // https://docs.opencv.org/3.4.7/d4/d15/group__videoio__flags__base.html#gaeb8dd9c89c10a5c63c139bf7c4f5704d



    //int counter = 0;
    //std::string pic_name = "frame";
    while ( true )
    {
	
        // get video frame from stream
        // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a473055e77dd7faa4d26d686226b292c1
        // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a199844fb74226a28b3ce3a39d1ff6765
        // frame = ......
	camera >> frame;

        // get size of the video frame
        // https://docs.opencv.org/3.4.7/d3/d63/classcv_1_1Mat.html#a146f8e8dda07d1365a575ab83d9828d1
        // frame_size = ......
	cv::Size frame_size(static_cast<int>(camera.get(3)), static_cast<int>(camera.get(4)));

        // transfer color space from BGR to BGR565 (16-bit image) to fit the requirement of the LCD
        // https://docs.opencv.org/3.4.7/d8/d01/group__imgproc__color__conversions.html#ga397ae87e1288a81d2363b61574eb8cab
        // https://docs.opencv.org/3.4.7/d8/d01/group__imgproc__color__conversions.html#ga4e0972be5de079fed4e3a10e24ef5ef0
	cv::cvtColor(frame, frame, cv::COLOR_BGR2BGR565);

        // output the video frame to framebufer row by row
        for ( int y = 0; y < frame_size.height; y++ )
        {
            // move to the next written position of output device framebuffer by "std::ostream::seekp()"
            // http://www.cplusplus.com/reference/ostream/ostream/seekp/
	    ofs.seekp(fb_info.bits_per_pixel * fb_info.xres_virtual * y / 8, std::ios::beg);

            // write to the framebuffer by "std::ostream::write()"
            // you could use "cv::Mat::ptr()" to get the pointer of the corresponding row.
            // you also need to cacluate how many bytes required to write to the buffer
            // http://www.cplusplus.com/reference/ostream/ostream/write/
            // https://docs.opencv.org/3.4.7/d3/d63/classcv_1_1Mat.html#a13acd320291229615ef15f96ff1ff738
	    ofs.write((char*)(frame.cv::Mat::ptr(y)), fb_info.xres_virtual * fb_info.bits_per_pixel / 8);
        }


	//int key = cv::waitKey(0);
	//if (key == 32)
	   //cv::destoryAllWindows();
	//if (key == 67) {
	//   cv::imwrite(pic_name + char(counter) + ".jpg", frame);
	//}
	
	//if(counter < 60)
	//   counter++;
	//else
	 //  counter = 0;
    }

    // closing video stream
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#afb4ab689e553ba2c8f0fec41b9344ae6
    camera.release ( );

    return 0;
}

struct framebuffer_info get_framebuffer_info ( const char *framebuffer_device_path )
{
    struct framebuffer_info fb_info;        // Used to return the required attrs.
    struct fb_var_screeninfo screen_info;   // Used to get attributes of the device from OS kernel.

    // open deive with linux system call "open( )"
    // https://man7.org/linux/man-pages/man2/open.2.html
    int fbfd;
    fbfd = open(framebuffer_device_path, O_RDWR);

    // get attributes of the framebuffer device thorugh linux system call "ioctl()"
    // the command you would need is "FBIOGET_VSCREENINFO"
    // https://man7.org/linux/man-pages/man2/ioctl.2.html
    // https://www.kernel.org/doc/Documentation/fb/api.txt
    ioctl(fbfd, FBIOGET_VSCREENINFO, &screen_info);

    // put the required attributes in variable "fb_info" you found with "ioctl() and return it."
    // fb_info.xres_virtual = ......
    // fb_info.bits_per_pixel = ......
    fb_info.xres_virtual = screen_info.xres;
    fb_info.bits_per_pixel = screen_info.bits_per_pixel;

    return fb_info;
};

#include "CImg.h"
#include <iostream>
#include "hough.h"
using namespace std;
using namespace cimg_library;

int main(void)
{
    string infilename = "test1.bmp";

    //Hough hough("test2.bmp",180,240,4);
    //Hough hough("test3.bmp",130,190,7);
    //Hough hough("test4.bmp",160,210,3);
/*
    Hough hough("test1.bmp",140,160,1); //弱，需要调低grad到20
    hough.edge_detect();
    hough.find_circle();
    hough.clear();
    Hough hough2("test2.bmp",180,240,4); //弱，需要调低grad到20
    hough2.edge_detect();
    hough2.find_circle();
    hough2.clear();
    Hough hough3("test3.bmp",130,190,7); //弱，需要调低grad到20
    hough3.edge_detect();
    hough3.find_circle();
    hough3.clear();*/
    Hough hough4("test4.bmp",160,210,3); //弱，需要调低grad到20
    hough4.edge_detect();
    hough4.find_circle();
    hough4.clear();/*
    Hough hough5("test5.bmp",440,600,2); //弱，需要调低grad到20
    hough5.edge_detect();
    hough5.find_circle();
    hough5.clear();
    Hough hough6("test6.bmp",30,100,5); //弱，需要调低grad到20
    hough6.edge_detect();
    hough6.find_circle();
    hough6.clear();*/
    return 0;
}

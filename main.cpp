#include <iostream>
#include <string>
#include <exception>
#include "code0plus.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include "hough.h"
using namespace std;

#define VERBOSE 0
#define BOOSTBLURFACTOR 90.0

int main(int argc,char** argv)
{
    cout<<"test test.bmp"<<endl;
    Hough hough("test.bmp");
    hough.edge_detect();
    hough.find_point();
    hough.clear();
    cout<<"test test2.bmp"<<endl;
    Hough hough2("test2.bmp");
    hough2.edge_detect();
    hough2.find_point();
    hough2.clear();
    cout<<"test test3.bmp"<<endl;
    Hough hough3("test3.bmp");
    hough3.edge_detect();
    hough3.find_point();
    hough3.clear();
    cout<<"test test4.bmp"<<endl;
    Hough hough4("test4.bmp");
    hough4.edge_detect();
    hough4.find_point();
    hough4.clear();
    cout<<"test test5.bmp"<<endl;
    Hough hough5("test5.bmp");
    hough5.edge_detect();
    hough5.find_point();
    hough5.clear();
    cout<<"test test6.bmp"<<endl;
    Hough hough6("test6.bmp");
    hough6.edge_detect();
    hough6.find_point();
    hough6.clear();
    return 0;
}

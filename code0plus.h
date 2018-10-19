#ifndef CODE0
#define CODE0

#include "CImg.h"
#include <string>
#include <vector>
using namespace cimg_library;
using namespace std;

class Canny{
private:
    CImg<unsigned char> image;
    CImg<unsigned char> edge;
    int rows;
    int cols;
    void make_gaussian_kernel(float sigma,vector<float>& kernel,int& windowsize);
    double angle_radians(double x, double y);
public:
    Canny();
    Canny(string infilename);
    void canny_main(float sigma,float tlow,float thigh,string fname);//主过程
    void gaussian_smooth(float sigma,vector<short int>& smoothedim);//高斯过滤
    void derrivative_x_y(vector<short int>& smoothedim,vector<short int>& delta_x,vector<short int>& delta_y);//求一阶导数
    void radian_direction(vector<short int>& delta_x,vector<short int>& delta_y,vector<float>& dir_radians,int xdirtag,int ydirtag);
    void magnitude_x_y(vector<short int>& delta_x,vector<short int>& delta_y,vector<short int>& magnitude);
    void non_max_supp(vector<short int>& mag,vector<short int>& gradx,vector<short int>& grady,vector<unsigned char>& result);
    void apply_hysteresis(vector<short int>& mag,vector<unsigned char>& nms,float tlow,float thigh);
    void merge_and_reduce(void);
    void write_file(string filename);
    int getrows()
    {
        return rows;
    }

    int getcols()
    {
        return cols;
    }
};

#endif // CODE0

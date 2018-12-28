#include "CImg.h"
#include "Canny.h"
#include "Process.h"
#include <string>
#include <iostream>
#define DownSampledSquareSize 500.0
using namespace std;
using namespace cimg_library;

int main(void)
{
	CImg<int> SrcImg, GrayImg, DownSampledImg, CannyImg, HoughSpaceImg,
		Edge_Point_Img, SrcImgWithVertexAndEdge, PaperModifiedImg;

	SrcImg.load_bmp("test.bmp");
	GrayImg = grayTheImg(SrcImg);
	GrayImg.display("GrayImg");
	DownSampledImg = downSampleTheImg(GrayImg, DownSampledSquareSize);
	//DownSampledImg.display("DownSampledImg");
	CannyImg = canny(DownSampledImg, DownSampledImg._width, DownSampledImg._height);
	CannyImg.display("EdgeImg");
    return 0;
}

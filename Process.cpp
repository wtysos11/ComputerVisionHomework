#include "Process.h"
#include <iostream>
#include <math.h>
using namespace std;

CImg<int> makeGrayImage(const CImg<int>& src) {
	CImg<int> gray = CImg<int>(src.width(), src.height(), 1, 1, 0);
	cimg_forXY(gray, x, y)
    {
		gray(x, y, 0) = (int)round((double)src(x, y, 0, 0) * 0.299 + (double)src(x, y, 0, 1) * 0.587 + (double)src(x, y, 0, 2) * 0.114);
	}
	return gray;
}

CImg<int> downSample(const CImg<int>& SrcGrayImg, float sampleSquareSize) {
	double sampleSize = (double)(SrcGrayImg._width + SrcGrayImg._height) / sampleSquareSize / 2.0;
	int anWidth = (int)round(SrcGrayImg._width / sampleSize);
	int anHeight = (int)round(SrcGrayImg._height / sampleSize);
	CImg<int> answer = CImg<int>(anWidth, anHeight, 1, 1, 0);

	cimg_forXY(answer, x, y) {
		double scrX = ((double)x) * ((double)SrcGrayImg._width / (double)anWidth);
		double scrY = ((double)y) * ((double)SrcGrayImg._height / (double)anHeight);
		double scrX_head = floor(scrX);
		double scrY_head = floor(scrY);
		double scrX_tail = 0, scrY_tail = 0;

		int intensity00 = SrcGrayImg((int)scrX_head, (int)scrY_head, 0, 0);
		int intensity01, intensity10, intensity11;
		intensity01 = intensity10 = intensity11 = 0;

		if ((int)scrY_head < SrcGrayImg._height - 1) {
			scrY_tail = scrY - scrY_head;
			intensity01 = SrcGrayImg((int)scrX_head, (int)scrY_head + 1, 0, 0);
		}
		if ((int)scrX_head < SrcGrayImg._width - 1) {
			scrX_tail = scrX - scrX_head;
			intensity10 = SrcGrayImg((int)scrX_head + 1, (int)scrY_head, 0, 0);
		}
		if (((int)scrY_head < SrcGrayImg._height - 1) && ((int)scrX_head < SrcGrayImg._width - 1)) {
			intensity11 = SrcGrayImg((int)scrX_head + 1, (int)scrY_head + 1);
		}

		double scrX_tail_anti = (double)1 - scrX_tail;
		double scrY_tail_anti = (double)1 - scrY_tail;

		double temp_intensity = (double)intensity00 * scrX_tail_anti * scrY_tail_anti
								+ (double)intensity01 * scrX_tail_anti * scrY_tail
								+ (double)intensity10 * scrX_tail * scrY_tail_anti
								+ (double)intensity11 * scrX_tail * scrY_tail;
		int new_intensity = (int)floor(temp_intensity);
		answer(x, y, 0) = new_intensity;
	}

	return answer;
}

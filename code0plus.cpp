#include "code0plus.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <queue>
#include <map>

#define VERBOSE 0
#define BOOSTBLURFACTOR 90.0

#define NOEDGE 255
#define POSSIBLE_EDGE 128
#define EDGE 0

using namespace std;
Canny::Canny(string infilename)
{
    image = CImg<unsigned char>(infilename.c_str());
    edge = nullptr;
    cols = image.width();
    rows = image.height();
}

void Canny::canny_main(float sigma,float tlow,float thigh,string fname)
{
    vector<short int> smoothedim,magnitude;
    vector<short int> delta_x,delta_y;
    vector<float> dir_radians;
    vector<unsigned char> nms;
    int r,c,pos;

   /****************************************************************************
   * Perform gaussian smoothing on the image using the input standard
   * deviation.
   * ��˹�˲�������������˹������������ͼ���м�Ȩƽ��
   ****************************************************************************/
   gaussian_smooth(sigma,smoothedim);

   /****************************************************************************
   * Compute the first derivative in the x and y directions.
   * ��x��y������һ�׵���
   ****************************************************************************/
   derrivative_x_y(smoothedim,delta_x,delta_y);

   /****************************************************************************
   * This option to write out the direction of the edge gradient was added
   * to make the information available for computing an edge quality figure
   * of merit.
   ****************************************************************************/

   if(fname != ""){
      /*************************************************************************
      * Compute the direction up the gradient, in radians that are
      * specified counteclockwise from the positive x-axis.
      *************************************************************************/
      radian_direction(delta_x, delta_y,dir_radians, -1, -1);

      /*************************************************************************
      * Write the gradient direction image out to a file.
      *************************************************************************/

      ofstream file(fname,ios::binary);
      for(unsigned int i = 0 ;i<rows*cols;i++)
      {
          file<<dir_radians[i];
      }
      file<<endl;
      file.close();
   }

   /****************************************************************************
   * Compute the magnitude of the gradient.
   * �����ݶȵĳ���
   ****************************************************************************/
   magnitude_x_y(delta_x, delta_y,magnitude);

   /****************************************************************************
   * Perform non-maximal suppression.
   * ִ�з��������suanfa
   ****************************************************************************/
   nms = vector<unsigned char>(rows*cols);
   non_max_supp(magnitude, delta_x, delta_y, nms);

   /****************************************************************************
   * Use hysteresis to mark the edge pixels.
   * ʹ���ͺ�����ȥ��Ǳ��ϵ�����
   ****************************************************************************/
   edge = CImg<unsigned char>(cols,rows);

   apply_hysteresis(magnitude, nms, tlow, thigh);
}

void Canny::gaussian_smooth(float sigma,vector<short int>& smoothedim)
{
    int r,c,rr,cc,windowsize,center;
    vector<float> tempim;
    vector<float> kernel;
    float dot,sum;


   /****************************************************************************
   * Create a 1-dimensional gaussian smoothing kernel.
   ****************************************************************************/
    make_gaussian_kernel(sigma,kernel,windowsize);//��˹�˺�����һά��
    center = windowsize/2;

   /****************************************************************************
   * Allocate a temporary buffer image and the smoothed image.
   ****************************************************************************/
   tempim = vector<float>(this->rows*this->cols);
   smoothedim = vector<short int>(this->rows*this->cols);

   /****************************************************************************
   * Blur in the x - direction.
   ****************************************************************************/
    for(r=0;r<rows;r++){
      for(c=0;c<cols;c++){
         dot = 0.0;
         sum = 0.0;
         for(cc=(-center);cc<=center;cc++){
            if(((c+cc) >= 0) && ((c+cc) < cols)){
               dot += (float)image.at(r*cols+(c+cc),0)* kernel[center+cc];
               sum += kernel[center+cc];
            }
         }
         tempim[r*cols+c] = dot/sum;
      }
   }
   /****************************************************************************
   * Blur in the y - direction.
   ****************************************************************************/
   for(c=0;c<cols;c++){
      for(r=0;r<rows;r++){
         sum = 0.0;
         dot = 0.0;
         for(rr=(-center);rr<=center;rr++){
            if(((r+rr) >= 0) && ((r+rr) < rows)){
               dot += tempim[(r+rr)*cols+c] * kernel[center+rr];//(r+rr,c)���д���
               sum += kernel[center+rr];
            }
         }
         smoothedim[r*cols+c] = (short int)(dot*BOOSTBLURFACTOR/sum + 0.5);//(r,c)
      }
   }
}

void Canny::make_gaussian_kernel(float sigma,vector<float>& kernel,int& windowsize)
{
   int center;
   float x, fx, sum=0.0;

   windowsize = 1 + 2 * ceil(2.5 * sigma);
   center = windowsize / 2;

   kernel = vector<float>(windowsize,0);

   for(int i=0;i<windowsize;i++){
      x = (float)(i - center);
      fx = pow(2.71828, -0.5*x*x/(sigma*sigma)) / (sigma * sqrt(6.2831853));
      kernel[i] = fx;
      sum += fx;
   }

    for(unsigned int i=0;i<windowsize;i++)
        kernel[i] /= sum;

}

void Canny::derrivative_x_y(vector<short int>& smoothedim,vector<short int>& delta_x,vector<short int>& delta_y)
{
   int r, c, pos;

   /****************************************************************************
   * Allocate images to store the derivatives.
   ****************************************************************************/
   delta_x = vector<short int>(rows*cols,0);
   delta_y = vector<short int>(rows*cols,0);

   /****************************************************************************
   * Compute the x-derivative. Adjust the derivative at the borders to avoid
   * losing pixels.
   ****************************************************************************/
   for(r=0;r<rows;r++){
      pos = r * cols;//��r��
      delta_x[pos] = smoothedim[pos+1] - smoothedim[pos];
      pos++;
      for(c=1;c<(cols-1);c++,pos++){
         delta_x[pos] = smoothedim[pos+1] - smoothedim[pos-1];
      }
      delta_x[pos] = smoothedim[pos] - smoothedim[pos-1];
   }

   /****************************************************************************
   * Compute the y-derivative. Adjust the derivative at the borders to avoid
   * losing pixels.
   ****************************************************************************/
   for(c=0;c<cols;c++){
      pos = c;
      delta_y[pos] = smoothedim[pos+cols] - smoothedim[pos];
      pos += cols;
      for(r=1;r<(rows-1);r++,pos+=cols){
         delta_y[pos] = smoothedim[pos+cols] - smoothedim[pos-cols];
      }
      delta_y[pos] = smoothedim[pos] - smoothedim[pos-cols];
   }
}

void Canny::radian_direction(vector<short int>& delta_x,vector<short int>& delta_y,vector<float>& dir_radians,int xdirtag,int ydirtag)
{
   int r, c, pos;
   double dx, dy;

   /****************************************************************************
   * Allocate an image to store the direction of the gradient.
   ****************************************************************************/
   dir_radians = vector<float>(rows*cols,0);

   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++){
         dx = (double)delta_x[pos];
         dy = (double)delta_y[pos];

         if(xdirtag == 1) dx = -dx;
         if(ydirtag == -1) dy = -dy;

         dir_radians[pos] = (float)angle_radians(dx, dy);
      }
   }
}

void Canny::magnitude_x_y(vector<short int>& delta_x,vector<short int>& delta_y,vector<short int>& magnitude)
{
   int r, c, pos, sq1, sq2;

   /****************************************************************************
   * Allocate an image to store the magnitude of the gradient.
   ****************************************************************************/
   magnitude = vector<short int>(rows*cols);

   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++){
         sq1 = (int)delta_x[pos] * (int)delta_x[pos];
         sq2 = (int)delta_y[pos] * (int)delta_y[pos];
         magnitude[pos] = (short)(0.5 + sqrt((float)sq1 + (float)sq2));
      }
   }

}

void Canny::non_max_supp(vector<short int>& mag,vector<short int>& gradx,vector<short int>& grady,vector<unsigned char>& result)
{
    int ncols = cols,nrows = rows;
    int rowcount, colcount,counting;
    int magrowptr,magptr;
    int gxrowptr,gxptr;
    int gyrowptr,gyptr,z1,z2;
    short m00,gx,gy;
    float mag1,mag2,xperp,yperp;
    int resultrowptr, resultptr;
   /****************************************************************************
   * Zero the edges of the result image.
   * ��ͼ�ı�Եȫ����Ϊ0
   ****************************************************************************/
    for(counting=0,resultrowptr=0,resultptr=ncols*(nrows-1);
        counting<ncols; resultptr++,resultrowptr++,counting++){
        result[resultrowptr] = 0;
        result[resultptr] = 0;
    }

    for(counting=0,resultptr=0,resultrowptr=ncols-1;
        counting<nrows; counting++,resultptr+=ncols,resultrowptr+=ncols){
        result[resultrowptr] = 0;
        result[resultptr] = 0;
    }

   /****************************************************************************
   * Suppress non-maximum points.
   ****************************************************************************/


   for(rowcount=1,magrowptr=ncols+1,gxrowptr=ncols+1,gyrowptr=ncols+1,resultrowptr=ncols+1;
        rowcount<nrows-2;
        rowcount++,magrowptr+=ncols,gyrowptr+=ncols,gxrowptr+=ncols,resultrowptr+=ncols){
      for(colcount=1,magptr=magrowptr,gxptr=gxrowptr,gyptr=gyrowptr,resultptr=resultrowptr;
            colcount<ncols-2;
            colcount++,magptr++,gxptr++,gyptr++,resultptr++){

        m00 = mag[magptr];
         if(m00 == 0){
            result[resultptr] = (unsigned char) NOEDGE;
         }
         else{
            xperp = -(gx = gradx[gxptr])/((float)m00);
            yperp = (gy = grady[gyptr])/((float)m00);
         }
        int dx = 1;
        int dy = ncols;

         if(gx >= 0){
            if(gy >= 0){

                    if (gx >= gy)
                    {
                        /* 111 */
                        /* Left point */
                        z1 = mag[magptr - dx];
                        z2 = mag[magptr - dy - dx];

                        mag1 = (m00 - z1)*xperp + (z2 - z1)*yperp;

                        /* Right point */
                        z1 = mag[magptr + dx];
                        z2 = mag[magptr + dy + dx];

                        mag2 = (m00 - z1)*xperp + (z2 - z1)*yperp;
                    }
                    else
                    {
                        /* 110 */
                        /* Left point */
                        z1 = mag[magptr - dy];
                        z2 = mag[magptr - dy - dx];

                        mag1 = (z1 - z2)*xperp + (z1 - m00)*yperp;

                        /* Right point */
                        z1 = mag[magptr + dy];
                        z2 = mag[magptr + dy + dx];

                        mag2 = (z1 - z2)*xperp + (z1 - m00)*yperp;
                    }
                }
                else
                {
                    if (gx >= -gy)
                    {
                        /* 101 */
                        /* Left point */
                        z1 = mag[magptr - dx];
                        z2 = mag[magptr + dy - dx];

                        mag1 = (m00 - z1)*xperp + (z1 - z2)*yperp;

                        /* Right point */
                        z1 = mag[magptr + dx];
                        z2 = mag[magptr - dy + dx];

                        mag2 = (m00 - z1)*xperp + (z1 - z2)*yperp;
                    }
                    else
                    {
                        /* 100 */
                        /* Left point */
                        z1 = mag[magptr + dy];
                        z2 = mag[magptr + dy - dx];

                        mag1 = (z1 - z2)*xperp + (m00 - z1)*yperp;

                        /* Right point */
                        z1 = mag[magptr - dy];
                        z2 = mag[magptr - dy + dx];

                        mag2 = (z1 - z2)*xperp  + (m00 - z1)*yperp;
                    }
                }
            }
            else
            {
                gy = grady[gyptr];
                if ( gy >= 0)
                {
                    if (-gx >= gy)
                    {
                        /* 011 */
                        /* Left point */
                        z1 = mag[magptr + dx];
                        z2 = mag[magptr - dy + dx];

                        mag1 = (z1 - m00)*xperp + (z2 - z1)*yperp;

                        /* Right point */
                        z1 = mag[magptr - dx];
                        z2 = mag[magptr + dy - dx];

                        mag2 = (z1 - m00)*xperp + (z2 - z1)*yperp;
                    }
                    else
                    {
                        /* 010 */
                        /* Left point */
                        z1 = mag[magptr - dy];
                        z2 = mag[magptr - dy + dx];

                        mag1 = (z2 - z1)*xperp + (z1 - m00)*yperp;

                        /* Right point */
                        z1 = mag[magptr + dy];
                        z2 = mag[magptr + dy - dx];

                        mag2 = (z2 - z1)*xperp + (z1 - m00)*yperp;
                    }
                }
                else
                {
                    if (-gx > -gy)
                    {
                        /* 001 */
                        /* Left point */
                        z1 = mag[magptr + dx];
                        z2 = mag[magptr + dy + dx];

                        mag1 = (z1 - m00)*xperp + (z1 - z2)*yperp;

                        /* Right point */
                        z1 = mag[magptr - dx];
                        z2 = mag[magptr - dy - dx];

                        mag2 = (z1 - m00)*xperp + (z1 - z2)*yperp;
                    }
                    else
                    {
                        /* 000 */
                        /* Left point */
                        z1 = mag[magptr + dy];
                        z2 = mag[magptr + dy + dx];

                        mag1 = (z2 - z1)*xperp + (m00 - z1)*yperp;

                        /* Right point */
                        z1 = mag[magptr - dy];
                        z2 = mag[magptr - dy - dx];

                        mag2 = (z2 - z1)*xperp + (m00 - z1)*yperp;
                    }
                }
            }

            /* Now determine if the current point is a maximum point */

            if ((mag1 > 0.0) || (mag2 > 0.0))
            {
                result[resultptr] = (unsigned char) NOEDGE;
            }
            else
            {
                if (mag2 == 0.0)
                    result[resultptr] = (unsigned char) NOEDGE;
                else
                    result[resultptr] = (unsigned char) POSSIBLE_EDGE;
            }
        }
    }
}

void Canny::apply_hysteresis(vector<short int>& mag,vector<unsigned char>& nms,float tlow,float thigh)
{
   int r, c, pos, numedges, lowcount, highcount, lowthreshold, highthreshold,
       i, hist[32768], rr, cc;
   short int maximum_mag, sumpix;

   int pos_edge1 = 0;
   int pos_edge2 = 0;

   int pos_edge3 = 0;
   int edge_num = 0;
   /****************************************************************************
   * Initialize the edge map to possible edges everywhere the non-maximal
   * suppression suggested there could be an edge except for the border. At
   * the border we say there can not be an edge because it makes the
   * follow_edges algorithm more efficient to not worry about tracking an
   * edge off the side of the image.
   ****************************************************************************/
  //���ݷ�������ƵĽ����edge���и�ֵ�����һЩ���ܴ��ڵıߵĵ�
   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++){
        if(nms[pos] == POSSIBLE_EDGE) edge.at(pos) = POSSIBLE_EDGE;
        else edge.at(pos) = NOEDGE;
      }
   }

//��ͼ��߽���д���
   for(r=0,pos=0;r<rows;r++,pos+=cols){
      edge.at(pos) = NOEDGE;
      edge.at(pos+cols-1) = NOEDGE;
   }
   for(c=0,pos = (rows-1) * cols;c<cols;c++,pos++){
      edge.at(c) = NOEDGE;
      edge.at(pos) = NOEDGE;
   }

   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++){
        if(edge.at(pos) == POSSIBLE_EDGE)
            pos_edge1++;
      }
   }
   /****************************************************************************
   * Compute the histogram of the magnitude image. Then use the histogram to
   * compute hysteresis thresholds.
   ****************************************************************************/
   for(r=0;r<32768;r++) hist[r] = 0;
   for(r=0,pos=0;r<rows;r++){
      for(int c=0;c<cols;c++,pos++){
        if(edge.at(pos) == POSSIBLE_EDGE) hist[mag[pos]]++;
      }
   }

   /****************************************************************************
   * Compute the number of pixels that passed the nonmaximal suppression.
   ****************************************************************************/
   for(r=1,numedges=0;r<32768;r++){
      if(hist[r] != 0) maximum_mag = r;
      numedges += hist[r];
   }

   highcount = (int)(numedges * thigh + 0.5);

   /****************************************************************************
   * Compute the high threshold value as the (100 * thigh) percentage point
   * in the magnitude of the gradient histogram of all the pixels that passes
   * non-maximal suppression. Then calculate the low threshold as a fraction
   * of the computed high threshold value. John Canny said in his paper
   * "A Computational Approach to Edge Detection" that "The ratio of the
   * high to low threshold in the implementation is in the range two or three
   * to one." That means that in terms of this implementation, we should
   * choose tlow ~= 0.5 or 0.33333.
   ****************************************************************************/
   r = 1;
   numedges = hist[1];
   while((r<(maximum_mag-1)) && (numedges < highcount)){
      r++;
      numedges += hist[r];
   }
   highthreshold = r;
   lowthreshold = (int)(highthreshold * tlow + 0.5);
   /****************************************************************************
   * This loop looks for pixels above the highthreshold to locate edges and
   * then calls follow_edges to continue the edge.
   ****************************************************************************/
   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++){
         if((edge.at(pos) == POSSIBLE_EDGE) && (mag[pos] >= highthreshold)){ // ԭ���Ǵ���highthreshold
                edge.at(pos) = EDGE;
                //follow_edges(pos, pos, lowthreshold, mag);
                queue<int> q;
                q.push(pos);
                while(!q.empty())
                {
                    int node = q.front();
                    q.pop();

                    int originx = node%cols;
                    int originy = node/cols;
                    int x[8] = {1,1,0,-1,-1,-1,0,1},
                       y[8] = {0,1,1,1,0,-1,-1,-1};
                    for(int i = 0;i<8;i++)
                    {
                        int xi = originx + x[i];
                        int yi = originy + y[i];
                        int position = yi*cols + xi;
                        if((edge.at(position) == POSSIBLE_EDGE) && (mag[position] > lowthreshold)){
                            edge.at(position) = (unsigned char) EDGE;
                            q.push(position);
                        }
                    }
                }
         }
      }
   }

  //����Ӧ�ò���һ������������8�������ڳ��Ȳ���20�ı���������
    //merge_and_reduce();
   /****************************************************************************
   * Set all the remaining possible edges to non-edges.
   ****************************************************************************/

   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++)
        if(edge.at(pos) != EDGE)
            edge.at(pos) = NOEDGE;
   }
}

void Canny::write_file(string filename)
{
    edge.save(filename.c_str());
}

/*******************************************************************************
* FUNCTION: angle_radians
* PURPOSE: This procedure computes the angle of a vector with components x and
* y. It returns this angle in radians with the answer being in the range
* 0 <= angle <2*PI.
*******************************************************************************/
double Canny::angle_radians(double x, double y)
{
  double xu, yu, ang;

  xu = fabs(x);
  yu = fabs(y);

  if((xu == 0) && (yu == 0)) return(0);

  ang = atan(yu/xu);

  if(x >= 0){
     if(y >= 0) return(ang);
     else return(2*M_PI - ang);
  }
  else{
     if(y >= 0) return(M_PI - ang);
     else return(M_PI + ang);
    }
}

void Canny::merge_and_reduce(void)
{
    map<int,int> edgeNum;//�ߵĵ�
    int counting = 0;
    vector<int> edgeId(rows*cols,0);//���������һ����

    int store = 0;//�ڲ���������Ҫ�޸�counting�Ĳ��֣�������Ҫ��ʱ����ֵ
    //��ÿ������б��
    for(int i = 0;i<rows;i++)
    {
        for(int j = 0;j<cols;j++)
        {
            int pos = i*cols+j;
            if(edge.at(pos)==EDGE && edgeId[pos] == 0)
            {
                counting = store;
                counting++;
                store = counting;
                edgeId[pos] = counting;
                edgeNum[counting] = 1;

                queue<int> q;
                q.push(pos);
                while(!q.empty())
                {
                    int node = q.front();
                    q.pop();

                    int originx = node%cols;
                    int originy = node/cols;
                    int x[8] = {1,1,0,-1,-1,-1,0,1},
                       y[8] = {0,1,1,1,0,-1,-1,-1};
                    for(int i = 0;i<8;i++)
                    {
                        int xi = originx + x[i];
                        int yi = originy + y[i];
                        int position = yi*cols + xi;
                        if(edge.at(position) == EDGE && edgeId[position]!=counting){//��������ͬ���ڷ��Լ��ߵĵ��Ⱦ���Լ��ߵġ�
                            edgeId[position] = counting;
                            edgeNum[counting]++;
                            q.push(position);
                        }
                        else if(edge.at(position) != EDGE && edgeId[position]==0)//�Աߵ���Χ������Ⱦ���޸���edgeNum���ǲ��޸�edgeId�������������ͬ�ı߷��ʵ���ͬһ���㣬��������������
                        {
                            edgeId[position] = counting;
                        }
                        else if(edge.at(position) != EDGE && edgeId[position]!=0 && edgeId[position] < counting)//���������֮ǰ�ı��������ĵ�
                        {
                            store = counting - 1;//����
                            counting = edgeId[position];
                            edge.at(position) = EDGE;
                            q.push(position);
                        }
                    }
                }
            }

        }
    }

    for(int i = 0;i<rows;i++)
    {
        for(int j = 0;j<cols;j++)
        {
            int pos = i*cols+j;
            if(edge.at(pos)==EDGE && edgeNum[edgeId[pos]]<20)
            {
                edge.at(pos)=NOEDGE;
            }
        }
    }
}

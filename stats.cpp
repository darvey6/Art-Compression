
#include "stats.h"

stats::stats(PNG & im){
  // initialize all private vectors so that, for each color channel,
  // (x,y) is the cumulative sum of the the color values from (0,0)
  // to (x,y), and the hist structure is as defined above.
    // Note that the hue (h) value of each pixel is represented by
    // its cartesian coordinates: X = s*cos(h) and Y = s*sin(h).
    // This is done to simplify distance computation. We calculate
    // the cumulative sums for X and Y separately, and then combine
    // them when we are doing color difference computation.

/* your code here */

  hist = vector<vector<vector<int>>> (0, vector<vector<int>>(0, vector<int>(36, 0)));
  sumHueX  = vector<vector<double>> (0, vector<double>(0));
  sumHueY = vector<vector<double>> (0, vector<double>(0));
  sumSat =  vector<vector<double>> (0, vector<double>(0));
  sumLum  = vector<vector<double>> (0, vector<double>(0));

  int totalX = 0;
  int totalY = 0;
  int totalSat = 0;
  int totalLum = 0;

  for(int j = 0; j<im.height(); j++){
    totalX = 0;
    totalY = 0;
    totalSat = 0;
    totalLum = 0;
    for(int i = 0; i<im.width(); i++){
      HSLAPixel *pix1 = im.getPixel(i,j);

      sumHueX[j][i] = pix1->s * cos(pix1->h);
      sumHueY[j][i] = pix1->s * sin(pix1->h);
      sumSat[j][i] = pix1->s;
      sumLum[j][i] = pix1->l;

      totalX = totalX + sumHueX[j][i];
      totalY = totalY + sumHueY[j][i];
      totalSat = totalSat  + sumSat[j][i];
      totalLum = totalLum + sumLum[j][i];

      if (j > 0){
        sumHueX[j][i] = totalX + sumHueX[j-1][i];
        sumHueY[j][i] = totalY + sumHueY[j-1][i];
        sumSat[j][i] = totalSat + sumSat[j-1][i];
        sumLum[j][i] = totalLum + sumLum[j-1][i];
      }
      for(int k = 0; k < 36; k++){
        if (i == 0 && j == 0){
          break;
        }
        if (i == 0){
          hist[i][j][k] = hist[i-1][j][k];
        }
        if (j == 0){
          hist[i][j][k] = hist[i][j-1][k];
        }
        hist[i][j][k] = hist[i][j-1][k] + hist[i-1][j][k] - hist[i-1][j-1][k];
      }
        hist[i][j][pix1->h/10]++;
    }
  }
}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
  // given a rectangle, return the number of pixels in the rectangle
  /* @param ul is (x,y) of the upper left corner of the rectangle
  * @param lr is (x,y) of the lower right corner of the rectangle */
/* your code here */
 int xval = lr.first - ul.first + 1;
 int yval = lr.second - lr.second + 1;
 int area = xval * yval;

 return area;
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
  /* Each color component of the pixel is the average value of that
	* component over the rectangle.
	* @param ul is (x,y) of the upper left corner of the rectangle
	* @param lr is (x,y) of the lower right corner of the rectangle */
    // The average hue value can be computed from the average X and
    // Y values using the arctan function. You should research the
    // details of this. Finally, please set the average alpha channel to
    // 1.0.
/* your code here */
  int avgH  = 0;
  double avgS = 0;
  double avgL = 0;
  long tPix= rectArea(ul,lr);

  if (ul.first == 0 && ul.second == 0){
    avgH = sumHueX[lr.first][lr.second]/tPix;
  } else if (ul.first == 0){
    avgH = (sumHueX[lr.first][lr.second] - sumHueX[lr.first][ul.second-1])/tPix;
    avgL = (sumLum[lr.first][lr.second] - sumLum[lr.first][ul.second-1])/tPix;
    avgS = (SumSat[lr.first][lr.second] - sumLum[lr.first][ul.second-1])/tPix;
  } else if (ul.second == 0){
    avgH = (sumHueX[lr.first][lr.second] - sumHueX[ul.first-1][lr.second])/tPix;
    avgL = (sumLum[lr.first][lr.second] - sumLum[ul.first-1][lr.second])/tPix;
    avgS = (SumSat[lr.fist][lr.second] - sumLum[ul.first-1][lr.second])/tPix;
  }
  avgH = (sumHueX[lr.first][lr.second] - sumHueX[lr.first][ul.second-1] - sumHueX[ul.first-1][lr.second] + sumHueX[ul.first-1][ul.second-1])/ tPix;
  avgL = (sumLum[lr.first][lr.second] - sumLum[lr.first][ul.second-1] - sumLum[ul.first-1][lr.second] + sumLum[ul.first-1][ul.second-1])/tPix;
  avgS = (SumSat[lr.fist][lr.second] - sumLum[lr.first][ul.second-1] - sumLum[ul.first-1][lr.second] + sumLum[ul.first-1][ul.second-1])/tPix;

  return new HSLAPixel (avgH ,avgS, double avgL, 1.0);
}
vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){

/* your code here */
}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area){

    double entropy = 0.;

/* your code here */

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 )
            entropy += ((double) distn[i]/(double) area)
                                    * log2((double) distn[i]/(double) area);
    }

    return  -1 * entropy;

}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){

/* your code here */

}

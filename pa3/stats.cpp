
#include "stats.h"

stats::stats(PNG & im){
    //init vectors to spec (leave edges empty to simplify computation)
    sumHueX.resize(im.width()+1, vector<double> (im.height()+1,0));
    sumHueY.resize(im.width()+1, vector<double> (im.height()+1,0));
    sumSat.resize(im.width()+1, vector<double> (im.height()+1, 0));
    sumLum.resize(im.width()+1, vector<double> (im.height()+1, 0));
    hist.resize(im.width()+1, vector<vector<int>> (im.height()+1, vector<int> (36, 0)));

    //fill in values
    for (unsigned x=0; x<im.width();x++){
        for (unsigned y=0; y<im.height();y++){
            HSLAPixel* curPix = im.getPixel(x,y);
            sumSat[x+1][y+1] = curPix->s + sumSat[x+1][y] + sumSat[x][y+1] - sumSat[x][y];
            sumLum[x+1][y+1] = curPix->l + sumLum[x+1][y] + sumLum[x][y+1] - sumLum[x][y];
            sumHueX[x+1][y+1] = cos((curPix->h) * PI / 180.0) + sumHueX[x+1][y] + sumHueX[x][y+1] - sumHueX[x][y];
            sumHueY[x+1][y+1] = sin((curPix->h) * PI / 180.0) + sumHueY[x+1][y] + sumHueY[x][y+1] - sumHueY[x][y];
            if (curPix->h == 360) {
                hist[x+1][y+1][0] = hist[x+1][y][0] + hist[x][y+1][0] - hist[x][y][0] + 1;
            } else {
                for (unsigned int k = 0; k < 36; k++) {
                    int curVal = hist[x+1][y][k] + hist[x][y+1][k] - hist[x][y][k];
                    if (10 * k <= curPix->h && curPix->h < 10 * (k + 1)) {
                        hist[x+1][y+1][k] = curVal + 1;
                    } else {
                        hist[x+1][y+1][k] = curVal;
                    }
                }
            }
        }
    }
}


long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
    long w = lr.second-ul.second;
    long h = lr.first-ul.first;
    return (w+1)*(h+1);
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){

    double totalHueX, totalHueY, totalSat, totalLum,
            avgHueX, avgHueY, avgSat, avgLum, avgHue;


    totalSat = sumSat[lr.first+1][lr.second+1]
                    - sumSat[lr.first+1][ul.second]
                    - sumSat[ul.first][lr.second+1]
                    + sumSat[ul.first][ul.second];

    totalLum = sumLum[lr.first+1][lr.second+1]
                  - sumLum[lr.first+1][ul.second]
                  - sumLum[ul.first][lr.second+1]
                  + sumLum[ul.first][ul.second];

    totalHueX = sumHueX[lr.first+1][lr.second+1]
                   - sumHueX[lr.first+1][ul.second]
                   - sumHueX[ul.first][lr.second+1]
                   + sumHueX[ul.first][ul.second];

    totalHueY = sumHueY[lr.first+1][lr.second+1]
                   - sumHueY[lr.first+1][ul.second]
                   - sumHueY[ul.first][lr.second+1]
                   + sumHueY[ul.first][ul.second];


    avgHueX = totalHueX / rectArea(ul,lr);
    avgHueY = totalHueY / rectArea(ul,lr);
    avgSat = totalSat / rectArea(ul,lr);
    avgLum = totalLum / rectArea(ul,lr);
    avgHue = atan2(avgHueY,avgHueX) * 180 / PI;
    if (avgHue < 0) {
        avgHue += 360;
    }

    return HSLAPixel(avgHue, avgSat, avgLum, 1);
}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){

/* code */

vector<int> temp;
temp.resize(36);

for(int i = 0; i < 36; i++)
{
   temp[i]= hist[lr.first+1][lr.second+1][i] - hist[lr.first+1][ul.second][i] - hist[ul.first][lr.second+1][i] + hist[ul.first][ul.second][i];
   }
   
   return temp;
}


// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area)
{
    double entropy = 0.0;

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    }

    return  -1 * entropy;

}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){

/* your code here */

vector<int> distn = buildHist(ul,lr);
long area = rectArea(ul, lr);

 return entropy(distn, area);;
}

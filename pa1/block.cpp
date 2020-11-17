#include "block.h"
#include <iostream>
using namespace std;

//From im, grabs the vertical strip of pixels whose upper left corner is at position (column,0), and whose width is width.
void Block::build(PNG & im, int column, int width) {
   for (int i = column; i < column + width; i++) {
    vector<HSLAPixel> temp;
    for (unsigned int j = 0; j < im.height(); j++) {
      HSLAPixel* p = im.getPixel(i, j);
      temp.push_back(*p);
    }
    data.push_back(temp);
  }
}

//Draws the current block at position (column,0) in im.
void Block::render(PNG & im, int column) const {
  int x = column;
  int y = 0;
  for (int i = 0; i < data.size() && x + i < im.width(); i++) {
    for (int j = 0; j < height() && y + j < im.height(); j++) {
      HSLAPixel* imPixel = im.getPixel(x+i, y+j);
      HSLAPixel output = data[i][j];
      *imPixel = output;
      
    }
  }
}

//This function changes the saturation of every pixel in the block to 0, which removes the color, leaving grey.
void Block::greyscale() {
  for (int i = 0; i < width(); i++) {
    for (int j = 0; j <height(); j++) {
      data[i][j].s = 0;
    }
  }
}

// return the width of the block
int Block::width() const {
  return data.size();
}

// return the height of the block
int Block::height() const {
  return data[0].size();
}
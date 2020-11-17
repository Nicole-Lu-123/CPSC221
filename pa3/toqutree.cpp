
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 

/* This constructor grabs the 2^k x 2^k sub-image centered */
/* in imIn and uses it to build a quadtree. It may assume  */
/* that imIn is large enough to contain an image of that size. */

/* your code here */
  int ul_x = (int) (imIn.width() - pow(2,k))/2;
	int ul_y = (int) (imIn.height() - pow(2,k))/2;
	auto length = (unsigned int) pow(2,k);
	PNG im = PNG(length,length);
	for (unsigned int x = 0; x < length; x++) {
		for (unsigned int y = 0; y < length; y++) {
			*(im.getPixel(x,y)) = *imIn.getPixel(ul_x+x, ul_y+y);
		}
	}
	root = buildTree(&im,k);
}

/**
    * Constructor that builds a toqutree out of the given PNG.
    * Every leaf in the tree corresponds to a pixel in the PNG.
    * Every non-leaf node corresponds to a square of pixels 
    * which may not be contiguous in the original PNG, due to .
    * the splittingg procedure we've chosen. In addition, the Node
    * stores a pixel representing the average color over the 
    * portion of the image it represents.
    *
    * Every node's children correspond to a partition
    * of the node's square into four smaller squares where a square
    * is defined under the assumption that the image's horizontal
    * and vertical borders "wrap" so that pixels at the top are 
    * considered to be adjacent to pixels at the bottom, and similarly
    * for the right and left edges of the image. The node's
    * square is partitioned by choosing a splitting point that
    * results in the four smaller squares whose Information Gain
    * is as large as possible. See the website for description.
    *
    * The children of a node are named NW, NE, SE, and SW corresponding
    * to the compass directions, and assuming N is up.
    * (see illustrations within the spec.)
    *
   * This function will call helper function buildTree.
    */
toqutree::Node * toqutree::buildTree(PNG * im, int k) {

/* your code here */
if (k >= 0){
		int ul_x = (int) (im->width() - pow(2,k))/2;
		int ul_y = (int) (im->height() - pow(2,k))/2;
		int lr_x = ul_x + (int) pow(2,k)-1;
		int lr_y = ul_y + (int) pow(2,k)-1;
		pair<int,int> ul = make_pair(ul_x,ul_y);
		pair<int,int> lr = make_pair (lr_x,lr_y);
		stats* s =  new stats(*im);
		HSLAPixel avg = s->getAvg(ul,lr);
		pair<int ,int > center = findmidpoint(im, k);
		Node* root = new Node(center,k,avg);
    int length = (int) pow(2,k);
		int quadrant_length = (int) pow(2,k)/2;
		PNG* NW_im = TreeHelper(im, (center.first+quadrant_length)%length, (center.second+quadrant_length)%length);
		PNG* NE_im = TreeHelper(im, center.first, (center.second+quadrant_length)%length);
		PNG* SE_im = TreeHelper(im, center.first, center.second);
		PNG* SW_im = TreeHelper(im, (center.first+quadrant_length)%length, center.second);
		root->NW = buildTree(NW_im, k-1);
		root->NE = buildTree(NE_im, k-1);
		root->SE = buildTree(SE_im, k-1);
		root->SW = buildTree(SW_im, k-1);
		delete s;
		delete NW_im;
		delete NE_im;
		delete SE_im;
		delete SW_im;
		return root;

	} else{
		return nullptr;
	}


// Note that you will want to practice careful memory use
// In this function. We pass the dynamically allocated image
// via pointer so that it may be released after it is used .
// similarly, at each level of the tree you will want to 
// declare a dynamically allocated stats object, and free it
// once you've used it to choose a split point, and calculate
// an average.
}
PNG* toqutree::TreeHelper(PNG *im, int x, int y){
	unsigned int Flength = im->width();
	unsigned int length = (im->width())/2;
	PNG* AnotherIm = new PNG(length,length);
	for (unsigned int i = 0; i < length; i++) {
		for (unsigned int j = 0; j < length; j++) {
			*(AnotherIm->getPixel(i,j)) = *(im->getPixel((x+i)%Flength, (y+j)%Flength));
		}
	}
	return AnotherIm;
}

pair<int,int> toqutree::findmidpoint(PNG* im,int k) {
	int width = im->width();
	int height = im->height();
	int length = pow(2, k - 1);
	double minEntropy = 0;
	double entropySum = 0;
	double entropySE = 0;
	double entropyNE = 0;
	double entropySW = 0;
	double entropyNW = 0;
	stats* s = new stats(*im);
	pair<int, int> center = make_pair(0, 0);

	for (int i = k - 1; i < k - 1 + length - 1; i++) {
		for (int j = k - 1; j < k - 1 + length - 1; j++) {
			int ul_x = i;
			int ul_y = j;
			int lr_x = (ul_x + length - 1) % width;
			int lr_y = (ul_y + length - 1) % height;
			//when lr_y + 1 < height
			if (lr_y + 1 < height) {
				if (lr_x + 1 < width) {
					entropySE = s->entropy(make_pair(ul_x, ul_y), make_pair(lr_x, lr_y));
					entropyNE = s->entropy(make_pair(ul_x, lr_y + 1), make_pair(lr_x, height - 1)) + s->entropy(make_pair(ul_x, 0), make_pair(lr_x, ul_y - 1));
					entropySW = s->entropy(make_pair(lr_x + 1, ul_y), make_pair(width - 1, lr_y)) + s->entropy(make_pair(0, ul_y), make_pair(ul_x - 1, lr_y));
					entropyNW = s->entropy(make_pair(lr_x + 1, lr_y + 1), make_pair(width - 1, height - 1)) + s->entropy(make_pair(0, 0), make_pair(ul_x - 1, ul_y - 1))
						+ s->entropy(make_pair(0, lr_y + 1), make_pair(ul_x - 1, height - 1)) + s->entropy(make_pair(lr_x + 1, 0), make_pair(width - 1, ul_y - 1));
				}
				else if (lr_x + 1 == width) {
					entropySE = s->entropy(make_pair(ul_x, ul_y), make_pair(lr_x, lr_y));
					entropyNE = s->entropy(make_pair(ul_x, lr_y + 1), make_pair(lr_x, height - 1)) + s->entropy(make_pair(ul_x, 0), make_pair(lr_x, ul_y - 1));
					entropySW = s->entropy(make_pair(0, ul_y), make_pair(ul_x - 1, lr_y));
					entropyNW = s->entropy(make_pair(0, lr_y + 1), make_pair(ul_x - 1, height - 1)) + s->entropy(make_pair(0, 0), make_pair(ul_x - 1, ul_y - 1));
				}
				else if (lr_x + 1 > width) {
					entropySE = s->entropy(make_pair(ul_x, ul_y), make_pair(width - 1, lr_x)) + s->entropy(make_pair(0, ul_y), make_pair(lr_x, lr_y));
					entropyNE = s->entropy(make_pair(ul_x, lr_y + 1), make_pair(width - 1, height - 1)) + s->entropy(make_pair(0, 0), make_pair(lr_x, ul_y - 1))
						+ s->entropy(make_pair(0, lr_y + 1), make_pair(lr_x, height - 1)) + s->entropy(make_pair(ul_x, 0), make_pair(width - 1, ul_y - 1));
					entropySW = s->entropy(make_pair(lr_x + 1, ul_y), make_pair(ul_x - 1, lr_y));
					entropyNW = s->entropy(make_pair(lr_x + 1, lr_y + 1), make_pair(ul_x - 1, height - 1)) + s->entropy(make_pair(lr_x + 1, 0), make_pair(ul_x - 1, ul_y - 1));
				}
			}
			// when lr_y + 1 == height
			if (lr_y + 1 == height) {
				if (lr_x + 1 < width) {
					entropySE = s->entropy(make_pair(ul_x, ul_y), make_pair(lr_x, lr_y));
					entropyNE = s->entropy(make_pair(ul_x, 0), make_pair(lr_x, ul_y - 1));
					entropySW = s->entropy(make_pair(lr_x + 1, ul_y), make_pair(width - 1, lr_y)) + s->entropy(make_pair(0, ul_y), make_pair(ul_x - 1, lr_y));
					entropyNW = s->entropy(make_pair(lr_x + 1, 0), make_pair(width - 1, ul_y - 1)) + s->entropy(make_pair(0, 0), make_pair(ul_x - 1, ul_y - 1));
				}
				else if (lr_x + 1 == width) {
					entropySE = s->entropy(make_pair(ul_x, ul_y), make_pair(lr_x, lr_y));
					entropyNE = s->entropy(make_pair(ul_x, 0), make_pair(lr_x, ul_y - 1));
					entropySW = s->entropy(make_pair(0, ul_y), make_pair(ul_x - 1, lr_y));
					entropyNW = s->entropy(make_pair(0, 0), make_pair(ul_x - 1, ul_y - 1));
				}
				else if (lr_x + 1 > width) {
					entropySE = s->entropy(make_pair(ul_x, ul_y), make_pair(width - 1, height - 1)) + s->entropy(make_pair(0, ul_y), make_pair(lr_x, height - 1));
					entropyNE = s->entropy(make_pair(ul_x, 0), make_pair(width - 1, ul_y - 1)) + s->entropy(make_pair(0, 0), make_pair(lr_x, ul_y - 1));
					entropySW = s->entropy(make_pair(lr_x + 1, ul_y), make_pair(ul_x - 1, height - 1));
					entropyNW = s->entropy(make_pair(lr_x + 1, 0), make_pair(ul_x - 1, ul_y - 1));
				}
			}
			// when lr_y + 1 > height
			if (lr_y + 1 > height) {
				if (lr_x + 1 < width) {
					entropySE = s->entropy(make_pair(ul_x, ul_y), make_pair(lr_x, height - 1)) + s->entropy(make_pair(ul_x, 0), make_pair(lr_x, lr_y));
					entropyNE = s->entropy(make_pair(ul_x, lr_y + 1), make_pair(lr_x, ul_y - 1));
					entropySW = s->entropy(make_pair(lr_x + 1, ul_y), make_pair(width - 1, height - 1)) + s->entropy(make_pair(0, 0), make_pair(ul_x - 1, lr_y))
						+ s->entropy(make_pair(0, ul_y), make_pair(ul_x - 1, height - 1)) + s->entropy(make_pair(lr_x + 1, 0), make_pair(width - 1, height - 1));
					entropyNW = s->entropy(make_pair(lr_x + 1, lr_y + 1), make_pair(width - 1, ul_y - 1)) + s->entropy(make_pair(0, lr_y + 1), make_pair(ul_x - 1, ul_y - 1));
				}
				else if (lr_x + 1 == width) {
					entropySE = s->entropy(make_pair(ul_x, ul_y), make_pair(width - 1, height - 1)) + s->entropy(make_pair(ul_x, 0), make_pair(lr_x, lr_y));
					entropyNE = s->entropy(make_pair(ul_x, lr_y + 1), make_pair(lr_x, ul_y - 1));
					entropySW = s->entropy(make_pair(0, ul_y), make_pair(ul_x - 1, height - 1)) + s->entropy(make_pair(0, 0), make_pair(ul_x - 1, lr_y));
					entropyNW = s->entropy(make_pair(0, lr_y + 1), make_pair(ul_x - 1, ul_y - 1));
				}
				else if (lr_x + 1 > width) {
					entropySE = s->entropy(make_pair(ul_x, ul_y), make_pair(width - 1, height - 1)) + s->entropy(make_pair(0, 0), make_pair(lr_x, lr_y))
						+ s->entropy(make_pair(0, ul_y), make_pair(lr_x, height - 1)) + s->entropy(make_pair(ul_x, 0), make_pair(width - 1, lr_y));
					entropyNE = s->entropy(make_pair(ul_x, lr_y + 1), make_pair(width - 1, ul_y - 1)) + s->entropy(make_pair(0, lr_y + 1), make_pair(lr_x, ul_y - 1));
					entropySW = s->entropy(make_pair(lr_x + 1, ul_y), make_pair(ul_x - 1, height - 1)) + s->entropy(make_pair(lr_x + 1, 0), make_pair(ul_x - 1, lr_y));
					entropyNW = s->entropy(make_pair(lr_x + 1, lr_y + 1), make_pair(ul_x - 1, ul_y - 1));
				}
			}
			entropySum = entropySE + entropyNE + entropySW + entropyNW;
			if (entropySum < minEntropy) {
				minEntropy = entropySum;
				center = make_pair(i, j);
			}
		}
	}
	delete s;
	return center;

}



int toqutree::size() {
 return Num(root);

}

int toqutree::Num(Node*curr){
	if(curr){
		return 1+Num(curr->NW)+Num(curr->NE)+Num(curr->SE)+Num(curr->SW);
	} else {
		return 0;
	}

}

/**
    * Render returns a PNG image consisting of the pixels
    * stored in the tree. may be used on pruned trees. Draws
    * every pixel onto a PNG canvas using the 
    * average color stored in the node.
    */

PNG toqutree::render(){

// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.

/* your code here */
int dim = root->dimension;
	auto length = pow(2,dim);
	PNG png = PNG(length,length);
	renderTree(root, &png);
	return png;
}

void toqutree::renderTree(Node* curr, PNG *png){
   int width =(int) pow(2,curr->dimension);

	if(curr->SE==NULL){
		for(int i=0;i<width;i++){
			for(int j=0;j<width;j++){
				*(png->getPixel(i,j))=curr->avg;
			}
		}
		return;

	}else{
		PNG *newIm= new PNG(width/2,width/2);
		int splitX=curr->center.first;
		int splitY=curr->center.second;
		renderTree(curr->SE,newIm);
		for(int i=0;i<width/2;i++){
			for(int j=0;j<width/2;j++){
				*(png->getPixel((splitX+i)%width,(splitY+j)%width))=*(newIm->getPixel(i,j));
			}
		}
		renderTree(curr->SW,newIm);
		for(int i=0;i<width/2;i++){
			for(int j=0;j<width/2;j++){
				*(png->getPixel((splitX+width/2+i)%width,(splitY+j)%width))=*(newIm->getPixel(i,j));
			}
		}
		renderTree(curr->NE,newIm);
		for(int i=0;i<width/2;i++){
			for(int j=0;j<width/2;j++){
				*(png->getPixel((splitX+i)%width,(splitY+width/2+j)%width))=*(newIm->getPixel(i,j));
			}
		}
		renderTree(curr->NW,newIm);
		for(int i=0;i<width/2;i++){
			for(int j=0;j<width/2;j++){
				*(png->getPixel((splitX+width/2+i)%width,(splitY+width/2+j)%width))=*(newIm->getPixel(i,j));
			}
		}
		delete newIm;
	}
}



 /*
    *  Prune function trims subtrees as high as possible in the tree.
    *  A subtree is pruned (cleared) if ALL of the subtree's leaves are within 
    *  tol of the average color stored in the root of the subtree. 
    *  Pruning criteria should be evaluated on the original tree, not 
    *  on any pruned subtree. (we only expect that trees would be pruned once.)
    *  
   * You may want a recursive helper function for this one.
    */
/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){
if (root != nullptr) {
 pruneTree(root,tol);
    
}
}
void toqutree::pruneTree(Node *n, double tol) {
    if (n->SW ==  nullptr || n->SE ==  nullptr|| n->NW ==  nullptr|| n->NE ==  nullptr) {
		return;
	}
        if (tolCheck(n, n, tol)) {
             clear(n->NW);
              clear(n->NE);
            clear(n->SW);
            clear(n->SE);
            n->NW =  nullptr;
            n->NE =  nullptr;
            n->SW =  nullptr;
            n->SE =  nullptr;
        } else {
            pruneTree(n->NW, tol);
            pruneTree(n->NE, tol);
             pruneTree(n->SW, tol);
            pruneTree(n->SE, tol);
        }
    
}
bool toqutree::tolCheck(Node *n, Node *subroot, double tol){
    if ( n->SW ==  nullptr || n->SE ==  nullptr|| n->NW ==  nullptr|| n->NE ==  nullptr) {
        return n->avg.dist(subroot->avg) <= tol;
    } else {
        return tolCheck(n->NE, subroot, tol) && tolCheck(n->NW, subroot, tol)
        && tolCheck(n->SW, subroot, tol) && tolCheck(n->SE, subroot, tol);
    }
}
/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
 if (curr != nullptr) {
	 clear(curr -> NW);
	 clear(curr -> NE);
	 clear(curr -> SE);
	 clear(curr -> SW);
	 delete curr;
     curr = nullptr;
	
 }
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {
Node* nd;
if (other == nullptr) {
    nd = nullptr;
}
else{
	nd = new Node(other-> center, other-> dimension, other-> avg);
	nd->NW = copy(other -> NW);
	nd->NE = copy(other -> NE);
	nd->SW = copy(other -> SW);
	nd->SE = copy(other -> SE);
}

 return nd;
}



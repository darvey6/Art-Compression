/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"
#include <math.h>
#define _USE_MATH_DEFINES


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

	PNG * n = new PNG(pow(2,k), pow(2, k));
	for (int i = 0; i < pow(2, k); i++) {
		for (int j = 0; j < pow(2, k); j++) {
			*n->getPixel(i, j) = *imIn.getPixel(i, j);
			*n->getPixel(i, j) = *imIn.getPixel(i, j);
		}
	}

	root = buildTree(n, k);


/* This constructor grabs the 2^k x 2^k sub-image centered */
/* in imIn and uses it to build a quadtree. It may assume  */
/* that imIn is large enough to contain an image of that size. */

}

int toqutree::size() {
/* your code here */
	return 1 + sizeHelper(root->SE)  + sizeHelper(root->SW) + sizeHelper(root->NE) + sizeHelper(root->NW);
}

int sizeHelper(Node* root){
	if (root == NULL){
		return 0;
	}

	return 1 + sizeHelper(root->NW) + sizeHelper(root->NE) + sizeHelper(root->SW) sizeHelper(root->SE);
}


toqutree::Node * toqutree::buildTree(PNG * im, int k) {

	if (k >= 0) {
		Node* n;
		stats* s = new stats(*im);

		pair<int, int> ul(0, 0);
		pair<int, int> lr(pow(2, k) - 1, pow(2, k) - 1);
		HSLAPixel avg = s->getAvg(ul, lr);

		if (k == 0) {
			pair<int, int> ctr(0, 0);
			n = new Node(ctr, k, avg);

			delete(im);
			im = NULL;
		}
		else if (k == 1) {
			pair<int, int> ctr(1, 1);
			n = new Node(ctr, k, avg);

			PNG* ne = new PNG(1, 1);
			*ne->getPixel(0, 0) = *im->getPixel(0, 0);
			PNG* nw = new PNG(1, 1);
			*nw->getPixel(0, 0) = *im->getPixel(1, 0);
			PNG* se = new PNG(1, 1);
			*se->getPixel(0, 0) = *im->getPixel(1, 0);
			PNG* sw = new PNG(1, 1);
			*sw->getPixel(0, 0) = *im->getPixel(1, 1);

			n->NE = buildTree(ne, 0);
			n->NW = buildTree(nw, 0);
			n->SE = buildTree(se, 0);
			n->SW = buildTree(sw, 0);

			delete(im);
			im = NULL;
		}
		else {
			pair<int, int> ctr = getSplit(im, k, *s);
			n = new Node(ctr, k, avg);

			PNG* ne = new PNG(pow(2, k - 1), pow(2, k - 1));
			PNG* nw = new PNG(pow(2, k - 1), pow(2, k - 1));
			PNG* se = new PNG(pow(2, k - 1), pow(2, k - 1));
			PNG* sw = new PNG(pow(2, k - 1), pow(2, k - 1));

			setupImages(im, k,  ne, nw, se, sw,ctr.first,ctr.second);

			delete(im);
			im = NULL;

			n->NE = buildTree(ne, 0);
			n->NW = buildTree(nw, 0);
			n->SE = buildTree(se, 0);
			n->SW = buildTree(sw, 0);
		}

		delete(s);
		s = NULL;

		return n;
	}





// Note that you will want to practice careful memory use
// In this function. We pass the dynamically allocated image
// via pointer so that it may be released after it is used .
// similarly, at each level of the tree you will want to
// declare a dynamically allocated stats object, and free it
// once you've used it to choose a split point, and calculate
// an average.

}

void toqutree::setupImages(PNG* im, int k,  PNG* ne, PNG* nw, PNG* se, PNG* sw, int splitx, int splity) {
	int corner = pow(2, k) - 1;
	int size = pow(2, k);
	int increment = pow(2, k - 1);

	for (int i = 0; i < increment; i++) {
		for (int j = 0; j < increment; j++) {
			sw->getPixel(i, j) = im->getPixel((splitx + i)%size, (splity + j)%size);
			se->getPixel(i, j) = im->getPixel((splitx + i + increment) % size, (splity + j) % size);
			nw->getPixel(i, j) = im->getPixel((splitx + i) % size, (splity + j + increment) % size);
			ne->getPixel(i, j) = im->getPixel((splitx + i + increment) % size, (splity + j + increment) % size);
		}
	}

}

pair<int,int> toqutree::getSplit(PNG* im, int k, stats & s) {

	double minAvgEnt = 0;
	int xcord = 0;
	int ycord = 0;
	int toprow = (pow(2, k) - pow(2, k - 1)) / 2;
	int botrow = pow(2, k) - 1 - toprow;
	for (int i = toprow; i <= botrow; i++) {
		for (int j = toprow; j <= botrow; j++) {
			if (i == toprow && j == toprow) {
				minAvgEnt = getAvgEnt(im, s, k, i, j);
				xcord = i;
				ycord = j;
			}
			else if (getAvgEnt(im, k, i, j) < minAvgEnt) {
				xcord = i;
				ycord = j;
				minAvgEnt = getAvgEnt(im, s, k, i, j);

			}
		}
	}

	pair<int, int> s(xcord, ycord);

	return s;

}

double getAvgEnt(PNG* im, stats & s, int k, int splitx, int splity) {
	double total;
	int area = pow(2, k - 1)*pow(2, k - 1);
	int corner = pow(2, k) - 1;
	int increment = pow(2, k - 1);

	// if it's in the location with no combining needed
	if (splitx == pow(2, k - 1) && splity == pow(2, k - 1)) {
		pair<int, int> seUl(splitx, splity);
		pair<int, int> seLr(corner, corner);
		double setot = s.entropy(seUl, seLr);

		pair<int, int> swUl(0, splity);
		pair<int, int> swLr(splitx-1, corner);
		double swtot = s.entropy(swUl, swLr);

		pair<int, int> neUl(splitx, 0);
		pair<int, int> neLr(corner, splity-1;
		double netot = s.entropy(neUl, neLr);

		pair<int, int> nwUl(0, 0);
		pair<int, int> nwLr(splitx-1, splity-1);
		double nwtot = s.entropy(nwUl, nwLr);

		total = setot + swtot + nwtot + netot;
	} // if nothing needs to be cut vertically
	else if (splitx == pow(2, k - 1)) {
		int startY;
		if (splity > pow(2, k - 1)) {
			startY = splity - increment;
		}
		else {
			startY = splity;
		}

		// Left Solid Square
		pair<int, int> LstartUl(0, startY);
		pair<int, int> LstartLr(splitx-1, startY + increment - 1);
		double LstartTot = s.entropy(LstartUl, LstartLr);

		// Right Solid Square
		pair<int, int> RstartUl(splitx, startY);
		pair<int, int> RstartLr(corner, startY + increment - 1);
		double RstartTot = s.entropy(RstartUl, RstartUl);

		// Left Vertical Split Square
		pair<int, int> LBotUl(0, startY + increment);
		pair<int, int> LBotLr(splitx - 1, corner);
		vector<int> LBotHist = s.buildHist(LBotUl, LBotLr);
		pair<int, int> LTopUl(0, 0);
		pair<int, int> LTopLr(splitx - 1, startY - 1);
		vector<int> LTopHist = s.buildHist(LTopUl, LTopLr);

		for (int i = 0; i < 36; i++) {
			LTopHist.at(i) += LBotHist.at(i);
		} // LTopHist stores sum

		double LnextTot = s.entropy(LTopHist, area);

		//Right Vertical Split Square
		pair<int, int> RBotUl(splitx, startY + increment);
		pair<int, int> RBotLr(corner, corner);
		vector<int> RBotHist = s.buildHist(RBotUl, RBotLr);
		pair<int, int> RTopUl(splitx, 0);
		pair<int, int> RTopLr(corner, startY - 1);
		vector<int> RTopHist = s.buildHist(RTopUl, RTopLr);

		for (int i = 0; i < 36; i++) {
			RTopHist.at(i) += RBotHist.at(i);
		}

		double RnextTot = s.entropy(RTopHist, area);

		//Compute Total
		total = LstartTot + RstartTot + LnextTot + RnextTot;

	} // Nothing needs to be cut horizontally
	else if (splity == pow(2, k - 1)) {
		int startX;
		if (splitx > pow(2, k - 1)) {
			startX = splitx - increment;
		}
		else {
			startX = splitx;
		}

		// Top Solid Square
		pair<int, int> TstartUl(startX, 0);
		pair<int, int> TstartLr(startX + increment - 1, splity - 1);
		double TstartTot = s.entropy(TstartUl, TstartLr);

		// Bottom Solid Square
		pair<int, int> BstartUl(startX, splity);
		pair<int, int> BstartLr(startX + increment - 1, corner);
		double BstartTot = s.entropy(BstartUl, BstartLr);

		// Top Horizontal Split Square
		pair<int, int> TLeftUl(startX + increment, 0);
		pair<int, int> TLeftLr(corner, splity - 1);
		vector<int> TLeftHist = s.buildHist(TLeftUl, TLeftLr);
		pair<int, int> TRightUl(0, 0);
		pair<int, int> TRightLr(startX - 1, splity - 1);
		vector<int> TRightHist = s.buildHist(TRightUl, TRightLr);

		for (int i = 0; i < 36; i++) {
			TLeftHist.at(i) += TRightHist.at(i);
		}

		double TnextTot = s.entropy(TLeftHist, area);

		// Bottom Horizontal Split Square
		pair<int, int> BLeftUl(startX + increment, splity);
		pair<int, int> BLeftLr(corner, corner);
		vector<int> BLeftHist = s.buildHist(BLeftUl, BLeftLr);
		pair<int, int> BRightUl(0, splity);
		pair<int, int> BRightLr(startX - 1, corner);
		vector<int> BRightHist = s.buildHist(BRightUl, BRightLr);

		for (int i = 0; i < 36; i++) {
			BLeftHist.at(i) += BRightHist.at(i);
		}

		double BnextTot = s.entropy(BLeftHist, area);

		total = TstartTot + BstartTot + TnextTot + BnextTot;
	}
	else {
		//get top left coordinate of only solid square
		int startX;
		int startY;
		if (splitx > pow(2, k - 1)) {
			startX = splitx - increment;
		}
		else {
			startX = splitx;
		}
		if (splity > pow(2, k - 1)) {
			startY = splity - increment;
		}
		else {
			startY = splity;
		}

		// Only Solid Square
		pair<int, int> SolidUl(startX, startY);
		pair<int, int> SolidLr(startX + increment - 1, startY + increment - 1);
		double Solidtot = s.entropy(SolidUl, SolidLr);

		// Vertically Split Square (one on top of other)
		pair<int, int> BotUl(startX, startY + increment);
		pair<int, int> BotLr(startX + increment - 1, corner);
		vector<int> BotHist = s.buildHist(BotUl, BotLr);
		pair<int, int> TopUl(startX, 0);
		pair<int, int> TopLr(startX + increment - 1, startY - 1);
		vector<int> TopHist = s.buildHist(TopUl, TopLr);

		for (int i = 0; i < 36; i++) {
			BotHist.at(i) += TopHist.at(i);
		}

		double VertSplitTot = s.entropy(BotHist, area);

		// Horizontally Split Square (one next to other)
		pair<int, int> LeftUl(startX + increment, startY);
		pair<int, int> LeftLr(corner, startY + increment - 1);
		vector<int> LeftHist = s.buildHist(LeftUl, LeftLr);
		pair<int, int> RightUl(0, startY);
		pair<int, int> RightLr(startX - 1, startY + increment - 1);
		vector<int> RightHist = s.buildHist(RightUl, RightLr);

		for (int i = 0; i < 36; i++) {
			LeftHist.at(i) += RightHist.at(i);
		}

		double HorizSplitTot = s.entropy(LeftHist, area);

		// Heavily Split Square (bunch of corners)
		pair<int, int> UprightCornUl(startX + increment, 0);
		pair<int, int> UprightCornLr(corner, startY - 1);
		vector<int> UprightCornHist = s.buildHist(UprightCornUl, UprightCornLr);
		pair<int, int> UpleftCornUl(0, 0);
		pair<int, int> UpleftCornLr(startX - 1, startY - 1);
		vector<int> UpleftCornHist = s.buildHist(UpleftCornUl, UpleftCornLr);
		pair<int, int> DownrightCornUl(startX + increment, startY + increment);
		pair<int, int> DownrightCornLr(corner, corner);
		vector<int> DownrightCornHist = s.buildHist(DownrightCornUl, DownrightCornLr);
		pair<int, int> DownleftCornUl(0, startY + increment);
		pair<int, int> DownleftCornLr(startX - 1, corner);
		vector<int> DownleftCornHist = s.buildHist(DownleftCornUl, DownleftCornLr);

		for (int i = 0; i < 36; i++) {
			UprightCornHist.at(i) = UprightCornHist.at(i) + UpleftCornHist.at(i) + DownrightCornHist.at(i) + DownleftCornHist.at(i);
		}

		double HeavySplitTot = s.entropy(UprightCornHist, area);


		total = Solidtot + VertSplitTot + HorizSplitTot + HeavySplitTot;

	}
	return total;
}

PNG toqutree::render(){

// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the
// quadtree, instead.

/* your code here */


}

/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){
	double current_upperbound = root.avg.h * (M_PI/180) + tol;
	double currnet_lowerbound = root.avg.h * (M_PI/180) - tol;
	if (pruneHelper(root->NE, current_upperbound, current_lowerbound, tol) &&  pruneHelper(root->NW, current_upperbound, current_lowerbound, tol) && pruneHelper(root->SE, current_upperbound, current_lowerbound, tol) && pruneHelper(root->SW, current_upperbound, current_lowerbound, tol)){
		clear(&root);
	}

}

bool pruneHelper(Node* root, double upperBound, double lowerBound, double  tol){
	if (root == NULL){
		return true;
	}

	//checking if leaf node is within the tol bound
	if (root->NE == NULL && root->NW == NULL && root->SE == NULL && root->NW == NULL){
		return root.avg.h * (M_PI/180)< upperBound && root.avg.h * (M_PI/180)>lowerBound;
	}

	// comparing parent's tol bound to the current node tol bound, then picking the tighter bound
	// used for child of the current node;
	double current_upperbound = root.avg.h * (M_PI/180) + tol;
	double currnet_lowerbound = root.avg.h * (M_PI/180) - tol;
	if (upperBound < current_upperbound){
		current_upperbound = upperBound;
	}
	if (lowerBound > current_lowerbound){
		current_lowerbound = lowerBound;
	}

	// checking if each subtree's leaf node are within the tolerance bound, if yes then returns true, ow returns false
	if (pruneHelper(root->NE, current_upperbound, current_lowerbound, tol) &&  pruneHelper(root->NW, current_upperbound, current_lowerbound, tol) && pruneHelper(root->SE, current_upperbound, current_lowerbound, tol) && pruneHelper(root->SW, current_upperbound, current_lowerbound, tol)){
		clear(&root);
		return true;
	}
	return false;
}


/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
/* your code here */

}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {

}

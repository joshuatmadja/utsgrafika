#include "shape.h"

#define PI  3.14159265
#define EPS 0.00001
using namespace std;

//Below is used to determine wether a point is inside a polygon
//Code from Competitive Programming 3 chapter 7
struct vec{
	double x,y;
	vec(double _x, double _y) : x(_x), y(_y) {}
};

vec toVec(Point a, Point b){
	return vec(b.getX() - a.getX(), b.getY() - a.getY());
}

double dot(vec a, vec b){
	return a.x * b.x + a.y * b.y;
}

double norm_sq(vec v){
	return v.x * v.x + v.y * v.y;
}

double angle(Point A, Point O, Point B){
	vec oa = toVec(O,A), ob = toVec(O,B);
	return acos(dot(oa,ob) / sqrt(norm_sq(oa) * norm_sq(ob)));
}

double cross(vec a, vec b){
	return a.x * b.y - a.y * b.x;
}

bool ccw(Point P, Point Q, Point R){
	return cross(toVec(P,Q), toVec(P,R)) > 0;
}

bool is_inside_polygon(Point P, vector<Point> edges){
	//Code from Competitive programming 3 page 287
	double sum = 0;
	edges.push_back(edges[0]);
	for(int i=0; i < (int) edges.size()-1; i++){
		if (ccw(P, edges[i], edges[i+1]))
			sum+=angle(edges[i], P, edges[i+1]);
		else
			sum-=angle(edges[i], P, edges[i+1]);
	}
	return fabs( fabs(sum) - 2 * PI ) < EPS;

}


Point calculate_center( vector<Point>& edge){
	if(edge.size() <= 0)
		return Point(0,0);
	int sumX=0, sumY=0;
	for(int i=0; i<edge.size(); i++){
		sumX += edge[i].getX();
		sumY += edge[i].getY();
	}
	return Point(sumX/edge.size(), sumY/edge.size());
}

Point getFloodFillSeed( vector<Point>& edge){
	int  MaxX = -1,MaxY=-1, MinX=1000000, MinY=1000000;
	for(int i=0; i<edge.size(); i++){
		MaxX = max(MaxX,edge[i].getX());
		MaxY = max(MaxY,edge[i].getY());
		MinX = min(MinX,edge[i].getX());
		MinY = min(MinY,edge[i].getY());
	}
	int x = rand() % (MaxX - MinX +1) + MinX ;
	int y = rand() % (MaxY - MinY +1) + MinY ;
	Point P(x,y);
	while(is_inside_polygon(P, edge) == false){
		x = rand() % (MaxX - MinX +1) + MinX ;
	 	y = rand() % (MaxY - MinY +1) + MinY ;

	 	P = Point(x,y);
	}
	return P;
}

Shape::Shape(){
	edges.clear();
	edges_modified.clear();
	Border = Color(0,0,0);
	Fill = Color(0,0,0);
}

Shape::Shape(vector<Point>& starting_edge, Color C ){
	edges.clear();
	edges = starting_edge;
	edges_modified = starting_edge;
	Border = C;
	Fill = Color(0,0,0);

	center = calculate_center(starting_edge);
	floodfill_seed = getFloodFillSeed(edges);
}

Shape::~Shape(){
	edges.clear();
	edges_modified.clear();
}

Shape::Shape(const Shape &obj){
	this->floodfill_seed = obj.floodfill_seed;
	this->edges = obj.edges;
	this->center = obj.center;
	this->Border=obj.Border;
	this->Fill = obj.Fill;
}

Shape& Shape::operator=(const Shape &obj){
	this->floodfill_seed = obj.floodfill_seed;
	this->edges = obj.edges;
	this->center = obj.center;
	this->Border=obj.Border;
	this->Fill = obj.Fill;
	return *this;
}
void Shape::moveBy(int deltaX, int deltaY){
	erase();
	for(int i=0; i<edges.size(); i++){
		edges[i].moveBy(deltaX, deltaY);
	}
	center.moveBy(deltaX, deltaY);
	floodfill_seed.moveBy(deltaX,deltaY);
	draw();
}



//rotate the object by theta degree clockwise
void Shape::Rotate(int theta){
	erase();
	center = calculate_center(edges);
	for(int i=0; i<edges.size(); i++){
		edges_modified[i].moveBy(-center.getX(), -center.getY());
		edges_modified[i].moveBy(center.getX(), center.getY());
		edges_modified[i].rotate(theta);
	}
	floodfill_seed.moveBy(-center.getX(), -center.getY());
	floodfill_seed.rotate(theta);
	floodfill_seed.moveBy(center.getX(), center.getY());
	draw();
}


void Shape::erase(){
	linedrawer.drawPolygon(edges_modified,Border );
	linedrawer.floodFill4Seed(floodfill_seed.getX(), floodfill_seed.getY(), Border, Color(0,0,0));
	linedrawer.drawPolygon(edges_modified,Color(0,0,0) );
}

void Shape::draw(){
	linedrawer.drawPolygon(edges_modified,Border);
	linedrawer.floodFill4Seed(floodfill_seed.getX(), floodfill_seed.getY(), Border, Fill);
}

//set floodfill color
void Shape::setFillColor(Color C){
	erase();
	Fill = C;
	draw();
}
//set Border Color to color c
void Shape::setBorderColor(Color c){
	erase();
	Border = c;
	draw();
}

void Shape::setCenter(Point P){
	center = P;
}

//tes rotate poros
void Shape::RotatePoros(int theta, Point poros){
	erase();
	for(int i=0; i<edges.size(); i++){
		//edges[i].moveBy(-center.getX(), -center.getY());
		edges[i].rotatePoros(theta, poros);
		//edges[i].moveBy(center.getX(), center.getY());

	}
	//floodfill_seed.moveBy(-center.getX(), -center.getY());
	floodfill_seed.rotatePoros(theta, poros);
	//floodfill_seed.moveBy(center.getX(), center.getY());
	draw();
}

//tes pesawat parabola
void Shape::PlaneParabola(int theta, Point poros){
	erase();
	for(int i=0; i<edges.size(); i++){
		edges[i].rotatePoros(theta, poros);
	}
	floodfill_seed.rotatePoros(theta, poros);
	draw();
}

void Shape::scale(double x){
	for(int i=0; i<edges.size();i++){
		edges_modified[i].x = edges[i].x * x;
		edges_modified[i].y = edges[i].y * x;
	}
	draw();
}

void Shape::zoom(Point center, double scale){
    int p_size = edges_modified.size();
		for(int j = 0;j < p_size; ++j){
			edges_modified[j] = edges[j].scaleUp(center,scale);
		}
}

vector<Point> Shape::getEdges(){
	return edges;
}

vector<Point> Shape::getEdgesModified(){
	return edges_modified;
}

Color Shape::getFill(){
	return Fill;
}

Color Shape::getBorder(){
	return Border;
}

Point Shape::getFloodFill_Seed(){
	return floodfill_seed;
}

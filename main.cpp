#include "datastructure/shape.h"
#include <iostream>

using namespace std;

Screen screen;
LineDrawer linedrawer;
/* Shape for obstacle */
Shape* world_shape[10]; //polygon
void BuildRandomShape(){
  int n = rand() % 10;
  int i, j;
  for (i = 0; i < n; i++){
    Color c(255, 255, 255);
    int rnd_num = rand() % 3;
    vector<Point> v;
    Point center(rand() % 200 + 200, rand() % 200 + 200);
    cout << rnd_num << endl;
    if (rnd_num == 0){
      /* Triangle */
        Point p1(center.getX(), center.getY() - 25);
        Point p2(center.getX()-25, center.getY() + 25);
        Point p3(center.getX()+25, center.getY() + 25);
        v.push_back(p1);
        v.push_back(p2);
        v.push_back(p3);
        world_shape[i] = new Shape(v, c);
        world_shape[i]->draw();
        break;
      }
      else if (rnd_num == 1){/* Rectangle */
        Point p1(center.getX()-25, center.getY() - 25);
        Point p2(center.getX()+25, center.getY() - 25);
        Point p3(center.getX()-25, center.getY() + 25);
        Point p4(center.getX()+25, center.getY() + 25);
        v.push_back(p1);
        v.push_back(p2);
        v.push_back(p3);
        v.push_back(p4);
        world_shape[i] = new Shape(v, c);
        world_shape[i]->draw();
      }
      /* other polygon */
      else{
        Point p1(center.getX()-25, center.getY() - 10);
        Point p2(center.getX(), center.getY() - 25);
        Point p3(center.getX()+25, center.getY() - 10);
        Point p4(center.getX()-25, center.getY() +10);
        Point p5(center.getX(), center.getY() + 25);
        Point p6(center.getX()+25, center.getY() + 10);
        v.push_back(p1);
        v.push_back(p2);
        v.push_back(p3);
        v.push_back(p4);
        v.push_back(p5);
        v.push_back(p6);
        world_shape[i] = new Shape(v, c);
        world_shape[i]->draw();
      }
    }
}


int main(){
	BuildRandomShape();
	vector<Point> edge;
	edge.push_back(Point(100,100));
	edge.push_back(Point(200,100));
	edge.push_back(Point(200,200));

	Shape s(edge, Color(255,0,0));
	//s.setFillColor(Color(0,255,050));
	s.draw();
	/*while(1){
		s.moveBy(10,1);
		sleep(1);
	}*/

	return 0;
}

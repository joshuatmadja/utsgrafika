#include "datastructure/shape.h"
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <termios.h>

using namespace std;

char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

/* Base color */
Color black(0,0,0);
Color green(0,204,0);
Color blue(0,0,204);
Color red(204, 0, 0);
Color white(255, 255, 255);

Screen screen;
LineDrawer linedrawer;
/* Shape for obstacle */
Shape* world_shape[10]; //polygon
int N;
Shape* frame;

void BuildRandomShape(){
  /* Random sheet*/
  srand(time(NULL));
  N = rand() % 10;
  int i, j;
  for (i = 0; i < N; i++){
    Color c(rand() % 255, rand() % 255, rand() % 255);
    /* Random sheet*/
    int rnd_num = rand() % 3;
    vector<Point> v;
    Point center(rand() % 200 + 200, rand() % 200 + 200);
    if (rnd_num == 0){
      /* Triangle */
        Point p1(center.getX(), center.getY() - 25);
        Point p2(center.getX()-25, center.getY() + 25);
        Point p3(center.getX()+25, center.getY() + 25);
        v.push_back(p1);
        v.push_back(p2);
        v.push_back(p3);
        world_shape[i] = new Shape(v, c);
      }
      else if (rnd_num == 1){
        /* Rectangle */
        Point p1(center.getX()-25, center.getY() - 25);
        Point p2(center.getX()+25, center.getY() - 25);
        Point p4(center.getX()+25, center.getY() + 25);
        Point p3(center.getX()-25, center.getY() + 25);
        v.push_back(p1);
        v.push_back(p2);
        v.push_back(p4);
        v.push_back(p3);
        world_shape[i] = new Shape(v, c);
      }
      /* other polygon */
      else{
        Point p1(center.getX()-25, center.getY() - 10);
        Point p2(center.getX(), center.getY() - 25);
        Point p3(center.getX()+25, center.getY() - 10);
        Point p4(center.getX()+25, center.getY() +10);
        Point p5(center.getX(), center.getY() + 25);
        Point p6(center.getX()-25, center.getY() + 10);
        v.push_back(p1);
        v.push_back(p2);
        v.push_back(p3);
        v.push_back(p4);
        v.push_back(p5);
        v.push_back(p6);
        world_shape[i] = new Shape(v, c);
      }
      world_shape[i]->draw();
      //world_shape[i]->setFillColor(c);
    }
}

void createFrame(){
  /* Rectangle */
  vector<Point> v;
  Point p1(100,100);
  Point p2(500,100);
  Point p4(500, 500);
  Point p3(100, 500);
  v.push_back(p1);
  v.push_back(p2);
  v.push_back(p4);
  v.push_back(p3);
  frame = new Shape(v, white);
  frame -> draw();
}

/* Belom Bener */
void zoom_world(Point center, double scale){
  for (int i = 0; i < N; i++){
    world_shape[i]->erase();
    world_shape[i]->zoom(center, scale);
  }
}

// bool inside_frame(Point P, bool zoomed){
// 	if(zoomed) return P.getX() < width_kecil && P.getY() < height && P.getX() >= 0 && P.getY() >= 0;
// 	else return P.getX() < width && P.getY() < height && P.getX() >= 0 && P.getY() >= 0;
// }

int main(){
  screen.ClearScreen();
  createFrame();
  Point center(250,250);
  // vector<Point> v;
  // Point p1(center.getX(), center.getY() - 25);
  // Point p2(center.getX()-25, center.getY() + 25);
  // Point p3(center.getX()+25, center.getY() + 25);
  // v.push_back(p1);
  // v.push_back(p2);
  // v.push_back(p3);
  // Shape k(v, green);
  // k.draw();
  // sleep(1);
  // k.zoom(center, 2.0);
  BuildRandomShape();
  while(1){
    sleep(1);
    zoom_world(center, 2.0);
  }
	return 0;
}

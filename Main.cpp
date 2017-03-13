#include <iostream>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
//#include <thread>
#include <vector>
#include "datastructure/shape.h"
using namespace std;

/* Base color */
Color black(0,0,0);
Color green(0,204,0);
Color blue(0,0,204);
Color red(204, 0, 0);
Color white(255, 255, 255);

/* Shape for obstacle */
Shape* world_shape[10]; //polygon

/* Build random shape from a random position */
void BuildRandomShape(){
  int n = rand() % 10;
  int i, j;
  for (i = 0; i < n; i++){
    Color c(rand()%255, rand()%255, rand()%255);
    int rnd_num = rand() % 3;
    vector<Point> v;;
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
  return 0;
}

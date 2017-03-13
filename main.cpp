#include "datastructure/shape.h"
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <termios.h>
using namespace std;

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

/* Base color */
Color black(0,0,0);
Color green(0,204,0);
Color blue(0,0,204);
Color red(204, 0, 0);
Color white(255, 255, 255);

/* Global Variable */
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
    double rnd_scale = fRand(0.0, 2.0);
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
      }
      world_shape[i] = new Shape(v, c);
      world_shape[i]->scale(rnd_scale);
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

/* Print inside frame (Clipping) Masih Salah!!!!!*/
void Print_Inside_Frame(){
  vector<Point> checked;
  for (int i = 0; i < N; i++){
    world_shape[i]->erase();
    for (int j = 0; j < world_shape[i]->getEdgesModified().size(); j++){
      /* Check wheter points of shape is in the frame or not */
      if ((world_shape[i]->getEdgesModified()[j].getX() > 100) && (world_shape[i]->getEdgesModified()[j].getX() < 500)
        && (world_shape[i]->getEdgesModified()[j].getY() > 100) && (world_shape[i]->getEdgesModified()[j].getY() < 500)){
          checked.push_back(world_shape[i]->getEdgesModified()[j]);
      }
      else if (world_shape[i]->getEdgesModified()[j].getX() < 100){
          Point temp(100, world_shape[i]->getEdgesModified()[j].getY());
          checked.push_back(temp);
      }
      else if (world_shape[i]->getEdgesModified()[j].getX() > 500){
          Point temp(100, world_shape[i]->getEdgesModified()[j].getY());
          checked.push_back(temp);
      }
      else if (world_shape[i]->getEdgesModified()[j].getY() < 100){
          Point temp(world_shape[i]->getEdgesModified()[j].getY(), 100);
          checked.push_back(temp);
      }
      if (world_shape[i]->getEdgesModified()[j].getY() > 500){
          Point temp(world_shape[i]->getEdgesModified()[j].getY(), 100);
          checked.push_back(temp);
      }
    }

    /* Draw line checked */
    linedrawer.drawPolygon(checked, world_shape[i]->getBorder());
  	linedrawer.floodFill4Seed(world_shape[i]->getFloodFill_Seed().getX(), world_shape[i]->getFloodFill_Seed().getY(),
      world_shape[i]->getBorder(), world_shape[i]->getFill());
    checked.clear();
  }
}

int main(){
  screen.ClearScreen();
  createFrame();
  Point center(250,250);
  BuildRandomShape();
  Print_Inside_Frame();
  while(1){
    sleep(1);
    screen.ClearScreen();
    createFrame();
    zoom_world(center, 2.0);
    Print_Inside_Frame();
  }
	return 0;
}

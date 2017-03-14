#include "datastructure/shape.h"
#include "datastructure/Circle.h"
#include <iostream>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <iostream>
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <thread>
#include <termios.h>

using namespace std;
const struct timespec* delayperframe = (const struct timespec[]){{0,2*16666667L}};
static termios old, news;

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
Point center_world(300,300);
float scale = 1.0;

/* Shape for obstacle */
Shape* world_shape[10]; //polygon
int N;
Shape* frame;
Shape* cursor;

/* Objective */
Circle* obj;

void initTermios(int echo){
  tcgetattr(0, &old);
  news = old;
  news.c_lflag &= ~ICANON;
  news.c_lflag &= echo ? ECHO : ~ECHO;
  tcsetattr(0, TCSANOW, &news);
}

void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &old);
}

char getch_(int echo){
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

char getch(void){
  return getch_(0);
}

void BuildRandomShape(){
  /* Random sheet*/
  srand(time(NULL));
  N =  rand() %  9 + 1;
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
      world_shape[i] = new Shape(v, white);
      world_shape[i]->setFillColor(c);
      world_shape[i]->setFloodFillSeed(center);
      world_shape[i]->scale(rnd_scale, center);
    }
}

void create_objective(){
  srand(time(NULL));
  Point center(rand() % 200 + 200, rand() % 200 + 200);
  int rds = rand()%3+3;
  obj = new Circle(center, rds, white);
}

bool checkWin(){
  /* Titik atas kursor */
  Point p1(center_world.getX(),center_world.getY()-5);
  float b = abs(p1.getY()-obj->getCenter().getY());
  float a = abs(p1.getX()-obj->getCenter().getX());
  int rads = obj->getRadius();
  int jarak = sqrt((a*a)+(b*b));
  if (jarak <= rads){
    return true;
  }
  else{
    return false;
  }
}

void make_center_point(){
  Point p1(center_world.getX(),center_world.getY()-5);
  Point p2(center_world.getX()-5,center_world.getY()+5);
  Point p3(center_world.getX(),center_world.getY()+3);
  Point p4(center_world.getX()+5,center_world.getY()+5);
  vector<Point> v;
  v.push_back(p1);
  v.push_back(p2);
  v.push_back(p3);
  v.push_back(p4);
  cursor = new Shape(v, white);
  cursor->setFillColor(white);
  cursor->setFloodFillSeed(center_world);
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

/* Zoom all shape in world */
void zoom_world(Point c, double s){
  for (int i = 0; i < N; i++){
    world_shape[i]->erase();
    world_shape[i]->zoom(c, s);
  }
  obj->zoom(c,s);
}

/* Print inside frame (Clipping) Masih Salah!!!!!*/
void Print_Inside_Frame(){
  zoom_world(center_world,scale);
  scale = 1.0;
	for (int i = 0; i < N; i++){
   		world_shape[i]->draw();
  }
  obj->draw();
  cursor->draw();
}

void move_world(int deltaX, int deltaY){
  for (int i = 0; i < N; i++){
    world_shape[i]->moveBy(deltaX,deltaY);
  }
  obj->moveBy(deltaX, deltaY);
}

void initAll(){
  create_objective();
  make_center_point();
  BuildRandomShape();
  Print_Inside_Frame();
  createFrame();
}

int main(){
  bool win = false;
  int life = 3;
  screen.ClearScreen();
  linedrawer.setView(Point(100,100),Point(500,500));
  initAll();
  while(!win){
    /* Lose */
    if (life == 0){
      break;
    }
    int a = getch();
    //cout << a << endl;
    switch (a){
      case 97 :
        //cout << "geser kiri" << endl;
        move_world(1,0);
        usleep(1000);
        break;
      case 100 :
        //cout << "geser kanan" << endl;
        move_world(-1,0);
        usleep(1000);
        break;
      case 115 :
        //cout << "geser atas" << endl;
        move_world(0,-1);
        usleep(1000);
        break;
      case 119 :
        //cout << "geser bawah" << endl;
        move_world(0,1);
        usleep(1000);
        break;
      case 91 :
        //cout << "zoom out" << endl;
        scale -= 0.3;
        usleep(1000);
        break;
      case 93 :
        //cout << "zoom in" << endl;
        scale += 0.3;
        usleep(1000);
        break;
      case 32 :
        if (checkWin()){
          win = true;
        }
        else{
          life--;
        }
        break;
    }
    screen.ClearScreen();
    Print_Inside_Frame();
    createFrame();
  }
	return 0;
}

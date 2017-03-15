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
      world_shape[i] = new Shape(v, c);
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
  screen.ClearScreen();
  linedrawer.setView(Point(0,0), Point(1000, 1000));
  vector<Point> temp;

  temp.push_back(Point( 150,100 ));
  temp.push_back(Point( 100,100 ));
  temp.push_back(Point( 100,150 ));
  temp.push_back(Point( 135,150 ));
  temp.push_back(Point( 135,175 ));
  temp.push_back(Point( 100,175 ));
  temp.push_back(Point( 100,200 ));
  temp.push_back(Point( 150,200 ));
  temp.push_back(Point( 150,100 ));

  Shape G(temp,red);
  G.setFillColor(red);
  G.draw();



  temp.clear();

  temp.push_back(Point( 140,110 ));
  temp.push_back(Point( 110,110 ));
  temp.push_back(Point( 110,135 ));
  temp.push_back(Point( 140,135 ));
  temp.push_back(Point( 140,110 ));
  Shape Go(temp,black);
  Go.setFillColor(black);
  Go.draw();

  //bikin A
  temp.clear();
  temp.push_back(Point( 175,100 ));
  temp.push_back(Point( 175,200 ));
  temp.push_back(Point( 190,200 ));
  temp.push_back(Point( 190,165 ));
  temp.push_back(Point( 235,165 ));
  temp.push_back(Point( 235,200 ));
  temp.push_back(Point( 250,200 ));
  temp.push_back(Point( 250,100 ));
  temp.push_back(Point( 175,100 ));

  Shape A(temp,red);
  A.setFillColor(red);
  A.draw();

  temp.clear();
  temp.push_back(Point( 190,110 ));
  temp.push_back(Point( 235,110 ));
  temp.push_back(Point( 235,140 ));
  temp.push_back(Point( 190,140 ));
  temp.push_back(Point( 190,110 ));
  Shape Ao(temp,black);
  Ao.setFillColor(black);
  Ao.draw();

  //bikin M
  temp.clear();
  temp.push_back(Point( 275,100 ));
  temp.push_back(Point( 300,100 ));
  temp.push_back(Point( 325,150 ));
  temp.push_back(Point( 350,100 ));
  temp.push_back(Point( 375,100 ));
  temp.push_back(Point( 375,200 ));
  temp.push_back(Point( 350,200 ));
  temp.push_back(Point( 350,130 ));
  temp.push_back(Point( 325,180 ));
  temp.push_back(Point( 300,130 ));
  temp.push_back(Point( 300,200 ));
  temp.push_back(Point( 275,200 ));
  temp.push_back(Point( 275,100 ));

  Shape M(temp,red);
  M.setFillColor(red);
  M.draw();

  //bikin E
  temp.clear();
  temp.push_back(Point( 400,100 ));
  temp.push_back(Point( 400,200 ));
  temp.push_back(Point( 450,200 ));
  temp.push_back(Point( 450,180 ));
  temp.push_back(Point( 420,180 ));
  temp.push_back(Point( 420,160 ));
  temp.push_back(Point( 450,160 ));
  temp.push_back(Point( 450,140 ));
  temp.push_back(Point( 420,140 ));
  temp.push_back(Point( 420,120 ));
  temp.push_back(Point( 450,120 ));
  temp.push_back(Point( 450,100 ));
  temp.push_back(Point( 400,100 ));

  Shape E1(temp,red);
  E1.setFillColor(red);
  E1.draw();

  //Bikin O
  temp.clear();
  temp.push_back(Point( 250,250 ));
  temp.push_back(Point( 320,250 ));
  temp.push_back(Point( 320,350 ));
  temp.push_back(Point( 250,350 ));
  temp.push_back(Point( 250,250 ));
  Shape O(temp,red);
  O.setFillColor(red);
  O.draw();

  temp.clear();
  temp.push_back(Point( 270,270 ));
  temp.push_back(Point( 300,270 ));
  temp.push_back(Point( 300,330 ));
  temp.push_back(Point( 270,330 ));
  temp.push_back(Point( 270,270 ));
  Shape Oo(temp,black);
  Oo.setFillColor(black);
  Oo.draw();

  //Bikin V
  temp.clear();
  temp.push_back(Point( 360,250 ));
  temp.push_back(Point( 380,250 ));
  temp.push_back(Point( 400,330 ));
  temp.push_back(Point( 420,250 ));
  temp.push_back(Point( 440,250 ));
  temp.push_back(Point( 415,350 ));
  temp.push_back(Point( 385,350 ));
  temp.push_back(Point( 360,250 ));
  Shape V(temp,red);
  V.setFillColor(red);
  V.draw();

  //Bikin E2
  temp.clear();
  temp.push_back(Point( 480,250 ));
  temp.push_back(Point( 480,350 ));
  temp.push_back(Point( 530,350 ));
  temp.push_back(Point( 530,330 ));
  temp.push_back(Point( 500,330 ));
  temp.push_back(Point( 500,310 ));
  temp.push_back(Point( 530,310 ));
  temp.push_back(Point( 530,290 ));
  temp.push_back(Point( 500,290 ));
  temp.push_back(Point( 500,270 ));
  temp.push_back(Point( 530,270 ));
  temp.push_back(Point( 530,250 ));
  temp.push_back(Point( 480,250 ));
  Shape E2(temp,red);
  E2.setFillColor(red);
  E2.draw();

  //bikin R
  temp.clear();
  temp.push_back(Point( 575,250 ));
  temp.push_back(Point( 575,350 ));
  temp.push_back(Point( 590,350 ));
  temp.push_back(Point( 590,315 ));
  temp.push_back(Point( 635,350 ));
  temp.push_back(Point( 650,350 ));
  temp.push_back(Point( 630,315 ));
  temp.push_back(Point( 650,315 ));
  temp.push_back(Point( 650,250 ));
  temp.push_back(Point( 575,250 ));
  Shape R(temp,red);
  R.setFillColor(red);
  R.draw();


  temp.clear();
  temp.push_back(Point( 595,270 ));
  temp.push_back(Point( 595,300 ));
  temp.push_back(Point( 630,300 ));
  temp.push_back(Point( 630,270 ));
  temp.push_back(Point( 595,270 ));
  Shape Ro(temp,black);
  Ro.setFillColor(black);
  Ro.draw();

  temp.clear();
  temp.push_back(Point( 550,100 ));
  temp.push_back(Point( 560,125 ));
  temp.push_back(Point( 580,115 ));
  temp.push_back(Point( 560,135 ));
  temp.push_back(Point( 570,160 ));
  temp.push_back(Point( 550,140 ));
  temp.push_back(Point( 530,160 ));
  temp.push_back(Point( 540,135 ));
  temp.push_back(Point( 520,115 ));
  temp.push_back(Point( 540,125 ));
  temp.push_back(Point( 550,100 ));
  Shape star(temp,green);
  star.setFillColor(green);
  star.draw();
  star.setCenter(Point(550,130));

  while(1){
  	screen.ClearScreen();
  	G.draw();
  	Go.draw();
  	A.draw();
  	Ao.draw();
  	M.draw();
  	E1.draw();
  	O.draw();
  	Oo.draw();
  	V.draw();
  	E2.draw();
  	R.draw();
  	Ro.draw();
  	star.setCenter(Point(550,130));
  	star.Rotate(10);
  	star.draw();
  	usleep(500000);
  }

  return 0;
}

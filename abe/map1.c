#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <math.h>
#include "event.c"

typedef struct Points {
    int x;
    int y;
} Point;
typedef struct Colors {
    int a;
    int r;
    int g;
    int b;
} Color;

typedef struct Buildings {

    int neff;
    Point * P;
} Building;

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

Color bg; // warna background
char* fbp; // memory map ke fb0
int fbfd; // file fb0
int screensize; // length nya si fbp
Point * window_center;
int bytePerPixel;
int overPixel = 10;
Point * window;
Color c_map;
int nBuilding = 0;
int nBuilding2 = 0;
int nBuilding3 = 0;
long location2;

Building * building;
Building * goal;
Building * goal_temp;

int i;
int window_width = 790;
int window_height = 590;

int zoom_width = 15;
int zoom_height = 15;

void setPoint(Point* p, int x, int y) {
    p -> x = x;
    p -> y = y;
}

void swapPoint(Point* p1, Point* p2) {
    int x = p1 -> x;
    int y = p1 -> y;
    setPoint(p1, p2 -> x, p2 -> y);
    setPoint(p2, x, y);
}

void setColor(Color* c, char r, char g, char b) {
    c -> a = 0;
    c -> r = r;
    c -> g = g;
    c -> b = b;
}

void changeARGB(int location, Color* c) {
    *(fbp + location) = c -> r;
    *(fbp + location + 1) = c -> g;
    *(fbp + location + 2) = c -> b;
    *(fbp + location + 3) = c -> a;
}

void clearScreen(Color* c) {
    int i, j;
    // iterasi pixel setiap baris dan setiap kolom
    for (j = 0; j < vinfo.yres; j++) {
        for (i = 0; i < vinfo.xres; i++) {
			long location = (i+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (j+vinfo.yoffset) * finfo.line_length;
            changeARGB(location, c);
            //location += bytePerPixel; // pixel sebelah kanannya berjarak sekian byte
        }
        //location += (bytePerPixel * overPixel); // pixel pertama pada baris selanjutnya berjarak sekian byte
    }
}

void loadBuildings4() {
    //printf("TES");
    char c;
    int valx, valy;
    Point pTemp;
    FILE *file;
    file = fopen("buildings.txt", "r");
    int i, j;
    building = malloc(200 * sizeof(Building));
    for (i = 0; i < 200; i++) {
        building[i].P = (Point *) malloc(50 * sizeof(Point));
    }
    int itbuilding;
    int itpoint;
    i = 0;

    while (!feof(file)) {
        do {
                c = getc(file);
                if (feof(file)) break;
        } while (c!='#');
        do {
                c = getc(file);
                if (feof(file)) break;
        } while (c!='\n');
        j = 0;
        if (!feof(file)) {
            while (c = getc(file) != '#'){
                fscanf (file, "%d", &valx);
                fscanf (file, "%d", &valy);
                setPoint(&building[i].P[j], valx, valy);
                j++;
            }
            building[i].neff = j;

            i++;
        }
    };
    nBuilding2 = i;
    fclose(file);
}

void loadBuildings5() {
    //printf("TES");
    char c;
    int valx, valy;
    Point pTemp;
    FILE *file;
    file = fopen("buildings2.txt", "r");
    int i, j;
    goal = malloc(200 * sizeof(Building));
    goal_temp = malloc(200 * sizeof(Building));
    for (i = 0; i < 200; i++) {
        goal[i].P = (Point *) malloc(50 * sizeof(Point));
        goal_temp[i].P = (Point *) malloc(50 * sizeof(Point));
    }
    int itgoal;
    int itpoint;
    i = 0;

    while (!feof(file)) {
        do {
                c = getc(file);
                if (feof(file)) break;
        } while (c!='#');
        do {
                c = getc(file);
                if (feof(file)) break;
        } while (c!='\n');
        j = 0;
        if (!feof(file)) {
            while (c = getc(file) != '#'){
                fscanf (file, "%d", &valx);
                fscanf (file, "%d", &valy);
                setPoint(&goal[i].P[j], valx, valy);
                j++;
            }
            goal[i].neff = j;
            i++;
        }
    };
    nBuilding3 = i;
    fclose(file);
}

int getOctant (int dx, int dy) {
    /*  peta octant:
        \2|1/
        3\|/0
        --+--
        4/|\7
        /5|6\
    */

    if (dy >= 0 && dy < dx) {
        return 0;
    } else if (dx > 0 && dx <= dy) {
        return 1;
    } else if (dx <= 0 && dx > -dy) {
        return 2;
    } else if (dy > 0 && dy <= -dx) {
        return 3;
    } else if (dy <= 0 && dy > dx) {
        return 4;
    } else if (dx < 0 && dx >= dy) {
        return 5;
    } else if (dx >= 0 && dx < -dy) {
        return 6;
    } else { // dy < 0 && dy >= -dx
        return 7;
    }
}

/*==============DRAW LINE UNTUK ZOOM==================*/
void drawZoomLineX(Point* p1, Point* p2, Color* c, int positif) {
	// algoritma Bresenham
    int dx = p2 -> x - p1 -> x;
    int dy = (p2 -> y - p1 -> y) * positif;
    int d = dy + dy - dx;
    int j = p1 -> y;
    int i, location;
    // lebih panjang horizontal, maka iterasi berdasarkan x
    for (i = p1 -> x; i <= p2 -> x; i++) {
		if(i<0 || i>vinfo.xres-1 || j<0 || j>vinfo.yres-1){
		}
		else{
			if(i<=window[0].x || i>=window[2].x || j<=window[0].y || j>=window[2].y){

			}
			else{
				Color * temp = &c_map;
				location = (i + vinfo.xoffset) * bytePerPixel + (j + vinfo.yoffset) * finfo.line_length;
				changeARGB(location, temp);
			}
		}
        if (d > 0) {
            // positif = 1 berarti y makin lama makin bertambah
            j += positif;
            d -= dx;
        }
        d += dy;
    }
}

void drawZoomLineY(Point* p1, Point* p2, Color* c, int positif) {
	// algoritma Bresenham
    int dx = (p2 -> x - p1 -> x) * positif;
    int dy = p2 -> y - p1 -> y;
    int d = dx + dx - dy;
    int i = p1 -> x;
    int j, location;
    // lebih panjang vertikal, maka iterasi berdasarkan y
    for (j = p1 -> y; j <= p2 -> y; j++) {
		if(i<0 || i>vinfo.xres-1 || j<0 || j>vinfo.yres-1){
		}
		else{
			if(i<=window[0].x || i>=window[2].x || j<=window[0].y || j>=window[2].y){

			}
			else{
				Color * temp = &c_map;
				location = (i + vinfo.xoffset) * bytePerPixel + (j + vinfo.yoffset) * finfo.line_length;
				changeARGB(location, temp);
			}
		}
        if (d > 0) {
            // positif = 1 berarti x makin lama makin bertambah
            i += positif;
            d -= dy;
        }
        d += dx;
    }
}


void drawZoomLine(Point* p1, Point* p2, Color* c) {
    int dx = p2 -> x - p1 -> x;
    int dy = p2 -> y - p1 -> y;
    int octant = getOctant(dx, dy);    
    // algoritma bresenham sebenernya hanya bisa menggambar di octant 0,
    // atur sedemikian rupa supaya 7 octant lainnya bisa masuk ke algoritma
    switch (octant) {
        case 0: drawZoomLineX(p1, p2, c, 1); break;
        case 1: drawZoomLineY(p1, p2, c, 1); break;
        case 2: drawZoomLineY(p1, p2, c, -1); break;
        case 3: swapPoint(p1, p2); drawZoomLineX(p1, p2, c, -1); swapPoint(p1, p2); break;
        case 4: swapPoint(p1, p2); drawZoomLineX(p1, p2, c, 1); swapPoint(p1, p2); break;
        case 5: swapPoint(p1, p2); drawZoomLineY(p1, p2, c, 1); swapPoint(p1, p2); break;
        case 6: swapPoint(p1, p2); drawZoomLineY(p1, p2, c, -1); swapPoint(p1, p2); break;
        case 7: drawZoomLineX(p1, p2, c, -1); break;
    }
}
/*------------------------------------------------------*/

/*==============DRAW LINE UNTUK BIASA===========*/
void drawLineX(Point* p1, Point* p2, Color* c, int positif) {
	// algoritma Bresenham
    int dx = p2 -> x - p1 -> x;
    int dy = (p2 -> y - p1 -> y) * positif;
    int d = dy + dy - dx;
    int j = p1 -> y;
    int i, location;
    // lebih panjang horizontal, maka iterasi berdasarkan x
    for (i = p1 -> x; i <= p2 -> x; i++) {
		if(i<0 || i>vinfo.xres-1 || j<0 || j>vinfo.yres-1){
		}
		else{
			location = (i + vinfo.xoffset) * bytePerPixel + (j + vinfo.yoffset) * finfo.line_length;
			changeARGB(location, c);
		}
        if (d > 0) {
            // positif = 1 berarti y makin lama makin bertambah
            j += positif;
            d -= dx;
        }
        d += dy;
    }
}

void drawLineY(Point* p1, Point* p2, Color* c, int positif) {
	// algoritma Bresenham
    int dx = (p2 -> x - p1 -> x) * positif;
    int dy = p2 -> y - p1 -> y;
    int d = dx + dx - dy;
    int i = p1 -> x;
    int j, location;
    // lebih panjang vertikal, maka iterasi berdasarkan y
    for (j = p1 -> y; j <= p2 -> y; j++) {
		if(i<0 || i>vinfo.xres-1 || j<0 || j>vinfo.yres-1){
		}
		else{
			location = (i + vinfo.xoffset) * bytePerPixel + (j + vinfo.yoffset) * finfo.line_length;
			changeARGB(location, c);
		}
        if (d > 0) {
            // positif = 1 berarti x makin lama makin bertambah
            i += positif;
            d -= dy;
        }
        d += dx;
    }
}

void drawLine(Point* p1, Point* p2, Color* c) {
    int dx = p2 -> x - p1 -> x;
    int dy = p2 -> y - p1 -> y;
    int octant = getOctant(dx, dy);    
    // algoritma bresenham sebenernya hanya bisa menggambar di octant 0,
    // atur sedemikian rupa supaya 7 octant lainnya bisa masuk ke algoritma
    //printf("%d-%d\n",p1->x,p1->y);
    //printf("%d-%d\n",p2->x,p2->y);
    switch (octant) {
        case 0: drawLineX(p1, p2, c, 1); break;
        case 1: drawLineY(p1, p2, c, 1); break;
        case 2: drawLineY(p1, p2, c, -1); break;
        case 3: swapPoint(p1, p2); drawLineX(p1, p2, c, -1); swapPoint(p1, p2); break;
        case 4: swapPoint(p1, p2); drawLineX(p1, p2, c, 1); swapPoint(p1, p2); break;
        case 5: swapPoint(p1, p2); drawLineY(p1, p2, c, 1); swapPoint(p1, p2); break;
        case 6: swapPoint(p1, p2); drawLineY(p1, p2, c, -1); swapPoint(p1, p2); break;
        case 7: drawLineX(p1, p2, c, -1); break;
    }
}
/*------------------------------------------------------*/

void connectBuffer() {
    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }
    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }
    // Figure out the size of the screen in bytes
    bytePerPixel = vinfo.bits_per_pixel / 8;
    screensize = (vinfo.xres + overPixel) * vinfo.yres * bytePerPixel;
    // Map the device to memory
    fbp = (char*) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (*fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
}

Point * initWindow(Point * window_center){
	Point * window = (Point*) malloc(5 * sizeof(Point));
	int x = window_center->x;
	int y = window_center->y;
	setPoint(&window[0], x-window_width/2, y-window_height/2);
	setPoint(&window[1], x+window_width/2, y-window_height/2);
	setPoint(&window[2], x+window_width/2, y+window_height/2);
	setPoint(&window[3], x-window_width/2, y+window_height/2);
	setPoint(&window[4], x-window_width/2, y-window_height/2);
	return window;
}

Point * initZoomWindow(Point * window_center){
	Point * zoom = (Point*) malloc(4 * sizeof(Point));
	int x = window_center->x;
	int y = window_center->y;
	setPoint(&zoom[0], x, y-zoom_height/2);
	setPoint(&zoom[1], x+zoom_width/2, y+zoom_height/2);
	setPoint(&zoom[2], x-zoom_width/2, y+zoom_height/2);
	setPoint(&zoom[3], x, y-zoom_height/2);
	return zoom;
}

void drawZoomBuilding(int zoom, Point* p, int numPoints, Color c) {
	int k, i;
		for (i = 0; i < numPoints-1; i++) {
			drawZoomLine(&p[i], &p[i+1], &c);
		}
		drawZoomLine(&p[i], &p[0], &c);
}

void drawZoomMap(Building* building, Color c) {
	int i, j;
	for(i = 0; i < nBuilding; i++) {
		drawZoomBuilding(1, building[i].P, building[i].neff, c);
	}
}

void zoom(float zoom, Point * zoomPoint, Building * building){
	Point * zoomtemp;
	zoomtemp = (Point*) malloc(1 * sizeof(Point));
	zoomtemp[0].x = zoomPoint->x * zoom;
	zoomtemp[0].y = zoomPoint->y * zoom;
	int deltax = zoomtemp[0].x - window_center->x;
	deltax=deltax*-1;
	int deltay = zoomtemp[0].y - window_center->y;
	deltay=deltay*-1;
	
	Building * temp;
	temp = malloc(nBuilding * sizeof(Building));
	for (int it = 0; it < nBuilding; it++) {
		temp[it].neff = building[it].neff;
        temp[it].P = (Point *) malloc(50 * sizeof(Point));
    }
    
	int a, b;
	for(a = 0; a < nBuilding; a++) {
		for (b = 0; b < building[a].neff; b++) {
			temp[a].P[b].x = (building[a].P[b].x * zoom) + deltax;
			temp[a].P[b].y = (building[a].P[b].y * zoom) + deltay;
		}
	}
	
	Color c;
	setColor(&c, 255, 0, 0);
	drawZoomMap(temp, c);
	free(temp);
}

void solidFill(Point* firepoint, Color c){
	Point newfp;
    if(firepoint->x>1 && firepoint->x<vinfo.xres-1 && firepoint->y>1 && firepoint->y<vinfo.yres-1){
        newfp.x = firepoint->x+1;
        newfp.y = firepoint->y;
        long location = (newfp.x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (newfp.y+vinfo.yoffset) * finfo.line_length;
        if(*(fbp + location)!=c.r || *(fbp + location +1)!=c.g || *(fbp + location +2) !=c.b){
            *(fbp + location) =c.r;
            *(fbp + location +1) = c.g;
            *(fbp + location +2) = c.b;
            *(fbp + location + 3) = c.a;
            solidFill(&newfp, c);
        }
        
        newfp.x = firepoint->x-1;
        newfp.y = firepoint->y;
        location = (newfp.x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (newfp.y+vinfo.yoffset) * finfo.line_length;
        if(*(fbp + location)!=c.r || *(fbp + location +1)!=c.g || *(fbp + location +2) !=c.b){
            *(fbp + location) =c.r;
            *(fbp + location +1) = c.g;
            *(fbp + location +2) = c.b;
            *(fbp + location + 3) = c.a;
            solidFill(&newfp, c);
        }
        
        newfp.x = firepoint->x;
        newfp.y = firepoint->y+1;
        location = (newfp.x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (newfp.y+vinfo.yoffset) * finfo.line_length;
        if(*(fbp + location)!=c.r || *(fbp + location +1)!=c.g || *(fbp + location +2) !=c.b){
            *(fbp + location) =c.r;
            *(fbp + location +1) = c.g;
            *(fbp + location +2) = c.b;
            *(fbp + location + 3) = c.a;
            solidFill(&newfp, c);
        }
        
        newfp.x = firepoint->x;
        newfp.y = firepoint->y-1;
        location = (newfp.x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (newfp.y+vinfo.yoffset) * finfo.line_length;
        if(*(fbp + location)!=c.r || *(fbp + location +1)!=c.g || *(fbp + location +2) !=c.b){
            *(fbp + location) =c.r;
            *(fbp + location +1) = c.g;
            *(fbp + location +2) = c.b;
            *(fbp + location + 3) = c.a;
            solidFill(&newfp, c);
        }
    }
}

//maaf ya parameter nya banyak males pisah
void initSolidFill(Point * firepoint, Point * polygon, int size_of_polygonArr, float zoom, Point * zoomPoint, Color cFill){
	int isInsideView = 1;
	int j;
	for(j = 0; j < size_of_polygonArr; j++) {
		if(polygon[j].x<0 || polygon[j].x>599 || polygon[j].y<0 || polygon[j].y>599){
			isInsideView = 0;
			break;
		}
	}
	
	if(isInsideView==1){
		Point * new_firepoint = (Point*) malloc(1 * sizeof(Point));
		setPoint(&new_firepoint[0], firepoint[0].x, firepoint[0].y);
		
		Point * zoomtemp;
		zoomtemp = (Point*) malloc(1 * sizeof(Point));
		zoomtemp[0].x = zoomPoint->x * zoom;
		zoomtemp[0].y = zoomPoint->y * zoom;
		int deltax = zoomtemp[0].x - window_center->x;
		deltax=deltax*-1;
		int deltay = zoomtemp[0].y - window_center->y;
		deltay=deltay*-1;
		
		new_firepoint[0].x = (new_firepoint[0].x * zoom) + deltax;
		new_firepoint[0].y = (new_firepoint[0].y * zoom) + deltay;
		
		//paksa ke border
		if(new_firepoint[0].x<0){
			new_firepoint[0].x=2;
		}
		else if(new_firepoint[0].x>vinfo.xres-1){
			new_firepoint[0].x=vinfo.xres-2;
		}
		
		if(new_firepoint[0].y>vinfo.yres-1){
			new_firepoint[0].y=vinfo.xres-2;
		}
		else if(new_firepoint[0].y<0){
			new_firepoint[0].y=2;
		}
		//printf("%d %d\n",new_firepoint[0].x,new_firepoint[0].y);
		solidFill(new_firepoint, cFill);
	}
	else{
		
	}
}

int main() {
    setColor(&bg, 0, 0, 0);
    connectBuffer();
    clearScreen(&bg);
    Color c, cDel, c_goal;
    setColor(&c, 255, 0, 0);
    setColor(&c_map, 0, 255, 255);
    setColor(&cDel, 255, 255, 0);
    setColor(&c_goal, 255, 0, 255);

	//inisialisasi view's border
    window_center = (Point*) malloc(1 * sizeof(Point));
	setPoint(&window_center[0], 400, 300);
	window = initWindow(window_center);
	
	//inisialisasi cursor's border
	Point * cursor_center = (Point*) malloc(1 * sizeof(Point));
	Point * cursor = (Point*) malloc(5 * sizeof(Point));
	setPoint(&cursor_center[0], 400, 300);
	cursor = initZoomWindow(cursor_center);
	
	Point * goals_center = (Point*) malloc(1 * sizeof(Point));
	setPoint(&goals_center[0], 309, 200);
	
    int i,j;
    char ch;
    //load dari file
    loadBuildings4();
    loadBuildings5();
    float zoom_val = 3;
    int finish = 0;
    char stroke;
    while(1){
		clearScreen(&bg);
		//draw map
		nBuilding = nBuilding2;
		setColor(&c_map, 0, 255, 255);
		zoom(zoom_val,cursor_center,building);
		nBuilding = nBuilding3;
		setColor(&c_map, 255, 0, 255);
		zoom(zoom_val,cursor_center,goal);
		
		//draw view's border
		for(j = 0; j < 4; j++) {
			drawLine(&window[j], &window[j + 1], &c);
		}
		
		//draw cursor's border
		for(j = 0; j < 3; j++) {
			drawLine(&cursor[j], &cursor[j + 1], &cDel);
		}
		initSolidFill(cursor_center,cursor,4,zoom_val,cursor_center,cDel);
		
		stroke = getch();
		switch (stroke) {
			case 'd': cursor_center->x+=5; break;
			case 's': cursor_center->y+=5; break;
			case 'a': cursor_center->x-=5; break;
			case 'w': cursor_center->y-=5; break;
			case 'z':
				zoom_val*=2;
				break;
			case 'x':
				zoom_val=zoom_val/2;
				break;
					
		}
		if(finish==1){
			clearScreen(&bg);
			printf("kamu menang\n");
			break;
		}
	}
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}

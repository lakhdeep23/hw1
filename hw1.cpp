//Lakhdeep Singh
//WORKS
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

//#include "log.h"
//#include "ppm.h"
extern "C" {
        #include "fonts.h"
}

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 1000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
    float x, y, z;
};

struct Shape {
    float width, height;
    float radius;
    Vec center;
};

struct Particle {
    Shape s;
    Vec velocity;
};

struct Game {
    Shape box;
    Shape box2;
        Shape box3;
        Shape box4;
        Shape box5;
        Shape circle;

    Particle particle[MAX_PARTICLES];
    int n;
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);
void circleDraw (float rad);


int main(void)
{
    srand (time(NULL));
        int done=0;
    srand(time(NULL));
    initXWindows();
    init_opengl();
    //declare game object
    Game game;
    game.n=0;

    //declare a box shape
    game.box.width = 80;
    game.box.height = 10;
    game.box.center.x = -200 + 5*65;
    game.box.center.y = 800 - 5*60;

    game.box2.width = 80;
    game.box2.height = 10;
    game.box2.center.x = -100 + 5*65;
    game.box2.center.y = 750 - 5*60;
    
    game.box3.width = 80;
        game.box3.height = 10;
    game.box3.center.x = 00 + 5*65;
    game.box3.center.y = 700 - 5*60;

    game.box4.width = 80;
        game.box4.height = 10;
    game.box4.center.x = 100 + 5*65;
    game.box4.center.y = 650 - 5*60;
    
    game.box5.width = 80;
    game.box5.height = 10;
    game.box5.center.x = 200 + 5*65;
    game.box5.center.y = 600 - 5*60;

    game.circle.radius = 100;
    game.circle.center.x = 700;
    game.circle.center.y = 100;
    game.circle.center.z = 0;
    


    //start animation
    while(!done) {
        while(XPending(dpy)) {
            XEvent e;
            XNextEvent(dpy, &e);
            check_mouse(&e, &game);
            done = check_keys(&e, &game);
        }
        movement(&game);
        render(&game);
        glXSwapBuffers(dpy, win);
    }
    cleanupXWindows();
    return 0;
}

void set_title(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "HW1 CS335");
}

void cleanupXWindows(void) {
    //do not change
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

void initXWindows(void) {
    //do not change
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        std::cout << "\n\tcannot connect to X server\n" << std::endl;
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if(vi == NULL) {
        std::cout << "\n\tno appropriate visual found\n" << std::endl;
        exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                            ButtonPress | ButtonReleaseMask |
                            PointerMotionMask |
                            StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
                    InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    //Set the screen background color
    glClearColor(0.1, 0.1, 0.1, 1.0);
}

void makeParticle(Game *game, int x, int y) {
    if (game->n >= MAX_PARTICLES)
        return;
    std::cout << "makeParticle() " << x << " " << y << std::endl;
    //position of particle
    
    Particle *p = &game->particle[game->n];
    p->s.center.x = x;
    p->s.center.y = y;
    int r = -(rand() % 4) + 1;
    p->velocity.y = r;
    int z = rand() % 2 + 1;
    p->velocity.x =  z;
    game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
    static int savex = 0;
    static int savey = 0;
    static int n = 0;

    if (e->type == ButtonRelease) {
        return;
    }
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) {
            //Left button was pressed
            int y = WINDOW_HEIGHT - e->xbutton.y;
            makeParticle(game, e->xbutton.x, y);
            return;
        }
        if (e->xbutton.button==3) {
            //Right button was pressed
            return;
        }
    }
    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        savex = e->xbutton.x;
        savey = e->xbutton.y;
        if (++n < 10)
            return;
        //int y = WINDOW_HEIGHT - e->xbutton.y;
        //makeParticle(game, e->xbutton.x, y);

    }
}

int check_keys(XEvent *e, Game *game)
{
    //Was there input from the keyboard?
    if (e->type == KeyPress) {
        int key = XLookupKeysym(&e->xkey, 0);
        if (key == XK_Escape) {
            return 1;
        }
        //You may check other keys here.
        if (key == XK_B) {
            makeParticle(game,120,540);
        }
    }
    return 0;
}

void movement(Game *game)
{
    Particle *p;

    if (game->n <= 0)
        return;
    
    for (int i=0; i<game->n; i++) {
        p = &game->particle[i];
        p->s.center.x += p->velocity.x;
        p->s.center.y += p->velocity.y;
    
        //gravity
        p->velocity.y -= 0.2;   
    
        //check for collision with shapes...
        double bounce = -.5;
        Shape *s;
        s = &game->box;
        if ((p->s.center.y > s->center.y - (s->height/2.0)) && 
        (p->s.center.x > s->center.x - (s->width)))
            if ((p->s.center.y <= s->center.y + (s->height/2.0)) && 
            (p->s.center.x <= s->center.x + (s->width)))
                p->velocity.y *= bounce;
                if (p->velocity.x > 1) {
                    p->velocity.x *= .9;
                }
         s = &game->box2;
            if ((p->s.center.y > s->center.y - (s->height/2.0)) &&
            (p->s.center.x > s->center.x - (s->width)))
                if ((p->s.center.y <= s->center.y + (s->height/2.0)) &&
                    (p->s.center.x <= s->center.x + (s->width)))
                    p->velocity.y *= bounce;
                    if (p->velocity.x > 1) {
                            p->velocity.x *= .9;
                    }
            s = &game->box3;
            if ((p->s.center.y > s->center.y - (s->height/2.0)) &&
        (p->s.center.x > s->center.x - (s->width)))
                    if ((p->s.center.y <= s->center.y + (s->height/2.0)) &&
                    (p->s.center.x <= s->center.x + (s->width)))
                        p->velocity.y *= bounce;
                        if (p->velocity.x > 1) {
                                p->velocity.x *= .9;
                        }
            s = &game->box4;
            if ((p->s.center.y > s->center.y - (s->height/2.0)) &&
            (p->s.center.x > s->center.x - (s->width)))
                    if ((p->s.center.y <= s->center.y + (s->height/2.0)) &&
                    (p->s.center.x <= s->center.x + (s->width)))
                        p->velocity.y *= bounce;
                        if (p->velocity.x > 1) {
                                p->velocity.x *= .9;
                    }
            s = &game->box5;
            if ((p->s.center.y > s->center.y - (s->height/2.0)) &&
            (p->s.center.x > s->center.x - (s->width)))
                    if ((p->s.center.y <= s->center.y + (s->height/2.0)) &&
                    (p->s.center.x <= s->center.x + (s->width)))
                        p->velocity.y *= bounce;
                        if (p->velocity.x > 1) {
                                p->velocity.x *= .9;
                        }
	s = &game->circle;
	float distance;
	float distx = (p->s.center.x - s->center.x);
	distx *= distx;
	float disty = (p->s.center.y - s->center.y);
	disty *= disty;
	distance = sqrt(distx + disty);

	if (distance < s->radius){
	    p->velocity.x *= -1;
	    p->velocity.y *= -0.7;
	}

		

        //check for off-screen
        if (p->s.center.y < 0.0) {
            std::cout << "off screen" << std::endl;
            game->particle[i] = game->particle[game->n - 1];
            game->n--;
        }
    }
}

void render(Game *game)
{
    makeParticle(game,120,540);

        float w, h;
    glClear(GL_COLOR_BUFFER_BIT);
    //Draw shapes...
    //draw circles
//draw boxes
    Shape *s;
    glColor3ub(90,140,90);
    s = &game->box;
    glPushMatrix();
    glTranslatef(s->center.x, s->center.y, s->center.z);
    w = s->width;
    h = s->height;
    glBegin(GL_QUADS);
        glVertex2i(-w,-h);
        glVertex2i(-w, h);
        glVertex2i( w, h);
        glVertex2i( w,-h);
    glEnd();
    glPopMatrix();
    
    glColor3ub(90,140,90);
    s = &game->box2;
    glPushMatrix();
    glTranslatef(s->center.x, s->center.y, s->center.z);
    w = s->width;
    h = s->height;
    glBegin(GL_QUADS);
        glVertex2i(-w,-h);
        glVertex2i(-w, h);
        glVertex2i( w, h);
        glVertex2i( w,-h);
    glEnd();
    glPopMatrix();
    
    glColor3ub(90,140,90);
    s = &game->box3;
    glPushMatrix();
    glTranslatef(s->center.x, s->center.y, s->center.z);
    w = s->width;
    h = s->height;
    glBegin(GL_QUADS);
        glVertex2i(-w,-h);
        glVertex2i(-w, h);
        glVertex2i( w, h);
        glVertex2i( w,-h);
    glEnd();
    glPopMatrix();
    
    glColor3ub(90,140,90);
    s = &game->box4;
    glPushMatrix();
    glTranslatef(s->center.x, s->center.y, s->center.z);
    w = s->width;
    h = s->height;
    glBegin(GL_QUADS);
        glVertex2i(-w,-h);
        glVertex2i(-w, h);
        glVertex2i( w, h);
        glVertex2i( w,-h);
    glEnd();
    glPopMatrix();


	glColor3ub(130,60,90);
	s = &game->circle;
	glPushMatrix();
	glTranslatef(s->center.x, s->center.y, s->center.z);
	circleDraw(s->radius);
	glPopMatrix();
    
    glColor3ub(90,140,90);
    s = &game->box5;
    glPushMatrix();
    glTranslatef(s->center.x, s->center.y, s->center.z);
    w = s->width;
    h = s->height;
    glBegin(GL_QUADS);
        glVertex2i(-w,-h);
        glVertex2i(-w, h);
        glVertex2i( w, h);
        glVertex2i( w,-h);
    glEnd();
    glPopMatrix();
    
    //draw circle
	


    //draw all particles here
    glPushMatrix();
    /*if (color ==1)
        glColor3ub(100,100,250);
    else if (color == 2)
        glColor3ub(50,50,220);
    else if (color == 3)
        glColor3ub(0,0,220);
    else if (color == 4)
        glColor3ub(50,100,250);
    else
        glColor3ub(50,50,220);
            */      
    for (int i=0; i<game->n; i++) {
        int color = rand() % 5;
        if (color ==1)
            glColor3ub(200,200,250);
        else if (color == 2)
            glColor3ub(160,160,250);
        else if (color == 3)
            glColor3ub(150,150,250);
        else if (color == 4)
            glColor3ub(180,180,250);
        else
            glColor3ub(150,150,250);
        Vec *c = &game->particle[i].s.center;
        w = 2;
        h = 2;
        glBegin(GL_QUADS);
            glVertex2i(c->x-w, c->y-h);
            glVertex2i(c->x-w, c->y+h);
            glVertex2i(c->x+w, c->y+h);
            glVertex2i(c->x+w, c->y-h);
        glEnd();
        glPopMatrix();
    }
}

void circleDraw (float rad)
    {
   	int i;
	static int firsttime=1;
	static float verts[32][2];
	static int n=32;
	if (firsttime) {
		float ang=0.0;
		float inc = 3.14159 * 2.0 / (float)n;
		for (i=0; i<n; i++) {
			verts[i][0] = sin(ang);
			verts[i][1] = cos(ang);
			ang += inc;
		}
		firsttime=0;
	}
	glBegin(GL_TRIANGLE_FAN);
		for (i=0; i<n; i++) {
			glVertex2f(verts[i][0]*rad, verts[i][1]*rad);
		}
	glEnd();
}


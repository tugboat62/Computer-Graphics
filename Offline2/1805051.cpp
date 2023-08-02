#define _USE_MATH_DEFINES

#include <iostream>
#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "bitmap_image.hpp"

using namespace std;

float width, height, dx, dy, left_x, top_y;
bitmap_image image;

float z_max = 2.0;

float **z_buffer;

struct point
{
    GLfloat x, y, z;
};

vector<point> points;

struct point pos; // position of the eye
struct point l;   // look/forward direction
struct point r;   // right direction
struct point u;   // up direction

static unsigned long int g_seed = 1;
inline int random()
{
 g_seed = (214013 * g_seed + 2531011);
 return (g_seed >> 16) & 0x7FFF;
}

/* Initialize OpenGL Graphics */
void initGL()
{
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}


void drawAxes()
{
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); // Red
    // X axis
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);

    glColor3f(0, 1, 0); // Green
    // Y axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);

    glColor3f(0, 0, 1); // Blue
    // Z axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);
    glEnd();
}

void drawTriangle(int up_down, GLdouble dec)
{
    glBegin(GL_TRIANGLES);
    glVertex3d(dec, 0, 0);           // Top
    glVertex3d(0, up_down * dec, 0); // Bottom-left
    glVertex3d(0, 0, dec);           // Bottom-right
    glEnd();
}

void display()
{
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
    glLoadIdentity();           // Reset the model-view matrix

    // default arguments of gluLookAt
    // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

    // control viewing (or camera)
    gluLookAt(pos.x, pos.y, pos.z,
              pos.x + l.x, pos.y + l.y, pos.z + l.z,
              u.x, u.y, u.z);

    glutSwapBuffers(); // Render now
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0)
        height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity();            // Reset the projection matrix
    /*if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }*/
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void readFile() {
    ifstream file("./config.txt");
    ifstream inputfile("./stage3.txt");
    if (file.is_open() && inputfile.is_open())
    {
        cout << "Files opened successfully." << endl;
    }
    else
    {
        cout << "Failed to open the files. Exiting program..." << endl;
        exit(1);
    }

    string line;
    float coord[3];
    while (getline(file, line))
    {
        string word;
        stringstream ss(line);
        int j = 0;
        while (ss >> word)
        {
            float number = stod(word);
            coord[j] = number;
            j++;
        }
    }

    file.close();
    width = coord[0];
    height = coord[1];

    int count = 0;
    while (getline(inputfile, line))
    {
        count++;
        string word;
        stringstream ss(line);
        int j = 0;
        while (ss >> word)
        {
            float number = stod(word);
            coord[j] = number;
            j++;
        }
        point p = {coord[0], coord[1], coord[2]};
        points.push_back(p);
    }
    
    float max_x = points[0].x, max_y = points[0].y, min_x = points[0].x, min_y = points[0].y;

    for (int i = 0; i < points.size(); i++)
    {
        // cout << points[i].x << " " << points[i].y << " " << points[i].z << endl;
        if(points[i].x > max_x) {
            max_x = points[i].x;
        }
        if(points[i].y > max_y) {
            max_y = points[i].y;
        }
        if(points[i].x < min_x) {
            min_x = points[i].x;
        }
        if(points[i].y < min_y) {
            min_y = points[i].y;
        }
    }
    // cout << "max_x: " << max_x << " max_y: " << max_y << " min_x: " << min_x << " min_y: " << min_y << endl;
    cout << "width: " << width << " height: " << height << endl;
    dx = (max_x - min_x) / width;
    dy = (max_y - min_y) / height;
    top_y = max_y - dy/2;
    left_x = min_x + dx/2;
    cout << "dx: " << dx << " dy: " << dy << endl;
    cout << "top_y: " << top_y << " left_x: " << left_x << endl;

    
}

void initialize_z_buffer() {
    z_buffer = new float*[(int)width];
 
    for (int i = 0; i < width; i++) {
        z_buffer[i] = new float[(int)height];
    }
    for (int i=0; i<width; i++) {
        for (int j=0; j<height; j++) {
            z_buffer[i][j] = z_max;
        }
    }
    image.setwidth_height(width, height);
    image.set_all_channels(0, 0, 0);
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{

    readFile();
    initialize_z_buffer();

    image.save_image("out.bmp");
    
    for (int i = 0; i < width; i++) 
        delete[] z_buffer[i];
    delete[] z_buffer;

    return 0;
}


// glutInit(&argc, argv);                                    // Initialize GLUT
    // glutInitWindowSize(width, height);                        // Set the window's initial width & height
    // glutInitWindowPosition(50, 50);                           // Position the window's initial top-left corner
    // glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
    // glutCreateWindow("OpenGL 3D Drawing");                    // Create a window with the given title
    // glutDisplayFunc(display);                                 // Register display callback handler for window re-paint
    // // glutReshapeFunc(reshapeListener);                         // Register callback handler for window re-shape
    // // glutKeyboardFunc(keyboardListener);                       // Register callback handler for normal-key event
    // // glutSpecialFunc(specialKeyListener);                      // Register callback handler for special-key event
    // initGL();       // Our own OpenGL initialization
    // glutMainLoop(); // Enter the event-processing loop
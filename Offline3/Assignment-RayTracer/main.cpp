#define _USE_MATH_DEFINES

#include <iostream>
#include <stdio.h>
#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "bitmap_image.hpp"
#include "shapes.h"

using namespace std;

bitmap_image image;
vector<sphere> spheres;
vector<pyramid> pyramids;
vector<cube> cubes;
vector<triangle> triangles;
vector<square> squares;
vector<normallight> normal_lights;
vector<spotlight> spot_lights;
vector<void *> objects;

struct point pos(0,-200, 35); // position of the eye
struct point l;   // look/forward direction
struct point r;   // right direction
struct point u;   // up direction

GLfloat near_plane;
GLfloat far_plane;
GLfloat fov, aspect_ratio, recursion_level, checkerboard;
GLfloat ka, kd, kr;
int pixel_size, no_objects, normal_light, spot_light;
int numTiles = 20;
float windowWidth = 4;
float windowHeight = 4;
int imageCount = 1;

/* Initialize OpenGL Graphics */
void initGL()
{
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}

// Global variables

float angle = 0.0;   // Rotation angle for animation
bool rotate = false; // Rotate triangle?
int drawgrid = 0;    // Toggle grids

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

void drawPyramid(struct pyramid p)
{
    glBegin(GL_TRIANGLES); // Begin drawing the pyramid with 4 triangles
    // Front
    glVertex3f(0.0f, p.height, 0.0f);
    glVertex3f(0.0f, 0.0f, p.width / sqrt(2));
    glVertex3f(p.width / sqrt(2), 0.0f, 0.0f);

    // Right
    glVertex3f(0.0f, p.height, 0.0f);
    glVertex3f(p.width / sqrt(2), 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -p.width / sqrt(2));

    // Back
    glVertex3f(0.0f, p.height, 0.0f);
    glVertex3f(0.0f, 0.0f, -p.width / sqrt(2));
    glVertex3f(-p.width / sqrt(2), 0.0f, 0.0f);

    // Left
    glVertex3f(0.0f, p.height, 0.0f);
    glVertex3f(-p.width / sqrt(2), 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, p.width / sqrt(2));
    glEnd();

    glBegin(GL_QUADS);
    // Bottom
    glVertex3f(p.width / sqrt(2), 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, p.width / sqrt(2));
    glVertex3f(0.0f, 0.0f, -p.width / sqrt(2));
    glVertex3f(-p.width / sqrt(2), 0.0f, 0.0f);
    glEnd(); 

}

void drawCube(struct cube c)
{
    glBegin(GL_QUADS); // Begin drawing the color cube with 6 quads
    // Top face
    // Define vertices in counter-clockwise (CCW) order with normal pointing out
    glVertex3f(c.side, c.side, -c.side);
    glVertex3f(0.0f, c.side, -c.side);
    glVertex3f(0.0f, c.side, 0.0f);
    glVertex3f(c.side, c.side, 0.0f);

    // Bottom face
    glVertex3f(c.side, 0.0f, -c.side);
    glVertex3f(0.0f, 0.0f, -c.side);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(c.side, 0.0f, 0.0f);

    // Front face
    glVertex3f(c.side, c.side, 0.0f);
    glVertex3f(0.0f, c.side, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(c.side, 0.0f, 0.0f);

    // Back face
    glVertex3f(c.side, 0.0f, -c.side);
    glVertex3f(0.0f, 0.0f, -c.side);
    glVertex3f(0.0f, c.side, -c.side);
    glVertex3f(c.side, c.side, -c.side);

    // Left face
    glVertex3f(0.0f, c.side, -c.side);
    glVertex3f(0.0f, c.side, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -c.side);

    // Right face
    glVertex3f(c.side, c.side, -c.side);
    glVertex3f(c.side, c.side, 0.0f);
    glVertex3f(c.side, 0.0f, 0.0f);
    glVertex3f(c.side, 0.0f, -c.side);
    glEnd(); // End of drawing color-cube
}

void drawSphere(struct sphere s)
{
    glutSolidSphere(s.r, 50, 50);
}

void drawGrid()
{
    int i;
    if (drawgrid == 1)
    {
        glColor3f(0.6, 0.6, 0.6); // grey
        glBegin(GL_LINES);
        {
            for (i = -8; i <= 8; i++)
            {

                if (i == 0)
                    continue; // SKIP the MAIN axes

                // lines parallel to Y-axis
                glVertex3f(i * 10, -90, 0);
                glVertex3f(i * 10, 90, 0);

                // lines parallel to X-axis
                glVertex3f(-90, i * 10, 0);
                glVertex3f(90, i * 10, 0);
            }
        }
        glEnd();
    }
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

    glRotated(angle, 0, 1, 0);

    for (int i = 0; i < cubes.size(); i++)
    {
        struct cube c = cubes[i];
        glPushMatrix();
        glTranslated(c.bottom_lower_left.x, c.bottom_lower_left.y, c.bottom_lower_left.z);
        glColor3f(c.color.x, c.color.y, c.color.z);
        drawCube(c);
        glPopMatrix();
    }

    for (int i = 0; i < spheres.size(); i++)
    {
        struct sphere s = spheres[i];
        glPushMatrix();
        glTranslated(s.center.x, s.center.y, s.center.z);
        glColor3f(s.color.x, s.color.y, s.color.z);
        drawSphere(s);
        glPopMatrix();
    }

    for (int i = 0; i < pyramids.size(); i++)
    {
        struct pyramid p = pyramids[i];
        glPushMatrix();
        glTranslated(p.lowest_point.x, p.lowest_point.y, p.lowest_point.z);
        glColor3f(p.color.x, p.color.y, p.color.z);
        drawPyramid(p);
        glPopMatrix();
    }

    glutSwapBuffers(); // Render now
}

void capture()
{
    cout << "Capturing Image" << endl;
    image.setwidth_height(pixel_size,pixel_size);

    // initialize bitmap image and set background color to black
    for (int i = 0; i < pixel_size; i++)
        for (int j = 0; j < pixel_size; j++)
            image.set_pixel(i, j, 0, 0, 0);

    // image.save_image("black.bmp");

    double planeDistance = (windowHeight / 2.0) / tan((M_PI * fov/2) / (360.0));

    point topLeft = pos + (l * planeDistance) + (u * (windowHeight / 2.0)) - (r * (windowWidth / 2.0));

    double du = windowWidth / (pixel_size * 1.0);
    double dv = windowHeight / (pixel_size * 1.0);

    // Choose middle of the grid cell
    topLeft = topLeft + (r * du / 2.0) - (u * dv / 2.0);

    int nearestObjectIndex = -1;
    double t, tMin;

    for (int i = 0; i < pixel_size; i++)
    {
        for (int j = 0; j < pixel_size; j++)
        {
            // calculate current pixel
            point pixel = topLeft + (r * du * i) - (u * dv * j);

            // cast ray from eye to pixel
            Ray ray(pos, pixel - pos);
            // Color color;
            double r = 0, g = 0, b = 0;

            // cout<<"Ray direction "<<ray.dir<<endl;

            // find nearest object
            tMin = -1;
            nearestObjectIndex = -1;
            for (int k = 0; k < no_objects; k++)
            {
                // t = objects[k]->intersect(ray, color, 0);
                if (t > 0 && (nearestObjectIndex == -1 || t < tMin))
                    tMin = t, nearestObjectIndex = k;
            }

            // if nearest object is found, then shade the pixel
            if (nearestObjectIndex != -1)
            {
                // cout<<"Object "<<nearestObjectIndex<<" intersected"<<endl;
                // color = objects[nearestObjectIndex]->color;
                // cout<<"Before Color "<<r<<" "<<g<<" "<<b<<endl;
                // double t = objects[nearestObjectIndex]->intersect(ray, color, 1);

                if (r > 1)
                    r = 1;
                if (g > 1)
                    g = 1;
                if (b > 1)
                    b = 1;

                if (r < 0)
                    r = 0;
                if (g < 0)
                    g = 0;
                if (b < 0)
                    b = 0;

                // cout<<"After Color "<<r<<" "<<g<<" "<<b<<endl;
                image.set_pixel(i, j, 255 * r, 255 * g, 255 * b);
            }
        }
    }

    image.save_image("Output_1" + to_string(imageCount) + ".bmp");
    imageCount++;
    cout << "Saving Image" << endl;
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

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y)
{
    double rate = 0.01;
    double v = 0.25;
    double s;
    switch (key)
    {
    // Rotation
    case '1':
        // look left
        r.x = r.x * cos(rate) + l.x * sin(rate);
        r.y = r.y * cos(rate) + l.y * sin(rate);
        r.z = r.z * cos(rate) + l.z * sin(rate);

        l.x = l.x * cos(rate) - r.x * sin(rate);
        l.y = l.y * cos(rate) - r.y * sin(rate);
        l.z = l.z * cos(rate) - r.z * sin(rate);
        break;
    case '2':
        // look right
        r.x = r.x * cos(-rate) + l.x * sin(-rate);
        r.y = r.y * cos(-rate) + l.y * sin(-rate);
        r.z = r.z * cos(-rate) + l.z * sin(-rate);

        l.x = l.x * cos(-rate) - r.x * sin(-rate);
        l.y = l.y * cos(-rate) - r.y * sin(-rate);
        l.z = l.z * cos(-rate) - r.z * sin(-rate);
        break;
    case '3':
        // look up
        l.x = l.x * cos(rate) + u.x * sin(rate);
        l.y = l.y * cos(rate) + u.y * sin(rate);
        l.z = l.z * cos(rate) + u.z * sin(rate);

        u.x = u.x * cos(rate) - l.x * sin(rate);
        u.y = u.y * cos(rate) - l.y * sin(rate);
        u.z = u.z * cos(rate) - l.z * sin(rate);
        break;
    case '4':
        // look down
        l.x = l.x * cos(-rate) + u.x * sin(-rate);
        l.y = l.y * cos(-rate) + u.y * sin(-rate);
        l.z = l.z * cos(-rate) + u.z * sin(-rate);

        u.x = u.x * cos(-rate) - l.x * sin(-rate);
        u.y = u.y * cos(-rate) - l.y * sin(-rate);
        u.z = u.z * cos(-rate) - l.z * sin(-rate);
        break;
    case '5':
        // tilt counterclockwise
        u.x = u.x * cos(rate) + r.x * sin(rate);
        u.y = u.y * cos(rate) + r.y * sin(rate);
        u.z = u.z * cos(rate) + r.z * sin(rate);

        r.x = r.x * cos(rate) - u.x * sin(rate);
        r.y = r.y * cos(rate) - u.y * sin(rate);
        r.z = r.z * cos(rate) - u.z * sin(rate);
        break;
    case '6':
        // tilt clockwise
        u.x = u.x * cos(-rate) + r.x * sin(-rate);
        u.y = u.y * cos(-rate) + r.y * sin(-rate);
        u.z = u.z * cos(-rate) + r.z * sin(-rate);

        r.x = r.x * cos(-rate) - u.x * sin(-rate);
        r.y = r.y * cos(-rate) - u.y * sin(-rate);
        r.z = r.z * cos(-rate) - u.z * sin(-rate);
        break;
    case 'a':
        // rotate obj clockwise
        angle -= 5;
        break;
    case 'd':
        // rotate obj counterclockwise
        angle += 5;
        break;
    case '.':
        drawgrid = 1 - drawgrid;
        break;
    case '0':
        // Output a bmp image using ray tracing

    // Control exit
    case 27:     // ESC key
        exit(0); // Exit window
        break;
    }
    glutPostRedisplay(); // Post a paint request to activate display()
}

/* Callback handler for special-key event */
void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    // Translation
    case GLUT_KEY_LEFT:
        pos.x -= r.x;
        pos.y -= r.y;
        pos.z -= r.z;
        break;
    case GLUT_KEY_RIGHT:
        pos.x += r.x;
        pos.y += r.y;
        pos.z += r.z;
        break;
    case GLUT_KEY_UP:
        pos.x += l.x;
        pos.y += l.y;
        pos.z += l.z;
        break;
    case GLUT_KEY_DOWN:
        pos.x -= l.x;
        pos.y -= l.y;
        pos.z -= l.z;
        break;
    case GLUT_KEY_PAGE_UP:
        pos.x += u.x;
        pos.y += u.y;
        pos.z += u.z;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x -= u.x;
        pos.y -= u.y;
        pos.z -= u.z;
        break;
    default:
        return;
    }
    glutPostRedisplay(); // Post a paint request to activate display()
}

void insert_cube_planes(cube c) {
    point p1(c.side, c.side, -c.side);
    p1 = p1+c.bottom_lower_left;
    point p2(0.0f, c.side, -c.side);
    p2 = p2+c.bottom_lower_left;
    point p3(0.0f, c.side, 0.0f);
    p3 = p3+c.bottom_lower_left;
    point p4(c.side, c.side, 0.0f);
    p4 = p4+c.bottom_lower_left;

    square s1(p1, p2, p3, p4, c.color, c.shine, c.ka, c.kd, c.ks, c.kr);

    // Bottom face
    point p5(c.side, 0.0f, -c.side);
    p5 = p5+c.bottom_lower_left;
    point p6(0.0f, 0.0f, -c.side);
    p6 = p6+c.bottom_lower_left;
    point p7(0.0f, 0.0f, 0.0f);
    p7 = p7+c.bottom_lower_left;
    point p8(c.side, 0.0f, 0.0f);
    p8 = p8+c.bottom_lower_left;     

    square s2(p5, p6, p7, p8, c.color, c.shine, c.ka, c.kd, c.ks, c.kr);

    // Front face
    square s3(p4, p3, p7, p8, c.color, c.shine, c.ka, c.kd, c.ks, c.kr);

    // Back face
    square s4(p5, p6, p2, p1, c.color, c.shine, c.ka, c.kd, c.ks, c.kr);

    // Left face
    square s5(p2, p3, p7, p6, c.color, c.shine, c.ka, c.kd, c.ks, c.kr);

    // Right face
    square s6(p1, p4, p8, p5, c.color, c.shine, c.ka, c.kd, c.ks, c.kr);
    squares.push_back(s1);
    squares.push_back(s2);
    squares.push_back(s3);
    squares.push_back(s4);
    squares.push_back(s5);
    squares.push_back(s6);
}

void insert_pyramid_planes(pyramid p) {
    // Front
    point p1(0.0f, p.height, 0.0f);
    p1 = p1+p.lowest_point;
    point p2(0.0f, 0.0f, p.width / sqrt(2));
    p2 = p2+p.lowest_point;
    point p3(p.width / sqrt(2), 0.0f, 0.0f);
    p3 = p3+p.lowest_point;

    triangle t1(p1, p2, p3, p.color, p.shine, p.ka, p.kd, p.ks, p.kr);

    // Right
    point p6(0.0f, 0.0f, -p.width / sqrt(2));
    p6 = p6+p.lowest_point;

    triangle t2(p1, p3, p6, p.color, p.shine, p.ka, p.kd, p.ks, p.kr);

    // Back
    point p9(-p.width / sqrt(2), 0.0f, 0.0f);
    p9 = p9+p.lowest_point;

    triangle t3(p1, p6, p9, p.color, p.shine, p.ka, p.kd, p.ks, p.kr);

    // Left
    triangle t4(p1, p9, p2, p.color, p.shine, p.ka, p.kd, p.ks, p.kr);

    // Bottom
    square s1(p3, p2, p9, p6, p.color, p.shine, p.ka, p.kd, p.ks, p.kr);

    triangles.push_back(t1);
    triangles.push_back(t2);
    triangles.push_back(t3);
    triangles.push_back(t4);
    squares.push_back(s1);
}

void readFile()
{
    ifstream file;
    file.open("input.txt");
    if (!file)
    {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    string line;
    getline(file, line);

    istringstream iss(line);
    string token;

    float coord[4];
    int j = 0;
    while (iss >> token)
    {
        float number = stod(token);
        coord[j] = number;
        j++;
    }
    near_plane = coord[0];
    far_plane = coord[1];
    fov = coord[2];
    aspect_ratio = coord[3];

    getline(file, line);
    recursion_level = stod(line);

    getline(file, line);
    pixel_size = stoi(line);

    getline(file, line);
    checkerboard = stod(line);

    getline(file, line);
    istringstream iss2(line);
    j = 0;
    while (iss2 >> token)
    {
        float number = stod(token);
        coord[j] = number;
        j++;
    }
    ka = coord[0];
    kd = coord[1];
    kr = coord[2];

    getline(file, line);
    no_objects = stoi(line);

    float tokens[13];

    while (getline(file, line))
    {
        if (line.compare("cube") == 0)
        {
            j = 0;
            for (int i = 0; i < 5; i++)
            {
                getline(file, line);
                istringstream iss3(line);
                while (iss3 >> token)
                {
                    float number = stod(token);
                    // cout << number << endl;
                    tokens[j] = number;
                    // cout << tokens[j] << endl;
                    j++;
                }
            }
            struct cube c;
            c.bottom_lower_left.x = tokens[0];
            // cout << c.bottom_lower_left.x << endl;
            c.bottom_lower_left.y = tokens[1];
            c.bottom_lower_left.z = tokens[2];
            c.side = tokens[3];
            c.color.x = tokens[4];
            c.color.y = tokens[5];
            c.color.z = tokens[6];
            c.ka = tokens[7];
            c.kd = tokens[8];
            c.ks = tokens[9];
            c.kr = tokens[10];
            c.shine = tokens[11];
            // cout << c.bottom_lower_left.x << " " << c.bottom_lower_left.y << " " << c.bottom_lower_left.z << " " << c.side << " " << c.color.x << " " << c.color.y << " " << c.color.z << " " << c.ka << " " << c.kd << " " << c.ks << " " << c.kr << " " << c.shine << endl;
            cubes.push_back(c);
            objects.push_back(&c);
            insert_cube_planes(c);
        }
        else if (line.compare("sphere") == 0)
        {
            j = 0;
            for (int i = 0; i < 5; i++)
            {
                getline(file, line);
                istringstream iss4(line);
                while (iss4 >> token)
                {
                    float number = stod(token);
                    tokens[j] = number;
                    j++;
                }
            }
            struct sphere s;
            s.center.x = tokens[0];
            s.center.y = tokens[1];
            s.center.z = tokens[2];
            s.r = tokens[3];
            s.color.x = tokens[4];
            s.color.y = tokens[5];
            s.color.z = tokens[6];
            s.ka = tokens[7];
            s.kd = tokens[8];
            s.ks = tokens[9];
            s.kr = tokens[10];
            s.shine = tokens[11];
            spheres.push_back(s);
            objects.push_back(&s);
        }
        else if (line.compare("pyramid") == 0)
        {
            j = 0;
            for (int i = 0; i < 5; i++)
            {
                getline(file, line);
                istringstream iss5(line);
                while (iss5 >> token)
                {
                    float number = stod(token);
                    tokens[j] = number;
                    j++;
                }
            }
            struct pyramid p;
            p.lowest_point.x = tokens[0];
            p.lowest_point.y = tokens[1];
            p.lowest_point.z = tokens[2];
            p.width = tokens[3];
            p.height = tokens[4];
            p.color.x = tokens[5];
            p.color.y = tokens[6];
            p.color.z = tokens[7];
            p.ka = tokens[8];
            p.kd = tokens[9];
            p.ks = tokens[10];
            p.kr = tokens[11];
            p.shine = tokens[12];
            // cout << p.lowest_point.x << " " << p.lowest_point.y << " " << p.lowest_point.z << " " << p.width << " " << p.height << " " << p.color.x << " " << p.color.y << " " << p.color.z << " " << p.ka << " " << p.kd << " " << p.ks << " " << p.kr << " " << p.shine << endl;
            pyramids.push_back(p);
            objects.push_back(&p);
            insert_pyramid_planes(p);
        }
        else
        {
            j = 0;
            normal_light = stoi(line);
            for (int i = 0; i < normal_light; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    getline(file, line);
                    istringstream iss6(line);
                    while (iss6 >> token)
                    {
                        float number = stod(token);
                        tokens[j] = number;
                        j++;
                    }
                }
                struct normallight nl;
                nl.position.x = coord[0];
                nl.position.y = coord[1];
                nl.position.z = coord[2];
                nl.color.x = coord[3];
                nl.color.y = coord[4];
                nl.color.z = coord[5];
                nl.falloff = coord[6];
                normal_lights.push_back(nl);
            }
            break;
        }
    }
    getline(file, line);
    spot_light = stoi(line);
    j = 0;
    for (int i = 0; i < spot_light; i++)
    {
        for (int k = 0; k < 4; k++)
        {
            getline(file, line);
            istringstream iss7(line);
            while (iss7 >> token)
            {
                float number = stod(token);
                tokens[j] = number;
                j++;
            }
        }
        struct spotlight sl;
        sl.position.x = tokens[0];
        sl.position.y = tokens[1];
        sl.position.z = tokens[2];
        sl.color.x = tokens[3];
        sl.color.y = tokens[4];
        sl.color.z = tokens[5];
        sl.falloff = tokens[6];
        sl.direction.x = tokens[7];
        sl.direction.y = tokens[8];
        sl.direction.z = tokens[9];
        sl.cutoff = tokens[10];
        spot_lights.push_back(sl);
    }
    file.close();

    for (int i = 0; i < cubes.size(); i++)
    {
        cout << cubes[i].bottom_lower_left.x << " " << cubes[i].bottom_lower_left.y << " " << cubes[i].bottom_lower_left.z << " " << cubes[i].side << " " << cubes[i].color.x << " " << cubes[i].color.y << " " << cubes[i].color.z << " " << cubes[i].ka << " " << cubes[i].kd << " " << cubes[i].ks << " " << cubes[i].kr << " " << cubes[i].shine << endl;
    }
    for (int i = 0; i < spheres.size(); i++)
    {
        cout << spheres[i].center.x << " " << spheres[i].center.y << " " << spheres[i].center.z << " " << spheres[i].r << " " << spheres[i].color.x << " " << spheres[i].color.y << " " << spheres[i].color.z << " " << spheres[i].ka << " " << spheres[i].kd << " " << spheres[i].ks << " " << spheres[i].kr << " " << spheres[i].shine << endl;
    }
    for (int i = 0; i < pyramids.size(); i++)
    {
        cout << pyramids[i].lowest_point.x << " " << pyramids[i].lowest_point.y << " " << pyramids[i].lowest_point.z << " " << pyramids[i].width << " " << pyramids[i].height << " " << pyramids[i].color.x << " " << pyramids[i].color.y << " " << pyramids[i].color.z << " " << pyramids[i].ka << " " << pyramids[i].kd << " " << pyramids[i].ks << " " << pyramids[i].kr << " " << pyramids[i].shine << endl;
    }
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    l.x = -pos.x;
    l.y = -pos.y;
    l.z = -pos.z;
    l.normalize();
    u.x = 0;
    u.y = 1;
    u.z = 0;
    r.x = l.x;
    r.y = -l.y;
    r.z = l.z;

    readFile();

    glutInit(&argc, argv);                                    // Initialize GLUT
    glutInitWindowSize(640, 640);                             // Set the window's initial width & height
    glutInitWindowPosition(50, 50);                           // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
    glutCreateWindow("OpenGL 3D Drawing");                    // Create a window with the given title
    glutDisplayFunc(display);                                 // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);                         // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);                       // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener);                      // Register callback handler for special-key event
    initGL();                                                 // Our own OpenGL initialization
    glutMainLoop();                                           // Enter the event-processing loop
    return 0;
}

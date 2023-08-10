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

struct triangle
{
    point p1, p2, p3;
};

vector<point> points;
vector<triangle> triangles;

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

void readFile()
{
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
        if (points[i].x > max_x)
        {
            max_x = points[i].x;
        }
        if (points[i].y > max_y)
        {
            max_y = points[i].y;
        }
        if (points[i].x < min_x)
        {
            min_x = points[i].x;
        }
        if (points[i].y < min_y)
        {
            min_y = points[i].y;
        }
    }
    // cout << "max_x: " << max_x << " max_y: " << max_y << " min_x: " << min_x << " min_y: " << min_y << endl;
    cout << "width: " << width << " height: " << height << endl;
    dx = (max_x - min_x) / width;
    dy = (max_y - min_y) / height;
    top_y = max_y - dy / 2;
    left_x = min_x + dx / 2;
    cout << "dx: " << dx << " dy: " << dy << endl;
    cout << "top_y: " << top_y << " left_x: " << left_x << endl;
}

void initialize_z_buffer()
{
    z_buffer = new float *[(int)width];

    for (int i = 0; i < width; i++)
    {
        z_buffer[i] = new float[(int)height];
    }
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            z_buffer[i][j] = z_max;
        }
    }
    image.setwidth_height(width, height);
    image.set_all_channels(0, 0, 0);
}

void getTriangles()
{
    for (int i = 0; i < points.size(); i = i + 3)
    {
        triangle t = {points[i], points[i + 1], points[i + 2]};
        triangles.push_back(t);
    }
}

bool isInside(triangle t, point p)
{
    float area = 0.5 * (-t.p2.y * t.p3.x + t.p1.y * (-t.p2.x + t.p3.x) + t.p1.x * (t.p2.y - t.p3.y) + t.p2.x * t.p3.y);
    float s = 1 / (2 * area) * (t.p1.y * t.p3.x - t.p1.x * t.p3.y + (t.p3.y - t.p1.y) * p.x + (t.p1.x - t.p3.x) * p.y);
    float t1 = 1 / (2 * area) * (t.p1.x * t.p2.y - t.p1.y * t.p2.x + (t.p1.y - t.p2.y) * p.x + (t.p2.x - t.p1.x) * p.y);
    float u = 1 - s - t1;
    if (s >= 0 && s <= 1 && t1 >= 0 && t1 <= 1 && u >= 0 && u <= 1)
    {
        return true;
    }
    return false;
}

float getZ(triangle t, point p)
{
    float area = 0.5 * (-t.p2.y * t.p3.x + t.p1.y * (-t.p2.x + t.p3.x) + t.p1.x * (t.p2.y - t.p3.y) + t.p2.x * t.p3.y);
    float s = 1 / (2 * area) * (t.p1.y * t.p3.x - t.p1.x * t.p3.y + (t.p3.y - t.p1.y) * p.x + (t.p1.x - t.p3.x) * p.y);
    float t1 = 1 / (2 * area) * (t.p1.x * t.p2.y - t.p1.y * t.p2.x + (t.p1.y - t.p2.y) * p.x + (t.p2.x - t.p1.x) * p.y);
    float u = 1 - s - t1;
    return s * t.p1.z + t1 * t.p2.z + u * t.p3.z;
}

bool x_inRange(float x, float min_x, float max_x)
{
    return (x >= min_x && x <= max_x);
}

float *getLeftRightScanline(triangle t, float top_scanline, float min_x, float max_x)
{
    float intersection_x1, intersection_x2, intersection_x3, left_scanline, right_scanline;
    intersection_x1 = (top_scanline - t.p1.y) * (t.p2.x - t.p1.x) / (t.p2.y - t.p1.y) + t.p1.x;
    intersection_x2 = (top_scanline - t.p2.y) * (t.p3.x - t.p2.x) / (t.p3.y - t.p2.y) + t.p2.x;
    intersection_x3 = (top_scanline - t.p3.y) * (t.p1.x - t.p3.x) / (t.p1.y - t.p3.y) + t.p3.x;
    if (!x_inRange(intersection_x1, min_x, max_x))
    {
        left_scanline = min(intersection_x2, intersection_x3);
        right_scanline = max(intersection_x2, intersection_x3);
    }
    else if (!x_inRange(intersection_x2, min_x, max_x))
    {
        left_scanline = min(intersection_x1, intersection_x3);
        right_scanline = max(intersection_x1, intersection_x3);
    }
    else
    {
        left_scanline = min(intersection_x1, intersection_x2);
        right_scanline = max(intersection_x1, intersection_x2);
    }
    float *result = new float[2];
    result[0] = left_scanline;
    result[1] = right_scanline;
    return result;
}

void startProcess()
{
    for (int i = 0; i < triangles.size(); i++)
    {
        // Clipping

        float max_x = max(triangles[i].p1.x, max(triangles[i].p2.x, triangles[i].p3.x));
        float max_y = max(triangles[i].p1.y, max(triangles[i].p2.y, triangles[i].p3.y));
        float min_x = min(triangles[i].p1.x, max(triangles[i].p2.x, triangles[i].p3.x));
        float min_y = min(triangles[i].p1.y, max(triangles[i].p2.y, triangles[i].p3.y));
        if (max_y > top_y)
            max_y = top_y;
        if (min_y < top_y - height * dy)
            min_y = top_y - height * dy;
        if (max_x > left_x + width * dx)
            max_x = left_x + width * dx;
        if (min_x < left_x)
            min_x = left_x;
        float top_scanline = max_y - dy / 2;
        float bottom_scanline = min_y + dy / 2;
        int red, green, blue;
        red = random();
        green = random();
        blue = random();
        point v1 = {triangles[i].p2.x - triangles[i].p1.x, triangles[i].p2.y - triangles[i].p1.y, triangles[i].p2.z - triangles[i].p1.z};
        point v2 = {triangles[i].p3.x - triangles[i].p1.x, triangles[i].p3.y - triangles[i].p1.y, triangles[i].p3.z - triangles[i].p1.z};
        point n = {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
        float a = n.x, b = n.y, c = n.z, d = -(a * triangles[i].p1.x + b * triangles[i].p1.y + c * triangles[i].p1.z);
        for (; top_scanline >= bottom_scanline; top_scanline -= dy)
        {
            float *left_right = getLeftRightScanline(triangles[i], top_scanline, min_x, max_x);
            float left_scanline = left_right[0];
            float right_scanline = left_right[1];
            delete[] left_right;

            for (float x = left_scanline; x <= right_scanline; x += dx)
            {
                float distance = (a * x + b * top_scanline + d) / sqrt(a * a + b * b + c * c);
                int z_x = (x - left_x) / dx;
                int z_y = (top_y - top_scanline) / dy;
                if (z_x < 0 || z_x >= width || z_y < 0 || z_y >= height)
                    continue;
                // cout << "z_x: " << z_x << " " << (top_y - top_scanline) / dy << " z_y: " << z_y << " " << (top_y - top_scanline) / dy << endl;
                if (distance < z_buffer[z_x][z_y])
                {
                    z_buffer[z_x][z_y] = distance;
                    image.set_pixel(z_x, z_y, red, green, blue);
                }
            }
            // cout << "Out of inner loop" << endl;
        }
        // cout << "Out of outer loop" << endl;
    }
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    readFile();
    initialize_z_buffer();

    getTriangles();

    startProcess();
    cout << "Process ended" << endl;

    image.save_image("out.bmp");

    image.clear();

    ofstream z_buffer_file;
    z_buffer_file.open("z_buffer.txt");
    if (!z_buffer_file)
    {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
            z_buffer_file << z_buffer[i][j] << " ";
        z_buffer_file << endl;
    }
    z_buffer_file.close();

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
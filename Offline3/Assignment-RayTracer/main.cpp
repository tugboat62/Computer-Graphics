#define _USE_MATH_DEFINES

#include <iostream>
#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "bitmap_image.hpp"

std::vector<float> vertices_for_circle;

struct point
{
    float x, y, z;
};

struct sphere
{
    point center;
    float r, kd, ks, ka, kr, shine;
    point color;
};

struct pyramid
{
    point lowest_point;
    float width, height, shine, kd, ks, ka, kr;
    point color;
};

struct cube
{
    point bottom_lower_left;
    float side, shine, kd, ks, ka, kr;
    point color;
};

struct normallight
{
    point position;
    point color;
    float falloff;
};

struct spotlight
{
    point position;
    point color;
    float falloff;
    point direction;
    float cutoff;
};

std::vector<sphere> spheres;
std::vector<pyramid> pyramids;
std::vector<cube> cubes;
std::vector<normallight> normal_lights;
std::vector<spotlight> spot_lights;

struct point pos; // position of the eye
struct point l;   // look/forward direction
struct point r;   // right direction
struct point u;   // up direction

GLfloat near_plane;
GLfloat far_plane;
GLfloat fov, aspect_ratio, recursion_level, checkerboard;
GLfloat ka, kd, kr;
int pixel_size, no_objects, normal_light, spot_light;

std::vector<float> buildUnitPositiveX(int subdivision)
{
    const float DEG2RAD = acos(-1) / 180.0f;

    std::vector<float> vertices;
    float n1[3]; // normal of longitudinal plane rotating along Y-axis
    float n2[3]; // normal of latitudinal plane rotating along Z-axis
    float v[3];  // direction vector intersecting 2 planes, n1 x n2
    float a1;    // longitudinal angle along Y-axis
    float a2;    // latitudinal angle along Z-axis

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
    for (unsigned int i = 0; i < pointsPerRow; ++i)
    {
        // normal for latitudinal plane
        // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
        // therefore, it is rotating (0,1,0) vector by latitude angle a2
        a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;

        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
        for (unsigned int j = 0; j < pointsPerRow; ++j)
        {
            // normal for longitudinal plane
            // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
            // therefore, it is rotating (0,0,-1) vector by longitude angle a1
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            float scale = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            // add a vertex into array
            vertices.push_back(v[0]);
            vertices.push_back(v[1]);
            vertices.push_back(v[2]);
        }
    }

    return vertices;
}

void drawCircle()
{
    int r = (int)sqrt(vertices_for_circle.size() / 3);
    int c = r * 3;
    float mat[r][c];
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < c; j += 1)
        {
            mat[i][j] = vertices_for_circle[i * c + j];
        }
    }
    for (int i = 0; i < r - 1; i++)
    {
        for (int k = 0; k + 3 < c; k += 3)
        {
            glBegin(GL_QUADS);
            glVertex3f(mat[i][k], mat[i][k + 1], mat[i][k + 2]);
            glVertex3f(mat[i][k + 3], mat[i][k + 4], mat[i][k + 5]);
            glVertex3f(mat[i + 1][k + 3], mat[i + 1][k + 4], mat[i + 1][k + 5]);
            glVertex3f(mat[i + 1][k], mat[i + 1][k + 1], mat[i + 1][k + 2]);
            glEnd();
        }
    }
}

int subdivision = 5;

/* Initialize OpenGL Graphics */
void initGL()
{
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
    vertices_for_circle = buildUnitPositiveX(subdivision);
}

// Global variables
int trans_rate = 16;
int sc_up_down = trans_rate;
double scaling_triangle = sqrt(2.0 / 3) / (double)(trans_rate);

GLfloat t_side = 1.0; // Triangle side length
float angle = 0.0;    // Rotation angle for animation
bool rotate = false;  // Rotate triangle?

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
    glVertex3d(dec, 0, 0);           // Bottom-left
    glVertex3d(0, up_down * dec, 0); // Top
    glVertex3d(0, 0, dec);           // Bottom-right
    glEnd();
}

void drawRectangle(int up_down, GLdouble dec)
{
    glBegin(GL_QUADS);
    glVertex3d(dec, 0, 0);
    glVertex3d(0, 0, dec);
    glVertex3d(-dec, 0, 0);
    glVertex3d(0, 0, -dec);
    glEnd();
}

void drawCylinder(GLdouble r, GLdouble h)
{
    double phi = M_PI - acos(-1 / 3);
    for (double theta = (M_PI - phi) / 2.0; theta < (M_PI + phi) / 2.0; theta += 0.001)
    {
        GLdouble x1 = r * cos(theta);
        GLdouble y1 = r * sin(theta);

        GLdouble x2 = r * cos(theta - 0.5 * M_PI / 180);
        GLdouble y2 = r * sin(theta - 0.5 * M_PI / 180);
        glBegin(GL_QUADS);
        glVertex3d(x1, y1, -h / 2.0);
        glVertex3d(x1, y1, h / 2.0);
        glVertex3d(x2, y2, h / 2.0);
        glVertex3d(x2, y2, -h / 2.0);
        glEnd();
    }
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
    glEnd(); // Done drawing the pyramid
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
    case ',':
        if (sc_up_down > 0)
            sc_up_down--;
        break;
    case '.':
        if (sc_up_down < trans_rate)
            sc_up_down++;
        break;
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

void readFile()
{
    std::ifstream file;
    file.open("input.txt");
    if (!file)
    {
        std::cout << "Unable to open file";
        exit(1); // terminate with error
    }
    std::string line;
    std::getline(file, line);

    std::istringstream iss(line);
    std::string token;

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

    std::getline(file, line);
    recursion_level = stod(line);

    std::getline(file, line);
    pixel_size = stoi(line);

    std::getline(file, line);
    checkerboard = stod(line);

    std::getline(file, line);
    std::istringstream iss2(line);
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

    std::getline(file, line);
    no_objects = stoi(line);

    float tokens[13];

    while (std::getline(file, line))
    {
        if (line.compare("cube") == 0)
        {
            j = 0;
            for (int i = 0; i < 5; i++)
            {
                std::getline(file, line);
                std::istringstream iss3(line);
                while (iss3 >> token)
                {
                    float number = stod(token);
                    // std::cout << number << std::endl;
                    tokens[j] = number;
                    // std::cout << tokens[j] << std::endl;
                    j++;
                }
            }
            struct cube c;
            c.bottom_lower_left.x = tokens[0];
            // std::cout << c.bottom_lower_left.x << std::endl;
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
            // std::cout << c.bottom_lower_left.x << " " << c.bottom_lower_left.y << " " << c.bottom_lower_left.z << " " << c.side << " " << c.color.x << " " << c.color.y << " " << c.color.z << " " << c.ka << " " << c.kd << " " << c.ks << " " << c.kr << " " << c.shine << std::endl;
            cubes.push_back(c);
        }
        else if (line.compare("sphere") == 0)
        {
            j = 0;
            for (int i = 0; i < 5; i++)
            {
                std::getline(file, line);
                std::istringstream iss4(line);
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
        }
        else if (line.compare("pyramid") == 0)
        {
            j = 0;
            for (int i = 0; i < 5; i++)
            {
                std::getline(file, line);
                std::istringstream iss5(line);
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
            // std::cout << p.lowest_point.x << " " << p.lowest_point.y << " " << p.lowest_point.z << " " << p.width << " " << p.height << " " << p.color.x << " " << p.color.y << " " << p.color.z << " " << p.ka << " " << p.kd << " " << p.ks << " " << p.kr << " " << p.shine << endl;
            pyramids.push_back(p);
        }
        else
        {
            j = 0;
            normal_light = stoi(line);
            for (int i = 0; i < normal_light; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    std::getline(file, line);
                    std::istringstream iss6(line);
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
    std::getline(file, line);
    spot_light = stoi(line);
    j = 0;
    for (int i = 0; i < spot_light; i++)
    {
        for (int k = 0; k < 4; k++)
        {
            std::getline(file, line);
            std::istringstream iss7(line);
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
        std::cout << cubes[i].bottom_lower_left.x << " " << cubes[i].bottom_lower_left.y << " " << cubes[i].bottom_lower_left.z << " " << cubes[i].side << " " << cubes[i].color.x << " " << cubes[i].color.y << " " << cubes[i].color.z << " " << cubes[i].ka << " " << cubes[i].kd << " " << cubes[i].ks << " " << cubes[i].kr << " " << cubes[i].shine << std::endl;
    }
    for (int i = 0; i < spheres.size(); i++)
    {
        std::cout << spheres[i].center.x << " " << spheres[i].center.y << " " << spheres[i].center.z << " " << spheres[i].r << " " << spheres[i].color.x << " " << spheres[i].color.y << " " << spheres[i].color.z << " " << spheres[i].ka << " " << spheres[i].kd << " " << spheres[i].ks << " " << spheres[i].kr << " " << spheres[i].shine << std::endl;
    }
    for (int i = 0; i < pyramids.size(); i++)
    {
        std::cout << pyramids[i].lowest_point.x << " " << pyramids[i].lowest_point.y << " " << pyramids[i].lowest_point.z << " " << pyramids[i].width << " " << pyramids[i].height << " " << pyramids[i].color.x << " " << pyramids[i].color.y << " " << pyramids[i].color.z << " " << pyramids[i].ka << " " << pyramids[i].kd << " " << pyramids[i].ks << " " << pyramids[i].kr << " " << pyramids[i].shine << std::endl;
    }
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    pos.x = 70;
    pos.y = 70;
    pos.z = 70;

    l.x = -.25;
    l.y = -.25;
    l.z = -.25;
    u.x = 0;
    u.y = 1;
    u.z = 0;
    r.x = .25;
    r.y = 0;
    r.z = -.25;

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

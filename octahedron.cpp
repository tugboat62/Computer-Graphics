#define _USE_MATH_DEFINES

#include <iostream>
#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>
#include "Cubesphere.h"
#include <vector>

std::vector<float> vertices_for_circle;

struct point {
    GLfloat x, y, z;
};

struct point pos;   // position of the eye
struct point l;     // look/forward direction
struct point r;     // right direction
struct point u;     // up direction

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

int subdivision = 3;

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

void drawOctahedron()
{
    double a = scaling_triangle * (double)(trans_rate - sc_up_down);
    GLdouble h = sqrt(2) - 2*scaling_triangle * (16 - sc_up_down)*cos(M_PI/6.0);
    double phi = (M_PI - acos(-1 / 3)) / 2.0;
    double new_center = scaling_triangle * (double)(trans_rate - sc_up_down) / (tan(phi)*sqrt(3));
    GLdouble r = scaling_triangle*(double)(trans_rate-sc_up_down) / tan(phi);
    double cen = (double)(trans_rate-sc_up_down)/16;

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        if (!(i % 2))
            glColor3f(1.0f, 0.5f, 0.8f);
        else
            glColor3f(0.0f, 1.0f, 1.0f);
        glRotated(i * 90, 0, 1, 0);
        glPushMatrix();
            glTranslated(new_center, new_center, new_center);
            drawTriangle(1, h / sqrt(2));
        glPopMatrix();

        if (!(i % 2))
            glColor3f(0.0f, 1.0f, 1.0f);
        else
            glColor3f(1.0f, 0.5f, 0.8f);

        glPushMatrix();
            glTranslated(new_center, -new_center, new_center);
            drawTriangle(-1, h / sqrt(2));
        glPopMatrix();
        

        glColor3f(1.0f, 1.0f, 0.0f);
        glPushMatrix();
            glTranslated(0, (1.0 - cen)/2, (1.0 - cen)/2);
            glRotated(45, 1, 0, 0);
            drawCylinder(r, h);
        glPopMatrix();
    
        glPushMatrix();
            glTranslated(0, -(1.0 - cen)/2, (1.0 - cen)/2);
            glRotated(135, 1, 0, 0);
            drawCylinder(r, h);
        glPopMatrix();

        glPushMatrix();
            glTranslated((1.0 - cen)/2, 0, (1.0 - cen)/2);
            glRotated(-45, 0, 1, 0);
            glRotated(-90, 0, 0, 1);
            drawCylinder(r, h);
        glPopMatrix();

        if (!(i % 2))
            glColor3f(0.0f, 1.0f, 0.0f);
        else
            glColor3f(0.0f, 0.0f, 1.0f);

        glPushMatrix();
            glTranslated(1.0 - cen, 0, 0);
            glScaled(r, r, r);
            drawCircle();
        glPopMatrix();
        glPopMatrix();
    }
    
    glColor3d(1, 0, 0);
    glPushMatrix();
        glRotated(90, 0, 0, 1);
        glTranslated(1.0 - cen, 0, 0);
        glScaled(r, r, r);
        drawCircle();
    glPopMatrix();

    glPushMatrix();
        glRotated(-90, 0, 0, 1);
        glTranslated(1.0 - cen, 0, 0);
        glScaled(r, r, r);
        drawCircle();
    glPopMatrix();
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
    gluLookAt(pos.x,pos.y,pos.z,
              pos.x+l.x,pos.y+l.y,pos.z+l.z,
              u.x,u.y,u.z);


    drawOctahedron();
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
    double v = 0.25;
    double s;
    double theta1 = atan(l.x / l.z);

    double r1 = sqrt(l.x * l.x + l.z * l.z);
    double theta2 = 1*M_PI/180;
    double theta = theta1 + theta2;
    double theta3 = atan2f(l.x, l.z);
    double r2 = sqrt(l.x * l.x + l.z * l.z);
    double theta4 = atanf(v / r2);
    int p = (int)(theta/90) % 4;
    switch (key)
    {
    // Rotation
    case '1':
        // look left
        l.x = -r1 * sin(theta1 + theta2);
        l.z = -r1 * cos(theta1 + theta2);
        break;
    case '2':
        // look right
        l.x = -r1 * sin(theta1 - theta2);
        l.z = -r1 * cos(theta1 - theta2);
        break;
    case '3':
        // look up
        
        break;
    case '4':
        pos.y -= v;
        break;
    case '5':
        pos.z += v;
        break;
    case '6':
        pos.z -= v;
        break;
    case ',':
        if (sc_up_down > 0)
            sc_up_down--;
        // std::cout << sc_up_down << std::endl;
        break;
    case '.':
        if (sc_up_down < trans_rate)
            sc_up_down++;
        // std::cout << sc_up_down << std::endl;
        break;
    // Control exit
    case 27:     // ESC key
        exit(0); // Exit window
        break;
    }
    glutPostRedisplay(); // Post a paint request to activate display()
}

void mat_mul(double a[3][3], double b[3][1], double c[3][1])
{
    int i, j, k;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 1; j++)
        {
            c[i][j] = 0;
            for (k = 0; k < 3; k++)
                c[i][j] += a[i][k] * b[k][j];
        }
}

/* Callback handler for special-key event */
void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    // Translation
    case GLUT_KEY_LEFT:
        pos.x-=r.x;
        pos.y-=r.y;
        pos.z-=r.z;
        break;
    case GLUT_KEY_RIGHT:
        pos.x+=r.x;
        pos.y+=r.y;
        pos.z+=r.z;
        break;
    case GLUT_KEY_UP:		//down arrow key
        pos.x+=l.x;
        pos.y+=l.y;
        pos.z+=l.z;
        break;
    case GLUT_KEY_DOWN:		// up arrow key
        pos.x-=l.x;
        pos.y-=l.y;
        pos.z-=l.z;
        break;
    case GLUT_KEY_PAGE_UP:
        pos.x+=u.x;
        pos.y+=u.y;
        pos.z+=u.z;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x-=u.x;
        pos.y-=u.y;
        pos.z-=u.z;
        break;
    default:
        return;
    }
    glutPostRedisplay(); // Post a paint request to activate display()
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    pos.x=3;pos.y=3;pos.z=3;

    l.x=-.25;l.y=-.25;l.z=-.25;
    u.x=0;u.y=1;u.z=0;
    r.x=.25;r.y=0;r.z=-.25;

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

#define _USE_MATH_DEFINES

#include <iostream>
#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>
#include "Cubesphere.h"
#include <vector>

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black and opaque
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
}

// Global variables
GLfloat eyex = 4, eyey = 4, eyez = 4;
GLfloat centerx = 0, centery = 0, centerz = 0;  // look = center - eye
GLfloat upx = 0, upy = 1, upz = 0;  // up . look != 0, direction -> up != look
int trans_rate = 16;
int sc_up_down = trans_rate;
double scaling_circle = 1.0/(trans_rate*sqrt(3));
double scaling_cylinder = 1.0/(trans_rate*sqrt(2));
double scaling_triangle = sqrt(2.0/3)/(trans_rate);

GLfloat t_side = 1.0;  // Triangle side length

void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
        glColor3f(1,0,0);   // Red
        // X axis
        glVertex3f(0,0,0);
        glVertex3f(2,0,0);

        glColor3f(0,1,0);   // Green
        // Y axis
        glVertex3f(0,0,0);
        glVertex3f(0,2,0);

        glColor3f(0,0,1);   // Blue
        // Z axis
        glVertex3f(0,0,0);
        glVertex3f(0,0,2);
    glEnd();
}

// std::vector<float> buildUnitPositiveX(int subdivision)
// {
//     const float DEG2RAD = acos(-1) / 180.0f;

//     std::vector<float> vertices;
//     float n1[3];        // normal of longitudinal plane rotating along Y-axis
//     float n2[3];        // normal of latitudinal plane rotating along Z-axis
//     float v[3];         // direction vector intersecting 2 planes, n1 x n2
//     float a1;           // longitudinal angle along Y-axis
//     float a2;           // latitudinal angle along Z-axis

//     // compute the number of vertices per row, 2^n + 1
//     int pointsPerRow = (int)pow(2, subdivision) + 1;

//     // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
//     for(unsigned int i = 0; i < pointsPerRow; ++i)
//     {
//         // normal for latitudinal plane
//         // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
//         // therefore, it is rotating (0,1,0) vector by latitude angle a2
//         a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
//         n2[0] = -sin(a2);
//         n2[1] = cos(a2);
//         n2[2] = 0;

//         // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
//         for(unsigned int j = 0; j < pointsPerRow; ++j)
//         {
//             // normal for longitudinal plane
//             // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
//             // therefore, it is rotating (0,0,-1) vector by longitude angle a1
//             a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
//             n1[0] = -sin(a1);
//             n1[1] = 0;
//             n1[2] = -cos(a1);

//             // find direction vector of intersected line, n1 x n2
//             v[0] = n1[1] * n2[2] - n1[2] * n2[1];
//             v[1] = n1[2] * n2[0] - n1[0] * n2[2];
//             v[2] = n1[0] * n2[1] - n1[1] * n2[0];

//             // normalize direction vector
//             float scale = 1 / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
//             v[0] *= scale;
//             v[1] *= scale;
//             v[2] *= scale;

//             // add a vertex into array
//             vertices.push_back(v[0]);
//             vertices.push_back(v[1]);
//             vertices.push_back(v[2]);
//         }
//     }

//     return vertices;
// }


void drawTriangle(int up_down, GLdouble dec) {
    glBegin(GL_TRIANGLES);
        glVertex3d(dec, 0, 0);   // Top
        glVertex3d(0, up_down*dec, 0); // Bottom-left
        glVertex3d(0, 0, dec);  // Bottom-right
    glEnd();
}

void drawCylinder(GLdouble r, GLdouble h) {
    double phi = 180-acos(-1/3);
    for (double theta = -(180-phi)/2; theta > -(180+phi)/2; theta-=0.5) {
        GLdouble x1 = r * cos(theta * M_PI / 180);
        GLdouble y1 = r * sin(theta * M_PI / 180);
        
        GLdouble x2 = r * cos((theta-0.5) * M_PI / 180);
        GLdouble y2 = r * sin((theta-0.5) * M_PI / 180);
        glBegin(GL_QUADS);
            glColor3d(1, 0, 0);
            glVertex3d(x1, y1, -h/2);
            glVertex3d(x1, y1, h/2);
            glVertex3d(x2, y2, h/2);
            glVertex3d(x2, y2, -h/2);
        glEnd();
    }
}

void drawOctahedron() {
    double new_center_x = scaling_triangle*(double)(trans_rate - sc_up_down)/sqrt(3);
    double new_center_y = scaling_triangle*(double)(trans_rate - sc_up_down)/sqrt(3);
    double new_center_z = scaling_triangle*(double)(trans_rate - sc_up_down)/sqrt(3); 
    
    for(int i=0; i<4; i++) {
        glPushMatrix();
            if (!(i%2)) glColor3f(1.0f, 0.5f, 0.8f);
            else glColor3f(0.0f, 1.0f, 1.0f);
            glRotated(i*90, 0, 1, 0);
            glPushMatrix();
                glTranslated(new_center_x, new_center_y, new_center_z);
                drawTriangle(1,(sqrt(2)-new_center_x*3.0)/sqrt(2));
            glPopMatrix();
            
            if (!(i%2)) glColor3f(0.0f, 1.0f, 1.0f);
            else glColor3f(1.0f, 0.5f, 0.8f);
            
            glPushMatrix();
                glTranslated(new_center_x, -new_center_y, new_center_z);
                drawTriangle(-1, (sqrt(2)-new_center_x*3.0)/sqrt(2));
            glPopMatrix();

            glPushMatrix();
                glColor3d(1, 0, 0);
                GLdouble a = scaling_triangle*sin(30)*(trans_rate - sc_up_down);
                GLdouble h = sqrt(2)*(sc_up_down)/16.0;
                double phi = (180-acos(-1/3))/2.0;
                GLdouble r = a/tan(phi);
                glTranslated(0, (1.0/sqrt(2) - a/cos(phi))*cos(45), (1.0/sqrt(2) - a/cos(phi))*sin(45));
                glRotated(45, 1, 0, 0);
                drawCylinder(r,h);
            glPopMatrix();
        glPopMatrix();
    }
}


void display() {
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);             // To operate on Model-View matrix
    glLoadIdentity();                       // Reset the model-view matrix

    Cubesphere sphere;
    sphere.setRadius(2.0f);
    sphere.setSubdivision(6);
    sphere.setSmooth(false);
    sphere.draw();          // draw surface only
    sphere.drawFace(0);     // draw only +X face, face index:0~5
        // default arguments of gluLookAt
    // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

    // control viewing (or camera)
    gluLookAt(eyex,eyey,eyez,
              centerx,centery,centerz,
              upx,upy,upz);
    
    drawOctahedron();
    drawAxes();

    glutSwapBuffers();  // Render now
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
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
void keyboardListener(unsigned char key, int x, int y) {
    float v = 1;
    double theta1 = atan(eyex/eyez);
    double r1 = sqrt(eyex*eyex + eyez*eyez);
    double theta2 = atan(v/r1);
    double theta3 = atan2f(centerx, centerz);
    double r2 = sqrt(centerx*centerx + centerz*centerz);
    double theta4 = atanf(v/r2);
    switch (key) {
    // Rotation
    case '1':
        eyex = r1 * sin(theta1 + theta2);
        eyez = r1 * cos(theta1 + theta2);
        centerx = r2 * sin(theta3 + theta4);
        centerz = r2 * cos(theta3 + theta4);
        break;
    case '2':
        eyex -= v;
        break;
    case '3':
        eyey += v;
        break;
    case '4':
        eyey -= v;
        break;
    case '5':
        eyez += v;
        break;
    case '6':
        eyez -= v;
        break;
    case ',':
        if(sc_up_down > 0) 
            sc_up_down--;
        // std::cout << sc_up_down << std::endl;
        break;
    case '.':
        if (sc_up_down < trans_rate) 
            sc_up_down++;
        // std::cout << sc_up_down << std::endl;
        break;
    // Control exit
    case 27:    // ESC key
        exit(0);    // Exit window
        break;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

/* Callback handler for special-key event */
void specialKeyListener(int key, int x,int y) {
    double v = 0.25;
    double lx = centerx - eyex;
    double lz = centerz - eyez;
    GLdouble ex = eyex - centerx;
    GLdouble ez = eyez - centerz;
    GLdouble ey = eyey - centery;
    GLdouble cx = 0, cy = 0, cz = 0;
    double s;
    switch (key) {
    // Translation
    case GLUT_KEY_LEFT:
        // eyex += v * (upy*lz);
        // eyez += v * (-lx*upy);
        // s = sqrt(eyex*eyex + eyez*eyez) / (4 * sqrt(2));
        // eyex /= s;
        // eyez /= s;
        ex -= v;
        ez += v;
        cx -= v;
        cz += v;
        eyex = ex + centerx;
        eyez = ez + centerz;
        centerx = cx + centerx;
        centerz = cz + centerz;
        break;
    case GLUT_KEY_RIGHT:
        // eyex += v * (-upy*lz);
        // eyez += v * (lx*upy);
        // s = sqrt(eyex*eyex + eyez*eyez) / (4 * sqrt(2));
        // eyex /= s;
        // eyez /= s;
        eyex += v;
        eyez -= v;
        centerx += v;
        centerz -= v; 
        break;
    case GLUT_KEY_UP:
        // eyey += v;
        eyez -= v;
        eyex -= v;
        eyey -= v;
        centerx -= v;
        centerz -= v;
        centery -= v;
        break;
    case GLUT_KEY_DOWN:
        // eyey -= v;
        eyez += v;
        eyex += v;
        eyey += v;
        centerx += v;
        centerz += v;
        centery += v;
        break;
    case GLUT_KEY_PAGE_UP:
        eyey += v;
        centery += v;
        break;
    case GLUT_KEY_PAGE_DOWN:
        eyey -= v;
        centery -= v;
        break;
    default:
        return;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                      // Initialize GLUT
    glutInitWindowSize(640, 640);               // Set the window's initial width & height
    glutInitWindowPosition(50, 50);             // Position the window's initial top-left corner
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color
    glutCreateWindow("OpenGL 3D Drawing");      // Create a window with the given title
    glutDisplayFunc(display);                   // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);           // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);         // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener);        // Register callback handler for special-key event
    initGL();                                   // Our own OpenGL initialization
    glutMainLoop();                             // Enter the event-processing loop
    return 0;
}

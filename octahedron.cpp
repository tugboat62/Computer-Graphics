#define _USE_MATH_DEFINES

#include <iostream>
#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>

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
double scaling_cylinder = 1.0/(trans_rate*sqrt(3));
double scaling_triangle = sqrt(2.0/3)/(trans_rate);

GLfloat t_side = 1.0;  // Triangle side length

void drawTriangle(int up_down, GLdouble dec) {
    glBegin(GL_TRIANGLES);
        glVertex3d(dec, 0, 0);   // Top
        glVertex3d(0, up_down*dec, 0); // Bottom-left
        glVertex3d(0, 0, dec);  // Bottom-right
    glEnd();
}

void drawOctahedron() {
    double new_center_x = scaling_triangle*(double)(trans_rate - sc_up_down)/sqrt(3);
    double new_center_y = scaling_triangle*(double)(trans_rate - sc_up_down)/sqrt(3);
    double new_center_z = scaling_triangle*(double)(trans_rate - sc_up_down)/sqrt(3); 
    
    for(int i=0; i<4; i++) {
        if (!(i%2)) glColor3f(1.0f, 0.5f, 0.8f);
        else glColor3f(0.0f, 1.0f, 1.0f);
        glPushMatrix();
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
        glPopMatrix();
    }
}
void display() {
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);             // To operate on Model-View matrix
    glLoadIdentity();                       // Reset the model-view matrix

    // default arguments of gluLookAt
    // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

    // control viewing (or camera)
    gluLookAt(eyex,eyey,eyez,
              centerx,centery,centerz,
              upx,upy,upz);
    
    drawOctahedron();

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

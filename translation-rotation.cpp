#include <iostream>
#include <GL/glut.h>

GLdouble sa = 0, dsa = 2;
GLdouble ta = 0, dta = 0.1;
GLdouble px = 0.0, py = 0.0;
GLdouble vx = 0.01, vy = 0.001;

void drawTriangle(){
    glColor3d(0, 1, 0);
    glBegin(GL_TRIANGLES);
        glVertex2d(-0.5, 0.5); 
        glVertex2d(0.0, 1.0);
        glVertex2d(0.5, 0.5);
    glEnd();
}

void drawSquare(){
    glColor3d(0, 0, 1);
    glBegin(GL_QUADS);
        glVertex2d(-0.5, 0.5);
        glVertex2d(0.5, 0.5);
        glVertex2d(0.5, -0.5);
        glVertex2d(-0.5, -0.5);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // Select the Modelview matrix for operation
    glLoadIdentity(); // Reset Modelview matrix
    // glPointSize(5);
    // glBegin(GL_POINTS);
    //     glColor3d(1, 0, 0);
    //     glVertex2d(px, py);
    //     glColor3d(0, 1, 0);
    //     glVertex2d(px + 0.5, py);
    //     glVertex2d(px, py + 0.5);
    // glEnd();

    // glLineWidth(10);
    // glBegin(GL_LINES);
    //     glColor3d(1, 0, 0);
    //     glVertex2d(px, py);
    //     glColor3d(0, 1, 0);
    //     glVertex2d(px + 0.5, py);
    // glEnd();

    // glBegin(GL_LINE_STRIP);
    //     glVertex2d(0,0);
    //     glVertex2d(0.5,0);
    //     glVertex2d(0,0.5);
    // glEnd();

    // glLineWidth(5);
    // glBegin(GL_LINE_LOOP);
    //     glVertex2d(0,0);
    //     glVertex2d(0.5,0);
    //     glVertex2d(0,0.5);
    // glEnd();

    // glBegin(GL_TRIANGLES);
    //     glColor3d(1, 0, 0);
    //     glVertex2d(px, py);
    //     glColor3d(0, 1, 0);
    //     glVertex2d(px + 0.5, py);
    //     glColor3d(0, 0, 1);
    //     glVertex2d(px, py + 0.5);
    // glEnd();

    // glBegin(GL_QUADS);
    //     glVertex2d(0,0);
    //     glVertex2d(0.5,0);
    //     glVertex2d(0.5,0.5);
    //     glVertex2d(0,0.5);
    // glEnd();

    // glColor3d(1, 0, 0);
    // glPointSize(30);

    // glBegin(GL_QUADS);
    //     glVertex2d(0,0);
    //     glVertex2d(0.1,0);
    //     glVertex2d(0.1,0.1);
    //     glVertex2d(0,0.1);
    // glEnd();


    // glBegin(GL_QUADS);
    //     glVertex2d(0,0);
    //     glVertex2d(0.1,0);
    //     glVertex2d(0.1,0.1);
    //     glVertex2d(0,0.1);
    // glEnd();

    glScaled(0.1, 0.1, 1);

    glPushMatrix(); // save the current matrix
        glRotated(sa, 0, 0, 1);
        drawSquare();
    glPopMatrix();  // restore the saved matrix

    glRotated(ta, 0, 0, 1);
    glTranslated(0, 5, 0);
    // glRotated(ta, 0, 0, 1);
    drawTriangle(); 
    // drawTriangle();
    // glRotated(90, 0, 0, 1);
    // drawTriangle();
    // glRotated(90, 0, 0, 1);
    // drawTriangle();
    // glRotated(90, 0, 0, 1);
    // drawTriangle();
    
    glFlush();
    
}

void idle() {
    sa += dsa;
    ta += dta;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    // if(key == 'd')
    // {
    //     px += vx;
    //     if (px >= 1.0 || px <= -1)
    //         vx = -vx;
    //     glutPostRedisplay();
    // }
    px += vx;
    if (px >= 1.0 || px <= -1)
        vx = -vx;
    glutPostRedisplay();
}

void special(int key, int x, int y) {
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        px += 0.1;
        break;
    case GLUT_KEY_LEFT:
        px -= 0.1;
        break;
    case GLUT_KEY_UP:
        py += 0.1;
        break;
    case GLUT_KEY_DOWN:
        py -= 0.1;
        break;    
    default:
        break;
    }
    glutPostRedisplay();
}

// void mouse(int button, int state, int x, int y) {
//     if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
//         cout << "Left button pressed at " << x << ", " << y << endl;
//     else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
//         cout << "Right button pressed at " << x << ", " << y << endl;
//     else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
//         cout << "Middle button pressed at " << x << ", " << y << endl;
//     glutPostRedisplay();
// }

void timerListener(int value) {
    px += vx;
    if (px >= 1.0 || px <= -1)
        vx = -vx;
    glutPostRedisplay();
    glutTimerFunc(10, timerListener, 0);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); // Select the Projection matrix for operation
    glLoadIdentity(); // Reset Projection matrix
    if(w >= h)
        gluOrtho2D(-w*1.0/h, w*1.0/h, -1, 1);
    else
        gluOrtho2D(-1, 1, -h*1.0/w, h*1.0/w);
    // glMatrixMode(GL_MODELVIEW); // Select the Model-View matrix for operation
    // glLoadIdentity(); // Reset Model-View matrix
    std::cout << "w = " << w << ", h = " << h << std::endl;
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 100);
    glutCreateWindow("OpenGL Setup Test");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    // glutKeyboardFunc(keyboard);
    // glutSpecialFunc(special);
    // glClearColor(1, 1, 1, 0);
    // glutTimerFunc(0, timerListener, 0);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}
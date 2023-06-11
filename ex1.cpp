#include <iostream>
#include <GL/glut.h>

GLdouble px = 0.0, py = 0.0;
GLdouble vx = 0.01, vy = 0.001;
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
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

    glBegin(GL_TRIANGLES);
        glColor3d(1, 0, 0);
        glVertex2d(px, py);
        glColor3d(0, 1, 0);
        glVertex2d(px + 0.5, py);
        glColor3d(0, 0, 1);
        glVertex2d(px, py + 0.5);
    glEnd();

    // glBegin(GL_QUADS);
    //     glVertex2d(0,0);
    //     glVertex2d(0.5,0);
    //     glVertex2d(0.5,0.5);
    //     glVertex2d(0,0.5);
    // glEnd();

    glFlush();
}

void idle() {
    px += vx;
    if (px >= 1.0 || px <= -1)
        vx = -vx;
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

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "Left button pressed at " << x << ", " << y << std::endl;
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        std::cout << "Right button pressed at " << x << ", " << y << std::endl;
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
        std::cout << "Middle button pressed at " << x << ", " << y << std::endl;
    glutPostRedisplay();
}

void timerListener(int value) {
    px += vx;
    if (px >= 1.0 || px <= -1)
        vx = -vx;
    glutPostRedisplay();
    glutTimerFunc(10, timerListener, 0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 100);
    glutCreateWindow("OpenGL Setup Test");
    glutDisplayFunc(display);
    // glutIdleFunc(idle);
    // glutKeyboardFunc(keyboard);
    // glutSpecialFunc(special);
    // glClearColor(1, 1, 1, 0);
    glutTimerFunc(0, timerListener, 0);
    glutMainLoop();

    return 0;
}
#define _USE_MATH_DEFINES

#include <iostream>
#include <GL/glut.h>
#include <cmath> 

GLdouble l = 0.4, bob = 0.05, r = 0.4, sr = 0.3;
GLdouble l_ind = 0.06, s_ind = 0.02;
GLdouble sec_hand = 0.2, min_hand = 0.15, hr_hand = 0.1;
double theta = -90.0, dtheta = -60.0/100;
double sa = 90, ma = 90, ha = -180;
bool sec = false;

void drawCircle(GLdouble r, GLdouble cx, GLdouble cy) {
    glBegin(GL_LINE_LOOP);  // All vertices form a single loop of single pixel width
        for (float theta = 0; theta < 360; theta += 10) {
            float x = cx + r * cos(theta/180*M_PI);
            float y = cy + r * sin(theta/180*M_PI);
            glVertex2f(x,y);
        }
    glEnd();
}

void drawSphere(GLdouble r, GLdouble cx, GLdouble cy) {
    glBegin(GL_POLYGON);    // All vertices form a single polygon
        // glColor3f(1.0f,0.5f,0.0f);  // Orange
        for (float angle = 0; angle < 360; angle += 10) {
            float x = cx + r * cos(angle/180*M_PI);
            float y = cy + r * sin(angle/180*M_PI);
            glVertex2f(x,y);
        }
    glEnd();
}

void drawPendulum() {
    drawSphere(bob, l * cos(theta * M_PI / 180), l * sin(theta * M_PI/180));
    drawSphere(.03, 0, 0);
    glLineWidth(10);
    glBegin(GL_LINES);
        glVertex2d(0, 0); // Anchor point at the top
        glVertex2d(l * cos(theta * M_PI / 180), l * sin(theta * M_PI/180));
    glEnd();
}

void drawLine(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2) {
    glBegin(GL_LINES);
        glVertex2d(x1, y1);
        glVertex2d(x2, y2);
    glEnd();
}

void drawFrame() {
    drawLine(l+bob, -(l+bob)*tan(30 * M_PI / 180), 0, -(l+bob+0.1));
    drawLine(-(l+bob), -(l+bob)*tan(30 * M_PI / 180), 0, -(l+bob+0.1));
    drawLine(l+bob, -(l+bob)*tan(30 * M_PI / 180), l+bob, 1);
    drawLine(-(l+bob), -(l+bob)*tan(30 * M_PI / 180), -(l+bob), 1);
    for (float angle=0; angle<360; angle+=30){
        glPushMatrix(); // save the current matrix
            glTranslated(0, r+bob, 0);
            glRotated(angle, 0, 0, 1);
            if ((int)angle%90 == 0){
                drawLine(0, sr, 0, sr-l_ind);
            } else {
                drawLine(0, sr, 0, sr-s_ind);
            }
        glPopMatrix();  // restore the saved matrix
    }
    glPointSize(3);
    glBegin(GL_POINTS);
        glVertex2d(0, r+bob);
    glEnd();
    drawCircle(r, 0, r+bob);
    drawCircle(sr, 0, r+bob);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // Select the Modelview matrix for operation
    glLoadIdentity(); // Reset Modelview matrix

    glLineWidth(.005);
    
    drawFrame();
    drawPendulum();

    float sx = sec_hand * cos(sa/180*M_PI);
    float sy = r+bob + sec_hand * sin(sa/180*M_PI);
    float mx = min_hand * cos(ma/180*M_PI);
    float my = r+bob + min_hand * sin(ma/180*M_PI);
    float hx = hr_hand * cos(ha/180*M_PI);
    float hy = r+bob + hr_hand * sin(ha/180*M_PI);

    //Second hand
    glLineWidth(0.5);
    drawLine(0, r+bob, sx, sy);
    // Minute hand
    glLineWidth(2);
    drawLine(0, r+bob, mx, my);
    // Hour hand
    glLineWidth(4); 
    drawLine(0, r+bob, hx, hy);

    glFlush();
}

void timerListener1(int value) {
    if (theta >= -60.0 || theta <= -120.0){
        dtheta = -dtheta;
        sa = (int)(sa-6)%360;
    }
    theta += dtheta;
    ma -= 1.0/1000;
    ha -= 1.0/60000;
    // std::cout << sa << " " << ma << " " << ha << std::endl;
    glutPostRedisplay();
    glutTimerFunc(10, timerListener1, 0);
}

void timerListener2(int value) {
    sa -= 6.0;
    glutPostRedisplay();
    glutTimerFunc(1000, timerListener2, 0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 100);
    glutCreateWindow("OpenGL Setup Test");
    glutDisplayFunc(display);
    glutTimerFunc(10, timerListener1, 0);
    glutMainLoop();

    return 0;
}
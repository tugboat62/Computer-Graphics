/*
    This code works fine in windows except for some values the output doesnot match with
    the test cases provided. Example: in test case 10.00000 and the generated output is 9.9999999.
    Differs only by .000000x with the given test cases. Also in linux the string compare method is
    not working. Have no idea why this happens:(
*/ 


#include <iostream>
#include <fstream>
#include <stack>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <string>
#include "bitmap_image.hpp"

#define _USE_MATH_DEFINES

using namespace std;

float width, height, dx, dy, left_x, top_y;
bitmap_image image;

float z_max = 2.0;

float **z_buffer;

struct Point {
    float x;
    float y;
    float z;

    // Overloading + operator
    Point operator+(const Point& other) const {
        return Point{x + other.x, y + other.y, z + other.z};
    }

    // Overloading - operator
    Point operator-(const Point& other) const {
        return Point{x - other.x, y - other.y, z - other.z};
    }

    // Overloading * operator
    Point operator*(const float& other) const {
        return Point{x * other, y * other, z * other};
    }
};

struct triangle
{
    Point p1, p2, p3;
};


vector<Point> points;
vector<triangle> triangles;

static unsigned long int g_seed = 1;
inline int random()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

void readFile(string path1, string path2)
{
    ifstream file(path1);
    ifstream inputfile(path2);
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

    while (getline(inputfile, line))
    {
        if (line.empty()) continue;
        string word;
        stringstream ss(line);
        int j = 0;
        while (ss >> word)
        {
            float number = stof(word);
            coord[j] = number;
            j++;
        }
        Point p = {coord[0], coord[1], coord[2]};
        points.push_back(p);
    }

    dx = 2 / width;
    dy = 2 / height;
    top_y = 1 - dy / 2;
    left_x = -1 + dx / 2;
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
    cout << "Total points: " << points.size() << endl;
    for (int i = 0; i < points.size(); i = i + 3)
    {
        triangle t = {points[i], points[i + 1], points[i + 2]};
        triangles.push_back(t);
    }
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
    if (!x_inRange(intersection_x1, min(t.p1.x, t.p2.x), max(t.p1.x, t.p2.x)))
    {
        left_scanline = min(intersection_x2, intersection_x3);
        right_scanline = max(intersection_x2, intersection_x3);
    }
    else if (!x_inRange(intersection_x2, min(t.p2.x, t.p3.x), max(t.p2.x, t.p3.x)))
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
    cout << "total triangles: " << triangles.size() << endl;
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
        Point v1 = {triangles[i].p2.x - triangles[i].p1.x, triangles[i].p2.y - triangles[i].p1.y, triangles[i].p2.z - triangles[i].p1.z};
        Point v2 = {triangles[i].p3.x - triangles[i].p1.x, triangles[i].p3.y - triangles[i].p1.y, triangles[i].p3.z - triangles[i].p1.z};
        Point n = {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
        float a = n.x, b = n.y, c = n.z, d = -(a * triangles[i].p1.x + b * triangles[i].p1.y + c * triangles[i].p1.z);

        int top_row = (int)((1-top_scanline) / dy);
        int bottom_row = (int)((1-bottom_scanline) / dy);
        if(top_row > height-1) top_row = height-1;
        if(bottom_row < 0) bottom_row = 0;

        for (int row = top_row; row <= bottom_row; row++)
        {
            float y = 1 - row * dy - dy/2;
            float *left_right = getLeftRightScanline(triangles[i], y, min_x, max_x);
            float left_scanline = left_right[0];
            float right_scanline = left_right[1];
            delete[] left_right;
            if (left_scanline <= left_x)
                left_scanline = left_x;
            if (right_scanline >= 1 - dx/2)
                right_scanline = 1 - dx/2;

            int cell = (int)((left_scanline + 1) / dx);
            // if (cell > width - 1)
            //     cell = width - 1;
            int cell2 = (int)((right_scanline + 1) / dx);
            // if (cell2 < 0)
            //     cell2 = 0;
            for (int column = cell; column <= cell2; column++)
            {
                float x = column * dx - 1 + dx/2;
                float distance = -(a * x + b * y + d) / c;
                
                if (distance < -1 || distance > 1)
                    continue;
                
                if (distance < z_buffer[row][column]) {
                    z_buffer[row][column] = distance;
                    image.set_pixel(column, row, red, green, blue);
                }
            }
        }
    }
}

struct Point eye, look, up;
float fovY, aspectRatio, near, far, fovX;

// initialize empty stack S 
const int ROWS = 4;
const int COLS = 4;

struct Matrix {
    float data[ROWS][COLS];

    Matrix(const float (&matrixData)[ROWS][COLS]) {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                data[i][j] = matrixData[i][j];
            }
        }
    }
};

// Global stack variable
stack<Matrix> transformationMatrices;

// initialize M = Identity matrix
float M[ROWS][COLS] = { {1, 0, 0, 0},
                        {0, 1, 0, 0},
                        {0, 0, 1, 0},
                        {0, 0, 0, 1} };

// Current State Matrix * B = C
void matrixMultiplication(float** B, float** C) {
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLS; j++) {
            C[i][j] = 0;
            for(int k=0; k<COLS; k++) {
                C[i][j] += M[i][k] * B[k][j];
            }
        }
    }
}

// RT matrix * B = C
void matrixMultiplication(float** B, float** RT, float** C) {
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLS; j++) {
            C[i][j] = 0;
            for(int k=0; k<COLS; k++) {
                C[i][j] += RT[i][k] * B[k][j];
            }
        }
    }
}

// Current State Matrix * B = Current State Matrix
void matrixMultiplication(float B[ROWS][COLS]) {
    float C[ROWS][COLS];
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLS; j++) {
            C[i][j] = 0;
            for(int k=0; k<COLS; k++) {
                C[i][j] += M[i][k] * B[k][j];
            }
        }
    }
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLS; j++) {
            M[i][j] = C[i][j];
        }
    }
}

// M1 * M2 = result
void matrixMultiplication(float m1[ROWS][COLS], float m2[ROWS][COLS], float** result) {
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLS; j++) {
            result[i][j] = 0;
            for(int k=0; k<COLS; k++) {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
}

// M1 * M2 = result
void matrixMultiplication(float m1[ROWS][COLS], float** m2, float** result) {
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLS; j++) {
            result[i][j] = 0;
            for(int k=0; k<COLS; k++) {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
}

void copyPoint(string line, struct Point* p) {
    string word;
    istringstream iss(line);
    float coord[3];
    int j = 0;
    while (iss >> word) {
        float number = stod(word);
        coord[j] = number;
        j++;
    }
    p->x = coord[0];
    p->y = coord[1];
    p->z = coord[2];
}

void copyPoint(string line, struct Point* p, float* angle) {
    string word;
    istringstream iss(line);
    float coord[4];
    int j = 0;
    while (iss >> word) {
        float number = stod(word);
        coord[j] = number;
        j++;
    }
    p->x = coord[1];
    p->y = coord[2];
    p->z = coord[3];
    *angle = coord[0];
}

void transformPoint(struct Point p, struct Point* newP) {
    float** pointMatrix = new float*[ROWS];
    for(int i=0; i<ROWS; i++) {
        pointMatrix[i] = new float[COLS];
    }
    pointMatrix[0][0] = p.x;
    pointMatrix[1][0] = p.y;
    pointMatrix[2][0] = p.z;
    pointMatrix[3][0] = 1;

    float** resultMatrix = new float*[ROWS];
    for(int i=0; i<ROWS; i++) {
        resultMatrix[i] = new float[COLS];
    }

    matrixMultiplication(pointMatrix, resultMatrix);

    newP->x = resultMatrix[0][0]/resultMatrix[3][0];
    newP->y = resultMatrix[1][0]/resultMatrix[3][0];
    newP->z = resultMatrix[2][0]/resultMatrix[3][0];
}

void transformPoint(struct Point p, struct Point* newP, float** RT) {
    float** pointMatrix = new float*[ROWS];
    for(int i=0; i<ROWS; i++) {
        pointMatrix[i] = new float[COLS];
    }
    pointMatrix[0][0] = p.x;
    pointMatrix[1][0] = p.y;
    pointMatrix[2][0] = p.z;
    pointMatrix[3][0] = 1;

    float** resultMatrix = new float*[ROWS];
    for(int i=0; i<ROWS; i++) {
        resultMatrix[i] = new float[COLS];
    }

    matrixMultiplication(pointMatrix, RT, resultMatrix);

    newP->x = resultMatrix[0][0]/resultMatrix[3][0];
    newP->y = resultMatrix[1][0]/resultMatrix[3][0];
    newP->z = resultMatrix[2][0]/resultMatrix[3][0];
}

void transformPoint(struct Point p, struct Point* newP, float P[ROWS][COLS]) {
    float** pointMatrix = new float*[ROWS];
    for(int i=0; i<ROWS; i++) {
        pointMatrix[i] = new float[COLS];
    }
    pointMatrix[0][0] = p.x;
    pointMatrix[1][0] = p.y;
    pointMatrix[2][0] = p.z;
    pointMatrix[3][0] = 1;

    float** resultMatrix = new float*[ROWS];
    for(int i=0; i<ROWS; i++) {
        resultMatrix[i] = new float[COLS];
    }

    matrixMultiplication(P, pointMatrix, resultMatrix); 

    newP->x = resultMatrix[0][0]/resultMatrix[3][0];
    newP->y = resultMatrix[1][0]/resultMatrix[3][0];
    newP->z = resultMatrix[2][0]/resultMatrix[3][0];
}

void translate(struct Point p) {
    float a[3] = {p.x, p.y, p.z};
    float I[ROWS][COLS] = { {1, 0, 0, 0},
                             {0, 1, 0, 0},
                             {0, 0, 1, 0},
                             {0, 0, 0, 1} }; 
    for(int i=0; i<3; i++) {
        I[i][3] = a[i];
    }
    matrixMultiplication(I);
}

void scale(struct Point p) {
    float a[3] = {p.x, p.y, p.z};
    float I[ROWS][COLS] = { {1, 0, 0, 0},
                             {0, 1, 0, 0},
                             {0, 0, 1, 0},
                             {0, 0, 0, 1} }; 
    for(int i=0; i<3; i++) {
        I[i][i] = a[i];
    }
    matrixMultiplication(I);
}

void normalize(struct Point *p) {
    float magnitude = sqrt(pow(p->x, 2) + pow(p->y, 2) + pow(p->z, 2));
    p->x = p->x / magnitude;
    p->y = p->y / magnitude;
    p->z = p->z / magnitude;
}


void rotate(struct Point p, float angle) {
    normalize(&p);
    float a[3] = {p.x, p.y, p.z};
    angle = angle * (M_PI/180);
    float c = cos(angle);
    float s = sin(angle);
    float t = 1 - c;
    float x = a[0];
    float y = a[1];
    float z = a[2];
    float I[ROWS][COLS] = { {t*x*x + c, t*x*y - z*s, t*x*z + y*s, 0},
                             {t*x*y + z*s, t*y*y + c, t*y*z - x*s, 0},
                             {t*x*z - y*s, t*y*z + x*s, t*z*z + c, 0},
                             {0, 0, 0, 1} };

    struct Point c1 = {I[0][0], I[1][0], I[2][0]};
    struct Point c2 = {I[0][1], I[1][1], I[2][1]};
    struct Point c3 = {I[0][2], I[1][2], I[2][2]};

    float R[ROWS][COLS] = { {c1.x, c2.x, c3.x, 0},
                             {c1.y, c2.y, c3.y, 0},
                             {c1.z, c2.z, c3.z, 0},
                             {0, 0, 0, 1} };

    matrixMultiplication(R);
}

void crossProduct(struct Point *p1, struct Point *p2, struct Point *p3) {
    float x = p1->y * p2->z - p1->z * p2->y;
    float y = p1->z * p2->x - p1->x * p2->z;
    float z = p1->x * p2->y - p1->y * p2->x;
    p3->x = x;
    p3->y = y;
    p3->z = z;
}

int main(int argc, char** argv) {

    string file1 = "./scene.txt";
    string config_file = "./config.txt";

    ifstream inputFile(file1);
    ofstream stage1("stage1.txt");
    ofstream stage2("stage2.txt");
    ofstream stage3("stage3.txt");

    if (inputFile.is_open() && stage1.is_open() && stage2.is_open() && stage3.is_open()) {
        cout << "Files opened successfully." << endl;
    } else {
        cout << "Failed to open the files. Exiting program..." << endl;
        exit(1);
    }

    
    string line; 
    for(int i = 0; i < 4; i++) {
        getline(inputFile, line);
        string word;
        istringstream iss(line);
        float coord[4];
        int j = 0;
        while (iss >> word) {
            float number = stod(word);
            coord[j] = number;
            j++;
        }
        if (i == 0) {
            eye.x = coord[0];
            eye.y = coord[1];
            eye.z = coord[2];
        } else if (i == 1) {
            look.x = coord[0];
            look.y = coord[1];
            look.z = coord[2];
        } else if (i == 2) {
            up.x = coord[0];
            up.y = coord[1];
            up.z = coord[2];
        } else {
            fovY = coord[0];
            aspectRatio = coord[1];
            near = coord[2];
            far = coord[3];
        }
    }
    fovX = fovY * aspectRatio;
    float t_ = near * tan(fovY*M_PI/360.0);
    float r_ = near * tan(fovX*M_PI/360.0);

    transformationMatrices.push(Matrix(M));
    
    struct Point l = look - eye;
    normalize(&l);
    struct Point r;
    crossProduct(&l, &up, &r);
    normalize(&r);
    struct Point u;
    crossProduct(&r, &l, &u);
    
    
    float T[ROWS][COLS] = { {1, 0, 0, -eye.x},
                             {0, 1, 0, -eye.y},
                             {0, 0, 1, -eye.z},
                             {0, 0, 0, 1} };

    float R[ROWS][COLS] = { {r.x, r.y, r.z, 0},
                             {u.x, u.y, u.z, 0},
                             {-l.x, -l.y, -l.z, 0},
                             {0, 0, 0, 1} };
    float** RT = new float*[ROWS];
    for(int i=0; i<ROWS; i++) {
        RT[i] = new float[COLS];
    }
    matrixMultiplication(R, T, RT);
    
    float P[ROWS][COLS] = { {near/r_, 0, 0, 0},
                             {0, near/t_, 0, 0},
                             {0, 0, -(far+near)/(far-near), -(2*far*near)/(far-near)},
                             {0, 0, -1, 0} };

    while (getline(inputFile, line)) {
        if (line.compare("triangle") == 0) {
            for(int i=0; i<3; i++){
                getline(inputFile, line);
                struct Point p;
                copyPoint(line, &p);
                struct Point newP;
                transformPoint(p, &newP);
                stage1 << fixed << setprecision(7) << newP.x << " " << newP.y << " " << newP.z << endl;
                struct Point newP2;
                transformPoint(newP, &newP2, RT);
                stage2 << fixed << setprecision(7) << newP2.x << " " << newP2.y << " " << newP2.z << endl;
                struct Point newP3;
                transformPoint(newP2, &newP3, P);
                stage3 << fixed << setprecision(7) << newP3.x << " " << newP3.y << " " << newP3.z << endl;
            }
            stage1 << endl;
            stage2 << endl;
            stage3 << endl;

        } else if (line.compare("translate") == 0)
        {
            getline(inputFile, line);
            struct Point p;
            copyPoint(line, &p);
            translate(p);

        } else if (line.compare("scale") == 0)
        {
            getline(inputFile, line);
            struct Point p;
            copyPoint(line, &p);
            scale(p);

        } else if (line.compare("rotate") == 0)
        {
            getline(inputFile, line);
            float angle;
            struct Point p;
            copyPoint(line, &p, &angle);
            rotate(p, angle);

        } else if (line.compare("push") == 0)
        {
            transformationMatrices.push(Matrix(M));
        } else if (line.compare("pop") == 0)
        {
            if (!transformationMatrices.empty()) 
            {
                const Matrix& I = transformationMatrices.top();
                for(int i=0; i<ROWS; i++) {
                    for(int j=0; j<COLS; j++) {
                        M[i][j] = I.data[i][j];
                    }
                }
                transformationMatrices.pop();
            }
        } else if (line.compare("end") == 0)
        {
            cout << "End of display code." << endl;
            break;
        }
        
    }

    inputFile.close();
    stage1.close();
    stage2.close();
    stage3.close();

    readFile(config_file, "./stage3.txt");
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
        for (int j = 0; j < height; j++){
            if (z_buffer[i][j] < 2) z_buffer_file << z_buffer[i][j] << " ";
        }
        z_buffer_file << endl;
    }
    z_buffer_file.close();

    for (int i = 0; i < width; i++)
        delete[] z_buffer[i];
    delete[] z_buffer;

    return 0;
}
#include <iostream>
#include <fstream>
#include <stack>
#include <sstream>
#include <cmath>
#include <iomanip>

#define _USE_MATH_DEFINES

using namespace std;

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

    string file1 = "4/scene.txt";

    if (argc == 2) {
        file1 = argv[1];
    }

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
                string output = to_string(newP.x) + " " + to_string(newP.y) + " " + to_string(newP.z);
                stage1 << output << endl; // output P’
                // stage1 << fixed << setprecision(7) << newP.x << " " << newP.y << " " << newP.z << endl;
                struct Point newP2;
                transformPoint(newP, &newP2, RT);
                string output2 = to_string(newP2.x) + " " + to_string(newP2.y) + " " + to_string(newP2.z);
                stage2 << output2 << endl; // output P’’
                // stage2 << fixed << setprecision(7) << newP2.x << " " << newP2.y << " " << newP2.z << endl;
                struct Point newP3;
                transformPoint(newP2, &newP3, P);
                string output3 = to_string(newP3.x) + " " + to_string(newP3.y) + " " + to_string(newP3.z);
                stage3 << output3 << endl; // output P’’’
                // stage3 << fixed << setprecision(7) << newP3.x << " " << newP3.y << " " << newP3.z << endl;
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
    return 0;
}
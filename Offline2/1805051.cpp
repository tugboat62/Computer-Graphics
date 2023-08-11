#define _USE_MATH_DEFINES

#include <iostream>
// #include <windows.h> // for MS Windows
// #include <GL/glut.h> // GLUT, include glu.h and gl.h
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

struct Point
{
    float x, y, z;
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


void readFile()
{
    ifstream file("./1/config.txt");
    ifstream inputfile("./1/stage3.txt");
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
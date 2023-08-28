#include<bits/stdc++.h>

using namespace std;

struct point
{
    double x, y, z;

    point()
    {
        x = y = z = 0.0;
    }

    point(double x, double y, double z) : x(x), y(y), z(z) {}
    point(double x, double y, double z, double n) : x(x), y(y), z(z) {}
    point(const point &p) : x(p.x), y(p.y), z(p.z) {}

    /** arithemtic operations  **/

    point operator+(point b) { return point(x + b.x, y + b.y, z + b.z); }
    point operator-(point b) { return point(x - b.x, y - b.y, z - b.z); }
    point operator*(double b) { return point(x * b, y * b, z * b); }
    point operator/(double b) { return point(x / b, y / b, z / b); }
    double operator*(point b) { return x * b.x + y * b.y + z * b.z; }                             // DOT PRODUCT
    point operator^(point b) { return point(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); } // CROSS PRODUCT
    point operator-() { return point(-x, -y, -z); }

    double length() {return sqrt(x*x+y*y+z*z);}

    void normalize()
    {
        double len = length();
        x /= len;
        y /= len;
        z /= len;
    }

    /** streams  **/

    friend ostream& operator<<(ostream &out, point p)
    {
        out << "(" << p.x << "," << p.y << "," << p.z << ")";
        return out;
    }

    friend istream& operator>>(istream &in, point &p)
    {
        in >> p.x >> p.y >> p.z;
        return in;
    }

    friend ofstream& operator<<(ofstream &output, point &p)
    {
        output << fixed << setprecision(7) << p.x << " " << p.y <<" " << p.z;
        return output;
    }
};

struct Ray{
    point origin, dir;
    
    Ray(point origin, point dir){
        this->origin = origin;
        dir.normalize();
        this->dir = dir;
    }

    // stream
    friend ostream& operator<<(ostream &out, Ray r)
    {
        out << "Origin : " << r.origin << ", Direction : " << r.dir;
        return out;
    }
};

struct Floor
{
    double floorWidth, tileWidth;
    point floorCenter, tileColor1, tileColor2;
    double shine, kd, ks, ka, kr;
};

struct triangle
{
    point a, b, c;

    double shine, kd, ks, ka, kr;
    point color;

    triangle(point a, point b, point c, point color, 
        double shine, double kd, double ks, double ka, double kr) : a(a), b(b), c(c), color(color), shine(shine), kd(kd), ks(ks), ka(ka), kr(kr) {}
    triangle(const triangle &t) : a(t.a), b(t.b), c(t.c), color(t.color), shine(t.shine), kd(t.kd), ks(t.ks), ka(t.ka), kr(t.kr){}

    double intercept(Ray ray)
    {
        point ab = b - a;
        point ac = c - a;
        point pvec = ray.dir ^ ac;
        double det = ab * pvec;
        if (det < 0.0000001)
            return -1;
        double invDet = 1 / det;
        point tvec = ray.origin - a;
        double u = (tvec * pvec) * invDet;
        if (u < 0 || u > 1)
            return -1;
        point qvec = tvec ^ ab;
        double v = (ray.dir * qvec) * invDet;
        if (v < 0 || u + v > 1)
            return -1;
        double t = (ac * qvec) * invDet;
        return t;
    }
};
 
struct square
{
    point a, b, c, d;

    double shine, kd, ks, ka, kr;
    point color;

    square(point a, point b, point c, point d, point color, 
        double shine, double kd, double ks, double ka, double kr) : a(a), b(b), c(c), d(d), color(color), shine(shine), kd(kd), ks(ks), ka(ka), kr(kr){}
    square(const square &s) : a(s.a), b(s.b), c(s.c), d(s.d), color(s.color), shine(s.shine), kd(s.kd), ks(s.ks), ka(s.ka), kr(s.kr){}

    double intercept(Ray ray)
    {
        point ab = b - a;
        point ac = c - a;
        point pvec = ray.dir ^ ac;
        double det = ab * pvec;
        if (det < 0.0000001)
            return -1;
        double invDet = 1 / det;
        point tvec = ray.origin - a;
        double u = (tvec * pvec) * invDet;
        if (u < 0 || u > 1)
            return -1;
        point qvec = tvec ^ ab;
        double v = (ray.dir * qvec) * invDet;
        if (v < 0 || u + v > 1)
            return -1;
        double t = (ac * qvec) * invDet;
        return t;
    }
};

struct sphere
{
    point center;
    double r, kd, ks, ka, kr, shine;
    point color;

    double intercept(Ray ray)
    {
        point oc = ray.origin - center;
        double a = ray.dir * ray.dir;
        double b = 2 * (oc * ray.dir);
        double c = (oc * oc) - r*r;
        double d = b * b - 4 * a * c;
        if (d < 0)
            return -1;
        else
        {
            double t1 = (-b + sqrt(d)) / (2 * a);
            double t2 = (-b - sqrt(d)) / (2 * a);
            if (t1 < 0 && t2 < 0)
                return -1;
            else if (t1 < 0)
                return t2;
            else if (t2 < 0)
                return t1;
            else
                return min(t1, t2);
        }
    }
};

struct pyramid
{
    point lowest_point;
    double width, height, shine, kd, ks, ka, kr;
    point color;
    
};

struct cube
{
    point bottom_lower_left;
    double side, shine, kd, ks, ka, kr;
    point color;
};

struct normallight
{
    point position;
    point color;
    double falloff;
};

struct spotlight
{
    point position;
    point color;
    double falloff;
    point direction;
    double cutoff;
};
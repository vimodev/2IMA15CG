#include "instance.h"

using namespace std;

class Sweepline {
    public:
        static void sweep(vector<Edge> *S);
};

class Point {
    public:
     Point(long double x, long double y);
     Point(long double x, long double y, int id);
     Point();
     long double x;
     long double y;
     int id;
};

class Segment {
    public:
     Segment(Point p1, Point p2);
     Segment(Point p1, Point p2, int id);
     Segment();
     Point p1;
     Point p2;
     int id;
};
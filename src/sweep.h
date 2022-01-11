#include "instance.h"
#include <vector>
#include <string>

using namespace std;

class Sweepline {
    public:
        static void sweep(vector<Edge> *S);
};

class Point {
    public:
     Point(long double x, long double y);
     Point();
     long double x;
     long double y;
};

enum EventType {
    UPPER,
    LOWER,
    INTERSECT
};

class Event {
    public:
        Event(long double x, long double y, int e1, int e2, EventType type);
        friend bool operator<(Event const& lhs, Event const& rhs) {
            return lhs.p.y > rhs.p.y;
        };
        Point p;
        int e1;
        int e2;
        EventType type;
};
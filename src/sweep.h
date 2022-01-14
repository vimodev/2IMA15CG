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
            if (lhs.p.y == rhs.p.y) {
                if (lhs.type == UPPER && rhs.type == LOWER) return false;
                if (lhs.type == INTERSECT && rhs.type == LOWER) return false;
                if (lhs.type == LOWER && rhs.type == UPPER) return true;
                if (lhs.type == INTERSECT && rhs.type == UPPER) return true;
                if (lhs.type == UPPER && rhs.type == INTERSECT) return false;
                if (lhs.type == LOWER && rhs.type == INTERSECT) return true;
                return rhs.p.x < lhs.p.x;
            }
            return lhs.p.y < rhs.p.y;
        };
        Point p;
        int e1;
        int e2;
        EventType type;
};
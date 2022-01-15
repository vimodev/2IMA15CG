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
    INTERSECTION
};

class Event {
    public:
        Event(long double x, long double y, int e1, int e2, EventType type);
        friend bool operator<(Event const& lhs, Event const& rhs) {
            if (lhs.p.y == rhs.p.y) {
                if (lhs.p.x == rhs.p.x) {
                    if (lhs.type == UPPER && rhs.type == LOWER) return false;
                    if (lhs.type == INTERSECTION && rhs.type == LOWER) return false;
                    if (lhs.type == LOWER && rhs.type == UPPER) return true;
                    if (lhs.type == INTERSECTION && rhs.type == UPPER) return true;
                    if (lhs.type == UPPER && rhs.type == INTERSECTION) return false;
                    if (lhs.type == LOWER && rhs.type == INTERSECTION) return true;
                }
                return rhs.p.x < lhs.p.x;
            }
            return lhs.p.y < rhs.p.y;
        };
        Point p;
        int e1;
        int e2;
        EventType type;
};
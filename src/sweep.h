#include "instance.h"
#include <vector>
#include <string>

using namespace std;

void sweepline(vector<Edge> S);

class Point {
    public:
     Point(long double x, long double y);
     Point();
     long double x;
     long double y;
};

class Event {
    public:
        Event(long double x, long double y, Edge *e1, Edge *e2, const string& type);
        friend bool operator<(Event const& lhs, Event const& rhs) {
            return lhs.p.y > rhs.p.y;
        };
        Point p;
        Edge *e1;
        Edge *e2;
        string type;
};
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "instance.h"

using namespace std;

int main(int argc, char **argv) {
    Instance inst("reecn73116.instance.json");
    cout << inst.id << " has " << inst.vertices->size() << " vertices and " << inst.m << " edges." << endl;
    return 0;
}
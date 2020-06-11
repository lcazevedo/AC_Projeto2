#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>

using namespace std;


int mediaInteira (int v[], int n) {
    int i;
    int total = 0;
    for (i = 0; i < n; i++) {
        total += v[i];
    }
    return total / n;
}

int main() {
    int v[3] = {2, 3, 4};
    int n = 3;

    int a = mediaInteira(v, n);
    cout << "a = " << a << endl;
}
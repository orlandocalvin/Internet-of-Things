#include <iostream>

using namespace std;

int main()
{
    int x = 10;
    // int y = x++; // x = 11, y = 10 (post-increment)
    int y = ++x; // x = 11, y = 11 (pre-increment)

    cout << x << endl;
    cout << y << endl;
    return 0;
}
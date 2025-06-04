#include <iostream>
using namespace std;

int main()
{
    int a = 1;
    int b = 2;

    // Swap the values of a and b (using a temporary variable)
    int temp = a;
    a = b;
    b = temp;

    cout << a;
    return 0;
}
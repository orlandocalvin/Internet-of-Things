#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main()
{
    // Rolling the dice Program
    const short minValue = 1;
    const short maxValue = 6;
    srand(time(nullptr));

    short dice1 = (rand() % (maxValue - minValue + 1)) + minValue;
    short dice2 = (rand() % (maxValue - minValue + 1)) + minValue;

    cout << dice1 << ", " << dice2 << endl;
    return 0;
}
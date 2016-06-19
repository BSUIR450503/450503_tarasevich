#include "memoryManager.h"
#include <iostream>
using std::cout;
using std::endl;
int main(int argc, char *argv[])
{
    int * ms = (int*)malloc(100 * sizeof(int));
    cout << "Allocating array of 100 integers" << endl;
    for (int i = 0; i < 100; i++)
    {
        ms[i] = i;
    }
    cout << "Show first 10" << endl;
    for (int i = 0; i < 10; i++)
    {
        cout << "ms[" << i << "] = " << ms[i] << endl;
    }
    cout << "Allocating new array of 10 integers" << endl;
    int * mss = (int*)malloc(10 * sizeof(int));
    for (int i = 0; i < 10; i++)
    {
        mss[i] = i * 2;
        cout << "mss[" << i << "] = " << mss[i] << endl;
    }
    cout << "Reallocating memory for mss by 15 int" << endl;
    mss = (int*)realloc(mss, 15 * sizeof(int));
    // after 10th element we won't know, what is in the element of the array, bcause we didn't assign any of them
    for (int i = 0; i < 15; i++)
    {
        cout << "mss[" << i << "] = " << mss[i]<<endl;
    }
    cout << "Show first 10 of ms" << endl;
    for (int i = 0; i < 10; i++){
        cout << "ms[" << i << "] = " << ms[i] << endl;
    }
    cout << "Clear memory for ms" << endl;
    free (ms);
    cout << "Allocating array of 100 integers to ms" << endl;
    ms = (int*)malloc(100 * sizeof(int));
    for (int i = 0; i < 10; i++){
        cout << "new ms[" << i << "] = " << ms[i] << endl;
    }

    return 0;
}
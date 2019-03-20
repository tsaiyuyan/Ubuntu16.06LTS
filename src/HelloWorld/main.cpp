#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int main(int argc, char *argv[])
{
	vector <string> strs = {"apple","banana","cat"};
	for_each(strs.begin(),strs.end(),[](string str){cout << str <<endl;});
	printf("Hello Linux!\n");
	return 0;
}

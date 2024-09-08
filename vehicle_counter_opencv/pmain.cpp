#include <iostream>

using namespace std;
int main() {
	int num1, num2;
	cout << "enter first num: ";
	cin >> num1;
	cout << "enter second num: ";
	cin >> num2;

	char opArray[5] = { '+','-', '*','/','%' };
	printf("%d %c %d = %d\n", num1, opArray[0], num2, (num1 + num2));
	printf("%d %c %d = %d\n", num1, opArray[1], num2, (num1 - num2));
	printf("%d %c %d = %d\n", num1, opArray[2], num2, (num1 * num2));
	printf("%d %c %d = %d\n", num1, opArray[3], num2, (num1 / num2));
	printf("%d %c %d = %d\n", num1, opArray[4], num2, (num1 % num2));
}
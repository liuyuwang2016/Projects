#include<iostream>  

using namespace std;

struct result {
	int max;
	int min;
};

/*
* ��һ�����飬λ��start��end(����start��end)λ��֮�����������������Сֵ��ʱ�临�Ӷ�ΪO(3*n/2)
*/
result* Get_Min_Max(int array[], int start, int end) {

	int len = end - start + 1;

	if (end < start) {
		return NULL;
	}

	result* res = new result();

	int max, min;

	if (len % 2 == 0) {
		//ż�������  
		res->max =
			array[start] > array[start + 1] ?
			array[start] : array[start + 1];
		res->min =
			array[start] < array[start + 1] ?
			array[start] : array[start + 1];

		start = start + 2; //�����ż��������Ҫ��i�ӵ�����Ԫ�ؿ�ʼ  
	}
	else {
		//���������  
		res->max = array[start];
		res->min = array[start];

		start = start + 1; //���������������Ҫ��i�ӵڶ���Ԫ�ؿ�ʼ  
	}

	while (start <= end) {
		max = array[start] > array[start + 1] ? array[start] : array[start + 1];
		min = array[start] < array[start + 1] ? array[start] : array[start + 1];

		res->max = res->max > max ? res->max : max;
		res->min = res->min < min ? res->min : min;

		start = start + 2;
	}

	return res;

}

int main() {

	int a[9] = { 5, 8, 0, -89, 9, 22, -1, -31, 98 };
	result* r1 = Get_Min_Max(a, 0, 8);
	cout << "���ֵΪ��" << r1->max << "����СֵΪ��" << r1->min << endl;

	int b[10] = { 5, 8, 0, -89, 9, 22, -1, -31, 98, 2222 };
	result* r2 = Get_Min_Max(b, 0, 9);
	cout << "���ֵΪ��" << r2->max << "����СֵΪ��" << r2->min << endl;

	delete r1;
	delete r2;

	return 0;
}
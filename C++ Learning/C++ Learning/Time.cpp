#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
ע��VS2013,2015�����gets����gets_s�����ˣ����鿴������������ܡ�
*/

template <typename T>
T findmax(T arr[], int len)
{
	T val = arr[0];
	for (int i = 1; i < len; i++)
	{
		if (arr[i]>val)
			val = arr[i];
	}
	return val;
} 
  
int main()
{
	int arr[4] = { 1, 42, 87, 100 };
	int result = findmax<int>( arr, 4);
	return 0;
}
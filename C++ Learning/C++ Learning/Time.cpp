#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
注：VS2013,2015下面把gets换成gets_s就行了，详情看《常见问题汇总》
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
//
//                       _oo0oo_
//                      o8888888o
//                      88" . "88
//                      (| -_- |)
//                      0\  =  /0
//                    ___/`---'\___
//                  .' \\|     |// '.
//                 / \\|||  :  |||// \
//                / _||||| -:- |||||- \
//               |   | \\\  -  /// |   |
//               | \_|  ''\---/''  |_/ |
//               \  .-\__  '-'  ___/-. /
//             ___'. .'  /--.--\  `. .'___
//          ."" '<  `.___\_<|>_/___.' >' "".
//         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//         \  \ `_.   \_ __\ /__ _/   .-` /  /
//     =====`-.____`.___ \_____/___.-`___.-'=====
//                       `=---='
//     ~~~~~~~~~~~~~���汣�ӣ�����Bug~~~~~~~~~~~~~
//#define _CRTDBG_MAP_ALLOC //����ڴ�й©
//#include <crtdbg.h>
//#include <vld.h>
#include "BasicDemo.h"
#include "EmptyDemo.h"
//��Ҫ�����ݶ���������棬���Լ���
#include "FreeGLUTCallbacks.h"

int main(int argc, char** argv)
{
	/*
	 * BasicDemo����Ҫ��ʾ���������ʱ��ʹ�õ�Functions.
	 * ��EmptyDemo���Լ��ڽ����������ݵ��Ե�ʱ��ʹ�õ�demo��
	 * ��ʹ�õ�ʱ��û���������塣
	 */

	BasicDemo demo;
	//EmptyDemo demo;
	return glutmain(argc, argv, 1920, 1080, "5-Axes Dispenser",&demo);
}
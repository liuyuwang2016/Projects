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
//     ~~~~~~~~~~~~~佛祖保佑，永无Bug~~~~~~~~~~~~~
//#define _CRTDBG_MAP_ALLOC //检查内存泄漏
//#include <crtdbg.h>
//#include <vld.h>
#include "BasicDemo.h"
#include "EmptyDemo.h"
//主要的内容都在这个里面，可以检索
#include "FreeGLUTCallbacks.h"

int main(int argc, char** argv)
{
	/*
	 * BasicDemo是需要显示虚拟物体的时候使用的Functions.
	 * 而EmptyDemo是自己在进行其他内容调试的时候使用的demo，
	 * 在使用的时候没有虚拟物体。
	 */

	BasicDemo demo;
	//EmptyDemo demo;
	return glutmain(argc, argv, 1920, 1080, "5-Axes Dispenser",&demo);
}
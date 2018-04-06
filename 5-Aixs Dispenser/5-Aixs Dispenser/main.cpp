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
//
//
//     ~~~~~~~~~~~~~佛祖保佑，永无Bug~~~~~~~~~~~~~
#include "Function.h"

int main(int argc, char** argv)
{
	/*--------------initial part--------------*/
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(iWidthColor, iHeightColor);
	glutCreateWindow("Perspective Projection");
	glutFullScreen();
	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(KinectUpdate);
	loadOBJModel();//Obj只load了一次
	Texture();//texture也只load了一次 在加载实验的工件的时候不使用纹理贴图
	/*--------------above this do not need check again--------------*/
	//update
	glutTimerFunc(1000, timer, 0);

	BuildPopupMenu();
	//MainLoop就是疯狂跑上面的东西永远不停下来
	glutMainLoop();
	return 0;
}
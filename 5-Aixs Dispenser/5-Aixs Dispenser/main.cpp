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
//     ~~~~~~~~~~~~~���汣�ӣ�����Bug~~~~~~~~~~~~~
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
	loadOBJModel();//Objֻload��һ��
	Texture();//textureҲֻload��һ�� �ڼ���ʵ��Ĺ�����ʱ��ʹ��������ͼ
	/*--------------above this do not need check again--------------*/
	//update
	glutTimerFunc(1000, timer, 0);

	BuildPopupMenu();
	//MainLoop���Ƿ��������Ķ�����Զ��ͣ����
	glutMainLoop();
	return 0;
}
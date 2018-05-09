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
//     ~~~~~~~~~~~~~·ð×æ±£ÓÓ£¬ÓÀÎÞBug~~~~~~~~~~~~~

#include "Function.h"

int main(int argc, char** argv)
{
	//loadConfigFile();
	/*--------------initial part--------------*/
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(iWidthColor, iHeightColor);
	glutCreateWindow("Perspective Projection");
	//glewInit();
	//if (glewIsSupported("GL_VERSION_2_0")) {
	//	printf("Ready for OpenGL 2.0\n");
	//}
	//else {
	//	printf("OpenGL 2.0 not supported\n");
	//	system("pause");
	//	exit(1);
	//}
	glutFullScreen();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	/*--------------default camera--------------*/
	g_Camera.vPosition = Vector3(-0.064403, 0.000998867, 0.0897838);
	g_Camera.vCenter = Vector3(-0.0494023, 0.0210018, 1.08971);
	g_Camera.vUpper = Vector3(-0.000199958, 0.99968, -0.019995);

	/*g_Camera.vPosition = Vector3(0, 0, 0);
	g_Camera.vCenter = Vector3(0, 0, 1);
	g_Camera.vUpper = Vector3(0, 1, 0);*/
	//loadOBJModel();
	glutDisplayFunc(RenderScene);
	glutIdleFunc(KinectUpdate);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	//setShaders();
	loadOBJModel();
	Texture();
	/*--------------above this do not need check again--------------*/
	glutTimerFunc(1000, timer, 0);

	BuildPopupMenu();
	glutMainLoop();
	for (int i = 0; i < filenames.size(); i++)
	{
		glmDelete(models[i].obj);
	}
	return 0;
}
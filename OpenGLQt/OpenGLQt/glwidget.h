#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>

class GLWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent);
	~GLWidget();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
	void keyPressEvent(QKeyEvent *e);

private:
	bool fullScreen;
};

#endif // GLWIDGET_H

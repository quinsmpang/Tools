#ifndef __MYGRAPHICSVIEW_H
#define __MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QtGui/QtGui>
#include <QtCore/QtCore>

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT

protected:
//	QPointF m_lastPos;

public:
	explicit MyGraphicsView(QWidget *parent = 0);
	void onSceneResize();	// ������С�ı�ӿڣ���Ҫ�� View �¼������ģ����Ҫ�Լ����������¼�ȥ���� View ��С

protected:
	virtual void paintEvent(QPaintEvent *e);
	// �������¼�һ��Ҫ���� Parent ����������Ͳ����Զ��ƶ���
	virtual void mouseMoveEvent(QMouseEvent * e);//����ƶ��¼���Ӧ
	virtual void mousePressEvent(QMouseEvent * e);//��굥���¼���Ӧ
	virtual void mouseReleaseEvent(QMouseEvent * e);//����ɿ��¼���Ӧ
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);
	virtual void resizeEvent(QResizeEvent *evt);	// View ��С�ı��¼�
};

#endif // GRAPHICSVIEW_H
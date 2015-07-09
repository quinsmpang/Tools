#ifndef __MyBasicListWidget_H
#define __MyBasicListWidget_H

#include "QtIncAll.h"

#include "ProjectItemBase.h"

class MyBasicListWidget : public QWidget
{
public:
	QLabel *m_label;
	QListWidget *m_list;

public:
	MyBasicListWidget(QWidget *parent = 0);
	void setViewMode(QListView::ViewMode mode);
};

#endif
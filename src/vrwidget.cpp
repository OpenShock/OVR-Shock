#include "vrwidget.h"

#include <QMouseEvent>
#include <QPainter>

ZapMe::VRWidget::VRWidget(QWidget* parent)
	: QWidget(parent)
{}

void ZapMe::VRWidget::mouseMoveEvent(QMouseEvent* event) {
	QWidget::mouseMoveEvent(event);

	// Draw a circle at the mouse position
	QPainter painter(this);
	painter.setPen(Qt::green);
	painter.drawEllipse(event->pos(), 10, 10);
}

void ZapMe::VRWidget::mousePressEvent(QMouseEvent* event) {
	QWidget::mousePressEvent(event);

	// Draw a circle at the mouse position
	QPainter painter(this);
	painter.setPen(Qt::red);
	painter.drawEllipse(event->pos(), 10, 10);
}
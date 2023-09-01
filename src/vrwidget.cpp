#include "vrwidget.h"

#include <QMouseEvent>
#include <QPainter>

ShockLink::VRWidget::VRWidget(QWidget* parent)
	: QWidget(parent)
{}

void ShockLink::VRWidget::mouseMoveEvent(QMouseEvent* event) {
	QWidget::mouseMoveEvent(event);

	// Draw a circle at the mouse position
	QPainter painter(this);
	painter.setPen(Qt::green);
	painter.drawEllipse(event->pos(), 10, 10);
}

void ShockLink::VRWidget::mousePressEvent(QMouseEvent* event) {
	QWidget::mousePressEvent(event);

	// Draw a circle at the mouse position
	QPainter painter(this);
	painter.setPen(Qt::red);
	painter.drawEllipse(event->pos(), 10, 10);
}
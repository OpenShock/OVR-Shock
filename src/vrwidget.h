#pragma once

#include <QWidget>
#include <QObject>

class QMouseEvent;

namespace ShockLink {
	class VRWidget : public QWidget {
		Q_OBJECT
		Q_DISABLE_COPY(VRWidget)
	public:
		VRWidget(QWidget* parent = nullptr);
	private:
		void mouseMoveEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
	};
}
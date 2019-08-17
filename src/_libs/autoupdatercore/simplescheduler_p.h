#pragma once
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QPair>
#include <QtCore/QHash>

namespace QtAutoUpdater {

	class SimpleScheduler : public QObject {
		Q_OBJECT

	public:
		explicit SimpleScheduler(QObject *parent = nullptr);

		//slots
		int startSchedule(int msecs, bool repeated = false, const QVariant &parameter = QVariant());
		int startSchedule(const QDateTime &when, const QVariant &parameter = QVariant());
		void cancelSchedule(int id);

	signals:
		void scheduleTriggered(const QVariant &parameter);

	protected:
		void timerEvent(QTimerEvent *event) override;

	private:
		typedef QPair<bool, QVariant> TimerInfo;

		QHash<int, TimerInfo> timerHash;
	};

}
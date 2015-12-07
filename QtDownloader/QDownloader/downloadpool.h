#ifndef DOWNLOADPOOL_H
#define DOWNLOADPOOL_H

#include <QObject>
#include <QList>
#include "qdownloader_global.h"

class DownloadTask;
class QDOWNLOADER_EXPORT DownloadPool : public QObject
{
	Q_OBJECT

public:
	~DownloadPool();

	void init();

	static DownloadPool& instance();

private:
	DownloadPool(QObject *parent);
	DownloadPool(const DownloadPool&);
	void operator=(const DownloadPool&);

	quint32 maxDownloadCount_;
	quint32 currentDownloadCount_;
	QList<DownloadTask*> lstTask_;
};

#define DownLoadPoolInstance	DownloadPool::instance()

#endif // DOWNLOADPOOL_H

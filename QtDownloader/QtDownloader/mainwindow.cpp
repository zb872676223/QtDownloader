#include "mainwindow.h"
#include <qt_windows.h>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QStandardItemModel>
#include "downloadtask.h"
#include "taskitemwidget.h"

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif

#define TASK_ITEM_DATA			"TaskItemData"

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent, 0)
	, mgr_(this)
	, task_(NULL)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
}

MainWindow::~MainWindow()
{

}

bool MainWindow::winEvent(MSG *message, long *result)
{
	if (message->message == WM_NCHITTEST)
	{
		QPoint cursorPos(GET_X_LPARAM(message->lParam), GET_Y_LPARAM(message->lParam));
		cursorPos = mapFromGlobal(cursorPos);
		*result = hitTest(cursorPos);
		return *result != HTCLIENT && *result != HTCAPTION;
	}
	else if (message->message == WM_LBUTTONDOWN)
	{
		QPoint cursorPos(GET_X_LPARAM(message->lParam), GET_Y_LPARAM(message->lParam));
		if (hitTest(cursorPos) == HTCAPTION)
		{
			PostMessage(message->hwnd, WM_SYSCOMMAND, HTCAPTION | SC_MOVE, 0);
			return true;
		}
	}
	else if (message->message == WM_LBUTTONDBLCLK)
	{
		QPoint cursorPos(GET_X_LPARAM(message->lParam), GET_Y_LPARAM(message->lParam));
		long ht = hitTest(cursorPos);
		if (ht == HTCAPTION)
		{
			onMaximize();
			return true;
		}
	}
	else if (message->message == WM_NCCALCSIZE)
	{
		*result = 0;
		QRect rect;
		if (message->wParam)
		{
			NCCALCSIZE_PARAMS *ncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(message->lParam);
			if (IsZoomed(message->hwnd))
			{
				QDesktopWidget desktopWidget;
				rect = desktopWidget.availableGeometry(this);
				ncsp->rgrc[0].left = rect.left();
				ncsp->rgrc[0].top = rect.top();
				ncsp->rgrc[0].right = rect.right();
				ncsp->rgrc[0].bottom = rect.bottom();
			}
		}
		return true;
	}
	return false;
}

void MainWindow::showEvent(QShowEvent *e)
{
	//DON'T set window style in constructor, cause when layout is applied, window style you set in constructor will be changed.
	if (isVisible())
	{
		HWND hWnd = reinterpret_cast<HWND>(winId());
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
	}
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() != Qt::Key_Escape)
	{
		__super::keyPressEvent(e);
	}
}

void MainWindow::onMinimize()
{
	PostMessage(reinterpret_cast<HWND>(winId()), WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void MainWindow::onMaximize()
{
	HWND hWnd = reinterpret_cast<HWND>(winId());
	if (IsZoomed(hWnd))
	{
		PostMessage(hWnd, WM_SYSCOMMAND, HTCAPTION | SC_RESTORE, 0);
	} 
	else
	{
		PostMessage(hWnd, WM_SYSCOMMAND, HTCAPTION | SC_MAXIMIZE, 0);
	}
}

void MainWindow::onInit()
{
	TaskItemData data;
	data.task = task_;
	ui.taskList->createCellWidget(ui.taskList->rowCount(), data);
}

void MainWindow::onStart()
{
	if (!task_)
	{
		task_ = new DownloadTask(this);
		connect(task_, SIGNAL(inited()), this, SLOT(onInit()));
		connect(task_, SIGNAL(downloadProgress(qint64, qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64, qint64)));
		connect(task_, SIGNAL(finished()), this, SLOT(onFinished()));
		task_->init("", "http://ermaopcassist.qiniudn.com/ErmaoPcAssist2.0.0.2.exe", &mgr_);
	}
	task_->start();
}

void MainWindow::onPause()
{
	task_->pause();
}

void MainWindow::onErase()
{
	task_->cancel();
}

void MainWindow::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, qint64 bytesPerSecond)
{
	DownloadTask *task = static_cast<DownloadTask*>(sender());
	TaskItemWidget *item = ui.taskList->findItemWidget(task);
	auto data = item->itemData();
	data.bytesPerSecond = bytesPerSecond;
	item->updateData(data);
}

void MainWindow::onFinished()
{

}

long MainWindow::hitTest(const QPoint &pos)
{
	if (IsZoomed(reinterpret_cast<HWND>(winId())))
	{
		auto child = childAt(pos);
		if (!child || child == this)
		{
			return HTCAPTION;
		}
		return HTCLIENT;
	}
	bool left = pos.x() <= 5;
	bool top = pos.y() <= 5;
	bool right = width() - pos.x() <= 5;
	bool bottom = height() - pos.y() <= 5;
	if (left && top)
	{
		return HTTOPLEFT;
	}
	else if (left && bottom)
	{
		return HTBOTTOMLEFT;
	}
	else if (right && top)
	{
		return HTTOPRIGHT;
	}
	else if (right && bottom)
	{
		return HTBOTTOMRIGHT;
	}
	else if (left)
	{
		return HTLEFT;
	}
	else if (top)
	{
		return HTTOP;
	}
	else if (right)
	{
		return HTRIGHT;
	}
	else if (bottom)
	{
		return HTBOTTOM;
	}
	else
	{
		auto child = childAt(pos);
		 if (!child || child == this)
		 {
			 return HTCAPTION;
		 }
	}
	return HTCLIENT;
}
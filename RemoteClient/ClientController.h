#pragma once
#include "ClientSocket.h"
#include "WatchDialog.h"
#include "RemoteClientDlg.h"
#include "StatusDlg.h"
#include "resource.h"
#include "Tool.h"
#include <map>


//#define WM_SEND_DATA (WM_USER+2) //发送数据
#define WM_SHOW_STATUS (WM_USER+3) //展示状态
#define WM_SHOW_WATCH (WM_USER+4) //远程监控
#define WM_SEND_MESSAGE (WM_USER+0x1000) //自定义消息处理

//业务逻辑和流程，是随时可能发生改变的！！！
//业务逻辑和流程，是随时可能发生改变的！！！
//业务逻辑和流程，是随时可能发生改变的！！！

class CClientController
{
public:
	//获取全局唯一对象
	static CClientController* getInstance();
	//初始化操作
	int InitController();
	//启动
	int Invoke(CWnd*& pMainWnd);
	//更新网络服务器的地址
	void UpdateAddress(int nIP, int nPort) {
		CClientSocket::getInstance()->UpdateAddress(nIP, nPort);
	}
	int DealCommand() {
		return CClientSocket::getInstance()->DealCommand();
	}
	void CloseSocket() {
		CClientSocket::getInstance()->CloseSocket();
	}

	//1.查看磁盘分区
	//2.查看指定目录下的文件
	//3.打开文件
	//4.下载文件
	//9.删除文件
	//5.鼠标操作
	//6.发送屏幕内容
	//7.锁机
	//8.解锁
	//1981.测试链接
	//返回值是状态，true成功，false失败
	bool SendCommandPacket(HWND hWnd/*数据包收到后的应答窗口*/, int nCmd, bool bAutoClose = true, BYTE* pData = NULL, size_t nLength = 0, WPARAM wParam=0);
	int GetImage(CImage& image) {
		CClientSocket* pClient = CClientSocket::getInstance();
		return CTool::Bytes2Image(image, pClient->GetPacket().strData);	
	}
	void DownloadEnd();
	int DownFile(CString strPath);
	void StartWatchScreen();
protected:
	void threadWatchScreen();
	static void threadWatchScreen(void* arg);
	CClientController() :m_statusDlg(&m_remoteDlg), m_watchDlg(&m_remoteDlg) {
		m_isClosed = true;
		m_hThreadWatch = INVALID_HANDLE_VALUE;
		m_hThread = INVALID_HANDLE_VALUE;
		m_nThreadID = -1;
	}
	~CClientController() {
		WaitForSingleObject(m_hThread, 100);
	}
	void threadFunc();
	static unsigned __stdcall threadEntry(void* arg);
	static void releaseInstance() {
		TRACE("CClientController has been called!\r\n");
		if (m_instance != NULL) {
			delete m_instance;
			TRACE("CClientController has released!\r\n");
			m_instance = NULL;
		}
	}
	LRESULT OnShowStatus(UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowWatcher(UINT nMsg, WPARAM wParam, LPARAM lParam);
private:
	typedef struct MsgInfo {
		MSG msg;
		LRESULT result;
		MsgInfo(MSG m) {
			result = 0;
			memcpy(&msg, &m, sizeof(MSG));
		}
		MsgInfo(const MsgInfo& m) {
			result = m.result;
			memcpy(&msg, &m.msg, sizeof(MSG));
		}
		MsgInfo& operator= (const MsgInfo& m) {
			if (this != &m) {
				result = m.result;
				memcpy(&msg, &m.msg, sizeof(MSG));
			}
			return *this;
		}
	} MSGINFO;
	typedef LRESULT(CClientController::* MSGFUNC)(UINT nMsg, WPARAM wParam, LPARAM lParam);
	static std::map<UINT, MSGFUNC> m_mapFunc;
	CWatchDialog m_watchDlg;
	CRemoteClientDlg m_remoteDlg;
	CStatusDlg m_statusDlg;
	HANDLE m_hThread;
	HANDLE m_hThreadWatch;
	bool m_isClosed;//监视是否关闭
	CString m_strRemote;
	//下载文件的远程路径
	CString m_strLocal;
	//下载文件的本地保存路径
	unsigned m_nThreadID;
	static CClientController* m_instance;
	class CHelper {
	public:
		CHelper() {
			//CClientController::getInstance();
		}
		~CHelper() {
			CClientController::releaseInstance();
		}
	};
	static CHelper m_helper;
};


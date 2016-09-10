#include "stdafx.h"
#include "ClientHandler.h"
#include "CefContext.h"
#include "CefBrowserApp.h"
#include "CefOtherApp.h"
#include "CefRendererApp.h"
#include "include/cef_app.h"
#include "include/cef_base.h"
#include "include/cef_command_line.h"


namespace
{
	CefString const browserProcessString = "";
	CefString const renderProcessString = "renderer";


	CefRefPtr<CefCommandLine> GetCefCommandLine()
	{
		CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();
		LPWSTR cmd = ::GetCommandLine();
		commandLine->InitFromString(cmd);
		return commandLine;
	}


	CefRefPtr<CefApp> GetApp(CefRefPtr<CefCommandLine> const &commandLine)
	{
		if (!commandLine->HasSwitch("type")) {
			return new CCefBrowserApp;
		}

		CefString processType = commandLine->GetSwitchValue("type");

		if (processType == renderProcessString) {
			return new CCefRendererApp;
		}

		return new CCefOtherApp;
	}
}


bool CefContext::isInstantiated = false;


CefContext::CefContext()
{ 
    assert(!isInstantiated);
    isInstantiated = true;

    bool isChromiumInitialized = Initialize();
    assert(isChromiumInitialized);
}


CefContext::~CefContext()
{
    Shutdown();
}


CefRefPtr<CefBrowser> CefContext::CreateBrowser(
	CefWindowHandle parentWindow,
	const std::string &url) const
{
	CefWindowInfo windowInfo;
	
	RECT clientRectangle;
	GetClientRect(parentWindow, &clientRectangle);
	windowInfo.SetAsChild(parentWindow, clientRectangle);

	CefBrowserSettings browserSettings;

	return CefBrowserHost::CreateBrowserSync(
        windowInfo, 
        new CClientHandler, 
        url, 
        browserSettings, 
        nullptr);
}


void CefContext::DoMessageLoopWork()
{
	if (isInstantiated) {
		CefDoMessageLoopWork();
	}
}


bool CefContext::Initialize()
{
	CefRefPtr<CefCommandLine> commandLine = GetCefCommandLine();
	m_pApp = GetApp(commandLine);

	CefMainArgs mainArgs;
	int exitCode = CefExecuteProcess(mainArgs, m_pApp, nullptr);
	ASSERT(exitCode < 0);

	CefSettings settings;
	settings.no_sandbox = true;
	//settings.single_process = true;
	
    return CefInitialize(mainArgs, settings, m_pApp, nullptr);
}


void CefContext::Shutdown()
{
    CefDoMessageLoopWork();
    CefDoMessageLoopWork();
    CefDoMessageLoopWork();
	CefShutdown();
}

//This file is derived from system_service.cpp which is part of the 
//iTALC project (http://italc.sourceforge.net) by Tobias Doerffel
//under the terms of the GNU General Public License version 2
//
/*! \todo This should be replaced with the QtService class
 * http://doc.trolltech.com/solutions/4/qtservice/qtservice.html.
 * All other classes are under the LGPL or some other non-GPL license;
 * replacing this class will allow us to adopt a non-GPL license
 * for the %Picto code.
 */

#include <QtCore/QProcess>
#include <QCoreApplication>
#include <QDir>

#include "systemservice.h"

SystemService::workThread * SystemService::serviceWorkThread = NULL;

SystemService::SystemService(
			const QString & _service_name,
			const QString & _service_arg,
			const QString & _service_display_name,
			const QString & _service_description,
			const QString & _service_dependencies,
			service_main _sm,
			int _argc,
			char * * _argv )
		:
	m_name( _service_name ),
	m_arg( _service_arg ),
	m_displayName( _service_display_name ),
	m_service_description( _service_description ),
	m_dependencies( _service_dependencies ),
	m_serviceMain( _sm ),
	m_running( FALSE ),
	m_argc( _argc ),
	m_argv( _argv )
{
}

bool SystemService::runInteractive()
{
	if(m_serviceMain)
	{
		return(m_serviceMain(this));
	}
	else
	{
		return(false);
	}
}

#ifdef BUILD_UNIX

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

inline bool isDebian()
{
	return( QFileInfo( "/etc/debian_version" ).exists() ||
		QFileInfo( "/etc/ubuntu_version" ).exists() );
}


bool SystemService::install(QString *) //errorDescription
{
	const QString sn = "/etc/init.d/" + m_name;
	QFile f( sn );
	f.open( QFile::WriteOnly | QFile::Truncate );
	f.write( QString( "%1 %2\n" ).
			arg( QCoreApplication::applicationFilePath() ).
			arg( m_arg ).toUtf8() );
	f.setPermissions( QFile::ReadOwner | QFile::WriteOwner |
							QFile::ExeOwner |
				QFile::ReadGroup | QFile::ExeGroup |
				QFile::ReadOther | QFile::ExeOther );
	f.close();
	if( isDebian() )
	{
		return( QProcess::execute( QString( "update-rc.d %1 defaults 50" ).
								arg( m_name ) ) == 0 );

		// This code would use the display manager to start the service
#if(0)		
		QFile f( "/etc/X11/default-display-manager" );
		f.open( QFile::ReadOnly );
		const QString dm = QString( f.readAll() ).section( '/', -1 );
		f.close();
		if( dm == "kdm" )
		{
			QFile f( "/etc/kde3/kdm/Xsetup" );
			f.open( QFile::WriteOnly );
			f.seek( f.size() );
			return( f.write( QString( "%1 %2" ).arg( sn ).arg( "start" ).
								toUtf8() ) > 0 );
		}
		else if( dm == "gdm" )
		{
		}
		else if( dm == "xdm" )
		{
		}
		else
		{
		}
#endif		
	}
	return( FALSE );
}

bool SystemService::remove(QString *) //errorDescription
{
	//TODO: implement function
	return( FALSE );
}

bool SystemService::isInstalled()
{
	//TODO: implement function
	return(false);
}

bool SystemService::start(QString *) //errorDescription
{
	return( QProcess::execute( QString( "/etc/init.d/%1 start" ).
							arg( m_name ) ) == 0 );
}

bool SystemService::stop(QString *) //errorDescription
{
	return( QProcess::execute( QString( "/etc/init.d/%1 stop" ).
							arg( m_name ) ) == 0 );
}

bool SystemService::isRunning()
{
	//TODO: implement function
	return(false);
}

bool SystemService::runAsService()
{
	if( m_running || m_serviceMain == NULL )
	{
		return( FALSE );
	}
	//new workThread( serviceMainThread );
	m_serviceMain( this );
	m_running = FALSE;
	return( TRUE );
}

void SystemService::serviceMainThread( void * _arg )
{
	SystemService * _this = static_cast<SystemService *>( _arg );
	_this->m_serviceMain( _this );
}

#elif WINCE

bool SystemService::install(QString *) //errorDescription
{
	//TODO: implement function
	return( FALSE );
}

bool SystemService::remove(QString *) //errorDescription
{
	//TODO: implement function
	return( FALSE );
}

bool SystemService::isInstalled()
{
	//TODO: implement function
	return(false);
}

bool SystemService::start(QString *) //errorDescription
{
	//TODO: implement function
  return(false);
}

bool SystemService::stop(QString *) //errorDescription
{
	//TODO: implement function
  return(false);
}

bool SystemService::isRunning()
{
	//TODO: implement function
	return(false);
}

bool SystemService::runAsService()
{
	if( m_running || m_serviceMain == NULL )
	{
		return( FALSE );
	}
	//new workThread( serviceMainThread );
	m_serviceMain( this );
	m_running = FALSE;
	return( TRUE );
}

void SystemService::serviceMainThread( void * _arg )
{
	SystemService * _this = static_cast<SystemService *>( _arg );
	_this->m_serviceMain( _this );
}

#elif WIN32

#include <stdio.h>
#include "../../common/memleakdetect.h"

SystemService * 	SystemService::s_this = NULL;
SERVICE_STATUS		SystemService::s_status;
SERVICE_STATUS_HANDLE	SystemService::s_statusHandle;
DWORD			SystemService::s_error = 0;
DWORD			SystemService::s_serviceThread = (DWORD) NULL;

bool SystemService::install(QString *) //errorDescription
{
	const unsigned int pathlength = 2048;
	char path[pathlength];
	char servicecmd[pathlength];

	// Get the filename of this executable
	if( GetModuleFileNameA( NULL, path, pathlength -
				( m_arg.length() + 2 ) ) == 0 )
	{
		return( FALSE );
	}

	// Append the service-start flag to the end of the path:
	if( strlen( path ) + 4 + m_arg.length() < pathlength )
	{
		sprintf_s( servicecmd, sizeof(servicecmd), "\"%s\" %s", path,
					m_arg.toLocal8Bit().constData() );
	}
	else
	{
		return( FALSE );
	}

	// Open the default, local Service Control Manager database
	SC_HANDLE hsrvmanager = OpenSCManager( NULL, NULL,
							SC_MANAGER_ALL_ACCESS );
	if( hsrvmanager == NULL )
	{
		return( FALSE );
	}

	// Create an entry for the service
	SC_HANDLE hservice = CreateServiceA(
			hsrvmanager,												// SCManager database
			m_name.toLocal8Bit().constData(),							// name of service
			m_displayName.toLocal8Bit().constData(),					// name to display
			SERVICE_ALL_ACCESS,											// desired access
			SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,	// service type																		
			SERVICE_AUTO_START,											// start type
			SERVICE_ERROR_NORMAL,										// error control type
			servicecmd,													// service's binary
			NULL,														// no load ordering group
			NULL,														// no tag identifier
			NULL,//m_dependencies.toLocal8Bit().constData(),			// dependencies
			NULL,														// LocalSystem account
			NULL );														// no password
	if( hservice == NULL)
	{
		CloseServiceHandle( hsrvmanager );
		return( FALSE );
	}

	SC_ACTION service_actions;
	service_actions.Delay = 100;
	service_actions.Type = SC_ACTION_RESTART;

	SERVICE_FAILURE_ACTIONS service_failure_actions;
	service_failure_actions.dwResetPeriod = 0;
	service_failure_actions.lpRebootMsg = NULL;
	service_failure_actions.lpCommand = NULL;
	service_failure_actions.lpsaActions = &service_actions;
	service_failure_actions.cActions = 1;
	ChangeServiceConfig2( hservice, SERVICE_CONFIG_FAILURE_ACTIONS,
						&service_failure_actions );

	SERVICE_DESCRIPTIONA service_description;
	char description[2000];
	strcpy_s(description,sizeof(description),m_service_description.toLocal8Bit().constData());
	service_description.lpDescription = description;
	ChangeServiceConfig2A( hservice, SERVICE_CONFIG_DESCRIPTION, &service_description);

	CloseServiceHandle( hservice );
	CloseServiceHandle( hsrvmanager );

	return( TRUE );
}

bool SystemService::remove(QString *) //errorDescription
{
	// Open the SCM
	SC_HANDLE hsrvmanager = OpenSCManager(
								NULL,						// machine (NULL == local)
								NULL,						// database (NULL == default)
								SC_MANAGER_ALL_ACCESS);		// access required										
	bool suc = TRUE;

	if( hsrvmanager )
	{ 
		SC_HANDLE hservice = OpenServiceA( hsrvmanager,
						m_name.toLocal8Bit().constData(),
							SERVICE_ALL_ACCESS );

		if( hservice != NULL )
		{
			SERVICE_STATUS status;

			// Try to stop the service
			if( ControlService( hservice, SERVICE_CONTROL_STOP,
								&status ) )
			{
				while( QueryServiceStatus( hservice, &status ) )
				{
					if( status.dwCurrentState ==
							SERVICE_STOP_PENDING )
					{
						Sleep( 1000 );
					}
					else
					{
						break;
					}
				}

				if( status.dwCurrentState != SERVICE_STOPPED )
				{
					suc = FALSE;
				}
			}

			// Now remove the service from the SCM
			if(suc)
			{
				suc = DeleteService(hservice);
			}

			CloseServiceHandle( hservice );
		}
		else
		{
			suc = FALSE;
		}
		CloseServiceHandle( hsrvmanager );
	}
	else
	{
		suc = FALSE;
	}
	return( suc );
}

bool SystemService::isInstalled()
{
	// Open the SCM
	SC_HANDLE hsrvmanager = OpenSCManager(
								NULL,						// machine (NULL == local)
								NULL,						// database (NULL == default)
								SC_MANAGER_ALL_ACCESS);		// access required
	if( hsrvmanager )
	{ 
		SC_HANDLE hservice = OpenServiceA( hsrvmanager,
					m_name.toLocal8Bit().constData(),
							SERVICE_ALL_ACCESS );

		if( hservice != NULL )
		{
			CloseServiceHandle( hservice );
			CloseServiceHandle( hsrvmanager );
			return(true);
		}
		else
		{
			CloseServiceHandle( hsrvmanager );
			return(false);
		}
	}
	else
	{
		return(false);
	}
}

bool SystemService::start(QString *) //errorDescription
{
	return( QProcess::execute( QString( "net start %1" ).arg( m_name ) )
									== 0 );
}

bool SystemService::stop(QString *) //errorDescription
{
	// Open the SCM
	SC_HANDLE hsrvmanager = OpenSCManager(
								NULL,						// machine (NULL == local)
								NULL,						// database (NULL == default)
								SC_MANAGER_ALL_ACCESS);		// access required
	if( hsrvmanager )
	{ 
		SC_HANDLE hservice = OpenServiceA( hsrvmanager,
					m_name.toLocal8Bit().constData(),
							SERVICE_ALL_ACCESS );

		if( hservice != NULL )
		{
			SERVICE_STATUS status;

			// Try to stop the service
			if( ControlService( hservice, SERVICE_CONTROL_STOP,
								&status ) )
			{
				while( QueryServiceStatus( hservice, &status ) )
				{
					if( status.dwCurrentState ==
							SERVICE_STOP_PENDING )
					{
						Sleep( 1000 );
					}
					else
					{
						break;
					}
				}

				if( status.dwCurrentState != SERVICE_STOPPED )
				{
					CloseServiceHandle( hservice );
					CloseServiceHandle( hsrvmanager );
					return( FALSE );
				}
			}
			CloseServiceHandle( hservice );
		}
		else
		{
			CloseServiceHandle( hsrvmanager );
			return( FALSE );
		}
		CloseServiceHandle( hsrvmanager );
	}
	else
	{
		return( FALSE );
	}
	return( TRUE );
}

bool SystemService::isRunning()
{
	// Open the SCM
	SC_HANDLE hsrvmanager = OpenSCManager(
								NULL,						// machine (NULL == local)
								NULL,						// database (NULL == default)
								SC_MANAGER_ALL_ACCESS);		// access required
	if( hsrvmanager )
	{ 
		SC_HANDLE hservice = OpenServiceA( hsrvmanager,
					m_name.toLocal8Bit().constData(),
							SERVICE_ALL_ACCESS );

		if( hservice != NULL )
		{
			SERVICE_STATUS status;
			bool bIsRunning;

			QueryServiceStatus(hservice, &status);

			if(status.dwCurrentState == SERVICE_RUNNING)
			{
				bIsRunning = true;
			}
			else
			{
				bIsRunning = false;
			}

			CloseServiceHandle( hservice );
			CloseServiceHandle( hsrvmanager );

			return(bIsRunning);
		}
		else
		{
			CloseServiceHandle( hsrvmanager );
			return(false);
		}
	}
	else
	{
		return(false);
	}
}

bool SystemService::runAsService()
{
	if( m_running || m_serviceMain == NULL )
	{
		return( FALSE );
	}
	m_running = TRUE;

	char name[2000];
	strcpy_s(name,sizeof(name),m_name.toLocal8Bit().data());

	// Create a service entry table
	SERVICE_TABLE_ENTRYA dispatchTable[] =
	{
		{ name, (LPSERVICE_MAIN_FUNCTIONA) main },
		{ NULL, NULL }
	} ;

	s_this = this;

	// call the service control dispatcher with our entry table
	if( !StartServiceCtrlDispatcherA( dispatchTable ) )
	{
		qCritical( "SystemService::runAsService(): "
					"StartServiceCtrlDispatcher failed." );
		return( FALSE );
	}

	return( TRUE );
}

void SystemService::serviceMainThread( void * _arg )
{
	// Save the current thread identifier
	s_serviceThread = GetCurrentThreadId();
	
	// report the status to the service control manager.
	if( !reportStatus(	SERVICE_RUNNING,	// service state
						NO_ERROR,			// exit code
						0 ) )				// wait hint
	{
		return;
	}

	SystemService * _this = static_cast<SystemService *>( _arg );
	_this->m_serviceMain( _this );

	// Mark that we're no longer running
	s_serviceThread = (DWORD) NULL;

	// Tell the service manager that we've stopped.
	reportStatus( SERVICE_STOPPED, s_error, 0 );
}

void WINAPI SystemService::main(DWORD,		//_argc
								char **)	//_argv
{
	DWORD context = 1;
	// register the service control handler
	s_statusHandle = RegisterServiceCtrlHandlerExA(
					s_this->m_name.toLocal8Bit().constData(),
								serviceCtrl, &context );

	if( s_statusHandle == 0 )
	{
		return;
	}

	// Set up some standard service state values
	s_status.dwServiceType = SERVICE_WIN32 | SERVICE_INTERACTIVE_PROCESS;
	s_status.dwServiceSpecificExitCode = 0;

	// Give this status to the SCM
	if( !reportStatus(
			SERVICE_START_PENDING,	// Service state
			NO_ERROR,				// Exit code type
			15000 ) )				// Hint as to how long WinVNC
									// should have hung before you
									// assume error
	{
		reportStatus( SERVICE_STOPPED, s_error, 0 );
		return;
	}

	//Services run in c:\Windows\System32 by default.  Change the working directory
	//to the application installation path.
	bool result = QDir::setCurrent(QCoreApplication::applicationDirPath());
	Q_ASSERT_X(result,"SystemService::main","Picto Server Service working directory was not succesfully reset to its application path.");

	//serviceMainThread(0);
	// now start the service for real
	//( new workThread( serviceMainThread, s_this ) )->start();
	serviceWorkThread = new workThread(serviceMainThread, s_this);
	serviceWorkThread->start();
	return;
}

// Service control routine
DWORD WINAPI SystemService::serviceCtrl(DWORD _ctrlcode,
										DWORD, //dwEventType
										LPVOID, //lpEventData
										LPVOID) //lpContext
{
	// What control code have we been sent?
	switch( _ctrlcode )
	{
		case SERVICE_CONTROL_STOP:
			// STOP : The service must stop
			s_status.dwCurrentState = SERVICE_STOP_PENDING;
			// Post a quit message to the main service thread
			if( s_serviceThread )
			{
				serviceWorkThread->exit();
				delete serviceWorkThread;
			}
			break;

		case SERVICE_CONTROL_INTERROGATE:
			// Service control manager just wants to know our state
			break;
		default:
			// Control code not recognised
			break;
	}

	// Tell the control manager what we're up to.
	reportStatus( s_status.dwCurrentState, NO_ERROR, 0 );
	return NO_ERROR;
}

// Service manager status reporting
bool SystemService::reportStatus( DWORD _state, DWORD _exit_code,
							DWORD _wait_hint )
{
	static DWORD checkpoint = 1;
	bool result = TRUE;

	// If we're in the start state then we don't want the control manager
	// sending us control messages because they'll confuse us.
  	if( _state == SERVICE_START_PENDING )
	{
		s_status.dwControlsAccepted = 0;
	}
	else
	{
		s_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SESSIONCHANGE;
	}

	// Save the new status we've been given
	s_status.dwCurrentState = _state;
	s_status.dwWin32ExitCode = _exit_code;
	s_status.dwWaitHint = _wait_hint;

	// Update the checkpoint variable to let the SCM know that we
	// haven't died if requests take a long time
	if( ( _state == SERVICE_RUNNING ) || ( _state == SERVICE_STOPPED ) )
	{
		s_status.dwCheckPoint = 0;
	}
	else
	{
		s_status.dwCheckPoint = checkpoint++;
	}

	// Tell the SCM our new status
	if( !( result = SetServiceStatus( s_statusHandle, &s_status ) ) )
	{
		qCritical( "SystemService::reportStatus(...): "
						"SetServiceStatus failed." );
	}

	return( result );
}


#endif


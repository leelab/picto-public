//This file is derived from system_service.h which is part of the 
//iTALC project (http://italc.sourceforge.net) by Tobias Doerffel
//under the terms of the GNU General Public License version 2
//
/*! \todo This should be replaced with the QtService class
 * http://doc.trolltech.com/solutions/4/qtservice/qtservice.html.
 * All other classes are under the LGPL or some other non-GPL license;
 * replacing this class will allow us to adopt a non-GPL license
 * for the %Picto code.
 */

#ifndef _SYSTEMSERVICE_H_
#define _SYSTEMSERVICE_H_

#if defined WIN32 || defined WINCE

#include <windows.h>
#endif

#include <QtCore/QString>
#include <QtCore/QThread>

/*!	\brief Lets us run the server as a System service.
 *
 *	This class lets us run the server as a system service, as opposed to as a
 *	user application.  We should replace this with the QtService class.
 */
class SystemService
{
public:
	typedef int(* service_main)(SystemService *);

	SystemService( const QString & _service_name,
			const QString & _service_arg = QString::null,
			const QString & _service_display_name = QString::null,
			const QString & _service_description = QString::null,
			const QString & _service_dependencies = QString::null,
			service_main _sm = NULL,
			int _argc = 0,
			char * * _argv = NULL );

	bool install(QString * errorDescription = NULL);
	bool remove(QString * errorDescription = NULL);
	bool isInstalled();
	bool reinstall()
	{
		return(remove() && install());
	}

	bool start(QString * errorDescription = NULL);
	bool stop(QString * errorDescription = NULL);
	bool isRunning();
	bool restart()
	{
		return(stop() && start());
	}

	bool runAsService();

	bool runInteractive();

	inline int & argc()
	{
		return( m_argc );
	}

	inline char * * argv()
	{
		return( m_argv );
	}

private:
	const QString m_name;
	const QString m_arg;
	const QString m_displayName;
	const QString m_service_description;
	const QString m_dependencies;
	service_main m_serviceMain;
	bool m_running;

	int m_argc;
	char * * m_argv;


	typedef void ( * workThreadFunctionPtr )( void * );

	class workThread : public QThread
	{
	public:
		workThread( workThreadFunctionPtr _ptr, void * _user = NULL ) :
			QThread(),
			m_workThreadFunction( _ptr ),
			m_user( _user )
		{
		}
		virtual void run( void )
		{
			m_workThreadFunction( m_user );
			deleteLater();
		}
	private:
		workThreadFunctionPtr m_workThreadFunction;
		void * m_user;
	} ;

	static workThread * serviceWorkThread;

	static void serviceMainThread( void * );

#if defined WIN32 && ! defined WINCE
	static void WINAPI main( DWORD, char * * );
	static DWORD WINAPI serviceCtrl( DWORD _ctrlcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext );
	static bool reportStatus( DWORD _state, DWORD _exit_code,
							DWORD _wait_hint );

	// we assume that a process won't contain more than one services
	// therefore we can make these members static
	static SystemService *		s_this;
	static SERVICE_STATUS		s_status;
	static SERVICE_STATUS_HANDLE	s_statusHandle;
	static DWORD			s_error;
	static DWORD			s_serviceThread;
#endif

};

#endif

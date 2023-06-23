#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUevent.h"

//--------------------------------------------------------------------------------
/*!
	@class		ClUevent
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@fn			ClUevent
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
ClUevent::ClUevent(QObject *parent)
	: QThread(parent)
{
	bTerminate = false;										//do initialization
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotLoopTerminate
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUevent::slotLoopTerminate()
{
	bTerminate = true;										//set true for end of loop
}
//--------------------------------------------------------------------------------
/*!
	@fn			run
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUevent::run()
{
	struct sockaddr_nl	snl;
	char cBuffer[512];
	qint32 lResult = 0;

	memset( &snl, 0, sizeof(snl) );							//clear struction

	snl.nl_family = AF_NETLINK;								//use netlink
	snl.nl_pid = getpid();									//
	snl.nl_groups = 1;										//

	lFd = 0;
	lFd = socket( PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT );	//open the socket for netlink
	if( lFd < 0 ){
		DBGFL("got error from socket");
		emit sigFatal();									//send Fatal error
		return;
	}
	lResult = bind( lFd, (struct sockaddr*)&snl, sizeof(snl) );	//bind sokect
	if( lResult < 0 ){
		DBGFL("got error from bind");
		close(lFd);
		emit sigFatal();									//send fatal error
		return;
	}
	//set blocking mode
	lResult = fcntl(lFd, F_GETFL, 0);
	if(lResult == -1){
		DBGFL("got error from fcntl");
		close(lFd);
		emit sigFatal();									//send fatal error
		return;
	}

	fcntl(lFd, F_SETFL, O_NONBLOCK);							//set non blocking mode

	while(bTerminate == false)								//turns around until terminate request
	{
		sleep(1);
		ssize_t length = recv( lFd, cBuffer, sizeof(cBuffer), 0 );	//receive message from OS, but may not exit until come on message
		if ( length < 0 ){
			if(errno == EAGAIN){							//not receive it because there are not data
				continue;
			}
			break;											//get error
		}else{
			emit sigUevent(QString(reinterpret_cast<const char *>(cBuffer)));
			memset(cBuffer,0,sizeof(cBuffer));
		}
	}
	emit sigTerminate();
	fcntl(lFd, F_SETFL, ~O_NONBLOCK);						//set blocking mode

	close(lFd);

}
//--------------------------------------------------------------------------------

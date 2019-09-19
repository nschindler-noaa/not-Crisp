/*   The status bar widget that displays selected reaches, releses, and dams
*	 it also displays latitude, longitude, as well as the time
*/

#ifndef QCSTATUSBAR_H
#define QCSTATUSBAR_H

#include <qvariant.h>
#include <qstatusbar.h>
#include <qdatetime.h>
#include <qlabel.h>

class QLabel;
class QString;

class QCStatusBar : public QStatusBar
{
	Q_OBJECT

public:
	QCStatusBar ( QWidget * parent , const char * name  );
	~QCStatusBar ();
	void setDamReach(char *);
	void setRelease(char *);
	void setLat(char* msg);
	void setLon(char* msg);
	void setMessage(char *msg);

protected:
	QString timeFormat;
	void timerEvent( QTimerEvent * );
	QLabel *timeLabel;
	QLabel *latLabel;
	QLabel *lonLabel;
	QLabel *reachDamLabel;
	QLabel *releaseLabel;


};

#endif

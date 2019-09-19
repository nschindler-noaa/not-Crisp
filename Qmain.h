

#ifndef QMAIN_H
#define QMAIN_H

#include <qapplication.h>
#include <qmutex.h>
class QCrisp;
class qIOTool;

#ifdef __cplusplus
extern "C" {
#endif
void addToLogWindow(char *msg, int id);
void AnnounceFatalError( void );

#ifdef __cplusplus
}
#endif

/* These functions return a reference to the QCrisp or qIOTool window (creating one if necessary) */
extern QCrisp* getQCrisp();
extern qIOTool* getIOTool();

class Qmain {
 public:
	Qmain(int argc, char ** argv);
    QMutex mutex;

 protected:
	void showStderrMsg( void );
	virtual void messageLog();

};

/* This class is necessary so that destroy signals from the top-level windows
 * may be routed to the correct functions to update their pointers. */
class qTopLevelManager : public QObject {
	Q_OBJECT

 public slots:
	 void cbQCrispDestroyed();
	 void cbIOToolDestroyed();

};



#endif /* QMAIN_H */




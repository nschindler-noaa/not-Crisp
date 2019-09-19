#ifndef QCrisp_H
#define QCrisp_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qmainwindow.h>


class QmapController;
class QCStatusBar ;
class qMapPane;
class qMapMedCrisp1;
class qHelpManager;

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QAction;

class QToolBar;
class QPopupMenu;
class Spacer;
class QToolButton;
class QPushButton;
class qMediatorBuilder;
class qMapMediator;
class qReleaseTool;

class QCrisp : public QMainWindow
{
   Q_OBJECT

public:
    QCrisp( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~QCrisp();

	/* The application window's menu is created dynamically. */
	void SetUpMenu( int full_gui ); //1=full gui, 0=reduced gui
	QPopupMenu * CreateRiverSegmentMenu( int *pidStart, int type, int flags = 0 );
	char * RiverSegmentName( int n, int type, int flags = 0 );
	char * ReleaseSiteName( int n );
	//event triggering fucntions
	void PurgeAllIDCs( void  ) ;
	void UpdateAllDataTokens( void ) ;
	void RunStatusChanged( void ) ;
	qMapMediator* getMapMediator() { return (qMapMediator*)mapMed_;}
	void SetStatusLine(char * text );
	  /* Interlocking of menu and monte carlo dialog */
    void updatedOutputLocking( void );
	
    Spacer* spacer5;
    QToolButton* OpenButton;
    QToolButton* SaveButton;
    QToolButton* PrintButton;
    Spacer* spacer1;
    QToolButton* MapToggle;
    QToolButton* GridShow;
    Spacer* spacer6;
    QToolButton* UnZoomButton;
    Spacer* spacer3;
    QToolButton* RunScenarioButton;
    Spacer* spacer2;
    QToolButton* CancelRunButton;
    Spacer* spacer4;
    QToolButton* MouseToolButton;
    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QPopupMenu *ViewMenu;
    QPopupMenu *ReleaseMenu;
    QPopupMenu *ReservoirMenu;
    QPopupMenu *FlowMenu;
    QPopupMenu *DamMenu;
    QPopupMenu *PassageMenu;
    QPopupMenu *RunMenu;
    QPopupMenu *AnalysisMenu;
    QPopupMenu *helpMenu;
    QToolBar *Toolbar;
    QAction* fileOpenAction;   
    QAction* fileSaveAsAction;
    QAction* filePrint_MapAction;
    QAction* fileExitAction;
    QAction* fileMouse_ToolAction;
    QAction* fileMessage_LogAction;
    QAction* fileAction;
    QAction* fileMessage_Lognew_itemAction;
    QAction* filenew_itemAction;
    QAction* viewUnZoomAction;
    QAction* viewGridAction;
    QAction* viewMapAction;
    QAction* helpContentsAction;
    QAction* helpIntroductionAction;
    QAction* helpGetting_StartedAction;
    QAction* helpKeyword_GlossaryAction;
    QAction* helpParameter_GlossaryAction;
    QAction* helpTechnical_DetailsAction;
    QAction* helpAbout_CRiSP1Action;
    QAction* releaseNewAction;
    QAction* releaseNewnew_itemAction;
    QAction* releaseRelease_ToolAction;
    QAction* runRun_ScenarioAction;
    QAction* runCancel_RunAction;
	QAction* runLock_Output_SettingsAction;
  
public slots:
	virtual void fileOpened();
    virtual void filePrint();
    virtual void fileExit();
    virtual void helpContents();
    virtual void helpAbout();
	virtual void helpGetting_Started();
	virtual void helpIntro();
    virtual void helpKeyword_Glossary();
    virtual void helpParameter_Glossary();
    virtual void helpTechnical_Details();
	virtual void messageLog();
	virtual void unZoom();
	virtual void gridShow();
	virtual void mapShow();
	virtual void mouseTool();
	virtual void ReleaseTool();
	virtual void cbRunStatusChanged( );
	virtual void DecodeCommand( int );
	virtual void ReachPredatorDensity();
	virtual void GasDistribution();
	virtual void ReachGasTheta();
	virtual void PredDistributionCoef();
	virtual void RiverParameters();
	virtual void ReachClasses();
	virtual void ReachClassNames();
	virtual void PredCoefReach();
	virtual void PredMortCoef();
	virtual void PredCoefForebay();
	virtual void PredationCoefTailrace();
	virtual void VVar();
	virtual void MigrationRateVar();
	virtual void PredTempEquation();
	virtual void MigrationEquation();
	virtual void GasMortEquation();
	virtual void HeadWaterMod();
	virtual void UnregMax();
	virtual void Transport();
	virtual void TransportMort();
	virtual void SpillEfficieny();
	virtual void SpillSchedule();
	virtual void SpillCap();
	virtual void DamPredden();
	virtual void PredProb();
	virtual void NSatEquation();
	virtual void DamGasTheta();
	virtual void KEntrain();
	virtual void DelayEquation();
	virtual void PHCapacity();
	virtual void PHSchedule();
	virtual void PHPriority();
	virtual void PHFlowMin();
 	virtual void Mortality();
	virtual void SluicewayProp();
 	virtual void FGE();
	virtual void FGEEquation();
	virtual void Mon();
	virtual void LockOutputSett(bool on);
	virtual void OutputSetting();
	virtual void Settings();
	virtual void InputReport();
	virtual void MonteReport();
	virtual void MonteAltCompare();
	virtual void MonteAnalysis();
	virtual void RunningScenario();
	virtual void RunCanceled();

protected:
	QmapController * mapController_;
	QCStatusBar * myStatus;
//	qMapPane * theMapPane_;
	qMapMedCrisp1 *mapMed_;
	qMediatorBuilder *medBuilder_;
//	qReleaseTool* theReleaseTool_;
	QRect rm;  //holds window geometry when hiding/showing map
	qHelpManager * helpWindow;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;
    QPixmap image4;
    QPixmap image5;
    QPixmap image6;
    QPixmap image7;
    QPixmap image8;   
    QPixmap image10;
	QPixmap image11;
    QPixmap image14;
    QPixmap image16;
    QPixmap image17;
    QPixmap image18;
    QPixmap image21;

	int idReleaseAddBase;   /* Release / Add */
	int idReservoirNSatDamBase;  /* Resevoir / NSat / Dam */
	int idReservoirNSatReachBase; /* Resevoir / NSat / Reach */
	int idReservoirHeadwaterTempBase;  /* Resevoir / Headwater Temp */
	int idReservoirDamWaterTempBase;  /* Resevoir / Water Temp / Dam */
	int idReservoirReachWaterTempBase;  /* Resevoir / Water Temp / Reach */
	int idReservoirHeadwaterTurbidityBase;  /* Resevoir / Headwater Turbidity */
	int idReservoirDamTurbidityBase;  /* Resevoir / Turbidity / Dam */
	int idReservoirReachTurbidityBase;  /* Resevoir / Turbidity / Reach */
	int idReservoirVelocityBase;   /* Resevoir / Velocity. */
	int idFlowDamsBase;            /* Flow / Dams */
	int idFlowReachesBase;        /* Flow / Reaches */
	int idFlowHeadwatersBase;    /* Flow / Headwaters / Flows */
	int idFlowLossBase;            /* Flow / Loss */
	int idFlowElevationBase;	 /* Flow / Elevation */
	int idFlowReservoirsBase;    /* Flow / Reservoirs */
	int idDamSpillBase;			/* Dam / Spill */
	int idPassageDamBase;        /* Passage / Passage / Dam */
	int idPassageReachBase;      /* Passage / Passage / Reach */
    int idPassageLengthBase;
#ifdef ROUTE_TRACKING
	int idPassageBypassBase;     /* Passage / Bypass Passage */
	int idPassageSluicewayBase;    /* Passage / Turbine Passage */
	int idPassageTurbineBase;    /* Passage / Turbine Passage */
	int idPassageSpillwayBase;   /* Passage / Spillway Passage */
	int idPassageRSWBase;    /* Passage / Turbine Passage */
	int idPassageTransportBase;  /* Passage / Transport Passage */
#endif
	int nDams, nReaches, nHeadwaters, nSites, nBasins;  /* For decoding. */

};




class QLogEvent : public QCustomEvent{

public:
	QString string;
	int level;

	QLogEvent(QString str, int logLevel) : QCustomEvent(QEvent::User+50){
		string = str;
		level = logLevel;
	}



};

#endif // QCrisp_H

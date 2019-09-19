//This is the all mighty resource file..
//
// THE SCOOP:
// Here's the scoop.. MSVC has a nice resource editor and it will automatically add 
// the #defines for your resources for you, down side is that the microsoft format for their
// .rc files is a bit different, actually, it looks like hell but they(the tyranny of Microsoft)
// argue that you're never supposed to see it/hack with it anyway.
//
// HOW TO DEAL:
// If you use ZFactory to build your dialogs you can have it generate the dialog resource file(s) for
// you.. then all you have to do is cut and paste the .dlg file into the crisp1.rc file.  Then 
// add whatever new #defs in here.  OR use the microsoft resource editor.. let it screw up the crisp1.rc 
// file and worry about getting it to work on the SunOS side later. your call.

#ifndef _RESOURCES_
#define _RESOURCES_

#include "qMapBitmaps.h"
#include "qMouseToolRC.h"

#define IDC_STATIC              -1

//Bitmaps
#define ID_GRID                 100
#define ID_PRINT                102
#define ID_MOUSETOOL            103
#define ID_CON                  104
#define ID_MAPSIZE              105
#define ID_MAPHELP              106
#define ID_LEFTARROW            107
#define ID_REDCIRC              108
#define ID_GRIDMENU             109
#define ID_MAPSIZEMENU          110
#define ID_FIXBTN               111
#define ID_UPARROW              112
#define ID_DOWNARROW            113
#define ID_ROUNDBTN             114
#define ID_CONDOTS              115
#define ID_CITYSCAPE            116
#define ID_HOLLOWBARS           117
#define ID_FILLEDBARS           118
#define ID_GROUPBTN             119

#define IDSL_LATITUDE           201
#define IDSL_LONGITUDE          202
#define IDSL_CLOCK              203  

#define IDS_MESSAGEWINDOW       300
#define IDT_MESSAGEWINDOWMESSAGELIST 301
#define IDD_MESSAGELOG          302
#define IDW_MESSAGEWINDOW       303
#define IDC_MESSAGELIST         304
#define IDC_PRINT               305
#define ID_ZTABCTRL1            306
//Status Line
#define PRINT                   400
#define MAINMENU                401 
#define GRAPHMENU               402
//Standard Menu Items
#define ID_OPEN                 410
#define ID_SAVE                 411
#define ID_SAVEAS               412
#define ID_NEW                  413
#define ID_CLOSE                414
#define ID_REFRESH              415
#define ID_EXIT                 416
#define ID_COPY                 417
#define ID_SELECT_ALL           418
#define ID_WARNING              419
#define ID_MESSAGE              420
#define ID_RAW                  421
#define ID_DEBUG                422
#define IDM_LOGWINDOW           423
#define ID_RELEASEMODIFY        424
#define ID_RELEASEADD           425
#define ID_REACHPREDDEN         426
#define ID_DAMPREDDEN           427
#define ID_PREDATIONPROB        428
#define ID_WATERTEMP            429
#define ID_RIVERPARAMETERS      430
#define ID_PREDATIONCOEF_REACH  431
#define ID_PREDATIONCOEF_FOREBAY  432
#define ID_PREDATIONCOEF_TAILRACE 433
#define ID_INRIVER_LATENT_MORT  4321
#define ID_TRANSPORT_LATENT_MORT  4322
#define ID_DIFFERENTIAL_RETURN  4323
#define ID_VVAR                 434
#define ID_MIGRATIONRATEVAR     435
#define ID_MIGRATIONEQUATION    436
#define ID_GASMORTEQUATION      437
#define ID_TRANSEQ              438
#define ID_SPILLEQ              439
#define ID_UNZOOM               440
#define IDM_TEXTOUT             445
#define ID_KEYGLOSS             450
#define ID_PARAMGLOSS           451
#define ID_FILEDESCR            452
#define ID_PREDTEMPEQUATION     453
#define ID_FLUSHTRANSEQUATION   455
#define ID_PREDDISTCOEF         456
#define ID_PREDMORTCOEF         458
#define ID_GASDISTRIBUTION      459

#define ID_DAMS                 500
#define ID_HEADWATERS           501
#define ID_FLOWS                502
#define ID_UNREGMAX             503
#define ID_LOSS                 504
#define ID_ELEVATION            505
#define ID_RESERVOIRS           506
#define ID_HEADWATERMOD         507
#define ID_WATERTRAVELTIME      508

/* Dam menu */
#define ID_TRANSPORT            509
#define ID_TRANSPORT_MORT       510
#define ID_SPILLEFFICIENCY      511
#define ID_SPILLSCHEDULE        512
#define ID_SPILLCAP             513
#define ID_NSATEQUATION         514
#define ID_DELAYPARAM           515
#define ID_DELAYSCALAR          516
#define ID_DELAYEQUATION        517
#define ID_PH_CAPACITY          518
#define ID_PH_SCHEDULE          519
#define ID_PH_PRIORITY          520
#define ID_PH_FLOW_MIN			521
#define ID_TAILRACELENGTH       522
#define ID_MORTALITY            523
#define ID_FGE                  524
#define ID_FGEEQUATION          525
#define ID_DAMSURVIVAL          526
#define ID_SEPARATIONPROB    	527
#define ID_SCE                  528
#define ID_MON                  529
#define ID_RUNMONTE             530
#define ID_SETTINGS             531
#define ID_GETSTA               532
#define ID_INTRODUCTION         533
#define ID_ABO                  534
#define ID_DAM_OUTPUTSETTING    535
#define ID_REACH_OUTPUTSETTING  536
#define ID_HEADW_OUTPUTSETTING  537
#define ID_SCECAN               538
#define ID_INPUTREPORT          539
#define ID_MONTEREPORT          540
#define ID_MONTEALTCOMPARE      541
#define ID_MONTEANALYSIS        542
#define ID_NIGHTNSATEQUATION    543
#define ID_BACKUPNSATEQUATION   544
#define ID_DAM_GAS_THETA        545
#define ID_REACH_GAS_THETA      546
#define ID_K_ENTRAIN            547
#define ID_LOCK_OUTPUT_SETTINGS 548
#define ID_REACH_CLASSES        549
#define ID_REACH_CLASS_NAMES    550
#define ID_RSWPROP				551
#define ID_SLUICEWAYPROP		552
#define ID_TRANSPORTDATES		553

#define IDD_SCHEDULE            600
#define IDD_SCHEDULETOOL        601
#define IDC_LIST1               602
#define IDC_STARTDAY            603
#define IDC_STOPDAY             604
#define IDC_VALUE               605
#define IDC_CLEAR               606
#define IDC_TITLETEXT           607
#define IDC_LABELTEXT           608
#define IDC_LEFTARROW           609
#define IDC_FRACTIONS           610

#define IDC_LISTBOX1            620
#define IDC_LISTBOX2            621
#define IDC_LISTBOX3            622
#define IDC_LISTBOX4            623

#define IDC_LLABEL1             630
#define IDC_LLABEL2             631
#define IDC_LLABEL3             632
#define IDC_LLABEL4             633
#define IDC_LLABEL5             634
#define IDC_LLABEL6             635
#define IDC_LLABEL7             636
#define IDC_LLABEL8             637

#define IDC_EDITBOX1            640
#define IDC_EDITBOX2            641
#define IDC_EDITBOX3            642
#define IDC_EDITBOX4            643
#define IDC_EDITBOX5            644
#define IDC_EDITBOX6            645
#define IDC_EDITBOX7            646
#define IDC_EDITBOX8            647
#define IDC_EDITBOX9            648
#define IDC_EDITBOX10           649

#define IDC_ELABEL1             650
#define IDC_ELABEL2             651
#define IDC_ELABEL3             652
#define IDC_ELABEL4             653
#define IDC_ELABEL5             654
#define IDC_ELABEL6             655
#define IDC_ELABEL7             656
#define IDC_ELABEL8             657

#define IDC_DIRTY1              660
#define IDC_DIRTY2              661
#define IDC_DIRTY3              662
#define IDC_DIRTY4              663
#define IDC_DIRTY5              664
#define IDC_DIRTY6              665
#define IDC_DIRTY7              666
#define IDC_DIRTY8              667

#define IDC_RADIOBUTTON1		670
#define IDC_RADIOBUTTON2		671
#define IDC_RADIOBUTTON3		672
#define IDC_RADIOBUTTON4		673
#define IDC_RADIOBUTTON5		674
#define IDC_RADIOBUTTON6		675
#define IDC_RADIOBUTTON7		676
#define IDC_RADIOBUTTON8		677
#define IDC_RADIOBUTTON9		678
#define IDC_RADIOBUTTON10		679


#define IDC_CHECKBOX1           680
#define IDC_CHECKBOX2           681

#define IDC_RBLABEL1            685
#define IDC_RBLABEL2            686

#define IDC_PROGRESSBAR1        690
#define IDC_PROGRESSBAR2        691
#define IDC_PROGRESSBAR3        692

#define IDC_CREATE              700
#define IDC_DELETE              701
#define IDC_README              702
#define ID_WAND                 703
#define IDC_GROUPBOX            704
#define IDC_TABCONTROL          705
#define IDC_CANCEL              706
#define IDC_APPLY               707
#define IDC_APPLY_ALL           708
#define IDC_DONE                709
#define IDC_OK                  710
#define IDC_DHELP               711
#define IDC_RESET               712
#define IDC_RESET_ALL           713
#define IDC_SCHEDULE            714
#define IDC_LIVEUPDATE          715
#define IDC_RENAME              716
#define IDC_GETDIR              717
#define IDC_GETFLOWFILE         718
#define IDC_SETNGAME            719
#define IDC_SETALLNGAME         720
#define IDC_GETYEARFILE         721
#define IDC_SETFILE             722
#define IDC_SETALLFILE          723
#define IDC_GOTODIR             724
#define IDC_USEFLOWFILE         725
#define IDC_USENFILES           726
#define IDC_CANCELRUN           727
#define IDC_VIEW                728
#define IDC_VIEWALL             729
#define ID_HELP                730

#define IDC_SCROLL1             750
#define IDC_SCROLL2             751
#define IDC_SCROLL3             752
#define IDC_SCROLL4             753
#define IDC_SCROLL5             754
#define IDC_SCROLL6             755
#define IDC_SCROLL7             756
#define IDC_SCROLL8             757

#define IDC_SLABEL1             760
#define IDC_SLABEL2             761
#define IDC_SLABEL3             762
#define IDC_SLABEL4             763
#define IDC_SLABEL5             764
#define IDC_SLABEL6             765
#define IDC_SLABEL7             766

#define IDC_COMBOBOX1           770
#define IDC_COMBOBOX2           771
#define IDC_COMBOBOX3           772
#define IDC_COMBOBOX4           773
#define IDC_COMBOBOX5           774
#define IDC_COMBOBOX6           765
#define IDC_COMBOBOX7           766

#define IDC_CLABEL1             780
#define IDC_CLABEL2             781
#define IDC_CLABEL3             782
#define IDC_CLABEL4             783
#define IDC_CLABEL5             784
#define IDC_CLABEL6             785
#define IDC_CLABEL7             786

#define IDC_GROUPBOX1           790
#define IDC_GROUPBOX2           791
#define IDC_GROUPBOX3           792
#define IDC_GROUPBOX4           793
#define IDC_GROUPBOX5           794
#define IDC_GROUPBOX6           795

#define IDS_NEW                 800
#define IDS_EXISTING            801
#define IDS_DATE                802
#define IDD_GRAPH               803
#define IDC_SAVEGRAPH           804
#define IDC_PRINTGRAPH          805
#define IDC_RESETGRAPH          806
#define IDC_POSITION            807
#define IDD_MONTE               808
#define IDD_CALIBRATION         809
#define IDD_MOUSETOOL           810
#define IDD_SETTINGS            811
#define ID_VARSUP               812
#define ID_PREDVOL              813
#define ID_AGEDEP               814
#define IDD_SLIDERVAL           815
#define ID_SLIDERVAL            816
#define ID_SLIDERLABEL          817
#define IDI_ICON1               818
#define IDD_GENERIC             819
#define IDD_TEST                820
#define IDD_TIMEBASE            821
#define IDD_EQUATION1           822
#define IDD_LISTBASE            823
#define IDD_SELECTBASE          824
#define IDD_RELEASETOOL         825
#define IDD_TRANSPORTTOOL       826
#define IDD_MONTECARLO          827
#define IDD_PROMPTFOR           828
#define IDD_SAVEDISCAN          829
#define IDD_DATAREPORTTOOL      830
#define IDD_MONTEALTCOMPARE     831
#define IDD_MONTEANALYSIS       832

#define IDDS_EQUATION1          833
#define IDDS_TIMEBASE           834
#define IDDS_MONTEALTCOMPARE    835
#define IDDS_MONTEANALYSIS      836
#define IDDS_MONTECARLO         837
#define IDDS_MOUSETOOL          838
#define IDDS_SETTINGS           839
#define IDDS_SCHEDULE           840
#define IDDS_LISTBASE           841
#define IDDS_SELECTBASE         842
#define IDDS_DATAREPORTTOOL     843
#define IDDS_RELEASETOOL        844
#define IDDS_SCHEDULETOOL       845
#define IDDS_TRANSPORTTOOL      846

#define IDD_ABOUT               847
#define IDDS_ABOUT              848

#define IDD_FATALERROR          849
#define IDDS_FATALERROR         850

#define IDD_REACHCLASSNAMES     851
#define IDDS_REACHCLASSNAMES     852
   

/* The end of the statically defined menu id's for the app's main menu. */
#define ID_DYNAMIC_BASE         1000

#endif

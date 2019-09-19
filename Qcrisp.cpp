#include <qvariant.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qthread.h>
#include <qevent.h>
#include <qiconset.h>
#include "qHelpManager.h"
#include "QLogWindow.h"
#include "Qcrisp.h"
#include "resource.h"
#include "crisp.h"
#include "monte.h"
#include "log.h"
#include "run_mode.h"
#include "batch.h"
#include "main.h"
#include "args.h"
#include "init.h"
#include "init_river.h"
#include "file.h"
#include "QmapController.h"
#include "Qmain.h"
#include "QCStatusBar.h"
#include "qMapPane.h"
#include "qMapMedCrisp1.h"
#include "qMapMedGridLines.h"
#include "qMapMedStatusBarUpdator.h"
#include "qMapMedDialogBuilder.h"
#include "qMouseTool.h"
#include "qGlobalEvents.h"
#include "qMediatorBuilder.h"
#include "qMapMedTypes.h"
#include "qUtils.h"
#include "qDamReachGDT.h"
#include "qEditGraphGDT.h"
#include "qPassHistGDT.h"
#include "qGraphDataTokenTypes.h"
#include "qListDialogMedBase.h"
#include "qListDialogBase.h"
#include "qIdcList.h"
#include "qCheckBoxDialogMedBase.h"
#include "qCheckBoxDialogBase.h"
#include "qReachClassCBDMed.h"
#include "qDamOutputCBDMedBase.h"
#include "qReachOutputCBDMedBase.h"
#include "qHeadwOutputCBDMedBase.h"
#include "qReachClassSDMed.h"
#include "qRunSettings.h"
#include "qReachClassTool.h"
#include "qInputReportTool.h"
#include "qMonteCarloTool.h"
#include "qMonteAltCmpDlg.h"
#include "qMonteAnalDlg.h"
#include "qMonteReportTool.h"
#include "qReleaseTool.h"
#include "qSelectDialogMedBase.h"
#include "qSelectDialogBase.h"
#include "qIdcSelect.h"
#include "qScheduleDialogMed.h"
#include "qTransportTool.h"
#include "qIOTool.h"
#include "qFileManager.h"
#include "qScenarioManager.h"

#include "parse.h"
#include "flow.h"
#include "c1tokens.h"
#include "writeutil.h"
#include "dataio.h"

//qEvents * c1eventManager = new qEvents();

qRunSettings *theRunSettings_ = NULL;
qReachClassTool *theReachClassTool_ = NULL;
qMonteCarloTool *theMonteCarloTool_ = NULL;
qMapPane *theMapPane_ = NULL;
qReleaseTool* theReleaseTool_ = NULL;
qTransportTool *theTransportTool_ = NULL;


extern "C" {
extern void rt_set_input_pathname( char *path );
extern void rt_set_datafile( char *name );
extern void run_realtime( int mode );
extern char *get_program_version();
}


/* These routines allow a pre-existing, but hidden, dialog box to be
 *  revealed to announce a fatal error.
 */


static const char* const image0_data[] = { 
"25 22 7 1",
"# c #000000",
"e c #808000",
"b c #808080",
"a c #c0c0c0",
"c c #ff0000",
"d c #ffff00",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaacccaaaaaaab#",
".aaaaaaaaaaacaaacacaaaab#",
".aaaaaaaaaaaaaaaaccaaaab#",
".aaaa###aaaaaaaacccaaaab#",
".aaa#.d.#######aaaaaaaab#",
".aaa#d.d.d.d.d#aaaaaaaab#",
".aaa#.d.d.d.d.#aaaaaaaab#",
".aaa#d.d.###########aaab#",
".aaa#.d.#dedededed#aaaab#",
".aaa#d.#dedededed#aaaaab#",
".aaa#.#dedededed#aaaaaab#",
".aaa##dedededed#aaaaaaab#",
".aaa###########aaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image1_data[] = { 
"25 22 5 1",
"# c #000000",
"c c #0000ff",
"b c #808080",
"a c #c0c0c0",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaa#############aaaab#",
".aaaaa#c#.......#b#aaaab#",
".aaaaa#c#.......#c#aaaab#",
".aaaaa#c#.......#c#aaaab#",
".aaaaa#c#.......#c#aaaab#",
".aaaaa#c#.......#c#aaaab#",
".aaaaa#cc#######cc#aaaab#",
".aaaaa#ccccccccccc#aaaab#",
".aaaaa#ccbbbbbbbcc#aaaab#",
".aaaaa#ccbccbbbbcc#aaaab#",
".aaaaa#ccbccbbbbcc#aaaab#",
".aaaaa#ccbccbbbbcc#aaaab#",
".aaaaaa############aaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image2_data[] = { 
"25 22 5 1",
"# c #000000",
"c c #0000ff",
"b c #808080",
"a c #c0c0c0",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaa########aaaaaab#",
".aaaaaaa#.......#aaaaaab#",
".aaaaaaa#.####..#aaaaaab#",
".aaaaaa#.......#aaaaaaab#",
".aaaaaa#.####..####aaaab#",
".aaaaa#.......#aa#.#aaab#",
".aaaa##########a#.##aaab#",
".aaa#aaaaaaaaaa#.#a#aaab#",
".aa##############aa#aaab#",
".aa#aaaaaaaaaaaa#a#aaaab#",
".aa#aaaaaaa###aa###aaaab#",
".aa#aaaaaaacccaa#.#aaaab#",
".aa#############.#aaaaab#",
".aaa#.........#.#aaaaaab#",
".aaaa###########aaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image3_data[] = { 
"25 22 5 1",
"# c #000000",
"c c #0000ff",
"b c #808080",
"a c #c0c0c0",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaccab#",
".aaaaaaaaaaaaaaaaaacccab#",
".aaaaaaaaaaaaaaaaacccaab#",
".aaaaaaaaaaaaaaacccccaab#",
".aaaaaaaaaaaaaccccaaaaab#",
".aaaaaaaaaaacccccaaaaaab#",
".aaaaaaaaaaccccaaaaaaaab#",
".aaaaaaccccccccaaaaaaaab#",
".aaacccccccccaaaaaaaaaab#",
".aaccccccccccaaaaaaaaaab#",
".acccccccccccccaaaaaaaab#",
".accccccaaaacccccaaaaaab#",
".accccaaaaaacccccaaaaaab#",
".acccaaaaaaaaaacccaaaaab#",
".acaaaaaaaaaaaaacccaaaab#",
".aaaaaaaaaaaaaaaaccccaab#",
".aaaaaaaaaaaaaaaaaaccccb#",
".aaaaaaaaaaaaaaaaaaacccb#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image4_data[] = { 
"25 22 5 1",
"# c #000000",
"b c #800000",
"c c #808080",
"a c #c0c0c0",
". c #ffffff",
"........................#",
".aaaabaaaabaaaabaaaabaac#",
".aaaabaaaabaaaabaaaabaac#",
".aaaabaaaabaaaabaaaabaac#",
".aaaabaaaabaaaabaaaabaac#",
".bbbbbbbabbbbabbbbabbbbc#",
".aaaabaaaabaaaabaaaabaac#",
".aaaabaaaabaaaabaaaabaac#",
".aaaaaaaaaaaaaaaaaaaaaac#",
".aaaabaaaabaaaabaaaabaac#",
".bbbbbbbabbbbabbbbabbbbc#",
".aaaabaaaabaaaabaaaabaac#",
".aaaabaaaabaaaabaaaabaac#",
".aaaaaaaaaaaaaaaaaaaaaac#",
".aaaabaaaabaaaabaaaabaac#",
".bbbbbbbabbbbabbbbabbbbc#",
".aaaabaaaabaaaabaaaabaac#",
".aaaabaaaabaaaabaaaabaac#",
".aaaabaaaabaaaabaaaabaac#",
".aaaabaaaabaaaabaaaabaac#",
".ccccccccccccccccccccccc#",
"#########################"};

static const char* const image5_data[] = { 
"25 22 7 1",
"# c #000000",
"e c #800000",
"c c #802b00",
"b c #808080",
"a c #c0c0c0",
"d c #ff8055",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaccdaaaaaaaaaaab#",
".aaaaaaaaccdaaaaaaaaaaab#",
".aaaaaaaaccdaaaaaaaaaaab#",
".aaaaaeaaacccdaaaaaaaaab#",
".aaaaadeaecccccdaaaaaaab#",
".aaaaaadeecccaccdaaaaaab#",
".aaaaaaaddcccaacdaaaaaab#",
".aaaaaaaaadcccaedaaaaaab#",
".aaaaaaaaadcccaaaaaaaaab#",
".aaaaaaaadecccdaaaaaaaab#",
".aaaaaaadecceecaaaaaaaab#",
".aaaaaaadccaaeedaaaaaaab#",
".aaaaaaaadccaaeedaaaaaab#",
".aaaaaaaaadeeaaeedaaaaab#",
".aaaaaaaaaaedaaaeeaaaaab#",
".aaaaaaaaaadaaaaeaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image6_data[] = { 
"25 22 8 1",
"# c #000000",
"f c #800000",
"d c #802b00",
"b c #808080",
"a c #c0c0c0",
"c c #ff0000",
"e c #ff8055",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".accaaaaaaaaaaaaaaaacccb#",
".acccaaaaddeaaaaaaacccab#",
".aaccccaaddeaaaaaacccaab#",
".aaaccccaddeaaaaacccaaab#",
".aaaaacccadddeaccccaaaab#",
".aaaaaecccddddccccaaaaab#",
".aaaaaaecccadccceaaaaaab#",
".aaaaaaaeccccccdeaaaaaab#",
".aaaaaaaaaccccafeaaaaaab#",
".aaaaaaaaacccccaaaaaaaab#",
".aaaaaaaacccdcccaaaaaaab#",
".aaaaaaacccdffcccaaaaaab#",
".aaaaaccccdaaffcccaaaaab#",
".aaaacccceddaaffccccaaab#",
".aaacccaaaeffaaffccccaab#",
".aacccaaaaafeaaaffacccab#",
".acccaaaaaaeaaaafaaacccb#",
".accaaaaaaaaaaaaaaaaacab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image7_data[] = { 
"25 22 4 1",
"# c #000000",
"b c #808080",
"a c #c0c0c0",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaa..aaaaaaaaaaaaaab#",
".aaaaaa.#.aaaaaaaaaaaaab#",
".aaaaaa.##.aaaaaaaaaaaab#",
".aaaaaa.###.aaaaaaaaaaab#",
".aaaaaa.####.aaaaaaaaaab#",
".aaaaaa.#####.aaaaaaaaab#",
".aaaaaa.######.aaaaaaaab#",
".aaaaaa.#######.aaaaaaab#",
".aaaaaa.########.aaaaaab#",
".aaaaaa.#####...aaaaaaab#",
".aaaaaa.##.###.aaaaaaaab#",
".aaaaaa.#.a.##.aaaaaaaab#",
".aaaaaaa.aa.###.aaaaaaab#",
".aaaaaaaaaaa.##.aaaaaaab#",
".aaaaaaaaaaaa..aaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image8_data[] = { 
"22 22 5 1",
". c None",
"# c #000000",
"c c #848200",
"a c #ffff00",
"b c #ffffff",
"......................",
"......................",
"......................",
"............####....#.",
"...........#....##.##.",
"..................###.",
".................####.",
".####...........#####.",
"#abab##########.......",
"#babababababab#.......",
"#ababababababa#.......",
"#babababababab#.......",
"#ababab###############",
"#babab##cccccccccccc##",
"#abab##cccccccccccc##.",
"#bab##cccccccccccc##..",
"#ab##cccccccccccc##...",
"#b##cccccccccccc##....",
"###cccccccccccc##.....",
"##cccccccccccc##......",
"###############.......",
"......................"};


static const char* const image10_data[] = { 
"22 22 88 2",
"Qt c None",
".2 c #000000",
".S c #08ff08",
"#v c #100810",
".U c #101010",
"#c c #101018",
".M c #181018",
"#e c #181818",
".A c #181821",
".L c #211821",
"#l c #212121",
".z c #212129",
"#m c #292129",
"#u c #292929",
"#n c #292931",
".R c #29ff29",
"#o c #312931",
".T c #313131",
"#p c #313139",
".Z c #31ff31",
"#q c #393139",
"#t c #393939",
".y c #393942",
"#s c #423942",
".o c #424242",
"#h c #4a4a52",
".n c #5a525a",
"#r c #5a5a63",
".I c #5ace5a",
"#b c #6b636b",
".p c #6b6b6b",
".x c #6b6b73",
".Y c #6bff63",
".l c #736b73",
".t c #7b737b",
".s c #7b7384",
".0 c #7bff7b",
".r c #847b84",
".u c #847b8c",
"#g c #84848c",
".v c #8c7b94",
"#i c #8c848c",
".w c #8c8494",
"#j c #8c8c8c",
".8 c #8c8c94",
".m c #948c94",
"#k c #948c9c",
"#f c #949494",
".q c #94949c",
".J c #94c694",
"#d c #9c949c",
"#a c #9c94a5",
".k c #9c9c9c",
".N c #9c9ca5",
".H c #9ccea5",
".K c #a59ca5",
"#. c #a59cad",
".i c #a5a5a5",
".3 c #a5a5ad",
"## c #ad9cad",
".V c #ada5ad",
".d c #adadad",
".j c #adadb5",
".9 c #b5adb5",
".# c #b5b5b5",
".a c #bdbdbd",
".7 c #bdd6bd",
".c c #c6c6c6",
".5 c #cec6ce",
".b c #cecece",
".4 c #ceced6",
".F c #d6ced6",
".G c #d6cede",
".h c #d6d6d6",
".E c #d6d6de",
".Q c #d6ffd6",
".B c #ded6de",
".1 c #ded6e7",
".g c #dedede",
".D c #dedee7",
".6 c #e7dee7",
".f c #e7e7e7",
".C c #e7e7ef",
".X c #e7ffe7",
".O c #efe7ef",
".e c #efefef",
".W c #f7f7f7",
".P c #ffffff",
"QtQtQtQtQtQt.#.a.b.b.b.b.c.c.a.a.d.aQtQtQtQt",
"QtQtQtQtQtQt.a.e.f.f.f.f.f.e.e.e.g.aQtQtQtQt",
"QtQtQtQtQtQt.a.c.c.c.b.b.c.c.c.c.a.cQtQtQtQt",
"QtQtQtQtQtQt.#.a.a.a.a.#.a.a.#.#.d.aQtQtQtQt",
"QtQtQtQtQt.c.d.c.a.c.c.c.a.a.a.c.#QtQtQtQtQt",
"QtQtQtQtQt.a.a.#.a.a.a.a.a.a.c.c.#QtQtQtQtQt",
"QtQtQtQtQt.a.#.c.a.a.a.a.a.c.a.c.dQtQtQtQtQt",
"QtQtQtQtQt.c.a.a.a.a.a.a.a.a.a.a.#QtQtQtQtQt",
"QtQtQtQtQt.d.b.f.g.g.g.g.g.g.h.g.i.i.jQtQtQt",
"QtQtQt.a.k.l.#.h.b.h.b.h.b.h.g.g.m.n.o.p.#Qt",
"QtQt.a.q.r.s.t.t.t.t.t.t.t.u.v.w.x.y.z.A.o.i",
"Qt.a.k.B.C.D.B.E.E.E.E.F.G.H.I.J.K.o.L.L.M.y",
".a.N.O.P.P.P.P.P.P.P.P.P.Q.R.S.R.b.v.T.A.U.L",
".V.W.P.P.P.P.P.P.P.P.P.P.X.Y.Z.0.P.1.t.A.2.L",
".3.E.4.5.4.h.E.E.g.6.D.B.D.E.7.F.4.5.8.M.2.A",
".m.9.j.V.3#..3.K#.#..i#..K#.###a.q.8#b#c.2.L",
".m.j.j#..3.K.K.K.N.K.N.N.N.N#a#d#d.w#b#c.2#e",
"#f#.#..K.N.K.N.N.N#a.k#a#d#d#d#a.m#g#b.M.2#h",
".m.3.K.K#a.k#a#d#a.k#a#d#a#d.q.m.8#i.x#c#e.d",
"#f#g#i.w#j.w#i.8.w#i.8.8.m.8.m#k.8.w#b#e#fQt",
".#.l.z.A#l.z#m#m#m#n#o#o#p#p#q#q#p#o#p#fQtQt",
"QtQt.d#r#s#s#t#p.T.T.T#u#u.z#e#e#v.o.kQtQtQt"};
/*
static const char* const image11_data[] = { 
"32 32 4 1",
"a c #000000",
"b c #947300",
". c #c6c6c6",
"# c #ffffff",
"................................",
".##############################a",
".##############################a",
".##############################a",
".###aaaa#########aa############a",
".##aa##aa######aaaa############a",
".#aa####a########aa############a",
".#aa#############aa############a",
".#aa#############aa############a",
".#aa#############aa############a",
".#aa#############aa############a",
".#aa####a########aa############a",
".##aa##aa########aa############a",
".###aaaa#########aa############a",
".##############################a",
".##############################a",
".##############################a",
".##############################a",
".#################a############a",
".################a.a#######a###a",
".#############aaa.#aaaaa##aa###a",
".##########aaa.#.#a#a#.#aa.a###a",
".#########a.b.#a#a#.#.#a#..a###a",
".###########a.b#.#.#.#a#...a###a",
".#########a#.b#..a######.a.a###a",
".##########aaa..a######..#aa###a",
".#############aaaaaaaaa.a##a###a",
".######################a#######a",
".##############################a",
".##############################a",
".##############################a",
".aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"};*/

static const char* const image11_data[] = { 
"20 20 8 1",
"b c #000000",
"e c #005284",
"a c #4a7ba5",
"f c #6b6b21",
". c #8494a5",
"c c #ad8400",
"d c #dece29",
"# c #ffffff",
"###################b",
"###################b",
"##.bc#####.b#######b",
"#db#ab###cbb#######b",
"#bc##a####cb#######b",
"#ba#######ab#######b",
"#bc#######cb#######b",
"#cb##b####ab#######b",
"##cbb#####cb#######b",
"###################b",
"###################b",
"###########b#######b",
"#########bbabbc#cb#b",
"#######bec#badabac#b",
"######.fcf.a#ac#.f#b",
"######acf#a####cab#b",
"#######a.bbbcbca.b#b",
"##############a####b",
"###################b",
"abebababfbebababfbab"};




static const char* const image14_data[] = { 
"25 22 7 1",
"# c #000000",
"e c #808000",
"b c #808080",
"a c #c0c0c0",
"c c #ff0000",
"d c #ffff00",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaacaaaaaaaab#",
".aaaaaaaaaaacaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaacaaaab#",
".aaaa#a#aaaaaaaaacaaaaab#",
".aaa#ada#a#a#a#aaaaaaaab#",
".aaaadadadadadaaaaaaaaab#",
".aaa#adadadada#aaaaaaaab#",
".aaaadada#a#a#a#a#a#aaab#",
".aaa#ada#aeaeaeaea#aaaab#",
".aaaada#aeaeaeaea#aaaaab#",
".aaa#a#aeaeaeaea#aaaaaab#",
".aaaa#aeaeaeaea#aaaaaaab#",
".aaa#a#a#a#a#a#aaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};


static const char* const image16_data[] = { 
"25 22 5 1",
". c #000000",
"c c #800000",
"a c #808080",
"b c #c0c0c0",
"# c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".ab#b#c#b#bcb#b#c#b#bcbb#",
".a#b#bcb#b#c#b#bcb#b#c#b#",
".ab#b#c#b#bcb#b#c#b#bcbb#",
".a#b#bcb#b#c#b#bcb#b#c#b#",
".accccccc#ccccbcccc#cccb#",
".a#b#bcb#b#c#b#bcb#b#c#b#",
".ab#b#c#b#bcb#b#c#b#bcbb#",
".a#b#b#b#b#b#b#b#b#b#b#b#",
".ab#b#c#b#bcb#b#c#b#bcbb#",
".acccccccbcccc#ccccbcccb#",
".ab#b#c#b#bcb#b#c#b#bcbb#",
".a#b#bcb#b#c#b#bcb#b#c#b#",
".ab#b#b#b#b#b#b#b#b#b#bb#",
".a#b#bcb#b#c#b#bcb#b#c#b#",
".accccccc#ccccbcccc#cccb#",
".a#b#bcb#b#c#b#bcb#b#c#b#",
".ab#b#c#b#bcb#b#c#b#bcbb#",
".a#b#bcb#b#c#b#bcb#b#c#b#",
".abbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image17_data[] = { 
"25 22 5 1",
". c #000000",
"c c #0000ff",
"a c #808080",
"b c #c0c0c0",
"# c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".abbbbbbbbbbbbbbbbbbbcbb#",
".abbbbbbbbbbbbbbbbbbcbbb#",
".abbbbbbbbbbbbbbbbbcbbbb#",
".abbbbbbbbbbbbbbcbcbcbbb#",
".abbbbbbbbbbbbbcbcbbbbbb#",
".abbbbbbbbbbcbcbcbbbbbbb#",
".abbbbbbbbbcbcbbbbbbbbbb#",
".abbbbbbcbcbcbcbbbbbbbbb#",
".abbbcbcbcbcbbbbbbbbbbbb#",
".abbcbcbcbcbcbbbbbbbbbbb#",
".abcbcbcbcbcbcbbbbbbbbbb#",
".acbcbcbbbbbcbcbcbbbbbbb#",
".abcbcbbbbbbbcbcbbbbbbbb#",
".acbcbbbbbbbbbbbcbbbbbbb#",
".abbbbbbbbbbbbbbbcbbbbbb#",
".abbbbbbbbbbbbbbbbcbcbbb#",
".abbbbbbbbbbbbbbbbbcbcbb#",
".abbbbbbbbbbbbbbbbbbcbcb#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image18_data[] = { 
"25 22 7 1",
"# c #000000",
"e c #800000",
"c c #802b00",
"b c #808080",
"a c #c0c0c0",
"d c #ff8055",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaacaaaaaaaaaaaab#",
".aaaaaaaacadaaaaaaaaaaab#",
".aaaaaaaaacaaaaaaaaaaaab#",
".aaaaaaaaaacadaaaaaaaaab#",
".aaaaadaaacacacaaaaaaaab#",
".aaaaaadaeaaaaacaaaaaaab#",
".aaaaaaadacacaaadaaaaaab#",
".aaaaaaaaaacacaeaaaaaaab#",
".aaaaaaaaadacaaaaaaaaaab#",
".aaaaaaaadacacaaaaaaaaab#",
".aaaaaaadacaeacaaaaaaaab#",
".aaaaaaaacaaaeadaaaaaaab#",
".aaaaaaaaacaaaeadaaaaaab#",
".aaaaaaaaadeaaaeadaaaaab#",
".aaaaaaaaaaadaaaeaaaaaab#",
".aaaaaaaaaadaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};


static const char* const image21_data[] = { 
"25 22 8 1",
"# c #000000",
"f c #800000",
"d c #802b00",
"b c #808080",
"a c #c0c0c0",
"c c #ff0000",
"e c #ff8055",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".acaaaaaaaaaaaaaaaaacacb#",
".aacaaaaadaeaaaaaaacacab#",
".aaacacaaadaaaaaaacacaab#",
".aaaacacadaeaaaaacacaaab#",
".aaaaacacadadaaacacaaaab#",
".aaaaaacacadadacacaaaaab#",
".aaaaaaacacadacaeaaaaaab#",
".aaaaaaaacacacadaaaaaaab#",
".aaaaaaaaacacaaaeaaaaaab#",
".aaaaaaaaaacacaaaaaaaaab#",
".aaaaaaaaacadacaaaaaaaab#",
".aaaaaaaacadafacaaaaaaab#",
".aaaaacacadaaafacaaaaaab#",
".aaaacacaeadaaafacacaaab#",
".aaacacaaaeafaaafacacaab#",
".aacacaaaaafaaaaafacacab#",
".acacaaaaaaaaaaafaaacacb#",
".aacaaaaaaaaaaaaaaaaacab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

/*
 *  Constructs a QCrisp as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
QCrisp::QCrisp( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl ),
      image0( (const char **) image0_data ),
      image1( (const char **) image1_data ),
      image2( (const char **) image2_data ),
      image3( (const char **) image3_data ),
      image4( (const char **) image4_data ),
      image5( (const char **) image5_data ),
      image6( (const char **) image6_data ),
      image7( (const char **) image7_data ),
      image8( (const char **) image8_data ),
      image10( (const char **) image10_data ),
	  image11( (const char **) image11_data ),
	  image14( (const char **) image14_data ),
	  image16( (const char **) image16_data ),
	  image17( (const char **) image17_data ),
	  image18( (const char **) image18_data ),
	  image21( (const char **) image21_data )

{
    myStatus = new QCStatusBar(this, "crisp status");


	setCaption( "COMPASS Map" );
	setIcon( image11 );

    if ( !name )
	setName( "COMPASS Map" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 21, 50 ) );
    setMaximumSize( QSize( 32767, 32767 ) );
    setMouseTracking( TRUE );
    setFocusPolicy( QMainWindow::ClickFocus );
    setUsesBigPixmaps( TRUE );
    setUsesTextLabel( FALSE );

    // actions
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( image8 ) );    
    fileSaveAsAction 
		= new QAction( this, "fileSaveAsAction" );
    filePrint_MapAction = new QAction( this, "filePrint_MapAction" );
    filePrint_MapAction->setIconSet( QIconSet( image10 ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    fileMouse_ToolAction = new QAction( this, "fileMouse_ToolAction" );
    fileMessage_LogAction = new QAction( this, "fileMessage_LogAction" );
    fileAction = new QAction( this, "fileAction" );
    fileMessage_Lognew_itemAction = new QAction( this, "fileMessage_Lognew_itemAction" );
    filenew_itemAction = new QAction( this, "filenew_itemAction" );
    viewUnZoomAction = new QAction( this, "viewUnZoomAction" );
    viewGridAction = new QAction( this, "viewGridAction" );
    viewMapAction = new QAction( this, "viewMapAction" );
	//Set to toggles
	viewMapAction->setToggleAction(true);
	viewGridAction->setToggleAction(true);
	viewMapAction->setOn(true);
	viewGridAction->setOn(true);

    helpContentsAction = new QAction( this, "helpContentsAction" );
    helpIntroductionAction = new QAction( this, "helpIntroductionAction" );
    helpGetting_StartedAction = new QAction( this, "helpGetting_StartedAction" );
    helpKeyword_GlossaryAction = new QAction( this, "helpKeyword_GlossaryAction" );
    helpParameter_GlossaryAction = new QAction( this, "helpParameter_GlossaryAction" );
    helpTechnical_DetailsAction = new QAction( this, "helpTechnical_DetailsAction" );
    helpAbout_CRiSP1Action = new QAction( this, "helpAbout_CRiSP1Action" );
    releaseNewAction = new QAction( this, "releaseNewAction" );
    releaseNewnew_itemAction = new QAction( this, "releaseNewnew_itemAction" );
    releaseRelease_ToolAction = new QAction( this, "releaseRelease_ToolAction" );

	runLock_Output_SettingsAction = new QAction( this, "runLock_Output_SettingsAction" );	
	runLock_Output_SettingsAction->setToggleAction(true);
	runRun_ScenarioAction = new QAction( this, "runRun_ScenarioAction" );
    runCancel_RunAction = new QAction( this, "runCancel_RunAction" );

   
    // toolbars
    Toolbar = new QToolBar( QString(""), this, DockTop ); 

    Toolbar->setFocusPolicy( QToolBar::TabFocus );
    Toolbar->setFrameShape( QToolBar::ToolBarPanel );
    Toolbar->setFrameShadow( QToolBar::Raised );
    Toolbar->setResizeEnabled( FALSE );
    Toolbar->setMovingEnabled( TRUE );
    Toolbar->setHorizontallyStretchable( FALSE );
    Toolbar->setVerticallyStretchable( FALSE );
    QSpacerItem* spacer = new QSpacerItem( 10, 22, QSizePolicy::Expanding, QSizePolicy::Minimum );
    QApplication::sendPostedEvents( Toolbar, QEvent::ChildInserted );
    Toolbar->boxLayout()->addItem( spacer );

    OpenButton = new QToolButton( Toolbar, "OpenButton" );
    OpenButton->setMinimumSize( QSize( 25, 22 ) );
    OpenButton->setMaximumSize( QSize( 25, 22 ) );

	QIconSet iconset = QIconSet();
	iconset.setPixmap(image0, QIconSet::Automatic,  QIconSet::Normal);
	iconset.setPixmap(image14, QIconSet::Automatic,  QIconSet::Disabled, QIconSet::Off);
	OpenButton->setIconSet( iconset); //image0 ) );
    OpenButton->setUsesBigPixmap( TRUE );

    SaveButton = new QToolButton( Toolbar, "SaveButton" );
    SaveButton->setMinimumSize( QSize( 25, 22 ) );
    SaveButton->setMaximumSize( QSize( 25, 22 ) );
    SaveButton->setIconSet( QIconSet( image1 ) );
    SaveButton->setUsesBigPixmap( TRUE );

    PrintButton = new QToolButton( Toolbar, "PrintButton" );
    PrintButton->setMinimumSize( QSize( 25, 22 ) );
    PrintButton->setMaximumSize( QSize( 25, 22 ) );
    PrintButton->setIconSet( QIconSet( image2 ) );
    PrintButton->setUsesBigPixmap( TRUE );
    QSpacerItem* spacer_2 = new QSpacerItem( 10, 22, QSizePolicy::Expanding, QSizePolicy::Minimum );
    QApplication::sendPostedEvents( Toolbar, QEvent::ChildInserted );
    Toolbar->boxLayout()->addItem( spacer_2 );

		
    MapToggle = new QToolButton( Toolbar, "MapToggle" );
    MapToggle->setMinimumSize( QSize( 25, 22 ) );
    MapToggle->setMaximumSize( QSize( 25, 22 ) );
    MapToggle->setToggleButton( TRUE );
    MapToggle->setOn( TRUE );
	QIconSet iconsetMaptoggle = QIconSet();
	iconsetMaptoggle.setPixmap(image3, QIconSet::Automatic,  QIconSet::Normal, QIconSet::Off);
	iconsetMaptoggle.setPixmap(image17, QIconSet::Automatic,  QIconSet::Normal, QIconSet::On);
	MapToggle->setIconSet( iconsetMaptoggle); //image0 ) );

    GridShow = new QToolButton( Toolbar, "GridShow" );
    GridShow->setMinimumSize( QSize( 25, 22 ) );
    GridShow->setMaximumSize( QSize( 25, 22 ) );
    GridShow->setToggleButton( TRUE );
    GridShow->setOn( TRUE );
    GridShow->setAutoRepeat( TRUE );
	QIconSet iconsetGrid = QIconSet();
	iconsetGrid.setPixmap(image4, QIconSet::Automatic,  QIconSet::Normal, QIconSet::Off);
	iconsetGrid.setPixmap(image16, QIconSet::Automatic,  QIconSet::Normal, QIconSet::On);
	GridShow->setIconSet( iconsetGrid); //image0 ) );    
    QSpacerItem* spacer_3 = new QSpacerItem( 10, 22, QSizePolicy::Expanding, QSizePolicy::Minimum );
    QApplication::sendPostedEvents( Toolbar, QEvent::ChildInserted );
    Toolbar->boxLayout()->addItem( spacer_3 );
	
	//Syncronize the menu and the toolbar options
	connect( viewMapAction, SIGNAL(activated  (  )), MapToggle, SLOT( toggle()));
	connect( viewGridAction, SIGNAL(activated  (  )), GridShow, SLOT( toggle()));
	connect( MapToggle, SIGNAL(clicked ()), viewMapAction, SLOT(toggle ()));
	connect( GridShow, SIGNAL(clicked ()), viewGridAction, SLOT(toggle ()));

    UnZoomButton = new QToolButton( Toolbar, "UnZoomButton" );
	UnZoomButton-> setAutoRaise(FALSE);

    QSpacerItem* spacer_4 = new QSpacerItem( 10, 22, QSizePolicy::Expanding, QSizePolicy::Minimum );
    QApplication::sendPostedEvents( Toolbar, QEvent::ChildInserted );
    Toolbar->boxLayout()->addItem( spacer_4 );

    RunScenarioButton = new QToolButton( Toolbar, "RunScenarioButton" );
    RunScenarioButton->setMinimumSize( QSize( 25, 22 ) );
    RunScenarioButton->setMaximumSize( QSize( 25, 22 ) );
	QIconSet iconsetRun = QIconSet();
	iconsetRun.setPixmap(image5, QIconSet::Automatic,  QIconSet::Normal);
	iconsetRun.setPixmap(image18, QIconSet::Automatic,  QIconSet::Disabled);
	RunScenarioButton->setIconSet( iconsetRun); //image0 ) );        
    QSpacerItem* spacer_5 = new QSpacerItem( 10, 22, QSizePolicy::Expanding, QSizePolicy::Minimum );
    QApplication::sendPostedEvents( Toolbar, QEvent::ChildInserted );
    Toolbar->boxLayout()->addItem( spacer_5 );

    CancelRunButton = new QToolButton( Toolbar, "CancelRunButton" );
    CancelRunButton->setMaximumSize( QSize( 25, 22 ) );
	CancelRunButton->setDisabled( TRUE );	
	QIconSet iconsetRunCan = QIconSet();
	iconsetRunCan.setPixmap(image6, QIconSet::Automatic,  QIconSet::Normal);
	iconsetRunCan.setPixmap(image21, QIconSet::Automatic,  QIconSet::Disabled);
	CancelRunButton->setIconSet( iconsetRunCan);     
    QSpacerItem* spacer_6 = new QSpacerItem( 10, 22, QSizePolicy::Expanding, QSizePolicy::Minimum );
    QApplication::sendPostedEvents( Toolbar, QEvent::ChildInserted );
    Toolbar->boxLayout()->addItem( spacer_6 );

    MouseToolButton = new QToolButton( Toolbar, "MouseToolButton" );
    MouseToolButton->setMaximumSize( QSize( 25, 22 ) );
    MouseToolButton->setIconSet( QIconSet( image7 ) );


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );

    MenuBar->setGeometry( QRect( 0, 0, 526, 29 ) );

    fileMenu = new QPopupMenu( this );
    fileOpenAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrint_MapAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileMouse_ToolAction->addTo( fileMenu );
    fileMessage_LogAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 2 );

    ViewMenu = new QPopupMenu( this );
    viewGridAction->addTo( ViewMenu );
    viewUnZoomAction->addTo( ViewMenu );
    viewMapAction->addTo( ViewMenu );
    MenuBar->insertItem( QString(""), ViewMenu, 3 );

    ReleaseMenu = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), ReleaseMenu, 4 );

    ReservoirMenu = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), ReservoirMenu, 5 );

    FlowMenu = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), FlowMenu, 6 );

    DamMenu = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), DamMenu, 7 );

    PassageMenu = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), PassageMenu, 8 );

    RunMenu = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), RunMenu, 9 );

    AnalysisMenu = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), AnalysisMenu, 10 );

    helpMenu = new QPopupMenu( this );
	helpContentsAction->addTo( helpMenu );
    helpIntroductionAction->addTo( helpMenu );
    helpGetting_StartedAction->addTo( helpMenu );
    helpKeyword_GlossaryAction->addTo( helpMenu );
    helpParameter_GlossaryAction->addTo( helpMenu );
    helpTechnical_DetailsAction->addTo( helpMenu );
    helpAbout_CRiSP1Action->addTo( helpMenu );
    MenuBar->insertItem( QString(""), helpMenu, 11 );

    languageChange();
    resize( QSize(800, 600).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
	
	//Initialize singletons
	qMouseTool *mouseTool = qMouseTool::instance(this);
	medBuilder_ = qMediatorBuilder::instance(this);

	//Build the map screen
	theMapPane_ = new qMapPane( this  );
	setCentralWidget(theMapPane_);
	theMapPane_->show();

	//Make new map controller
	mapController_ = new QmapController("DFLT_RIVER_DESC");

	//Mediatate the map and add sub mediators
	mapMed_ = new qMapMedCrisp1(theMapPane_, mapController_);	
	mapMed_->insertSubMediator(new qMapMedStatusBarUpdator(theMapPane_, myStatus, "main window statbar"));
	mapMed_->insertSubMediator(new qMapMedDialogBuilder(theMapPane_,mouseTool, "main map mediator"));
	mapMed_->insertSubMediator(new qMapMedGridLines(theMapPane_, "main gridlines"));

    // signals and slots connections
	
    connect( fileOpenAction, SIGNAL( activated() ), &fileManager, SLOT( fileOpen() ) );
    connect( &fileManager, SIGNAL( fileOpened() ), this, SLOT( fileOpened() ) );
    connect( fileSaveAsAction, SIGNAL( activated() ), &fileManager, SLOT( fileSaveAs() ) );
    connect( filePrint_MapAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    
	
	connect( helpContentsAction, SIGNAL( activated() ), this, SLOT( helpContents() ) );
	connect( helpIntroductionAction, SIGNAL( activated() ), this, SLOT( helpIntro() ) );
    connect( helpGetting_StartedAction, SIGNAL( activated() ), SLOT( helpGetting_Started()));
    connect( helpKeyword_GlossaryAction, SIGNAL( activated() ), SLOT( helpKeyword_Glossary()));
    connect( helpParameter_GlossaryAction, SIGNAL( activated() ), SLOT( helpParameter_Glossary()));
    connect( helpTechnical_DetailsAction, SIGNAL( activated() ), SLOT( helpTechnical_Details()));
    connect( helpAbout_CRiSP1Action, SIGNAL( activated() ), SLOT( helpAbout()));

	connect( fileMessage_LogAction, SIGNAL( activated() ), this, SLOT( messageLog() ) );
	connect( UnZoomButton, SIGNAL( clicked() ), this, SLOT( unZoom() ) );
	connect( viewUnZoomAction, SIGNAL( activated() ), this, SLOT( unZoom() ) );
	connect( viewGridAction, SIGNAL( activated() ), this, SLOT( gridShow() ) );
	connect( GridShow, SIGNAL( clicked() ), this, SLOT( gridShow() ) );
	connect( viewMapAction, SIGNAL(activated() ), this, SLOT( mapShow() ) );
	connect( MapToggle, SIGNAL( clicked() ), this, SLOT( mapShow() ) );
	connect( fileMouse_ToolAction, SIGNAL(activated()), this, SLOT(mouseTool()));
	connect( MouseToolButton, SIGNAL( clicked() ), this, SLOT( mouseTool()));
	connect( releaseRelease_ToolAction, SIGNAL( activated()), this, SLOT( ReleaseTool()));
	connect( RunScenarioButton, SIGNAL( clicked()), &scenarioManager, SLOT(runScenario()));
	connect( runRun_ScenarioAction, SIGNAL( activated()), &scenarioManager, SLOT(runScenario()));
	connect( OpenButton, SIGNAL( clicked()), &fileManager, SLOT( fileOpen()));
	connect( runCancel_RunAction,  SIGNAL( activated()), &scenarioManager, SLOT(runCancel()));
	connect( CancelRunButton,  SIGNAL( clicked()), &scenarioManager, SLOT(runCancel()));
	connect( PrintButton, SIGNAL( clicked()), this, SLOT( filePrint()));
	connect( SaveButton, SIGNAL( clicked()), &fileManager, SLOT( fileSaveAs()));
	connect( runLock_Output_SettingsAction, SIGNAL(toggled(bool )), this , SLOT(LockOutputSett(bool )));
	

	runCancel_RunAction->setEnabled(FALSE);
	run_mode.running = FALSE;
//	GERunStatusChanged_.attach( this, &C1Main::_cbRunStatusChanged );
	connect(qEvents::Instance(), SIGNAL(GERunStatusChanged_()), this, SLOT(cbRunStatusChanged()));

	/* This code was originally in QCrisp::ModelStartup()
	 * It should work here since the model startup code should have just run when QCrisp is created */
    GOutputsValid_[0] = 1;  /* flow, gas, etc, are now valid */
	//We use data in the model to build the mapObjects in the mapController
	//So we read the map after the model is done running
	mapController_->ReadMap();
    // Move the map to show the river.
	if (!BATCH_MODE_ACTIVE) {
        mapMed_->setBoundingBox(
            mapController_->minLon() - 1,
            mapController_->minLat() - 1,
            mapController_->maxLon() + 1,
            mapController_->maxLat() + 1 );
    }
    //Calling size() sets the map pane as dirty so that it will redraw the map.
	theMapPane_->repaint();

	/* Fill in the menu (optionally the full GUI menu) */
	SetUpMenu(user_args.full_gui);
}

void QCrisp::SetStatusLine(char * text ){
	
	/* Need to lock GUI thread since the running thread in C code shouldn't interfere with the GUI here */
		qApp->lock();

		myStatus->setMessage(text);

		qApp->unlock();



}

void QCrisp::cbRunStatusChanged( void ){

	fileOpenAction->setEnabled(!run_mode.running);
	runRun_ScenarioAction->setEnabled(!run_mode.running);;
    runCancel_RunAction->setEnabled(run_mode.running);;

	RunScenarioButton->setEnabled(!run_mode.running);	
	CancelRunButton->setEnabled(run_mode.running);	
	OpenButton->setEnabled(!run_mode.running);	

	if (!run_mode.running) myStatus->setMessage("Done.");
}

void QCrisp::messageLog(){
	//create a new mesage log using auto create
	QLogWindow *logw = QLogWindow::instance(TRUE);
	logw->show();
}

void QCrisp::mouseTool(){
	
	qMouseTool::instance(this)->show();

}

void QCrisp::ReleaseTool(){


	if (theReleaseTool_ != NULL)
		theReleaseTool_->setShown( TRUE);
	else
		theReleaseTool_ = new qReleaseTool( this );
	//log_msg(0, (char*)QString::number((int)theReleaseTool_).ascii());


}

void QCrisp::fileExit(){
	
	close();
}

void QCrisp::filePrint(){
	//Print the map
	theMapPane_->printMap();
		
}


void QCrisp::helpContents(){
	helpManager->showHelp(QString("1.html") );		
}

void QCrisp::helpIntro(){
	helpManager->showHelp(QString("6.html") );		
}

void QCrisp::helpAbout(){
	helpManager->about();
}

void QCrisp::helpGetting_Started(){
	helpManager->showHelp( QString("2.html") );
}

void QCrisp::helpKeyword_Glossary(){
	helpManager->showHelp( QString("3.html") );
}

void QCrisp::helpParameter_Glossary(){
	helpManager->showHelp( QString("4.html") );
}

void QCrisp::helpTechnical_Details(){
	helpManager->showHelp( QString("5.html") );
}

void QCrisp::unZoom(){
	QRect r;

	r = theMapPane_->rect();
	if(r.right() == 0 || r.bottom() == 0) return;
	mapCordset *pmc = mapMed_->popCoordset();
	if (pmc != NULL)
		delete pmc;
	theMapPane_->repaint();

}

void QCrisp::gridShow(){
	
	if(GridShow->isOn()){
		mapMed_->insertSubMediator(new qMapMedGridLines(theMapPane_, "GridLine Mediator"));
		theMapPane_->repaint();
	}
	else {
		mapMed_->removeSubMediator(MMT_GRIDLINES);
		theMapPane_->repaint();
	}

}

void QCrisp::mapShow(){
	
    /* If we are revealing the map, reveal the status line
     *  and resize the app's main frame to a 4/3 aspect ratio.
     * If we are hiding the map, hide the status line and 
     *  resize the app's main frame to just show the toolbar.
     */
	QRect rx;
    rx = geometry();
	//getExterior( rx );
    if (!MapToggle->isOn()) {
        //setGeometry ( rx.left(), rx.top(), rx.width(),  27 );
		rm = theMapPane_->geometry().normalize();
		rm.setHeight(rm.height() + statusBar()->height());
		//set new size
		resize ( rx.width(),  rx.height()-rm.height()  );
		//MenuBar.heightForWidth(rx.width())
		theMapPane_->clearFocus ();
		myStatus->hide();
		theMapPane_->hide();
	
    }
    else {	   
		theMapPane_->show();	
		myStatus->show();	
		resize ( rx.width(),  rm.height() + rx.height() -3 );			
    }
	

}

/*
 *  Destroys the object and frees any allocated resources
 */
QCrisp::~QCrisp()
{
	
	model_cleanup();
	reset_all_spill();
	reset_all_dams();
	reset_all_releases();

	
	delete mapMed_;
	delete mapController_;
	delete myStatus;
	delete medBuilder_;
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QCrisp::languageChange()
{
	
    setCaption( tr( "COMPASS Map" ) );
    QToolTip::add( this, QString::null );
    fileOpenAction->setText( tr( "Open" ) );
    fileOpenAction->setMenuText( tr( "&Open..." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );   
    fileSaveAsAction->setText( tr( "Save As" ) );
    fileSaveAsAction->setMenuText( tr( "Save &As..." ) );
    fileSaveAsAction->setAccel( QString::null );
    filePrint_MapAction->setText( tr( "&Print Map..." ) );
    filePrint_MapAction->setMenuText( tr( "&Print Map..." ) );
    filePrint_MapAction->setAccel( tr( "Ctrl+P" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    fileExitAction->setAccel( QString::null );
    helpContentsAction->setText( tr( "Contents" ) );
    helpContentsAction->setMenuText( tr( "&Contents..." ) );
    helpContentsAction->setAccel( QString::null );
    fileMouse_ToolAction->setText( tr( "Mouse Tool" ) );
    fileMessage_LogAction->setText( tr( "Message Log" ) );
    fileAction->setText( QString::null );
    fileMessage_Lognew_itemAction->setText( tr( "new item" ) );
    filenew_itemAction->setText( tr( "new item" ) );
    viewUnZoomAction->setText( tr( "UnZoom" ) );
    viewGridAction->setText( tr( "Grid" ) );
    viewMapAction->setText( tr( "Map" ) );
    helpContentsAction->setText( tr( "Contents" ) );
    helpIntroductionAction->setText( tr( "Introduction" ) );
    helpGetting_StartedAction->setText( tr( "Getting Started" ) );
    helpKeyword_GlossaryAction->setText( tr( "Keyword Glossary" ) );
    helpParameter_GlossaryAction->setText( tr( "Parameter Glossary" ) );
    helpTechnical_DetailsAction->setText( tr( "Technical Details" ) );
    helpAbout_CRiSP1Action->setText( tr( "About COMPASS" ) );
    releaseNewAction->setText( tr( "New" ) );
    releaseNewnew_itemAction->setText( tr( "new item" ) );
    releaseRelease_ToolAction->setText( tr( "Release Tool..." ) );
	runLock_Output_SettingsAction->setText( tr( "Lock Output Settings" ) );
	runRun_ScenarioAction->setText( tr( "Run Scenario" ) );
    runCancel_RunAction->setText( tr( "Cancel Run" ) );
    Toolbar->setLabel( tr( "Toolbar" ) );
    OpenButton->setText( QString::null );
    QToolTip::add( OpenButton, tr( "Open New Dat file" ) );
    SaveButton->setText( QString::null );
    QToolTip::add( SaveButton, tr( "Save As Dat File" ) );
    PrintButton->setText( QString::null );
    QToolTip::add( PrintButton, tr( "Print Map" ) );
    MapToggle->setText( QString::null );
    QToolTip::add( MapToggle, tr( "Enables/Disables Map" ) );
    GridShow->setText( QString::null );
    QToolTip::add( GridShow, tr( "Enable Map Grid" ) );
    UnZoomButton->setText( tr( "UnZoom" ) );
    QToolTip::add( UnZoomButton, tr( "Fully Zooms Out Map" ) );
    RunScenarioButton->setText( QString::null );
    QToolTip::add( RunScenarioButton, tr( "Run Scenario" ) );
    CancelRunButton->setText( QString::null );
    QToolTip::add( CancelRunButton, tr( "Cancel Run" ) );
    MouseToolButton->setText( QString::null );
    QToolTip::add( MouseToolButton, tr( "Configure Mouse Tool" ) );
    MenuBar->findItem( 2 )->setText( tr( "&File" ) );
    MenuBar->findItem( 3 )->setText( tr( "&View" ) );
    MenuBar->findItem( 4 )->setText( tr( "&Release" ) );
    MenuBar->findItem( 5 )->setText( tr( "R&eservoir" ) );
    MenuBar->findItem( 6 )->setText( tr( "F&low" ) );
    MenuBar->findItem( 7 )->setText( tr( "&Dam" ) );
    MenuBar->findItem( 8 )->setText( tr( "&Passage" ) );
    MenuBar->findItem( 9 )->setText( tr( "R&un" ) );
    MenuBar->findItem( 10 )->setText( tr( "&Analysis" ) );
    MenuBar->findItem( 11 )->setText( tr( "&Help" ) );
	
}

/* These global flags are examined by all output graphs. They indicate whether
 *  the output side of the data base is the result of a valid calculation
 *  from the current input side (value 1) or if the results have not been
 *  recalculated from the current inputs (value 0) (e.g. because current
 *  inputs have been edited). There are several flags because there are
 *  several classes of output (currently two: the passage and everything else).
 *  Each output graph is assigned to one output class by the virtual function
 *  qGraphDataToken::outputType()
 */
/* [0] = most things; [1] = passage */

 int GOutputsValid_[2] = {0, 0};
 int nGOutputsValid_ = 2;

//Event handling stuff
 
void QCrisp::PurgeAllIDCs( void  ) {
	qEvents::Instance()->emitGEPurgeAllIDCs_();
}

void QCrisp::UpdateAllDataTokens( void ) {
	qEvents::Instance()->emitGEUpdateAllDataTokens_();
}

void QCrisp::RunStatusChanged( void ) {
	qEvents::Instance()->emitGERunStatusChanged_();
}

void QCrisp::updatedOutputLocking( void ) {
	runLock_Output_SettingsAction->setOn(  lock_output_settings);
}

/* This routine is called exactly once, at application startup, after 
 *  the river description file has been read in. This routine creates the 
 *  application's main menu dynamically and assigns menu item values which 
 *  cannot be assigned at compile time, because they depend, for instance, 
 *  on the number of reach segments in the model or the number of dams.
 * NOTE: the "full gui" option shows all possible menu items. The
 *  "reduced gui" option does not show those which manipulate calibrated
 *  parameters.
 */

void QCrisp::SetUpMenu( int full_gui )
{
	QPopupMenu *newMenu, *newSubMenu, *newSubSubMenu;
    int idStart = ID_DYNAMIC_BASE;  /* Next dynamically allocated menu id. */

	/* Construct the existing releases menu. Notice the pathological data
	 *  connection to the release tool.
	 * NOTE This call must come last because the number of releases varies,
	 *  and to get the "menu()->update()" call to happen after all other
	 *  menu items are added.
	 */

	idReleaseAddBase = idStart;
	ReleaseMenu->insertItem( "&New", CreateReleaseSiteMenu( this, &idStart )  ); 
	releaseRelease_ToolAction->addTo( ReleaseMenu );

	nSites = idStart - idReleaseAddBase;

	/* Construct the "Resevoir" top level menu. */
	if (full_gui) {
		ReservoirMenu->insertItem("&Reach Predator Density...",  this, SLOT(ReachPredatorDensity()), 0,ID_REACHPREDDEN);
		ReservoirMenu->insertItem("&Predator Distribution Coefficient...",  this, SLOT(PredDistributionCoef()),0,ID_PREDDISTCOEF );
		ReservoirMenu->insertItem("Reach &Gas Theta...",  this, SLOT(ReachGasTheta()) ,0, ID_REACH_GAS_THETA);
		ReservoirMenu->insertItem("Gas D&istribution",  this, SLOT(GasDistribution()),0, ID_GASDISTRIBUTION );
	}

	newSubMenu = new QPopupMenu();
	idReservoirNSatDamBase = idStart;
	newSubMenu->insertItem( "&Dam", CreateRiverSegmentMenu( &idStart, DAM_SEG ) );
	nDams = idStart - idReservoirNSatDamBase;
	idReservoirNSatReachBase = idStart;
	newSubMenu->insertItem(  "&Reach", CreateRiverSegmentMenu( &idStart, REACH_SEG ) );
	nReaches = idStart - idReservoirNSatReachBase;
	ReservoirMenu->insertItem(  "&TDG Saturation > 100%", newSubMenu );

	newSubMenu = new QPopupMenu();
	idReservoirDamWaterTempBase = idStart;
	newSubMenu->insertItem( "&Dam", CreateRiverSegmentMenu( &idStart, DAM_SEG ) );
	nDams = idStart - idReservoirDamWaterTempBase;
	idReservoirReachWaterTempBase = idStart;
	newSubMenu->insertItem( "&Reach", CreateRiverSegmentMenu( &idStart, REACH_SEG ) );
	nReaches = idStart - idReservoirReachWaterTempBase;

	idReservoirHeadwaterTempBase = idStart;
	newSubMenu->insertItem( "&Headwater", CreateRiverSegmentMenu( &idStart, HEADWATER_SEG ) );
	nHeadwaters = idStart - idReservoirHeadwaterTempBase;
	ReservoirMenu->insertItem(  "&Water Temperature" , newSubMenu);
	/* turbidity menu item  */
	
	newSubMenu = new QPopupMenu();
	idReservoirDamTurbidityBase = idStart;
	newSubMenu->insertItem( "&Dam", CreateRiverSegmentMenu( &idStart, DAM_SEG ) );
	nDams = idStart - idReservoirDamTurbidityBase;
	idReservoirReachTurbidityBase = idStart;
	newSubMenu->insertItem("&Reach", CreateRiverSegmentMenu( &idStart, REACH_SEG ));
	nReaches = idStart - idReservoirReachTurbidityBase;
	idReservoirHeadwaterTurbidityBase = idStart;
	newSubMenu->insertItem("&Headwater", CreateRiverSegmentMenu( &idStart, HEADWATER_SEG ));
	nHeadwaters = idStart - idReservoirHeadwaterTurbidityBase;
	ReservoirMenu->insertItem( "&Turbidity" , newSubMenu );
	/* turbidity end */
	
	idReservoirVelocityBase = idStart;
	ReservoirMenu->insertItem("&Velocity", CreateRiverSegmentMenu( &idStart, REACH_SEG ));
	if (full_gui) {
		ReservoirMenu->insertItem( "R&iver Parameters...", this, SLOT(RiverParameters()),0, ID_RIVERPARAMETERS);
		ReservoirMenu->insertItem( "Reach &Classes...",this, SLOT(ReachClasses()),0, ID_REACH_CLASSES);
		ReservoirMenu->insertItem( "Reach Class &Names...",this, SLOT(ReachClassNames()),0, ID_REACH_CLASS_NAMES);
	}
	//menu()->appendDropDown( newMenu, "&Reservoir" );

	/* Construct the "Behavior" top level menu. */
	if (full_gui) {
		newMenu = new QPopupMenu();
		newSubMenu = new QPopupMenu();
		newSubSubMenu = new QPopupMenu();
		newSubSubMenu->insertItem("&Activity...",this, SLOT(PredCoefReach()),0, ID_PREDATIONCOEF_REACH  );
		newSubSubMenu->insertItem("&Time and Dist...",this, SLOT(PredMortCoef()),0, ID_PREDMORTCOEF  );
		newSubMenu->insertItem("&Reach" , newSubSubMenu );
		newSubMenu->insertItem("&Forebay...",this, SLOT(PredCoefForebay()),0, ID_PREDATIONCOEF_FOREBAY  );
		newSubMenu->insertItem("&Tailrace...",this, SLOT(PredationCoefTailrace()),0,  ID_PREDATIONCOEF_TAILRACE );
		newMenu->insertItem("&Predation Coef", newSubMenu );
		newMenu->insertItem("&VVar...",this, SLOT(VVar()),0, ID_VVAR );
		newMenu->insertItem("Migration &Rate Variance...",this, SLOT(MigrationRateVar()),0, ID_MIGRATIONRATEVAR );
		newMenu->insertItem("Predation &Temperature Response Equation...",this, SLOT(PredTempEquation()),0, ID_PREDTEMPEQUATION);
		newMenu->insertItem("&Migration Equation...",this, SLOT(MigrationEquation()),0, ID_MIGRATIONEQUATION);
		newMenu->insertItem("&Gas Mortality Equation...",this, SLOT(GasMortEquation()),0, ID_GASMORTEQUATION);
		MenuBar->insertItem("&Behavior", newMenu, -1, 4  );
	}

	/* Construct the "Flow" top level menu. */
	idFlowDamsBase = idStart;
	FlowMenu->insertItem(  "&Dams", CreateRiverSegmentMenu( &idStart, DAM_SEG ) );
	idFlowReachesBase = idStart;
	FlowMenu->insertItem(  "&Reaches", CreateRiverSegmentMenu( &idStart, REACH_SEG ) );
	newSubMenu = new QPopupMenu();
	idFlowHeadwatersBase = idStart;
	newSubMenu->insertItem( "&Flows", CreateRiverSegmentMenu( &idStart, HEADWATER_SEG ) );
	newSubMenu->insertItem( "&Unregulated Max...",this, SLOT(UnregMax()),0, ID_UNREGMAX );
	FlowMenu->insertItem( "&Headwaters" , newSubMenu);
	idFlowLossBase = idStart;
	FlowMenu->insertItem("&Loss" ,CreateRiverSegmentMenu( &idStart, REACH_SEG ));
	idFlowElevationBase = idStart;
	FlowMenu->insertItem("&Elevation", CreateRiverSegmentMenu( &idStart, REACH_SEG ) );
	idFlowReservoirsBase = idStart;
	FlowMenu->insertItem("Re&servoirs" , CreateRiverSegmentMenu( &idStart, DAM_SEG, 1 ));
	nBasins = idStart - idFlowReservoirsBase;
	FlowMenu->insertItem( "Headwater &Modulation...", this, SLOT(HeadWaterMod()),0, ID_HEADWATERMOD);
	
	/* Construct the "Dam" top level menu. */
	DamMenu->insertItem(  "&Transport...", this, SLOT(Transport()),0, ID_TRANSPORT );
	if (full_gui) {
		DamMenu->insertItem( "Transport &Mortality Equation...", this, SLOT(TransportMort()),0, ID_TRANSPORT_MORT);
		DamMenu->insertItem( "Spill &Efficiency...", this, SLOT(SpillEfficieny()),0,ID_SPILLEFFICIENCY );
	}
	idDamSpillBase = idStart;
	DamMenu->insertItem( "&Spill", CreateRiverSegmentMenu( &idStart, DAM_SEG ));
	
	nDams = idStart - idDamSpillBase;
	
	DamMenu->insertItem( "Sp&ill Schedule...", this, SLOT(SpillSchedule()),0, ID_SPILLSCHEDULE);
	if (full_gui) {
		DamMenu->insertItem("Spill &Cap...", this, SLOT(SpillCap()),0, ID_SPILLCAP);
		DamMenu->insertItem("Predator Densit&y...", this, SLOT(DamPredden()),0, ID_DAMPREDDEN );
		DamMenu->insertItem("Predation Pro&bability...", this, SLOT(PredProb()),0, ID_PREDATIONPROB);
		DamMenu->insertItem("&TDG Saturation Equations...", this, SLOT(NSatEquation()),0, ID_NSATEQUATION);
		DamMenu->insertItem("Dam Gas Theta...", this, SLOT(DamGasTheta()),0, ID_DAM_GAS_THETA);
		DamMenu->insertItem("K Entrain...", this, SLOT(KEntrain()),0, ID_K_ENTRAIN);
		DamMenu->insertItem("De&lay Equation...", this, SLOT(DelayEquation()),0, ID_DELAYEQUATION);
		DamMenu->insertItem("&Powerhouse Capacity...", this, SLOT(PHCapacity()),0, ID_PH_CAPACITY);
	}
	DamMenu->insertItem("P&owerhouse Schedule...", this, SLOT(PHSchedule()),0, ID_PH_SCHEDULE);
	DamMenu->insertItem("Po&werhouse Priority...", this, SLOT(PHPriority()),0, ID_PH_PRIORITY);
	DamMenu->insertItem("Powerho&use Minimum Flows...", this, SLOT(PHFlowMin()),0, ID_PH_FLOW_MIN);
	if (full_gui) {
		DamMenu->insertItem( "Mo&rtality..." , this, SLOT(Mortality()),0, ID_MORTALITY );
		DamMenu->insertItem("&Sluiceway...", this, SLOT(SluicewayProp()),0, ID_SLUICEWAYPROP );
		DamMenu->insertItem("&FGE...", this, SLOT(FGE()),0, ID_FGE );
		DamMenu->insertItem("F&GE Equation...", this, SLOT(FGEEquation()),0, ID_FGEEQUATION );
	}
	//menu()->insertItem( newMenu, "&Dam" );

	/* Construct the "Passage Histogram" top level menu. */

	newSubMenu = new QPopupMenu();
	idPassageDamBase = idStart;
	newSubMenu->insertItem("&Dam" , CreateRiverSegmentMenu( &idStart, DAM_SEG )	);
	idPassageReachBase = idStart;
	newSubMenu->insertItem("&Reach" , CreateRiverSegmentMenu( &idStart, REACH_SEG )	);
	PassageMenu->insertItem( "&Passage", newSubMenu );
	idPassageLengthBase = idStart;
	PassageMenu->insertItem("&Length", CreateRiverSegmentMenu( &idStart, REACH_SEG ) );
#ifdef ROUTE_TRACKING
	idPassageBypassBase = idStart;
	PassageMenu->insertItem("&Bypass Passage" ,CreateRiverSegmentMenu( &idStart, DAM_SEG ));
	idPassageSluicewayBase = idStart;
	PassageMenu->insertItem("S&luiceway Passage" ,CreateRiverSegmentMenu( &idStart, DAM_SEG ));
	idPassageTurbineBase = idStart;
	PassageMenu->insertItem("&Turbine Passage" ,CreateRiverSegmentMenu( &idStart, DAM_SEG )	);
	idPassageSpillwayBase = idStart;
	PassageMenu->insertItem("&Spillway Passage",CreateRiverSegmentMenu( &idStart, DAM_SEG ));
	idPassageRSWBase = idStart;
	PassageMenu->insertItem("&RSW Passage" ,CreateRiverSegmentMenu( &idStart, DAM_SEG ));
	idPassageTransportBase = idStart;
	PassageMenu->insertItem("T&ransport Passage" ,  CreateRiverSegmentMenu( &idStart, DAM_SEG ));
#endif
	//menu()->insertItem( newMenu, "&Passage" );

	/* Construct the "Run" top level menu. */
 	runRun_ScenarioAction->addTo(RunMenu);
    runCancel_RunAction->addTo(RunMenu);
	RunMenu->insertItem("&Monte Carlo", this, SLOT(Mon()),0,ID_MON  );
	RunMenu->insertSeparator();
	runLock_Output_SettingsAction->addTo(RunMenu);    
	RunMenu->insertItem("&Output Settings", this, SLOT(OutputSetting()),0,ID_DAM_OUTPUTSETTING );
	RunMenu->insertItem("&Runtime Settings...", this, SLOT(Settings()),0,ID_SETTINGS );

	/* Construct the "Analysis" top level menu. */

	AnalysisMenu->insertItem("&Input Data Report", this, SLOT(InputReport()),0,ID_INPUTREPORT );
	AnalysisMenu->insertItem("Mo&nte Data Report", this, SLOT(MonteReport()),0,ID_MONTEREPORT );
	AnalysisMenu->insertItem("Monte Alt. &Comparison", this, SLOT(MonteAltCompare()),0,ID_MONTEALTCOMPARE );
	AnalysisMenu->insertItem("Monte &Analysis", this, SLOT(MonteAnalysis()),0,ID_MONTEANALYSIS );	

	runLock_Output_SettingsAction->setOn(lock_output_settings);
}

/* These routines create sub-menus which contain an entry for each dam,
 *  each reach segment, each headwater, or each release site in the model,
 *  respectively.
 * Possible values for "type" are DAM_SEG, REACH_SEG, HEADWATER_SEG.
 * "flags" have special meaning depending on segment type.
 *  DAM_SEG:  flags != 0 ==>> only get dams segments with storage basins.
 * Menu ids are assigned sequentially from "idStart". Menu ids are assigned
 *  in the order that segments or sites are encountered when iterating
 *  through their respective lists with the "next" pointers.
 */
QPopupMenu * QCrisp::CreateRiverSegmentMenu( int *pidStart, int type, int flags )
{
    struct river_segment *seg = river_mouth;
	QPopupMenu *menu = new QPopupMenu();
	int appendIt;

	while (seg) {
		appendIt = 0;
		if (type == DAM_SEG && flags) {
			if (seg->seg_type == type && seg->current->dam.storage_basin)
				appendIt = 1;
		}
		else if (seg->seg_type == type)
			appendIt = 1;

		if (appendIt)
			menu->insertItem( seg->seg_name,this, SLOT(DecodeCommand(int)),0, (*pidStart)++ );
		seg = seg->next;
	}
	return menu;

}

/* These routines get the names of river segments and release
    segments. This is needed for dynamic menu item handling (the
	dialogs which will be created want names of segments and sites).
	"flags" may have special meaning for certain segment types.
	DAM_SEG:  flags != 0 ==>> count only dams with storage basins.
 */
char *QCrisp::RiverSegmentName( int n, int type, int flags )
{
    struct river_segment *seg = river_mouth;
	int countIt;

	while (seg && n >= 0) {
		countIt = 0;
		if (seg->seg_type == DAM_SEG && flags) {
			if (seg->seg_type == type && seg->current->dam.storage_basin)
				countIt = 1;
		}
		else if (seg->seg_type == type)
			countIt = 1;
		if (countIt) {
			if (n == 0)
				return seg->seg_name;
			n--;
		}
		seg = seg->next;
	}
	return NULL;  /* Should never happen */
}

char *QCrisp::ReleaseSiteName( int n )
{
	struct release_site  *site = release_site;

	while (site && n >= 0) {
		if (n == 0)
			return site->name;
		n--;
		site = site->next;
	}
	return NULL; /* Should never happen. */
}

void QCrisp::DecodeCommand(int id){


	char * msg = new char[40];
	sprintf(msg, " id : %d", id);

	if (id >= idReleaseAddBase 
			 && id < idReleaseAddBase + nSites) {

			struct release_site  *site = release_site;
			for (int i = 0; i < id - idReleaseAddBase; i++)
				site = site->next;

			if (theReleaseTool_ != NULL) {
				theReleaseTool_->setShown( true );
				theReleaseTool_->selectRelease( site->name );
			}
			else {
				theReleaseTool_ = new qReleaseTool( this, site->name );
			}
		}	

	if (id >= idReservoirNSatDamBase 
		 && id < idReservoirNSatDamBase + nDams) {
		medBuilder_->makeGraphDialog( 
			new qDamNSatGDT( 
				RiverSegmentName( id - idReservoirNSatDamBase, DAM_SEG ) ) );
	}
	
	if (id >= idReservoirNSatReachBase 
		 && id < idReservoirNSatReachBase + nReaches) {
		medBuilder_->makeGraphDialog( 
			new qReachNSatGDT( 
				RiverSegmentName( id - idReservoirNSatReachBase, REACH_SEG ) ) );
	}
	
	if (id >= idReservoirDamWaterTempBase 
		 && id < idReservoirDamWaterTempBase + nDams) {
		medBuilder_->makeGraphDialog( 
			new qDamWaterTempGDT( 
				RiverSegmentName( id - idReservoirDamWaterTempBase, DAM_SEG ) ) );
	}
	
	if (id >= idReservoirReachWaterTempBase 
		 && id < idReservoirReachWaterTempBase + nReaches) {
		medBuilder_->makeGraphDialog( 
			new qReachWaterTempGDT( 
				RiverSegmentName( id - idReservoirReachWaterTempBase, REACH_SEG ) ) );
	}
	/* Turbidity  */
	if (id >= idReservoirDamTurbidityBase 
		 && id < idReservoirDamTurbidityBase + nDams) {
		medBuilder_->makeGraphDialog( 
			new qDamTurbidityGDT( 
				RiverSegmentName( id - idReservoirDamTurbidityBase, DAM_SEG ) ) );
	}
	
	if (id >= idReservoirReachTurbidityBase 
		 && id < idReservoirReachTurbidityBase + nReaches) {
		medBuilder_->makeGraphDialog( 
			new qReachTurbidityGDT( 
				RiverSegmentName( id - idReservoirReachTurbidityBase, REACH_SEG ) ) );
	}

	if (id >= idReservoirHeadwaterTempBase 
		 && id < idReservoirHeadwaterTempBase + nHeadwaters) {
		medBuilder_->makeEditGraphDialog( 
			new qHeadwaterTempGDT( 
				RiverSegmentName( id - idReservoirHeadwaterTempBase, HEADWATER_SEG ) ) );
	}
	
	if (id >= idReservoirHeadwaterTurbidityBase 
		 && id < idReservoirHeadwaterTurbidityBase + nHeadwaters) {
		medBuilder_->makeEditGraphDialog( 
			new qHeadwaterTurbidityGDT( 
				RiverSegmentName( id - idReservoirHeadwaterTurbidityBase, HEADWATER_SEG ) ) );
	}

	if (id >= idReservoirVelocityBase 
		 && id < idReservoirVelocityBase + nReaches) {
		/* Resevoir / Velocity submenu. */
		medBuilder_->makeGraphDialog( 
			new qReachVelGDT( 
				RiverSegmentName( id - idReservoirVelocityBase, REACH_SEG ) ) );
	}

	if (id >= idFlowDamsBase 
		 && id < idFlowDamsBase + nDams) {
		/* Flow / Dams submenu */
		medBuilder_->makeGraphDialog( 
			new qDamFlowGDT( 
				RiverSegmentName( id - idFlowDamsBase, DAM_SEG ) ) );
	}

	if (id >= idFlowReachesBase 
		 && id < idFlowReachesBase + nReaches) {
		/* Flow / Reaches submenu. */
		medBuilder_->makeGraphDialog( 
			new qReachFlowGDT( 
				RiverSegmentName( id - idFlowReachesBase, REACH_SEG ) ) );
	}

	if (id >= idFlowHeadwatersBase 
		 && id < idFlowHeadwatersBase + nHeadwaters) {
		/* Flow / Reaches submenu. */
		medBuilder_->makeEditGraphDialog( 
			new qHeadwaterFlowGDT( 
				RiverSegmentName( id - idFlowHeadwatersBase, HEADWATER_SEG ) ) );
	}

	if (id >= idFlowLossBase 
		 && id < idFlowLossBase + nReaches) {
		medBuilder_->makeEditGraphDialog( 
			new qFlowLossGDT( 
				RiverSegmentName( id - idFlowLossBase, REACH_SEG ) ) );
	}

	if (id >= idFlowElevationBase 
		 && id < idFlowElevationBase + nReaches) {
		medBuilder_->makeEditGraphDialog( 
			new qElevationChangeGDT( 
				RiverSegmentName( id - idFlowElevationBase, REACH_SEG ) ) );
	}

	if (id >= idFlowReservoirsBase 
		 && id < idFlowReservoirsBase + nBasins) {
		qGraphDialog *d;
		QRect r;

		medBuilder_->makeEditGraphDialog( 
			new qBasinVolumeGDT( 
				RiverSegmentName( id - idFlowReservoirsBase, DAM_SEG, 1 ) ),
			HIDE_ACTIVE );
		d = medBuilder_->makeEditGraphDialog( 
			new qEditDamFlowGDT( 
				RiverSegmentName( id - idFlowReservoirsBase, DAM_SEG, 1 ) ),
			HIDE_ACTIVE );
		r = d->frameGeometry();
		d->move( r.left()+25, r.top()+25 );
	}

	if (id >= idDamSpillBase 
		 && id < idDamSpillBase + nDams) {
		/* Passage / Dam submenu. */
		medBuilder_->makeGraphDialog( 
			new qDamSpillGDT( 
				RiverSegmentName( id - idDamSpillBase, DAM_SEG ) ) );
	}

	if (id >= idPassageDamBase 
		 && id < idPassageDamBase + nDams) {
		/* Passage / Dam submenu. */
		medBuilder_->makeSpeciesDialog( 
			new qDamPassHistGDT( 
				RiverSegmentName( id - idPassageDamBase, DAM_SEG ) ) );
	}

	if (id >= idPassageReachBase 
		 && id < idPassageReachBase + nReaches) {
		/* Passage / Reach submenu. */
		medBuilder_->makeSpeciesDialog( 
			new qReachPassHistGDT( 
				RiverSegmentName( id - idPassageReachBase, REACH_SEG ) ) );
	}

	if (id >= idPassageLengthBase 
		 && id < idPassageLengthBase + nReaches) {
		/* Passage / Reach submenu. */
		medBuilder_->makeSpeciesDialog( 
			new qReachLengthHistGDT( 
				RiverSegmentName( id - idPassageLengthBase, REACH_SEG ) ) );
	}

#ifdef ROUTE_TRACKING
	if (id >= idPassageBypassBase 
		 && id < idPassageBypassBase + nDams) {
		/* Passage / Bypass submenu. */
		medBuilder_->makeSpeciesDialog( 
			new qBypassPassHistGDT( 
				RiverSegmentName( id - idPassageBypassBase, DAM_SEG ) ) );
	}

	if (id >= idPassageSluicewayBase 
		 && id < idPassageSluicewayBase + nDams) {
		/* Passage / Sluiceway submenu. */
		medBuilder_->makeSpeciesDialog( 
			new qSluicewayPassHistGDT( 
				RiverSegmentName( id - idPassageSluicewayBase, DAM_SEG ) ) );
	}

	if (id >= idPassageTurbineBase 
		 && id < idPassageTurbineBase + nDams) {
		/* Passage / Turbine submenu. */
		medBuilder_->makeSpeciesDialog( 
			new qTurbinePassHistGDT( 
				RiverSegmentName( id - idPassageTurbineBase, DAM_SEG ) ) );
	}

	if (id >= idPassageSpillwayBase 
		 && id < idPassageSpillwayBase + nDams) {
		/* Passage / Spillway submenu. */
		medBuilder_->makeSpeciesDialog( 
			new qSpillwayPassHistGDT( 
				RiverSegmentName( id - idPassageSpillwayBase, DAM_SEG ) ) );
	}

	if (id >= idPassageRSWBase 
		 && id < idPassageRSWBase + nDams) {
		/* Passage / RSW submenu. */
		medBuilder_->makeSpeciesDialog( 
			new qRSWPassHistGDT( 
				RiverSegmentName( id - idPassageRSWBase, DAM_SEG ) ) );
	}

	if (id >= idPassageTransportBase 
		 && id < idPassageTransportBase + nDams) {
		/* Passage / Transport submenu. */
		medBuilder_->makeSpeciesDialog( 
			new qTransportPassHistGDT( 
				RiverSegmentName( id - idPassageTransportBase, DAM_SEG ) ) );
	}
#endif
		


}

void QCrisp::ReachPredatorDensity(){
	new qSelectDialogMedBase( &qIdcReachPredDensity_,
				new qSelectDialogBase( this , 2, 0), "reach predator density (predators km^-2)" );
			
}

void QCrisp::GasDistribution(){
	medBuilder_->makeEquationDialog(GDTT_REACH_GasDist,NULL);

}

void QCrisp::ReachGasTheta(){
	new qListDialogMedBase( &qIdcReachGasTheta_,
				new qListDialogBase( this), 	"Reach Gas Theta" );
			

}


void QCrisp::PredDistributionCoef(){
		new qListDialogMedBase( &qIdcPredDistCoef_,
				new qListDialogBase( this ), "predator distribution coefficient" );

}

void QCrisp::RiverParameters(){
	new qListDialogMedBase( &qIdcRiverParameters_,
			new qListDialogBase( this),	"river parameters" );
}

void QCrisp::ReachClasses(){
		qCheckBoxDialogBase *d =
					new qCheckBoxDialogBase( this );
				new qReachClassCBDMed( &qIdcReachClassSetting_,
						d, "Reach Classes" );

}
void QCrisp::ReachClassNames(){
	  if (theReachClassTool_ != NULL)
            theReachClassTool_->setShown( true );
        else
            theReachClassTool_ = new qReachClassTool( this );
}

void QCrisp::PredCoefReach(){
	new qReachClassSDMed( &qIdcReachPredCoeff_,
		new qSelectDialogBase( this,  2, 0 ),
				"reach predation coefficient (km^2 predator^-1 day^-1)" );
	


}

void QCrisp::PredMortCoef(){
	new qReachClassSDMed( &qIdcPredMortCoef_,
			new qSelectDialogBase( this,  2, 0 ),
					"time and distance coefficients" );
		

}
void QCrisp::PredCoefForebay(){
	new qListDialogMedBase( &qIdcForebayPredCoeff_,
			new qListDialogBase( this),"forebay predation coefficient (km^2 predator^-1 day^-1)" );

}
void QCrisp::PredationCoefTailrace(){
	new qListDialogMedBase( &qIdcTailracePredCoeff_,
			new qListDialogBase( this),
					"tailrace predation coefficient (km^2 predator^-1 day^-1)" );

}
void QCrisp::VVar(){
	new qReachClassSDMed( &qIdcVVAR_,
			new qSelectDialogBase( this,  2, 0 ),
					"velocity variance parameter" );	

}

void QCrisp::MigrationRateVar(){
	new qReachClassSDMed( &qIdcMigRateVar_,
			new qSelectDialogBase( this,  2, 0 ),
					"migration rate variance" );	

}

void QCrisp::PredTempEquation(){
	medBuilder_->makeEquationDialog( GDTT_BEHAVIOREQN_PredTemp, NULL );

}

void QCrisp::MigrationEquation(){
	 qEquationDialogBase *d;
     QRect r;
	d = medBuilder_->makeEquationDialog( 
                        GDTT_BEHAVIOREQN_Migration, NULL );
}

void QCrisp::GasMortEquation(){
	qEquationDialogBase *d;
	QRect r;
	medBuilder_->makeEquationDialog( 
						GDTT_BEHAVIOREQN_FDens, NULL );
	d = medBuilder_->makeEquationDialog( 
						GDTT_BEHAVIOREQN_GasMort, NULL );
	r = d->frameGeometry( );

    d->move( r.left()+25, r.top()+25 );


}

void QCrisp::UnregMax(){
		new qListDialogMedBase( &qIdcHdwUnregMaxFlow_,
				new qListDialogBase( this),	"unregulated headwater flow maximums" );

}

void QCrisp::HeadWaterMod(){
	
	qListDialogBase * dlg = new qListDialogBase( this );
	new qListDialogMedBase( &qIdcHeadwaterModulation_,
			dlg,"headwater and storage basin modulation" );
	dlg->resize(500,100);

}

void QCrisp::Transport(){
	if (theTransportTool_ != NULL)
		theTransportTool_->setShown( TRUE );
	else
		theTransportTool_ = new qTransportTool( this );	
}

void QCrisp::TransportMort(){
	qEquationDialogBase * edb = medBuilder_->makeEquationDialog(GDTT_DAMEQUATION_Trans,NULL);
	edb->show();
	
}

void QCrisp::SpillEfficieny(){	
	medBuilder_->makeEquationDialog(GDTT_DAMEQUATION_Spill,NULL);			
}

void QCrisp::SpillSchedule(){
	new qSDMPlannedSpillDay( this );
	new qSDMPlannedSpillNight( this );
}

void QCrisp::SpillCap(){
	new qListDialogMedBase( &qIdcDamSpillCap_,
		new qListDialogBase( this	),	"spill cap" );
}

void QCrisp::DamPredden(){
	new qSelectDialogMedBase( &qIdcDamPredDensity_,
		new qSelectDialogBase( this	),	"dam predator density (predators km^-2)" );

}

void QCrisp::PredProb(){
		medBuilder_->makeTimeBaseDialog(ID_PREDATIONPROB);
}

void QCrisp::NSatEquation(){
	qEquationDialogBase *d;
    QRect r;
	medBuilder_->makeEquationDialog( 
                    GDTT_DAMEQUATION_BackupNSat, NULL );
	d = medBuilder_->makeEquationDialog( 
                    GDTT_DAMEQUATION_NightNSat, NULL );
	r = d->frameGeometry(  );
	d->move( r.left()+25, r.top()+25 );
	d = medBuilder_->makeEquationDialog( 
                    GDTT_DAMEQUATION_NSat, NULL );
	r = d->frameGeometry(  );
	d->move( r.left()+50, r.top()+50  );
}

void QCrisp::DamGasTheta(){
	new qListDialogMedBase( &qIdcDamGasTheta_,
			new qListDialogBase( this ), "Dam Gas Theta" );
}

void QCrisp::KEntrain(){
	new qListDialogMedBase( &qIdcDamKEntrain_,
			new qListDialogBase( this), "K Entrain" );
}

void QCrisp::DelayEquation(){
   qEquationDialogBase *d;
   QRect r;
	d = medBuilder_->makeEquationDialog( 
                           GDTT_DAMEQUATION_Delay, NULL );

}

void QCrisp::PHCapacity(){
	new qListDialogMedBase( &qIdcPhouseCap_,
		new qListDialogBase( this	),	"powerhouse capacity" );	
}

void QCrisp::PHSchedule(){
	new qSDMPhouseSchedList( this );
		
}

void QCrisp::PHPriority(){

	new qCheckBoxDialogMedBase( &qIdcPhousePriority_,
			new qCheckBoxDialogBase( this),	"Powerhouse Priority" );

}

void QCrisp::PHFlowMin(){
	new qListDialogMedBase( &qIdcPhouseFlowMin_,
			new qListDialogBase( this),	"powerhouse flow minimum" );
		
}

void QCrisp::Mortality(){
	new qSelectDialogMedBase( &qIdcDamMortality_,
		new qSelectDialogBase( this),	"mortality" );
}

void QCrisp::SluicewayProp(){
	new qSelectDialogMedBase( &qIdcSluicewayProp_,
		new qSelectDialogBase( this),	"Proportion of non-spilled fish through sluiceway" );
}

void QCrisp::FGE(){
	new qSelectDialogMedBase( &qIdcFGE_,
			new qSelectDialogBase( this	),		"Fish Guidance Efficiency" );
}

void QCrisp::FGEEquation(){
		medBuilder_->makeEquationDialog( GDTT_DAMEQUATION_FGE, NULL );
}

void QCrisp::Mon(){
	if (theMonteCarloTool_ != NULL)
		theMonteCarloTool_->show();
	else
		theMonteCarloTool_ = new qMonteCarloTool( this );
			
}

void QCrisp::LockOutputSett(bool on){
	lock_output_settings = on;
	if (theMonteCarloTool_ != NULL)
			theMonteCarloTool_->setLockOutputSettings( on );
}
void QCrisp::OutputSetting(){

	QPoint p;
	qCheckBoxDialogBase *d =
		new qCheckBoxDialogBase( this );
	new qDamOutputCBDMedBase( &qIdcDamOutputSetting_, d, "Output Settings for Dams" );
	p = d->pos();

	d = new qCheckBoxDialogBase( this );
	new qReachOutputCBDMedBase( &qIdcReachOutputSetting_, d, "Output Settings for Reaches" );
	d->move( p.x() + 25, p.y()+25 );

	d = new qCheckBoxDialogBase( this );
	new qHeadwOutputCBDMedBase( &qIdcHeadwOutputSetting_, d, "Output Settings for Headwaters" );
	d->move( p.x()+50, p.y()+50 );

}
void QCrisp::Settings(){
	 if (theRunSettings_ != NULL)
         theRunSettings_->setShown( true );
     else
         theRunSettings_ = new qRunSettings( this );
		
}

void QCrisp::InputReport(){
	qInputReportTool *irp = qInputReportTool::instance( this );
	if (irp != NULL)
		irp->show( );
}

void QCrisp::MonteReport(){
	qMonteReportTool *mrp = qMonteReportTool::instance( this );
	if (mrp != NULL)
		mrp->show();
}

void QCrisp::MonteAltCompare(){
	qMonteAltCmpDlg *mad = qMonteAltCmpDlg::instance( this );
	if (mad != NULL)
		mad->show( );
}

void QCrisp::MonteAnalysis(){
	qMonteAnalDlg *mad = qMonteAnalDlg::instance( this );
	if (mad != NULL)
		mad->show( );
}

void QCrisp::fileOpened() {
		GOutputsValid_[0] = 1; /* Flow, gas, etc, now valid */
        GOutputsValid_[1] = 0; /* Passage now invalid */
		//theMapPane_->size(NULL); /* map shows active releases. */
		theMapPane_->repaint(); /* map shows active releases. */
}

void QCrisp::RunningScenario() {
	myStatus->setMessage("Running Scenario...");	

	GOutputsValid_[0] = 1;  /* Flow, gas, etc, valid */
    GOutputsValid_[1] = 1;  /* Passage valid */
}

void QCrisp::RunCanceled() {
	/* Watchdog timer actually does GUI changes, etc. */
	GOutputsValid_[0] = 0;   /* Flow, gas, etc, invalid */
    GOutputsValid_[1] = 0;   /* Passage invalid */
}
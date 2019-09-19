#include "qMapMedIcons.h"
#include "qMercatorTransform.h"
#include "QmapController.h"
#include "QmapObject.h"
#include "qMapSubMediator.h"

#include "qMapMedTypes.h"
#include "qUtils.h"
#include "crisp.h"
#include <qrect.h>
#include <qpainter.h>
#include "log.h"
#include <qbitmap.h>

static const char* const image2_data[] = { 
"13 8 3 1",
". c #000000",
"# c #0000ff",
"a c #ffffff",
".............",
".###########.",
".##.##.##.##.",
".##.##.##.##.",
"..#.##.##.#..",
"a.#########.a",
"a.#########.a",
"a...........a"};

static const char* const image3_data[] = { 
"52 33 3 1",
". c #000000",
"# c #0000ff",
"a c #ffffff",
"....................................................",
"....................................................",
"....................................................",
"....................................................",
"....############################################....",
"....############################################....",
"....############################################....",
"....############################################....",
"....########....########....########....########....",
"....########....########....########....########....",
"....########....########....########....########....",
"....########....########....########....########....",
"....########....########....########....########....",
"....########....########....########....########....",
"....########....########....########....########....",
"....########....########....########....########....",
"........####....########....########....####........",
"........####....########....########....####........",
"........####....########....########....####........",
"........####....########....########....####........",
"aaaa....####....########....########....####....aaaa",
"aaaa....####....########....########....####....aaaa",
"aaaa....####################################....aaaa",
"aaaa....####################################....aaaa",
"aaaa....####################################....aaaa",
"aaaa....####################################....aaaa",
"aaaa....####################################....aaaa",
"aaaa....####################################....aaaa",
"aaaa....####################################....aaaa",
"aaaa............................................aaaa",
"aaaa............................................aaaa",
"aaaa............................................aaaa",
"aaaa............................................aaaa"};


static const char* const image4_data[] = { 
"21 13 6 1",
"# c #000000",
"d c #808000",
"a c #c0c0c0",
"c c #ff0000",
". c #ffff00",
"b c #ffffff",
".....................",
".........#...........",
"........#a#.......#..",
".....###ab#####..##..",
"..###ababcbcbab##a#..",
".#addbcbcbababcbda#..",
"..bcadbdbababcbdaa#..",
".#badbdacbbbbbbd#a#..",
"..###aacbbbddbda.##..",
".....#########a#..#..",
"..............#......",
".....................",
"....................."};

static const char* const image5_data[] = { 
"80 42 6 1",
"# c #000000",
"d c #808000",
"a c #c0c0c0",
"c c #ff0000",
". c #ffff00",
"b c #ffffff",
"................................................................................",
"................................................................................",
"................................................................................",
"................................................................................",
".....................................###........................................",
"....................................####........................................",
"....................................######......................................",
".................................###aaaa##...............................#......",
"................................####aaaa##..............................##......",
"..............................######aaaa##.............................###......",
"..............................######aaaa##............................####......",
"............................####aaaabbbb####........................######......",
"...................#############aaaabbbb##################.........#######......",
"..................##############aaaabbbb########################..########......",
"...........#########aaaabbbbaaaabbbbccccbbbbccccbbbbaaaabbbb########aaaa##......",
"..........##########aaaabbbbaaaabbbbccccbbbbccccbbbbaaaabbbb########aaaa##......",
"........############aaaabbbbaaaabbbbccccbbbbccccbbbbaaaabbbb########aaaa##......",
"........############aaaabbbbaaaabbbbccccbbbbccccbbbbaaaabbbb########aaaa##......",
"......##aaaaddddddddbbbbccccbbbbccccbbbbaaaabbbbaaaabbbbccccbbbbddddaaaa##......",
"....####aaaaddddddddbbbbccccbbbbccccbbbbaaaabbbbaaaabbbbccccbbbbddddaaaa##......",
"....####aaaaddddddddbbbbccccbbbbccccbbbbaaaabbbbaaaabbbbccccbbbbddddaaaa##......",
"........bbbbccccaaaaddddbbbbddddbbbbaaaabbbbaaaabbbbccccbbbbddddaaaaaaaa##......",
"........bbbbccccaaaaddddbbbbddddbbbbaaaabbbbaaaabbbbccccbbbbddddaaaaaaaa##......",
"........bbbbccccaaaaddddbbbbddddbbbbaaaabbbbaaaabbbbccccbbbbddddaaaaaaaa##......",
"....####bbbbaaaaddddbbbbddddaaaaccccccbbbbbbbbbbbbbbbbbbbbbbdddd####aaaa##......",
"....####bbbbaaaaddddbbbbddddaaaaccccccbbbbbbbbbbbbbbbbbbbbbbdddd####aaaa##......",
"....####bbbbaaaaddddbbbbddddaaaaccccbbbbbbbbbbbbbbbbbbbbbbbbdddd####aaaa##......",
"....####bbbbaaaaddddbbbbddddaaaaccccbbbbbbbbbbbbbbbbbbbbbbbbdddd####aaaa##......",
"......##############aaaaaaaaccccbbbbbbbbbbbbddddddddbbbbddddaaaa..########......",
"..........##########aaaaaaaaccccbbbbbbbbbbbbddddddddbbbbddddaaa.....######......",
"................####aaaaaaaaccccbbbbbbbbbbbbddddddddbbbbddddaaaa....######......",
"....................####################################aaaa####......####......",
".........................###############################aaaa####........##......",
"....................................................####aaaa####........##......",
"....................................................####aaaa####.........#......",
"......................................................########..................",
"........................................................####....................",
"........................................................####....................",
"................................................................................",
"................................................................................",
"................................................................................",
"................................................................................"};


static const char* const image6_data[] = { 
"17 12 3 1",
"# c #000000",
"a c #00ff00",
". c #ffffff",
".....##########..",
"..###aaaaaaaaaa#.",
"..#..#aaaaaaaaaa#",
"..#..#aaa###a#aa#",
".#a###aa#...#aaa#",
"#aa#a#aaa###a#aa#",
"#aa#a#aaaaaaaaaa#",
"#aa#a#aaaaaaaaaa#",
"#################",
"..##........##...",
".#..#......#..#..",
"..##........##..."};

static const char* const image7_data[] = { 
"68 49 3 1",
"# c #000000",
"a c #00ff00",
". c #ffffff",
".....................######################################.........",
"....................########################################........",
"...................##########################################.......",
"..................############################################......",
"...........###########aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa#####.....",
"..........###########aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa#####....",
".........############aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa#####...",
"........##############aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa#####..",
"........#####.....#####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa#####.",
"........####.......#####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa#####",
"........####........####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"........####........####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"........####........####aaaaaaaaaaaaa##########aaaaaa##aaaaaaaaa####",
"........####........####aaaaaaaaaaaa############aaaa####aaaaaaaa####",
".......######.......####aaaaaaaaaaa##############aa#####aaaaaaaa####",
"......########.....#####aaaaaaaaaa#####################aaaaaaaaa####",
".....###################aaaaaaaaa#####........########aaaaaaaaaa####",
"....#####aa#############aaaaaaaa#####..........######aaaaaaaaaaa####",
"...#####aaaa############aaaaaaaa#####..........######aaaaaaaaaaa####",
"..#####aaaaa############aaaaaaaaa#####........########aaaaaaaaaa####",
".#####aaaaaa#####aa#####aaaaaaaaaa#####################aaaaaaaaa####",
"#####aaaaaaa####aaaa####aaaaaaaaaaa##############aa#####aaaaaaaa####",
"####aaaaaaaa####aaaa####aaaaaaaaaaaa############aaaa####aaaaaaaa####",
"####aaaaaaaa####aaaa####aaaaaaaaaaaaa##########aaaaaa##aaaaaaaaa####",
"####aaaaaaaa####aaaa####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"####aaaaaaaa####aaaa####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"####aaaaaaaa####aaaa####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"####aaaaaaaa####aaaa####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"####aaaaaaaa####aaaa####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"####aaaaaaaa####aaaa####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"####aaaaaaaa####aaaa####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"####aaaaaaaa#####aa#####aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa####",
"####################################################################",
"####################################################################",
"####################################################################",
".###################################################################",
"..#################################################################.",
".........######..................................######.............",
"........########................................########............",
".......##########..............................##########...........",
"......############............................############..........",
".....#####....#####..........................#####....#####.........",
".....####......####..........................####......####.........",
".....####......####..........................####......####.........",
".....#####....#####..........................#####....#####.........",
"......############............................############..........",
".......##########..............................##########...........",
"........########................................########............",
"........########.................................######............."};

static const char* const image0_data[] = { 
"18 10 5 1",
"# c #000000",
"c c #808000",
"a c #c0c0c0",
"b c #ff0000",
". c #ffffff",
"........#.........",
".......#a#.......#",
"....###a.#####..##",
".###a.a.b.b.a.##a#",
"#acc.b.b.a.a.b.ca#",
"..bac.c.a.a.b.caa#",
"#.ac.cab......c#a#",
".###aab...cc.ca.##",
"....#########a#..#",
".............#...."};


static const char* const image1_data[] = { 
"72 34 5 1",
"# c #000000",
"c c #808000",
"a c #c0c0c0",
"b c #ff0000",
". c #ffffff",
".................................###....................................",
"................................####....................................",
"................................######..................................",
".............................###aaaa##...............................#..",
"............................####aaaa##..............................##..",
"..........................######aaaa##.............................###..",
"..........................######aaaa##............................####..",
"........................####aaaa....####........................######..",
"...............#############aaaa....##################.........#######..",
"..............##############aaaa....########################..########..",
".......#########aaaa....aaaa....bbbb....bbbb....aaaa....########aaaa##..",
"......##########aaaa....aaaa....bbbb....bbbb....aaaa....########aaaa##..",
"....############aaaa....aaaa....bbbb....bbbb....aaaa....########aaaa##..",
"....############aaaa....aaaa....bbbb....bbbb....aaaa....########aaaa##..",
"..##aaaacccccccc....bbbb....bbbb....aaaa....aaaa....bbbb....ccccaaaa##..",
"####aaaacccccccc....bbbb....bbbb....aaaa....aaaa....bbbb....ccccaaaa##..",
"####aaaacccccccc....bbbb....bbbb....aaaa....aaaa....bbbb....ccccaaaa##..",
"........bbbbaaaacccc....cccc....aaaa....aaaa....bbbb....ccccaaaaaaaa##..",
"........bbbbaaaacccc....cccc....aaaa....aaaa....bbbb....ccccaaaaaaaa##..",
"........bbbbaaaacccc....cccc....aaaa....aaaa....bbbb....ccccaaaaaaaa##..",
"####....aaaacccc....ccccaaaabbbbbb......................cccc####aaaa##..",
"####....aaaacccc....ccccaaaabbbbbb......................cccc####aaaa##..",
"####....aaaacccc....ccccaaaabbbb........................cccc####aaaa##..",
"####....aaaacccc....ccccaaaabbbb........................cccc####aaaa##..",
"..##############aaaaaaaabbbb............cccccccc....ccccaaaa..########..",
"......##########aaaaaaaabbbb............cccccccc....ccccaaa.....######..",
"............####aaaaaaaabbbb............cccccccc....ccccaaaa....######..",
"................####################################aaaa####......####..",
".....................###############################aaaa####........##..",
"................................................####aaaa####........##..",
"................................................####aaaa####.........#..",
"..................................................########..............",
"....................................................####................",
"....................................................####................"};



qMapMedIcons::qMapMedIcons(qMapPane *mp, QmapController *mc) : qMapSubMediator(mp, "Icon Mediator"), mapCon_(mc), 
	  image0( (const char **) image0_data ),
      image1( (const char **) image1_data ),
      image2( (const char **) image2_data ),
      image3( (const char **) image3_data ),
      image4( (const char **) image4_data ),
      image5( (const char **) image5_data ),
      image6( (const char **) image6_data ),
      image7( (const char **) image7_data )
{
	
	bdVector_.setAutoDelete(true);

	bdVector_.append(&image0 );
	bdVector_.append(&image1 );
	bdVector_.append(&image2 );
	bdVector_.append(&image3 );
	bdVector_.append(&image4 );
	bdVector_.append(&image5 );
	bdVector_.append(&image6 );
	bdVector_.append(&image7 );

	RegisterEvents();
}

qMapMedIcons::~qMapMedIcons(){
	bdVector_.clear();
}

void qMapMedIcons::RegisterEvents(){
	qMapSubMediator::RegisterEvents();
}

int qMapMedIcons::isA(){
	return MMT_ICONS;
}

void qMapMedIcons::evDraw(){
						 	
	QRect r;
	qMercatorTransform *zpt; 
	QPtrListIterator <QmapObject>*itr;
	QPoint tp;
	mapPoint  *mpo=NULL;
	QmapObject *mo=NULL;		
	unsigned int vIndex=0;
	struct river_segment *seg;
	struct release *rel;
	QPainter* p = mapPane_->currPainter; //create the painter
	
	//if(!lock_){
		lock_=1;
		//log_msg(0,"We are not locked");
 		//mapPane_->canvas()->getVisible(r);
		r = mapPane_->drawArea;
		if(r.right() == 0 || r.bottom() == 0) return;
		//mapPane_->canvas()->lock();

		zpt = (qMercatorTransform *)(mapPane_->getTransform());
		itr = mapCon_->getIconIterator();
	//}
	
        /* First, run through and draw all releases (only).
         */
	//	itr->toFirst();
		while((mo = itr->current()) != 0 ){
		//	mo = itr->current();
			++(*itr);
		
			mpo=mo->at(0);
			zpt->revX(10);
			if(mo->intersects((*zpt)(r.left(),r.top()),zpt->revXY(r.right(),r.bottom()))){
				tp = (*zpt)(mpo->lon(),mpo->lat());
				if(mo->objType() == RELEASE) {
					// Now see if there is an active release at this site.
					vIndex = 0;
					for (rel = current->release_list; rel != NULL; 
							rel = rel->next) {
						if (mo->getLabel().compare(rel->site->name)==0) {
							//log_msg(1,"acxtive release at this site");
							vIndex = 4;
							break;
						}
					}
				}
				else
					continue;
				if(mapPane_->isPrintMode()){
					log_msg(1,"is Print mode");
					vIndex++;
				}
				if( vIndex > bdVector_.count()) 
					continue;
				//essentialy draw this pixmap on the mappane
				QPoint location(tp.x()-bdVector_.at(vIndex)->width()/2,tp.y()-bdVector_.at(vIndex)->height()/2);
				p->drawPixmap(location,  *bdVector_.at(vIndex));
				//bdVector_.at(vIndex)->copyTo(mapPane_->canvas(),tp.x()-bdVector_at(vIndex)->width()/2,
				//						tp.y()-bdVector_at(vIndex)->height()/2,
				//						bdVector_at(vIndex)->width(),bdVector_at(vIndex)->height(),0,0);
				
			}
		}

        /* Then, run through again and draw all dams.
         */
        //itr->reset();
		
		itr->toFirst();
		while((mo = itr->current()) != 0 ){
			mo = itr->current();
			++(*itr);
			mpo=mo->at(0);
			if(mo->intersects((*zpt)(r.left(),r.top()),zpt->revXY(r.right(),r.bottom()))){
				tp = (*zpt)(mpo->lon(),mpo->lat());
				if(mo->objType() == DAM) {
					// Now see if there is an active transport at this dam.
					vIndex = 2;
					if ((seg = getRiverSegmentHandle( mo->getLabel().ascii(),
						             DAM_SEG )) != NULL)
						if (seg->current->dam.transport != NULL)
							vIndex = 6;
				}
				else
					continue;
				if(mapPane_->isPrintMode())
					vIndex++;
				if((unsigned int)vIndex > bdVector_.count()) 
					continue;
				//essentialy draw this pixmap on the mappane
				QPoint location(tp.x()-bdVector_.at(vIndex)->width()/2,tp.y()-bdVector_.at(vIndex)->height()/2);
				p->drawPixmap(location,  *bdVector_.at(vIndex));
				//bdVector_[vIndex]->copyTo(mapPane_->canvas(),tp.x()-bdVector_[vIndex]->getBitmap()->size().width()/2,
				//						tp.y()-bdVector_[vIndex]->getBitmap()->size().height()/2,
				//						bdVector_[vIndex]->getBitmap()->size().width(),bdVector_[vIndex]->getBitmap()->size().height(),0,0);
			}
		}

		// Add highlights as necessary
		itr->toFirst();
		while((mo = itr->current()) != 0 ){
			mo = itr->current();
			++(*itr);
			mpo=mo->at(0);
			if((mo->objType() == DAM || mo->objType() == RELEASE) && mo->isSelected() && mo->intersects((*zpt)(r.left(),r.top()),zpt->revXY(r.right(),r.bottom()))){
				p->setPen(QPen(blue,1,Qt::SolidLine));
				p->setBrush(QBrush(white));
			
				RasterOp o = p->rasterOp();
				p->setRasterOp(XorROP);
				tp = (*zpt)(mpo->lon(),mpo->lat());
				p->drawEllipse(QRect(tp-QPoint(10,10),tp+QPoint(10,10)));
				p->setRasterOp(o);
			}
		}
	
		delete itr;
		//mapPane_->canvas()->unlock();
		lock_=0;
}


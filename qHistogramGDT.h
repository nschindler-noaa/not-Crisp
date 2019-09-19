/* qHistogramGDT: This class takes a qGraphDataToken and creates a histogram of
 * the data which is also a subclass of a qGraphDataToken. This data can then
 * be graphed easily. */

#ifndef	__qHISTOGRAMGDT_H__
#define __qHISTOGRAMGDT_H__

#include "qGraphDataToken.h"

#define	IO_HISTOGRAM_GDT_DEFAULT_BINS	40

class qHistogramGDT : public qGraphDataToken {
protected:
	qGraphDataToken *gdt; /* The data to be histogrammed. This GDT will be deleted when the histogram itself is destroyed */
	int bins; /* The number of bins in the histogram */

public:
	qHistogramGDT(qGraphDataToken *gdt, int bins = IO_HISTOGRAM_GDT_DEFAULT_BINS);
	~qHistogramGDT() {delete gdt;}

	virtual int Fill( const char *feature = NULL, const char *species = NULL );

	void setBins(int _bins) {bins = _bins; Fill();}
};

#endif


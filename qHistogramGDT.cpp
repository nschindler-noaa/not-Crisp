#include "qHistogramGDT.h"
#include "unused.h"

qHistogramGDT::qHistogramGDT(qGraphDataToken *_gdt, int _bins)
:	qGraphDataToken(_gdt->getGraphTitle(), _gdt->getSourceStr(), GRT_INTEGER, _gdt->getEventTrigger(), _gdt->getGraphDataRange(), _gdt->getGraphDataTypeYAxis()),
	gdt(_gdt),
	bins(_bins)
{
	insertLabel((_gdt->getGraphTitle() + " (Monte Carlo)").ascii());
	setGraphXAxisLabel(_gdt->getGraphXAxisLabel());
	setGraphYAxisLabel(_gdt->getGraphYAxisLabel());
	Fill();
	setShowDots(0);
}

/* Helper function for determining bounds */
void calculate_bounds(double min, double max, double *new_min, double *new_max) {
	int min_decimal_place = floor(log(min < 0 ? -min : min)/log(10.0)) + 0.1;
	int max_decimal_place = floor(log(max < 0 ? -max : max)/log(10.0)) + 0.1;

	int max_precision = 3;
	if (min > 1 || max > 1) max_precision = 2;

	int precision;
	for (precision = 0; precision <= max_precision; precision++) {
		double tmp = exp(log(10.0)*(max_decimal_place - precision));
		int min_digits = ((int)(min/tmp));
		int max_digits = ((int)(max/tmp));
		if ((max_digits - min_digits) > 2 || precision == max_precision) {
			*new_min = tmp * min_digits;
			*new_max = tmp * (max_digits + 1);
			return;
		}
	}
}
int qHistogramGDT::Fill(const char *UNUSED(feature), const char *UNUSED(species)) {
	int i;

	/* Initialization. */
	setNumLines(1);
	setNumElementsPerLine(bins);
	dataArray_.resize(bins);
	while (dataArray_.size() < (unsigned int) bins)
		dataArray_.append( 0.0 );
	for (i = 0; i < bins; i++)
		dataArray_[i] = 0.0;

	double min = gdt->at(0);
	double max = gdt->at(0);

	/* Find min and max */
	for (i = 1; i < gdt->count(); i++) {
		if (gdt->at(i) < min) min = gdt->at(i);
		if (gdt->at(i) > max) max = gdt->at(i);
	}

	double binSize;

	calculate_bounds(min, max, &min, &max);

	binSize = (max - min) / bins;

	for (i = 0; i < gdt->count(); i++) {
		int index = (gdt->at(i) - min) / binSize;
		if (index < 0) index = 0;
		if (index >= ((int) bins)) index = bins - 1;
		dataArray_[index] += 1;
	}

	setHistogramMode(TRUE);
	setXMin(min);
	setXMax(max);
	setXAxisScale(0.0); /* Right now this is just a flag to print out more decimal places */
	setYBounds(1);

	return TRUE;
}

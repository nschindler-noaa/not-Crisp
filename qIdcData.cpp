#include "qIdcData.h"
#include "log.h"
#include "qGlobalEvents.h"
#include <stdlib.h>
#include "crisp.h"

/* Implementation of the string IDC */
qIdcDataString::qIdcDataString(char **_data) : dirty(0), data(_data) {
	if (data == NULL) log_msg(L_ERROR, "IDC for nonexistent string. This is a programming error and will crash!\n");
	if (!connect(qEvents::Instance(), SIGNAL(GEUpdateAllDataTokens_()), this, SLOT(reset())))
		log_msg(L_ERROR, "Could not connect qIDCDataString signal handlers\n");
}

void qIdcDataString::setDirty(int _dirty) {if (dirty != _dirty) {dirty = _dirty; emit dirtyChanged();}}

void qIdcDataString::setData(const QString& str) {
	cache = str;
	setDirty(TRUE);
	emit invalidated();
}
void qIdcDataString::apply() {
	if (*data) free(*data);
	if (cache.isNull()) *data = NULL;
	else *data = strdup(cache.ascii());
	setDirty(FALSE);
	emit invalidated();
}
void qIdcDataString::reset() {
	cache = *data;
	setDirty(FALSE);
	emit invalidated();
}

/* Implementation of the integer IDC */
qIdcDataInt::qIdcDataInt(int*_data) : dirty(0), data(_data) {
	if (data == NULL) log_msg(L_ERROR, "IDC for nonexistent string. This is a programming error and will crash!\n");
	if (!connect(qEvents::Instance(), SIGNAL(GEUpdateAllDataTokens_()), this, SLOT(reset())))
		log_msg(L_ERROR, "Could not connect qIDCDataString signal handlers\n");
}

void qIdcDataInt::setDirty(int _dirty) {if (dirty != _dirty) {dirty = _dirty; emit dirtyChanged();}}

void qIdcDataInt::setData(int str) {
	cache = str;
	setDirty(TRUE);
	emit invalidated();
}
void qIdcDataInt::apply() {
	*data = cache;
	setDirty(FALSE);
	emit invalidated();
}
void qIdcDataInt::reset() {
	cache = *data;
	setDirty(FALSE);
	emit invalidated();
}

/* Uses of the IDCs */
qIdcDataInt postBonnevilleHypothesisIDC(&run[0].post_bonneville_hypothesis);

#ifndef	__qIdcData_h
#define	__qIdcData_h

#include <qobject.h>

/* IDCs don't appear to work with strings very well for whatever reasons (memory
 * leaks, the dirty setting isn't preserved, none of the current IDCs use strings
 * so strings are used differently than doubles, etc. etc. etc.), so here's a
 * different take on IDC-like functionality. Unfortunately, Qt's metaobject
 * compiler doesn't support templates... so I guess this could be copied by hand
 * to support other data types */
class qIdcDataString : public QObject {
	Q_OBJECT

protected:
	int dirty;
	char ** const data;
	QString cache;

public:
	qIdcDataString(char **_data);

	const QString& getData() {return cache;}

	int isDirty() {return dirty;}
	void setDirty(int _dirty);

public slots:
	void setData(const QString& str);
	void apply();
	void reset();

signals:
	void invalidated();
	void dirtyChanged();
};

/* Qt's metaobject compiler doesn't support templates... */
class qIdcDataInt : public QObject {
	Q_OBJECT

protected:
	int dirty;
	int* const data;
	int cache;

public:
	qIdcDataInt(int*_data);

	int getData() {return cache;}

	int isDirty() {return dirty;}
	void setDirty(int _dirty);

public slots:
	void setData(int str);
	void apply();
	void reset();

signals:
	void invalidated();
	void dirtyChanged();
};

/* The string IDC is used in qIONotes.cpp */

/* The integer IDC is used for global ints like the post-Bonneville hypothesis */
extern qIdcDataInt postBonnevilleHypothesisIDC;


#endif


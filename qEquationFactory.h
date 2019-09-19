#ifndef _qEquationFactory_
#define _qEquationFactory_

#include "qEquationBase.h"

class qEquationFactory {

	static qEquationFactory *instance_;
	qEquationFactory();

public:

	~qEquationFactory();

	static qEquationFactory *qEquationFactory::instance();

	qEquationBase *BuildEquation(int id);

};


#endif


#ifndef _qGraphDataTokenBuilder_h_
#define _qGraphDataTokenBuilder_h_

#include "qGraphDataToken.h"

struct river_segment;
class qDatContainer;

class qGraphDataTokenBuilder {

		static qGraphDataTokenBuilder *instance_;
		
		qGraphDataTokenBuilder();

public:

		~qGraphDataTokenBuilder();
		static qGraphDataTokenBuilder *instance();

		qGraphDataToken *getToken(int dataType,
			               const char *sourceName, char *speciesName = NULL );
		qGraphDataToken *getToken(int dataType, qDatContainer *dat);

		void applyToken(qGraphDataToken *token);
};


#endif



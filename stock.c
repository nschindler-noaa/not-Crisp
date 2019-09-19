/** \file
 \brief  general routines for Stock construction, destruction, and
 * maintenance.
 */

#include <malloc.h>
#include <string.h>
#include "strtools.h"
#include "stock.h"

/** allocate and return a stock with the requested name.  makes a copy
 * of the name pointer.  returns 0 on failure.
 */
StockT *construct_stock(name)
    char *name;
{
    StockT *stock = (StockT *) malloc(sizeof(StockT));
    if (!stock)
	return 0;
	bzero(stock, sizeof(StockT));
    stock->name = strdup(name);
    if (name && !stock->name) {
	free(stock);
	return 0;
    }
    return stock;
}

/** construct a new stock of the requested name and insert it into the global
 * list.  return 0 on error, new number of stocks on success.
 */
extern struct stock_info stocks; /* global list */

int new_global_stock(name)
    char *name;
{
    StockT **newvec;
    StockT *stock = construct_stock(name);
    if (!stock)
	return 0;
    if (stocks.num == 0) 
	newvec = (StockT **) malloc(sizeof(StockT *));
    else
	newvec = (StockT **) realloc((char *)stocks.s,
				     sizeof(StockT *) * (stocks.num+1));
    if (!newvec) {
	destroy_stock(&stock);
	return 0;
    }
    stocks.s = newvec;
    stocks.s[stocks.num++] = stock;
    return stocks.num;
}

/** deletes a stock and zeros the pointer to it.
 */
void destroy_stock(stockpp)
    StockT **stockpp;
{
    StockT *ptr = *stockpp;
    if (ptr->name)
	free(ptr->name);
    free(ptr);
    *stockpp = 0;
}

#ifdef NOTDEF	
/** copy the data from one stock to another.  if the from "param" is null,
 * fill with generic data.
 */
extern void copy_stock( /* StockT *from, StockT *to */ )
    StockT *from;
    StockT *to;
{
    if (from == to)
	return;
    
    if (to->name)
	free(to->name);

    if (!from) {
	bzero((char *)to, sizeof(StockT));
	to->name = strdup("Generic");
    }
    else {
	bcopy((char *)from, (char *)to, sizeof(StockT));
	to->name = strdup(from->name);
    }
}

#endif

StockT stock_defaults;


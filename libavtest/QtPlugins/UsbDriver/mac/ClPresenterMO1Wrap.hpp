#ifndef __CLPRESENTERMO1WRAP_MAC_H_
#define __CLPRESENTERMO1WRAP_MAC_H_


#include <QImage>


class AbstCapture
{
public:
    virtual ~AbstCapture( void ){}
    virtual bool didStart( void ) = 0;
    virtual bool grabFrame (void ) = 0;
    virtual void retrieveFrame( QImage& ) = 0;
};
AbstCapture*    createCapClass( void );
void            deleteCapClass( AbstCapture  *pclCaptures );


#endif //__CLPRESENTERMO1WRAP_MAC_H_

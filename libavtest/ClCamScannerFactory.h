#ifndef CLCAMSCANNERFACTORY_H
#define CLCAMSCANNERFACTORY_H
#include "AbsCamScanner.h"

class ClCamScannerFactory
{
public:
    enum eSCANNER_TYPE
    {
        SCANNER_TYPE_WEBCAM,
        SCANNER_TYPE_USB_DOC_CAM,
        SCANNER_TYPE_WIFI_DOC_CAM,

        SCANNER_TYPE_MAX
    };

    AbsCamScanner* getInstance(eSCANNER_TYPE);
};

#endif // CLCAMSCANNERFACTORY_H

#include "ClCamScannerFactory.h"
#include "ClUsbCamScanner.h"
#include "ClDocCamFilter.h"
//#include "ClCommonWebcamFilter.h"
//#include "ClWifiDocCamScanner.h"

AbsCamScanner* ClCamScannerFactory::getInstance(
        ClCamScannerFactory::eSCANNER_TYPE eType)
{
    AbsCamScanner* instance = NULL;

    switch (eType) {

    case SCANNER_TYPE_USB_DOC_CAM:
        instance = new ClUsbCamScanner(new ClDocCamFilter);
        instance->setName("ELMO UVC Scanner");
        break;
#if 0
    case SCANNER_TYPE_WIFI_DOC_CAM:
        instance = new ClWifiDocCamScanner;
        instance->setName("ELMO Wifi Scanner");
        break;
#endif
    default:
        break;
    }

    return instance;
}

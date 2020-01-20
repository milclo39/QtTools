#include "cltabletcanvas.h"
#include <msinkaut_i.c>

//--------------------------------------------------------------------------------------
clTabletCanvas::clTabletCanvas(QWidget* parent, Qt::WindowFlags f) :
	QAxWidget(parent, f)
{
	long hr = 0;
	m_pInkPicture = NULL;
	IInkDrawingAttributes *pDefInkAttr = NULL;

	this->setControl(QUuid(CLSID_InkPicture).toString());
	hr = this->queryInterface("{e85662e0-379a-40d7-9b5c-757d233f9923}",(void **)&m_pInkPicture);
	if(hr != 0){
		qDebug() << "InkPicture Failure";
	}
	m_pInkPicture->put_Enabled(VARIANT_FALSE);
	m_pInkPicture->put_EraserMode(IOERM_StrokeErase);
	m_pInkPicture->get_DefaultDrawingAttributes(&pDefInkAttr);

	float width;
	float height;
	pDefInkAttr->get_Width(&width);
	pDefInkAttr->get_Height(&height);

	pDefInkAttr->put_Width(width * 2);
	m_pInkPicture->putref_DefaultDrawingAttributes(pDefInkAttr);
	m_pInkPicture->put_DynamicRendering(VARIANT_TRUE);
	m_pInkPicture->put_Enabled(VARIANT_TRUE);

	m_pInkCollector = NULL;

	QAxObject *qObject = new QAxObject(this);
	qObject->setControl(QUuid(CLSID_InkCollector).toString());
	hr = qObject->queryInterface(QUuid("{F0F060B5-8B1F-4a7c-89EC-880692588A4F}"),(void **)&m_pInkCollector);
	if(hr != 0){
		qDebug() << "InkCollector Failure";
	}

	m_pInkCollector->put_Enabled(VARIANT_FALSE);
	m_pInkCollector->put_hWnd((long)winId());
	m_pInkCollector->putref_DefaultDrawingAttributes(pDefInkAttr);
	m_pInkCollector->put_AutoRedraw(VARIANT_TRUE);
//	m_pInkCollector->SetEventInterest(ICEI_MouseMove,VARIANT_TRUE);
	m_pInkCollector->put_DynamicRendering(VARIANT_TRUE);
	m_pInkCollector->put_Enabled(VARIANT_TRUE);
}
//--------------------------------------------------------------------------------------
clTabletCanvas::~clTabletCanvas()
{
	if(m_pInkCollector != NULL){
		m_pInkCollector->put_Enabled(VARIANT_FALSE);
		m_pInkCollector->Release();
	}
}
//--------------------------------------------------------------------------------------
void clTabletCanvas::setPenColor(COLORREF color)
{
	IInkDrawingAttributes *attr = NULL;
	m_pInkPicture->get_DefaultDrawingAttributes(&attr);
	attr->put_Color(color);
	m_pInkPicture->putref_DefaultDrawingAttributes(attr);
}
//--------------------------------------------------------------------------------------
void clTabletCanvas::setPenWidth(int width)
{
	IInkDrawingAttributes *attr = NULL;
	m_pInkPicture->get_DefaultDrawingAttributes(&attr);
	attr->put_Width(53 * width);
	m_pInkPicture->putref_DefaultDrawingAttributes(attr);
}
//--------------------------------------------------------------------------------------
void clTabletCanvas::setPenTransparency(int value)
{
	IInkDrawingAttributes *attr = NULL;
	m_pInkPicture->get_DefaultDrawingAttributes(&attr);
	attr->put_Transparency(value);
	m_pInkPicture->putref_DefaultDrawingAttributes(attr);
}
//--------------------------------------------------------------------------------------
void clTabletCanvas::setPenMode(InkOverlayEditingMode mode)
{
	m_pInkPicture->put_Enabled(VARIANT_FALSE);
	m_pInkPicture->put_EditingMode(mode);
	m_pInkPicture->put_Enabled(VARIANT_TRUE);
}
//--------------------------------------------------------------------------------------
void clTabletCanvas::setSaveData(VARIANT* pdata)
{
	IInkDisp *disp = NULL;
	m_pInkPicture->get_Ink(&disp);
	disp->Save(IPF_InkSerializedFormat, IPCM_Default, pdata);
	m_pInkPicture->putref_Ink(disp);
}
//--------------------------------------------------------------------------------------
void clTabletCanvas::setLoadData(VARIANT data)
{
	IInkDisp *disp = NULL;
	m_pInkPicture->get_Ink(&disp);
	disp->Load(data);
	m_pInkPicture->putref_Ink(disp);
}
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

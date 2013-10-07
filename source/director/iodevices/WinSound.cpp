#include "WinSound.h"
#include "../../common/memleakdetect.h"

namespace Picto {

WinSound::WinSound(QString path)
{

	pGraph_ = NULL;
    pControl_ = NULL;
    pEvent_ = NULL;
	pPosition_ = NULL;
	loadSuccess_ = false;

    // Initialize the COM library.
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        printf("ERROR - Could not initialize COM library");
        return;
    }

    // Create the filter graph manager and query for interfaces.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, (void **)&pGraph_);
    if (FAILED(hr))
    {
        printf("ERROR - Could not create the Filter Graph Manager.");
        return;
    }

    hr = pGraph_->QueryInterface(IID_IMediaControl, (void **)&pControl_);
    hr = pGraph_->QueryInterface(IID_IMediaEvent, (void **)&pEvent_);
	hr = pGraph_->QueryInterface(IID_IMediaPosition, (void **)&pPosition_);

    // Build the graph. IMPORTANT: Change this string to a file on your system.
    hr = pGraph_->RenderFile(path.toStdWString().c_str(), NULL);
	loadSuccess_ = SUCCEEDED(hr);
}

WinSound::~WinSound()
{
	pControl_->Release();
    pEvent_->Release();
    pGraph_->Release();
	pPosition_->Release();
	CoUninitialize();
}

void WinSound::play()
{
	pControl_->Stop();
	pPosition_->put_CurrentPosition(0.0f);
	// Run the graph.
    HRESULT hr = pControl_->Run();
    Q_ASSERT(SUCCEEDED(hr));
}
void WinSound::stop()
{
	pControl_->Stop();
	pPosition_->put_CurrentPosition(0.0f);
}
void WinSound::setVolume(int percent)
{

}

bool WinSound::playing()
{
	long eventCode;
	HRESULT res = pEvent_->WaitForCompletion(0,&eventCode);
	if(res == E_ABORT)
		return true;
	return false;
}

int WinSound::volume()
{
	return 100;
}

bool WinSound::isReady()
{
	return loadSuccess_;
}

QString WinSound::errorString()
{
	if(!loadSuccess_)
		return "File could not be loaded.";
	return "";
}

QSharedPointer<PreloadedSound> WinSound::createWinSound(QString path)
{
	return QSharedPointer<PreloadedSound>(new WinSound(path));
}


}; //namespace Picto

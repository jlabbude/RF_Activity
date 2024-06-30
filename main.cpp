#include <iostream>
#include <windows.h>
#include <wincodec.h>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void takeScreenshot(int left, int top, int width, int height, LPCWSTR filename) {
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL bRet = BitBlt(hDC, 0, 0, width, height, hScreen, left, top, SRCCOPY);

    IWICImagingFactory* pFactory = NULL;
    IWICBitmapEncoder* pEncoder = NULL;
    IWICBitmapFrameEncode* pFrame = NULL;
    IWICStream* pStream = NULL;

    CoInitialize(NULL);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
    pFactory->CreateStream(&pStream);
    pStream->InitializeFromFilename(filename, GENERIC_WRITE);
    pFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder);
    pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
    pEncoder->CreateNewFrame(&pFrame, NULL);
    pFrame->Initialize(NULL);
    pFrame->SetSize(width, height);

    IWICBitmap* pWICBitmap = NULL;
    pFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUseAlpha, &pWICBitmap);

    GUID format = GUID_WICPixelFormat32bppBGRA;
    pFrame->SetPixelFormat(&format);

    pFrame->WriteSource(pWICBitmap, NULL);
    pFrame->Commit();
    pEncoder->Commit();

    if (pWICBitmap) pWICBitmap->Release();
    if (pFrame) pFrame->Release();
    if (pEncoder) pEncoder->Release();
    if (pStream) pStream->Release();
    if (pFactory) pFactory->Release();
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
    CoUninitialize();
}

bool shouldClick(string imageName, double threshold) {

    takeScreenshot(0, 0, 1920, 1080, L"screenfind2.png");

    Mat screen = imread("screenfind2.png");
    Mat templateImage = imread(imageName);

    Mat result;
    matchTemplate(screen, templateImage, result, TM_CCOEFF_NORMED);

    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    if (maxVal >= threshold) {
        cout << maxVal;
        return true;
    }
    else {
        cout << maxVal;
        return false;
    }

}

int main() {
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    while (!shouldClick("new_bar.png", 0.82)) {
        cout << " waiting ";
    }
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    return 0;
}
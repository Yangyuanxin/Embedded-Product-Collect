#include "CHidIO.h"
#include <tchar.h>
#include <vector>
//#include "dbt.h"

extern "C" {
#include "setupapi.h"
#include "hidsdi.h"
}

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "Setupapi.lib")

HANDLE CHidIO::GetActiveHandle() const
{
    return m_hWriteHandle[m_szActiveDeviceIndex];
}

CHidIO::CHidIO()
    : m_bUseTwoHandle(TRUE)
    , m_hAbordEvent(CreateEvent(NULL,TRUE,FALSE,NULL))
    , m_hReadEvent(CreateEvent(NULL,TRUE,FALSE,NULL))
    , m_hWriteEvent(CreateEvent(NULL,TRUE,FALSE,NULL))
    , m_szActiveDeviceIndex(0)
    , m_hReadHandle()
    , m_hWriteHandle()
{
}
CHidIO::~CHidIO()
{
    CloseDevice();
    CloseHandle(m_hWriteEvent);
    CloseHandle(m_hReadEvent);
    CloseHandle(m_hAbordEvent);
}

size_t CHidIO::GetDeviceLength() const
{
    return m_hReadHandle.size();
}

size_t CHidIO::SetActiveDevice(size_t szIndex)
{
    m_DeviceData.Close();

    size_t szOldIndex = m_szActiveDeviceIndex;
    m_szActiveDeviceIndex = szIndex;

    if(m_szActiveDeviceIndex < m_hReadHandle.size()) {
        std::basic_string<TCHAR> path = m_sNames[m_szActiveDeviceIndex].c_str();
        for(size_t i = 0; i < path.size(); ++i) {
            if(!::isalnum(path[i]))
                path[i] = _T('_');
        }
        m_DeviceData.SetName(path.c_str());
        m_DeviceData.Open();
    }
    return szOldIndex;
}

size_t CHidIO::GetActiveDevice() const
{
    return m_szActiveDeviceIndex;
}

ShareArea<CHidShare> &CHidIO::GetActiveDeviceData()
{
    return m_DeviceData;
}

void CHidIO::CloseDevice()
{
    if(m_bUseTwoHandle) {
        size_t i;

        for(i = 0; i < m_hReadHandle.size(); ++i) {
            if(m_hReadHandle[i] != INVALID_HANDLE_VALUE)
                CancelIo(m_hReadHandle[i]);
        }

        for(i = 0; i < m_hWriteHandle.size(); ++i) {
            if(m_hWriteHandle[i] != INVALID_HANDLE_VALUE)
                CancelIo(m_hWriteHandle[i]);
        }

        for(i = 0; i < m_hReadHandle.size(); ++i) {
            if(m_hReadHandle[i] != INVALID_HANDLE_VALUE) {
                CloseHandle(m_hReadHandle[i]);
                m_hReadHandle[i] = INVALID_HANDLE_VALUE;
            }
        }

        for(i = 0; i < m_hWriteHandle.size(); ++i) {
            if(m_hWriteHandle[i] != INVALID_HANDLE_VALUE) {
                CloseHandle(m_hWriteHandle[i]);
                m_hWriteHandle[i] = INVALID_HANDLE_VALUE;
            }
        }
    } else {
        size_t i;
        for(i = 0; i < m_hReadHandle.size(); ++i) {
            if(m_hReadHandle[i] != INVALID_HANDLE_VALUE)
                CancelIo(m_hReadHandle[i]);
        }

        for(i = 0; i < m_hReadHandle.size(); ++i) {
            if(m_hReadHandle[i] != INVALID_HANDLE_VALUE) {
                CloseHandle(m_hReadHandle[i]);
                m_hReadHandle[i] = INVALID_HANDLE_VALUE;
            }
        }
    }
    m_hReadHandle.clear();
    m_hWriteHandle.clear();
    m_sNames.clear();
}

void CHidIO::CloseInactiveDevice()
{
    if(m_bUseTwoHandle) {
        size_t i;

        for(i = 0; i < m_hReadHandle.size(); ++i) {
            if(m_hReadHandle[i] != INVALID_HANDLE_VALUE
                    && m_szActiveDeviceIndex != i)
                CancelIo(m_hReadHandle[i]);
        }

        for(i = 0; i < m_hWriteHandle.size(); ++i) {
            if(m_hWriteHandle[i] != INVALID_HANDLE_VALUE
                    && m_szActiveDeviceIndex != i)
                CancelIo(m_hWriteHandle[i]);
        }

        for(i = 0; i < m_hReadHandle.size(); ++i) {
            if(m_hReadHandle[i] != INVALID_HANDLE_VALUE
                    && m_szActiveDeviceIndex != i) {
                CloseHandle(m_hReadHandle[i]);
                m_hReadHandle[i] = INVALID_HANDLE_VALUE;
            }
        }

        for(i = 0; i < m_hWriteHandle.size(); ++i) {
            if(m_hWriteHandle[i] != INVALID_HANDLE_VALUE
                    && m_szActiveDeviceIndex != i) {
                CloseHandle(m_hWriteHandle[i]);
                m_hWriteHandle[i] = INVALID_HANDLE_VALUE;
            }
        }
    } else {
        size_t i;
        for(i = 0; i < m_hReadHandle.size(); ++i
                && m_szActiveDeviceIndex != i) {
            if(m_hReadHandle[i] != INVALID_HANDLE_VALUE)
                CancelIo(m_hReadHandle[i]);
        }

        for(i = 0; i < m_hReadHandle.size(); ++i
                && m_szActiveDeviceIndex != i) {
            if(m_hReadHandle[i] != INVALID_HANDLE_VALUE) {
                CloseHandle(m_hReadHandle[i]);
                m_hReadHandle[i] = INVALID_HANDLE_VALUE;
            }
        }
    }
}

BOOL CHidIO::OpenDevice(BOOL bUseTwoHandle, USHORT usVID, USHORT usPID0, USHORT usPID1, USHORT usPID2)
{
    m_bUseTwoHandle = bUseTwoHandle;

    //CString MyDevPathName="";
    TCHAR MyDevPathName[MAX_PATH];

    //定義一個GUID的結構體HidGuid來保存HID設備的接口類GUID。
    GUID HidGuid;
    //定義一個DEVINFO的句柄hDevInfoSet來保存獲取到的設備信息集合句柄。
    HDEVINFO hDevInfoSet;
    //定義MemberIndex，表示當前搜索到第幾個設備，0表示第一個設備。
    DWORD MemberIndex;
    //DevInterfaceData，用來保存設備的驅動接口信息
    SP_DEVICE_INTERFACE_DATA DevInterfaceData;
    //定義一個BOOL變量，保存函數調用是否返回成功
    BOOL Result;
    //定義一個RequiredSize的變量，用來接收需要保存詳細信息的緩衝長度。
    DWORD RequiredSize;
    //定義一個指向設備詳細信息的結構體指針。
    PSP_DEVICE_INTERFACE_DETAIL_DATA	pDevDetailData;
    //定義一個用來保存打開設備的句柄。
    HANDLE hDevHandle;
    HANDLE hReadHandle;
    HANDLE hWriteHandle;
    //定義一個HIDD_ATTRIBUTES的結構體變量，保存設備的屬性。
    HIDD_ATTRIBUTES DevAttributes;

    //初始化設備未找到
    BOOL MyDevFound=FALSE;

    //初始化讀、寫句柄為無效句柄。
    m_hReadHandle.clear();
    m_hWriteHandle.clear();
    m_sNames.clear();

    //對DevInterfaceData結構體的cbSize初始化為結構體大小
    DevInterfaceData.cbSize=sizeof(DevInterfaceData);
    //對DevAttributes結構體的Size初始化為結構體大小
    DevAttributes.Size=sizeof(DevAttributes);

    //調用HidD_GetHidGuid函數獲取HID設備的GUID，並保存在HidGuid中。
    HidD_GetHidGuid(&HidGuid);

    //根據HidGuid來獲取設備信息集合。其中Flags參數設置為
    //DIGCF_DEVICEINTERFACE|DIGCF_PRESENT，前者表示使用的GUID為
    //接口類GUID，後者表示只列舉正在使用的設備，因為我們這裡只
    //查找已經連接上的設備。返回的句柄保存在hDevinfo中。注意設備
    //信息集合在使用完畢後，要使用函數SetupDiDestroyDeviceInfoList
    //銷毀，不然會造成內存洩漏。
    hDevInfoSet=SetupDiGetClassDevs(&HidGuid,
                                    NULL,
                                    NULL,
                                    DIGCF_DEVICEINTERFACE|DIGCF_PRESENT);

    //AddToInfOut("開始查找設備");
    //然後對設備集合中每個設備進行列舉，檢查是否是我們要找的設備
    //當找到我們指定的設備，或者設備已經查找完畢時，就退出查找。
    //首先指向第一個設備，即將MemberIndex置為0。
    MemberIndex=0;
    while(1) {
        //調用SetupDiEnumDeviceInterfaces在設備信息集合中獲取編號為
        //MemberIndex的設備信息。
        Result=SetupDiEnumDeviceInterfaces(hDevInfoSet,
                                           NULL,
                                           &HidGuid,
                                           MemberIndex,
                                           &DevInterfaceData);

        //如果獲取信息失敗，則說明設備已經查找完畢，退出循環。
        if(Result==FALSE) break;

        //將MemberIndex指向下一個設備
        MemberIndex++;

        //如果獲取信息成功，則繼續獲取該設備的詳細信息。在獲取設備
        //詳細信息時，需要先知道保存詳細信息需要多大的緩衝區，這通過
        //第一次調用函數SetupDiGetDeviceInterfaceDetail來獲取。這時
        //提供緩衝區和長度都為NULL的參數，並提供一個用來保存需要多大
        //緩衝區的變量RequiredSize。
        Result=SetupDiGetDeviceInterfaceDetail(hDevInfoSet,
                                               &DevInterfaceData,
                                               NULL,
                                               NULL,
                                               &RequiredSize,
                                               NULL);

        //然後，分配一個大小為RequiredSize緩衝區，用來保存設備詳細信息。
        pDevDetailData=(PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(RequiredSize);
        if(pDevDetailData==NULL) { //如果內存不足，則直接返回。
            //MessageBox("內存不足!");
            SetupDiDestroyDeviceInfoList(hDevInfoSet);
            return FALSE;
        }

        //並設置pDevDetailData的cbSize為結構體的大小（注意只是結構體大小，
        //不包括後面緩衝區）。
        pDevDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        //然後再次調用SetupDiGetDeviceInterfaceDetail函數來獲取設備的
        //詳細信息。這次調用設置使用的緩衝區以及緩衝區大小。
        Result=SetupDiGetDeviceInterfaceDetail(hDevInfoSet,
                                               &DevInterfaceData,
                                               pDevDetailData,
                                               RequiredSize,
                                               NULL,
                                               NULL);

        //將設備路徑複製出來，然後銷毀剛剛申請的內存。
        //MyDevPathName=pDevDetailData->DevicePath;
        _tcscpy_s(MyDevPathName, pDevDetailData->DevicePath);
        free(pDevDetailData);

        //如果調用失敗，則查找下一個設備。
        if(Result==FALSE) continue;

        //如果調用成功，則使用不帶讀寫訪問的CreateFile函數
        //來獲取設備的屬性，包括VID、PID、版本號等。
        //對於一些獨佔設備（例如USB鍵盤），使用讀訪問方式是無法打開的，
        //而使用不帶讀寫訪問的格式才可以打開這些設備，從而獲取設備的屬性。
        hDevHandle=CreateFile(MyDevPathName,
                              NULL,
                              FILE_SHARE_READ|FILE_SHARE_WRITE,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);

        //如果打開成功，則獲取設備屬性。
        if(hDevHandle!=INVALID_HANDLE_VALUE) {
            //獲取設備的屬性並保存在DevAttributes結構體中
            Result=HidD_GetAttributes(hDevHandle,
                                      &DevAttributes);

            //關閉剛剛打開的設備
            CloseHandle(hDevHandle);

            //獲取失敗，查找下一個
            if(Result==FALSE) continue;

            //如果獲取成功，則將屬性中的VID、PID以及設備版本號與我們需要的
            //進行比較，如果都一致的話，則說明它就是我們要找的設備。
            if(DevAttributes.VendorID == usVID
                    && ((DevAttributes.ProductID == usPID0)
                        || (DevAttributes.ProductID == usPID1)
                        || (DevAttributes.ProductID == usPID2))) {
                MyDevFound=TRUE; //設置設備已經找到
                //AddToInfOut("設備已經找到");

                if(bUseTwoHandle) {
                    //饒繫憩岆扂蠅猁梑腔扢掘ㄛ煦梗妏蚚黍迡源宒湖羲眳ㄛ甜悵湔む曆梟
                    //甜й恁寁峈祑祭溼恀源宒﹝
                    //黍源宒湖羲扢掘
                    hReadHandle=CreateFile(MyDevPathName,
                                           GENERIC_READ,
                                           FILE_SHARE_READ|FILE_SHARE_WRITE,
                                           NULL,
                                           OPEN_EXISTING,
                                           //FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                                           FILE_ATTRIBUTE_NORMAL,
                                           NULL);
                    //if(hReadHandle!=INVALID_HANDLE_VALUE)AddToInfOut("黍溼恀湖羲扢掘傖髡");
                    //else AddToInfOut("黍溼恀湖羲扢掘囮啖");

                    //迡源宒湖羲扢掘
                    hWriteHandle=CreateFile(MyDevPathName,
                                            GENERIC_WRITE,
                                            FILE_SHARE_READ|FILE_SHARE_WRITE,
                                            NULL,
                                            OPEN_EXISTING,
                                            //FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL);
                    //if(hWriteHandle!=INVALID_HANDLE_VALUE)AddToInfOut("迡溼恀湖羲扢掘傖髡");
                    //else AddToInfOut("迡溼恀湖羲扢掘囮啖");
                } else {
                    hWriteHandle =
                        hReadHandle = CreateFile(MyDevPathName,
                                                 GENERIC_READ | GENERIC_WRITE,
                                                 FILE_SHARE_READ|FILE_SHARE_WRITE,
                                                 NULL,
                                                 OPEN_EXISTING,
                                                 //FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                                                 FILE_ATTRIBUTE_NORMAL,
                                                 NULL);
                }

                m_hReadHandle.push_back(hReadHandle);
                m_hWriteHandle.push_back(hWriteHandle);
                m_sNames.push_back(MyDevPathName);
                //忒雄揖楷岈璃ㄛ�繹螫邪禢蒆抳硌椒刵苤�秪峈婓涴眳ヶ甜羶衄覃蚚
                //黍杅擂腔滲杅ㄛ珩憩祥頗竘れ岈璃腔莉汜ㄛ垀眕剒猁珂忒雄揖楷珨
                //棒岈璃ㄛ�繹螫邪禢蒆抳硌椒刵苤�
                //	SetEvent(ReadOverlapped.hEvent);

                //珆尨扢掘腔袨怓﹝
                //SetDlgItemText(IDC_DS,"扢掘眒湖羲");

                //梑善扢掘ㄛ豖堤悜遠﹝掛最唗硐潰聆珨跺醴梓扢掘ㄛ脤梑善綴憩豖堤
                //脤梑賸﹝�蝜�斕剒猁蔚垀衄腔醴梓扢掘飲蹈堤懂腔趕ㄛ褫眕扢离珨跺
                //杅郪ㄛ梑善綴憩悵湔婓杅郪笢ㄛ眻善垀衄扢掘飲脤梑俇救符豖堤脤梑
                //break;
                //樟哿脤梑
            }
        }
        //如果打開失敗，則查找下一個設備
        else continue;
    }

    //調用SetupDiDestroyDeviceInfoList函數銷毀設備信息集合
    SetupDiDestroyDeviceInfoList(hDevInfoSet);

    this->SetActiveDevice(0);
    //如果設備已經找到，那麼應該使能各操作按鈕，並同時禁止打開設備按鈕
    return MyDevFound;
}


BOOL CHidIO::ReadFile(char *pcBuffer, size_t szMaxLen, DWORD *pdwLength, DWORD dwMilliseconds)
{
    if(m_szActiveDeviceIndex >= m_hReadHandle.size())
        return FALSE;
    HANDLE hReadHandle = m_hReadHandle[m_szActiveDeviceIndex];
    HANDLE events[2] = {m_hAbordEvent, m_hReadEvent};

    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = m_hReadEvent;

    if(pdwLength != NULL)
        *pdwLength = 0;

    if(!::ReadFile(hReadHandle, pcBuffer, szMaxLen, NULL, &overlapped)) {
        DWORD dwError = ::GetLastError();
        if(dwError != ERROR_IO_PENDING) {
            return FALSE;
        }

        /* Wait for pending IO event */
        DWORD dwIndex = WaitForMultipleObjects(2, events, FALSE, dwMilliseconds);
        if(dwIndex == WAIT_OBJECT_0
                || dwIndex == WAIT_OBJECT_0 + 1) {
            ResetEvent(events[dwIndex - WAIT_OBJECT_0]);
            if(dwIndex == WAIT_OBJECT_0) {
                return FALSE;	//Abort event
            }
        } else {
            return FALSE;
        }
    }


    DWORD dwLength = 0;
    //Read OK
    GetOverlappedResult(hReadHandle, &overlapped, &dwLength, TRUE);
    if(pdwLength != NULL)
        *pdwLength = dwLength;

    return TRUE;
}

BOOL CHidIO::WriteFile(const char *pcBuffer, size_t szLen, DWORD *pdwLength, DWORD dwMilliseconds)
{
    if(m_szActiveDeviceIndex >= m_hWriteHandle.size())
        return FALSE;
    HANDLE hWriteHandle = m_hWriteHandle[m_szActiveDeviceIndex];
    HANDLE events[2] = {m_hAbordEvent, m_hWriteEvent};

    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = m_hWriteEvent;

    if(pdwLength != NULL)
        *pdwLength = 0;

    if(!::WriteFile(hWriteHandle, pcBuffer, szLen, NULL, &overlapped)) {
        if(::GetLastError() != ERROR_IO_PENDING)
            return FALSE;

        DWORD dwIndex = WaitForMultipleObjects(2, events, FALSE, dwMilliseconds);
        if(dwIndex == WAIT_OBJECT_0
                || dwIndex == WAIT_OBJECT_0 + 1) {
            ResetEvent(events[dwIndex - WAIT_OBJECT_0]);

            if(dwIndex == WAIT_OBJECT_0)
                return FALSE;	//Abort event
        } else
            return FALSE;
    }

    DWORD dwLength = 0;
    //Write OK
    GetOverlappedResult(hWriteHandle, &overlapped, &dwLength, TRUE);
    if(pdwLength != NULL)
        *pdwLength = dwLength;

    return TRUE;
}

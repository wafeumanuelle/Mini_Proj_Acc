// (c) 2013-2023 Marius VASILIU
// SATIE/Polytech Univ. PARIS-SACLAY
// based on JoyWarrior 24F8 datasheet

// rajouter au niveau du projet (C++/preproc) les macros suivantes :
// _CRT_SECURE_NO_WARNINGS;_WIN32_WINNT=0x0501;WINVER=0x0501
// pour émuler un seul capteur virtuel définir la macro VIRTUAL_SENSOR
#ifndef _ACC_SENSOR
#define _ACC_SENSOR

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#define _USE_MATH_DEFINES
#include <math.h>

extern "C" { 
#include "hidsdi.h" 
#include "setupapi.h"
}
#include <Dbt.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"hid.lib")
#pragma comment(lib,"setupapi.lib")

#pragma pack(push)
#pragma pack(1)
struct SensData // 8 bytes via 1st interface
{
  BYTE b0;
  WORD ax, ay, az;
  BYTE b1;
};
#pragma pack(pop)

//#define VIRTUAL_SENSOR
#ifdef VIRTUAL_SENSOR
class AccSensor
{
  static const DWORD DTIME=1000;       // change position every second
  static const DWORD EJECT_TIME=50000; // eject after 50s
  static const int ACC_MAX=200;        // acceleration magnitude
  static const int NOISE_MAX=10;       // noise magnitude
  
  DWORD last_t, last_t_st, first_t;
  short curr_coef[3];
  short last_acc[3];
public:
  static const short G_ACC=256;
  HANDLE hdev[2];
  HIDP_CAPS caps[2];
  ULONG nserial;
  AccSensor() {memset(this,0,sizeof(*this));}
  ~AccSensor() {}
  void ShowInfo(FILE* pf)
  {
    fprintf(pf,"  Virtual (fake) sensor, no serial number\n");
  }
  bool ShowSensData()
  {
    return true;
  }
  bool ReadSensData(SensData& data)
  {
    return true;
  }
  bool ReadAcc(short* a)
  {
    DWORD curr_t=::GetCurrentTime();
    if(last_t)
    {
      if(curr_t > first_t+EJECT_TIME) return false; 
      if(curr_t > last_t_st+DTIME)
      {
        last_t_st=curr_t;
        if((rand() & 3)<1)
        {
          int curr_st=rand();
          curr_coef[0]=(short)(ACC_MAX*cos(((curr_st & 0x7F00)>> 8)*M_PI/0x7F));
          curr_coef[1]=(short)(ACC_MAX*sin(((curr_st & 0x7F00)>> 8)*M_PI/0x7F)*sin((curr_st & 0x00FF)*M_PI/0x80));
          curr_coef[2]=(short)(ACC_MAX*sin(((curr_st & 0x7F00)>> 8)*M_PI/0x7F)*cos((curr_st & 0x00FF)*M_PI/0x80));
        }
      }
    }
    else
    {
      first_t=last_t_st=curr_t;
    }
    for(int i=0; i<3; i++)
    {
      last_acc[i]=a[i]=(curr_coef[i]+last_acc[i])/2;
      a[i]+=(rand()%(2*NOISE_MAX))-NOISE_MAX;
    }
    last_t=curr_t;
    return true;
  }
  bool ReadAcc(short* ax, short* ay, short* az)
  {
    return true;
  }
  bool ReadAcc(float* a) // 3-float table
  {
    return true;
  }
  bool ReadAcc(float* ax, float* ay, float* az)
  {
    return true;
  }
  bool ReadNormAcc(float* a)
  {
    return true;
  }
  bool ReadNormAcc(float* ax, float* ay, float* az)
  {
    return true;
  }
  BYTE ReadData(BYTE adr)
  {
    return 0;
  }
  static void GetPreparsedData(HANDLE handle, HIDP_CAPS& Capabilities) // Get informations from device
  {
    PHIDP_PREPARSED_DATA PreparsedData;
    HidD_GetPreparsedData(handle, &PreparsedData);
    HidP_GetCaps(PreparsedData, &Capabilities);
    HidD_FreePreparsedData(PreparsedData);
  }
};

// class to manage an unique virtual sensor
class AccManager
{
  AccSensor sens;
public:
  AccManager(bool _asynch=false) {}
  ~AccManager() {}
  bool MustReset() {return false;}
  DWORD GetNbSensors(bool _reset=false) { return 1; }
  AccSensor* GetSensor(DWORD idx) { return (idx<1) ? &sens:0; }
};

#else // VIRTUAL_SENSOR not defined

class AccSensor
{
  // "built-in" USB interfaces capabilities :
  static const DWORD w0_len=0; // write packet length (1st interface) 
  static const DWORD r0_len=8; // read packet length (1st interface) 
  static const DWORD w1_len=9; // write packet length (2nd interface) 
  static const DWORD r1_len=9; // read packet length (2nd interface) 
public:
  static const short G_ACC=256;
  HANDLE hdev[2];
  HIDP_CAPS caps[2];
  ULONG nserial;
  AccSensor() {memset(this,0,sizeof(*this));}
  ~AccSensor() {if(hdev[0]) CloseHandle(hdev[0]); if(hdev[1]) CloseHandle(hdev[1]); }
  void ShowInfo(FILE* pf)
  {
    fprintf(pf,"  SerNumb=%04lX, dev0=%p (%hu,%hu,%hu), dev1=%p (%hu,%hu,%hu)\n",nserial,hdev[0],caps[0].InputReportByteLength,caps[0].OutputReportByteLength,caps[0].FeatureReportByteLength,
      hdev[1],caps[1].InputReportByteLength,caps[1].OutputReportByteLength,caps[1].FeatureReportByteLength);
  }
  bool ShowSensData()
  {
    DWORD NumberOfBytesRead = 0;
    SensData data;
    if(!ReadFile(hdev[0], &data, sizeof(SensData), &NumberOfBytesRead, 0)|| NumberOfBytesRead!=sizeof(SensData)) return false;
//    printf("(%5hu,%5hu,%5hu) %02hX %02hx\n",data.ax,data.ay,data.az,data.b0,data.b1);
    printf("(%5hd,%5hd,%5hd)\n",((short)data.ax)-511,((short)data.ay)-511,((short)data.az)-511);
    return true;
  }
  bool ReadSensData(SensData& data)
  {
    DWORD NumberOfBytesRead = 0;
    if(!ReadFile(hdev[0], &data, sizeof(SensData), &NumberOfBytesRead, 0)|| NumberOfBytesRead!=sizeof(SensData)) return false;
    return true;
  }
  bool ReadAcc(short* a)
  {
    DWORD NumberOfBytesRead = 0;
    SensData data;
    if(!ReadFile(hdev[0], &data, sizeof(SensData), &NumberOfBytesRead, 0)|| NumberOfBytesRead!=sizeof(SensData)) return false;
    a[0]=((short)data.ax)-511; a[1]=((short)data.ay)-511; a[2]=((short)data.az)-511;
    return true;
  }
  bool ReadAcc(short* ax, short* ay, short* az)
  {
    DWORD NumberOfBytesRead = 0;
    SensData data;
    if(!ReadFile(hdev[0], &data, sizeof(SensData), &NumberOfBytesRead, 0)|| NumberOfBytesRead!=sizeof(SensData)) return false;
    *ax=((short)data.ax)-511; *ay=((short)data.ay)-511; *az=((short)data.az)-511;
    return true;
  }
  bool ReadAcc(float* a) // 3-float table
  {
    DWORD NumberOfBytesRead = 0;
    SensData data;
    if(!ReadFile(hdev[0], &data, sizeof(SensData), &NumberOfBytesRead, 0)|| NumberOfBytesRead!=sizeof(SensData)) return false;
    a[0]=(((short)data.ax)-511.0f)/512; a[1]=(((short)data.ay)-511.0f)/512; a[2]=(((short)data.az)-511.0f)/512;
    return true;
  }
  bool ReadAcc(float* ax, float* ay, float* az)
  {
    DWORD NumberOfBytesRead = 0;
    SensData data;
    if(!ReadFile(hdev[0], &data, sizeof(SensData), &NumberOfBytesRead, 0)|| NumberOfBytesRead!=sizeof(SensData)) return false;
    *ax=(((short)data.ax)-511.0f)/512; *ay=(((short)data.ay)-511.0f)/512; *az=(((short)data.az)-511.0f)/512;
    return true;
  }
  bool ReadNormAcc(float* a)
  {
    DWORD NumberOfBytesRead = 0;
    SensData data;
    if(!ReadFile(hdev[0], &data, sizeof(SensData), &NumberOfBytesRead, 0)|| NumberOfBytesRead!=sizeof(SensData)) return false;
    a[0]=((short)data.ax)-511.0f; a[1]=((short)data.ay)-511.0f; a[2]=((short)data.az)-511.0f;
    float norm=(float)(1/sqrt((a[0]*a[0]+a[1]*a[1]+a[2]*a[2])));
    a[0]*=norm; a[1]*=norm; a[2]*=norm;
    return true;
  }
  bool ReadNormAcc(float* ax, float* ay, float* az)
  {
    DWORD NumberOfBytesRead = 0;
    SensData data;
    if(!ReadFile(hdev[0], &data, sizeof(SensData), &NumberOfBytesRead, 0)|| NumberOfBytesRead!=sizeof(SensData)) return false;
    *ax=((short)data.ax)-511.0f; *ay=((short)data.ay)-511.0f; *az=((short)data.az)-511.0f;
    float norm=1/(*ax**ax+*ay**ay+*az**az);
    *ax*=norm; *ay*=norm; *az*=norm;
    return true;
  }
  BYTE ReadData(BYTE adr)
  {
    HidD_FlushQueue(hdev[1]);
    BYTE wbuf[w1_len]={0x00,0x82,adr | 0x80,0x00,0x00,0x00,0x00,0x00,0x00};
    DWORD nb_rw=0;
    if(!WriteFile(hdev[1],wbuf,w1_len,&nb_rw,0) || nb_rw!=w1_len) return 0;
    BYTE rbuf[r1_len];
    nb_rw=0;
    if(!ReadFile(hdev[1],rbuf,r1_len,&nb_rw,0) || nb_rw!=r1_len) return 0;
    return rbuf[3];
  }
  static void GetPreparsedData(HANDLE handle, HIDP_CAPS& Capabilities) // Get informations from device
  {
    PHIDP_PREPARSED_DATA PreparsedData;
    HidD_GetPreparsedData(handle, &PreparsedData);
    HidP_GetCaps(PreparsedData, &Capabilities);
    HidD_FreePreparsedData(PreparsedData);
  }
};

// class to manage all acceleration sensors
class AccManager
{
public:
  static const ULONG CM_Pid=0x1113;
  static const ULONG CM_Vid=0x07C0;
private:
  static int EnumHidDevices(ULONG productID=CM_Pid, ULONG vendorID=CM_Vid) // Enumerate and count all specified devices
  {
    GUID HidGuid;
    HidD_GetHidGuid(&HidGuid);	

    HDEVINFO hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    SP_DEVICE_INTERFACE_DATA devInfoData;
    ZeroMemory(&devInfoData, sizeof(devInfoData));
    devInfoData.cbSize = sizeof(devInfoData);
    int	MemberIndex = 0, nbdev=0;
    while(SetupDiEnumDeviceInterfaces (hDevInfo, NULL, &HidGuid, MemberIndex, &devInfoData))
    {
      ULONG Length = 0;
      SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);
      PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(Length);
      detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
      ULONG Required;
      SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Length, &Required, NULL);

      HANDLE DeviceHandle = CreateFile(detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
      if(DeviceHandle==INVALID_HANDLE_VALUE)
      {
        // printf("Error at %lu: can't open %S\n",MemberIndex,detailData->DevicePath);
        free(detailData);
        MemberIndex++;
        continue;
      }
      HIDD_ATTRIBUTES	Attributes;
      Attributes.Size = sizeof(Attributes);
      HidD_GetAttributes(DeviceHandle, &Attributes);
      if(Attributes.VendorID == vendorID && Attributes.ProductID == productID)
      {
        WCHAR sn[32]; memset(&sn, 0, 32);
        HidD_GetSerialNumberString(DeviceHandle, &sn, 32);
        ULONG sn_number=0;
        swscanf(sn,L"%08lX",&sn_number);
//        printf("%4d: HID Vid=%04hX Pid=%04hX Rev=%04hX, serial=%S, sn=%08lX\n",MemberIndex,Attributes.VendorID,Attributes.ProductID,Attributes.VersionNumber,sn,sn_number);
        nbdev++;
      }
      CloseHandle(DeviceHandle);
      free(detailData);
      MemberIndex++;
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return nbdev>>1; // any acc. sensor shown 2 HID devices/inerfaces
  }
  void OpenAllHidDevices(ULONG productID=CM_Pid, ULONG vendorID=CM_Vid) // Enumerate all specified devices and store their serialnumber
  {
    if(!sens) return;
    GUID HidGuid;
    HidD_GetHidGuid(&HidGuid);	

    HDEVINFO hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    SP_DEVICE_INTERFACE_DATA devInfoData;
    ZeroMemory(&devInfoData, sizeof(devInfoData));
    devInfoData.cbSize = sizeof(devInfoData);
    DWORD	MemberIndex = 0, nbdev=0;
    while(SetupDiEnumDeviceInterfaces (hDevInfo, NULL, &HidGuid, MemberIndex, &devInfoData))
    {
      ULONG Length = 0;
      SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);
      PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(Length);
      detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
      ULONG Required;
      SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Length, &Required, NULL);

      HANDLE DeviceHandle = CreateFile(detailData->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
      if(DeviceHandle==INVALID_HANDLE_VALUE)
      {
        // printf("Error at %lu: can't open %S\n",MemberIndex,detailData->DevicePath);
        free(detailData);
        MemberIndex++;
        continue;
      }
      HIDD_ATTRIBUTES	Attributes;
      Attributes.Size = sizeof(Attributes);
      HidD_GetAttributes(DeviceHandle, &Attributes);
      if(Attributes.VendorID == vendorID && Attributes.ProductID == productID)
      {
        WCHAR sn[32]; memset(&sn, 0, 32);
        BOOLEAN res=HidD_GetSerialNumberString(DeviceHandle, &sn, 32);
        //if(!res) printf("Error from HidD_GetSerialNumberString\n");
        ULONG sn_number=0; swscanf(sn,L"%08lX",&sn_number); // sensors have 16 bits hex serials
        //printf("New serial at %lu: %08lX\n",MemberIndex,sn_number);
        int idx=-1;
        for(DWORD i=0; i<nbdev; i++) if(sn_number==sens[i].nserial) {idx=i; break;}
        if(idx==-1) // found new device/interface of new sensor
        {
          //printf("found new device/interface of new sensor\n");
          _ASSERT(nbdev<nbsens);
          sens[nbdev].nserial=sn_number;
          sens[nbdev].hdev[0]=DeviceHandle;
          AccSensor::GetPreparsedData(DeviceHandle,sens[nbdev].caps[0]);
          _ASSERT(sens[nbdev].caps[0].InputReportByteLength==sizeof(SensData));
          nbdev++;
        }
        else // found second device/interface of old sensor
        {
          //printf("found second device/interface of old sensor\n");
          _ASSERT(sens[idx].hdev[0]!=0);
          sens[idx].hdev[1]=DeviceHandle;
          AccSensor::GetPreparsedData(DeviceHandle,sens[idx].caps[1]);
        }
      }
      else CloseHandle(DeviceHandle);
      free(detailData);
      MemberIndex++;
    }
    _ASSERT(nbdev==nbsens);
    SetupDiDestroyDeviceInfoList(hDevInfo);
  }
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wP, LPARAM lP)
  {
    if(uMsg==WM_DEVICECHANGE)
    {
//      printf("Thread  = %08lX, ThID= %08lX\n",::GetCurrentThread(),::GetCurrentThreadId());
      DWORD v=DBT_DEVTYP_DEVICEINTERFACE;
//      printf("Message WM_DEVICECHANGE: WP=%08lX LP=%08lX DevType=%lu\n",wP,lP,((DEV_BROADCAST_HDR *)lP)->dbch_devicetype);
      switch(wP)
      {
      case DBT_DEVICEARRIVAL:
//        printf(" Sensor inserted: %S\n",((DEV_BROADCAST_DEVICEINTERFACE*)lP)->dbcc_name);
        ((AccManager*)(LONG_PTR)GetWindowLongPtr(hwnd,GWLP_USERDATA))->must_reset=true;
        break;
      case DBT_DEVICEREMOVECOMPLETE:
//        printf(" Sensor removed: %S\n",((DEV_BROADCAST_DEVICEINTERFACE*)lP)->dbcc_name);
        ((AccManager*)(LONG_PTR)GetWindowLongPtr(hwnd,GWLP_USERDATA))->must_reset=true;
        break;
      case DBT_DEVNODES_CHANGED:
//        printf(" Node changed: \n");
        ((AccManager*)(LONG_PTR)GetWindowLongPtr(hwnd,GWLP_USERDATA))->must_reset=true;
        break;
      default: ;
      }
    }
    return ::DefWindowProc(hwnd,uMsg,wP,lP);
  }
  static DWORD WINAPI DevProcTh(LPVOID arg)
  {
    ((AccManager*)arg)->DevProc();
    return 0;
  }
  // create a special hide window for receiving device messages
  void DevProc()
  {
//    printf("Thread  = %08lX, ThID= %08lX\n",::GetCurrentThread(),::GetCurrentThreadId());
    GUID HidGuid;
    HidD_GetHidGuid(&HidGuid);	
    DEV_BROADCAST_DEVICEINTERFACE bdi={sizeof(DEV_BROADCAST_DEVICEINTERFACE),DBT_DEVTYP_DEVICEINTERFACE,0,HidGuid,{0}};
    HWND hw=::CreateWindow(L"Message",L"RecMsg",0,0,0,0,0, HWND_MESSAGE,0,0,0);
    SetWindowLongPtr(hw,GWLP_WNDPROC,(LONG)(LONG_PTR)WndProc);
    SetWindowLongPtr(hw,GWLP_USERDATA,(LONG)(LONG_PTR)this);
    HDEVNOTIFY hdevnot=RegisterDeviceNotification(hw,&bdi,DEVICE_NOTIFY_WINDOW_HANDLE);
    MSG msg;
    while(::GetMessage(&msg,0,0,0)) ::DispatchMessage(&msg);
    UnregisterDeviceNotification(hdevnot);
    ::DestroyWindow(hw);
//    printf("End of DevProc thread\n");
  }
  void Clean()
  {
    if(nbsens==0) return;
    nbsens=0;
    if(sens) delete[] sens; sens=0;
  }
  void Reset()
  {
    Clean();
    nbsens=EnumHidDevices();
    sens=new AccSensor[nbsens];
    OpenAllHidDevices();
    must_reset=false;
  }
  bool must_reset;
  DWORD th_id, nbsens;
  AccSensor* sens;
public:
  // if _asynch is true, a special thread supervises HID devices status and notify any change in must_reset attribute
  AccManager(bool _asynch=false): th_id(0), nbsens(0), sens(0), must_reset(true)
  {
//    printf("Thread  = %08lX, ThID= %08lX\n",::GetCurrentThread(),::GetCurrentThreadId());
    if(_asynch)
    {
      HANDLE hd_th=::CreateThread(0,0,DevProcTh,this,0,&th_id);
      ::CloseHandle(hd_th);
    }
    Reset();
  }
  ~AccManager()
  {
    if(th_id)
    {
      ::PostThreadMessage(th_id,WM_QUIT,0,0);
      Sleep(100);
    }
    Clean();
  }
  bool MustReset() {return must_reset;}
  DWORD GetNbSensors(bool _reset=false) { if(_reset || must_reset) Reset(); return nbsens; } //donne le nombre de capteurs disponible
  AccSensor* GetSensor(DWORD idx) { return (idx<nbsens)?(sens+idx):0; } //il nous donne un pointeur qui va nous permettre de communiquer avec le capteur, on n'a pas besion de detruire de pointeur à la fin

  // methode statique de test en mode console
  static void Test1()
  {
    AccManager accmgr(true);
    AccSensor* sens=0;
    for(int i=0; i<1000; i++)
    {
      Sleep(100);
      if(i==0 || accmgr.MustReset())
      {
        Sleep(1000);
        DWORD nbs=accmgr.GetNbSensors();
        if(nbs==0) {printf("Info : pas de capteur, en attente ...\n"); sens=0;}
        else
        {
          printf("Info : au moins un capteur detecte :\n");
          sens=accmgr.GetSensor(0);
          sens->ShowInfo(stdout);
        }
      }
      if(sens)
      {
        float fv[3];
        if(!sens->ReadAcc(fv)) {printf("Erreur: lecture de l'accelaration\n"); sens=0;}
        else printf("(%+6.3f, %+6.3f, %+6.3f)=%+6.3f\n",fv[0],fv[1],fv[2], sqrt(fv[0]*fv[0]+fv[1]*fv[1]+fv[2]*fv[2]));
      }
    }
  }
};
#endif // VIRTUAL_SENSOR
#endif // _ACC_SENSOR

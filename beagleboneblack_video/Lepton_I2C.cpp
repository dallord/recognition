#include "Lepton_I2C.h"

#include "leptonSDKEmb32PUB/LEPTON_SDK.h"
#include "leptonSDKEmb32PUB/LEPTON_SYS.h"
#include "leptonSDKEmb32PUB/LEPTON_VID.h"
#include "leptonSDKEmb32PUB/LEPTON_AGC.h"
#include "leptonSDKEmb32PUB/LEPTON_Types.h"

bool _connected;

LEP_CAMERA_PORT_DESC_T _port;
LEP_SYS_SCENE_STATISTICS_T _st;
LEP_SYS_VIDEO_ROI_T _roi;
LEP_SYS_AUX_TEMPERATURE_KELVIN_T _t;

LEP_SYS_FFC_SHUTTER_MODE_OBJ_T ffc;


int lepton_connect() {
    LEP_OpenPort(1, LEP_CCI_TWI, 400, &_port);
    //LEP_SetSysTelemetryEnableState(&_port, LEP_TELEMETRY_ENABLED);
    //qDebug() << LEP_RunCommand(&_port, 0x1200);
    //LEP_SetVidFocusCalcEnableState(&_port, LEP_VID_FOCUS_CALC_ENABLE);
    //LEP_SetRadEnableState(&_port, LEP_RAD_ENABLE);
    _connected = true;
    return 0;
}

void lepton_perform_ffc() {
    if(!_connected) {
        lepton_connect();
    }
    //LEP_RunSysFFCNormalization(&_port);
    LEP_SetAgcEnableState(&_port, LEP_AGC_DISABLE);
}

void lepton_set_roi(int s_c, int e_c, int s_r, int e_r){
    if(!_connected) {
        lepton_connect();
    }
    _roi.startCol = s_c;
    _roi.endCol = e_c;
    _roi.startRow = s_r;
    _roi.endRow = e_r;

    LEP_SetSysSceneRoi(&_port, _roi);
}

int lepton_get_roi(int type){
    if(!_connected) {
        lepton_connect();
    }
    LEP_GetSysSceneRoi(&_port, &_roi);
    switch (type)
    {
    case 0:
        return _roi.startCol;
        break;
    case 1:
        return _roi.endCol;
        break;
    case 2:
        return _roi.startRow;
        break;
    case 3:
        return _roi.endRow;
        break;
    default:
        return 0;
    }
}

int lepton_scene_statistic(int type){
    if(!_connected) {
        lepton_connect();
    }
    LEP_GetSysSceneStatistics(&_port, &_st);
    //qDebug() << _st.minIntensity << " " << _st.maxIntensity;
    switch (type)
    {
    case 0:
        return _st.meanIntensity;
        break;
    case 1:
        return _st.minIntensity;
        break;
    case 2:
        return _st.maxIntensity;
        break;
    case 3:
        return _st.numPixels;
        break;
    default:
        return 0;
    }
}

int lepton_get_temperature(){
    if(!_connected) {
        lepton_connect();
    }
    LEP_GetSysAuxTemperatureKelvin(&_port, &_t);
    return _t;
}

void test(){
/*
    if(!_connected) {
        lepton_connect();
    }*/
    qDebug() << "ok";
   // LEP_SetAgcHeqClipLimitLow(&_port, 1024);
   // LEP_SetAgcEnableState(&_port, LEP_AGC_ENABLE);



}



#ifndef _TPCONNECTION_H_
#define _TPCONNECTION_H_

#include <wx/wxprec.h>
#include <wx/socket.h>

#include <iostream>

using namespace std;


#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class TPConnection
{
    private:
        struct TPInfoSt
        {
            wxString type;
            wxString sdkVersion;
            wxString tpVersionString;
            wxString tpVersionCode;
            wxString pluginVersion;
            wxString status;
        };
        struct TPJSONSt
        {
            wxString type;
            wxString pluginId;
            wxString actionId;
            wxString listId;
            wxString listValue;
            wxString instanceId;
            wxString event;
            wxString pageName;
            wxArrayString dataId;
            wxArrayString dataValue;
        };
        TPJSONSt TPEvents[30];
        TPInfoSt TPInfo;
        int TPEventCount, OpenSBracketCount, OpenBracketCount;
        bool EscapeCharacter, StartKeySave, StartValueSave, KeySaved, DataArray, InfoResponse;
        wxIPV4address TPAddr;
        wxSocketClient *TPSocket;
        wxString pID;
        wxString rData;

        void ClearPreviousData();
        void StoreKeyValue(wxString,wxString);

    public:
        TPConnection();
        TPConnection(wxString);
        ~TPConnection();
        bool IsConnected() { return TPSocket->IsConnected(); };
        bool Error() { return TPSocket->Error(); };
        bool IsData();

        //Only Use For Debugging
        wxString ReadData() { return rData; };

        //Set States/Lists In Touch Portal
        void UpdateState(wxString, wxString);
        void UpdateList(wxString, wxArrayString);
        void UpdateList(wxString, wxString, wxArrayString);

        //Get Version Info And Connection Status
        wxString GetSDKVersion() { return TPInfo.sdkVersion; }
        wxString GetTPVersionString() { return TPInfo.tpVersionString; }
        long GetTPVersionCode() { long n; TPInfo.tpVersionCode.ToLong(&n); return n; }
        double GetTPPluginVersion() { double n; TPInfo.pluginVersion.ToDouble(&n); return n; }
        wxString GetTPPluginStatus() { return TPInfo.status; }

        //Get Amount Of Events Currently Received
        int GetTPeCount() { return TPEventCount; }

        //Get Information Of The Events Received
        wxString GetTPeType(int i) { return TPEvents[i].type; }
        wxString GetTPePluginId(int i) { return TPEvents[i].pluginId; }
        wxString GetTPeActionId(int i) { return TPEvents[i].actionId; }
        wxString GetTPeListId(int i) { return TPEvents[i].listId; }
        wxString GetTPeListValue(int i) { return TPEvents[i].listValue; }
        wxString GetTPeInstanceId(int i) { return TPEvents[i].instanceId; }
        wxString GetTPeEvent(int i) { return TPEvents[i].event; }
        wxString GetTPePageName(int i) { return TPEvents[i].pageName; }
        int GetTPeDataCount(int i) { return TPEvents[i].dataId.GetCount(); }
        wxString GetTPeDataId(int i, int c) { return TPEvents[i].dataId[c]; }
        wxString GetTPeDataValue(int i, int c) { return TPEvents[i].dataValue[c]; }

};

#endif

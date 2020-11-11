#include "TPConnection.h"


//Default Constructor ( Do Not Use )
TPConnection::TPConnection()
{

}

//Constructor You Should Use When Connecting
TPConnection::TPConnection(wxString id)
{
    pID = id;

    //Get LocalHost Address
    TPAddr.LocalHost();

    //Set Touch Portal Plugin Port = 12136;
    TPAddr.Service(12136);

    //Create A Socket To Connect To Touch Portal
    TPSocket = new wxSocketClient();

    //Connect To Touch Portal
    TPSocket->Connect(TPAddr);
    if (TPSocket->IsConnected())
        cout << (wxT("Connected to ") + TPAddr.IPAddress()) << endl;
    else
        cout << (wxT("Can not connect to ") + TPAddr.IPAddress()) << endl;

    //Create Pairing JSON Request
    wxString request = wxT("{ \"type\":\"pair\", \"id\":\"") + pID + wxT("\" }\n");

    //Send The Request To Touch Portal
    InfoResponse = true;
    TPSocket->Write(request.mb_str(), request.Length());
    cout << "Request sent" << endl;
}

//Public Function - Check For Data And Parse If Data Found
bool TPConnection::IsData()
{
    if(TPSocket->IsData())
    {
        ClearPreviousData();
        char c = 0x00;
        wxString tKey;
        wxString tValue;

        while( TPSocket->IsConnected() && !TPSocket->Error() && TPSocket->IsData())
        {
            TPSocket->Read(&c, 1);
            rData.Append((wxChar)c, 1);
            switch(c)
            {
                case '{':
                    if(!EscapeCharacter)
                        if(!StartValueSave && !StartKeySave)
                            OpenSBracketCount += 1;
                    break;
                case '}':
                    if(!EscapeCharacter)
                    {
                        if(!StartValueSave && !StartKeySave)
                            OpenSBracketCount -= 1;
                        if(StartValueSave)
                            { StartValueSave = false; StoreKeyValue(tKey,tValue); tKey.Empty(); tValue.Empty(); }
                    }
                    break;
                case '[':
                    if(!EscapeCharacter)
                        { OpenBracketCount += 1; StartKeySave = false; StartValueSave = false; tKey.Empty(); tValue.Empty(); }
                    break;
                case ']':
                    if(!EscapeCharacter)
                        OpenBracketCount -= 1;
                    break;
                case '\\':
                    if(!EscapeCharacter)
                        EscapeCharacter = true;
                    break;
                case '"':
                    if(!EscapeCharacter)
                        if(!StartKeySave && !StartValueSave)
                            StartKeySave = true;
                    break;
                case ':':
                    if(!EscapeCharacter)
                        if(!StartValueSave)
                            { StartValueSave = true; StartKeySave = false; }
                    break;
                case ',':
                    if(!EscapeCharacter)
                        if(StartValueSave)
                            { StartValueSave = false; StoreKeyValue(tKey,tValue); tKey.Empty(); tValue.Empty(); }
                    break;
                case '\n':
                    TPEventCount++;
                    break;
                default:
                    if(StartKeySave)
                        tKey.Append((wxChar)c, 1);
                    if(StartValueSave)
                        tValue.Append((wxChar)c, 1);
                    break;
            }
        }
        if(InfoResponse)
            { InfoResponse = false; TPEventCount = 0; }
        return true;
    }
    return false;
}

//Public Function - Update Touch Portal State
void TPConnection::UpdateState(wxString id, wxString value)
{
    wxString request = wxT("{\"type\":\"stateUpdate\",\"id\":\"") + id + wxT("\",\"value\":\"") + value + wxT("\"}\n");
    if(TPSocket->IsConnected())
        TPSocket->Write(request.mb_str(), request.Length());
}

//Public Function - Update List Without Instance Id
void TPConnection::UpdateList(wxString id, wxArrayString valueList)
{
    wxString request = wxT("{\"type\":\"choiceUpdate\",\"id\":\"") + id + wxT("\",\"value\":[");
    for(int i = 0; i < valueList.GetCount(); i++)
        if(i == (valueList.GetCount() - 1))
            request.Append(wxT("\"") + valueList[i] + wxT("\""));
        else
            request.Append(wxT("\"") + valueList[i] + wxT("\","));
    request.Append(wxT("]}\n"));
    if(TPSocket->IsConnected())
        TPSocket->Write(request.mb_str(), request.Length());
}

//Public Function - Update List With Instance Id
void TPConnection::UpdateList(wxString id, wxString instanceId, wxArrayString valueList)
{
    wxString request = wxT("{\"type\":\"choiceUpdate\",\"id\":\"") + id + wxT("\",\"instanceId\":\"") + instanceId + wxT("\",\"value\":[");
    for(int i = 0; i < valueList.GetCount(); i++)
        if(i == (valueList.GetCount() - 1))
            request.Append(wxT("\"") + valueList[i] + wxT("\""));
        else
            request.Append(wxT("\"") + valueList[i] + wxT("\","));
    request.Append(wxT("]}\n"));
    if(TPSocket->IsConnected())
        TPSocket->Write(request.mb_str(), request.Length());
}

//Private Function - Save Keys And Values
void TPConnection::StoreKeyValue(wxString k, wxString v)
{
    if(k.IsSameAs(wxT("type")) && v.IsSameAs(wxT("closePlugin")))
    {
        TPSocket->Close();
        return;
    }

    if(InfoResponse)
    {
        if(k.IsSameAs(wxT("type")))
            { TPInfo.type = v; return; }
        if(k.IsSameAs(wxT("tpVersionString")))
            { TPInfo.tpVersionString = v; return; }
        if(k.IsSameAs(wxT("pluginVersion")))
            { TPInfo.pluginVersion = v; return; }
        if(k.IsSameAs(wxT("tpVersionCode")))
            { TPInfo.tpVersionCode = v; return; }
        if(k.IsSameAs(wxT("sdkVersion")))
            { TPInfo.sdkVersion = v; return; }
        if(k.IsSameAs(wxT("status")))
            { TPInfo.status = v; return; }
    }
    else
    {
        if(OpenBracketCount > 0)
        {
            if(k.IsSameAs(wxT("id")))
                { TPEvents[TPEventCount].dataId.Add(v); return; }
            if(k.IsSameAs(wxT("value")))
                { TPEvents[TPEventCount].dataValue.Add(v); return; }
        }
        else
        {
            if(k.IsSameAs(wxT("type")))
                { TPEvents[TPEventCount].type = v; return; }
            if(k.IsSameAs(wxT("pluginId")))
                { TPEvents[TPEventCount].pluginId = v; return; }
            if(k.IsSameAs(wxT("actionId")))
                { TPEvents[TPEventCount].actionId = v; return; }
            if(k.IsSameAs(wxT("listId")))
                { TPEvents[TPEventCount].listId = v; return; }
            if(k.IsSameAs(wxT("value")))
                { TPEvents[TPEventCount].listValue = v; return; }
            if(k.IsSameAs(wxT("instanceId")))
                { TPEvents[TPEventCount].instanceId = v; return; }
            if(k.IsSameAs(wxT("event")))
                { TPEvents[TPEventCount].event = v; return; }
            if(k.IsSameAs(wxT("pageName")))
                { TPEvents[TPEventCount].pageName = v; return; }
        }
    }
}

//Private Function Clear Previously Received Data
void TPConnection::ClearPreviousData()
{
    if(TPEventCount > 0)
    {
        for(int i = 0; i < TPEventCount; i++)
        {
            TPEvents[i].type.Empty();
            TPEvents[i].pluginId.Empty();
            TPEvents[i].actionId.Empty();
            TPEvents[i].listId.Empty();
            TPEvents[i].listValue.Empty();
            TPEvents[i].instanceId.Empty();
            TPEvents[i].event.Empty();
            TPEvents[i].pageName.Empty();
            TPEvents[i].dataId.Clear();
            TPEvents[i].dataValue.Clear();
        }
    }
    rData.Empty();
    OpenBracketCount = 0;
    OpenSBracketCount = 0;
    TPEventCount = 0;
}

//Deconstructor
TPConnection::~TPConnection()
{

}

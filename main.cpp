#include "main.h"

using namespace std;

wxIMPLEMENT_APP_CONSOLE(TP_SDK);

bool TP_SDK::OnInit()
{
    return true;
}

int TP_SDK::OnRun()
{
    //Create A Connection To Touch Portal With Your Plugin ID
    TPConnection* MyTPSocket = new TPConnection(wxT("tp_sdk"));

    //Display If Connection Paired To Touch Portal
    //cout << "Status - " << MyTPSocket->GetTPPluginStatus() << endl;
    
    // Main Loop As Long As Socket Is Connected To Touch Portal
    while( MyTPSocket->IsConnected() && !MyTPSocket->Error() )
    {
        //Check If We Receive Any Data From Touch Portal And Grab It
        if(MyTPSocket->IsData())
        {
            //Loop Through All Of The Actions/Events Received
            for(int i = 0; i < MyTPSocket->GetTPeCount(); i++)
            {
                //Check For Action
                if(MyTPSocket->GetTPeType(i).IsSameAs(wxT("action")))
                {
                    //Loop Through Action Data
                    for(int c = 0; c < MyTPSocket->GetTPeDataCount(i); c++)
                    {
                        //Display Data Received
                        //cout << "Data - " << MyTPSocket->GetTPeDataId(i,c) << " : " << MyTPSocket->GetTPeDataValue(i,c) << endl;
                    }
                }
                //Check For List Change
                if(MyTPSocket->GetTPeType(i).IsSameAs(wxT("listChange")))
                {
                    cout << "listChange" << endl;
                }
                //Check For Broadcast
                if(MyTPSocket->GetTPeType(i).IsSameAs(wxT("broadcast")))
                {
                    cout << "broadcast" << endl;
                }
            }
            //Uncomment For Debugging ( ReadData Command Only Used For Debugging )
            //cout << MyTPSocket->ReadData() << endl;
        }

        //Commands For Sending Data To Touch Portal

        //Update A State In Touch Portal
        MyTPSocket->UpdateState(wxT("stateId"),wxT("stateValue"));

        //Create List Items To Update
        wxArrayString aList;
        aList.Add(wxT("List Item 1"));
        aList.Add(wxT("List Item 2"));

        //Update A List In Touch Portal Without Instance Id
        MyTPSocket->UpdateList(wxT("id"),aList);

        //Update A List In Touch Portal With An Instance Id
        MyTPSocket->UpdateList(wxT("id"),wxT("instanceId"),aList);
    }
    //cout << "Exited" << endl;

    return 0;
}

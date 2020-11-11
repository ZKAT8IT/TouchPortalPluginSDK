#ifndef _MAIN_H_
#define _MAIN_H_

#include <iostream>
#include <wx/app.h>
#include <wx/wxprec.h>

#include "TPConnection\TPConnection.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class TP_SDK: public wxAppConsole
{
    public:
        virtual bool OnInit();
        virtual int OnRun();
};

#endif

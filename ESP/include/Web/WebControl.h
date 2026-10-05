#ifndef WEB_CONTROL_H
#define WEB_CONTROL_H

struct WebControl
{
    struct
    {
        bool manualOperation;
    } pump;

    struct
    {
        bool manualOperation;
    } watering;
};

extern WebControl webControl;

#endif
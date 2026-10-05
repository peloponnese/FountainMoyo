#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>

enum RequestCommand
{
    REQUEST_NONE,

    REQUEST_GET_STATE,
    REQUEST_GET_SETTINGS,

    REQUEST_SET_SETTINGS,
    REQUEST_SET_PUMP,
    REQUEST_SET_WATERING,
    REQUEST_SET_LEDS
};

using RequestPriority = uint8_t;

struct Request
{
    RequestCommand command;
    RequestPriority priority;
};

void commandHandlerBegin();
void commandHandlerUpdate();

bool request(RequestCommand command, RequestPriority priority);

void commandReply();

#endif
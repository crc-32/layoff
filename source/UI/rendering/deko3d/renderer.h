#pragma once
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <switch.h>

namespace renderer
{
bool init (NWindow *win);

void newFrame ();

void render ();

void exit ();

void SlowMode();
void FastMode();
}
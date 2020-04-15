#pragma once
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

namespace renderer
{
bool init ();

void newFrame ();

void render ();

void exit ();
}
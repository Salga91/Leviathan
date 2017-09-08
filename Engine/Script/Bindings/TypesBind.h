// Leviathan Game Engine
// Copyright (c) 2012-2017 Henri Hyyryläinen
#pragma once
// ------------------------------------ //
#include "BindHelpers.h"


namespace Leviathan{

bool BindTypes(asIScriptEngine* engine);
void RegisterTypes(asIScriptEngine* engine, std::map<int, std::string> &typeids);

}


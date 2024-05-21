#pragma once

enum class LogChannel
{
	General,
	Inputs,
	Graphics,
	Gameplay
};

const char* LogChannelToString(LogChannel channel);
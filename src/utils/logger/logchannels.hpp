#pragma once

enum class LogChannel
{
	General,
	Inputs,
	Graphics,
	Gameplay,
	Memory
};

const char* LogChannelToString(LogChannel channel);
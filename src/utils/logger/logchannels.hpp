#pragma once

enum class LogChannel
{
	General,
	Inputs,
	Graphics,
	Gameplay,
	Memory,
	Resource
};

const char* LogChannelToString(LogChannel channel);
#include <utils/logger/logchannels.hpp>

const char* LogChannelToString(LogChannel channel)
{
	switch (channel)
	{
	case LogChannel::General:
		return "General";
		break;
	case LogChannel::Inputs:
		return "Inputs";
		break;
	case LogChannel::Graphics:
		return "Graphics";
		break;
	case LogChannel::Gameplay:
		return "Gameplay";
		break;
	default:
		return "Unknown";
		break;
	}
}
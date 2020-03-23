#include "utils.h"

#include <string>
#include <sstream>


static std::string BadIndex_GenerateMsg(int index, int min, int max, const char* msg)
{
	std::stringstream ss;
	ss << "Require index between " << min << " and " << max << ". But found " << index << ". " << msg;
	return ss.str();
}

static std::string InvalidParameter_GenerateMsg(const char* parameter, const char* msg)
{
	std::stringstream ss;
	ss << "Invalid value for " << parameter << " parameter. " << msg;
	return ss.str();
}



BadIndex::BadIndex(int index, int min, int max, const char* msg) :
	exception{ BadIndex_GenerateMsg(index, min, max, msg).c_str() }
{}

InvalidParameter::InvalidParameter(const char* parameter, const char* msg) :
	exception{ InvalidParameter_GenerateMsg(parameter, msg).c_str() }
{}

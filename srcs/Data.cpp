#include "../includes/Data.hpp"

// Status Code

StringData::StringData(): std::string(), type(D_ERROR)
{}

StringData::StringData(std::string base): std::string(base), type(D_ERROR)
{}

StringData::StringData(std::string base, DataType type): std::string(base), type(type)
{}

StringData::StringData(const char *base): std::string(base), type(D_ERROR)
{}

StringData::StringData(const char *base, DataType type): std::string(base), type(type)
{}

StringData::~StringData()
{}

// Getters

DataType StringData::getType() const
{
	return this->type;
}
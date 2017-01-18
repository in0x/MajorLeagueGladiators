#include "Prototype.h"
#include "EventData.h"

FEventData::FEventData()
	: type(typeid(nullptr))
	, data(nullptr)
{}

FEventData::FEventData(const std::type_info& TypeId, const void* Data)
	: type(TypeId)
	, data(Data)
{}

bool FEventData::HasData() const
{
	return data != nullptr;
}

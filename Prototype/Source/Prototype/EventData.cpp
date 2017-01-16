#include "Prototype.h"

#include "EventData.h"

FEventData::FEventData()
	: type(typeid(void*))
	, data(nullptr)
{}

FEventData::FEventData(const std::type_info& TypeId, void* Data)
	: type(TypeId)
	, data(Data)
{}

bool FEventData::HasData()
{
	return data != nullptr;
}

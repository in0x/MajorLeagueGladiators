#pragma once

#include <typeinfo>
#include <typeindex>

#include "EventData.generated.h"

USTRUCT()
struct FEventData
{
	GENERATED_BODY()
private:
	FEventData(const std::type_info& TypeId, void* Data);

public:
	FEventData();

	template<class TYPE>
	static FEventData Make()
	{
		return EventData(typeid(TYPE), &data);
	}

	template<class TYPE> 
	const TYPE& GetData() const
	{
		checkf(std::type_index(typeid(TYPE)) == type, TEXT("Invalid Type used to access data."));
		checkf(data != nullptr, TEXT("data is nullptr"));
		return *static_cast<TYPE*>(data);
	}

	bool HasData();

private:
	std::type_index type;
	void* data;

};

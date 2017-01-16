#pragma once

#include <typeinfo>
#include <typeindex>
#include "EventData.generated.h"

USTRUCT()
struct FEventData
{
	GENERATED_BODY()
private:
	FEventData(const std::type_info& TypeId, const void* Data);

public:
	FEventData();

	template<class TYPE>
	static FEventData Make(const TYPE& Data)
	{
		return FEventData(typeid(TYPE), static_cast<const void*>(&Data));
	}

	template<class TYPE> 
	const TYPE& GetData() const
	{
		checkf(std::type_index(typeid(TYPE)) == type, TEXT("Invalid Type used to access data."));
		checkf(HasData(), TEXT("data is nullptr"));
		return *static_cast<const TYPE*>(data);
	}

	bool HasData() const;

private:
	std::type_index type;
	const void* data;
};

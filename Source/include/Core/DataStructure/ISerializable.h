#pragma once
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>


namespace Core
{
	class ISerializable
	{
		virtual void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) = 0;
	};

	enum
	{
		E_EMPTY,
		E_COMPANION_CUBE,
		E_BUTTON,
		E_DOOR,
		E_TURRET,
		E_PLAYER,
		E_PLATFORM,
		E_FLOOR,
		E_DIRECTIONAL_LIGHT,
		E_POINT_LIGHT,
		E_SPOT_LIGHT,
		E_RADIO
	};
}
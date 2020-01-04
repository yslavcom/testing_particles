#pragma once 

#include "ErrorCode.h"
#include "DebugLog.h"
#include "Screen.h"
#include "BasicShapes.h"
#include "BasicStructs.h"

namespace BASIC_SHAPES_2D
{
	class Axis
	{
	public:
		enum class Type
		{
			X,
			Y
		};

	private:
		Type m_type;

		float m_low_limit;
		float m_hgh_limit;
		float m_gap;
		rgb_color_normalized m_colour;

	public:
		Axis() = delete;
		Axis(Type type,
			rgb_color_normalized&& colour)
			: m_type(type)
			, m_low_limit(0.1)
			, m_hgh_limit(0.9)
			, m_gap(0.1)
			, m_colour(colour)
		{}

		ErrorCode draw(screen_ptr screen,
			pixel_vec_2d& pixel2d_buf)
		{
			switch (m_type)
			{
			case Type::X:
				BasicShapes::draw_line(
					screen, pixel2d_buf,
					{ m_low_limit, 1-m_gap },
					{ m_hgh_limit, 1 - m_gap },
					std::forward<decltype(m_colour)>(m_colour)
				);
				break;

			case Type::Y:
				BasicShapes::draw_line(
					screen, pixel2d_buf,
					{  m_gap, m_low_limit },
					{  m_gap, m_hgh_limit },
					std::forward<decltype(m_colour)>(m_colour)
				);
				break;
			}

			return ErrorCode::OK;
		}
	};
}
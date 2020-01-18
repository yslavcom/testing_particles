#pragma once 

#include "ErrorCode.h"
#include "DebugLog.h"
#include "Screen.h"
#include "BasicShapesDraw.h"
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
		Axis() {};
		Axis(Type type,
			rgb_color_normalized&& colour)
			: m_type(type)
			, m_low_limit(0.1f)
			, m_hgh_limit(0.9f)
			, m_gap(0.1f)
			, m_colour(colour)
		{}

		Axis(Axis&& other)
		{
			m_type = std::move(other.m_type);
			m_low_limit = std::move(other.m_low_limit);
			m_hgh_limit = std::move(other.m_hgh_limit);
			m_gap = std::move(other.m_gap);
			m_colour = std::move(other.m_colour);
		}

		Axis(const Axis& other)
			: m_type(other.m_type)
			, m_low_limit(other.m_low_limit)
			, m_hgh_limit(other.m_hgh_limit)
			, m_gap(other.m_gap)
			, m_colour(other.m_colour)
		{}

		Axis& operator=(const Axis& other)
		{
			m_type = other.m_type;
			m_low_limit = other.m_low_limit;
			m_hgh_limit = other.m_hgh_limit;
			m_gap = other.m_gap;
			m_colour = other.m_colour;
			return *this;
		}

		Axis& operator=(Axis&& other)
		{
			m_type = std::move(other.m_type);
			m_low_limit = std::move(other.m_low_limit);
			m_hgh_limit = std::move(other.m_hgh_limit);
			m_gap = std::move(other.m_gap);
			m_colour = std::move(other.m_colour);
			return *this;
		}

		template <typename W>
		ErrorCode draw(screen_ptr screen,
			W&& window,
			pixel_vec_2d& pixel2d_buf)
		{
			switch (m_type)
			{
			case Type::X:
				BasicShapesDraw::draw_line(
					screen, 
					window,
					pixel2d_buf,
					pixel_2d_coord_normal{ m_low_limit, m_gap },
					pixel_2d_coord_normal{ m_hgh_limit, m_gap },
					m_colour
				);
				break;

			case Type::Y:
				BasicShapesDraw::draw_line(
					screen, 
					window,
					pixel2d_buf,
					pixel_2d_coord_normal{  m_gap, m_low_limit },
					pixel_2d_coord_normal{  m_gap, m_hgh_limit },
					m_colour
				);
				break;
			}

			return ErrorCode::OK;
		}
	};
}
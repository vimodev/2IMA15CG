#pragma once
#include "geom.hpp"
#include "bentley_ottmann_any_intersection.hpp"
#include <optional>
#include <cstddef>
#include <sstream>
#include <unordered_map>


namespace boi {
	/**
	 * Structure that represents an error in a coloring;
	 * either an error in the encoding (is_intersection is false),
	 * or a pair of intersecting segments (is_intersection is true),
	 * in which case the information about the segment indices,
	 * the color class and the approximate location of the intersection
	 * are valid. The message field always describes the error.
	 */
	struct ColoringError {
		ColoringError(std::size_t s1, std::size_t s2, std::size_t cc, double x, double y, std::string m, bool ii) noexcept : 
			segment_index1(s1), segment_index2(s2), color_class(cc),
			approximate_x(x), approximate_y(y), message(m), is_intersection(ii)
		{}

		std::size_t segment_index1, segment_index2;
		std::size_t color_class;
		double approximate_x, approximate_y;
		std::string message;
		bool is_intersection;
	};

	/**
	 * Verifier for colorings of line segments.
	 */
	class ColoringVerifier {
	public:
		/**
		 * Construct a verifier for the given segments and the given coloring,
		 * where coloring[i] represents the color of segment segs[i].
		 */
		ColoringVerifier(const std::vector<Segment>& segs, const std::vector<std::size_t>& coloring) noexcept :
			m_segments(&segs), m_coloring(&coloring), num_colors(0)
		{}

		/**
		 * Verify the given coloring, returning an error if one is found.
		 * After verification, the number of colors in the coloring is 
		 * available in the field num_colors.
		 */
		std::optional<ColoringError> verify() {
			if(!sanity_check()) {
				return m_intersection;
			}
			std::unordered_map<std::size_t, std::vector<std::size_t> > color_classes;
			for(std::size_t i = 0, n = m_coloring->size(); i < n; ++i) {
				std::size_t c = (*m_coloring)[i];
				auto iter = color_classes.find(c);
				if(iter == color_classes.end()) {
					iter = color_classes.emplace(c, std::vector<std::size_t>{}).first;
				}
				iter->second.push_back(i);
			}
			num_colors = color_classes.size();
			for(std::size_t c = 0, nc = color_classes.size(); c < nc; ++c) {
				verify_color_class(c, color_classes[c]);
				if(m_intersection) {
					return m_intersection;
				}
			}
			return {};
		}

	private:
		/**
		 * Sanity check for the coloring such as checking that
		 * the number of segments corresponds to the number of colored vertices.
		 */
		bool sanity_check() {
			if(m_segments->size() != m_coloring->size()) {
				const std::size_t npos = std::numeric_limits<std::size_t>::max();
				const double qnan = std::numeric_limits<double>::quiet_NaN();
				std::stringstream msg;
				msg << "Number of colored segments (" << m_coloring->size() 
					<< ") does not match number of segments in instance ("
					<< m_segments->size() << ")";
				m_intersection = ColoringError{npos, npos, npos, qnan, qnan, msg.str(), false};
				return false;
			}
			return true;
		}

		/**
		 * Verify the segments of a color class. Sets m_intersection if
		 * an error is found.
		 */
		void verify_color_class(std::size_t color, const std::vector<std::size_t>& color_class) {
			std::vector<std::size_t> subindex_to_index;
			std::vector<Segment> subsegments;
			for(std::size_t segment_index : color_class) {
				subindex_to_index.push_back(segment_index);
				subsegments.push_back((*m_segments)[segment_index]);
			}
			prepare_segments(subsegments);
			BentleyOttmannAnyIntersection algorithm(subsegments);
			auto result = algorithm.find_intersection();
			if(result) {
				std::size_t i1 = result->segment_index[0] = subindex_to_index[result->segment_index[0]];
				std::size_t i2 = result->segment_index[1] = subindex_to_index[result->segment_index[1]];
				Segment s1 = (*m_segments)[i1], s2 = (*m_segments)[i2];
				std::stringstream msg;
				msg << "Intersection between segments " << i1
					<< " (" << s1 << ") and " << i2 << " ("
					<< s2 << ") near (" << result->approximate_location[0] << ", "
					<< result->approximate_location[1] << ")";
				m_intersection = ColoringError{
					i1, i2, color,
					result->approximate_location[0],result->approximate_location[1],
					msg.str(), true
				};
			}
		}

		const std::vector<Segment>* m_segments;
		const std::vector<std::size_t>* m_coloring;
		std::optional<ColoringError> m_intersection;

	public:
		std::size_t num_colors;
	};
}


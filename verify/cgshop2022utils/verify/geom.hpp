#pragma once
#include <cstdint>
#include <utility>
#include <optional>
#include <array>
#include <iostream>

namespace boi {
	/**
	 * Represents a point with integer coordinates.
	 */
	struct Point {
		Point() noexcept = default;

		Point(std::int64_t x, std::int64_t y) noexcept :
			x(x), y(y)
		{}

		bool operator<(const Point& other) const noexcept {
			return x < other.x || (x == other.x && y < other.y);
		}

		bool operator==(const Point& other) const noexcept {
			return x == other.x && y == other.y;
		}

		std::int64_t x, y;
	};

	/**
	 * Output a point.
	 */
	inline std::ostream &operator<<(std::ostream& o, const Point& p) {
		return o << '(' << p.x << ", " << p.y << ')';
	}

	/**
	 * Represents a line segment defined by two points with integer coordinates.
	 */
	struct Segment {
		Segment(Point s, Point t) noexcept :
			s(s), t(t)
		{}

		std::pair<Point,Point> left_and_right() const noexcept {
			if(s < t) {
				return {s,t};
			} else {
				return {t,s};
			}
		}

		Segment ordered() const noexcept {
			if(s < t) {
				return *this;
			} else {
				return Segment{t,s};
			}
		}

		Point s, t;
	};

	/**
	 * Output a line segment for a human reader.
	 */
	inline std::ostream& operator<<(std::ostream& o, const Segment& s) {
		return o << s.s << "--" << s.t;
	}

	/**
	 * Check the orientation of three points
	 * (0: collinear, 1: counterclockwise turn, -1: clockwise turn).
	 */
	inline int orientation(Point p1, Point p2, Point p3) {
		std::int64_t v1 = (p2.y - p1.y) * (p3.x - p2.x);
		std::int64_t v2 = (p2.x - p1.x) * (p3.y - p2.y);
		if(v1 == v2) {
			return 0; // collinear
		}
		return (v1 > v2) ? -1 : 1; // clockwise (-1) or counterclockwise (1)
	}

	namespace impl {
		static inline std::int64_t cross(Point p1, Point p2) noexcept {
			return p1.x * p2.y - p2.x * p1.y;
		}

		static inline bool collinear_order_correct(Point p1, Point p2, Point p3) noexcept {
			if(p1.x == p2.x) {
				return p1.y > p2.y ? p3.y <= p2.y : p2.y <= p3.y;
			}
			if(p1.x < p2.x) {
				return p2.x <= p3.x;
			} else {
				return p3.x <= p2.x;
			}
		}

		static inline bool ranges_overlap(std::int64_t min1, std::int64_t max1, std::int64_t min2, std::int64_t max2) {
			if(min1 < min2) {
				return max1 >= min2;
			} else {
				return max2 >= min1;
			}
		}
	}

	/**
	 * Compute an approximate intersection point for the two given segments.
	 */
	inline std::array<double, 2> approximate_intersection_point(Segment s1, Segment s2) {
		Point p = s1.s;
		Point r{s1.t.x - s1.s.x, s1.t.y - s1.s.y};
		Point q = s2.s;
		Point s{s2.t.x - s2.s.x, s2.t.y - s2.s.y};
		Point qmp{q.x - p.x, q.y - p.y};
		std::int64_t qpr = impl::cross(qmp, r);
		std::int64_t rs = impl::cross(r, s);
		if(!qpr && !rs) { // collinear
			if(impl::collinear_order_correct(s1.s, s2.s, s1.t)) {
				return {(double)s2.s.x, (double)s2.s.y};
			} else if(impl::collinear_order_correct(s1.s, s2.t, s1.t)) {
				return {(double)s2.t.x, (double)s2.t.y};
			} else if(impl::collinear_order_correct(s2.s, s1.s, s2.t)) {
				return {(double)s1.s.x, (double)s1.s.y};
			} else if(impl::collinear_order_correct(s1.s, s1.t, s2.t)) {
                return {(double)s1.t.x, (double)s1.t.y};
			} else {
				throw std::runtime_error("Overlap check positive, but no overlap!");
			}
		} else if(!rs) {
			// no intersection?!
			throw std::runtime_error("Intersection check positive, but no intersection!");
		} else {
			double u = (double)qpr / rs;
			double x = q.x + u * s.x;
			double y = q.y + u * s.y;
			return {x,y};
		}
	}

	/**
	 * Do the given segments intersect? If yes, an approximate intersection point is returned.
	 * Both s1 and s2 must have si.s <= si.t.
	 */
	inline std::optional<std::array<double, 2>> do_intersect(Segment s1, Segment s2) {
		std::array<double, 2> result;
		if(s2.s < s1.s) {
			std::swap(s1, s2);
		}
		if(s1.s == s2.s) {
			if(!orientation(s1.s, s1.t, s2.t)) {
				if(impl::collinear_order_correct(s1.s, s1.t, s2.t)) {
					result[0] = s1.t.x;
					result[1] = s1.t.y;
				} else {
					result[0] = s2.t.x;
					result[1] = s2.t.y;
				}
				return result;
			}
			return {};
		}
		if(s1.t == s2.t) {
			if(!orientation(s1.s, s2.s, s1.t)) {
				result[0] = s2.s.x;
				result[1] = s2.s.y;
				return result;
			}
			return {};
		}
		if(s1.t == s2.s) {
			return {};
		}
		int o1 = orientation(s1.s, s1.t, s2.s),
			o2 = orientation(s1.s, s1.t, s2.t),
			o3 = orientation(s2.s, s2.t, s1.s),
			o4 = orientation(s2.s, s2.t, s1.t);
		if(o1 != o2 && o3 != o4) { // general case
			return approximate_intersection_point(s1, s2);
		}
		if(!o1 && !o2) { // all collinear
			std::int64_t mnxr1 = (std::min)(s1.s.x, s1.t.x), mxxr1 = (std::max)(s1.s.x, s1.t.x);
			std::int64_t mnyr1 = (std::min)(s1.s.y, s1.t.y), mxyr1 = (std::max)(s1.s.y, s1.t.y);
			std::int64_t mnxr2 = (std::min)(s2.s.x, s2.t.x), mxxr2 = (std::max)(s2.s.x, s2.t.x);
			std::int64_t mnyr2 = (std::min)(s2.s.y, s2.t.y), mxyr2 = (std::max)(s2.s.y, s2.t.y);
			if(impl::ranges_overlap(mnxr1, mxxr1, mnxr2, mxxr2) &&
			   impl::ranges_overlap(mnyr1, mxyr1, mnyr2, mxyr2)) 
			{
				return approximate_intersection_point(s1, s2);
			}
		}
		return {};
	}
}


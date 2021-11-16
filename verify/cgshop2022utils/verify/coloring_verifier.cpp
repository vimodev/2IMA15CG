/**
 * C++ code for the python binding.
 * When imported as python module, it exposes
 * the classes Point, Segment and ColoringError as well
 * as the methods verify_coloring and do_intersect.
 *
 * From python, verify_coloring can be called with a list
 * of segments, a list of 4-tuples of point coordinates
 * (s.x, s.y, t.x, t.y), or a list of 2-tuples
 * ((s.x,s.y),(t.x,t.y)). The second argument always is a
 * coloring as a list of non-negative integers, where
 * entry coloring[i] is the color of segment[i].
 */
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "verify_coloring.hpp"
#include <tuple>

namespace py = pybind11;
using namespace boi;

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#ifndef VERSION_INFO
#define VERSION_INFO unknown
#endif

std::string module_version() {
	return MACRO_STRINGIFY(VERSION_INFO);
}

std::tuple<std::optional<ColoringError>, std::size_t>
	verify_coloring(std::vector<Segment> segments, const std::vector<std::size_t>& coloring)
{
	prepare_segments(segments);
	ColoringVerifier verifier(segments, coloring);
	auto error_result = verifier.verify();
	return std::tuple<std::optional<ColoringError>, std::size_t>(error_result, verifier.num_colors);
}
typedef std::tuple<std::optional<ColoringError>, std::size_t> (*vc_segments)(std::vector<Segment>, const std::vector<std::size_t>&);

std::tuple<std::optional<ColoringError>, std::size_t>
	verify_coloring(const std::vector<std::tuple<std::int64_t, std::int64_t, std::int64_t, std::int64_t> >& segs, const std::vector<std::size_t>& coloring)
{
	std::vector<Segment> segments;
	segments.reserve(segs.size());
	for(const auto& s : segs) {
		std::int64_t sx = std::get<0>(s), sy = std::get<1>(s);
		std::int64_t tx = std::get<2>(s), ty = std::get<3>(s);
		segments.push_back(Segment{{sx, sy}, {tx, ty}});
	}
	return verify_coloring(std::move(segments), coloring);
}
typedef std::tuple<std::optional<ColoringError>, std::size_t> (*vc_tup4)(const std::vector<std::tuple<std::int64_t, std::int64_t, std::int64_t, std::int64_t> >&, const std::vector<std::size_t>&);


std::tuple<std::optional<ColoringError>, std::size_t>
	verify_coloring(const std::vector<std::pair<std::pair<std::int64_t, std::int64_t>, std::pair<std::int64_t, std::int64_t> > >& segs, const std::vector<std::size_t>& coloring)
{
	std::vector<Segment> segments;
	segments.reserve(segs.size());
	for(const auto& s : segs) {
		std::int64_t sx = s.first.first, sy = s.first.second;
		std::int64_t tx = s.second.first, ty = s.second.second;
		segments.push_back(Segment{{sx, sy}, {tx, ty}});
	}
	return verify_coloring(std::move(segments), coloring);
}
typedef std::tuple<std::optional<ColoringError>, std::size_t> (*vc_2tup2)(const std::vector<std::pair<std::pair<std::int64_t, std::int64_t>, std::pair<std::int64_t, std::int64_t> > >&, const std::vector<std::size_t>&);


PYBIND11_MODULE(coloring_verifier, m) {
    m.doc() = "Python binding for C++ coloring verifier";
	
	py::class_<ColoringError>(m, "ColoringError")
		.def(py::init<std::size_t, std::size_t, std::size_t, double, double, std::string, bool>())
		.def_readonly("segment_index1", &ColoringError::segment_index1)
		.def_readonly("segment_index2", &ColoringError::segment_index2)
		.def_readonly("color_class", &ColoringError::color_class)
		.def_readonly("approximate_x", &ColoringError::approximate_x)
		.def_readonly("approximate_y", &ColoringError::approximate_y)
		.def_readonly("message", &ColoringError::message)
		.def_readonly("is_intersection", &ColoringError::is_intersection)
		.def("__str__", [] (const ColoringError& err) { return err.message; })
		.def("__repr__", [] (const ColoringError& err) -> std::string {
				std::stringstream repr;
				repr << "ColoringError(" << err.segment_index1 << ", " << err.segment_index2 << ", "
				     << err.color_class << ", " << err.approximate_x << ", " << err.approximate_y << ", \""
					 << err.message << "\", " << std::boolalpha << err.is_intersection << ")";
				return repr.str();
			});

	py::class_<Point>(m, "Point")
		.def(py::init<std::int64_t, std::int64_t>())
		.def_readwrite("x", &Point::x)
		.def_readwrite("y", &Point::y)
		.def("__str__", [] (const Point& p) -> std::string { std::stringstream s; s << p; return s.str(); })
		.def("__repr__", [] (const Point& p) -> std::string { std::stringstream s; s << "Point(" << p.x << ", " << p.y << ")"; return s.str(); });

	py::class_<Segment>(m, "Segment")
		.def(py::init<Point, Point>())
		.def_readwrite("s", &Segment::s)
		.def_readwrite("t", &Segment::t)
		.def("ordered", &Segment::ordered)
		.def("__str__", [] (const Segment& s) -> std::string { std::stringstream ss; ss << s; return ss.str(); })
		.def("__repr__", [] (const Segment& s) -> std::string { std::stringstream ss; ss << "Segment((" << s.s.x << ", " << s.s.y << "), (" << s.t.x << ", " << s.t.y << "))"; return ss.str(); });

	m.def("verify_coloring", static_cast<vc_segments>(&verify_coloring));
	m.def("verify_coloring", static_cast<vc_tup4>(&verify_coloring));
	m.def("verify_coloring", static_cast<vc_2tup2>(&verify_coloring));
	m.def("do_intersect", &do_intersect);
	m.def("module_version", &module_version);
}


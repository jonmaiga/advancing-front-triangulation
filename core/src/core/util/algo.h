#pragma once

#include <algorithm>
#include <set>

namespace playchilla {
template <typename Container, typename ContainerRhs>
void append(Container& destination, const ContainerRhs& source) {
	destination.insert(destination.end(), source.begin(), source.end());
}

template <typename Container, typename Predicate>
bool any_of(const Container& c, Predicate p) {
	return std::any_of(c.begin(), c.end(), p);
}

template <typename Container, typename ElementType>
bool contains(const Container& c, const ElementType& value) {
	return std::any_of(c.begin(), c.end(), [&value](const auto& e) { return value == e; });
}

template <typename Container, typename Predicate>
bool all_of(const Container& c, Predicate p) {
	return std::all_of(c.begin(), c.end(), p);
}

template <typename ContainerA, typename ContainerB>
bool contains_all_of_rhs(const ContainerA& lhs, const ContainerB& rhs) {
	return all_of(rhs, [&lhs](const auto& e) { return contains(lhs, e); });
}

template <typename ContainerA, typename ContainerB>
bool contains_any_of_rhs(const ContainerA& lhs, const ContainerB& rhs) {
	return any_of(rhs, [&lhs](const auto& e) { return contains(lhs, e); });
}


template <typename SortedContainer>
bool is_subset(const SortedContainer& a, const SortedContainer& potentialSubset) {
	return std::includes(a.begin(), a.end(), potentialSubset.begin(), potentialSubset.end());
}

template <typename Container>
void sort(Container& a) { std::sort(a.begin(), a.end()); }

template <typename Container, typename Predicate>
void sort(Container& a, Predicate p) { std::sort(a.begin(), a.end(), p); }

template <class T, class Comp, class Alloc, class Predicate>
void discard_if(std::set<T, Comp, Alloc>& c, const Predicate& predicate) {
	for (auto it{c.begin()}, end{c.end()}; it != end;) {
		if (predicate(*it)) {
			it = c.erase(it);
		}
		else {
			++it;
		}
	}
}

template <typename Container, typename Predicate>
int count_if(Container& a, Predicate p) { return std::count_if(a.begin(), a.end(), p); }
}

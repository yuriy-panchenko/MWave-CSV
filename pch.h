// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <vector>
#include <thread>
#include <cassert>
#include "..\lib\YxShare\YxShare.h"
#include "defins.h"

template<typename T>
CArchive& operator<<(CArchive& ar, const std::vector<T>& v)
{
	const uint64_t size{ v.size() };
	ar.Write(&size, sizeof size);
	ar.Write(v.data(), UINT(size * sizeof(T)));
	return ar;
}

template<typename T>
CArchive& operator>>(CArchive& ar, std::vector<T>& v)
{
	uint64_t size;
	ar.Read(&size, sizeof size);
	v.resize(size);
	ar.Read(v.data(), UINT(size * sizeof(T)));
	return ar;
}

#endif //PCH_H

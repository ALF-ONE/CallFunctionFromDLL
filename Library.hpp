/*
	MIT License
	
	Copyright (c) 2019 ALF-ONE
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once

#ifndef LIBRARY_HPP_
#define LIBRARY_HPP_

#include <Windows.h>

class CLibrary
{
private:
	HMODULE m_hLibrary = nullptr;
	bool m_bLoad = false;

	template<typename Ret>
	struct _call
	{
		_call(HMODULE lib, const char *func) : m_lib(lib), m_func(func) {}

		template<typename... Args>
		auto arguments(Args... args) -> Ret
		{
			if (!m_lib) return Ret(NULL);

			FARPROC address = GetProcAddress(m_lib, m_func);
			if (!address) return Ret(NULL);

			typedef Ret(__stdcall *_fn)(Args...);
			_fn __ret = reinterpret_cast<_fn>(address);

			return (*__ret)(args...);
		}

	private:
		HMODULE m_lib;
		const char *m_func;
	};

public:
	CLibrary(const char *lib)
	{
		m_hLibrary = GetModuleHandle(lib);
		if (!m_hLibrary)
		{
			m_hLibrary = LoadLibrary(lib);
			m_bLoad = true;
		}
	}
	~CLibrary() {
		if (m_hLibrary && m_bLoad) FreeLibrary(m_hLibrary);
		m_hLibrary = nullptr;
	}

	template<typename Ret>
	_call<Ret> call(const char *func) {
		return _call<Ret>(m_hLibrary, func);
	}

	template<typename Ret, typename... Args>
	static auto call(const char *lib, const char *func, Args... args) -> Ret
	{
		bool bLoad = false;
		HMODULE hLib = GetModuleHandle(lib);

		if (!hLib)
		{
			hLib = LoadLibrary(lib);
			if (!hLib) return Ret(NULL);

			bLoad = true;
		}

		FARPROC address = GetProcAddress(hLib, func);
		if (!address) {
			FreeLibrary(hLib);
			return Ret(NULL);
		}

		typedef Ret(__stdcall *_fn)(Args...);
		_fn __ret = reinterpret_cast<_fn>(address);

		if (bLoad) FreeLibrary(hLib);

		return (*__ret)(args...);
	}
};

#endif // LIBRARY_HPP_
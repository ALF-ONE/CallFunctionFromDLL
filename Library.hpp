/*
	MIT License
	
	Copyright (c) 2019-2020 ALF-ONE
	
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
	HMODULE m_hLibrary;
	bool m_bLoad;

	template<typename Ret>
	struct _call
	{
		_call(HMODULE hLib, const char* szFunc) : m_hLib(hLib), m_szFunc(szFunc) {}

		template<typename... Args>
		auto arguments(Args... args) -> Ret
		{
			if (!m_hLib || !m_szFunc) return Ret(NULL);

			FARPROC address = GetProcAddress(m_hLib, m_szFunc);
			if (!address) return Ret(NULL);

			return (*reinterpret_cast<Ret(__stdcall*)(Args...)>(address))(args...);
		}

	private:
		HMODULE m_hLib;
		const char* m_szFunc;
	};

public:
	explicit CLibrary(const char* szLib) : m_hLibrary(nullptr), m_bLoad(false)
	{
		m_hLibrary = GetModuleHandle(szLib);
		if (!m_hLibrary)
		{
			m_hLibrary = LoadLibrary(szLib);
			m_bLoad = true;
		}
	}
	~CLibrary() {
		if (m_hLibrary && m_bLoad) FreeLibrary(m_hLibrary);
	}

	template<typename Ret>
	inline _call<Ret> call(const char* szFunc) {
		return _call<Ret>(m_hLibrary, szFunc);
	}

	template<typename Ret, typename... Args>
	static auto call(const char* szLib, const char* szFunc, Args... args) -> Ret
	{
		if (!szLib || !szFunc) return Ret(NULL);

		bool bLoad = false;
		HMODULE hLib = GetModuleHandle(szLib);

		if (!hLib)
		{
			hLib = LoadLibrary(szLib);
			if (!hLib) return Ret(NULL);

			bLoad = true;
		}

		FARPROC address = GetProcAddress(hLib, szFunc);
		if (!address) {
			FreeLibrary(hLib);
			return Ret(NULL);
		}

		Ret __ret = (*reinterpret_cast<Ret(__stdcall*)(Args...)>(address))(args...);
		if (bLoad) FreeLibrary(hLib);

		return __ret;
	}
};

#endif // LIBRARY_HPP_
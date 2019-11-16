# CallFunctionFromDLL
dynamic function loading from dll

#### example #1
```c++
/* The first parameter is the return parameter. */
auto ret_1 = CLibrary::call<int, HWND, LPCTSTR, LPCTSTR, UINT>("User32.dll", "MessageBoxA", NULL, "Hello, World!", "Example 1", MB_OK);
```

#### example #2
```c++
CLibrary Lib("User32.dll");
auto ret_2 = Lib.call<int>("MessageBoxA").arguments<HWND, LPCTSTR, LPCTSTR, UINT>(NULL, "Hello, World!", "Example 2", MB_OK);
```

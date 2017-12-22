# C++ 对象声明周期管理

> 2017/12/21
>
> 

[heading-numbering]

## [no-toc] [no-number] TOC

[TOC]

## Code

``` cpp
class BaseCtor {
public:
    BaseCtor () { setup (); }
    void setup () { virtual_setup (); }
    virtual void virtual_setup () = 0;
};

class DerivedCtor : BaseCtor {
    void virtual_setup () override {}
};

class BaseDtor {
public:
    ~BaseDtor () { clear (); }
    void clear () { virtual_clear (); }
    virtual void virtual_clear () = 0;
};

class DerivedDtor : BaseDtor {
    void virtual_clear () override {}
};

{
    // - R6025: pure virtual function call
    // - pure virtual method called (core dumped)
    DerivedCtor objCtor;
    DerivedDtor objDtor;
}
```

``` cpp
enum {
    WM_SEND_BEFORE_DESTROY = WM_USER,
    WM_POST_BEFORE_DESTROY
};

LRESULT CALLBACK WndProc (
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SEND_BEFORE_DESTROY:
        break;                                              // 3
    case WM_POST_BEFORE_DESTROY:
        break;                                              // -1!!!
    case WM_CLOSE:
        PostMessageW (hwnd, WM_POST_BEFORE_DESTROY, 0, 0);  // 1
        SendMessageW (hwnd, WM_SEND_BEFORE_DESTROY, 0, 0);  // 2
        DestroyWindow (hwnd);                               // 4
        break;                                              // 7!!!
    case WM_DESTROY:
        PostQuitMessage (0);                                // 5
        break;                                              // 6
    default:
        return DefWindowProc (hwnd, msg, wParam, lParam);
    }
    return 0;
}
```

- https://msdn.microsoft.com/en-us/library/windows/desktop/ff381396(v=vs.85).aspx
- https://msdn.microsoft.com/en-us/library/windows/desktop/ms632598(v=vs.85).aspx#destroying_win
- https://msdn.microsoft.com/en-us/library/windows/desktop/ms644927(v=vs.85).aspx#quequed_messages

``` cpp
class Menu : public CWindowImpl<Menu> {
  BEGIN_MSG_MAP_EX(Menu)
    MSG_WM_ACTIVATE(OnActivate)
  END_MSG_MAP()

public:
  virtual ~Menu() {
    if (IsWindow())
      DestroyWindow();  // implicit call OnActivate!
  }

  void OnActivate(UINT nState, BOOL, CWindow) {
    if (nState == WA_INACTIVE)
      HideMenu();  // leading to _purecall in CRT
  }

  virtual void HideMenu() = 0;
};

class MyMenu : public Menu {
public:
  void HideMenu() override {}
};

// Client Code
{
  MyMenu menu;
  menu.Create(nullptr, CRect{}, nullptr, WS_POPUP);
  menu.ShowWindow(SW_SHOW);
}  // Crash here!
```

![Call Stack Top](Cpp-Lifetime-Management/call-stack-top.png)
![Call Stack Recursion](Cpp-Lifetime-Management/call-stack-recursion.png)
![Call Stack Root](Cpp-Lifetime-Management/call-stack-root.png)

## 写在最后 [no-number]

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2017, BOT Man

# C++ 编译时检查

> 2018/11/10
>
> 让编译器帮你检查业务逻辑限制

[heading-numbering]

## [no-toc] [no-number] TOC

[TOC]

- const 不允许修改对象，并往下传递
- constexpr 强制要求编译时算出结果
- private/protected 缩小访问作用域
- explicit 避免隐式构造
- override 避免错误重载
- DISALLOW_COPY_AND_ASSIGN 避免拷贝
  - T(const T&) = delete;
  - T& operator=(const T&) = delete;

## 限制生命周期管理

## 禁止未定义操作

## 检查业务实体类型

- [Avoiding Disasters with Strongly Typed C++](https://github.com/CppCon/CppCon2018/blob/master/Presentations/avoiding_disasters_with_strongly_typed_cpp/avoiding_disasters_with_strongly_typed_cpp__arno_lepisk__cppcon_2018.pdf) by Arno Lepisk
- [NamedType](https://github.com/joboccara/NamedType) by joboccara

``` cpp
bool SendWebsiteToEmail(const std::wstring& email,
                        const std::wstring& url,
                        const std::wstring& title);

SendWebsiteToEmail(email, url, title);  // good :-)
SendWebsiteToEmail(email, title, url);  // bad  :-(
SendWebsiteToEmail(url, title, email);  // bad  :-(
```

## 写在最后 [no-number]

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man

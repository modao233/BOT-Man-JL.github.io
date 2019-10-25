# CppCon 2019 导读

> 2019/10/15
> 
> C makes it easy to shoot yourself in the foot; C++ makes it harder, but when you do it blows your whole leg off. —— Bjarne Stroustrup

这段时间抽空看了一些今年的 [CppCon 演讲](https://github.com/CppCon/CppCon2019)，选一些 **我觉得有意思的** 推荐给大家~

## 使用指导

- [C++ at 40](https://github.com/CppCon/CppCon2019/blob/master/Presentations/cpp_at_40/cpp_at_40__bjarne_stroustrup__cppcon_2019.pdf) by Bjarne Stroustrup —— 四十岁的 C++ 发展的现状和趋势
- [Avoid Misuse of Contracts](https://github.com/CppCon/CppCon2019/blob/master/Presentations/avoid_misuse_of_contracts/avoid_misuse_of_contracts__rostislav_khlebnikov__cppcon_2019.pdf) by Rostislav Khlebnikov —— 如何合理使用契约（虽然契约从 C++ 20 中移除了，但内容仍有指导意义）
  - 仅检查不能违反的限制（违反则出现 未定义行为 _(undefined behavior)_，而不是抛出/返回异常）
  - 检查不能影响核心逻辑，不能产生 副作用 _(side effect)_
  - 检查不能完全替代 测试/文档/输入合法性校验，需要适当权衡
- [Rethinking the Way We Do Templates in C++](https://github.com/CppCon/CppCon2019/blob/master/Presentations/rethinking_the_way_we_do_templates_in_cpp/rethinking_the_way_we_do_templates_in_cpp__mateusz_pusz__cppcon_2019.pdf) by Mateusz Pusz —— 从使用体验、语言新特性、性能的角度，讨论 C++ 模板（关于 concept 和模板性能 的讨论非常精彩）
- [How to Choose the Right Standard Library Container](https://github.com/CppCon/CppCon2019/blob/master/Presentations/how_to_choose_the_right_standard_library_container/how_to_choose_the_right_standard_library_container__alan_talbot__cppcon_2019.pdf) by Alan Talbot —— 如何选择 STL 容器（关于 vector/list 的一些不易察觉的细节）
- [C++ Code Smells](https://github.com/CppCon/CppCon2019/blob/master/Presentations/cpp_code_smells/cpp_code_smells__jason_turner__cppcon_2019.pdf) by Jason Turner —— 细数 C++ 代码中的“坏味道”
- [Are We Macro-free Yet](https://github.com/CppCon/CppCon2019/blob/master/Presentations/are_we_macrofree_yet/are_we_macrofree_yet__zhihao_yuan__cppcon_2019.pdf) by Zhihao Yuan —— 如何用现代 C++ 新特性，摆脱对宏的依赖
- [Naming is Hard: Let's Do Better](https://github.com/CppCon/CppCon2019/blob/master/Presentations/naming_is_hard_lets_do_better/naming_is_hard_lets_do_better__kate_gregory__cppcon_2019.pdf) by Kate Gregory —— ~~C++ 程序~~ 通用命名原则

## 标准前瞻

- [An Overview of Standard Ranges](https://github.com/CppCon/CppCon2019/blob/master/Presentations/an_overview_of_standard_ranges/an_overview_of_standard_ranges__tristan_brindle__cppcon_2019.pdf) by Tristan Brindle —— [NanoRange](https://github.com/tcbrindle/NanoRange) 作者介绍 C++ 20 的最新 Range 标准
- [Generators, Coroutines and Other Brain Unrolling Sweetness](https://github.com/CppCon/CppCon2019/blob/master/Presentations/generators_coroutines_and_other_brain_unrolling_sweetness/generators_coroutines_and_other_brain_unrolling_sweetness__adi_shavit__cppcon_2019.pdf) by Adi Shavit —— 关于 C++ 20 的最新 Coroutine 介绍（里边提到了 Arthur O’Dwyer 写的 [C++2a Coroutines and dangling references](https://quuxplusone.github.io/blog/2019/07/10/ways-to-get-dangling-references-with-coroutines/)，介绍了 Coroutine 参数的生命周期问题）
- [Practical C++ Modules](https://github.com/CppCon/CppCon2019/blob/master/Presentations/practical_cpp_modules/practical_cpp_modules__boris_kolpackov__cppcon_2019.pdf) by Boris Kolpackov —— 关于 C++ 20 的 Modules 的最佳实践（如何在老项目中无缝接入）
- [Using C++20’s Three-way Comparison](https://github.com/CppCon/CppCon2019/blob/master/Presentations/using_cpp20s_threeway_comparison_/using_cpp20s_threeway_comparison___jonathan_m%C3%BCller__cppcon_2019.pdf) by Jonathan Müller —— 关于 C++ 20 的 Three-way Comparison 特性
- [C++ Standard Library "Little Things"](https://github.com/CppCon/CppCon2019/blob/master/Presentations/cpp_standard_library_little_things/cpp_standard_library_little_things__billy_oneal__cppcon_2019.pdf) by Billy O'Neal —— 微软最近开源的 STL（及对 C++ 17/20 的支持）

## 最新提案

- [Reflections](https://github.com/CppCon/CppCon2019/blob/master/Presentations/reflections/reflections__andrew_sutton__cppcon_2019.pdf) & [Meta++](https://github.com/CppCon/CppCon2019/blob/master/Presentations/meta/meta__andrew_sutton__cppcon_2019.pdf) by Andrew Sutton —— 动手派！基于 [lock3/clang](https://gitlab.com/lock3/clang) 探索 C++ 的静态反射和元编程新特性
- [Pattern Matching: A Sneak Peek](https://github.com/CppCon/CppCon2019/blob/master/Presentations/pattern_matching_a_sneak_peek/pattern_matching_a_sneak_peek__michael_park__cppcon_2019.pdf) by Michael Park —— 关于 Pattern Matching 的最新提案（看到最后的正则表达式，感觉已经不像 C++ 的语法了）
- [Higher Order Functions in Modern C++ Existing Techniques And Function Ref](https://github.com/CppCon/CppCon2019/blob/master/Presentations/higher_order_functions_in_modern_cpp_existing_techniques_and_function_ref/higher_order_functions_in_modern_cpp_existing_techniques_and_function_ref__vittorio_romeo__cppcon_2019.pdf) by Vittorio Romeo —— 关于 `std::function_ref` 提案，实现 non-owning 语义（类似 `std::string_view`，和 Regular Type 派系明显冲突）
- [The Networking TS in Practice: Patterns for Real World Problems](https://github.com/CppCon/CppCon2019/blob/master/Presentations/the_networking_ts_in_practice_patterns_for_real_world_problems/the_networking_ts_in_practice_patterns_for_real_world_problems__robert_leahy__cppcon_2019.pdf) by Robert Leahy —— 关于 Network 提案的相关介绍（主要基于 Boost.Asio）

## 算法优化

- [Abseil's Open Source Hashtable: 2 Years In](https://github.com/CppCon/CppCon2019/blob/master/Presentations/abseils_open_source_hashtable_2_years_in/abseils_open_source_hashtable_2_years_in__matthew_kulukundis__cppcon_2019.pdf) by Matthew Kulukundis —— Google/Abseil 改进的哈希表
- [Floating-Point charconv Making Your Code 10x Faster With C++17's Final Boss](https://github.com/CppCon/CppCon2019/blob/master/Presentations/floatingpoint_charconv_making_your_code_10x_faster_with_cpp17s_final_boss/floatingpoint_charconv_making_your_code_10x_faster_with_cpp17s_final_boss__stephan_t_lavavej__cppcon_2019.pdf) by Stephan T. Lavavej —— 浮点数字符串转换的算法优化
- [Mesh: Automatically Compacting Your C++ Application's Memory](https://github.com/CppCon/CppCon2019/blob/master/Presentations/mesh_automatically_compacting_your_cpp_applications_memory/mesh_automatically_compacting_your_cpp_applications_memory__emery_berger__cppcon_2019.pdf) by Emery Berger —— 改进的 Allocator，号称比 TCMalloc 更优，在 Redis/Firefox 上效果明显

## 并发/并行

- [Concurrency in C++20 and beyond](https://github.com/CppCon/CppCon2019/blob/master/Presentations/concurrency_in_cpp20_and_beyond/concurrency_in_cpp20_and_beyond__anthony_williams__cppcon_2019.pdf) by Anthony Williams —— [C++ Concurrency in Action](http://www.cplusplusconcurrencyinaction.com/) 作者针对 C++ 最新标准介绍的并发编程
- [Asynchronous Programming in Modern C++](https://github.com/CppCon/CppCon2019/blob/master/Presentations/asynchronous_programming_in_modern_cpp/asynchronous_programming_in_modern_cpp__hartmut_kaiser__cppcon_2019.pdf) by Hartmut Kaiser —— 关于 C++ 标准并行库 [hpx](https://github.com/STEllAR-GROUP/hpx) 的概述
- [A Unifying Abstraction for Async in C++](https://github.com/CppCon/CppCon2019/blob/master/Presentations/a_unifying_abstraction_for_async_in_cpp/a_unifying_abstraction_for_async_in_cpp__eric_niebler_david_s_hollman__cppcon_2019.pdf) by Eric Niebler, David S Hollman —— 延迟计算 + 并行处理 = 异步新接口

## 效率工具

- [(Ab)using compiler tools](https://github.com/CppCon/CppCon2019/blob/master/Presentations/abusing_compiler_tools/abusing_compiler_tools__reka_kovacs__cppcon_2019.pdf) by Reka Kovacs —— [Bloaty](https://github.com/google/bloaty) 分析二进制文件空间占用，[BOLT](https://github.com/facebookincubator/BOLT) 优化二进制布局（类似 PGO）
- [Clang Based Refactoring](https://github.com/CppCon/CppCon2019/blob/master/Presentations/clang_based_refactoring/clang_based_refactoring__fred_tingaud__cppcon_2019.pdf) by Fred Tingaud —— [Clang-Tidy](http://clang.llvm.org/extra/clang-tidy/) 原理与应用
- [How to Write a Heap Memory Profiler](https://github.com/CppCon/CppCon2019/blob/master/Presentations/how_to_write_a_heap_memory_profiler/how_to_write_a_heap_memory_profiler__milian_wolff__cppcon_2019.pdf) by Milian Wolff —— [heaptrack](https://github.com/KDE/heaptrack) 堆内存分析工具
- [Lifetime analysis for everyone](https://github.com/CppCon/CppCon2019/blob/master/Presentations/lifetime_analysis_for_everyone/lifetime_analysis_for_everyone__matthias_gehre_gabor_horvath__cppcon_2019.pptx) by Matthias Gehre, Gabor Horvath —— Visual Studio 生命周期分析工具

由于 CppCon 2019 刚刚结束，相关资料还在持续上传，欢迎关注 **原文持续更新**。😉

以上只是一些 **个人见解**。如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man

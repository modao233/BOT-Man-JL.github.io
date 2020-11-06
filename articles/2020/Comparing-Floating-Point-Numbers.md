# 如何比较两个浮点数

> 2020/11/6
> 
> 夫自细视大者不尽，自大视细者不明。——《庄子·秋水》

很久没写文章了，最近抽时间拼凑一篇短文，记录一下 **比较两个（二进制）浮点数是否相等** 的几种方法（虽然本文使用 C++ 演示，但原理和语言无关）。

> 本文主要参考了 “浮点数专家” [Bruce Dawson](https://randomascii.wordpress.com/) 写的 [Comparing Floating Point Numbers, 2012 Edition](https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/)，也推荐他 [关于浮点数的其他文章](https://randomascii.wordpress.com/category/floating-point/)（最近还一直在更新 👍）。
> 
> 本文提到的 `Equals()` / `AlmostEqualsAbs()` / `AlmostEqualsRel()` / `AlmostEqualsUlp()` 函数的具体实现参考 [`almost_equals.h`](Comparing-Floating-Point-Numbers/almost_equals.h)（[在线演示](https://godbolt.org/z/xofPTx)）👈

## 绝对相等？不靠谱

比较两个数是否相等，最直观的 **比较方法** 就是直接使用 `==` 进行比较：

``` cpp
f1 == f2  // Equals()
```

根据 [IEEE 754 浮点数标准](https://en.wikipedia.org/wiki/IEEE_754)，浮点数 $\pm p \cdot b^e$（其中 $b$ 为 固定的基数，$\pm$ 为 符号位，$p$ 为 尾数，$e$ 为 指数）表示为 **符号位-指数-尾数** 形式：

- 十进制浮点数（`b == 10`）就是 数学中常用的 [**科学计数法** _(scientific notation)_](https://en.wikipedia.org/wiki/Scientific_notation)
- 二进制浮点数（`b = 2`）常用于 计算机中的表示和存储，其中 [单精度 _(single-precision)_](https://en.wikipedia.org/wiki/Single-precision_floating-point_format) 对应 32 位，[双精度 _(double-precision)_](https://en.wikipedia.org/wiki/Double-precision_floating-point_format) 对应 64 位

> 在 C++ 中，`0.1` 属于 双精度浮点数，`0.1f` 属于 单精度浮点数。
> 
> 为什么计算机使用二进制浮点数？因为基于现有的 CPU 架构，二进制运算效率更高。

虽然在数学上，任意十进制数和二进制数可以 **相互转换**，它们的 浮点数表示形式 也可以相互转换：

| 十进制小数 | 十进制科学计数法 | 二进制小数 | 二进制科学计数法 |
|---|---|---|---|
| `0.125` | $1.25 \times 10^{-1}$ | `0.001` | ($2^{0} + 0) \times 2^{-3}$ |
| `0.1` | $1.0 \times 10^{-1}$ | `0.000110011...` <br/>（`0011` 循环）| $(2^{0} + 2^{-1} + 2^{-4} + 2^{-5} + ...) \times 2^{-4}$ |

但是，**有限精度** 的二进制浮点数 **不能准确表示** 所有的十进制数（[在线转换工具](https://www.h-schmidt.net/FloatConverter/IEEE754.html)）：

- 在二进制科学计数法中，十进制数 `0.1` 的尾数 $p$ 是 `1.100110011...`（`0011` 循环）
- 由于 32 位单精度浮点数 的尾数 $p$ 只有 23 位，所以 `1.100110011...` 只能保留小数点后 23 位，**近似表示** 为二进制的 `1.10011001100110011001101`
- 所以，近似表示的二进制数为 `0.000110011001100110011001101`，约等于 十进制的 `0.10000000149`，导致转换 **出现误差**

**存在问题** —— **有限精度** 的浮点数 **近似表示** 会带来误差，而浮点数运算则会 **放大误差**，所以 判断两个浮点数（其中至少一个是运算结果）的 **绝对相等** 往往是不可靠的：

``` cpp
float sum = 0.0f;
for (int i = 0; i < 10; ++i) { sum += 0.1f; }
assert(Equals(sum, 1.0f) == false);  // expect true
```

为此，编译器会 **警告** 浮点数的 `==`/`!=` 比较：

```
warning: comparing floating point with == or != is unsafe [-Wfloat-equal]
```

**解决办法** —— 由于误差的存在，只能 比较两个浮点数是否 **近似相等**：

``` cpp
assert(AlmostEqualsAbs(sum, 1.0f) == true);
assert(AlmostEqualsRel(sum, 1.0f) == true);
assert(AlmostEqualsUlp(sum, 1.0f) == true);
```

## 近似相等 —— 绝对误差

最简单的 **比较方法** 是直接比较 两数差值的绝对值 是否小于一个 **允许的误差值** `epsilon`：

``` cpp
fabs(f1 - f2) <= epsilon  // AlmostEqualsAbs()
```

其中，`epsilon` 常用 [`FLT_EPSILON` / `DBL_EPSILON`](https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon)，表示在 单精度 `1.0f` / 双精度 `1.0` 附近的最小误差。

**存在问题** —— 由于 **绝对误差** 是一个固定值，不一定适用于 任意浮点数的比较：

``` cpp
assert(AlmostEqualsAbs(67329.234f, 67329.242f) == false);  // expect true
assert(AlmostEqualsAbs(1.2e-32f, 2.4e-32f) == true);       // expect false
```

- 当比较的两个数 远大于 `1` 时，`FLT_EPSILON`/`DBL_EPSILON` 允许的误差过小（例如，虽然 `67329.234f` 和 `67329.242f` 一般被认为近似相等，但差值远大于 `FLT_EPSILON`，所以被误判为不相等）
- 当比较的两个数 远小于 `1` 时，`FLT_EPSILON`/`DBL_EPSILON` 允许的误差过大（例如，尽管 `1.2e-32f` 和 `2.4e-32f` 相差了一倍，应该被认为不相等，但差值远小于 `FLT_EPSILON`，所以被误判为近似相等）

**解决办法** —— 需要借助 **相对误差** 弥补上述局限性：

``` cpp
assert(AlmostEqualsRel(67329.234f, 67329.242f) == true);
assert(AlmostEqualsUlp(67329.234f, 67329.242f) == true);
assert(AlmostEqualsRel(1.2e-32f, 2.4e-32f) == false);
assert(AlmostEqualsUlp(1.2e-32f, 2.4e-32f) == false);
```

## 近似相等 —— 相对误差

**a)** 一种 **比较方法** 是 把绝对的 `epsilon` 放缩到待比较数的 **量级** _(magnitude)_，即 `epsilon` 乘以 两数绝对值的较大值：

``` cpp
fabs(f1 - f2) <= epsilon * std::max(fabs(f1), fabs(f2))  // AlmostEqualsRel()
```

**b)** 另一种方法是 基于浮点数二进制表示的 [ULP _(unit in the last place)_](https://en.wikipedia.org/wiki/Unit_in_the_last_place) 进行比较：
  - 根据 IEEE 754 标准，**相邻** 两个浮点数的 **二进制表示** 恰好也是相邻的 —— 相邻两个浮点数之间的距离 就是 “它们的 ULP”（定义参考：[What Every Computer Scientist Should Know About Floating-Point Arithmetic by _David Goldberg_](https://www.itu.dk/~sestoft/bachelor/IEEE754_article.pdf)）
  - 例如，距离 `1.0f` 最近的下一个浮点数是 [`nextafter(1.0f)`](https://en.cppreference.com/w/cpp/numeric/math/nextafter)，恰好等于 `1.0f + FLT_EPSILON`，所以 `ULP(1.0f)` 等于 [`FLT_EPSILON`](https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon)（即 $2^{-23}$）：

| 浮点数 | 二进制表示 | 二进制科学计数法 |
|---|---|---|
| `1.0f` | `0 01111111 00000000000000000000000` | $(2^{0} + 0) \times 2^{0}$ |
| `nextafter(1.0f)` | `0 01111111 00000000000000000000001` | $(2^{0} + 2^{-23}) \times 2^{0}$ |

直观上，**比较方法** 很简单 —— 比较浮点数的 **二进制表示** 的 差值的绝对值 是否小于一个 **允许的 ULP 误差** `max_ulp`：

``` cpp
abs(Biased(Bits(f1)) - Biased(Bits(f2))) <= max_ulp  // AlmostEqualsUlp()
```

- 首先，将两个 单精度/双精度 **浮点数** 直接 **按位** 转成 32/64 位 **整数**
- 然后，由于 这两个整数表示为 [**符号位-数值** _(sign-magnitude)_](https://en.wikipedia.org/wiki/Signed_number_representations#Signed_magnitude_representation) 格式，导致 [`+0`/`-0`](https://en.wikipedia.org/wiki/Signed_zero) 的表示形式不一致，所以需要转换为 连续范围 上的 **无符号整数**
- 最后，再比较 两个无符号整数 **差值的绝对值** 是否小于 `max_ulp`

> 另外，还需要考虑 [`+∞`/`-∞`](https://en.wikipedia.org/wiki/Infinity) 和 [`NaN` _(not a number)_](https://en.wikipedia.org/wiki/NaN) 等情况。

根据 [`gtest-internal.h`](https://github.com/google/googletest/blob/master/googletest/include/gtest/internal/gtest-internal.h)，一般 `max_ulp` 取值为 `4`：

> The maximum error of a single floating-point operation is 0.5
> units in the last place.  On Intel CPU's, all floating-point
> calculations are done with 80-bit precision, while double has 64
> bits.  Therefore, 4 should be enough for ordinary use.

再举个例子，`4.0f` 和 它的下一个浮点数 `nextafter(4.0f)` 之间相差了 4 倍的 `FLT_EPSILON`（即 `ULP(4.0f) == 4 FLT_EPSILON`）：

| 浮点数 | 二进制表示 | 二进制科学计数法 |
|---|---|---|
| `4.0f` | `0 10000001 00000000000000000000000` | $(2^{0} + 0) \times 2^{2}$ |
| `nextafter(4.0f)` | `0 10000001 00000000000000000000001` | $(2^{0} + 2^{-23}) \times 2^{2}$ |

[align-center]

![ulp-vs-epsilon](Comparing-Floating-Point-Numbers/ulp-vs-epsilon.png)

所以，无法使用 **绝对误差** 判断 `4.0f` 和 `nextafter(4.0f)` 是否近似相等，只能使用 **相对误差**：

``` cpp
const float next_after_4 = nextafter(4.0f, FLT_MAX);
assert(AlmostEqualsAbs(4.0f, next_after_4) == false);  // expect true
assert(AlmostEqualsRel(4.0f, next_after_4) == true);
assert(AlmostEqualsUlp(4.0f, next_after_4) == true);
```

**存在问题** —— 如果需要比较的数值 **非常接近 0**，就无法使用 **相对误差** 和 0 比较：

``` cpp
assert(AlmostEqualsRel(FLT_EPSILON, 0.0f) == false);  // expect true
assert(AlmostEqualsUlp(FLT_EPSILON, 0.0f) == false);  // expect true
```

即使放大允许的误差范围，也不一定可行 —— 因为和 0 比起来，相对误差还是太大了 —— 在这种情况下，基于相对误差的比较方法 往往是 **没有实际意义** 的：

``` cpp
assert(AlmostEqualsRel(FLT_EPSILON, 0.0f, 0.1f) == false);  // expect true
assert(AlmostEqualsUlp(FLT_EPSILON, 0.0f, 1000) == false);  // expect true
```

**解决办法** —— 只能使用 **绝对误差** 进行 “有意义” 的比较：

``` cpp
assert(AlmostEqualsAbs(FLT_EPSILON, 0.0f) == true);
```

## 常量的陷阱

需要注意，**从低精度转为高精度**（从 `float` 转为 `double`）会导致 “巨大” 的误差：

``` cpp
assert(Equals<double>(0.1f, 0.1) == false);           // expect true
assert(AlmostEqualsAbs<double>(0.1f, 0.1) == false);  // expect true
assert(AlmostEqualsRel<double>(0.1f, 0.1) == false);  // expect true
assert(AlmostEqualsUlp<double>(0.1f, 0.1) == false);  // expect true
```

为此，可以通过 **放大允许的误差**，缓解精度丢失的问题：

``` cpp
assert(AlmostEqualsAbs<double>(0.1f, 0.1, static_cast<double>(FLT_EPSILON)) ==
       true);
assert(AlmostEqualsRel<double>(0.1f, 0.1, static_cast<double>(FLT_EPSILON)) ==
       true);
assert(AlmostEqualsUlp<double>(0.1f, 0.1, FLT_EPSILON / DBL_EPSILON) == true);
```

相反，**从高精度转为低精度**（从 `double` 转为 `float`）则会抹除细微的误差：

``` cpp
assert(Equals(static_cast<float>(0.1), 0.1f) == true);
```

另外，由于精度的限制，两个 **非常接近**（相差 `0 ULP`）的十进制数 **字面量** _(literal)_，只能表示为 相同的二进制数：

``` cpp
assert(Equals(67329.234f, 67329.235f) == true);  // expect false
```

## 写在最后 [no-toc]

[Bruce Dawson 的总结是 “没有银弹”](https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/) —— 在比较两个浮点数是否近似相等时，需要根据具体场景，选择更合适的比较方法：

- 如果 要判断 一个浮点数 **是否接近 0**
  - 应该使用 **绝对误差**
  - 因为此时的 **相对误差** 没有实际意义
- 如果 要比较的 两个浮点数 **都不是 0**
  - 一般使用更通用的 **相对误差**
  - 但如果能 **提前判断** 允许的误差范围，也可以使用 **绝对误差**
- 所以，并不存在 判断 **任意** 两个浮点数 的通用方法 🙃

现实世界也一样 ——

- 对于有钱的人来说，`AlmostEquals(百元钞票, 0) == true`
- 对于没钱的人来说，`AlmostEquals(一元硬币, 0) == false`

最后，今年双十一将抽取一名在 **下方留言** 的幸运读者，~~送出一张 **玛莎拉蒂 5 元代金券**~~ 🙃

[align-center]

[img=width:80%]

![Maserati-Voucher](Comparing-Floating-Point-Numbers/Maserati-Voucher.jpg)

[align-center]

（图片来自网络）

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2020, BOT Man

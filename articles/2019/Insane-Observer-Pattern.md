# 令人抓狂的观察者模式

> 2019/1/30
> 
> 本文主要介绍观察者模式 C++ 实现的一些问题，并给出基于 chromium 的解决方案。

读完这篇文章，带你绕过 C++ 观察者模式的那些坑。

## 背景 TL;DR

[观察者模式](../2017/Design-Patterns-Notes-3.md#Observer) 是一个大家所熟知的设计模式。如果对观察者模式还不是很了解，可以先阅读 [理解观察者、中介者模式](../2017/Observer-Mediator-Explained.md)。

观察者模式主要用于解耦消息的发送者和接收者，能让代码更具有可扩展性。

> 用生活中的例子解释观察者模式 —— 有一家 **农场** 🚜 生产面粉，有多家 **面包房** 🏠 从他们那里购进面粉：
> 
> - 如果不采用观察者模式，农场只卖现货 —— 每次生产了新一批的面粉，就需要派人去各个面包房 **推销** 面粉（农场只能把面粉卖给推销员能到达的面包房）
> - 如果采用观察者模式，农场可以卖期货 —— 哪些面包房需要面粉，留下电话号码预定；等到新一批面粉生产出来了，就从电话簿里逐个 **通知**（农场不需要关心最终把面粉卖给了哪些面包房，只需要卖出去就行）
> 
> 基于后一种模式：农场可以更快的开拓销路，甚至把面粉卖给更远的面包房；面包房可以引入更多的供应商，不仅仅从当地农场购买面粉。

定义面粉观察者 `FarmObserver`：

``` cpp
class FarmObserver {
 public:
  virtual void OnFlourReady() = 0;

 protected:
  virtual ~FarmObserver() = default;
};
```

定义农场 `Farm`：

``` cpp
class Farm {
 public:
  void AddObserver(FarmObserver* ob);
  void RemoveObserver(FarmObserver* ob);

 private:
  void NotifyFlourReady();
};
```

定义面包房 `Bakery`：

``` cpp
class Bakery : public FarmObserver {
 protected:
  void OnFlourReady() override;
};
```

假设有 1 个农场 和 2 个面包房：

``` cpp
Farm farm;
Bakery bakery1, bakery2;
```

面包房签订了农场的面粉购买合同，成为面粉观察者：

``` cpp
farm.AddObserver(&bakery1);
farm.AddObserver(&bakery2);
```

当麦子 🌾 成熟时，农场 🚜 开始生成面粉 📦，然后通知所有观察者 🕵️‍；面包房 🏠 买回面粉后，开始生成 面包 🍞、曲奇 🍪、蛋糕 🎂...

``` cpp
farm.OnWheatReady()
-> farm.ProduceFlourFromWheat()
-> farm.NotifyFlourReady()
   -> bakery1.OnFlourReady()
      -> bakery1.BuyFlourFromFarm()
      -> bakery1.ProduceBreadFromFlour()
      -> bakery1.ProduceCookieFromFlour()
   -> bakery2.OnFlourReady()
      -> bakery2.BuyFlourFromFarm()
      -> bakery2.ProduceCakeFromFlour()
```

但如果系统内大量使用观察者模式，可能会出现令人抓狂的问题。

> 理想很丰满，现实很骨感。

## 生命周期问题

> 兄弟们，我先撤了。

### 问题：观察者先销毁

> 农场还在，面包房跑路了。

假设 bakery1 对象失效了，导致 `farm.NotifyFlourReady()` 调用的 `bakery1.OnFlourReady()` 会出现异常。

**解决办法**（约定俗成的）：

如果观察者不再继续观察，那么需要把它从被观察者的列表中移除：

``` cpp
farm.RemoveObserver(&bakery1);
```

**更安全的方法**（参考 chromium）：

- 使用 [RAII _(resource acquisition is initialization)_](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization) 风格的资源管理，例如
  - [`ScopedObserver`](https://github.com/chromium/chromium/blob/master/base/scoped_observer.h) 对象在析构时自动调用 `RemoveObserver`
  - 在对象析构调用 `RemoveObserver` 时，需要确保观察者和被观察者仍然有效
- 使用弱引用检查观察者的有效性，例如
  - [`base::ObserverList`](https://github.com/chromium/chromium/blob/master/base/observer_list.h) + [`base::CheckedObserver`](https://github.com/chromium/chromium/blob/master/base/observer_list_types.h) 在通知前检查观察者的有效性，避免因为通知无效观察者导致崩溃

### 问题：被观察者先销毁

> 面包房还在，农场跑路了。

由于 `Bakery` 在生命周期里，需要引用 `farm_` 对象（面包房从农场购买面粉），所以往往会把 `AddObserver/RemoveObserver` 放到 `Bakery` 的构造函数/析构函数里：

``` cpp
class Bakery : public FarmObserver {
 public:
  Bakery(Farm* farm) : farm_(farm) {
    if (farm_)
      farm_->AddObserver(this);
  }
  ~Bakery() {
    if (farm_)
      farm_->RemoveObserver(this);
  }

 protected:
  void OnFlourReady() override {
    if (!farm_)
      return;

    // Buy Flour from farm_
    // Produce ... from Flour
  }

 private:
  Farm* farm_ = nullptr;
};
```

假设 farm_ 对象失效了，导致 `~Bakery()` 调用的 `RemoveObserver()` 可能出现异常。

**解决办法**（参考 chromium）：

- 引入外部协调者，完成注册/反注册操作，例如
  - 让 `farm` 和 `bakery1`/`bakery2` 的生命周期管理者，添加/移除观察关系
  - 而不是在 `Bakery` 的构造函数/析构函数里实现（回到最初的方式）
- 被观察者销毁时，通知观察者反注册，例如
  - 在 `views::View` 析构时，通知观察者 [`views::ViewObserver::OnViewIsDeleting`](https://github.com/chromium/chromium/blob/master/ui/views/view_observer.h)
  - 注意：在回调时，不能直接从 `std::list`/`std::vector` 容器中移除观察者；而应该标记为“待移除”，然后等迭代结束后移除（参考 [`base::ObserverList`](https://github.com/chromium/chromium/blob/master/base/observer_list.h)）
- 用弱引用替换裸指针，移除时检查被观察者的有效性，例如
  - 使用 [`base::WeakPtr`](https://github.com/chromium/chromium/blob/master/base/memory/weak_ptr.h) 把 `Farm* farm_` 替换为 `base::WeakPtr<Farm> farm_`（比较灵活）

## 调用关系问题

> 这些在合同里没说清楚。

### 问题：死循环

> how old are you? —— 怎么老是你？

除了 [sec|问题：被观察者先销毁] 提到的

> 在回调时，不能直接从 `std::list`/`std::vector` 容器中移除观察者（类似于下边的代码）
> 
> ``` cpp
> for(auto it = c.begin(); it != c.end(); ++it)
>   c.erase(it);  // crash in the next turn!
> ```

在回调时，还要避免隐式的同步触发当前回调；否则，一旦逻辑变得复杂，很容易进入死循环。

例如，对于用户配置管理、数据同步管理的代码：

- 同步管理器 `SyncManager` 监听配置数据 `ConfigModel` 的变化，即 `class SyncManager : public ConfigObserver`
- 当 `ConfigModel` 更新后，通知观察者 `ConfigObserver::OnDataUpdated`
  - 同步管理器在处理 `SyncManager::OnDataUpdated` 时，可能先把 `ConfigModel` 的“最近一次同步时间”字段更新，再与服务器同步
- 由于 `ConfigModel` 更新，导致再次通知所有观察者 `ConfigObserver::OnDataUpdated`
  - 从而进入了死循环

实际场景中，如果触发条件比较隐蔽时，问题就难以复现，例如

- 在处理 `SyncManager::OnDataUpdated` 时，调用检查配置的合法性
  - 如果配置非法，先修正用户配置（例如，`2019/2/29` -> `2018/3/1`）
  - 然后更新原来有问题的配置，间接导致重入
- 假设测试用例没有覆盖“配置非法”的分支，配置就不会被修正，从而这个问题也不会被发现

**解决办法**：

方法很简单：

- （绕开）根据具体情况，打破循环的条件，例如
  - 在处理 `SyncManager::OnDataUpdated` 时，检查是哪些配置更新了
  - 如果只是“最近一次同步时间”字段的更新，就直接跳过合法性检查
- （根治）重构逻辑，避免出现这种情况，例如
  - 让 `ConfigModel` 进行合法性检查、配置修正的操作
  - `SyncManager` 收到 `OnDataUpdated` 拿到的就已经时正确的数据了

### 问题：乱序回调

> 东西还没准备好就别来找我！

真实世界往往并不像农场和面包房那么简单 —— 观察者可能同时观察多个状态的变化，而这组状态变化的通知顺序往往是不确定的。

例如，对于用户头像按钮根据是否登录、用户配置显示的代码：

- 用户头像按钮 `AvatarButton` 监听
  - 配置数据 `ConfigModel` 的变化，即 `class AvatarButton : public ConfigObserver`
  - 登录状态 `LoginStatus` 的变化，即 `class AvatarButton : public LoginObserver`
- 用户登录时，配置数据会同步到本地；登出时，配置数据会被还原（都可能触发 `ConfigModel` 更新）
- 当 `LoginStatus` 登录/登出时，通知观察者 `LoginObserver::OnLogin/OnLogout`
  - 用户头像按钮在处理 `AvatarButton::OnLogin` 时，显示用户的头像图片
  - 用户头像按钮在处理 `AvatarButton::OnLogout` 时，显示默认未登录图片
- 当 `ConfigModel` 更新后，通知观察者 `ConfigObserver::OnDataUpdated`
  - 用户头像按钮在处理 `AvatarButton::OnDataUpdated` 时，根据配置修改按钮样式（圆形/方形/心形）

实际场景中，两个事件的触发时机是不明确的，例如

- 在处理 `AvatarButton::OnDataUpdated` 时，用户的登录状态不一定是最新的
  - 可能用户登录了，显示的是未登录图片
  - 而用户登出了，显示的还是登录头像
- 在收到 `OnLogin/OnLogout` 消息时，没有明确是已经登录成功，还是正在开始尝试登录（可能之后失败了）

如果模块读取中间状态后，只是内自己使用，问题不大；如果模块利用中间状态，计算出一个结果，然后传播或保存起来，那么可能出错，例如

- 如果 `AvatarButton` 只是根据 是否登录、用户配置 显示不同的按钮内容，问题不大
- 在处理 `AvatarButton::OnDataUpdated` 时，如果用户配置变为心形样式 且 用户已登录，那么把 logo 隐藏
  - 假设这时用户已经登录成功，但是读取到的 `LoginStatus` 没有更新（仍为未登录状态）
  - 那么 logo 就不会被隐藏了

**解决办法**（参考 chromium）：

- 分别使用两个回调事件，表示 状态正在变化 和 状态变化完成，例如
  - 在 `views::Widget` 销毁时，通知观察者 [`views::WidgetObserver::OnWidgetDestroying`](https://github.com/chromium/chromium/blob/master/ui/views/widget/widget_observer.h)
  - 在 `views::Widget` 销毁后，通知观察者 [`views::WidgetObserver::OnWidgetDestroyed`](https://github.com/chromium/chromium/blob/master/ui/views/widget/widget_observer.h)
- 将操作延迟到相关回调全部结束后执行，避免读取到变化的中间状态，例如
  - 在观察者回调函数 `OnDataUpdated` 调用 [`base::TaskRunner::PostTask`](https://github.com/chromium/chromium/blob/master/base/task_runner.h)，把实际的处理操作抛到队尾，延迟到当前任务结束后异步执行
  - 当操作被执行时，状态变化已经结束（已经不在同一个调用栈里），从而避免读取到不确定的变化中的状态
- （根治）重构逻辑，使用中介者模式协调各个事件的回调顺序，参考 [理解观察者、中介者模式](../2017/Observer-Mediator-Explained.md)

## 写在最后

本文仅是我在项目中遇到的一些问题，以及我的一些个人理解。如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man

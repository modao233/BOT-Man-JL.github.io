# 令人抓狂的观察者模式

> 2019/1/30
> 
> 本文主要介绍观察者模式 C++ 实现的一些问题，并给出基于 chromium 的解决方案。

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
  - 在 `views::View` 析构时通知观察者 [`views::ViewObserver::OnViewIsDeleting`](https://github.com/chromium/chromium/blob/master/ui/views/view_observer.h)
  - 注意：在回调时，不能直接从 `std::list`/`std::vector` 容器中移除观察者；而应该标记为“待移除”，然后等迭代结束后移除（参考 [`base::ObserverList`](https://github.com/chromium/chromium/blob/master/base/observer_list.h)）
- 用弱引用替换裸指针，例如
  - 使用 [`base::WeakPtr`](https://github.com/chromium/chromium/blob/master/base/memory/weak_ptr.h) 把 `Farm* farm_` 替换为 `base::WeakPtr<Farm> farm_`（比较灵活）

## 回调顺序问题

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

例如，对于用户配置管理的代码：

- 同步管理器 `SyncManager` 监听配置数据 `ConfigModel` 的变换，即 `class SyncManager : public ConfigObserver`
- 当 `ConfigModel` 更新时，通知观察者 `ConfigObserver::OnDataUpdated`
  - 同步管理器在处理 `SyncManager::OnDataUpdated` 时，把 `ConfigModel` 的“最近一次同步时间”字段更新，然后与服务器同步
- 由于 `ConfigModel` 更新，导致再次通知所有观察者 `ConfigObserver::OnDataUpdated`
  - 从而进入了死循环

**解决办法**：

方法很简单，只需要根据具体情况，打破循环的条件即可。例如，上边的例子中，在处理 `SyncManager::OnDataUpdated` 时，检查是哪些配置更新了；如果只是“最近一次同步时间”字段的更新，就直接跳过。

### 问题：处在中间状态

> 东西还没准备好就别来找我！

**解决办法**（参考 chromium）：

- 分别使用两个回调事件，表示 状态正在变换 和 状态变换完成，例如
  - 在 `views::Widget` 销毁时通知观察者 [`views::WidgetObserver::OnWidgetDestroying`](https://github.com/chromium/chromium/blob/master/ui/views/widget/widget_observer.h)
  - 在 `views::Widget` 销毁后通知观察者 [`views::WidgetObserver::OnWidgetDestroyed`](https://github.com/chromium/chromium/blob/master/ui/views/widget/widget_observer.h)

### 问题：观察者之间的顺序

> 谁先发货？

- `ConfigObserver::OnDataUpdated`
- `LoginObserver::OnLogin`

**解决办法**：

- 使用中介者模式协调变化顺序，参考 [理解观察者、中介者模式](../2017/Observer-Mediator-Explained.md)
- 将操作抛到队尾异步处理，避免读取到变换的中间状态，例如
  - 在观察者回调函数 `OnDataUpdated` 调用 [`base::TaskRunner::PostTask`](https://github.com/chromium/chromium/blob/master/base/task_runner.h) 把实际的处理操作，延迟到当前任务结束后执行
  - 在实际操作执行时，状态变换已经结束（已经不在同一个调用栈里），从而避免读取到不确定的变化中的状态

## 写在最后

本文仅是我的一些个人理解。如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man


#include <functional>
#include <memory>
#include <unordered_map>

enum Action {
  ClickNew,
  ClickOpen,
  ClickSave,
};

class File {
 public:
  static std::unique_ptr<File> New();   // opreation
  static std::unique_ptr<File> Open();  // opreation
  void Save() const;                    // opreation

 private:
  // data
};

using FilePtr = std::unique_ptr<File>;

using Command = std::function<void()>;

class Client {
  // Receiver
 private:
  FilePtr file_;  // data storage

  // Indirection
 private:
  std::unordered_map<Action, Command> handlers_;

 public:
  Client() {
    handlers_.emplace(ClickNew, [this] { file_ = File::New(); });
    handlers_.emplace(ClickOpen, [this] { file_ = File::Open(); });
    handlers_.emplace(ClickSave, [this] {
      if (!file_)
        return;
      file_->Save();
    });
  }

  // Sender
 public:
  void OnAction() const {
    Action action /* = ... */;
    handlers_.at(action)();
  }
};

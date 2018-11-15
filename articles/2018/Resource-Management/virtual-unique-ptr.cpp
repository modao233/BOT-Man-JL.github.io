#include <memory>
#include <string>
#include <utility>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;

class BookmarkNode {
 public:
  virtual ~BookmarkNode() = default;

  // common getters
  virtual bool is_url() const = 0;
  virtual string& title() = 0;

  // url getters
  virtual string& url() = 0;

  // folder getters
  using Children = vector<unique_ptr<BookmarkNode>>;
  virtual Children& children() = 0;
};

class UrlNode : public BookmarkNode {
 public:
  UrlNode(string title, string url) : title_(title), url_(url) {}

  bool is_url() const override { return true; }
  string& title() override { return title_; }
  string& url() override { return url_; }

  Children& children() override {
    static Children dummy;
    return dummy;
  }

 private:
  string title_;
  string url_;
};

class FolderNode : public BookmarkNode {
 public:
  FolderNode(string title, Children children)
      : title_(title), children_(std::move(children)) {}

  bool is_url() const override { return false; }
  string& title() override { return title_; }
  Children& children() override { return children_; }

  string& url() override {
    static string dummy;
    return dummy;
  }

 private:
  string title_;
  Children children_;
};

unique_ptr<BookmarkNode> NewUrlNode(string title, string url) {
  return std::make_unique<UrlNode>(title, url);
}

template <typename... Nodes>
unique_ptr<BookmarkNode> NewFolderNode(string title, Nodes&&... nodes) {
  BookmarkNode::Children children;
  using Expander = int[];
  (void)Expander{
      0, ((void)children.emplace_back(std::forward<Nodes>(nodes)), 0)...};

  return std::make_unique<FolderNode>(title, std::move(children));
}

int main() {
  auto root = NewFolderNode(
      "Bookmark Root", NewUrlNode("Github", "https://github.com/"),
      NewUrlNode("BOT Man", "https://bot-man-jl.github.io/"),
      NewFolderNode("Search Engines",
                    NewUrlNode("Bing", "https://cn.bing.com/"),
                    NewUrlNode("Baidu", "https://www.baidu.com/")));
  return 0;
}

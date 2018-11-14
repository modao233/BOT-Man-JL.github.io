#include <memory>
#include <string>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;

class BookmarkNode {
 public:
  virtual ~BookmarkNode() = default;

  // common getters
  virtual bool is_url() const = 0;
  virtual string title() = 0;

  // url getters
  virtual string url() = 0;

  // folder getters
  using Children = vector<unique_ptr<BookmarkNode>>;
  virtual Children& children() = 0;
};

class UrlNode : public BookmarkNode {
 public:
  UrlNode(string title, string url) : title_(title), url_(url) {}

  bool is_url() const override { return true; }
  string title() override { return title_; }
  string url() override { return url_; }

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
  FolderNode(string title, Children&& children)
      : title_(title), children_(std::move(children)) {}

  bool is_url() const override { return false; }
  string title() override { return title_; }
  Children& children() override { return children_; }

  string url() override { return {}; }

 private:
  string title_;
  Children children_;
};

unique_ptr<BookmarkNode> NewBookmarkNode(string title, string url) {
  return std::make_unique<UrlNode>(title, url);
}

unique_ptr<BookmarkNode> NewBookmarkNode(string title,
                                         BookmarkNode::Children children) {
  return std::make_unique<FolderNode>(title, std::move(children));
}

template <typename... Args>
BookmarkNode::Children NewBookmarkNodes(Args&&... args) {
  BookmarkNode::Children ret;

  auto emplace_back = [&ret](auto&& arg) {
    ret.emplace_back(std::forward<decltype(arg)>(arg));
    return 0;
  };

  using expender = int[];
  (void)expender{0, (emplace_back(std::forward<Args>(args)), 0)...};
  return ret;
}

int main() {
  auto root = NewBookmarkNode(
      "Bookmark Root",
      NewBookmarkNodes(
          NewBookmarkNode("Github", "https://github.com/"),
          NewBookmarkNode("BOT Man", "https://bot-man-jl.github.io/"),
          NewBookmarkNode(
              "Search Engines",
              NewBookmarkNodes(
                  NewBookmarkNode("Bing", "https://cn.bing.com/"),
                  NewBookmarkNode("Baidu", "https://www.baidu.com/")))));
  return 0;
}

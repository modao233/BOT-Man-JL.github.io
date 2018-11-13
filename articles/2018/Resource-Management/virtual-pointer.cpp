#include <string>
#include <vector>

using std::string;
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
  using Children = vector<BookmarkNode*>;
  virtual Children children() = 0;
};

class UrlNode : public BookmarkNode {
 public:
  UrlNode(string title, string url) : title_(title), url_(url) {}

  bool is_url() const override { return true; }
  string title() override { return title_; }
  string url() override { return url_; }

  Children children() override { return {}; }

 private:
  string title_;
  string url_;
};

class FolderNode : public BookmarkNode {
 public:
  FolderNode(string title, Children children)
      : title_(title), children_(children) {}

  bool is_url() const override { return false; }
  string title() override { return title_; }
  Children children() override { return children_; }

  string url() override { return {}; }

 private:
  string title_;
  Children children_;
};

constexpr auto kBookmarkNodeSize = sizeof(BookmarkNode);
constexpr auto kUrlNodeSize = sizeof(UrlNode);
constexpr auto kFolderNodeSize = sizeof(FolderNode);

void DestroyBookmarkTree(BookmarkNode* root) {
  if (!root || root->is_url())
    return;

  for (BookmarkNode* child : root->children())
    DestroyBookmarkTree(child);

  delete root;
}

int main() {
  auto root = new FolderNode{
      "Bookmark Root",
      {
          new UrlNode{"Github", "https://github.com/"},
          new UrlNode{"BOT Man", "https://bot-man-jl.github.io/"},
          new FolderNode{
              "Search Engines",
              {
                  new UrlNode{"Bing", "https://cn.bing.com/"},
                  new UrlNode{"Baidu", "https://www.baidu.com/"},
              },
          },
      },
  };
  DestroyBookmarkTree(root);
  return 0;
}

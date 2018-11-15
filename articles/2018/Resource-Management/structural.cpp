#include <string>
#include <vector>

using std::string;
using std::vector;

struct BookmarkNode {
  // common properties
  bool is_url;
  string title;

  // url properties
  string url;

  // folder properties
  using Children = vector<BookmarkNode>;
  Children children;
};

int main() {
  auto root = BookmarkNode{
      false,
      "Bookmark Root",
      "",
      {
          BookmarkNode{true, "Github", "https://github.com/", {}},
          BookmarkNode{true, "BOT Man", "https://bot-man-jl.github.io/", {}},
          BookmarkNode{
              false,
              "Search Engines",
              "",
              {
                  BookmarkNode{true, "Bing", "https://cn.bing.com/", {}},
                  BookmarkNode{true, "Baidu", "https://www.baidu.com/", {}},
              },
          },
      },
  };
  return 0;
}

#include <string>
#include <vector>

using std::string;
using std::vector;

struct BookmarkNode {
  // common properties
  string title;

  // url properties
  string url;

  // folder properties
  using Children = vector<BookmarkNode>;
  Children children;
};

int main() {
  auto root = BookmarkNode{
      "Bookmark Root",
      "",
      {
          BookmarkNode{"Github", "https://github.com/", {}},
          BookmarkNode{"BOT Man", "https://bot-man-jl.github.io/", {}},
          BookmarkNode{
              "Search Engines",
              "",
              {
                  BookmarkNode{"Bing", "https://cn.bing.com/", {}},
                  BookmarkNode{"Baidu", "https://www.baidu.com/", {}},
              },
          },
      },
  };
  return 0;
}

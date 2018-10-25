# 使用高阶函数消除循环和临时变量

> 2018/10/25
>
> 你可能听说过的：Map/Reduce/Filter

## 写在前面

- 尽量使用语义更明确的字符串查找/比较函数
  - !Equal    -> `0 != active_page_url_.compare(clip_board_text)`
  - StartWith -> `0 == active_page_url_.find(clip_board_text)`
  - !Contains -> `std::wstring::npos == clip_board_text.find(STR_HTTP)`
- cheerio.js 使用 map/forEach 化简逻辑 `$('.myclass ul').map((i, e) => $('li a', e).map((i, e) => $(e).text()));`

``` js
// => [ [ 'id', 'id', ...], [ 'id', 'id', ...], ... ]
function selectPeople(config) {
  // https://stackoverflow.com/questions/1427608/fast-stable-sorting-algorithm-implementation-in-javascript/48660568#48660568
  const stableSort = (arr, compare) => arr
    .map((item, index) => ({ item, index }))
    .sort((a, b) => compare(a.item, b.item) || a.index - b.index)
    .map(({ item }) => item);
  const cmp = (a, b) => (a.records || []).length - (b.records || []).length;
  return config.selection_rules.map(({ key, count }) =>
    stableSort(config[key], cmp)
      .map(({ id }) => id)
      .slice(0, count)
  );
}

function selectPeople2(config) {
  // https://stackoverflow.com/questions/1427608/fast-stable-sorting-algorithm-implementation-in-javascript/48660568#48660568
  const stableSort = (arr, compare) => arr
    .map((item, index) => ({ item, index }))
    .sort((a, b) => compare(a.item, b.item) || a.index - b.index)
    .map(({ item }) => item);
  const cmp = (a, b) => (a.records || []).length - (b.records || []).length;

  const groups = [];
  for (const { key, count } of config.selection_rules) {
    const group = [];
    for (const { id } of stableSort(config[key], cmp)) {
      group.push(id);
    }
    groups.push(group.slice(0, count));
  }
  return groups;
}

function shouldNotify(config) {
  const day_now = new Date().getDay();
  return config.days_should_notify.some(day => day % 7 === day_now);
}

// append date to config.*_list[i].records
function getNewConfig(config) {
  const people_list = selectPeople(config);
  const date_now = dateToString(new Date());
  const isPeopleSelected = (id, selected_people) =>
    selected_people.some(sid => sid === id);
  const constructNewList = (people_in_list, selected_people) =>
    people_in_list.map(({ id, records }) =>
      Object.assign({ id, records }, isPeopleSelected(id, selected_people) ?
        { records: (records || []).concat(date_now) } : null)
    );
  const constructNewLists = (new_lists, { key }, rule_index) =>
    Object.assign(new_lists, {
      [key]: constructNewList(config[key], people_list[rule_index])
    });
  return Object.assign({}, config,
    config.selection_rules.reduce(constructNewLists, {}));
}

// [ [x], [y], [z] ] => [ x, y, z ]
function arrayToFlat(array) {
  const addElemToArr = (arr, elem) => arr.concat(elem);
  return array.reduce(addElemToArr, []);
}

// [ x, y, y ] => [ x, y ]
function arrayToUnique(array) {
  const addElemToSet = (set, elem) => Object.assign(set, { [elem]: null });
  return Object.keys(array.reduce(addElemToSet, {})).sort();
}
```

## 写在最后

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man

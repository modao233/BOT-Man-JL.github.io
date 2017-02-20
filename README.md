# BOT Frame

**BOT Frame** is a **Javascript/Markdown-based Blog Framework** for *GitHub Pages*.

## Features

- Auto rendering **relative paths** in *Markdown*.
- Loading **modules** from *Markdown*.
- Generating **links to articles** from normal relative links.

## Basic Usage

- Update some content of `assets.md`.
- Update files inside the `articles` folder, except `index.html`.
- Update `favicon.ico`, if you like.
- Upload to your *GitHub Pages* repository and Enjoy it. 😉

## Advanced Usage

If you don't like current theme, you can replace it with your new one. 😉

> The following sections are the details of current settings of mine.

### `assets.md`

This file consists of all the essential modules for rendering.

``` markdown
<homeTitleSec> Home | BOT Man JL </homeTitleSec>
<articlesTitleSec> Articles | BOT Man JL </articlesTitleSec>
<aboutTitleSec> About | BOT Man JL </aboutTitleSec>
<headerSec> ## ~$ [🏠](/) | [📝](/articles/) | [😊](/about/) _ </headerSec>
<footerSec>
  About this [Framework](https://github.com/BOT-Man-JL/BOT-Man-JL.github.io)
  BOT Man JL, 2017
</footerSec>
<contactSec> ...
<homeSec> ...
<articlesSec> ...
<aboutSec> ...
```

### Home Page

This page is described in `/index.html`.

``` html
<title class="homeTitleSec">Loading...</title>
...
<header>
  <div class="headerSec"></div>
  <div class="contactSec"></div>
</header>
<section class="markdown-body">
  <div class="promptSec"></div>
  <div class="homeSec"></div>
</section>
<footer class="footerSec"></footer>
...
<script>
RenderSectionWithPrompt("/assets.md", "promptSec",
  [
    "headerSec",
    "footerSec",
    "contactSec",
    "homeSec",
    { tagName: "homeTitleSec", isMd: false }
  ]);
</script>
```

### Articles Page

This page is described in `articles/index.html`, used for

- rendering articles (with `location.hash`),
- displaying the content of articles (without `location.hash`).

``` html
<title class="articlesTitleSec">Loading...</title>
...
<header>
  <div class="headerSec"></div>
  <div class="contactSec"></div>
  <div id="articleHeaderSec"></div>
</header>
<section class="markdown-body">
  <div class="promptSec"></div>
  <div class="articlesSec"></div>
  <div class="contentSec"></div>
</section>
<footer class="footerSec"></footer>
```

#### With `location.hash` value

The *Markdown* file name is given in `location.hash`,
and it is rendered in section `contentSec`.

``` javascript
RenderSectionWithPrompt("/assets.md", "promptSec",
  [
    "headerSec",
    "footerSec"
  ], function (isOK)
  {
    if (!isOK) return;
    RenderSectionWithPrompt(location.hash.substr(1) + ".md",
      "promptSec", "contentSec", function (isOK2)
      {
        if (!isOK2) return;
        FixLayout(document.getElementsByTagName("HEADER")[0],
          document.getElementsByClassName("contentSec")[0]);
      });
  });
```

#### Without `location.hash` value

Param `articlesPath` specifies the relative path
of articles to *articles rendering page* (aka this page).

``` javascript
RenderSectionWithPrompt("/assets.md", "promptSec",
  [
    "headerSec",
    "footerSec",
    "contactSec",
    { tagName: "articlesSec", articlesPath: "./" },
    { tagName: "articlesTitleSec", isMd: false }
  ]);
```

### About Page

This page is described in `about/index.html`.

``` html
<title class="aboutTitleSec">Loading...</title>
...
<header>
    <div class="headerSec"></div>
    <div class="contactSec"></div>
</header>
<section class="markdown-body">
    <div class="promptSec"></div>
    <div class="aboutSec"></div>
</section>
<footer class="footerSec"></footer>
...
<script>
  RenderSectionWithPrompt("/assets.md", "promptSec",
    [
      "headerSec",
      "footerSec",
      "contactSec",
      "aboutSec",
      { tagName: "aboutTitleSec", isMd: false }
    ]);
</script>
```

### `RenderSection` @ `bot-frame.js`

``` javascript
RenderSection (fileName, tags, callback);
```

- `fileName`: file to render, retrieved by `GET` method
- `tags`: `tagObj` or [`tagObj`] or `tagName` or [`tagName`]
  - If `tags` be Object(s), `tagName` be retrived by property; else by Object itself
  - If `tags` is an `Array`: For **each** `tag` in `tags`,
    Render the file content **between** `tagName` into all elements
  - If `tags` is **NOT** an `Array`:
    Render the **entire** file content into all elements
- `tagObj.tagName`: `tagName` of section to render
  - file content are rendered into all elements of `class tagName`
  - if `tags` is **NOT** an `Array`, it renders the entire content of file
- `tagObj.isMd` (Optional, default: `true`): is *Markdown* content
  - `true`: content before rendering is parsed from *Markdown*
  - `false`: content is rendered as *Plain-Text*
- `tagObj.articlesPath` (Optional): the path of *articles rendering page*
  - `null`: relative paths inside the content are corrected
  - `articlesPath`: local paths inside the content are set to `articles-rendering-Path#article`
- `callback`: called at rendering done
  - `callback(true)` if rendering successfully
  - `callback(false)` if rendering failed

### `RenderSectionWithPrompt` @ `minimal.fix.js`

``` javascript
RenderSectionWithPrompt (fileName, promptTag, tags, callback);
```

- `promptTag`: render prompt section into elements of `class promptTag`
- *other params* are same as `RenderSection`
- This function is wrapper of `RenderSection`
  - Rendering prompt of loading status
  - Set `document.title` if loading failed

### `FixLayout` @ `minimal.fix.js`

``` javascript
FixLayout (dstSec, srcSec);
```

- `srcSec` and `dstSec`: containers of article
- This function is used for *articles rendering page*
  - Move the beginning `H1` and `BLOCKQUOTE` from `srcSec` to `dstSec` (Appending)
  - Set the content of `H1` to document title

## Credits

- **Markdown Parser JS** by [chjj](https://github.com/chjj/marked)
- **Theme CSS** by [orderedlist](https://github.com/orderedlist/minimal)
- **GitHub Markdown CSS** by [sindresorhus](https://github.com/sindresorhus/github-markdown-css)
- JS Markdown Rendering **inspired** by [Super Woods](https://cjmm.github.io/profile/)
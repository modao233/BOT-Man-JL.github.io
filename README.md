# BOT Frame

**BOT Frame** is a **Javascript/Markdown-based Blog Framework** for *GitHub Pages*.

## Features

- Auto rendering **relative paths** in *Markdown*.
- Loading **modules** from *Markdown*.
- Generating **links to articles** from normal relative links.

## Basic Usage

- Update the content of `assets.md`.
- Update files inside the `articles` folder, except `index.html`.
- Update `favicon.ico`, if you like.
- Upload to your *GitHub Pages* repository and Enjoy it. 😉

## Default Layout

### `assets.md`

This file consists of all the essential modules for rendering.

``` markdown
<homeTitleSec>
  Home | BOT Man JL
</homeTitleSec>
<articlesTitleSec>
  Articles | BOT Man JL
</articlesTitleSec>
<headerSec>
  ## ~$ [🏠](/) | [📝](/articles/) _
</headerSec>
<usernameSec>
  # BOT Man | John Lee
</usernameSec>
<contactSec> ...
<archiveSec> ...
<profileSec> ...
<footerSec>
  About this [Framework](https://github.com/BOT-Man-JL/BOT-Man-JL.github.io)
  BOT Man, 2016
</footerSec>
```

### Home Page

In this page, **BOT-Frame** loads the following modules
  from `/assets.md` to the corresponding sections of `class` *tagName*:

- `headerSec`
- `footerSec`
- `usernameSec`
- `contactSec`
- `archiveSec`
- `profileSec`
- `homeTitleSec`

where

- `archiveSec` is rendered as page of links to articles, relative to `/articles/`.
- `homeTitleSec` is rendered as plain-text, rather than `Markdown`.

``` html
<title class="homeTitleSec">Loading...</title>
...
<header>
  <div class="headerSec"></div>
  <div class="usernameSec"></div>
  <div class="contactSec"></div>
</header>
<section class="markdown-body">
  <div class="promptSec"></div>
  <div class="archiveSec"></div>
  <div class="profileSec"></div>
</section>
<footer class="footerSec"></footer>
...
<script>
  RenderSectionWithPrompt("/assets.md", "promptSec",
    [
      "headerSec",
      "footerSec",
      "usernameSec",
      "contactSec",
      {
        tagName: "archiveSec",
        articlesPath: "/articles/"
      },
      "profileSec",
      {
        tagName: "homeTitleSec",
        isMd: false
      }
    ], null);
</script>
```

### Articles Page

This page is `index.html` in the folder `articles`, used for

- rendering articles (with `location.hash`),
- displaying the content of articles (without `location.hash`).

``` html
<title class="articlesTitleSec">Loading...</title>
...
<header>
  <div class="headerSec"></div>
  <div id="articleHeaderSec"></div>
  <div class="contactSec"></div>
</header>
<section class="markdown-body">
  <div class="promptSec"></div>
  <div class="archiveSec"></div>
  <div class="contentSec"></div>
</section>
<footer class="footerSec"></footer>
```

#### With `location.hash` value

In this page, **BOT-Frame** loads the following modules
  from `/assets.md` to the corresponding sections of `class` *tagName*:

- `headerSec`
- `footerSec`

Then it loads the article of path `location.hash` to `contentSec`.
At last, it fixes the layout of this page.

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
        document.title =
          FixLayout(document.getElementById("articleHeaderSec"),
            document.getElementsByClassName("contentSec")[0]);
      });
  });
```

#### Without `location.hash` value

In this page, **BOT-Frame** loads the following modules
  from `/assets.md` to the corresponding sections of `class` *tagName*:

- `headerSec`
- `footerSec`
- `contactSec`
- `archiveSec`
- `articlesTitleSec`

where

- `archiveSec` is rendered as page of links to articles, relative to current path.
- `articlesTitleSec` is rendered as plain-text, rather than `Markdown`.

Then it fixes the layout of this page.

``` javascript
RenderSectionWithPrompt("/assets.md", "promptSec",
  [
    "headerSec",
    "footerSec",
    "contactSec",
    {
      tagName: "archiveSec",
      articlesPath: "./"
    },
    {
      tagName: "articlesTitleSec",
      isMd: false
    }
  ], function (isOK)
  {
    if (!isOK) return;
    FixLayout(document.getElementById("articleHeaderSec"),
      document.getElementsByClassName("archiveSec")[0]);
  });
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
- `tagObj.tagName`: tag/class name to render
  - file content are rendered into all elements of `class tagName`
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

- `promptTag`: tag/class name to render prompt section
- *other params* are same as `RenderSection`
- This function is wrapper of `RenderSection`
  - Rendering prompt of loading states
  - Set `document.title` if loading failed

### `FixLayout` @ `minimal.fix.js`

``` javascript
FixLayout (dstSec, srcSec);
```

- `srcSec` and `dstSec`: containers of article
- This function is used for *articles rendering page*
  - Move the beginning `H1` and `BLOCKQUOTE` from `srcSec` to `dstSec`
  - Return the content of `H1` (*title of article*)

## Credits

- **Markdown Parser JS** by [chjj](https://github.com/chjj/marked)
- **Theme CSS** by [orderedlist](https://github.com/orderedlist/minimal)
- **GitHub Markdown CSS** by [sindresorhus](https://github.com/sindresorhus/github-markdown-css)
- JS Markdown Rendering **inspired** by [Super Woods](https://cjmm.github.io/profile/)
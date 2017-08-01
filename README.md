# BOT Frame

**BOT Frame** is a **Markdown-based Blog Framework** for *GitHub Pages*, based on **BOT Mark**.

## Features

- pure **JavaScript**, do everything in your browser
- **responsive** page design (_desktop_, _mobile_ and _print_)
- rendering **BOT Mark** (detailed in following section)
  - `[TOC]`
  - number headings
  - LaTeX math (powered by KaTeX)
  - citation
  - cross-reference
  - slides
  - style-setters

## Get Started

- Write  `static/config.json` for your basic info
- Design `static/home.md` as your home page
- Write  `about/about-[en/zh].md` as your resume
- Replace articles in `articles/` with yours
- Write  `articles/archive` as the content table of your articles
- Update `favicon.ico` as your icon
- Upload to your _GitHub Pages repository_
- Enjoy it 😉

## BOT Frame

### [static/config.json](static/config.json)

Set `name`, `github`, `email`, (`phone`), `pages` (navigation), `footer` to yours.

``` json
{
  "name": "name_in_title",
  "contact": {
    "name": "name_in_contact_pane",
    "github": {
      "text": "xxx",
      "link": "https://github.com/xxx",
      "img": "/static/github.svg"
    },
    "email": {
      "text": "xxx@xxx.com",
      "link": "mailto:xxx@xxx.com",
      "img": "/static/email.svg"
    },
    "phone": {
      "text": "+86-xxxx",
      "link": "tel:+86-xxx",
      "img": "/static/phone.svg"
    }
  },
  "pages": {
    "🏠": "/",
    "📝": "/articles/",
    "😊": "/about/"
  },
  "footer": [
    "<a href='https://xxx.github.io'>https://xxx.github.io</a>",
    "xxx, 2017"
  ]
}
```

### [articles/archive.md](articles/archive.md)

Add links to your articles without `.md` extension.

``` markdown
- [file_in_articles_folder](xxx)
- [file_in_articles_2017_folder](2017/xxx)
```

### articles/xxx.md

- BOT Frame will resolve the correct path to **relative _links_ and _images_** for each article. If a link or image works/shows correctly in your markdown editor, it can work/show on your blog as well.
- BOT Frame will **move first two elements** (recommending a heading and a quoteblock) to left pane, and remove existing contact section.

## BOT Mark

> BOT Mark is a BOT Man flavored Markdown renderer.

### Specs

Original markdown spec follows [chjj `marked`](https://github.com/chjj/marked), here we list the additional specs of _BOT Mark_ below.

- heading numbering setting
  - `<p>[heading-numbering]</p>` => auto numbering headings
- heading setting
  - `[no-number]` => no number for this heading
  - `[no-toc]` => not add this heading to TOC
- TOC
  - `<p>[TOC]</p>` => TOC list
- citation
  - `[...]:` (note) => `^^^ [i]`
  - `[...]` (reference) => `[i]` (superscript)
- cross-reference
  - `<p>[...||...]</p>` (target) => invisible anchor
  - `[...|...]` (item) => `i` (target index)
  - `[sec|...]` (item) => `§ NUMBER/TEXT` (section)
- slide
  - `---` / `<hr>` / `</hr>` => slide wrapper
  - slide wrapper would seperate article into slides
- keyword
  - set next element's style (excluding keywords / style setters)
  - `<p>[page-break]</p>` => page break when print
  - `<p>[float-left]</p>`, `<p>[float-right]</p>` => float
  - `<p>[align-left]</p>`, `<p>[align-right]</p>`, `<p>[align-center]</p>` => alignment
  - `cite-sec` => citation style
- style setter
  - `<p>[TAG=STYLE]</p>`
  - set next TAG's (if present) style to STYLE

### Renderer

> BOT Mark is rendered by [MarkdownRenderer](javascripts/bot-mark.js).

#### `MarkdownRenderer.prototype.anchor`

- `function (text) => anchor`
- encode raw `text` to `anchor` format

#### `MarkdownRenderer.prototype.heading`

- `function (level, headingNumber, anchor, text) => html`
- render heading to `html`

#### `MarkdownRenderer.prototype.toc`

- `function (level, headingNumber, anchor, text) => html`
- render TOC item to `html`

#### `MarkdownRenderer.prototype.tocTags`

- `['<div class="markdown-toc">', '</div>']`
- specify surrounding tag pair of TOC

#### `MarkdownRenderer.prototype.renderToc`

- `function (mdHtml, tocArray) => mdHtml`
- render `<p>[TOC]</p>` with `tocArray`
- Note that it will omit first item by default

#### `MarkdownRenderer.prototype.mathTags`

- `[ ['$', '$'], ['\\(', '\\)'], ['\\[', '\\]'], ['$$', '$$'], 'beginend' ]`
- specify LaTeX math tag pairs
- https://github.com/ViktorQvarfordt/marked

#### `MarkdownRenderer.prototype.citeNote`

- `function (anchor, noteIndex) => html`
- render cite note into `html`

#### `MarkdownRenderer.prototype.citeRef`

- `function (anchor, noteIndex, refIndex) => html`
- render cite reference into `html`, binding with cite note

#### `MarkdownRenderer.prototype.citeDeref`

- `function (anchor, refIndex) => html`
- render cite de-reference into `html`, binding with corresponding cite note and cite reference

#### `MarkdownRenderer.prototype.renderCitation`

- `function (mdHtml) => mdHtml`
- render `[...]:` as cite notes
- render `[...]` as cite references
- add de-references for each note

#### `MarkdownRenderer.prototype.refTarget`

- `function (type, value) => html`
- render reference `value` with its `type` into target `html`

#### `MarkdownRenderer.prototype.refItem`

- `function (type, value, index) => html`
- render reference item into `html`, binding with its target

#### `MarkdownRenderer.prototype.refSection`

- `function (anchor, text) => html`
- render reference item into `html`, binding with its section heading

#### `MarkdownRenderer.prototype.renderReference`

- `function (mdHtml, tocArray) => html`
- render `<p>[...||...]</p>` as reference targets
- render `[...|...]` as reference items
- render `[sec|...]` as spec for section heading
- detect invalid reference items

#### `MarkdownRenderer.prototype.slideTags`

- `['<div class="markdown-slide">', '</div>']`
- specify surrounding tag pair of slides

#### `MarkdownRenderer.prototype.renderSlides`

- `function (mdHtml) => mdHtml`
- render `<hr>` tags into slide wrappers

#### `MarkdownRenderer.prototype.keywordTags`

``` json
[
    'page-break', 'float-left', 'float-right',
    'align-left', 'align-right', 'align-center',
    'cite-sec'
]
```

- specify keyword tags
- always use for control the style of next element

#### `MarkdownRenderer.prototype.renderKeywordTags`

- `function (mdHtml) => mdHtml`
- render `<p>[KEYWORD]</p>` into `<div class="KEYWORD"></div>`

#### `MarkdownRenderer.prototype.renderStyleSetters`

- `function (mdHtml) => mdHtml`
- detect `<p>[...=...]</p>` as style setters (TAG = STYLE)
- add STYLE to the next TAG

#### `MarkdownRenderer.prototype.render`

- `function (mdText) => mdHtml`
- rendering pipeline
  - `marked`
  - `renderToc`
  - `renderCitation`
  - `renderReference`
  - `renderSlides`
  - `renderKeywordTags`
  - `renderStyleSetters`

## Credits

- **Markdown Parser JS** by
  - [chjj](https://github.com/chjj/marked) - orginal
  - [ViktorQvarfordt](https://github.com/ViktorQvarfordt/marked) - patch
  - [BOT Man](javascripts/bot-mark.js) - flavor
- **GitHub Markdown CSS** by [sindresorhus](https://github.com/sindresorhus/github-markdown-css)

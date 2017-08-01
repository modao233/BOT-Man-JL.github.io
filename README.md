# BOT Frame

**BOT Frame** is a **Markdown-based Blog Framework** for *GitHub Pages*, based on **BOT Mark**.

## Features

- pure **JavaScript**, do everything in your browser
- **responsive** page design (_desktop_, _mobile_ and _print_)
- rendering **BOT Mark**
  - `[TOC]`
  - number headings
  - LaTeX math (powered by KaTeX)
  - citation
  - cross-reference
  - slide-mode
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

## Documents

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

- BOT Frame will resolve the correct path to **relative** _links_ and _images_ for each article.
- if a link or image works/shows correctly in your markdown editor, it can work/show on your blog as well.

## Credits

- **Markdown Parser JS** by
  - [chjj](https://github.com/chjj/marked) - orginal
  - [ViktorQvarfordt](https://github.com/ViktorQvarfordt/marked) - patch
  - [BOT Man](javascripts/bot-mark.js) - flavor
- **GitHub Markdown CSS** by [sindresorhus](https://github.com/sindresorhus/github-markdown-css)

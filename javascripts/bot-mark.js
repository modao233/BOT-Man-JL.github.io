/*
	BOT-Mark - a BOT Man flavored Markdown renderer :-)

    - https://github.com/BOT-Man-JL/BOT-Man-JL.github.io
    - based on marked (Christopher Jeffrey, https://github.com/chjj/marked)
    - with math patch (ViktorQvarfordt, https://github.com/ViktorQvarfordt/marked)

	BOT Man, 2017 (MIT License)
*/

if (typeof require !== 'undefined')
    var marked = require('./marked');

if (typeof marked === 'undefined') {
    var errMsg = 'marked must be loaded before BOT-Mark';
    if (console) console.error(errMsg);
    if (alert) alert(errMsg);
}

function MarkdownRenderer() { }

MarkdownRenderer.prototype._escapeRegExp = function (text) {
    return text.replace(/([.*+?^=!:${}()|\[\]\/\\])/g, '\\$1');
};

MarkdownRenderer.prototype.anchor = function (text) {
    return text.toLowerCase()
        .replace(/[^\w\u4E00-\u9FFF]+/g, '-')
        .replace(/^-+/g, '')
        .replace(/-+$/g, '');
};

MarkdownRenderer.prototype.toc = function (level, headingNumber, anchor, text) {
    return '<p style="padding-left:' + level * 1.5 + 'em"><a href="#' +
        anchor + '">' + headingNumber + text + '</a></p>';
};

MarkdownRenderer.prototype.renderToc = function (mdHtml, tocArray) {
    // omit first toc item (H1 heading)
    var minLevel = 1024;
    for (var i = 1; i < tocArray.length; i++)
        minLevel = Math.min(minLevel, tocArray[i].level);

    var tocHtml = '<div class="markdown-toc">';
    for (var i = 1; i < tocArray.length; i++) {
        var item = tocArray[i];
        tocHtml += this.toc(item.level - minLevel,
            item.headingNumber, item.anchor, item.text);
    }
    tocHtml += '</div>';

    return mdHtml.replace(/<p>\[TOC\]<\/p>/g, tocHtml);
};

MarkdownRenderer.prototype.citeNote = function (anchor, noteIndex) {
    return '<span class="cite-note" id="cite-note-' +
        anchor + '">[' + noteIndex + ']</a></span>';
};

MarkdownRenderer.prototype.citeRef = function (anchor, noteIndex, refIndex) {
    return '<span class="cite-ref" id="cite-ref-' + anchor + '-' + refIndex +
        '"><a href="#cite-note-' + anchor + '">[' + noteIndex + ']</a></span>';
};

MarkdownRenderer.prototype.citeDeref = function (anchor, refIndex) {
    return '<span class="cite-deref"><a href="#cite-ref-' +
        anchor + '-' + refIndex + '">^</a></span>';
};

MarkdownRenderer.prototype.renderCitation = function (mdHtml) {
    var that = this;
    var citeTags = [];

    // Set Note
    var noteIndex = 0;
    mdHtml = mdHtml.replace(/\[.*\]:/g, function (text) {
        citeTags.push(text.substr(0, text.length - 1));
        var anchor = that.anchor(text.substr(1, text.length - 3));
        return that.citeNote(anchor, ++noteIndex);
    });

    for (var i = 0; i < citeTags.length; i++) {
        var anchor = this.anchor(citeTags[i].substr(1, citeTags[i].length - 2));
        var noteIndex = i + 1;

        // Set Ref
        var refIndex = 0;
        var derefHTML = ' ';  // Use RegExp to construct global replacement
        mdHtml = mdHtml.replace(new RegExp(that._escapeRegExp(citeTags[i]), 'g'), function () {
            derefHTML += that.citeDeref(anchor, ++refIndex) + ' ';
            return that.citeRef(anchor, noteIndex, refIndex);
        });

        // Set Deref
        var noteStr = this.citeNote(anchor, noteIndex);
        var indexToInsert = mdHtml.indexOf(noteStr) + noteStr.length;
        mdHtml = mdHtml.substr(0, indexToInsert) + derefHTML + mdHtml.substr(indexToInsert);
    }

    return mdHtml;
};

MarkdownRenderer.prototype.refTarget = function (type, value) {
    return '<div class="ref-target" id="ref-' +
        type + '-' + value + '"></div>';
};

MarkdownRenderer.prototype.refEntry = function (type, value, index) {
    return '<span class="ref-item"><a href="#ref-' +
        type + '-' + value + '">' + index + '</a></span>';
};

MarkdownRenderer.prototype.refSection = function (anchor, headingNumber) {
    return '<span class="ref-item"><a href="#' +
        anchor + '">&sect; ' + headingNumber + '</a></span>';
};

MarkdownRenderer.prototype.renderReference = function (mdHtml, tocArray) {
    var that = this;
    var indexMap = new Map();
    var countRefs = new Map();

    // Ref target
    var len1 = '<p>['.length;
    var len2 = ']</p>'.length + len1;
    mdHtml = mdHtml.replace(/<p>\[[^\].]+\|\|[^\].]+\]<\/p>/g, function (text) {
        text = text.substr(len1, text.length - len2);
        var fragments = text.split('||');
        fragments[0] = that.anchor(fragments[0]);

        var count = countRefs.get(fragments[0]);
        count = (count == null ? 0 : count) + 1;
        countRefs.set(fragments[0], count);

        fragments[1] = that.anchor(fragments[1]);
        indexMap.set(fragments.join('-'), count);

        return that.refTarget(fragments[0], fragments[1]);
    });

    // Ref entry (or section)
    mdHtml = mdHtml.replace(/\[[^\].]+\|[^\].]+\]/g, function (text) {
        var fragments = text.substr(1, text.length - 2).split('|');
        fragments[0] = that.anchor(fragments[0]);

        if (fragments[0] == 'sec') {
            for (var i = 0; i < tocArray.length; i++)
                if (tocArray[i].text == fragments[1]) {
                    return that.refSection(tocArray[i].anchor,
                        tocArray[i].headingNumber);
                }
        }
        else {
            fragments[1] = that.anchor(fragments[1]);
            var index = indexMap.get(fragments.join('-'));
            if (index != null) {
                return that.refEntry(fragments[0], fragments[1], index);
            }
        }

        var errMsg = 'Invalid ref text: ' + text;
        if (console) console.error(errMsg);
        if (alert) alert(errMsg);
        return text;
    });

    return mdHtml;
};

MarkdownRenderer.prototype.slideTags = [
    '<div class="slide">',
    '</div>'
];

MarkdownRenderer.prototype.renderSlides = function (mdHtml) {
    var that = this;
    var matchHr = mdHtml.match(/<hr>/g);
    if (matchHr == null) return mdHtml;

    var count = 0;
    mdHtml = mdHtml.replace(/<hr>/g, function () {
        ++count;
        var ret = '';

        if (count != 1) ret += that.slideTags[1];
        if (count != matchHr.length) ret += that.slideTags[0];

        return ret;
    });

    return mdHtml;
};

MarkdownRenderer.prototype.keywordTags = [
    'page-break', 'float-left', 'float-right',
    'align-left', 'align-right', 'align-center',
    'cite-sec'
];

MarkdownRenderer.prototype.renderKeywordTags = function (mdHtml) {
    if (this.keywordTags.lenght == 0) return mdHtml;

    var condStr = this.keywordTags[0];
    for (var i = 1; i < this.keywordTags.length; i++)
        condStr += '|' + this.keywordTags[i];

    var regExp = new RegExp('<p>\\[(' + condStr + ')\\]<\\/p>', 'g');
    return mdHtml.replace(regExp, '<div class="$1"></div>');
};

MarkdownRenderer.prototype.renderStyleSetters = function (mdHtml) {
    var len1 = '<p>['.length;
    var len2 = ']</p>'.length + len1;

    var styleSetters = mdHtml.match(/<p>\[[^\].]+=[^\].]+\]<\/p>/g);  // avoid ']' inside pairs
    if (styleSetters == null) styleSetters = [];

    for (var j = 0; j < styleSetters.length; j++) {
        var styleSetter = styleSetters[j];
        var tagPrefix = '<' + styleSetter.substr(len1,
            styleSetter.indexOf('=') - len1);
        var tagStyle = styleSetter.substr(1 + styleSetter.indexOf('='),
            styleSetter.length - len2 - tagPrefix.length);
        alert(tagPrefix);
        alert(tagStyle);

        var firstTagIndex = mdHtml.indexOf(styleSetter) + styleSetter.length;

        // Avoid prefix matches
        while (true) {
            firstTagIndex = mdHtml.indexOf(tagPrefix, firstTagIndex);
            if (firstTagIndex == -1) break;
            if (mdHtml[firstTagIndex + tagPrefix.length] == ' ' ||
                mdHtml[firstTagIndex + tagPrefix.length] == '>') {
                break;
            }
            else {
                firstTagIndex += tagPrefix.length;  // find next
            }
        }

        if (firstTagIndex == -1) continue;  // not found
        mdHtml = mdHtml.substr(0, firstTagIndex + tagPrefix.length).replace(styleSetter, '') +
            ' style="' + tagStyle + '"' + mdHtml.substr(firstTagIndex + tagPrefix.length);
    }
    return mdHtml;
};

MarkdownRenderer.prototype.render = function (mdText) {
    var that = this;

    // Check heading numbering
    var headingIndice = null;
    var headingNumberingRegExp = /(\r)?\n\[heading\-numbering\](\r)?\n(\r)?\n/g;
    if (mdText.search(headingNumberingRegExp) != -1)
        headingIndice = [0, 0, 0, 0, 0, 0];
    mdText = mdText.replace(headingNumberingRegExp, '\n');

    // Patch for TOC support
    // Ref: https://github.com/chjj/marked/issues/545
    var renderer = new marked.Renderer();
    var tocArray = [];
    var anchorMap = new Map();
    renderer.heading = function (text, level, raw) {

        var noNumber = raw.indexOf('[no-number]') != -1;
        var noToc = raw.indexOf('[no-toc]') != -1;

        text = text.replace(/[\s]*\[.+\][\s]*/g, '');
        raw = raw.replace(/[\s]*\[.+\][\s]*/g, '');

        // Set anchor
        var anchor = that.anchor(raw);
        var count = anchorMap.get(anchor);
        count = (count == null ? 0 : count) + 1;
        anchorMap.set(anchor, count);
        if (count > 1) anchor += '_' + count;

        // Add heading numbers
        var headingNumber = '';
        if (headingIndice != null && !noNumber) {
            var index = level - 1;
            if (index < headingIndice.length && index != 0) {

                // Generate number text
                ++headingIndice[index];
                headingNumber += headingIndice[1];
                for (var i = 2; i <= index; i++)
                    headingNumber += '.' + headingIndice[i];

                // Clear lower indice
                for (var i = index + 1; i < headingIndice.length; i++)
                    headingIndice[i] = 0;
            }
            headingNumber += ' ';
        }

        // Add to TOC
        if (!noToc)
            tocArray.push({
                text: text, level: level, anchor: anchor,
                headingNumber: headingNumber
            });

        return '<h' + level + ' id="' + anchor + '">' +
            headingNumber + text + '</h' + level + '>\n';
    };

    // Patch for math support
    // Ref: https://github.com/ViktorQvarfordt/marked

    var mdHtml = marked(mdText, {
        renderer: renderer,
        mathDelimiters: [
            ['$', '$'], ['\\(', '\\)'], ['\\[', '\\]'], ['$$', '$$'], 'beginend'
        ]
    });

    mdHtml = this.renderToc(mdHtml, tocArray);
    mdHtml = this.renderCitation(mdHtml);
    mdHtml = this.renderReference(mdHtml, tocArray);
    mdHtml = this.renderSlides(mdHtml);
    mdHtml = this.renderKeywordTags(mdHtml);
    mdHtml = this.renderStyleSetters(mdHtml);
    return mdHtml;
};

MarkdownRenderer.prototype.marked = function (mdText) {
    return marked(mdText);
};

if (typeof module !== 'undefined' && typeof exports === 'object') {
    module.exports = MarkdownRenderer;
}

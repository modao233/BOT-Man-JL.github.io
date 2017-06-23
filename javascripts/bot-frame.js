/*
	Modules for GitHub Page :-)
	BOT Man JL, 2016 (MIT License)

	Note that: Remenber to Load "marked.js" Before this
*/;

Ajax = function (method, path, data, callback) {
    if (window.XMLHttpRequest)
        // code for IE7+, Firefox, Chrome, Opera, Safari
        var xmlhttp = new XMLHttpRequest();
    else
        // code for IE6, IE5
        var xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");

    xmlhttp.open(method, path, true);
    xmlhttp.onreadystatechange = function () {
        if (xmlhttp.readyState != 4)
            return;

        if (xmlhttp.status == 200)
            callback(xmlhttp.responseText);
        else
            callback(null);
    };

    var content = data;
    if (data != null && data.header != null) {
        for (var i = 0; i < data.header.length; i++) {
            var field = data.header[i];
            if (field.length >= 2)
                xmlhttp.setRequestHeader(field[0], field[1]);
        }
        content = data.content;
    }
    xmlhttp.send(content);
};

RenderSection = function (fileName, tags, callback) {

    var EscapeRegExp = function (str) {
        return str.replace(/([.*+?^=!:${}()|\[\]\/\\])/g, "\\$1");
    };

    var RenderFlavoredMarkdown = function (mdText) {
        var FixAnchorText = function (text) {
            return text.toLowerCase()
                .replace(/[^\w\u4E00-\u9FFF]+/g, '-')
                .replace(/^-+/g, '').replace(/-+$/g, '');
        };

        // Patch for Heading Numbering
        var headingIndice = null;
        {
            var headingNumberingRegExp = /(\r)?\n\[heading\-numbering\](\r)?\n(\r)?\n/g;
            if (mdText.search(headingNumberingRegExp) != -1)
                headingIndice = [0, 0, 0, 0, 0, 0];
            mdText = mdText.replace(headingNumberingRegExp, '\n');
        }

        // Patch for [TOC]
        var GetTOC = function (tocArray) {
            var tocHTML = "";

            var minLevel = 1024;
            for (var i = 1; i < tocArray.length; i++)
                minLevel = Math.min(minLevel, tocArray[i].level);

            for (var i = 1; i < tocArray.length; i++) {
                var item = tocArray[i];
                tocHTML += "<p style='padding-left:" +
                    (item.level - minLevel) * 1.5 + "em'><a href='#" +
                    item.anchor + "'>" + item.headingNumber +
                    item.text + "</a></p>";
            }
            return tocHTML;
        };

        // Ref: https://github.com/chjj/marked/issues/545
        var toc = [];
        var renderer = new marked.Renderer();
        var anchorMap = new Map();
        renderer.heading = function (text, level, raw) {

            var noNumber = raw.indexOf('[no-number]') != -1;
            var noToc = raw.indexOf('[no-toc]') != -1;
            var tocHeading = raw.indexOf('[toc-heading]') != -1;

            text = text.replace(/[\s]*\[.+\][\s]*/g, '');
            raw = raw.replace(/[\s]*\[.+\][\s]*/g, '');

            // Set anchor
            var anchor = FixAnchorText(raw);
            var count = anchorMap.get(anchor);
            count = (count == null ? 0 : count) + 1;
            anchorMap.set(anchor, count);
            if (count > 1) anchor += "_" + count;

            // Add heading numbers
            var headingNumber = "";
            if (headingIndice != null && !noNumber) {
                var index = level - 1;
                if (index < headingIndice.length && index != 0) {

                    // Generate number text
                    ++headingIndice[index];
                    headingNumber += headingIndice[1];
                    for (var i = 2; i <= index; i++)
                        headingNumber += "." + headingIndice[i];

                    // Clear lower indice
                    for (var i = index + 1; i < headingIndice.length; i++)
                        headingIndice[i] = 0;
                }
                headingNumber += " ";
            }

            // Add to TOC
            if (!noToc && !tocHeading)
                toc.push({
                    text: text, level: level,
                    anchor: anchor, headingNumber: headingNumber
                });

            return '<h' + level +
                (tocHeading ? ' class="toc-heading"' : '') +
                ' id="' + anchor + '">' + headingNumber + text +
                '</h' + level + '>\n';
        };

        // Patch for Math Support
        // Ref: https://github.com/ViktorQvarfordt/marked
        marked.setOptions({
            renderer: renderer,
            mathDelimiters: [
                ['$', '$'], ['\\(', '\\)'], ['\\[', '\\]'], ['$$', '$$'], 'beginend'
            ]
        });
        var mdHtml = marked(mdText);

        // Render citation
        {
            var getCiteNoteHTML = function (citeContent, noteIndex) {
                return '<span class="cite-note" id="cite-note-' + citeContent +
                    '">[' + noteIndex + ']</a></span>';
            };
            var getCiteRefHTML = function (citeContent, noteIndex, refIndex) {
                return '<span class="cite-ref" id="cite-ref-' + citeContent + '-' + refIndex +
                    '"><a href="#cite-note-' + citeContent + '">[' + noteIndex + ']</a></span>';
            };
            var getCiteDerefHTML = function (citeContent, refIndex) {
                return '<span class="cite-deref"><a href="#cite-ref-' +
                    citeContent + '-' + refIndex + '">^</a></span>';
            };

            var cites = mdHtml.match(/\[.*\]:/g);
            var countCites = 0;
            mdHtml = mdHtml.replace(/\[.*\]:/g, function (refText) {
                ++countCites;
                var citeContent = FixAnchorText(refText.substr(1, refText.length - 3));
                return getCiteNoteHTML(citeContent, countCites);
            });

            if (cites == null) cites = [];
            for (var i = 0; i < cites.length; i++) {
                cites[i] = cites[i].substr(0, cites[i].length - 1);
                var citeIndex = i + 1;
                var citeContent = FixAnchorText(cites[i].substr(1, cites[i].length - 2));

                var countRefs = 0;
                mdHtml = mdHtml.replace(new RegExp(EscapeRegExp(cites[i]), 'g'), function (refText) {
                    ++countRefs;
                    return getCiteRefHTML(citeContent, citeIndex, countRefs);
                });

                var derefHTML = " ";
                for (var j = 0; j < countRefs; j++) {
                    derefHTML += getCiteDerefHTML(citeContent, j + 1) + " ";
                }
                var noteStr = getCiteNoteHTML(citeContent, citeIndex);
                var indexToInsert = mdHtml.indexOf(noteStr) + noteStr.length;
                mdHtml = mdHtml.substr(0, indexToInsert) + derefHTML + mdHtml.substr(indexToInsert);
            }
        }

        // Render reference
        {
            var refMap = new Map();

            // Ref target
            mdHtml = mdHtml.replace(/<p>\[[^\].]+\|\&[^\].]+\]<\/p>/g, function (refText) {
                var len1 = "<p>[".length;
                var len2 = "]</p>".length + len1;
                refText = refText.substr(len1, refText.length - len2);
                var fragments = refText.split('|&amp;');
                return "<div class='ref-target' id='ref-" +
                    FixAnchorText(fragments.join('-')) + "'></div>";
            });

            // Ref base
            var countRefs = new Map();
            mdHtml = mdHtml.replace(/\[[^\].]+\|\|[^\].]+\]/g, function (refText) {
                refText = refText.substr(1, refText.length - 2);
                var fragments = refText.split('||');

                var count = countRefs.get(fragments[0]);
                count = (count == null ? 0 : count) + 1;
                countRefs.set(fragments[0], count);
                refMap.set(FixAnchorText(fragments.join('-')), count);

                return "<span class='ref-base'>" + count + "</span>";
            });

            // Ref item
            mdHtml = mdHtml.replace(/\[[^\].]+\|[^\].]+\]/g, function (refText) {
                var refTextNew = refText.substr(1, refText.length - 2);
                var fragments = refTextNew.split('|');

                var refAnchor = "";
                if (fragments[0] == "sec") {
                    var tocIndex = 0;
                    for (; tocIndex < toc.length; tocIndex++)
                        if (toc[tocIndex].text == fragments[1]) {
                            refAnchor = toc[tocIndex].anchor;
                            refText = "&sect; " + toc[tocIndex].headingNumber;
                            break;
                        }
                    if (tocIndex == toc.length) {
                        alert("Invalid refText: " + refText);
                        return refText;
                    }
                }
                else {
                    refAnchor = FixAnchorText(fragments.join('-'));
                    var count = refMap.get(refAnchor);
                    if (count == null) {
                        alert("Invalid refText: " + refText);
                        return refText;
                    }

                    refAnchor = "ref-" + refAnchor;
                    refText = count;
                }
                return "<span class='ref-item'><a href='#" +
                    refAnchor + "'>" + refText + "</a></span>";
            });
        }

        // Render style setters
        {
            mdHtml = mdHtml.replace(/<p>(\[[^\].]+=[^\].]+\])<\/p>/g, '$1');
            var styleSetters = mdHtml.match(/\[[^\].]+=[^\].]+\]/g);  // avoid ']' inside pairs
            if (styleSetters == null) styleSetters = [];
            for (var j = 0; j < styleSetters.length; j++) {
                var styleSetter = styleSetters[j];
                var tagPrefix = "<" + styleSetter.substr(1,
                    styleSetter.indexOf('=') - 1);
                var tagStyle = styleSetter.substr(1 + tagPrefix.length,
                    styleSetter.length - 2 - tagPrefix.length);

                var firstTagIndex = mdHtml.indexOf(styleSetter) + styleSetter.length;

                // Avoid prefix matches
                while (true) {
                    firstTagIndex = mdHtml.indexOf(tagPrefix, firstTagIndex);
                    if (firstTagIndex == -1) break;
                    if (mdHtml[firstTagIndex + tagPrefix.length] == " " ||
                        mdHtml[firstTagIndex + tagPrefix.length] == ">") {
                        break;
                    }
                    else {
                        firstTagIndex += tagPrefix.length;
                    }
                    console.log(mdHtml.substr(firstTagIndex));
                }

                if (firstTagIndex == -1) continue;
                mdHtml = mdHtml.substr(0, firstTagIndex + tagPrefix.length)
                    .replace(styleSetter, '') +
                    " style='" + tagStyle + "'" +
                    mdHtml.substr(firstTagIndex + tagPrefix.length);
            }
        }

        // Render [TOC]
        {
            if (toc.length > 0) {
                var tocHTML = "<div class='markdown-toc'>" +
                    GetTOC(toc) + "</div>";
                mdHtml = mdHtml.replace(/<p>\[TOC\]<\/p>/g, tocHTML);
            }
        }

        // Render predefined style tags
        {
            var predefinedTags = [
                "page-break", "float-left", "float-right",
                "align-left", "align-right", "align-center",
                "cite-sec"
            ];
            var condStr = predefinedTags[0];
            for (var i = 1; i < predefinedTags.length; i++)
                condStr += "|" + EscapeRegExp(predefinedTags[i]);
            var regExp = new RegExp("<p>\\[(" + condStr + ")\\]<\\/p>", "g");
            mdHtml = mdHtml.replace(regExp, "<div class='$1'></div>");
        }

        // Render slides
        {
            var targetStrs = [
                "<hr><div class='slide'>",
                "</div><hr><div class='slide'>",
                "</div><hr>"
            ];
            var newContent = "";
            var curIndex = mdHtml.indexOf("<hr>"), preIndex = 0;
            var step = "<hr>".length;
            while (true) {
                if (curIndex != -1)
                    newContent += mdHtml.substr(preIndex, curIndex - preIndex);
                var nxtIndex = mdHtml.indexOf("<hr>", curIndex + step);
                if (nxtIndex != -1) {
                    if (preIndex == 0)
                        newContent += targetStrs[0];
                    else
                        newContent += targetStrs[1];
                }
                else {
                    if (preIndex == 0)
                        newContent += "<hr>";
                    else
                        newContent += targetStrs[2];
                    newContent += mdHtml.substr(curIndex + step);
                    break;
                }
                preIndex = curIndex + step;
                curIndex = nxtIndex;
            }
            if (curIndex != -1)
                mdHtml = newContent;
        }

        return mdHtml;
    };

    var GetAbsPath = function (fileName) {
        // Trick: set and then get 'href' to find the absolute path of fileName
        // http://stackoverflow.com/questions/2639070/get-the-full-uri-from-the-href-property-of-a-link
        var aElem = document.createElement("A");
        aElem.href = fileName;

        // NOT Same Origin File
        if (aElem.href.indexOf(location.origin) == -1)
            return null;

        // Get absPath (including ending '/')
        return aElem.href.substr(0, aElem.href.lastIndexOf("/") + 1);
    };

    var GetRelPath = function (basePath, destPath) {
        var commonLen = 0;
        for (; commonLen < basePath.length || commonLen < destPath.length; commonLen++)
            if (basePath[commonLen] != destPath[commonLen])
                break;

        for (; commonLen > 0; commonLen--)
            if (basePath[commonLen - 1] == '/')
                break;

        // Case: Contains Relationship
        if (commonLen >= basePath.length || commonLen >= destPath.length)
            commonLen = Math.min(basePath.length, destPath.length);

        var parents = basePath.substring(commonLen)
            .replace(/[^\/]*$/, '').replace(/.*?\//g, '../');

        return (parents + destPath.substring(commonLen)) || './';
    };

    // Another approach: <base href="" />
    var FixRelativePaths = function (mdSec, mdAbsPath, fixStyle) {
        // Trick: use reflection 'attrName' to access property
        // http://stackoverflow.com/questions/4244896/dynamically-access-object-property-using-variable
        var fixAll = function (tagName, attrName, fixStyle) {
            var elems = mdSec.getElementsByTagName(tagName);
            for (var j = 0; j < elems.length; j++) {
                // Store original literal of attr
                var rawSrc = elems[j].getAttribute(attrName);
                var src = elems[j][attrName];

                if (src.indexOf(location.origin) != -1 &&  // Same Origin
                    rawSrc.indexOf(location.origin) == -1 &&  // Relative Path
                    rawSrc[0] != "/" &&  // NOT Root-Relative Path
                    rawSrc[0] != "#")  // NOT Hash
                {
                    // Get absPath
                    var aElem = document.createElement("A");
                    aElem.href = mdAbsPath + rawSrc;
                    var absPath = aElem.href;

                    // Get relPath
                    var relPath = GetRelPath(location.origin + location.pathname, absPath);

                    // Set with pattern
                    elems[j][attrName] = fixStyle(absPath, relPath);
                }
            }
        };

        fixAll("A", "href", fixStyle);
        fixAll("IMG", "src", function (absPath, relPath) { return absPath; });
    };

    var GetSection = function (mdSource, keyword) {
        var regex = new RegExp("<" + EscapeRegExp(keyword) + ">(.|\n|\r)*?</" + EscapeRegExp(keyword) + ">", "m");
        var ret = regex.exec(mdSource);
        if (ret != null)
            return ret[0].replace("<" + keyword + ">", "").replace("</" + keyword + ">", "");
        else
            return "";  // Ignore Not Found Sections
    };

    Ajax("GET", encodeURI(fileName), null,
        function (mdSource) {
            if (mdSource == null) {
                callback(false);
                return;
            }

            var mdAbsPath = GetAbsPath(fileName);

            // tags NOT Array = rendered the entire file
            var isSingleFile = !Array.isArray(tags);
            if (isSingleFile)
                tags = [tags];

            for (var i = 0; i < tags.length; i++) {
                var tag = tags[i];

                var tagName = tag;
                if (tag.tagName != null)
                    tagName = tag.tagName;

                var isMd = true;
                if (tag.isMd != null)
                    isMd = tag.isMd;

                var fixStyle = function (absPath, relPath) { return absPath; };
                if (tag.fixStyle != null)
                    fixStyle = tag.fixStyle;

                var content = isSingleFile ? mdSource : GetSection(mdSource, tagName);
                if (isMd) {
                    if (isSingleFile) content = RenderFlavoredMarkdown(content);
                    else content = marked(content);
                }

                // Fill content and Fix paths
                var secs = document.getElementsByClassName(tagName);
                for (var j = 0; j < secs.length; j++) {
                    secs[j].innerHTML = content;

                    if (mdAbsPath != null)
                        FixRelativePaths(secs[j], mdAbsPath, fixStyle);
                }
            }
            callback(true);
        });
};

RenderSectionWithPrompt = function (fileName, promptTag, tags, callback) {
    var promptSecs = document.getElementsByClassName(promptTag);
    for (var i = 0; i < promptSecs.length; i++)
        promptSecs[i].innerHTML = "Loading <em>" + fileName + "</em> 😇";

    RenderSection(fileName, tags, function (isOK) {
        for (var i = 0; i < promptSecs.length; i++)
            promptSecs[i].innerHTML = isOK ? "" :
                "<p>Loading <a href='" + fileName + "'><em>" +
                fileName + "</em></a> <strong>Failed</strong> 😅</p>" +
                "<p>👉👉👉 <a href='javascript:void(0)' onclick='location.reload();'>" +
                "Click to Refresh</a> 👈👈👈</p>";
        if (!isOK) document.title = "Loading Failed";

        if (callback != null && isOK) callback();
    });
};

// Move First Element from 'srcSec to 'dstSec, and return innerHTML
MoveFirstElement = function (dstSec, srcSec) {
    var childLen = srcSec.childNodes.length;
    var curElem = 0;

    // Find First Element
    while (curElem < childLen &&
        srcSec.childNodes[curElem].nodeType != 1)
        curElem++;

    content = null;
    if (curElem < childLen) {
        content = srcSec.childNodes[curElem].innerHTML;
        dstSec.innerHTML +=
            "<" + srcSec.childNodes[curElem].tagName + ">" +
            content +
            "</" + srcSec.childNodes[curElem].tagName + ">";
        srcSec.removeChild(srcSec.childNodes[curElem]);
    }
    return content;
};

// Loading Issues from GitHub
LoadIssues = function (userRepoName, callback) {
    Ajax("GET", "https://api.github.com/repos/" + userRepoName + "/issues", null,
        function (jsonText) {
            if (jsonText == null)
                return callback(null);

            json = JSON.parse(jsonText);
            var issuesHTML = "";
            for (var i = 0; i < json.length; i++) {
                var issue = json[i];
                // comment-avatar
                issuesHTML += " <a href='" + issue.user.html_url +
                    "'><img class='comment-avatar' src='" + issue.user.avatar_url +
                    "' /></a>";

                // comment-box
                issuesHTML += "<div class='comment-box'>";

                // > comment-box-header
                issuesHTML += "<div class='comment-box-header'>" +
                    issue.user.login + " • " + issue.title +
                    "<span class='comment-box-timestamp'>" +
                    issue.updated_at +
                    "</span>" + "</div>";
                // > comment-box-content
                issuesHTML += "<div class='comment-box-content'>" +
                    issue.body + "</div>";

                // (comment-box)
                issuesHTML += "</div>";
            }
            return callback(issuesHTML);
        });
};

GetSearchParam = function (searchStr, target) {
    var beg = searchStr.indexOf(target);
    if (beg == -1) return null;

    beg += target.length + 1;
    var end = searchStr.indexOf('&', beg);
    if (end == -1) end = searchStr.length;

    return searchStr.substr(beg, end - beg);
};

ParseStyles = function (stylesStr, callback) {
    var styles = (stylesStr != null ? stylesStr : "").split('+');
    for (var i = 0; i < styles.length; i++) {
        callback(styles[i]);
    }
}
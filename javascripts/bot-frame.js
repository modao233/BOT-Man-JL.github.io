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

    var GetSection = function (mdSource, keyword) {
        var regex = new RegExp("<" + EscapeRegExp(keyword) + ">(.|\n|\r)*?</" + EscapeRegExp(keyword) + ">", "m");
        var ret = regex.exec(mdSource);
        if (ret != null)
            return ret[0].replace("<" + keyword + ">", "").replace("</" + keyword + ">", "");
        else
            return "";  // Ignore Not Found Sections
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

    var GetTOC = function (tocArray) {
        var tocHTML = "";

        var minLevel = 1024;
        for (var i = 1; i < tocArray.length; i++)
            minLevel = Math.min(minLevel, tocArray[i].level);

        for (var i = 1; i < tocArray.length; i++) {
            var item = tocArray[i];
            tocHTML += "<p style='padding-left:" +
                (item.level - minLevel) * 1.5 + "em'><a href='#" +
                item.anchor + "'>" +
                item.text + "</a></p>";
        }
        return tocHTML;
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

                var secText = isSingleFile ? mdSource : GetSection(mdSource, tagName);

                // Patch for [TOC]
                // Ref: https://github.com/chjj/marked/issues/545
                var toc = [];
                var renderer = new marked.Renderer();
                var anchorMap = new Map();
                renderer.heading = function (text, level, raw) {
                    var anchor = this.options.headerPrefix +
                        raw.toLowerCase().replace(/[^\w\u4E00-\u9FFF]+/g, '-')
                            .replace(/^-+/g, '').replace(/-+$/g, '');

                    // Fix duplicate ID issue
                    var count = anchorMap.get(anchor);
                    count = (count == null ? 0 : count) + 1;
                    anchorMap.set(anchor, count);
                    if (count > 1) anchor += "_" + count;

                    // Ignore 'TOC heading
                    if (anchor.replace('_', '-').search(/(toc|table-of-contents)/ig) == -1)
                        toc.push({ text: text, level: level, anchor: anchor });

                    return '<h' + level + ' id="' + anchor + '">' +
                        text + '</h' + level + '>\n';
                };

                // Patch for Math Support
                marked.setOptions({
                    renderer: renderer,
                    mathDelimiters: [
                        ['$', '$'], ['\\(', '\\)'], ['\\[', '\\]'], ['$$', '$$'], 'beginend'
                    ]
                });

                var content = isMd ? marked(secText) : secText;

                if (isMd && isSingleFile) {

                    // Render references
                    var refs = content.match(/\[.*\]:/g);
                    var refCount = 0;
                    content = content.replace(/\[.*\]:/g, function (refText) {
                        ++refCount;
                        var refHref = 'ref-' + refCount + '-' +
                            refText.substr(1, refText.length - 3).toLowerCase()
                                .replace(/[^\w\u4E00-\u9FFF]+/g, '-')
                                .replace(/^-+/g, '').replace(/-+$/g, '');
                        return '<span class="reference-base" id="' + refHref + '"><a href="#' +
                            refHref + '">[' + refCount + ']</a></span>';
                    });

                    if (refs == null) refs = [];
                    for (var i = 0; i < refs.length; i++) {
                        refCount = i + 1;
                        refs[i] = refs[i].substr(0, refs[i].length - 1);
                        var refHref = 'ref-' + refCount + '-' +
                            refs[i].substr(1, refs[i].length - 2).toLowerCase()
                                .replace(/[^\w\u4E00-\u9FFF]+/g, '-')
                                .replace(/^-+/g, '').replace(/-+$/g, '');
                        content = content.replace(new RegExp(EscapeRegExp(refs[i]), 'g'),
                            '<span class="reference-item"><a href="#' + refHref + '">[' + refCount + ']</a></span>');
                    }

                    // Render style setters
                    content = content.replace(/<p>(\[[^\].]+=[^\].]+\])<\/p>/g, '$1');
                    var styleSetters = content.match(/\[[^\].]+=[^\].]+\]/g);  // avoid ']' inside pairs
                    if (styleSetters == null) styleSetters = [];
                    for (var j = 0; j < styleSetters.length; j++) {
                        var styleSetter = styleSetters[j];
                        var tagPrefix = "<" + styleSetter.substr(1,
                            styleSetter.indexOf('=') - 1);
                        var tagStyle = styleSetter.substr(1 + tagPrefix.length,
                            styleSetter.length - 2 - tagPrefix.length);

                        var firstTagIndex = content.indexOf(tagPrefix,
                            content.indexOf(styleSetter) + styleSetter.length);
                        content = content.substr(0, firstTagIndex + tagPrefix.length)
                            .replace(styleSetter, '') +
                            " style='" + tagStyle + "'" +
                            content.substr(firstTagIndex + tagPrefix.length);
                    }

                    // Render [TOC]
                    if (toc.length > 0) {
                        var tocHTML = "<div class='markdown-toc'>" +
                            GetTOC(toc) + "</div>";
                        content = content
                            .replace(/<p>\[TOC\]<\/p>/ig, tocHTML);
                    }

                    // Render predefined-tags
                    var predefinedTags = [
                        "page-break", "float-left", "float-right",
                        "align-left", "align-right", "align-center"];
                    var condStr = predefinedTags[0];
                    for (var i = 1; i < predefinedTags.length; i++)
                        condStr += "|" + EscapeRegExp(predefinedTags[i]);
                    var regExp = new RegExp("<p>\\[(" + condStr + ")\\]<\\/p>", "g");
                    content = content.replace(regExp, "<div class='$1'></div>");

                    // Render slides
                    var targetStrs = [
                        "<hr><div class='slide'>",
                        "</div><hr><div class='slide'>",
                        "</div><hr>"
                    ];
                    var newContent = "";
                    var curIndex = content.indexOf("<hr>"), preIndex = 0;
                    var step = "<hr>".length;
                    while (true) {
                        if (curIndex != -1)
                            newContent += content.substr(preIndex, curIndex - preIndex);
                        var nxtIndex = content.indexOf("<hr>", curIndex + step);
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
                            newContent += content.substr(curIndex + step);
                            break;
                        }
                        preIndex = curIndex + step;
                        curIndex = nxtIndex;
                    }
                    if (curIndex != -1)
                        content = newContent;
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
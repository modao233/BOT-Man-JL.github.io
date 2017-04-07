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
    var GetSection = function (mdSource, keyword) {
        var regex = new RegExp("<" + keyword + ">(.|\n|\r)*?</" + keyword + ">", "m");
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

    var FixRelativePaths = function (mdSec, absPath) {
        // Trick: use reflection 'attrName' to access property
        // http://stackoverflow.com/questions/4244896/dynamically-access-object-property-using-variable
        var fixAll = function (tagName, attrName) {
            var elems = mdSec.getElementsByTagName(tagName);
            for (var j = 0; j < elems.length; j++) {
                // Store original literal of attr
                var preSrc = elems[j].getAttribute(attrName);

                if (elems[j][attrName].indexOf(location.origin) != -1 &&  // Same Origin
                    preSrc.indexOf(location.origin) == -1 &&  // Relative Path
                    preSrc[0] != "/" &&  // NOT Root-Relative Path
                    preSrc[0] != "#")  // NOT Hash
                {
                    // Relative to absPath
                    elems[j][attrName] = absPath + preSrc;
                }
            }
        };

        fixAll("A", "href");
        fixAll("IMG", "src");
    };

    var FixArticlesPaths = function (mdSec, articlesPath) {
        var tagName = "A";
        var attrName = "href";

        var elems = mdSec.getElementsByTagName(tagName);
        for (var j = 0; j < elems.length; j++) {
            // Store original literal of attr
            var preSrc = elems[j].getAttribute(attrName);

            if (elems[j][attrName].indexOf(location.origin) != -1)  // Same Origin
            {
                // Relative to articlesPath
                elems[j][attrName] = articlesPath + "#" +
                    GetRelPath(articlesPath, elems[j][attrName]);
            }
        }
    };

    var GetTOC = function (tocArray) {
        var tocHTML = "";

        // Omit the first header
        for (var i = 1; i <= tocArray.length; i++) {
            var levelDiff = i < tocArray.length ?
                tocArray[i].level - tocArray[i - 1].level :
                tocArray[0].level - tocArray[i - 1].level;
            for (; levelDiff > 0; --levelDiff) tocHTML += "<ul>";
            for (; levelDiff < 0; ++levelDiff) tocHTML += "</ul>";
            if (i < tocArray.length)
                tocHTML += "<li>" + tocArray[i].text + "</li>";
        }
        return tocHTML;
    };

    Ajax("GET", encodeURI(fileName), null,
        function (mdSource) {
            if (mdSource == null) {
                callback(false);
                return;
            }

            var absPath = GetAbsPath(fileName);

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

                var articlesPath = null;
                if (tag.articlesPath != null)
                    articlesPath = GetAbsPath(tag.articlesPath);

                var secText = isSingleFile ? mdSource : GetSection(mdSource, tagName);

                // Patch for [TOC]
                // Ref: https://github.com/chjj/marked/issues/545
                var toc = [];
                var renderer = new marked.Renderer();
                renderer.heading = function (text, level, raw) {
                    var anchor = this.options.headerPrefix +
                        raw.toLowerCase().replace(/[^\w]+/g, '-');

                    // Add all headers
                    toc.push({ text: text, level: level });

                    return '<h' + level + ' id="' + anchor + '">' +
                        text + '</h' + level + '>\n';
                };
                marked.setOptions({
                    renderer: renderer
                });

                var content = isMd ? marked(secText) : secText;
                if (isMd && toc.length > 0) {
                    var tocHTML = "<div class='markdown-toc'><h1>Table of Content</h1>" +
                        GetTOC(toc) + "</div>";
                    content = content.replace("[TOC]", tocHTML);
                }

                var secs = document.getElementsByClassName(tagName);
                for (var j = 0; j < secs.length; j++) {
                    secs[j].innerHTML = content;

                    if (absPath != null)
                        FixRelativePaths(secs[j], absPath);

                    if (articlesPath != null)
                        FixArticlesPaths(secs[j], articlesPath);
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
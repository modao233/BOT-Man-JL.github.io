/*
	Extension for BOT Frame :-)
	BOT Man JL, 2016 (MIT License)

	Note that: Remenber to Load "bot-frame.js" and "marked.js" Before this
*/;

RenderSectionWithPrompt = function (fileName, promptTag, tags, callback)
{
    var promptSecs = document.getElementsByClassName(promptTag);
    for (var i = 0; i < promptSecs.length; i++)
        promptSecs[i].innerHTML = "Loading <em>" + fileName + "</em> 😇";

    RenderSection(fileName, tags, function (isOK)
    {
        for (var i = 0; i < promptSecs.length; i++)
            promptSecs[i].innerHTML = isOK ? "" :
                "<p>Loading <a href='" + fileName + "'><em>" +
                fileName + "</em></a> <strong>Failed</strong> 😅</p>" +
                "<p>👉👉👉 <a href='javascript:void(0)' onclick='location.reload();'>" +
                "Click to Refresh</a> 👈👈👈</p>";
        if (!isOK) document.title = "Loading Failed";

        if (callback != null) callback(isOK);
    });
};

// 1. Move the beginning H1 and BLOCKQUOTE from 'srcSec to 'dstSec
// 2. Return the content of H1 (title of article)
FixLayout = function (dstSec, srcSec)
{
    var childLen = srcSec.childNodes.length;
    if (childLen == 0) return;

    var curElem = 0;
    var ret = "";

    // First Element
    while (curElem < childLen &&
        srcSec.childNodes[curElem].nodeType != 1)
        curElem++;

    // Title of the Markdown Article
    if (curElem < childLen &&
        srcSec.childNodes[curElem].tagName.toUpperCase() == "H1")
    {
        ret = srcSec.childNodes[curElem].innerHTML;
        dstSec.innerHTML += '<h1>' + ret + '</h1>';
        srcSec.removeChild(srcSec.childNodes[curElem]);
    }

    // Second Element
    while (curElem < childLen &&
        srcSec.childNodes[curElem].nodeType != 1)
        curElem++;

    // BlockQuote Under the Title
    if (curElem < childLen &&
        srcSec.childNodes[curElem].tagName.toUpperCase() == "BLOCKQUOTE")
    {
        dstSec.innerHTML +=
            '<blockquote style="padding: 0 0 0 15px">' +
            srcSec.childNodes[curElem].innerHTML +
            '</blockquote>';
        srcSec.removeChild(srcSec.childNodes[curElem]);
    }

    return ret;
};
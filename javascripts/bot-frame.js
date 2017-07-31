/*
	Modules for GitHub Page :-)
	BOT Man, 2016 (MIT License)
*/

function GetSearchParam(target) {
    var searchStr = document.location.search;

    var beg = searchStr.indexOf(target);
    if (beg == -1) return "";

    beg += target.length + 1;
    var end = searchStr.indexOf('&', beg);
    if (end == -1) end = searchStr.length;

    return searchStr.substr(beg, end - beg);
}

function FileLoader() { }

FileLoader.prototype._get = function (url, callback) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState != 4) return;
        if (xhr.status == 200) callback(xhr.responseText);
        else callback(null);
    };
    xhr.send();
};

FileLoader.prototype.loading = function (filename) {
    return 'Loading <em>' + filename + '</em> 😇';
};

FileLoader.prototype.failed = function (filename) {
    if (document && document.title) document.title = "Loading Failed";
    return '<p>Loading <a href="' + filename + '"><em>' +
        filename + '</em></a> <strong>Failed</strong> 😅</p>' +
        '<p>👉👉👉 <a href="javascript:void(0)" onclick="location.reload();">' +
        'Try again</a> 👈👈👈</p>';
};

FileLoader.prototype.done = function (filename) {
    return '';
};

FileLoader.prototype.loadFile = function (filename, promptState, done) {
    var that = this;
    if (promptState) promptState(this.loading(filename));

    this._get(encodeURI(filename), function (text) {
        if (text == null) {
            if (promptState) promptState(that.failed(filename));
            return;
        }

        if (promptState) promptState(that.done(filename));
        if (done) done(text);
    });
};

function ConfigLoader() { }

ConfigLoader.prototype.nav = function (config) {
    var navHtml = '<h2>' + '~/ ';
    for (var page in config.pages) {
        navHtml += '<a href="' + config.pages[page] + '">' + page + '</a>' + ' | ';
    }
    navHtml = navHtml.substr(0, navHtml.length - 3) + '</h2>';
    return navHtml;
};

ConfigLoader.prototype.contact = function (config) {
    var contact = config.contact;
    var contactHtml = '<h1>' + contact.name + '</h1>';
    for (var entry in contact) {
        if (entry == 'name') continue;
        contactHtml += '<p><a href="' + contact[entry].link +
            '"><img src="' + contact[entry].img +
            '" /> - ' + contact[entry].text +
            '</a></p>';
    }
    return contactHtml;
};

ConfigLoader.prototype.footer = function (config) {
    var footerHtml = '';
    for (var entry in config.footer) {
        footerHtml += '<p>' + config.footer[entry] + '</p>';
    }
    return footerHtml;
};

function LoadLayout(filename, callback) {
    var configLoader = new ConfigLoader();
    function loadPart(config, sec) {
        document.getElementsByClassName(sec + 'Sec')[0]
            .innerHTML = configLoader[sec](config);
    }

    var mdSec = document.getElementsByClassName('markdown-body')[0];
    function promptState(text) {
        mdSec.innerHTML = text;
    }

    (new FileLoader()).loadFile('/static/config.json', promptState, function (config) {
        try {
            config = JSON.parse(config);

            loadPart(config, 'nav');
            loadPart(config, 'contact');
            loadPart(config, 'footer');

            (new FileLoader()).loadFile(filename, promptState, function (text) {
                if (callback) callback(text, function (title) {
                    document.title = title + ' | ' + config.name;
                });
            });

        } catch (e) {
            promptState('Bad JSON format in <em>' + filename + '</em> 😑');
            document.title = "Loading Failed";
        }
    });
}

function LoadStyle() {
    var isCover = false, isTocPageBreak = false,
        isWordMargin = false, isSlideMode = false;

    var styles = GetSearchParam("style").split('+');
    for (var i = 0; i < styles.length; i++) {
        if (styles[i] == "cover") isCover = true;
        else if (styles[i] == "toc-page-break") isTocPageBreak = true;
        else if (styles[i] == "word") isWordMargin = true;
        else if (styles[i] == "slide") isSlideMode = true;
    }

    var styleElem = document.createElement('style');
    if (isCover) {
        // TODO: slide-cover
        document.getElementsByClassName("headerTitleSec")[0].classList.add('cover-title');
        document.getElementsByClassName("headerQuoteSec")[0].classList.add('cover-subtitle');
        document.getElementsByTagName("header")[0].style.pageBreakAfter = "always";
    }
    if (isWordMargin)
        styleElem.innerHTML += "@page { margin: 25.4mm 31.8mm; }";
    else
        styleElem.innerHTML += "@page { margin: 15mm 15mm; }";
    if (isSlideMode)
        styleElem.innerHTML += "@page { size: 297mm 210mm; }";
    else
        styleElem.innerHTML += "@page { size: 210mm 297mm; }";
    if (isTocPageBreak)
        styleElem.innerHTML += "@media print { .markdown-toc { page-break-after: always; } }";
    document.head.appendChild(styleElem);
}

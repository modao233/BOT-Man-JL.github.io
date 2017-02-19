var qrcode = function (options)
{
	if (typeof options === 'string')
	{
		options = { text: options };
	}

	options = $.extend({}, {
		typeNumber: -1,
		correctLevel: QRErrorCorrectLevel.L,
		background: "#ffffff",
		foreground: "#000000"
	}, options);

	var _qrcode = new QRCode(options.typeNumber, options.correctLevel);
	_qrcode.addData(options.text);
	_qrcode.make();

	return _qrcode;
};

var qrcodeSVG = function (options)
{
	var _qrcode = qrcode(options);

	options = $.extend({}, {
		size: 256,
		color: "#000000"
	}, options);

	var cellSize = options.size / _qrcode.getModuleCount();
	var size = options.size;
	var c, mc, r, mr, qrSvg = '', rect;

	rect = 'l' + cellSize + ',0 0,' + cellSize +
	  ' -' + cellSize + ',0 0,-' + cellSize + 'z ';

	qrSvg += '<svg';
	qrSvg += ' width="' + size + 'px"';
	qrSvg += ' height="' + size + 'px"';
	qrSvg += ' xmlns="http://www.w3.org/2000/svg"';
	qrSvg += '>';
	qrSvg += '<path d="';

	for (r = 0; r < _qrcode.getModuleCount() ; r += 1)
	{
		mr = r * cellSize;
		for (c = 0; c < _qrcode.getModuleCount() ; c += 1)
		{
			if (_qrcode.isDark(r, c))
			{
				mc = c * cellSize;
				qrSvg += 'M' + mc + ',' + mr + rect;
			}
		}
	}

	qrSvg += '" stroke="transparent" fill="' + options.color + '"/>';
	qrSvg += '</svg>';

	return qrSvg;
};

var qrcodeCanvas = function (options)
{
	var _qrcode = qrcode(options);

	options = $.extend({}, {
		size: 256,
		color: "#000000"
	}, options);

	// create canvas element
	var canvas = document.createElement('canvas');
	canvas.width = options.size;
	canvas.height = options.size;
	canvas.style = "background-color:transparent;";

	// compute tileW/tileH based on options.width/options.height
	var tileW = canvas.width / _qrcode.getModuleCount();
	var tileH = canvas.height / _qrcode.getModuleCount();

	// draw in the canvas
	var ctx = canvas.getContext('2d');
	ctx.fillStyle = options.color;
	for (var row = 0; row < _qrcode.getModuleCount() ; row++)
	{
		for (var col = 0; col < _qrcode.getModuleCount() ; col++)
		{
			if (_qrcode.isDark(row, col))
			{
				var w = (Math.ceil((col + 1) * tileW) - Math.floor(col * tileW));
				var h = (Math.ceil((row + 1) * tileW) - Math.floor(row * tileW));
				ctx.fillRect(Math.round(col * tileW), Math.round(row * tileH), w, h);
			}
		}
	}

	return canvas;
};

function utf16to8(str)
{
	var out = "";
	for (var i = 0; i < str.length; i++)
	{
		var c = str.charCodeAt(i);
		if ((c >= 0x0001) && (c <= 0x007F))
		{
			out += str.charAt(i);
		}
		else if (c > 0x07FF)
		{
			out += String.fromCharCode(0xE0 | ((c >> 12) & 0x0F));
			out += String.fromCharCode(0x80 | ((c >> 6) & 0x3F));
			out += String.fromCharCode(0x80 | ((c >> 0) & 0x3F));
		}
		else
		{
			out += String.fromCharCode(0xC0 | ((c >> 6) & 0x1F));
			out += String.fromCharCode(0x80 | ((c >> 0) & 0x3F));
		}
	}
	return out;
}

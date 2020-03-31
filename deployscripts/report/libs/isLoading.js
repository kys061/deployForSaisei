(function (global, factory) {
	typeof exports === 'object' && typeof module !== 'undefined' ? module.exports = factory() :
	typeof define === 'function' && define.amd ? define(factory) :
	(global.isLoading = factory());
}(this, (function () { 'use strict';

function unwrapExports (x) {
	return x && x.__esModule ? x['default'] : x;
}

function createCommonjsModule(fn, module) {
	return module = { exports: {} }, fn(module, module.exports), module.exports;
}

var index$1 = createCommonjsModule(function (module, exports) {
'use strict';

exports.__esModule = true;
exports.default = createElement;
/**
 * Create a DOM element from a CSS query with option to include content
 *
 * @author Laurent Blanes <laurent.blanes@gmail.com>
 * @param {String} querySelector (optional) default to div
 * @param {...*} [content] (optional) String|Number|DOMElement
 * @return DOMElement
 *
 * @example
 * - createElement(); // <div>
 * - createElement('span#my-id.my-class.second-class'); // <span id="my-id" class="my-class second-class">
 * - createElement('#my-id.my-class.second-class', 'text to insert', 12345); // <div id="my-id" class="my-class second-class">
 * - const div = createElement('#my-div',
 *     'Random text',
 *     createElement('p.paragraph', 'my text'),
 *     createElement('p.paragraph', 'my second text'),
 *     createElement('a.link[href=https://github.com/hekigan/create-element]', 'link to a site'),
 * ); // <div id="my-id" class="my-class second-class">
 *    //   Random text
 *    //   <p class="paragraph">my text</p>
 *    //   <p class="paragraph">my second text</p>
 *    //   <a class="link" href="https://github.com/hekigan/create-element" class="paragraph">link to a site</a>
 *    // </div>
 */
function createElement() {
    var querySelector = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 'div';

    var nodeType = querySelector.match(/^[a-z]+/i);
    var id = querySelector.match(/#([a-z]+[a-z0-9-]*)/gi);
    var classes = querySelector.match(/\.([a-z]+[a-z0-9-]*)/gi);
    var attributes = querySelector.match(/\[([a-z][a-z-]+)(=['|"]?([^\]]*)['|"]?)?\]/gi);
    var node = nodeType ? nodeType[0] : 'div';

    if (id && id.length > 1) {
        throw CreateElementException('only 1 ID is allowed');
    }

    var elt = document.createElement(node);

    if (id) {
        elt.id = id[0].replace('#', '');
    }

    if (classes) {
        var attrClasses = classes.join(' ').replace(/\./g, '');
        elt.setAttribute('class', attrClasses);
    }

    if (attributes) {
        attributes.forEach(function (item) {
            item = item.slice(0, -1).slice(1);

            var _item$split = item.split('='),
                label = _item$split[0],
                value = _item$split[1];

            if (value) {
                value = value.replace(/^['"](.*)['"]$/, '$1');
            }
            elt.setAttribute(label, value || '');
        });
    }

    for (var _len = arguments.length, content = Array(_len > 1 ? _len - 1 : 0), _key = 1; _key < _len; _key++) {
        content[_key - 1] = arguments[_key];
    }

    content.forEach(function (item) {
        if (typeof item === 'string' || typeof item === 'number') {
            elt.appendChild(document.createTextNode(item));
        } else if (item.nodeType === document.ELEMENT_NODE) {
            elt.appendChild(item);
        }
    });

    return elt;
}

function CreateElementException(message) {
    this.message = message;
    this.name = 'CreateElementException';
}
module.exports = exports['default'];
});

var createElement = unwrapExports(index$1);

var _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; };

var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

var index = (function () {
    for (var _len = arguments.length, params = Array(_len), _key = 0; _key < _len; _key++) {
        params[_key] = arguments[_key];
    }

    return new IsLoading(params);
});

var formElements = ['form', 'input', 'textarea', 'label', 'fieldset', 'select', 'button'];

var optionsDefault = {
    'type': 'switch', // switch | replace | full-overlay | overlay
    'text': 'loading', // Text to display in the loader
    'disableSource': true, // true | false
    'disableList': []
};

var IsLoading = function () {
    function IsLoading(params) {
        _classCallCheck(this, IsLoading);

        var options = {};
        if (params.length === 0 || params.length === 1 && !params[0].nodeType) {
            this._target = null;
            options = _extends({}, params[0], { type: 'full-overlay' });
        } else {
            this._target = params[0];
            options = params[1];
        }
        this._options = _extends({}, optionsDefault, options);
        this._fullOverlayId = 'is-loading-full-overlay';
    }

    IsLoading.prototype.loading = function loading() {
        switch (this._options.type) {
            case 'replace':
                this._onReplaceType();break;
            case 'full-overlay':
                this._onFullOverlayType();break;
            case 'overlay':
                this._onElementOverlayType();break;
            default:
                this._onSwitchType();break;
        }
    };

    IsLoading.prototype.restoreContent = function restoreContent() {
        var content = this._target.getAttribute('data-is-loading-content');
        if (this.isTargetValue) {
            this._target.value = content;
        } else {
            this._target.textContent = content;
        }
    };

    IsLoading.prototype._onSwitchType = function _onSwitchType() {
        this._toggleElements(false);
        this._target.setAttribute('data-is-loading-content', this.targetContent);
        this.targetContent = this._options.text;
    };

    IsLoading.prototype._onReplaceType = function _onReplaceType() {
        this._toggleElements(false);
        this._target.setAttribute('data-is-loading-content', this.targetContent);
        this._target.innerHTML = '';
        this._target.appendChild(createElement('span.is-loading.is-loading-target', this._options.text));
    };

    IsLoading.prototype._onElementOverlayType = function _onElementOverlayType() {
        this._toggleElements(false);
        var overlayWrapperClass = '.is-loading-element-overlay';

        if (this._prop('position') === 'static') {
            this._target.setAttribute('data-is-loading-position', 'static');
            this._target.classList.add('is-loading-element-overlay-target');
        }

        if (!this._target.querySelector(overlayWrapperClass)) {
            var overlay = createElement(overlayWrapperClass, createElement('.is-loading-text-wrapper', this._options.text));
            overlay.style.borderRadius = this._prop('border-radius');
            this._target.appendChild(overlay);
        }
    };

    IsLoading.prototype._onFullOverlayType = function _onFullOverlayType() {
        this._toggleElements(false);
        this._showFullOverlay();
    };

    IsLoading.prototype._showFullOverlay = function _showFullOverlay() {
        var overlay = document.querySelector(this._fullOverlayId);

        if (!overlay) {
            overlay = createElement('#' + this._fullOverlayId, createElement('.is-loading-text-wrapper', this._options.text));
            document.querySelector('body').appendChild(overlay);
        }
    };

    IsLoading.prototype._prop = function _prop(prop) {
        return window.getComputedStyle(this._target).getPropertyValue(prop);
    };

    IsLoading.prototype._toggleElements = function _toggleElements() {
        var status = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : true;

        var list = [].concat(this._options.disableList);
        if (this._target && this._options.disableSource === true) {
            list.unshift(this._target);
        }
        list.forEach(function (item) {
            if (formElements.includes(item.tagName.toLowerCase())) {
                if (status === true) {
                    item.removeAttribute('disabled');
                } else {
                    item.setAttribute('disabled', 'disabled');
                }
            }
            if (status === true) {
                item.classList.remove('disabled');
            } else {
                item.classList.add('disabled');
            }
        });
    };

    IsLoading.prototype.remove = function remove() {
        this._toggleElements(true);
        if (this._options.type === 'switch') {
            this.restoreContent();
        }
        if (this._target) {
            this._target.removeAttribute('data-is-loading-content');
        }
        if (this._options.type === 'full-overlay') {
            var overlay = document.getElementById(this._fullOverlayId);
            document.querySelector('body').removeChild(overlay);
        }
        if (this._target && this._target.getAttribute('data-is-loading-position')) {
            this._target.classList.remove('is-loading-element-overlay-target');
        }
    };

    _createClass(IsLoading, [{
        key: 'targetContent',
        get: function get() {
            if (this.isTargetValue) {
                return this._target.value;
            } else {
                return this._target.textContent;
            }
        },
        set: function set(val) {
            if (this.isTargetValue) {
                this._target.value = val;
            } else {
                this._target.textContent = val;
            }
        }
    }, {
        key: 'isTargetValue',
        get: function get() {
            var node = this._target.nodeName.toLowerCase();
            var type = this._target.attributes.type;

            return node === 'input' && type && (type.value.toLowerCase() === 'button' || type.value.toLowerCase() === 'submit');
        }
    }]);

    return IsLoading;
}();

return index;

})));

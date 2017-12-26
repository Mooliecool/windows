function ProgressBar() {
    var _width = 540;
    var _height = 15;
    var _maxValue = 100;
    var _minValue = 1;
    var _step = 1;
    var _value = 0;
    var _id = "DefaultProgressBar";
    var _backClass = "DefaultProgressBackClass";
    var _barClass = "DefaultProgressBarClass";
    this.ID = _id;
    var _bar;
    var _Hbar;
    this.Width = _width;
    this.Height = _height;
    this.MaxValue = _maxValue;
    this.MinValue = _minValue;
    this.Step = _step;
    this.Value = _value;
    this.BackClass = _backClass;
    this.BarClass = _barClass;

    this.Init = function (container, id) {
        _id = id;
        var con = document.getElementById(container);
        var progressbar = document.createElement("DIV");
        progressbar.style.width = _width + "px";
        progressbar.style.height = _height + "px";
        progressbar.ID = "progressbar_" + _id;
        progressbar.className = _backClass;
        progressbar.style.textAlign = "left";
        con.appendChild(progressbar);
        _Hbar = progressbar;

        var progressbarInner = document.createElement("DIV");
        progressbarInner.style.width = "0";
        progressbarInner.style.height = _height + "px";
        progressbarInner.ID = "progressbarInner_" + _id;
        progressbarInner.className = _barClass;
        progressbar.appendChild(progressbarInner);
        _bar = progressbarInner;
        changeBar();
    }
    var changeBar = function () {

        var progressbarInner = _bar;
        if (progressbarInner == null) return;
        innerWidth = Math.floor((_value / _maxValue) * _width);
        progressbarInner.style.width = innerWidth + "px";
        if (_value == 0 || innerWidth == _width) {
            _Hbar.style.display = "none";
            _value = 0;
        }
        else {
            _Hbar.style.display = "block";
        }
    }
    this.SetProgress = function (num) {
        _value = num;
        changeBar();
    }
}

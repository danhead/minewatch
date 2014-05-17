document.addEventListener( "DOMContentLoaded", function() {
	document.removeEventListener( "DOMContentLoaded", arguments.callee, false );
	new form();
}, false );

var form = function() {
	this.el = {
		"bgcolour":document.getElementById("bgcolour"),
		"notify":document.getElementById("notify"),
		"flick":document.getElementById("flick"),
		"update":document.getElementById("update"),
		"submit":document.getElementById("submit"),
		"apikey":document.getElementById("apikey"),
		"save":document.getElementById("save")
	};
	this.config = this.getConfig();
	this.updateForm();
    this.showVersion(this.config.pblversion,this.config.jsversion);
	this.attach();
}
form.prototype.getConfig = function() {
	var getParameterByName = function(name) {
 	   name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
	    var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(location.search);
  	  return results == null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
	}
	var config = {};
    config.jsversion = getParameterByName("jsversion");
    config.pblversion = getParameterByName("pblversion");
	config.apikey = getParameterByName("apikey");
	config.update = getParameterByName("update");
	config.dark = (getParameterByName("dark") == "true")?true:false;
	config.notify = (getParameterByName("notify") == "true")?true:false;
	config.flick = (getParameterByName("flick") == "true")?true:false;
	return config;
}
form.prototype.updateForm = function() {
	if(this.config.apikey.length>0) this.el.apikey.value = this.config.apikey;
	if(this.config.update.length>0) this.el.update.value = this.config.update;

	if(this.config.dark) {
		this.el.bgcolour.className = "black";
		this.el.bgcolour.children[0].innerHTML = "BLACK";
	} else {
		this.el.bgcolour.className = "white";
		this.el.bgcolour.children[0].innerHTML = "WHITE";
	}
	if(this.config.notify) {
		this.el.notify.className = "on";
		this.el.notify.children[0].innerHTML = "ON";
	} else {
		this.el.notify.className = "off";
		this.el.notify.children[0].innerHTML = "OFF";
	}
	if(this.config.flick) {
		this.el.flick.className = "on";
		this.el.flick.children[0].innerHTML = "ON";
	} else {
		this.el.flick.className = "off";
		this.el.flick.children[0].innerHTML = "OFF";
	}
}
form.prototype.showVersion = function(pbl,js) {
    var version = (pbl)?pbl:js;
    var listItems = document.getElementsByClassName("app-version");
    for (var i=0;i<listItems.length;i++) {
        if(listItems[i].getAttribute("data-version") == version) {
            var h3 = listItems[i].getElementsByTagName("h3")[0];
            h3.innerHTML = h3.innerHTML + " &ndash; <span>INSTALLED</span>";
        }
    }
    if(pbl && js) {
        document.getElementById("appversioninfo").innerHTML = pbl;
        document.getElementById("jsversioninfo").innerHTML = js;
        document.getElementById("versioninfo").style.display = "inline";
    }
}
form.prototype.attach = function() {
	var that = this;
	this.el.bgcolour.addEventListener("click", function(e) {
		e.preventDefault();
		that.config.dark = !that.config.dark
		that.updateForm();
	});
	this.el.notify.addEventListener("click", function(e) {
		e.preventDefault();
		that.config.notify = !that.config.notify;
		that.updateForm();
	});
	this.el.flick.addEventListener("click", function(e) {
		e.preventDefault();
		that.config.flick = !that.config.flick;
		that.updateForm();
	});
	this.el.update.addEventListener("change", function() {
		that.config.update = this.value;
	});
	this.el.apikey.addEventListener("change", function() {
		that.config.apikey = this.value;
	})
	this.el.save.addEventListener("click", function(e) {
		e.preventDefault();
		var str = "pebblejs://close#" + encodeURIComponent(JSON.stringify(that.config));
		window.location.href = str;
	});
	/*
	document.getElementById("reset").addEventListener("click", function(e) {
		e.preventDefault();
		window.location.href = "pebblejs://close#" + encodeURIComponent(JSON.stringify({"reset":true}));		
	})*/
}
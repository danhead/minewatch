var config = {
	"jsversion":"1.0.2",
	"pblversion":(localStorage.getItem("pblversion") || ""),
	"dark":((localStorage.getItem("dark") == "true")?true:false),
	"notify":((localStorage.getItem("notify") == "true")?true:false),
	"flick":((localStorage.getItem("flick") == "true")?true:false),
	"apikey":(localStorage.getItem("apikey") || ""),
	"update":(localStorage.getItem("update") || "15")
};

var formatHashrate = function(a) {
	var val = Math.abs(a).toFixed(0);
	var suffix = "MH/s";
	if(val>9999) {
		val = (val/1000).toFixed(0);
		suffix = "GH/s";
	}
	return val+suffix;
}

var formatLastShare = function(a) {

	var date = new Date(a*1000);
	var h = date.getHours(), m = date.getMinutes(), s = date.getSeconds();
	return (h<10?"0"+h:h) + ":" + (m<10?"0"+m:m) + ":" + (s<10?"0"+s:s);
}

var calcSendProgress = function(amount,send) {
	return Math.ceil(144*(amount/send));
}

var sendResponseWithUpdate = function(obj) {
	if(config.apikey == "") {
		Pebble.sendAppMessage({
			"response":"NoAPI"
		});
		return;
	}
	var req = new XMLHttpRequest();
	req.onreadystatechange = function() {
		if (req.readyState == 4) {
			if(req.status == 200) {
				var response = JSON.parse(req.responseText);
				if(response) {
					var workers = 0, total = 0, lastshare = 0;
					for(var i in response.workers) {
						total++;
						if(response.workers[i].alive) workers++;
						if(response.workers[i].last_share > lastshare) lastshare = response.workers[i].last_share;
					}
					
					if(lastshare>0)	lastshare = formatLastShare(lastshare);//((new Date().getTime()/1000).toFixed(0) - lastshare);
					else lastshare = "Unknown";
					
					var reward = Math.abs(response.confirmed_reward).toFixed(4);
					var res = {
						"response":"OK",
						"workers":workers.toString(),
						"hashrate":formatHashrate(response.hashrate),
						"reward":reward.toString()+" BTC",
						"lastshare":lastshare,
						"sendprogress":calcSendProgress(reward,response.nmc_send_threshold).toString(),
						"update":config.update.toString()
					};
					if(obj) {
						for(var i in obj) {
							res[i] = obj[i];
						}
					}
					Pebble.sendAppMessage(res);
				}
			} else if(req.status == 401) {
				if(!obj) obj = {};
				obj["response"] = "InvAPI";
				Pebble.sendAppMessage(obj);
			}
		}
	}
	req.open('GET', "https://mining.bitcoin.cz/accounts/profile/json/" + config.apikey, true);
	req.timeout = 30000;
	req.ontimeout = function() {
		if(!obj) obj = {};
		obj["response"] = "TIMEOUT";
		Pebble.sendAppMessage(obj);
	}
	req.send(null);
}
var getSettings = function() {
	return {
		"dark":((config.dark)?"true":"false"),
		"notify":((config.notify)?"true":"false"),
		"flick":((config.flick)?"true":"false")
	};
}


Pebble.addEventListener("ready", function(e) {
	sendResponseWithUpdate(getSettings());
});

Pebble.addEventListener("appmessage", function(e) {
	if(e.payload.version) {
		config.pblversion = e.payload.version;
		localStorage.setItem("pblversion",e.payload.version);
	}
	sendResponseWithUpdate();
});
Pebble.addEventListener("showConfiguration", function(e) {
	var str = "?";
	for(var i in config) {
		if(config[i] != "error") str += i + "=" + config[i] + "&";
	}
	str = str.substring(0, str.length - 1);
	Pebble.openURL("https://danielhead.com/minewatch/config" + str);
});

Pebble.addEventListener("webviewclosed", function(e) {
	var c = JSON.parse(decodeURIComponent(e.response));
	if(c.reset) {
		localStorage.removeItem("apikey");
		localStorage.removeItem("update");
		localStorage.removeItem("dark");
		localStorage.removeItem("notify");
		localStorage.removeItem("flick");
	} else {
		config.apikey = c.apikey;
		localStorage.setItem("apikey",config.apikey);
		config.update = c.update;
		localStorage.setItem("update",config.update);
		config.dark = c.dark;
		localStorage.setItem("dark",config.dark);
		config.notify = c.notify;
		localStorage.setItem("notify",config.notify);
		config.flick = c.flick;
		localStorage.setItem("flick",config.flick);
	}
	sendResponseWithUpdate(getSettings());
});

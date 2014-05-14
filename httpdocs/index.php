<?php
define("accessAllowed", true);
?>
<!doctype html>
<html class="no-js" lang="en">
<head>
    <meta charset="utf-8"/>
    <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
    <title>MineWatch &ndash; Your Bitcoin workers, now on your wrist</title>
    <link rel="stylesheet" href="css/foundation.css"/>
    <link rel="stylesheet" href="css/main.css"/>
    <link href='http://fonts.googleapis.com/css?family=Holtwood+One+SC' rel='stylesheet' type='text/css'>
    <script src="js/vendor/modernizr.js"></script>
    <script src="http://coinwidget.com/widget/coin.js"></script>
</head>
<body>
<div id="fb-root"></div>
<script>(function (d, s, id) {
        var js, fjs = d.getElementsByTagName(s)[0];
        if (d.getElementById(id)) return;
        js = d.createElement(s);
        js.id = id;
        js.src = "//connect.facebook.net/en_GB/all.js#xfbml=1&appId=503038196473681";
        fjs.parentNode.insertBefore(js, fjs);
    }(document, 'script', 'facebook-jssdk'));</script>
<div class="row heading">
    <div class="show-for-medium-up medium-4 columns"><img src="img/pebble_white.png" alt="Pebble"/></div>
    <div class="medium-8 small-12 columns">
        <h1>MineWatch</h1>

        <h2>Your Bitcoin workers, now on your wrist</h2>

        <div class="show-for-small pebble-img"><img src="img/pebble_white.png" alt="Pebble"/></div>
        <div class="panel">
            <h3 class="show-for-small">Features</h3>
            <ul class="features">
                <li>Alerts when a worker goes offline</li>
                <li>Update intervals from 1 minute to 60 minutes</li>
                <li>Payout progress indicator</li>
                <li>Flick to update option</li>
                <li>Light and Dark theme</li>
            </ul>
        </div>
    </div>
</div>

<div class="row">
    <div class="large-12 columns">
        <div class="panel news">
            <h3>NEWS</h3>
            <hr/>
            <h4>MineWatch updated to 1.0.2</h4>
            <p class="date">by Dan &ndash; 15th May at 19:42</p>

            <p>v1.0.2 includes the following fixes:</p>

            <ul>
                <li>Increase timeout from 10 to 30 seconds</li>
                <li>Update settings to show installed version</li>
            </ul>

            <hr/>
            <h4>MineWatch released</h4>
            <p class="date">by Dan &ndash; 8th May at 21:42</p>

            <p>MineWatch has been published to the Pebble Appstore today. It is available immediately on Android and is currently pending approval on iOS, this process should take no more than 2 weeks.</p>

            <p>The watchface currently only supports Slush's Pool, please fill out the form below if you with to request support for other Pools.</p>
        </div>
    </div>
</div>
<div class="row">
    <div class="large-8 medium-8 columns">
        <div class="panel">
            <?php
            if (isset($_GET["success"]) && $_GET["success"] == "true") {
                include("includes/success.php");
            } else {
                include("includes/form.php");
            }
            ?>
        </div>
    </div>
    <div class="large-4 medium-4 columns">
        <div class="panel social">
            <h3>Follow me</h3>
            <a href="https://twitter.com/danhead" class="twitter-follow-button" data-show-count="false">Follow
                @danhead</a>

            <div class="fb-like" data-href="http://danielhead.com/minewatch" data-width="120" data-layout="button_count"
                 data-action="like" data-show-faces="true" data-share="true"></div>
            <script>!function (d, s, id) {
                    var js, fjs = d.getElementsByTagName(s)[0], p = /^http:/.test(d.location) ? 'http' : 'https';
                    if (!d.getElementById(id)) {
                        js = d.createElement(s);
                        js.id = id;
                        js.src = p + '://platform.twitter.com/widgets.js';
                        fjs.parentNode.insertBefore(js, fjs);
                    }
                }(document, 'script', 'twitter-wjs');</script>
            <!-- Place this tag where you want the +1 button to render. -->
            <div class="g-plusone" data-size="medium"></div>

            <!-- Place this tag after the last +1 button tag. -->
            <script type="text/javascript">
                window.___gcfg = {lang: 'en-GB'};

                (function () {
                    var po = document.createElement('script');
                    po.type = 'text/javascript';
                    po.async = true;
                    po.src = 'https://apis.google.com/js/platform.js';
                    var s = document.getElementsByTagName('script')[0];
                    s.parentNode.insertBefore(po, s);
                })();
            </script>
        </div>
        <div class="panel donate">
            <h3>Donate</h3>

            <p>Like this watch face? donations, no matter how small are much appreciated! I accept PayPal and
                Bitcoins</p>

            <form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
                <input type="hidden" name="cmd" value="_s-xclick">
                <input type="hidden" name="hosted_button_id" value="TM7D9WU67C6MJ">
                <input type="image" src="https://www.paypalobjects.com/en_GB/i/btn/btn_donate_SM.gif" border="0"
                       name="submit" alt="PayPal – The safer, easier way to pay online.">
                <img alt="" border="0" src="https://www.paypalobjects.com/en_GB/i/scr/pixel.gif" width="1" height="1">
            </form>
            <script>
                CoinWidgetCom.go({
                    wallet_address: "1CCsuAXhEbLiL2a43x1yxuhk5F6hEt1EBQ",
                    currency: "bitcoin",
                    counter: "count",
                    alignment: "bl",
                    qrcode: true,
                    auto_show: false,
                    lbl_button: "Donate",
                    lbl_address: "My Bitcoin Address:",
                    lbl_count: "donations",
                    lbl_amount: "BTC"
                });
            </script>
        </div>
    </div>
</div>


<div class="row footer">
    <div class="large-12 columns">
        <p>&copy;2014 <a href="http://danielhead.com/">Daniel Head</a></p>
    </div>
</div>

<script src="js/vendor/jquery.js"></script>
<script src="js/foundation.min.js"></script>
<script src="js/main.js"></script>
<script>
    $(document).foundation();
    (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
        (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
        m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
    })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

    ga('create', 'UA-20070613-3', 'danielhead.com');
    ga('send', 'pageview');

</script>
</body>
</html>

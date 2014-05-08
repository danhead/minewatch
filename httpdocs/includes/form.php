<?php
if(!defined("accessAllowed")) die();
function hasFailed() {
    return (isset($_GET["success"]) && $_GET["success"] == "false") ? true : false;
}
function showError($param) {
    $failed = hasFailed();
    if(!isset($_GET[$param]) && $failed) return true;
    return false;
}
function getInput($param,$raw) {
    $failed = hasFailed();
    if(isset($_GET[$param]) && $failed) {
        if($raw) return $_GET[$param];
        else return 'value="' . $_GET[$param] . '"';
    }
    return "";
}
function checkOption($value) {
    $failed = hasFailed();
    if($failed && isset($_GET["reason"]) && $_GET["reason"] == $value) return true;
    else return false;
}
?>
<form id="contactform" method="post" action="submit.php">
  <div class="row">
    <div class="large-12 columns">
        <h3>Contact</h3>
        <p>Found a bug? Want to request a new feature? Want to use another mining pool? Contact me now and let me know!</p>
        <?php if(hasFailed()) {
            echo '<p class="error">Your feedback has not been sent, please review the form below and retry.</p>';
        }
        ?>
    </div>
  </div>
  <div class="row">
    <div class="large-4 medium-4 columns">
      <label for="form_name">Name:</label>
      <input <?php echo (showError("name"))?'class="red" ' : ''; ?>type="text" id="form_name" placeholder="Name" name="name" <?php echo getInput("name",false); ?>/>
    </div>
    <div class="large-4 medium-4 columns">
      <label for="form_email">Email address:</label>
      <input <?php echo (showError("email"))?'class="red" ' : ''; ?>type="email" id="form_email" placeholder="Email address" name="email" <?php echo getInput("email",false); ?>/>
    </div>
    <div class="large-4 medium-4 columns">
      <div class="row collapse">
        <label for="form_reason">Reason</label>
        <select name="reason" id="form_reason">
          <option value="bug"<?php echo (checkOption("bug")) ? " selected" : ""; ?>>Bug</option>
          <option value="feature"<?php echo (checkOption("feature")) ? " selected" : ""; ?>>Request new feature</option>
          <option value="newpool"<?php echo (checkOption("newpool")) ? " selected" : ""; ?>>Add support for new pool</option>
          <option value="other"<?php echo (checkOption("bug")) ? " selected" : ""; ?>>Other</option>
        </select>
      </div>
    </div>
  </div>
  <div class="row">
    <div class="large-12 columns">
      <label for="form_feedback">Feedback:</label>
      <textarea <?php echo (showError("feedback"))?'class="red" ' : ''; ?>name="feedback" id="form_feedback" cols="30" rows="10"><?php echo getInput("feedback",true);?></textarea>
    </div>
  </div>
  <div class="row">
    <div class="large-12 columns">
      <input class="small button" type="submit" name="submit" value="Send" />
    </div>
  </div>
</form>
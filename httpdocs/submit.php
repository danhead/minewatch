<?php
$headers = getallheaders();
$json = ($headers['Accept'] == "application/json") ? true : false;
$success = false;
$name = (isset($_POST['name'])) ? $_POST['name'] : false;
$email = (isset($_POST['email'])) ? $_POST['email'] : false;
$reason = (isset($_POST['reason'])) ? $_POST['reason'] : false;
$feedback = (isset($_POST['feedback'])) ? $_POST['feedback'] : false;

if(!filter_var($email,FILTER_VALIDATE_EMAIL)) $email = false;
if($name && $email && $reason && $feedback) {
    $message = "Name: " . $name . "\r\n";
    $message .= "Reason: " . $reason . "\r\n";
    $message .= "Feedback: " . $feedback;
    $message = wordwrap($message, 70, "\r\n");
    $headers = 'From: ' . $email . "\r\n" . 'Reply-To: ' . $email . "\r\n";
    mail('me@danielhead.com', 'MineWatch feedback', $message, $headers);
    $success = true;
}

if($json) {
    $response = array("success" => $success);
    $data = array(
        "name"=>$name,
        "email"=>$email,
        "reason"=>$reason,
        "feedback"=>$feedback
    );
    $response["data"] = $data;
    echo json_encode($response);
} else {
    $str = "";
    if($success) {
        $str .= "?success=true";
    } else {
        $str .= "?success=false";
        if ($name) $str .= "&name=" . $name;
        if ($email) $str .= "&email=" . $email;
        if ($reason) $str .= "&reason=" . $reason;
        if ($feedback) $str .= "&feedback=" . $feedback;
    }
    header("Location: /minewatch/" . $str . "#thanks");
}
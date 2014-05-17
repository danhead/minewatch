$("#contactform").submit(function(e) {
  e.preventDefault();
  $.ajax({
    type: "POST",
		dataType: "json",
    contentType: "application/x-www-form-urlencoded",
    url: "/minewatch/submit.php",

    headers: {
      "Accept" : "application/json"
    },
    data: {
      name: $("#form_name").val(),
      email: $("#form_email").val(),
      reason: $("#form_reason").val(),
      feedback: $("#form_feedback").val()
    }
  }).done(function(response) {
		var data = response.data;
    if(response.success) {
			$.ajax({
				url:"/minewatch/includes/success.php"
			}).done(function(response) {
				var thanks = $(response);
				thanks.hide();
				$("#contactform").fadeOut("fast",function() {
					$(this).replaceWith(thanks);
					thanks.fadeIn("fast");
				})
			});
		} else {
			if(data.name == "") $("#form_name").addClass("red");
			if(data.email == "") $("#form_email").addClass("red");
			if(data.feedback == "") $("#form_feedback").addClass("red");
		}
  });
});
$("#contactform input, #contactform textarea").blur(function() {
	if($(this).val().length>0) $(this).removeClass("red");
});
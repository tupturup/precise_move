$(function() {
	var submit_form = function(e) {
		$.getJSON(
			//url - A string containing the URL to which the request is sent.
			$SCRIPT_ROOT+'/_add_numbers',
			//data - A plain object or string that is sent to the server with the request.
			{
				value_x: $('input[name="value_x"]').val(),
				value_y: $('input[name="value_y"]').val(),
				value_z: $('input[name="value_z"]').val(),
				name: $('input [name="name"]').val()
			},
			//success - A callback function that is executed if the request succeeds.
			function(data) {
				$('#result').text(data.result);
				$('input[name=x]').focus().select();
			});
			return false;
		};
		$('button#calculate').click(submit_form);
		$('input[type=text]').keydown(function(e) {
			if (e.keyCode == 13) {
				submit_form(e);
			}
		});
		$('input[name=x]').focus();
});

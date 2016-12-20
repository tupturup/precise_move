$(function() {
	var submit_form = function(e) {
		$.getJSON(
			//url - A string containing the URL to which the request is sent.
			$SCRIPT_ROOT+'/_add_numbers',
			//data - A plain object or string that is sent to the server with the request.
			{
				x: $('input[name="x"]').val(),
				y: $('input[name="y"]').val(),
				z: $('input[name="z"]').val(),
				t: $('input [name="t"]').val()
			},
			//success - A callback function that is executed if the request succeeds.
			function(data) {
				$('#result').text(data.result);
				$('input[name=x]').focus().select();
			});
			return false;
		};
		//$('button#calculate').on('click', submit_form);
		$('button#calculate').click(submit_form);
		//$('input[type=text]').on('keydown', function(e) {
		$('input[type=text]').keydown(function(e) {
			if (e.keyCode == 13) {
				submit_form(e);
			}
		});
		$('input[name=x]').focus();
	});

$(document).ready(function() {

 $("#run").click(function() {

    $.getJSON({
    url: "/run/{{tgt_id}}",
    data: "{ result:result, targets:targets }",
    success: function(data){
        $("#result").html(data);
    }
    });

    //document.getElementById('result').innerHTML = rez;

 });


   $('#add_new_target').click(function() {
      $.ajax("{{ url_for('add_numbers') }}").done(function (reply) {
         $('#target_table').html(reply);
      });
   });

   $('#run').click(function() {
      $.ajax("{{ url_for('run_target') }}").done(function (reply) {
         $('#returned_result').html(reply);
      });
   });

});

$(document).ready(function() {

 $("#run").click(function() {
    $.getJSON({
      url: "/targets/{{tgt_id}}/run",
      data: "{ result:result, targets:targets }",
      success: function(data){
          $("#result").html(data);
      }
      });
      $("#add_search").hide();
    //document.getElementById('result').innerHTML = rez;
 });
});

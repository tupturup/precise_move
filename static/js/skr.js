$(document).ready(function() {

 $("#run").click(function() {

    $.getJSON({
    url: "/run/{{tgt_id}}",
    data: { result:result, targets:targets },
    success: function(data){
        $("#result").html(data);
    }
    });

    //document.getElementById('result').innerHTML = rez;

 });

});

$(document).ready(function() {

 $("#run").click(function() {

    $.getJSON({
    url: "/run",
    data: { result: result },
    success: function(data){
        $("#result").html(data);
    }
    });

    //document.getElementById('result').innerHTML = rez;

 });

});

$(function() {
  var close = document.getElementsByClassName("closebtn");
  var i;

  for (i = 0; i < close.length; i++) {
    close[i].onclick = function(){
      var div = this.parentElement;
      div.style.opacity = "0";
      setTimeout(function(){ div.style.display = "none"; }, 600);
    }
  }

//---------------------------------

  // var h = document.getElementsByTagName('h1');
  // var i;
  //
  // $(h).click(function(){
  //   //if(this.attr("name" == "delButton")){
  //     $(this).parent().parent().hide(300);
  //   //}
  // });

//----------------------------------



/*  document.body.onmousemove = function(e){
    e = e || window.event;

    var pageY = e.pageY;
    var pageX = e.pageX;

    if(pageX === undefined)
    {
      pageX = e.clientX + document.body.scrollLeft + document.documentElement.scrollLeft;
      pageY = e.clientY + document.body.scrollTop + document.documentElement.scrollTop;
    }
    document.getElementById('mouseX').value =  pageX;
    document.getElementById('mouseY').value =  pageY;
  };*/
});

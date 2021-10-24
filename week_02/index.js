const meter = document.getElementById("meter");


function consultSensorInput(){
    let url = 'http://localhost:3000/';

    setInterval(function(){
      axios.get(url)
          .then(function (response) {
              changeWidth(response.data.value);
          }).catch(function (error) { console.log(error);}) }, 10);
}
function changeWidth(valor){
  console.log(valor);
  meter.style.width = valor + "px";
}

consultSensorInput();

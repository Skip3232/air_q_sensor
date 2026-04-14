const char reb_d[] PROGMEM = R"rawliteral(
   function rebootdev() {
        modal.style.display = "block";
     }
   var t = 60;
   var time = document.getElementsByClassName("clock")[0];
     function fun() {
         t--;
        time.innerHTML = "<b>Оставшееся время: "+ t +" сек.</b>"
       if(t <= 0) {
         location.href = "/";
         clearInterval(inter);
       }
   }
   
  function rebootdev_s() {
      var msg = document.getElementsByClassName("modal-body")[0];
      var req5 = new XMLHttpRequest();
            req5.open('GET','/?page=dev_reboot');
            req5.onreadystatechange = function(){
              if (req5.readyState === 4 && req5.status === 200) {
                     msg.innerHTML ="<br><b>Выполняется перезапуск устройства...</b><br>"
                     var inter = setInterval("fun()", 1000);
              }
            }
        req5.send();
}

// js
      setInterval(server_time, 1000);
      function server_time(){
         var worktime = new XMLHttpRequest();
             worktime.open("GET","?page=time",true);
             worktime.onreadystatechange = function(){
          if (worktime.readyState === 4 && worktime.status === 200) {
                const obj = JSON.parse(worktime.responseText);
                document.getElementById("timew").innerHTML      = obj.time;
                document.getElementById("temp").innerHTML       = obj.temp;
                document.getElementById("hum").innerHTML        = obj.hum;
                document.getElementById("AQI").innerHTML        = obj.AQI;
                document.getElementById("TVOC").innerHTML       = obj.TVOC;
                document.getElementById("ECO2").innerHTML       = obj.ECO2;
                var status = "Нет данных";
                if(obj.ensStatus == "0"){status = "Работает нормально"}
                if(obj.ensStatus == "1"){status = "Прогрев: 3 мин после включения"}
                if(obj.ensStatus == "2"){status = "Первоначальный запуск: выход на режим 1 час"}
                document.getElementById("ensStatus").innerHTML  = status;
                }
              }
             worktime.send();
           }

//flying
          var flying = document.getElementById('flying');
          flying.addEventListener('input', function() {
                  response.innerHTML = flying.value;
            });
          flying.addEventListener('change', function() {
            var value = flying.value;
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '?page=indata', true);
            xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
            xhr.onreadystatechange = function() {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    document.getElementById('response').innerHTML = xhr.responseText;
                }
            };
            xhr.send('flevel=' + encodeURIComponent(value));
        });

)rawliteral";

//=========================================
const char js_wifi[] PROGMEM = R"rawliteral(
document.addEventListener('DOMContentLoaded', async function() {
    try {
        const response = await fetch('?page=scan_wifi');
        const data = await response.text(); 
        document.getElementById('response').innerHTML = data;
        
    } catch (error) {
        console.error('Ошибка загрузки:', error);
        document.getElementById('response').innerHTML = 
            `<div class="error">Ошибка загрузки ${error.message}</div>`;
    }
});
  )rawliteral";
//=========================================

const char html_style[] PROGMEM = R"rawliteral(
                  body {
                  background: linear-gradient(to bottom, #060345, #023f78, #540110, #013a42, #1f3d6e);
                    }
                  form, div.frame {
                       margin: 20px auto 0;
                       max-width: 500px;
                       padding: 10px;
                       border-radius: 10px;
                       background-color: #fff;
                    }
                  div#response2, div#response21 {
                       margin: 10px auto 0;
                       max-width: 500px;
                       padding: 20px;
                       border-radius: 10px;
                       background: linear-gradient(to bottom right, #9df3f5, #e6e8e7);
                    }
                    label {
                        display: block;
                        color: #5769a1;
                        margin: 10px;
                    }
                     input[type=text], input[type=password], input[type=number], input[type=file], select {
                        width: 100%;
                        padding: 10px;
                        margin-bottom: 15px;
                        border: 1px solid #ccc;
                        border-radius: 4px;
                        box-sizing: border-box;
                      }
                     input[type=submit] {
                        background-color: #3482f7;
                        color: white;
                        padding: 12px 20px;
                        border: none;
                        border-radius: 4px;
                        cursor: pointer;
                     }
                    h2 {
                        text-align: center;
                        margin-top: 10px;
                        font-size: 30px;
                        color: white;
                    }
                    text {
                       text-align: left;
                       margin-top: 5px;
                       font-size: 18px;
                       font-family: Noto Sans;
                       color: #023c52;
                     }
                    div.live {
                       text-align: right;
                       margin-top: 0px;
                       font-size: 30px;
                       font-family: Arial;
                       color: #066094;
                     }
                     div.livePM {
                       text-align: right;
                       margin-top: 10px;
                       margin-bottom: 10px;
                       font-size: 17px;
                       font-family: Monotype Corsiva;
                       color: #800404;
                     }
                    div.modal {
                       text-align: left;
                       margin-top: 0px;
                       font-size: 30px;
                       color: #066094;
                     }
                      a {
                       text-align: center;
                       margin-top: 0px;
                       font-size: 16px;
                       text-decoration: none;
                       color: #9ee8e8;
                     }
                     a.mod {
                       text-align: center;
                       margin-top: 0px;
                       font-size: 18px;
                       text-decoration: none;
                       color: #3e5d80;
                     }
                    .spoiler input, .spoiler div  { 
                        display: none; 
                     }
                    .spoiler :checked ~ div {
                     display: block;
                     }
                     footer {
                       text-align: center;
                       margin-top: 10px;
                       font-size: 15px;
                       color: #cbd6f2;
                     }
                    #myBtn {                     
                      background-color: #9C27B0;  
                      color: white; 
                      padding: 12px 16px;
                      font-size: 18px;   
                      font-weight: bold;   
                      border: none;   
                      cursor: pointer;   
                      width: 180px;        
                     }                       
                      .modal {
                        display: none; /* Hidden by default */
                        position: fixed; /* Stay in place */
                        z-index: 1; /* Sit on top */
                        padding-top: 250px; /* Location of the box */
                        left: 0;
                        top: 0;
                        width: 100%; /* Full width */
                        height: 100%; /* Full height */
                        overflow: auto; /* Enable scroll if needed */
                        background-color: rgb(0,0,0); /* Fallback color */
                        background-color: rgba(0,0,0,0.4); /* Black w/ opacity */
                        border-radius: 10px;
                    }
                    .modal-content {
                        position: relative;
                        background-color: #97e1f7;
                        margin: auto;
                        padding: 0;
                        border: 0px solid #cdeef7;
                        width: fit-content;
                        border-radius: 10px;
                        box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2),0 6px 20px 0 rgba(0,0,0,0.19);
                        -webkit-animation-name: animatetop;
                        -webkit-animation-duration: 0.4s;
                        animation-name: animatetop;
                        animation-duration: 0.4s;
                    }
                   @-webkit-keyframes animatetop {
                        from {top:-300px; opacity:0}
                        to {top:0; opacity:1}
                    }
                   @keyframes animatetop {
                        from {top:-300px; opacity:0}
                        to {top:0; opacity:1}
                    }
                    .close {
                        color: white;
                        float: right;
                        font-size: 28px;
                        font-weight: bold;
                    }
                    .close:hover,
                    .close:focus {
                        color: #000;
                        text-decoration: none;
                        cursor: pointer;
                    }
                    .modal-header {
                        padding: 5px 16px;
                        color: white;
                        font-size: 12px;
                        text-align: left;
                        border-radius: 10px;
                    }
                    .modal-body {
                        padding: 12px 16px;
                        font-size: 18px;
                        text-align: center;
                    }
                    .modal-footer {
                        padding: 12px 16px;
                        color: #702457;
                        font-size: 16px;
                        text-align: right;
                        border-radius: 10px;
                    }
                    .input-file {
                        position: relative;
                        display: inline-block;
                        padding: 10px 20px;
                        border: none;
                        border-radius: 4px;
                    }
                   .input-file span {
                       position: relative;
                       display: inline-block;
                       cursor: pointer;
                       outline: none;
                       text-decoration: none;
                       font-size: 14px;
                       vertical-align: middle;
                       color: rgb(255 255 255);
                       text-align: center;
                       border-radius: 4px;
                       background-color: #419152;
                       line-height: 22px;
                       height: 40px;
                       padding: 10px 20px;
                       box-sizing: border-box;
                       border: none;
                       margin: 0;
                       transition: background-color 0.2s;
                  }
                  .btn_on {
                       display: inline-block; /* Строчно-блочный элемент */
                       background: #800e1b; /* Серый цвет фона */
                       color: #fff; /* Белый цвет текста */
                       padding: 1rem 1.5rem; /* Поля вокруг текста */
                       text-decoration: none; /* Убираем подчёркивание */
                       border-radius: 3px; /* Скругляем уголки */
                      }
                  .input-file input[type=file] {
                       position: absolute;
                       margin: 10;
                       z-index: -1;
                       opacity: 0;
                       display: block;
                       width: 0;
                       height: 0;
                    }
                   .hidden {
                      display: none; /* Скрыть элемент */
                    }
                  /* Focus */
                  .input-file input[type=file]:focus + span {
                      box-shadow: 0 0 0 0.2rem rgba(0,123,255,.25);
                    }
 
                  /* Hover/active */
                  .input-file:hover span {
                      background-color: #59be6e;
                   }
                  .input-file:active span {
                      background-color: #2E703A;
                    }
 
                  /* Disabled */
                  .input-file input[type=file]:disabled + span {
                      background-color: #eee;
                  }
                  
                  /* Переключатели*/
                  .switch {
                     position: relative;
                     display: inline-block;
                     width: 60px;
                     height: 34px;
                  }
                  
                  .switch input {display:none;}
                  
                  .slider {
                      position: absolute;
                      cursor: pointer;
                      top: 0;
                      left: 0;
                      right: 0;
                      bottom: 0;
                      background-color: #ccc;
                      -webkit-transition: .4s;
                      transition: .4s;
                  }
                  
                  .slider:before {
                      position: absolute;
                      content: "";
                      height: 26px;
                      width: 26px;
                      left: 4px;
                      bottom: 4px;
                      background-color: white;
                      -webkit-transition: .4s;
                      transition: .4s;
                  }
                  
                  input:checked + .slider {
                      background-color: #2196F3;
                  }
                  
                  input:focus + .slider {
                      box-shadow: 0 0 1px #2196F3;
                  }
                  
                  input:checked + .slider:before {
                     -webkit-transform: translateX(26px);
                     -ms-transform: translateX(26px);
                     transform: translateX(26px);
                  }
                  
                  /* Rounded sliders */
                  .slider.round {
                     border-radius: 34px;
                  }
                  
                  .slider.round:before {
                     border-radius: 50%;
                  }
                  
                  .slider2 {
                     display: flex;
                     justify-content: center;
                     align-items: center;
                     margin: 0 auto;
                     margin-top: 50px;
                     height: 30px;
                     max-width: 450px;
                     padding: 15px 10px;
                     border-radius: 10px;
                     background: #58b4fc;
                  }

                  .slider2--body {
                     background: rgb(250, 250, 250);
                     height: 4px;
                     width: 100%;
                     position: relative;
                     border-radius: 5px;
                  }

                  .slider2--progress {
                      position: absolute;
                      left: 100%;
                      right: 100%;
                      background: #163a94;
                      height: 4px;
                      border-radius: 10px;
                  }

                  .slider2--inputs {
                     position: relative;
                  }

                  .slider2--inputs > input {
                     pointer-events: none;
                  }

                  .slider2--input {
                      position: absolute;
                      top: -2.4px;
                      left: -3px;
                      height: 5px;
                      width: calc(100% + 2px);
                      background: none;
                      -webkit-appearance: none;
                  }

                  .slider2--input::-webkit-slider-thumb {
                    width: 15px;
                    height: 15px;
                    border-radius: 50%;
                    border: 1px solid #021033;
                    pointer-events: auto;
                    -webkit-appearance: none;
                    background: #021647;
                  }
                  #prg {
                    width:0;
                    color:white;
                    margin-bottom: 10px;
                    padding-top: 0px;
                    padding-bottom: 0px;
                    text-align:center;
                    border-radius: 3px;
                    border: none;
                  })rawliteral";

//================================================
const char divcode[] PROGMEM = R"rawliteral(
    <div id="myModal" class="modal">
       <div class="modal-content">
       <div class="modal-header">
          <span class="close">&times;</span>
          <text>Перезагрузка устройства</text>
       </div>
       <div class="modal-body"><br><a class ="btn_on" href="#" onclick="rebootdev_s()"><b>Перезагрузить устройство</b></a><br><br></div>
     <div class="modal-footer">
        <span class = "clock"> <br></span>
     </div>
   </div>
 </div>
 <script>
      var modal = document.getElementById("myModal");
      var span = document.getElementsByClassName('close')[0];
      span.onclick = function() {
          modal.style.display = "none";
        }
       window.onclick = function(event) {
          if(event.target == modal) {
            modal.style.display = "none";
          }
       }
    </script>)rawliteral";
 //==============================================
 // Скрипт для формы конфигурации
const char *config_js PROGMEM = R"rawliteral(
        const form = document.getElementById('config-form');
              form.addEventListener('submit', (event) => {
                    event.preventDefault();
                    const formData = new FormData(form);
                    const xhr = new XMLHttpRequest(); 
                          xhr.open('POST', '/?page=indata');  
                          xhr.addEventListener('load', () => {
                           if (xhr.status === 200) {
                                  document.getElementById('response').innerHTML = xhr.responseText;
                                    }else{
                                       document.getElementById('response').innerHTML = 'Ошибка отправки';
                                          }
                                    });
                          xhr.send(formData);
                       });

          document.getElementById('switch').addEventListener('change', function() {
                  var status_s = this.checked ? 'on' : 'off';
                  var xhr = new XMLHttpRequest();
                  xhr.open('POST', '?page=indata', true);
                  xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
                  xhr.onreadystatechange = function() {
                   if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
                            document.getElementById('response2').innerHTML = xhr.responseText;
                          //console.log('Данные успешно отправлены на сервер' + status_s);
                        }
                    };
                  xhr.send('switch=' + status_s);
                });
  )rawliteral";
 //========================================
 //JS смены пароля
 const char pass_js[] PROGMEM = R"rawliteral(
       const form = document.getElementById('config-form');
             form.addEventListener('submit', (event) => {
                  event.preventDefault();
                  const formData = new FormData(form);
                  const xhr = new XMLHttpRequest();
                        xhr.open('POST', '?page=indata');
                        xhr.addEventListener('load', () => {
                              if (xhr.status === 200) { 
                                    document.getElementById('response').innerHTML = xhr.responseText; 
                                   }else{
                                    document.getElementById('response').innerHTML = 'Ошибка отправки';
                                 }
                            });
                   var newPassword = document.getElementById("password").value;
                   var confirmNewPassword = document.getElementById("newpassword").value;
                      if (newPassword != confirmNewPassword) {
                               document.getElementById('response').innerHTML = 'Пароли не совпадают!';
                               return false;
                             }else{
                               xhr.send(formData);
                              }
                          });
  )rawliteral";
//=================================================
  //Update js
  const char update_js[] PROGMEM = R"rawliteral(
         const form = document.getElementById('config-form');
               form.addEventListener('submit', (event) => {
               prg.style.backgroundColor = 'blue';
               event.preventDefault();
               const formData = new FormData(form);
               const xhr = new XMLHttpRequest();
                     xhr.open('POST', '/update');
                     xhr.upload.addEventListener('progress', p=>{
                      let w = Math.round(p.loaded/p.total*100) + '%';
                        if(p.lengthComputable){
                             prg.innerHTML = w;
                             prg.style.width = w;
                             prg.style.paddingTop = '10px';
                             prg.style.paddingBottom = '10px';
                          }
                        if(w == '100%') prg.style.backgroundColor = 'green';
                        });
                     xhr.addEventListener('load', () => {
                  if (xhr.status === 200) {
                         document.getElementById('response').innerHTML = xhr.responseText.replace('Update Success! Rebooting...','<text>Обновление выполнено успешно! Выполняется перезагрузка устройства, подождите...</text>'); 
                      }else{
                         document.getElementById('response').innerHTML = '<text>Ошибка отправки</text>'; 
                       }
               });
                     document.getElementById('response').innerHTML = '<text>Выполняется процесс обновления прошивки... Пожалуйста, не покидайте страницу в процессе обновления!</text>';
                     xhr.send(formData);
            }); )rawliteral";
//==============================================
// веб-страница с графиком для отображения данных датчиков
// Сжатая версия страницы графика (вставьте в const_js.h)
const char CHART_PAGE[] PROGMEM = R"raw(<html><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>График датчиков</title><script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.min.js"></script><script src="https://code.jquery.com/jquery-3.7.1.min.js"></script><style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:Arial,sans-serif;background:#667eea;padding:10px}.container{max-width:1200px;margin:0 auto}.header{background:#fff;border-radius:10px;padding:15px;margin-bottom:15px}h1{font-size:20px;color:#333}.sensor-info{display:flex;gap:10px;flex-wrap:wrap;margin-top:10px}.info-card{background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;padding:10px;border-radius:8px;flex:1;min-width:100px;text-align:center}.info-card h3{font-size:12px}.info-card .value{font-size:22px;font-weight:700}.chart-container{background:#fff;border-radius:10px;padding:15px;margin-bottom:15px}.chart-title{font-size:16px;font-weight:700;margin-bottom:10px}.controls{background:#fff;border-radius:10px;padding:15px;margin-bottom:15px;display:flex;gap:10px;align-items:center}.btn{background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;border:none;padding:8px 15px;border-radius:6px;cursor:pointer}.btn-danger{background:linear-gradient(135deg,#f093fb,#f5576c)}.status{font-size:12px;color:#666;margin-left:auto}.update-time{font-size:11px;color:#999;text-align:center;margin-top:10px}canvas{max-height:300px;width:100%}</style></head><body><div class=container><div class=header><h1>📊 Мониторинг воздуха</h1><div class=sensor-info><div class=info-card><h3>Температура</h3><div class=value id=temp>--</div><div class=unit>°C</div></div><div class=info-card><h3>Влажность</h3><div class=value id=hum>--</div><div class=unit>%</div></div><div class=info-card><h3>AQI</h3><div class=value id=aqi>--</div></div><div class=info-card><h3>TVOC</h3><div class=value id=tvoc>--</div><div class=unit>ppb</div></div><div class=info-card><h3>eCO2</h3><div class=value id=eco2>--</div><div class=unit>ppm</div></div></div></div><div class=controls><button class=btn onclick=refresh()>🔄 Обновить</button><button class="btn btn-danger" onclick=clearData()>🗑️ Очистить</button><div class=status id=status>🟢 Онлайн</div></div><div class=chart-container><div class=chart-title>🌡️ Температура и Влажность</div><canvas id=chart1></canvas></div><div class=chart-container><div class=chart-title>💨 Качество воздуха</div><canvas id=chart2></canvas></div><div class=update-time id=time></div></div><script>let ch1,ch2;function init(){const a=document.getElementById('chart1').getContext('2d');ch1=new Chart(a,{type:'line',data:{labels:[],datasets:[{label:'Температура (°C)',data:[],borderColor:'rgb(255,99,132)',tension:.4},{label:'Влажность (%)',data:[],borderColor:'rgb(54,162,235)',tension:.4}]},options:{responsive:!0,maintainAspectRatio:!0}});const b=document.getElementById('chart2').getContext('2d');ch2=new Chart(b,{type:'line',data:{labels:[],datasets:[{label:'AQI',data:[],borderColor:'rgb(75,192,192)',tension:.4},{label:'TVOC (ppb)',data:[],borderColor:'rgb(153,102,255)',tension:.4},{label:'eCO2 (ppm)',data:[],borderColor:'rgb(255,159,64)',tension:.4}]},options:{responsive:!0,maintainAspectRatio:!0}})}async function load(){try{const a=await fetch('/api/data'),b=await a.json();if(b.success){ch1.data.labels=b.data.map(a=>a.time);ch1.data.datasets[0].data=b.data.map(a=>a.temp);ch1.data.datasets[1].data=b.data.map(a=>a.hum);ch1.update('none');ch2.data.labels=b.data.map(a=>a.time);ch2.data.datasets[0].data=b.data.map(a=>a.aqi);ch2.data.datasets[1].data=b.data.map(a=>a.tvoc);ch2.data.datasets[2].data=b.data.map(a=>a.eco2);ch2.update('none');b.latest&&(document.getElementById('temp').innerHTML=b.latest.temp||'--',document.getElementById('hum').innerHTML=b.latest.hum||'--',document.getElementById('aqi').innerHTML=b.latest.aqi||'--',document.getElementById('tvoc').innerHTML=b.latest.tvoc||'--',document.getElementById('eco2').innerHTML=b.latest.eco2||'--');document.getElementById('time').innerHTML='Последнее обновление: '+new Date().toLocaleString()}}catch(a){console.error(a),document.getElementById('status').innerHTML='🔴 Ошибка'}}function refresh(){load()}async function clearData(){confirm('Очистить историю?')&&(await fetch('/api/clear',{method:'POST'}),refresh())}$(document).ready(function(){init(),load(),setInterval(load,6e4)})</script></body></html>)raw";

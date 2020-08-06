const ESP8266_IP = '192.168.2.108';

function enableActiveiOS() {
  // Habilita o CSS active no iOS
  // Add this event to your JS to enable active states on all of your elements.
  // This can be a bit slow on huge pages so it might be worth restricting it to certain elements instead of document
  document.addEventListener("touchstart", function () { }, true);
}

async function checkEsp8266OnTheLocalNetwork() {
  const section3 = document.querySelector('.section-3');
  const title = document.querySelector('.section-3__title');
  const loader = document.querySelector('.section-3__loader');

  try {
    const espUrl = `http://${ESP8266_IP}/health`;
    const res = await fetch(espUrl);
    const obj = await res.json();

    console.log(obj);
    title.innerHTML = 'ESP8266 NodeMCU Disponível';
  } catch (error) {
    title.innerHTML = 'ESP8266 NodeMCU Indisponível';
    section3.style.background = '#8d8d8d';
    console.error(new Error('Não foi obtido o status do ESP8266.'));
  }

  setTimeout(() => {
    loader.style.display = 'none';
  }, 250);
}

function init() {
  enableActiveiOS();
  checkEsp8266OnTheLocalNetwork();
}

init();

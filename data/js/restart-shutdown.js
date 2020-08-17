import settings from "./app-global-settings.js"

function updateContainerError(error) {
  const containerError = document.querySelector('#container-error');

  if (!error) {
    containerError.innerHTML = "";
    return;
  }

  containerError.innerHTML = JSON.stringify(error);
}

async function sendRequestRestart() {
  const url = new URL(`http://${settings.baseUrlESP8266}/restart`);

  try {
    const response = await axios.get(url.toString());
    alert('Globo Terrestre IoT Reiniciado');
  } catch (error) {
    console.log(error);
    updateContainerError(error.toString());
  }
}

async function sendRequestShutdown() {
  const url = new URL(`http://${settings.baseUrlESP8266}/shutdown`);

  try {
    const response = await axios.get(url.toString());
    alert('Globo Terrestre IoT Desligado');
  } catch (error) {
    console.log(error);
    updateContainerError(error.toString());
  }
}

function startEventListener() {
  const buttonRestart = document.querySelector('#button-restart');
  const buttonShutdown = document.querySelector('#button-shutdown');

  buttonRestart.addEventListener('click', (e) => sendRequestRestart());
  buttonShutdown.addEventListener('click', (e) => sendRequestShutdown());
}

startEventListener();

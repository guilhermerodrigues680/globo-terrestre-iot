import settings from "./app-global-settings.js"

function getSelectedRadioButtonSteps() {
  const radioButtonsSteps = document.querySelectorAll('#inputs-steps input[name=steps-radio]');
  const inputCustom = document.querySelector('#inputs-steps input[type=number]');

  for (const [idx, radioBtn] of radioButtonsSteps.entries()) {
    if (radioBtn.checked && radioBtn.value === 'custom') {
      console.debug(radioBtn, inputCustom);
      return inputCustom;
    } else if (radioBtn.checked) {
      console.debug(radioBtn);
      return radioBtn;
    }
  }
}

function updateContainerCoordinates(coordinatesData) {
  const globeH3 = document.createElement('h3');
  const stepperH3 = document.createElement('h3');
  const globeUl = document.createElement('ul');
  const stepperUl = document.createElement('ul');

  globeH3.innerText = 'Globo';
  stepperH3.innerText = 'Motor';

  for (const value in coordinatesData.globe) {
    const li = document.createElement('li');
    li.innerText = `${value}: ${coordinatesData.globe[value]}`;
    globeUl.appendChild(li);
  }

  for (const value in coordinatesData.stepper) {
    const li = document.createElement('li');
    li.innerText = `${value}: ${coordinatesData.stepper[value]}`;
    stepperUl.appendChild(li);
  }

  const containerCoordinates = document.querySelector('#container-coordinates');
  containerCoordinates.innerHTML = '';
  containerCoordinates.appendChild(globeH3);
  containerCoordinates.appendChild(globeUl);
  containerCoordinates.appendChild(stepperH3);
  containerCoordinates.appendChild(stepperUl);
}

async function sendRequestCenterGlobe(requestBody) {
  const url = new URL(`http://${settings.baseUrlESP8266}/center-globe`);

  try {
    const response = await axios.post(url.toString(), requestBody);
    const coordinates = response.data.data;
    updateContainerCoordinates(coordinates);
  } catch (error) {
    console.log(error);
  }
}

async function sendRequestGetCoordinates() {
  const url = new URL(`http://${settings.baseUrlESP8266}/coord`);

  try {
    const response = await axios.get(url.toString());
    const coordinates = response.data.data;
    updateContainerCoordinates(coordinates);
  } catch (error) {
    console.log(error);
  }
}

function moveGlobeValueSteps(steps) {
  const requestBody = {
    steps: steps,
    setCenter: false
  };
  sendRequestCenterGlobe(requestBody);
}

function setGlobeCenter() {
  const requestBody = {
    steps: 0,
    setCenter: true
  };
  sendRequestCenterGlobe(requestBody);
}

function startEventListener() {
  const buttonIncrement = document.querySelector('#button-increment');
  const buttonDecrement = document.querySelector('#button-decrement');
  const buttonSetCenter = document.querySelector('#button-set-center');
  const buttonRefreshCoord = document.querySelector('#button-refresh-coordinates');

  buttonIncrement.addEventListener('click', (e) => {
    const steps = +getSelectedRadioButtonSteps().value;
    moveGlobeValueSteps(steps)
  });

  buttonDecrement.addEventListener('click', (e) => {
    const steps = +getSelectedRadioButtonSteps().value;
    moveGlobeValueSteps(-steps)
  });

  buttonSetCenter.addEventListener('click', (e) => setGlobeCenter());
  buttonRefreshCoord.addEventListener('click', (e) => sendRequestGetCoordinates());
}

startEventListener();
sendRequestGetCoordinates();

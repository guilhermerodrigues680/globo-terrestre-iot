let IP;
let PORT;
let socket = null;

export function initWebSocker(ip, port) {
  IP = ip;
  PORT = port;
  // const url = `ws://${IP}:${PORT}`;

  // const socket = new WebSocket(url);

  // socket.onopen = function(e) {
  //   alert("[open] Connection established");
  //   alert("Sending to server");
  //   // socket.send("My name is John");
  //   socket.send(`{"lat":-16.96242661161516,"lon":-28.714400061048288,"height":8901623.110352391}`);
  // };

  // socket.onmessage = function(event) {
  //   alert(`[message] Data received from server: ${event.data}`);
  // };

  // socket.onclose = function(event) {
  //   if (event.wasClean) {
  //     alert(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
  //   } else {
  //     // e.g. server process killed or network down
  //     // event.code is usually 1006 in this case
  //     alert('[close] Connection died');
  //   }
  // };

  // socket.onerror = function(error) {
  //   alert(`[error] ${error.message}`);
  // };

  // return socket;
}

export function sendCoord(coord) {
  const url = `ws://${IP}:${PORT}`;

  const socket = new WebSocket(url);

  socket.onopen = function(e) {
    console.info("[open] Connection established");
    console.info("Sending to server");
    // socket.send("My name is John");
    socket.send(JSON.stringify(coord));
  };

  socket.onmessage = function(event) {
    console.log(`[message] Data received from server: ${event.data}`);
  };

  socket.onclose = function(event) {
    if (event.wasClean) {
      console.error(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
    } else {
      // e.g. server process killed or network down
      // event.code is usually 1006 in this case
      console.error('[close] Connection died');
    }
  };

  socket.onerror = function(error) {
    alert(`[error] ${error.message}`);
  };

  return socket;
}

export function sendCoord2(coord) {
  if (socket === null || socket.readyState !== socket.OPEN) {
    const url = `ws://${IP}:${PORT}`;
    socket = new WebSocket(url);

    socket.onopen = function(e) {
      console.info("[open] Connection established");
      console.info("Sending to server");
      // socket.send("My name is John");
      socket.send(JSON.stringify(coord));
    };

    socket.onmessage = function(event) {
      console.log(`[message] Data received from server: ${event.data}`);
    };

    socket.onclose = function(event) {
      if (event.wasClean) {
        console.error(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
      } else {
        // e.g. server process killed or network down
        // event.code is usually 1006 in this case
        console.error('[close] Connection died');
      }
    };

    socket.onerror = function(error) {
      alert(`[error] ${error.message}`);
    };
  } else {
    socket.send(JSON.stringify(coord));
  }
}
